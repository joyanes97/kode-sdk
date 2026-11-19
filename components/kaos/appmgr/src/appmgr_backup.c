/* SPDX-License-Identifier: GPL-3.0-or-later */
/**
 * @file appmgr_backup.c
 * @brief Which installed apps have a copy on the card.
 */
#include "appmgr_backup.h"

#include <string.h>

/**
 * @brief Where the table holds @p seq.
 * @param t   The table.
 * @param seq The app's install number.
 * @return Its place; t->count when the table does not hold it.
 */
static uint32_t backup_find(const appmgr_backup_t *t, uint32_t seq)
{
    uint32_t at = t->count;

    for ( uint32_t i = 0U; i < t->count; ++i )
    {
        if ( t->seq[i] == seq )
        {
            at = i;
            break;
        }
    }
    return at;
}

void appmgr_backup_reset(appmgr_backup_t *t)
{
    t->count = 0U;
}

appmgr_backup_state_t appmgr_backup_get(const appmgr_backup_t *t, uint32_t seq)
{
    appmgr_backup_state_t state = APPMGR_BACKUP_UNSEEN;
    uint32_t const        at    = backup_find(t, seq);

    if ( at < t->count )
    {
        state = (appmgr_backup_state_t)t->state[at];
    }
    return state;
}

void appmgr_backup_set(appmgr_backup_t *t, uint32_t seq, appmgr_backup_state_t state)
{
    uint32_t const at = backup_find(t, seq);

    if ( APPMGR_BACKUP_UNSEEN == state )
    {
        appmgr_backup_forget(t, seq);
    }
    else if ( at < t->count )
    {
        t->state[at] = (uint8_t)state;
    }
    else if ( KAPPS_MAX_ENTRIES > t->count )
    {
        t->seq[t->count]   = seq;
        t->state[t->count] = (uint8_t)state;
        t->count += 1U;
    }
    else
    {
        /* Full: the app stays unseen, and the card is asked about it again. */
    }
}

void appmgr_backup_forget(appmgr_backup_t *t, uint32_t seq)
{
    uint32_t const at = backup_find(t, seq);

    if ( at < t->count )
    {
        /* The last one takes its place: the table keeps no order. */
        t->count -= 1U;
        t->seq[at]   = t->seq[t->count];
        t->state[at] = t->state[t->count];
    }
}

/**
 * @brief The live entry of @p d installed as @p seq.
 * @param d   The directory.
 * @param seq The install number.
 * @return The entry; NULL when no live entry has it.
 */
static const kapps_entry_t *backup_entry(const kapps_dir_t *d, uint32_t seq)
{
    const kapps_entry_t *found = NULL;

    for ( uint32_t i = 0U; (i < d->count) && (NULL == found); ++i )
    {
        if ( ((d->e[i].flags & KAPPS_ENTRY_VALID) != 0U) && (d->e[i].seq == seq) )
        {
            found = &d->e[i];
        }
    }
    return found;
}

void appmgr_backup_prune(appmgr_backup_t *t, const kapps_dir_t *d)
{
    uint32_t i = 0U;

    while ( i < t->count )
    {
        if ( NULL == backup_entry(d, t->seq[i]) )
        {
            /* The last one takes this place: it is looked at next. */
            appmgr_backup_forget(t, t->seq[i]);
        }
        else
        {
            ++i;
        }
    }
}

const kapps_entry_t *appmgr_backup_next(const appmgr_backup_t *t, const kapps_dir_t *d)
{
    const kapps_entry_t *next = NULL;

    for ( uint32_t i = 0U; i < d->count; ++i )
    {
        if ( (d->e[i].flags & KAPPS_ENTRY_VALID) != 0U )
        {
            appmgr_backup_state_t const state = appmgr_backup_get(t, d->e[i].seq);
            if ( (APPMGR_BACKUP_UNSEEN == state) && ((NULL == next) || (d->e[i].seq < next->seq)) )
            {
                next = &d->e[i];
            }
        }
    }
    return next;
}

bool appmgr_backup_same(uint32_t file_len, const uint8_t *file_build,
                        uint32_t image_len, const uint8_t *image_build)
{
    return (file_len == image_len)
        && (memcmp(file_build, image_build, KAPP_BUILD_LEN) == 0);
}

/**
 * @brief Whether @p build is an id of zeros: what a file with no build id reads as.
 * @param build KAPP_BUILD_LEN bytes.
 * @return true when every byte is zero.
 */
static bool written_none(const uint8_t *build)
{
    bool none = true;

    for ( uint32_t i = 0U; (i < KAPP_BUILD_LEN) && none; ++i )
    {
        none = (0U == build[i]);
    }
    return none;
}

bool appmgr_written_has(const appmgr_written_t *w, const uint8_t *build)
{
    bool held = false;

    if ( !written_none(build) )
    {
        for ( uint32_t i = 0U; (i < w->count) && (!held); ++i )
        {
            held = (memcmp(w->build[i], build, KAPP_BUILD_LEN) == 0);
        }
    }
    return held;
}

bool appmgr_written_add(appmgr_written_t *w, const uint8_t *build)
{
    bool changed = false;

    if ( (!written_none(build)) && (!appmgr_written_has(w, build)) )
    {
        if ( APPMGR_WRITTEN_MAX <= w->count )
        {
            /* Full: the oldest goes, and the others move up one place. */
            for ( uint32_t i = 1U; i < APPMGR_WRITTEN_MAX; ++i )
            {
                (void)memcpy(w->build[i - 1U], w->build[i], KAPP_BUILD_LEN);
            }
            w->count = APPMGR_WRITTEN_MAX - 1U;
        }
        (void)memcpy(w->build[w->count], build, KAPP_BUILD_LEN);
        w->count += 1U;
        changed = true;
    }
    return changed;
}
