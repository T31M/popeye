#include "stipulation/branch.h"
#include "stipulation/proxy.h"
#include "pypipe.h"
#include "trace.h"

#include <assert.h>

/* Order in which the slice types at the root appear
 */
static slice_index const root_slice_rank_order[] =
{
  STProxy,
  STSetplayFork,
  STMoveInverterRootSolvableFilter,
  STMoveInverterSolvableFilter,
  STReflexAttackerFilter,
  STReflexDefenderFilter,
  STReadyForAttack,
  STReadyForDefense,
  STReadyForHelpMove,
  STReadyForSeriesMove,
  STAmuMateFilter,
  STUltraschachzwangGoalFilter,
  STCirceSteingewinnFilter,
  STSelfCheckGuard,
  STAttackMoveLegalityChecked,
  STDefenseMoveLegalityChecked,
  STHelpMoveLegalityChecked,
  STSeriesMoveLegalityChecked
};

enum
{
  nr_root_slice_rank_order_elmts = (sizeof root_slice_rank_order
                                    / sizeof root_slice_rank_order[0])
};

/* Determine the rank of a slice type
 * @param type defense slice type
 * @return rank of type; nr_defense_slice_rank_order_elmts if the rank can't
 *         be determined
 */
static unsigned int get_root_slice_rank(SliceType type)
{
  unsigned int result;

  TraceFunctionEntry(__func__);
  TraceEnumerator(SliceType,type,"");
  TraceFunctionParamListEnd();

  for (result = 0; result!=nr_root_slice_rank_order_elmts; ++result)
    if (root_slice_rank_order[result]==type)
      break;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static void root_branch_insert_slices_recursive(slice_index si,
                                                slice_index const prototypes[],
                                                unsigned int nr_prototypes)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",nr_prototypes);
  TraceFunctionParamListEnd();

  {
    SliceType const prototype_type = slices[prototypes[0]].type;
    unsigned int prototype_rank = get_root_slice_rank(prototype_type);

    do
    {
      if (slices[si].type==STProxy)
        si = slices[si].u.pipe.next;
      else if (slices[si].type==STQuodlibet || slices[si].type==STReciprocal)
      {
        root_branch_insert_slices_recursive(slices[si].u.binary.op1,
                                            prototypes,nr_prototypes);
        root_branch_insert_slices_recursive(slices[si].u.binary.op2,
                                            prototypes,nr_prototypes);
        break;
      }
      else
      {
        unsigned int const rank_si = get_root_slice_rank(slices[si].type);
        if (rank_si==nr_root_slice_rank_order_elmts)
          break;
        else if (rank_si>prototype_rank)
        {
          pipe_append(slices[si].prev,copy_slice(prototypes[0]));
          if (nr_prototypes>1)
            root_branch_insert_slices_recursive(si,
                                                prototypes+1,nr_prototypes-1);
          break;
        }
        else
          si = slices[si].u.pipe.next;
      }
    } while (prototype_type!=slices[si].type);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Insert slices into a root branch.
 * The inserted slices are copies of the elements of prototypes; the elements of
 * prototypes are deallocated by root_branch_insert_slices().
 * Each slice is inserted at a position that corresponds to its predefined rank.
 * @param si identifies starting point of insertion
 * @param prototypes contains the prototypes whose copies are inserted
 * @param nr_prototypes number of elements of array prototypes
 */
void root_branch_insert_slices(slice_index si,
                               slice_index const prototypes[],
                               unsigned int nr_prototypes)
{
  unsigned int i;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",nr_prototypes);
  TraceFunctionParamListEnd();

  root_branch_insert_slices_recursive(si,prototypes,nr_prototypes);

  for (i = 0; i!=nr_prototypes; ++i)
    dealloc_slice(prototypes[i]);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Order in which the slice types dealing with goals appear
 */
static slice_index const leaf_slice_rank_order[] =
{
  STAmuMateFilter,
  STGoalMateReachedTester,
  STGoalStalemateReachedTester,
  STGoalDoubleStalemateReachedTester,
  STGoalTargetReachedTester,
  STGoalCaptureReachedTester,
  STGoalSteingewinnReachedTester,
  STGoalEnpassantReachedTester,
  STGoalDoubleMateReachedTester,
  STGoalCounterMateReachedTester,
  STGoalCastlingReachedTester,
  STGoalAutoStalemateReachedTester,
  STGoalCircuitReachedTester,
  STGoalExchangeReachedTester,
  STGoalCircuitByRebirthReachedTester,
  STGoalExchangeByRebirthReachedTester,
  STGoalAnyReachedTester,
  STGoalProofgameReachedTester,
  STGoalAToBReachedTester,
  STGoalMateOrStalemateReachedTester,
  STGoalCheckReachedTester,
  STSelfCheckGuard,
  STGoalNotCheckReachedTester,
  STGoalImmobileReachedTester,
  STGoalReachedTested,
  STDefenseMoveLegalityChecked,
  STHelpMoveLegalityChecked,
  STSeriesMoveLegalityChecked,
  STLeaf
};

enum
{
  nr_leaf_slice_rank_order_elmts = (sizeof leaf_slice_rank_order
                                    / sizeof leaf_slice_rank_order[0])
};

/* Determine the rank of a slice type
 * @param type defense slice type
 * @return rank of type; nr_defense_slice_rank_order_elmts if the rank can't
 *         be determined
 */
static unsigned int get_leaf_slice_rank(SliceType type)
{
  unsigned int result;

  TraceFunctionEntry(__func__);
  TraceEnumerator(SliceType,type,"");
  TraceFunctionParamListEnd();

  for (result = 0; result!=nr_leaf_slice_rank_order_elmts; ++result)
    if (leaf_slice_rank_order[result]==type)
      break;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static void leaf_branch_insert_slices_recursive(slice_index si,
                                                slice_index const prototypes[],
                                                unsigned int nr_prototypes)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",nr_prototypes);
  TraceFunctionParamListEnd();

  {
    SliceType const prototype_type = slices[prototypes[0]].type;
    unsigned int prototype_rank = get_leaf_slice_rank(prototype_type);

    do
    {
      if (slices[si].type==STProxy)
        si = slices[si].u.pipe.next;
      else if (slices[si].type==STQuodlibet || slices[si].type==STReciprocal)
      {
        leaf_branch_insert_slices_recursive(slices[si].u.binary.op1,
                                            prototypes,nr_prototypes);
        leaf_branch_insert_slices_recursive(slices[si].u.binary.op2,
                                            prototypes,nr_prototypes);
        break;
      }
      else
      {
        unsigned int const rank_si = get_leaf_slice_rank(slices[si].type);
        if (rank_si==nr_leaf_slice_rank_order_elmts)
          break;
        else if (rank_si>prototype_rank)
        {
          pipe_append(slices[si].prev,copy_slice(prototypes[0]));
          if (nr_prototypes>1)
            leaf_branch_insert_slices_recursive(si,
                                                prototypes+1,nr_prototypes-1);
          break;
        }
        else
          si = slices[si].u.pipe.next;
      }
    } while (prototype_type!=slices[si].type);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Insert slices into a leaf branch.
 * The inserted slices are copies of the elements of prototypes; the elements of
 * prototypes are deallocated by leaf_branch_insert_slices().
 * Each slice is inserted at a position that corresponds to its predefined rank.
 * @param si identifies starting point of insertion
 * @param prototypes contains the prototypes whose copies are inserted
 * @param nr_prototypes number of elements of array prototypes
 */
void leaf_branch_insert_slices(slice_index si,
                               slice_index const prototypes[],
                               unsigned int nr_prototypes)
{
  unsigned int i;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",nr_prototypes);
  TraceFunctionParamListEnd();

  leaf_branch_insert_slices_recursive(si,prototypes,nr_prototypes);

  for (i = 0; i!=nr_prototypes; ++i)
    dealloc_slice(prototypes[i]);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Allocate a new branch slice
 * @param type which slice type
 * @param length maximum number of half moves until end of slice
 * @param min_length minimum number of half moves until end of slice
 * @return newly allocated slice
 */
slice_index alloc_branch(SliceType type,
                         stip_length_type length,
                         stip_length_type min_length)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceEnumerator(SliceType,type,"");
  TraceFunctionParam("%u",length);
  TraceFunctionParam("%u",min_length);
  TraceFunctionParamListEnd();

  result = alloc_pipe(type);
  slices[result].u.branch.length = length;
  slices[result].u.branch.min_length = min_length;
  slices[result].u.branch.imminent_goal.type = no_goal;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Find the next slice with a specific type in a branch
 * @param type type of slice to be found
 * @param si identifies the slice where to start searching
 * @return identifier for slice with type type; no_slice if none is found
 */
slice_index branch_find_slice(SliceType type, slice_index si)
{
  slice_index result = si;
  boolean slices_visited[max_nr_slices] = { false };

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  do
  {
    TraceValue("%u\n",slices[result].u.pipe.next);
    result = slices[result].u.pipe.next;
    if (result==no_slice || slices[result].type==type)
      break;
    else if (slices_visited[result]
             || !slice_has_structure(result,slice_structure_pipe))
    {
      result = no_slice;
      break;
    }
    else
      slices_visited[result] = true;
  } while (true);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Initialise the full_length and remaing fields of a
 * stip_moves_traversal struct from a branch slice if necessary
 * @param si identifies the branch slice
 * @param st refers to the struct to be initialised
 */
void stip_traverse_moves_branch_init_full_length(slice_index si,
                                                 stip_moves_traversal *st)
{
  if (st->remaining==0)
  {
    st->full_length = slices[si].u.branch.length;
    TraceValue("->%u\n",st->full_length);
    st->remaining = slices[si].u.branch.length;
  }
}

/* Traversal of the moves of some branch slice
 * @param si identifies root of subtree
 * @param st address of structure representing traversal
 */
void stip_traverse_moves_branch_slice(slice_index si, stip_moves_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_moves_branch_init_full_length(si,st);

  stip_traverse_moves_pipe(si,st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Traversal of the moves of some branch slice
 * @param si identifies root of subtree
 * @param st address of structure representing traversal
 */
void stip_traverse_moves_move_slice(slice_index si, stip_moves_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_moves_branch_init_full_length(si,st);

  --st->remaining;
  TraceValue("->%u\n",st->remaining);
  stip_traverse_moves_pipe(si,st);
  ++st->remaining;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Traversal of the moves of a branch
 * @param si identifies root of subtree
 * @param st address of structure representing traversal
 */
void stip_traverse_moves_branch(slice_index si, stip_moves_traversal *st)
{
  stip_length_type const save_remaining = st->remaining;
  stip_length_type const save_full_length = st->full_length;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  ++st->level;
  st->remaining = 0;

  stip_traverse_moves(si,st);

  st->full_length = save_full_length;
  st->remaining = save_remaining;
  TraceFunctionParam("->%u\n",st->remaining);
  --st->level;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Link a pipe slice to the entry slice of a branch
 * @param pipe identifies the pipe slice
 * @param entry identifies the entry slice of the branch
 */
void link_to_branch(slice_index pipe, slice_index entry)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",pipe);
  TraceFunctionParam("%u",entry);
  TraceFunctionParamListEnd();

  if (slices[entry].prev==no_slice)
    pipe_link(pipe,entry);
  else
    pipe_set_successor(pipe,entry);


  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
