#if !defined(CONDITIONS_REPUBLICAN_H)
#define CONDITIONS_REPUBLICAN_H

#include "solving/solve.h"
#include "conditions/conditions.h"

#include <stddef.h>

extern ConditionNumberedVariantType RepublicanType;

/* Perform the necessary verification steps for solving a Republican
 * Chess problem
 * @param si identifies root slice of stipulation
 * @return true iff verification passed
 */
boolean republican_verifie_position(slice_index si);

/* Instrument a stipulation
 * @param si identifies root slice of stipulation
 */
void stip_insert_republican_king_placers(slice_index si);

/* Try to solve in n half-moves.
 * @param si slice index
 * @param n maximum number of half moves
 * @return length of solution found and written, i.e.:
 *            previous_move_is_illegal the move just played is illegal
 *            this_move_is_illegal     the move being played is illegal
 *            immobility_on_next_move  the moves just played led to an
 *                                     unintended immobility on the next move
 *            <=n+1 length of shortest solution found (n+1 only if in next
 *                                     branch)
 *            n+2 no solution found in this branch
 *            n+3 no solution found in next branch
 */
stip_length_type republican_king_placer_solve(slice_index si,
                                               stip_length_type n);

/* Try to solve in n half-moves.
 * @param si slice index
 * @param n maximum number of half moves
 * @return length of solution found and written, i.e.:
 *            previous_move_is_illegal the move just played is illegal
 *            this_move_is_illegal     the move being played is illegal
 *            immobility_on_next_move  the moves just played led to an
 *                                     unintended immobility on the next move
 *            <=n+1 length of shortest solution found (n+1 only if in next
 *                                     branch)
 *            n+2 no solution found in this branch
 *            n+3 no solution found in next branch
 */
stip_length_type republican_type1_dead_end_solve(slice_index si,
                                                  stip_length_type n);

#endif
