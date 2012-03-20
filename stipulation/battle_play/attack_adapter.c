#include "stipulation/battle_play/attack_adapter.h"
#include "stipulation/branch.h"
#include "stipulation/battle_play/branch.h"
#include "stipulation/battle_play/defense_adapter.h"
#include "pypipe.h"
#include "trace.h"

#include <assert.h>

/* Allocate a STAttackAdapter slice.
 * @param length maximum number of half-moves of slice (+ slack)
 * @param min_length minimum number of half-moves of slice (+ slack)
 * @return index of allocated slice
 */
slice_index alloc_attack_adapter_slice(stip_length_type length,
                                       stip_length_type min_length)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",length);
  TraceFunctionParam("%u",min_length);
  TraceFunctionParamListEnd();

  result = alloc_branch(STAttackAdapter,length,min_length);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Wrap the slices representing the initial moves of the solution with
 * slices of appropriately equipped slice types
 * @param adapter identifies attack adapter slice
 * @param st address of structure holding the traversal state
 */
void attack_adapter_make_root(slice_index adapter,
                              stip_structure_traversal *st)
{
  spin_off_state_type * const state = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",adapter);
  TraceFunctionParamListEnd();

  battle_make_root(adapter,state);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Wrap the slices representing the nested slices
 * @param adapter identifies attack adapter slice
 * @param st address of structure holding the traversal state
 */
void attack_adapter_make_intro(slice_index adapter,
                               stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",adapter);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children(adapter,st);

  if (st->level==structure_traversal_level_nested
      && slices[adapter].u.branch.length>slack_length)
  {
    spin_off_state_type * const state = st->param;
    battle_spin_off_intro(adapter,state);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Traverse a subtree
 * @param si root slice of subtree
 * @param st address of structure defining traversal
 */
void stip_traverse_structure_attack_adpater(slice_index si,
                                            stip_structure_traversal *st)
{
  structure_traversal_level_type const save_level = st->level;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  assert(st->context==stip_traversal_context_global);

  st->context = stip_traversal_context_attack;
  st->level = structure_traversal_level_nested;
  stip_traverse_structure_pipe(si,st);
  st->level = save_level;
  st->context = stip_traversal_context_global;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Traversal of the moves of some adapter slice
 * @param si identifies root of subtree
 * @param st address of structure representing traversal
 */
void stip_traverse_moves_attack_adapter(slice_index si,
                                        stip_moves_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (st->context==stip_traversal_context_global)
  {
    assert(st->remaining==STIP_MOVES_TRAVERSAL_LENGTH_UNINITIALISED);
    assert(st->full_length==STIP_MOVES_TRAVERSAL_LENGTH_UNINITIALISED);
    st->full_length = slices[si].u.branch.length-slack_length;
    TraceValue("->%u\n",st->full_length);
    st->remaining = st->full_length;
    st->context = stip_traversal_context_attack;

    stip_traverse_moves_pipe(si,st);

    st->context = stip_traversal_context_global;
    st->remaining = STIP_MOVES_TRAVERSAL_LENGTH_UNINITIALISED;
    st->full_length = STIP_MOVES_TRAVERSAL_LENGTH_UNINITIALISED;
  }
  else
    stip_traverse_moves_pipe(si,st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Attempt to add set play to an attack stipulation (battle play, not
 * postkey only)
 * @param adapter identifies attack adapter slice
 * @param st address of structure representing traversal
 */
void attack_adapter_apply_setplay(slice_index adapter, stip_structure_traversal *st)
{
  spin_off_state_type * const state = st->param;
  stip_length_type const length = slices[adapter].u.branch.length;
  stip_length_type const min_length = slices[adapter].u.branch.min_length;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",adapter);
  TraceFunctionParamListEnd();

  if (length>slack_length)
  {
    slice_index zigzag = branch_find_slice(STCheckZigzagJump,adapter);
    if (zigzag==no_slice)
    {
      battle_branch_make_setplay(adapter,state);
      if (state->spun_off[adapter]!=no_slice)
      {
        slice_index const nested = state->spun_off[adapter];
        state->spun_off[adapter] = alloc_defense_adapter_slice(length-1,min_length-1);
        link_to_branch(state->spun_off[adapter],nested);
      }
    }
    else
    {
      /* set play of some pser stipulation */
      slice_index const proto = alloc_defense_adapter_slice(slack_length,
                                                            slack_length);
      attack_branch_insert_slices(adapter,&proto,1);

      {
        slice_index const defense_adapter = branch_find_slice(STDefenseAdapter,
                                                              adapter);
        assert(defense_adapter!=no_slice);
        battle_branch_make_root_slices(defense_adapter,state);
        assert(state->spun_off[defense_adapter]!=no_slice);
        state->spun_off[adapter] = state->spun_off[defense_adapter];
        pipe_remove(defense_adapter);
      }
    }
  }

  TraceValue("%u\n",state->spun_off[adapter]);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Try to solve in n half-moves after a defense.
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @return length of solution found and written, i.e.:
 *            slack_length-2 defense has turned out to be illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type attack_adapter_attack(slice_index si, stip_length_type n)
{
  stip_length_type result;
  slice_index const next = slices[si].u.pipe.next;
  stip_length_type const length = slices[si].u.branch.length;
  stip_length_type nr_moves_needed;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  nr_moves_needed = attack(next,length);
  if (nr_moves_needed<slack_length)
    result = slack_length-2;
  else if (nr_moves_needed<=length)
    result = n;
  else
    result = n+2;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Traverse a subtree
 * @param si root slice of subtree
 * @param st address of structure defining traversal
 */
void stip_traverse_structure_ready_for_attack(slice_index si,
                                              stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  assert(st->context==stip_traversal_context_attack);

  stip_traverse_structure_pipe(si,st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Traversal of the moves of some adapter slice
 * @param si identifies root of subtree
 * @param st address of structure representing traversal
 */
void stip_traverse_moves_ready_for_attack(slice_index si,
                                          stip_moves_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  assert(st->context==stip_traversal_context_attack);

  stip_traverse_moves_pipe(si,st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
