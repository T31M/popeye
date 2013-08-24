#if !defined(SOLVING_BATTLE_PLAY_TRY_H)
#define SOLVING_BATTLE_PLAY_TRY_H

#include "utilities/table.h"
#include "stipulation/stipulation.h"

/* This module provides functionality dealing with writing tries.
 * This functionality is a superset of that provided by solution_writer
 */

/* Table where refutations are collected
 * Exposed for read-only access only */
extern table refutations;

/* Read the maximum number of refutations that the user is interested
 * to see
 * @param tok input token from which to read the number
 * @return true iff the number could be successfully read
 */
boolean read_max_nr_refutations(char const *tok);

/* Set the maximum number of refutations that the user is interested
 * to see to some value
 * @param mnr maximum number of refutations that the user is
 *            interested to see
 */
void set_max_nr_refutations(unsigned int mnr);

/* Allocate a STRefutationsAllocator defender slice.
 * @return index of allocated slice
 */
slice_index alloc_refutations_allocator(void);

/* Try to solve in n half-moves.
 * @param si slice index
 * @param n maximum number of half moves
 * @return length of solution found and written, i.e.:
 *            previous_move_is_illegal the move just played (or being played)
 *                                     is illegal
 *            immobility_on_next_move  the moves just played led to an
 *                                     unintended immobility on the next move
 *            <=n+1 length of shortest solution found (n+1 only if in next
 *                                     branch)
 *            n+2 no solution found in this branch
 *            n+3 no solution found in next branch
 */
stip_length_type refutations_allocator_solve(slice_index si, stip_length_type n);

/* Try to defend after an attacking move
 * When invoked with some n, the function assumes that the key doesn't
 * solve in less than n half moves.
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 *         <=n solved  - <=acceptable number of refutations found
 *                       return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - >acceptable number of refutations found */
stip_length_type refutations_solver_solve(slice_index si, stip_length_type n);

/* Allocate a STRefutationsCollector slice.
 * @param max_nr_refutations maximum number of refutations to be allowed
 * @return index of allocated slice
 */
slice_index alloc_refutations_collector_slice(unsigned int max_nr_refutations);

/* Try to solve in n half-moves.
 * @param si slice index
 * @param n maximum number of half moves
 * @return length of solution found and written, i.e.:
 *            previous_move_is_illegal the move just played (or being played)
 *                                     is illegal
 *            immobility_on_next_move  the moves just played led to an
 *                                     unintended immobility on the next move
 *            <=n+1 length of shortest solution found (n+1 only if in next
 *                                     branch)
 *            n+2 no solution found in this branch
 *            n+3 no solution found in next branch
 */
stip_length_type refutations_collector_solve(slice_index si,
                                              stip_length_type n);

/* Allocate a STRefutationsAvoider slice.
 * @param max_nr_refutations maximum number of refutations to be allowed
 * @return index of allocated slice
 */
slice_index alloc_refutations_avoider_slice(unsigned int max_nr_refutations);

/* Try to solve in n half-moves.
 * @param si slice index
 * @param n maximum number of half moves
 * @return length of solution found and written, i.e.:
 *            previous_move_is_illegal the move just played (or being played)
 *                                     is illegal
 *            immobility_on_next_move  the moves just played led to an
 *                                     unintended immobility on the next move
 *            <=n+1 length of shortest solution found (n+1 only if in next
 *                                     branch)
 *            n+2 no solution found in this branch
 *            n+3 no solution found in next branch
 */
stip_length_type refutations_avoider_solve(slice_index si, stip_length_type n);

/* Try to solve in n half-moves.
 * @param si slice index
 * @param n maximum number of half moves
 * @return length of solution found and written, i.e.:
 *            previous_move_is_illegal the move just played (or being played)
 *                                     is illegal
 *            immobility_on_next_move  the moves just played led to an
 *                                     unintended immobility on the next move
 *            <=n+1 length of shortest solution found (n+1 only if in next
 *                                     branch)
 *            n+2 no solution found in this branch
 *            n+3 no solution found in next branch
 */
stip_length_type refutations_filter_solve(slice_index si, stip_length_type n);

/* Instrument the stipulation structure with slices solving tries
 * @param si identifies entry branch into stipulation
 */
void stip_insert_try_solvers(slice_index si);

#endif
