/* Local diagnostic protocol only; never linked into the installed core.
 * 64 little-endian bytes per record, at most64 records (4096 bytes) per writer.
 * Regular-file write latency is not bounded by this byte/count limit. */
#ifndef INPUT_TRACE_H
#define INPUT_TRACE_H
#include <stddef.h>
#include <stdint.h>
#define IT_BYTES 64u
#define IT_LIMIT 64u
enum it_type {
    IT_INIT = 1,
    IT_FORK,
    IT_START,
    IT_POST,
    IT_POLL,
    IT_STATE,
    IT_UNAVAILABLE,
    IT_END,
    IT_SATURATED
};
enum it_role { IT_HELPER = 1, IT_CORE };
enum it_status {
    IT_ABSENT,
    IT_EMPTY,
    IT_TRUNCATED,
    IT_INVALID,
    IT_UNFINISHED,
    IT_COMPLETE,
    IT_FULL
};
typedef struct {
    int fd, disabled;
    uint32_t count, role, pid, peer;
} it_writer;
typedef struct {
    uint32_t type, sequence, role, pid, peer, nsec, error, a, b, c;
    uint64_t sec;
} it_record;
typedef struct {
    enum it_status status;
    uint32_t count;
    int clock_ok;
    it_record records[IT_LIMIT];
} it_trace;
void it_emit(it_writer *writer, uint32_t type, uint32_t a, uint32_t b, uint32_t c);
int it_begin(it_writer *writer, int fd, uint32_t role, uint32_t peer);
void it_read(const char *path, uint32_t role, it_trace *trace);
const char *it_status_name(enum it_status status);
/* Parent/child identity and clock validity only. Caller must establish the same
 * host/clock domain via fresh descriptor ownership before comparing times. */
int it_pair(const it_trace *helper, const it_trace *core);
#endif
