#if !defined(PYSELFCG_H)
#define PYSELFCG_H

/* STSelfCheckGuard slice - stops solutions with moves that expose the
 * own king
 */

#include "stipulation/battle_play/attack_play.h"
#include "stipulation/battle_play/defense_play.h"

/* Instrument a stipulation with slices dealing with selfcheck detection
 * @param si root of branch to be instrumented
 */
void stip_insert_selfcheck_guards(slice_index si);

/* Allocate a STSelfCheckGuard slice
 * @return allocated slice
 */
slice_index alloc_selfcheck_guard_slice(void);

/* Try to solve in n half-moves after a defense.
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @return length of solution found and written, i.e.:
 *            slack_length-2 defense has turned out to be illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type selfcheck_guard_attack(slice_index si, stip_length_type n);

/* Try to defend after an attacking move
 * When invoked with some n, the function assumes that the key doesn't
 * solve in less than n half moves.
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 *         <=n solved  - <=acceptable number of refutations found
 *                       return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - >acceptable number of refutations found */
stip_length_type selfcheck_guard_defend(slice_index si, stip_length_type n);

#endif
