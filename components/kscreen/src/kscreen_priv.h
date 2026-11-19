/* SPDX-License-Identifier: GPL-3.0-or-later */
/**
 * @file kscreen_priv.h
 * @brief What kscreen.c tells the rest of the component about the display it
 *        opened; private to this component.
 */
#ifndef KSCREEN_PRIV_H_
#define KSCREEN_PRIV_H_

#include "sdkconfig.h"

#include "kscreen.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief The panel's framebuffers, each of which a present paints its strips into once. */
#define KSCREEN_PRESENT_BUFFERS  2U

/**
 * @brief Wait for the flush posted last, LVGL's or a present's, to complete.
 *
 * Until it has, the engine may still read the framebuffer it flips to, and
 * the back buffer is not free. Does nothing when no flush is posted.
 *
 * @param s The renderer.
 */
void kscreen_flush_settle(kscreen_t *s);

/**
 * @brief Hand rows of the panel to AO_Display, the frame's last flush or a chunk of it.
 *
 * With one of the panel's framebuffers as @p pixels and @p last set, the
 * panel flips to that buffer at its next refresh and nothing is copied.
 * Exactly one completion follows, which kscreen_flush_settle() waits for.
 *
 * @param s      The renderer.
 * @param rows   The rectangle, in LVGL's inclusive coordinates on the panel.
 * @param pixels Its pixels, RGB565.
 * @param last   Whether this is the frame's last flush.
 */
void kscreen_flush_post(kscreen_t *s, const lv_area_t *rows, uint8_t *pixels, bool last);

#if defined(CONFIG_KODE_COMPOSE)
/**
 * @brief The renderer that opened a display.
 *
 * Asks kscreen's own record, never the display's user data: a display
 * kscreen did not open may carry anything there.
 *
 * @param disp The display; may be NULL.
 * @return The kscreen_t kscreen_open() bound to @p disp, or NULL.
 */
kscreen_t *kscreen_of(lv_display_t const *disp);
#endif

#ifdef __cplusplus
}
#endif

#endif /* KSCREEN_PRIV_H_ */
