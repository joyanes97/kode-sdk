/* SPDX-License-Identifier: GPL-3.0-or-later */
/**
 * @file kdrv_ble_own.h
 * @brief The NimBLE-free half of the board's own Bluetooth address: which one it advertises, and what a random static one must look like.
 *
 * A phone files a bond under the address it met. When the board loses every
 * bond the phone keeps its half, and iOS refuses the link until a human
 * forgets the device by hand; under an address it never met, the board is a
 * stranger and pairs from zero. So the board advertises a random static
 * address kept in NVS, and draws a new one when every bond is gone. A board
 * that holds bonds and keeps no address goes on with its public one, which is
 * the address its phones know.
 *
 * Addresses are in NimBLE's order: byte 0 first on the air, byte 5 the most
 * significant, whose top two bits say what kind of random address it is.
 */
#ifndef KDRV_BLE_OWN_H_
#define KDRV_BLE_OWN_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KDRV_BLE_OWN_LEN       6U          /**< Bytes in a Bluetooth address. */
#define KDRV_BLE_OWN_NVS_NS    "kble"      /**< The NVS namespace of the address the board advertises. */
#define KDRV_BLE_OWN_NVS_KEY   "addr"      /**< Its key: the six bytes, in NimBLE's order. */

/** @brief Which address the board advertises at the start of the stack. */
typedef enum
{
    KDRV_BLE_OWN_STORED = 0, /**< The random static address kept in NVS.                      */
    KDRV_BLE_OWN_PUBLIC,     /**< The controller's own: none is kept and the store has bonds. */
    KDRV_BLE_OWN_NEW         /**< A new random static one: none is kept and nobody is bonded. */
} kdrv_ble_own_t;

/**
 * @brief Whether six bytes are a random static address NimBLE takes.
 *
 * The two top bits of byte 5 set, and the 46 others neither all 0 nor all 1
 * (Core v5.0, Vol 6, Part B, 1.3.2.1).
 *
 * @param addr The six bytes; NULL is not one.
 * @return true when they are one.
 */
bool kdrv_ble_own_valid(const uint8_t *addr);

/**
 * @brief Make six random bytes a random static address, in place.
 *
 * Sets the two top bits of byte 5, and turns over the lowest bit of byte 0
 * when the 46 others came out all 0 or all 1, so the result is always valid.
 *
 * @param addr The six bytes. NULL does nothing.
 */
void kdrv_ble_own_static(uint8_t *addr);

/**
 * @brief Which address the board advertises at the start of the stack.
 * @param stored Whether NVS keeps a valid address.
 * @param bonds  The peers the bond store holds.
 * @return The stored one when there is one; else the public one while
 *         anybody is bonded; else a new one.
 */
kdrv_ble_own_t kdrv_ble_own_pick(bool stored, uint32_t bonds);

#ifdef __cplusplus
}
#endif

#endif /* KDRV_BLE_OWN_H_ */
