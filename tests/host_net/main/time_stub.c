/* SPDX-License-Identifier: GPL-3.0-or-later */
/**
 * @file time_stub.c
 * @brief khal_time_ms() for a binary that tests budgets, not time, and the
 *        wall clock kdrv_net names when a certificate's dates are refused.
 */
#include <stdio.h>

#include "khal_time.h"
#include "kdrv_clocks.h"
#include "time_stub.h"

/** @brief What khal_time_ms() reports. */
static uint32_t s_now_ms;

/** @brief What kdrv_clocks_wall_iso() writes; NULL while the clock is not set. */
static const char *s_wall_iso;

/** @brief What kdrv_clocks_wall_approx() reports. */
static bool s_wall_approx;

void time_stub_set_ms(uint32_t ms)
{
    s_now_ms = ms;
}

void time_stub_advance_ms(uint32_t ms)
{
    s_now_ms += ms;
}

uint32_t khal_time_ms(void)
{
    return s_now_ms;
}

void time_stub_set_wall(const char *iso, bool approx)
{
    s_wall_iso    = iso;
    s_wall_approx = approx;
}

kode_status_t kdrv_clocks_wall_iso(char *out, size_t n)
{
    kode_status_t rc = KODE_DENIED;

    if ( (NULL != s_wall_iso) && (NULL != out) && (0U != n) )
    {
        (void)snprintf(out, n, "%s", s_wall_iso);
        rc = KODE_OK;
    }
    return rc;
}

bool kdrv_clocks_wall_approx(void)
{
    return s_wall_approx;
}
