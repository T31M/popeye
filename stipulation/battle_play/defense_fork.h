#if !defined(STIPULATION_BATTLE_PLAY_DEFENSE_FORK_H)
#define STIPULATION_BATTLE_PLAY_DEFENSE_FORK_H

#include "stipulation/battle_play/defense_play.h"

/* This module provides functionality dealing with the defending side
 * in STDefenseFork stipulation slices.
 */

/* Allocate a STDefenseFork defender slice.
 * @param length maximum number of half-moves of slice (+ slack)
 * @param min_length minimum number of half-moves of slice (+ slack)
 * @param proxy_to_next identifies slice leading towards goal
 * @return index of allocated slice
 */
slice_index alloc_defense_fork_slice(stip_length_type length,
                                     stip_length_type min_length,
                                     slice_index proxy_to_next);

/* Traversal of the moves beyond a series fork slice 
 * @param si identifies root of subtree
 * @param st address of structure representing traversal
 */
void stip_traverse_moves_defense_fork(slice_index si, stip_moves_traversal *st);

/* Try to defend after an attacking move
 * When invoked with some n, the function assumes that the key doesn't
 * solve in less than n half moves.
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @return <=n solved  - return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - <=acceptable number of refutations found
 *         n+4 refuted - >acceptable number of refutations found
 */
stip_length_type defense_fork_defend_in_n(slice_index si,
                                          stip_length_type n,
                                          stip_length_type n_max_unsolvable);

/* Determine whether there are defenses after an attacking move
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @return <=n solved  - return value is maximum number of moves
                         (incl. defense) needed
           n+2 refuted - <=acceptable number of refutations found
           n+4 refuted - >acceptable number of refutations found
 */
stip_length_type
defense_fork_can_defend_in_n(slice_index si,
                             stip_length_type n,
                             stip_length_type n_max_unsolvable);

#endif
