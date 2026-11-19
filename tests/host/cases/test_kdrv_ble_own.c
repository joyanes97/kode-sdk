/* SPDX-License-Identifier: GPL-3.0-or-later */
/**
 * @file test_kdrv_ble_own.c
 * @brief The board's own Bluetooth address: which one it advertises, and the shape of a random static one.
 */
#include <string.h>

#include "unity.h"

#include "kdrv_ble_own.h"

/** @brief The byte whose two top bits say what kind of random address it is. */
#define OWN_TOP          (KDRV_BLE_OWN_LEN - 1U)
/** @brief Those two bits, and their value for a static address. */
#define OWN_KIND_STATIC  0xC0U
/** @brief The rest of that byte. */
#define OWN_RANDOM_MASK  0x3FU
/** @brief Bonds a board may hold: the ceiling, three. */
#define OWN_BONDS_FULL   3U
/** @brief How many random patterns the shaping is tried on. */
#define OWN_PATTERNS     256U

TEST_CASE("a random static address is valid; the other kinds, flat ones and NULL are not", "[ble_own]")
{
    uint8_t const ok[KDRV_BLE_OWN_LEN]     = { 0x12U, 0x34U, 0x56U, 0x78U, 0x9AU, 0xC3U };
    uint8_t const pub[KDRV_BLE_OWN_LEN]    = { 0x12U, 0x34U, 0x56U, 0x78U, 0x9AU, 0x03U };
    uint8_t const rpa[KDRV_BLE_OWN_LEN]    = { 0x12U, 0x34U, 0x56U, 0x78U, 0x9AU, 0x43U };
    uint8_t const zeros[KDRV_BLE_OWN_LEN]  = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0xC0U };
    uint8_t const ones[KDRV_BLE_OWN_LEN]   = { 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU };
    uint8_t const one_bit[KDRV_BLE_OWN_LEN] = { 0x01U, 0x00U, 0x00U, 0x00U, 0x00U, 0xC0U };

    TEST_ASSERT_TRUE(kdrv_ble_own_valid(ok));
    TEST_ASSERT_TRUE(kdrv_ble_own_valid(one_bit));
    TEST_ASSERT_FALSE(kdrv_ble_own_valid(pub));
    TEST_ASSERT_FALSE(kdrv_ble_own_valid(rpa));
    TEST_ASSERT_FALSE(kdrv_ble_own_valid(zeros));
    TEST_ASSERT_FALSE(kdrv_ble_own_valid(ones));
    TEST_ASSERT_FALSE(kdrv_ble_own_valid(NULL));
}

TEST_CASE("six bytes of any kind come out a valid static address, the rest of their bits kept", "[ble_own]")
{
    for ( uint32_t p = 0U; OWN_PATTERNS > p; p++ )
    {
        uint8_t a[KDRV_BLE_OWN_LEN];
        uint8_t was[KDRV_BLE_OWN_LEN];

        for ( uint32_t i = 0U; KDRV_BLE_OWN_LEN > i; i++ )
        {
            a[i] = (uint8_t)((p * 37U) + (i * 101U));
        }
        (void)memcpy(was, a, sizeof(a));
        kdrv_ble_own_static(a);
        TEST_ASSERT_TRUE(kdrv_ble_own_valid(a));
        TEST_ASSERT_EQUAL_HEX8(OWN_KIND_STATIC, a[OWN_TOP] & OWN_KIND_STATIC);
        TEST_ASSERT_EQUAL_HEX8(was[OWN_TOP] & OWN_RANDOM_MASK, a[OWN_TOP] & OWN_RANDOM_MASK);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(&was[1], &a[1], OWN_TOP - 1U);
    }
}

TEST_CASE("all zeros and all ones come out valid by one bit of the first byte", "[ble_own]")
{
    uint8_t zeros[KDRV_BLE_OWN_LEN] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U };
    uint8_t ones[KDRV_BLE_OWN_LEN]  = { 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU };
    uint8_t const zeros_out[KDRV_BLE_OWN_LEN] = { 0x01U, 0x00U, 0x00U, 0x00U, 0x00U, 0xC0U };
    uint8_t const ones_out[KDRV_BLE_OWN_LEN]  = { 0xFEU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU };

    kdrv_ble_own_static(zeros);
    kdrv_ble_own_static(ones);
    kdrv_ble_own_static(NULL);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(zeros_out, zeros, KDRV_BLE_OWN_LEN);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ones_out, ones, KDRV_BLE_OWN_LEN);
    TEST_ASSERT_TRUE(kdrv_ble_own_valid(zeros));
    TEST_ASSERT_TRUE(kdrv_ble_own_valid(ones));
}

TEST_CASE("a stored address wins; without one, bonds keep the public address and none makes a new one", "[ble_own]")
{
    TEST_ASSERT_EQUAL(KDRV_BLE_OWN_STORED, kdrv_ble_own_pick(true, 0U));
    TEST_ASSERT_EQUAL(KDRV_BLE_OWN_STORED, kdrv_ble_own_pick(true, OWN_BONDS_FULL));
    TEST_ASSERT_EQUAL(KDRV_BLE_OWN_PUBLIC, kdrv_ble_own_pick(false, 1U));
    TEST_ASSERT_EQUAL(KDRV_BLE_OWN_PUBLIC, kdrv_ble_own_pick(false, OWN_BONDS_FULL));
    TEST_ASSERT_EQUAL(KDRV_BLE_OWN_NEW, kdrv_ble_own_pick(false, 0U));
}
