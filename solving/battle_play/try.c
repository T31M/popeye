#include "stipulation/battle_play/try.h"
#include "pydata.h"
#include "pypipe.h"
#include "stipulation/branch.h"
#include "stipulation/battle_play/defense_play.h"
#include "stipulation/battle_play/branch.h"
#include "stipulation/battle_play/attack_play.h"
#include "trace.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

/* Table where refutations are collected
 */
table refutations;

/* Maximum number of refutations to look for as indicated by the user
 */
static unsigned int user_set_max_nr_refutations;

/* are we currently solving refutations? */
static boolean are_we_solving_refutations;

/* Read the maximum number of refutations that the user is interested
 * to see
 * @param tok input token from which to read the number
 * @return true iff the number could be successfully read
 */
boolean read_max_nr_refutations(char const *tok)
{
  boolean result;
  char *end;
  unsigned long ul;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%s",tok);
  TraceFunctionParamListEnd();

  ul = strtoul(tok,&end,10);
  if (tok==end || ul>UINT_MAX)
  {
    user_set_max_nr_refutations = 0;
    result = false;
  }
  else
  {
    user_set_max_nr_refutations = ul;
    result = true;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Set the maximum number of refutations that the user is interested
 * to see to some value
 * @param mnr maximum number of refutations that the user is
 *            interested to see
 */
void set_max_nr_refutations(unsigned int mnr)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",mnr);
  TraceFunctionParamListEnd();

  user_set_max_nr_refutations = mnr;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Allocate a STTrySolver defender slice.
 * @return index of allocated slice
 */
static slice_index alloc_try_solver(void)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  result = alloc_pipe(STTrySolver);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Try to defend after an attacking move
 * When invoked with some n, the function assumes that the key doesn't
 * solve in less than n half moves.
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @return <=n solved  - return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - acceptable number of refutations found
 *         n+4 refuted - >acceptable number of refutations found
 */
stip_length_type try_solver_defend_in_n(slice_index si,
                                        stip_length_type n,
                                        stip_length_type n_max_unsolvable)
{
  stip_length_type result;
  slice_index const next = slices[si].u.branch.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_max_unsolvable);
  TraceFunctionParamListEnd();

  if (table_length(refutations)>0)
  {
    defense_defend_in_n(next,n,n_max_unsolvable);

    are_we_solving_refutations = true;
    defense_can_defend_in_n(next,n,n);
    are_we_solving_refutations = false;

    result = n+2;
  }
  else
    result = defense_defend_in_n(next,n,n_max_unsolvable);

  free_table();
  refutations = table_nil;

  TraceFunctionExit(__func__);
  TraceValue("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether there are defenses after an attacking move
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @return <=n solved  - return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - <=acceptable number of refutations found
 *         n+4 refuted - >acceptable number of refutations found
 */
stip_length_type try_solver_can_defend_in_n(slice_index si,
                                            stip_length_type n,
                                            stip_length_type n_max_unsolvable)
{
  stip_length_type result;
  slice_index const next = slices[si].u.branch.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_max_unsolvable);
  TraceFunctionParamListEnd();

  assert(refutations==table_nil);
  refutations = allocate_table();

  result = defense_can_defend_in_n(next,n,n_max_unsolvable);

  if (result<=n)
  {
    if (table_length(refutations)>0)
      result = n+2;
  }
  else
  {
    free_table();
    refutations = table_nil;
  }

  TraceFunctionExit(__func__);
  TraceValue("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Allocate a STRefutationsCollector slice.
 * @return index of allocated slice
 */
static slice_index alloc_refutations_collector_slice(void)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  result = alloc_pipe(STRefutationsCollector);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether there is a solution in n half moves.
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @return length of solution found, i.e.:
 *            slack_length_battle-2 defense has turned out to be illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type
refutations_collector_has_solution_in_n(slice_index si,
                                        stip_length_type n,
                                        stip_length_type n_max_unsolvable)
{
  stip_length_type result;
  slice_index const next = slices[si].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_max_unsolvable);
  TraceFunctionParamListEnd();

  if (are_we_solving_refutations)
  {
    if (is_current_move_in_table(refutations))
      attack_solve_in_n(next,n,n);

    result = n;
  }
  else
  {
    result = attack_has_solution_in_n(next,n,n_max_unsolvable);

    if (result>n)
    {
      assert(get_top_table()==refutations);
      TraceValue("%u\n",get_top_table());
      append_to_top_table();
      if (table_length(get_top_table())<=user_set_max_nr_refutations)
        result = n;
    }
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Try to solve in n half-moves after a defense.
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @return length of solution found and written, i.e.:
 *            slack_length_battle-2 defense has turned out to be illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type
refutations_collector_solve_in_n(slice_index si,
                                 stip_length_type n,
                                 stip_length_type n_max_unsolvable)
{
  stip_length_type result;
  slice_index const next = slices[si].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_max_unsolvable);
  TraceFunctionParamListEnd();

  if (is_current_move_in_table(refutations))
    result = n+2;
  else
    result = attack_solve_in_n(next,n,n_max_unsolvable);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Insert try handler slices into the stipulation if applicable
 * @param si identifies slice to be replaced
 * @param st address of structure defining traversal
 */
static void insert_try_handlers(slice_index si, stip_structure_traversal *st)
{
  boolean * const inserted = st->param;
  slice_index defense_move;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children(si,st);

  defense_move = branch_find_slice(STDefenseMove,si);
  if (defense_move!=no_slice
      && slices[defense_move].u.branch.length>slack_length_battle)
  {
    slice_index const prototypes[] =
    {
      alloc_try_solver(),
      alloc_refutations_collector_slice()
    };

    enum
    {
      nr_prototypes = sizeof prototypes / sizeof prototypes[0]
    };

    root_branch_insert_slices(si,prototypes,nr_prototypes);

    *inserted = true;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Instrument the stipulation representation so that it can deal with
 * tries
 * @param si identifies slice where to start
 * @return true iff the stipulation could be instrumented (i.e. iff
 *         try play applies to the stipulation)
 */
boolean stip_insert_try_handlers(slice_index si)
{
  boolean result = false;
  stip_structure_traversal st;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceStipulation(si);

  stip_structure_traversal_init(&st,&result);
  stip_structure_traversal_override_single(&st,
                                           STAttackRoot,&insert_try_handlers);
  stip_traverse_structure(si,&st);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}
