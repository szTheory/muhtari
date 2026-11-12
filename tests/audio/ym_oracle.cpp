/* Receipt-bound comparison of the C17 port to the already-built exact OPM pin. */
#define _Static_assert static_assert
extern "C" {
#include "internal.h"
}
#undef _Static_assert

#include "ym_vectors.h"
#include "ymfm_opm.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

namespace {

constexpr const char *seed_id = "81aec25ccbb98f4873a255f7551ac4dadac59b4a";
constexpr uint64_t fnv_offset = UINT64_C(14695981039346656037);
constexpr uint64_t fnv_prime = UINT64_C(1099511628211);

struct OracleInterface final : ymfm::ymfm_interface {
    uint64_t now = 0;
    uint64_t busy_end = 0;
    uint64_t deadline[2] = {0, 0};
    bool timer_running[2] = {false, false};
    bool irq = false;

    void ymfm_sync_mode_write(uint8_t data) override
    {
        m_engine->engine_mode_write(data);
    }

    void ymfm_sync_check_interrupts() override
    {
        m_engine->engine_check_interrupts();
    }

    void ymfm_set_timer(uint32_t which, int32_t duration) override
    {
        if (which >= 2u)
            throw std::runtime_error("seed scheduled an invalid timer");
        if (duration < 0) {
            timer_running[which] = false;
            deadline[which] = 0;
        } else {
            timer_running[which] = true;
            deadline[which] = now + static_cast<uint32_t>(duration);
        }
    }

    void ymfm_set_busy_end(uint32_t clocks) override
    {
        busy_end = now + clocks;
    }

    bool ymfm_is_busy() override
    {
        return now < busy_end;
    }

    void ymfm_update_irq(bool asserted) override
    {
        irq = asserted;
    }
};

struct OracleChip final : ymfm::ym2151 {
    explicit OracleChip(OracleInterface &interface) : ymfm::ym2151(interface) {}

    void expire_timer(uint32_t which)
    {
        m_fm.engine_timer_expired(which);
    }

    ymfm::fm_operator<ymfm::opm_registers> *op(uint32_t index)
    {
        return m_fm.debug_operator(index);
    }

    uint32_t algorithm()
    {
        return m_fm.regs().ch_algorithm(0);
    }
};

struct Pair {
    OracleInterface interface;
    OracleChip seed;
    muhtari *c = nullptr;
    ymfm::ym2151::output_data last_output{};

    Pair() : seed(interface)
    {
        c = static_cast<muhtari *>(std::calloc(1u, sizeof *c));
        if (c == nullptr)
            throw std::runtime_error("could not allocate C test instance");
        muhtari__synth_reset(c, 0u, 0u);
        muhtari__ym2151_reset(c);
        seed.reset();
    }

    ~Pair() { std::free(c); }

    void write(uint8_t reg, uint8_t value)
    {
        seed.write_address(reg);
        seed.write_data(value);
        const uint64_t tick = interface.now * 2u;
        muhtari__ym2151_write(c, 0x0460u, reg, tick);
        muhtari__ym2151_write(c, 0x0461u, value, tick);
        compare_state("write");
    }

    void compare_state(const char *where)
    {
        const uint8_t seed_status = seed.read_status() & 0x7fu;
        const uint8_t c_status = muhtari__ym2151_peek(c, 0x0461u) & 0x7fu;
        if (seed_status != c_status)
            throw std::runtime_error(std::string(where) + " status differs at phiM " +
                                     std::to_string(interface.now) + ": seed=" +
                                     std::to_string(seed_status) + " C=" +
                                     std::to_string(c_status));
        if (interface.irq != (muhtari__ym2151_irq(c) != 0))
            throw std::runtime_error(std::string(where) + " IRQ differs at phiM " +
                                     std::to_string(interface.now));
        for (unsigned timer = 0; timer < 2; timer++) {
            if (interface.timer_running[timer] != (c->ym.timer_running[timer] != 0u))
                throw std::runtime_error(std::string(where) + " timer running differs at phiM " +
                                         std::to_string(interface.now));
            if (interface.timer_running[timer] &&
                interface.deadline[timer] != c->ym.timer_deadline[timer])
                throw std::runtime_error(std::string(where) + " timer deadline differs at phiM " +
                                         std::to_string(interface.now));
        }
    }

    void discard_completed_frames(uint64_t tick)
    {
        const struct muhtari__timing timing = muhtari__timing(c);
        if (timing.frame_ticks == 0u || tick < c->synth.horizon)
            throw std::runtime_error("invalid audio frame boundary");
        while (tick - c->synth.horizon >= timing.frame_ticks) {
            const uint64_t end = c->synth.horizon + timing.frame_ticks;
            if (end > tick || c->synth.pcm_count > MUHTARI__SYNTH_PCM_CAPACITY)
                throw std::runtime_error("YM audio exceeded the bounded render interval");
            const uint32_t discarded = muhtari__synth_render(c, end, nullptr);
            if (discarded > timing.max_samples || c->synth.horizon != end ||
                c->synth.pcm_count > MUHTARI__SYNTH_PCM_CAPACITY)
                throw std::runtime_error("YM frame discard exceeded its bounded capacity");
        }
        if (c->synth.pcm_count > MUHTARI__SYNTH_PCM_CAPACITY)
            throw std::runtime_error("YM PCM FIFO exceeded its declared capacity");
    }

    void clock_one()
    {
        interface.now++;
        for (unsigned timer = 0; timer < 2; timer++) {
            if (interface.timer_running[timer] && interface.deadline[timer] <= interface.now)
                seed.expire_timer(timer);
        }
        const uint64_t tick = interface.now * 2u;
        muhtari__ym2151_catch_up(c, tick);
        if ((interface.now & 63u) == 0u) {
            seed.generate(&last_output, 1u);
            if (c->ym.output[0] != last_output.data[0] ||
                c->ym.output[1] != last_output.data[1]) {
                for (const uint32_t index : {0u, 16u, 8u, 24u}) {
                    auto *reference = seed.op(index);
                    const auto &ported = c->ym.operators[index];
                    std::fprintf(stderr,
                                 "op=%u phase seed=%u C=%u attenuation seed=%u C=%u "
                                 "state seed=%u C=%u step seed=%u C=%u wave=%u\n",
                                 index, reference->phase(), ported.phase >> 10,
                                 reference->debug_eg_attenuation(), ported.attenuation,
                                 static_cast<unsigned>(reference->debug_eg_state()),
                                 ported.envelope_state, reference->debug_cache().phase_step,
                                 ported.cache.phase_step,
                                 reference->debug_cache().waveform[reference->phase()]);
                }
                std::fprintf(stderr, "algorithm seed=%u C=%u regs20=%02x\n",
                             seed.algorithm(), c->ym.registers[0x20u],
                             c->ym.registers[0x20u]);
                throw std::runtime_error("sample differs at phiM " +
                                         std::to_string(interface.now) + ": seed=(" +
                                         std::to_string(last_output.data[0]) + "," +
                                         std::to_string(last_output.data[1]) + ") C=(" +
                                         std::to_string(c->ym.output[0]) + "," +
                                         std::to_string(c->ym.output[1]) + ")");
            }
        }
        discard_completed_frames(tick);
        compare_state("clock");
    }
};

uint64_t hash_sample(uint64_t hash, int16_t left, int16_t right)
{
    const uint16_t values[2] = {static_cast<uint16_t>(left), static_cast<uint16_t>(right)};
    for (uint16_t value : values) {
        hash ^= value & 0xffu;
        hash *= fnv_prime;
        hash ^= value >> 8;
        hash *= fnv_prime;
    }
    return hash;
}

void write_action(Pair *pairs[2], uint8_t count, const ym_case_action &action)
{
    if (action.chip == YM_CASE_ALL) {
        for (uint8_t i = 0; i < count; i++)
            pairs[i]->write(action.reg, action.value);
    } else if (action.chip < count) {
        pairs[action.chip]->write(action.reg, action.value);
    }
}

void run_vectors()
{
    for (const ym_case_vector &test : ym_cases) {
        Pair first;
        Pair second;
        Pair *pairs[2] = {&first, &second};
        const uint8_t count = test.chip_count;
        for (size_t i = 0; i < sizeof ym_common / sizeof ym_common[0]; i++)
            write_action(pairs, count, ym_common[i]);

        uint64_t hash = fnv_offset;
        uint32_t samples = 0;
        for (uint16_t action_index = 0; action_index < test.action_count; action_index++) {
            const ym_case_action &action = test.actions[action_index];
            if (action.reg != YM_CASE_STEP)
                write_action(pairs, count, action);
            for (uint16_t sample = 0; sample < action.samples; sample++) {
                for (unsigned clock = 0; clock < 64; clock++)
                    for (uint8_t chip = 0; chip < count; chip++)
                        pairs[chip]->clock_one();
                for (uint8_t chip = 0; chip < count; chip++)
                    hash = hash_sample(hash,
                                       static_cast<int16_t>(pairs[chip]->last_output.data[0]),
                                       static_cast<int16_t>(pairs[chip]->last_output.data[1]));
                samples++;
            }
        }
        if (samples != test.sample_count || samples > 2048u)
            throw std::runtime_error(std::string(test.name) + " has an invalid sample count: " +
                                     std::to_string(samples) + " expected " +
                                     std::to_string(test.sample_count));
        if (hash != test.hash)
            throw std::runtime_error(std::string(test.name) +
                                     " differs from its fixed signed-output vector");
        std::printf("case=%s samples=%u hash=%016llx seed=%s\n", test.name, samples,
                    static_cast<unsigned long long>(hash), seed_id);
    }
}

void run_timer_case(unsigned timer, uint32_t value, uint64_t period, bool race)
{
    Pair pair;
    uint8_t mode;
    const uint8_t flag = timer == 0 ? 1u : 2u;
    if (timer == 0) {
        pair.write(0x10u, static_cast<uint8_t>(value >> 2));
        pair.write(0x11u, static_cast<uint8_t>(value & 3u));
        mode = 0x05u;
    } else {
        pair.write(0x12u, static_cast<uint8_t>(value));
        mode = 0x0au;
    }
    pair.write(0x14u, mode);
    if (!pair.interface.timer_running[timer] || pair.interface.deadline[timer] != period ||
        pair.c->ym.timer_deadline[timer] != period)
        throw std::runtime_error("source timer period differs from independent formula");

    while (pair.interface.now < period - 1u)
        pair.clock_one();
    if ((muhtari__ym2151_peek(pair.c, 0x0461u) & flag) != 0u)
        throw std::runtime_error("timer flag asserted before documented carry");
    if (race)
        pair.write(0x14u, static_cast<uint8_t>(mode | (timer == 0 ? 0x10u : 0x20u)));
    pair.clock_one();
    if ((muhtari__ym2151_peek(pair.c, 0x0461u) & flag) == 0u || !pair.interface.irq)
        throw std::runtime_error("timer flag or IRQ missing on exact carry");
    pair.clock_one();
    if ((muhtari__ym2151_peek(pair.c, 0x0461u) & flag) == 0u)
        throw std::runtime_error("timer flag did not remain set after carry");

    pair.write(0x14u, static_cast<uint8_t>(mode | (timer == 0 ? 0x10u : 0x20u)));
    if ((muhtari__ym2151_peek(pair.c, 0x0461u) & flag) != 0u || pair.interface.irq)
        throw std::runtime_error("timer reset bit did not clear flag and IRQ");
    std::printf("timer=%c value=%u phiM=%llu exact-edge=pass\n", timer == 0 ? 'A' : 'B', value,
                static_cast<unsigned long long>(period));
}

void run_busy_qualification()
{
    Pair pair;
    pair.write(0x20u, 0x00u);
    uint32_t mismatch = 0;
    for (uint64_t clock = 1; clock <= 69; clock++) {
        pair.clock_one();
        const bool seed_busy = (pair.seed.read_status() & 0x80u) != 0u;
        const bool c_busy = (muhtari__ym2151_peek(pair.c, 0x0461u) & 0x80u) != 0u;
        mismatch += seed_busy != c_busy;
        if (clock == 67 && (!c_busy || seed_busy))
            throw std::runtime_error("67-clock busy source edge is wrong");
        if (clock == 68 && (c_busy || seed_busy))
            throw std::runtime_error("68-clock busy source edge is wrong");
        if (clock == 69 && (c_busy || seed_busy))
            throw std::runtime_error("69-clock busy source edge is wrong");
    }
    if (mismatch != 4u)
        throw std::runtime_error("busy divergence was not exactly the documented 4 clocks");
    std::printf("busy=manual-68 seed-64 mismatch-clocks=%u edges=67/68/69-pass\n", mismatch);
}

void run_same_edge_write()
{
    Pair pair;
    pair.write(0x10u, 0xffu);
    pair.write(0x11u, 3u);
    pair.write(0x14u, 0x05u);
    while (pair.interface.now < 64u)
        pair.clock_one();
    if ((muhtari__ym2151_peek(pair.c, 0x0461u) & 1u) == 0u)
        throw std::runtime_error("timer A did not assert before same-edge mode write");
    pair.write(0x14u, 0x15u);
    if ((muhtari__ym2151_peek(pair.c, 0x0461u) & 1u) != 0u || pair.interface.irq)
        throw std::runtime_error("same-edge timer reset ordering did not clear the flag");
    std::printf("timer=A mode-reset-at-carry=seed-order-pass\n");
}

} // namespace

int main()
{
    try {
        run_busy_qualification();
        run_timer_case(0, 1023u, UINT64_C(64) * (1024u - 1023u), true);
        run_timer_case(0, 0u, UINT64_C(64) * (1024u - 0u), false);
        run_timer_case(1, 255u, UINT64_C(1024) * (256u - 255u), true);
        run_timer_case(1, 0u, UINT64_C(1024) * (256u - 0u), false);
        run_same_edge_write();
        run_vectors();
    } catch (const std::exception &error) {
        std::fprintf(stderr, "ym oracle failed: %s\n", error.what());
        return 1;
    }
    return 0;
}
