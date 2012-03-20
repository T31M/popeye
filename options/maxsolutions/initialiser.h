#if !defined(OPTIONS_MAXSOLUTIONS_INITIALISER_H)
#define OPTIONS_MAXSOLUTIONS_INITIALISER_H

#include "py.h"
#include "stipulation/battle_play/attack_play.h"

/* This module provides functionality dealing with
 * STMaxSolutionsInitialiser stipulation slice type.
 * Slices of this type make sure that solving stops after the maximum
 * number of solutions have been found
 */

/* Allocate a STMaxSolutionsInitialiser slice.
 * @return allocated slice
 */
slice_index alloc_maxsolutions_initialiser_slice(void);

/* Try to solve in n half-moves after a defense.
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @return length of solution found and written, i.e.:
 *            slack_length-2 defense has turned out to be illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type maxsolutions_initialiser_attack(slice_index si, stip_length_type n);

#endif
