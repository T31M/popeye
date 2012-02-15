#include "solving/battle_play/continuation.h"
#include "stipulation/testing_pipe.h"
#include "stipulation/battle_play/branch.h"
#include "trace.h"

#include <assert.h>

/* Allocate a STContinuationSolver defender slice.
 * @return index of allocated slice
 */
slice_index alloc_continuation_solver_slice(void)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  result = alloc_testing_pipe(STContinuationSolver);

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
 * @return <slack_length - no legal defense found
 *         <=n solved  - <=acceptable number of refutations found
 *                       return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - >acceptable number of refutations found
 */
stip_length_type continuation_solver_defend(slice_index si, stip_length_type n)
{
  stip_length_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  result = defend(slices[si].u.fork.fork,n);
  if (slack_length<=result && result<=n)
  {
    stip_length_type const n_next = n<result ? n : result;
#if !defined(NDEBUG)
    stip_length_type const defend_result =
#endif
    defend(slices[si].u.fork.next,n_next);
    assert(defend_result==result);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static void insert_continuation_solvers_avoid_goal_branches(slice_index si,
                                                            stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (slices[si].u.branch.length>slack_length)
    stip_traverse_structure_children(si,st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* nested defense branch
 */
static void insert_continuation_solvers_postktey_play(slice_index si,
                                                      stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children(si,st);

  if (st->level!=structure_traversal_level_root
      && slices[si].u.branch.length>slack_length)
  {
    slice_index const prototype = alloc_continuation_solver_slice();
    battle_branch_insert_slices(si,&prototype,1);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* attack branch
 */
static void insert_continuation_solvers_attack(slice_index si,
                                               stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children(si,st);

  {
    slice_index const prototypes[] =
    {
        alloc_pipe(STSolvingContinuation),
        alloc_continuation_solver_slice()
    };
    enum { nr_prototypes = sizeof prototypes / sizeof prototypes[0] };
    battle_branch_insert_slices(si,prototypes,nr_prototypes);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static structure_traversers_visitors const continuation_solver_inserters[] =
{
  { STAttackAdapter,  &insert_continuation_solvers_avoid_goal_branches },
  { STDefenseAdapter, &insert_continuation_solvers_postktey_play       },
  { STReadyForAttack, &insert_continuation_solvers_attack              }
};

enum
{
  nr_continuation_solver_inserters = sizeof continuation_solver_inserters / sizeof continuation_solver_inserters[0]
};

/* Instrument the stipulation structure with STContinuationSolver slices
 * @param root_slice root slice of the stipulation
 */
void stip_insert_continuation_solvers(slice_index si)
{
  stip_structure_traversal st;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_structure_traversal_init(&st,0);
  stip_structure_traversal_override_by_function(&st,
                                                slice_function_conditional_pipe,
                                                &stip_traverse_structure_pipe);
  stip_structure_traversal_override(&st,
                                    continuation_solver_inserters,
                                    nr_continuation_solver_inserters);
  stip_traverse_structure(si,&st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

