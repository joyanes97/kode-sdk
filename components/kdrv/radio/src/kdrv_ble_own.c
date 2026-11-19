/* SPDX-License-Identifier: GPL-3.0-or-later */
/**
 * @file kdrv_ble_own.c
 * @brief The board's own Bluetooth address: the rule for which one, and the shape of a random static one.
 */
#include "kdrv_ble_own.h"

#include <stddef.h>

#define OWN_TOP          (KDRV_BLE_OWN_LEN - 1U)  /**< The byte that holds the kind of address. */
#define OWN_KIND_MASK    0xC0U  /**< Its two top bits: the kind of random address. */
#define OWN_KIND_STATIC  0xC0U  /**< Both set: a static one. */
#define OWN_RANDOM_MASK  0x3FU  /**< The rest of that byte: random bits. */
#define OWN_BYTE_ONES    0xFFU  /**< A byte with every bit set. */
#define OWN_FLIP_BIT     0x01U  /**< The bit turned over when the random part came out all 0 or all 1. */

/**
 * @brief Whether the 46 random bits of an address are all 0 or all 1.
 * @param addr The six bytes.
 * @return true when they are.
 */
static bool own_random_flat(const uint8_t *addr)
{
    bool zeros = (0U == (addr[OWN_TOP] & OWN_RANDOM_MASK));
    bool ones  = (OWN_RANDOM_MASK == (addr[OWN_TOP] & OWN_RANDOM_MASK));

    for ( uint32_t i = 0U; OWN_TOP > i; i++ )
    {
        zeros = zeros && (0U == addr[i]);
        ones  = ones && (OWN_BYTE_ONES == addr[i]);
    }
    return zeros || ones;
}

/* cppcheck-suppress misra-c2012-8.7 ; called by kdrv_ble.c, which is a file this check does not join to this one */
bool kdrv_ble_own_valid(const uint8_t *addr)
{
    bool valid = false;

    if ( NULL != addr )
    {
        valid = (OWN_KIND_STATIC == (addr[OWN_TOP] & OWN_KIND_MASK)) && !own_random_flat(addr);
    }
    return valid;
}

/* cppcheck-suppress misra-c2012-8.7 ; called by kdrv_ble.c, which is a file this check does not join to this one */
void kdrv_ble_own_static(uint8_t *addr)
{
    if ( NULL != addr )
    {
        addr[OWN_TOP] = (uint8_t)(addr[OWN_TOP] | OWN_KIND_STATIC);
        if ( own_random_flat(addr) )
        {
            addr[0] = (uint8_t)(addr[0] ^ OWN_FLIP_BIT);
        }
    }
}

/* cppcheck-suppress misra-c2012-8.7 ; called by kdrv_ble.c, which is a file this check does not join to this one */
kdrv_ble_own_t kdrv_ble_own_pick(bool stored, uint32_t bonds)
{
    kdrv_ble_own_t own = KDRV_BLE_OWN_NEW;

    if ( stored )
    {
        own = KDRV_BLE_OWN_STORED;
    }
    else if ( 0U < bonds )
    {
        own = KDRV_BLE_OWN_PUBLIC;
    }
    else
    {
        /* nobody is bonded: a new one costs no phone anything */
    }
    return own;
}
