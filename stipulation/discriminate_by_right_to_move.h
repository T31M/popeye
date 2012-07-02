#if !defined(STIPULATION_DISCRIMINATE_BY_RIGHT_TO_MOVE_H)
#define STIPULATION_DISCRIMINATE_BY_RIGHT_TO_MOVE_H

#include "solving/battle_play/attack_play.h"

/* This module provides the STDiscriminateByRightToMove slice type.
 * Such slices decide on the successor slice depending on the right to move.
 */

/* Allocate a STDiscriminateByRightToMove slice
 * @param white successor if White has the right to move
 * @param black successof if Black has the right to move
 */
slice_index alloc_discriminate_by_right_to_move_slice(slice_index white,
                                                      slice_index black);

/* Try to solve in n half-moves after a defense.
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @return length of solution found and written, i.e.:
 *            slack_length-2 defense has turned out to be illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type discriminate_by_right_to_move_attack(slice_index si,
                                                      stip_length_type n);

#endif
