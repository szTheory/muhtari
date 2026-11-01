/* Private state payload visitor. The canonical field traversal is introduced
   after its continuation test; public serialization remains unavailable. */
#include <string.h>

#include "internal.h"

size_t muhtari__state_payload_size(const muhtari *m)
{
    if (m == NULL || m->loaded == 0u)
        return 0u;
    return 10972u + (size_t)m->cart.info.ram_size +
           (m->cart.expansion.kind == MUHTARI_EXPANSION_OFF ? 0u : 131072u) + m->battery.bytes;
}

int muhtari__state_payload_write(const muhtari *m, uint8_t *bytes, size_t size)
{
    size_t expected = muhtari__state_payload_size(m);
    if (bytes == NULL || expected == 0u || size != expected)
        return 0;
    memset(bytes, 0, size);
    return 1;
}

int muhtari__state_payload_validate(const muhtari *m, const uint8_t *bytes, size_t size)
{
    return m != NULL && m->loaded != 0u && bytes != NULL && size == muhtari__state_payload_size(m);
}

void muhtari__state_payload_apply(muhtari *m, const uint8_t *bytes, size_t size)
{
    (void)m;
    (void)bytes;
    (void)size;
}
