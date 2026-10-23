/* API version 4 adds descriptive Activision dump orientation and an
   assumed-layout warning. Keep their values and the existing ABI frozen. */
#include <string.h>

#include "muhtari.h"
#include "status_text.h"
#include "../check.h"

int main(void)
{
    CHECK(MUHTARI_API_VERSION >= 4);
    CHECK_EQ_U64(MUHTARI_CART_ACTIVISION_HALF_SWAPPED, 4);
    CHECK_EQ_U64(MUHTARI_CART_WARN_ACTIVISION_LAYOUT_ASSUMED, 1024);
    CHECK_EQ_U64(sizeof(muhtari_cart_info), 140);

    muhtari_versions versions;
    memset(&versions, 0, sizeof versions);
    versions.size = (uint32_t)sizeof versions;
    CHECK_EQ_U64(muhtari_get_versions(&versions), MUHTARI_STATUS_OK);
    CHECK(versions.api >= 4);
    CHECK_EQ_U64(versions.abi, 1);

    CHECK(strcmp(muhtari__bit_name(MUHTARI__BITS_FLAGS, 0), "header") == 0);
    CHECK(strcmp(muhtari__bit_name(MUHTARI__BITS_FLAGS, 1), "hash_match") == 0);
    CHECK(strcmp(muhtari__bit_name(MUHTARI__BITS_FLAGS, 2), "activision_half_swapped") == 0);
    CHECK(strcmp(muhtari__bit_name(MUHTARI__BITS_FLAGS, 3), "unknown") == 0);
    CHECK(strcmp(muhtari__bit_name(MUHTARI__BITS_WARNINGS, 9), "cart_type_conflict") == 0);
    CHECK(strcmp(muhtari__bit_name(MUHTARI__BITS_WARNINGS, 10), "activision_layout_assumed") == 0);
    CHECK(strcmp(muhtari__bit_name(MUHTARI__BITS_WARNINGS, 11), "unknown") == 0);
    CHECK_DONE();
}
