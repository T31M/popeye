#if !defined(PYKEEPMT_H)
#define PYKEEPMT_H

/* Implementation of the "keep mating piece" optimisation:
 * Solving stops once the last piece of the mating side that could
 * deliver mate has been captured.
 */

#include "stipulation/battle_play/attack_play.h"
#include "stipulation/battle_play/defense_play.h"
#include "stipulation/help_play/play.h"

/* Instrument stipulation with STKeepMatingGuard slices
 */
void stip_insert_keepmating_guards(void);

/* Try to defend after an attempted key move at root level
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_min minimum number of half-moves of interesting variations
 *              (slack_length_battle <= n_min <= slices[si].u.branch.length)
 * @param max_nr_refutations how many refutations should we look for
 * @return <slack_length_battle - stalemate
 *         <=n solved  - return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - <=max_nr_refutations refutations found
 *         n+4 refuted - >max_nr_refutations refutations found
 */
stip_length_type keepmating_guard_root_defend(slice_index si,
                                              stip_length_type n,
                                              stip_length_type n_min,
                                              unsigned int max_nr_refutations);

/* Determine whether there is a solution in n half moves.
 * @param si slice index of slice being solved
 * @param n maximum number of half moves until end state has to be reached
 * @param n_min minimal number of half moves to try
 * @return length of solution found, i.e.:
 *            <n_min defense put defender into self-check
 *            n_min..n length of shortest solution found
 *            >n no solution found
 *         (the second case includes the situation in self
 *         stipulations where the defense just played has reached the
 *         goal (in which case n_min<slack_length_battle and we return
 *         n_min)
 */
stip_length_type
keepmating_guard_direct_has_solution_in_n(slice_index si,
                                          stip_length_type n,
                                          stip_length_type n_min);

/* Solve a slice
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @param n_min minimal number of half moves to try
 * @return number of half moves effectively used
 *         n+2 if no solution was found
 *         (n-slack_length_battle)%2 if the previous move led to a
 *            dead end (e.g. self-check)
 */
stip_length_type keepmating_guard_direct_solve_in_n(slice_index si,
                                                    stip_length_type n,
                                                    stip_length_type n_min);

/* Try to defend after an attempted key move at non-root level
 * When invoked with some n, the function assumes that the key doesn't
 * solve in less than n half moves.
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_min minimum number of half-moves of interesting variations
 *              (slack_length_battle <= n_min <= slices[si].u.branch.length)
 * @return true iff the defender can defend
 */
boolean keepmating_guard_defend_in_n(slice_index si,
                                     stip_length_type n,
                                     stip_length_type n_min);

/* Determine whether there are refutations after an attempted key move
 * at non-root level
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_min minimum number of half-moves of interesting variations
 *              (slack_length_battle <= n_min <= slices[si].u.branch.length)
 * @param max_nr_refutations how many refutations should we look for
 * @return <slack_length_battle - stalemate
           <=n solved  - return value is maximum number of moves
                         (incl. defense) needed
           n+2 refuted - <=max_nr_refutations refutations found
           n+4 refuted - >max_nr_refutations refutations found
 */
stip_length_type
keepmating_guard_can_defend_in_n(slice_index si,
                                 stip_length_type n,
                                 stip_length_type n_min,
                                 unsigned int max_nr_refutations);

/* Determine whether the defense just played defends against the threats.
 * @param threats table containing the threats
 * @param len_threat length of threat(s) in table threats
 * @param si slice index
 * @param n maximum number of moves until goal
 * @return true iff the defense defends against at least one of the
 *         threats
 */
boolean keepmating_guard_are_threats_refuted_in_n(table threats,
                                                  stip_length_type len_threat,
                                                  slice_index si,
                                                  stip_length_type n);

/* Determine and write the threats after the move that has just been
 * played.
 * @param threats table where to add threats
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @param n_min minimal number of half moves to try
 * @return length of threats
 *         (n-slack_length_battle)%2 if the attacker has something
 *           stronger than threats (i.e. has delivered check)
 *         n+2 if there is no threat
 */
stip_length_type
keepmating_guard_direct_solve_threats_in_n(table threats,
                                           slice_index si,
                                           stip_length_type n,
                                           stip_length_type n_min);

/* Solve in a number of half-moves
 * @param si identifies slice
 * @param n exact number of half moves until end state has to be reached
 * @return true iff >=1 solution was found
 */
boolean keepmating_guard_help_solve_in_n(slice_index si, stip_length_type n);

/* Determine whether there is a solution in n half moves.
 * @param si slice index of slice being solved
 * @param n exact number of half moves until end state has to be reached
 * @return true iff >= 1 solution has been found
 */
boolean keepmating_guard_help_has_solution_in_n(slice_index si,
                                            stip_length_type n);
  
/* Determine and write threats
 * @param threats table where to add first moves
 * @param si slice index of slice being solved
 * @param n exact number of half moves until end state has to be reached
 */
void keepmating_guard_help_solve_threats_in_n(table threats,
                                              slice_index si,
                                              stip_length_type n);


/* Solve in a number of half-moves
 * @param si identifies slice
 * @param n exact number of half moves until end state has to be reached
 * @return true iff >=1 solution was found
 */
boolean keepmating_guard_series_solve_in_n(slice_index si, stip_length_type n);

/* Determine whether there is a solution in n half moves.
 * @param si slice index of slice being solved
 * @param n exact number of half moves until end state has to be reached
 * @return true iff >= 1 solution has been found
 */
boolean keepmating_guard_series_has_solution_in_n(slice_index si,
                                                  stip_length_type n);

/* Determine and write threats
 * @param threats table where to add first moves
 * @param si slice index of slice being solved
 * @param n exact number of half moves until end state has to be reached
 */
void keepmating_guard_series_solve_threats_in_n(table threats,
                                                slice_index si,
                                                stip_length_type n);

#endif
