#if !defined(STIPULATION_GOAL_EXCHANGE_BY_REBIRTH_REACHED_TESTER_H)
#define STIPULATION_GOAL_EXCHANGE_BY_REBIRTH_REACHED_TESTER_H

#include "stipulation/battle_play/attack_play.h"

/* This module provides functionality dealing with slices that detect
 * whether a exchange (by rebirth) goal has just been reached
 */

/* Allocate a system of slices that tests whether exchange_by_rebirth has been reached
 * @return index of entry slice
 */
slice_index alloc_goal_exchange_by_rebirth_reached_tester_system(void);

/* Try to solve in n half-moves after a defense.
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @return length of solution found and written, i.e.:
 *            slack_length-2 defense has turned out to be illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type
goal_exchange_by_rebirth_reached_tester_attack(slice_index si,
                                               stip_length_type n);

#endif
