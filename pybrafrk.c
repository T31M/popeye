#include "pybrafrk.h"
#include "pydirect.h"
#include "pyhelp.h"
#include "pyseries.h"
#include "pydata.h"
#include "trace.h"

#include <assert.h>

/* **************** Initialisation ***************
 */

/* Allocate a STBranchFork slice.
 * @param next identifies next slice
 * @param towards_goal identifies slice leading towards goal
 * @return index of allocated slice
 */
slice_index alloc_branch_fork_slice(stip_length_type length,
                                    stip_length_type min_length,
                                    slice_index next,
                                    slice_index towards_goal)
{
  slice_index const result = alloc_slice_index();

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",length);
  TraceFunctionParam("%u",min_length);
  TraceFunctionParam("%u",next);
  TraceFunctionParam("%u",towards_goal);
  TraceFunctionParamListEnd();

  slices[result].type = STBranchFork;
  slices[result].starter = slices[towards_goal].starter;
  slices[result].u.pipe.next = next;
  slices[result].u.pipe.u.branch.length = length;
  slices[result].u.pipe.u.branch.min_length = min_length;
  slices[result].u.pipe.u.branch.towards_goal = towards_goal;
  
  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* **************** Implementation of interface Help ***************
 */

/* Solve in a number of half-moves
 * @param si identifies slice
 * @param n exact number of half moves until end state has to be reached
 * @return true iff >=1 solution was found
 */
boolean branch_fork_help_solve_in_n(slice_index si, stip_length_type n)
{
  boolean result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  assert(n>=slack_length_help);

  if (n==slack_length_help)
    result = slice_solve(slices[si].u.pipe.u.branch.towards_goal);
  else
    result = help_solve_in_n(slices[si].u.pipe.next,n);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether there is a solution in n half moves.
 * @param si slice index of slice being solved
 * @param n exact number of half moves until end state has to be reached
 * @return true iff >= 1 solution has been found
 */
boolean branch_fork_help_has_solution_in_n(slice_index si, stip_length_type n)
{
  boolean result;
  slice_index const next = slices[si].u.pipe.next;
  slice_index const to_goal = slices[si].u.pipe.u.branch.towards_goal;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  assert(n>=slack_length_help);

  if (n==slack_length_help)
    result = slice_has_solution(to_goal)==has_solution;
  else
    result = help_has_solution_in_n(next,n);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine and write solution(s): add first moves to table (as
 * threats for the parent slice. First consult hash table.
 * @param continuations table where to add first moves
 * @param si slice index of slice being solved
 * @param n exact number of half moves until end state has to be reached
 */
void branch_fork_help_solve_continuations_in_n(table continuations,
                                               slice_index si,
                                               stip_length_type n)
{
  slice_index const next = slices[si].u.pipe.next;
  slice_index const to_goal = slices[si].u.pipe.u.branch.towards_goal;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  assert(n>=slack_length_help);

  if (n==slack_length_help)
    slice_solve_threats(continuations,to_goal);
  else
    help_solve_continuations_in_n(continuations,next,n);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* **************** Implementation of interface Series ***************
 */

/* Solve in a number of half-moves
 * @param si identifies slice
 * @param n exact number of half moves until end state has to be reached
 * @return true iff >=1 solution was found
 */
boolean branch_fork_series_solve_in_n(slice_index si, stip_length_type n)
{
  boolean result;
  slice_index const next = slices[si].u.pipe.next;
  slice_index const to_goal = slices[si].u.pipe.u.branch.towards_goal;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  assert(n>=slack_length_series);

  if (n==slack_length_series)
    result = slice_solve(to_goal);
  else
    result = series_solve_in_n(next,n);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether there is a solution in n half moves.
 * @param si slice index of slice being solved
 * @param n exact number of half moves until end state has to be reached
 * @return true iff >= 1 solution has been found
 */
boolean branch_fork_series_has_solution_in_n(slice_index si,
                                             stip_length_type n)
{
  boolean result;
  slice_index const next = slices[si].u.pipe.next;
  slice_index const to_goal = slices[si].u.pipe.u.branch.towards_goal;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  assert(n>=slack_length_series);

  if (n==slack_length_series)
    result = slice_has_solution(to_goal);
  else
    result = series_has_solution_in_n(next,n);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine and write threats
 * @param threats table where to add first moves
 * @param si slice index of slice being solved
 * @param n exact number of half moves until end state has to be reached
 */
void branch_fork_series_solve_threats_in_n(table threats,
                                           slice_index si,
                                           stip_length_type n)
{
  slice_index const next = slices[si].u.pipe.next;
  slice_index const to_goal = slices[si].u.pipe.u.branch.towards_goal;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  assert(n>=slack_length_series);

  if (n==slack_length_series)
    slice_solve_threats(threats,to_goal);
  else
    series_solve_threats_in_n(threats,next,n);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* **************** Implementation of interface Slice ***************
 */

/* Determine whether a slice has a solution
 * @param si slice index
 * @return whether there is a solution and (to some extent) why not
 */
has_solution_type branch_fork_has_solution(slice_index si)
{
  has_solution_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  result = slice_has_solution(slices[si].u.pipe.u.branch.towards_goal);

  TraceFunctionExit(__func__);
  TraceEnumerator(has_solution_type,result,"");
  TraceFunctionResultEnd();
  return result;
}

/* Detect starter field with the starting side if possible. 
 * @param si identifies slice
 * @param same_side_as_root does si start with the same side as root?
 * @return does the leaf decide on the starter?
 */
who_decides_on_starter branch_fork_detect_starter(slice_index si,
                                                  boolean same_side_as_root)
{
  slice_index const towards_goal = slices[si].u.pipe.u.branch.towards_goal;
  who_decides_on_starter result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",same_side_as_root);
  TraceFunctionParamListEnd();

  result = slice_detect_starter(towards_goal,same_side_as_root);
  slices[si].starter = slices[towards_goal].starter;
  TraceValue("%u\n",slices[si].starter);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}


/* Solve a slice at root level
 * @param si slice index
 * @return true iff >=1 solution was found
 */
boolean branch_fork_root_solve(slice_index si)
{
  boolean result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  result = slice_root_solve(slices[si].u.pipe.u.branch.towards_goal);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Impose the starting side on a stipulation
 * @param si identifies branch
 * @param st address of structure that holds the state of the traversal
 * @return true iff the operation is successful in the subtree of
 *         which si is the root
 */
boolean branch_fork_impose_starter(slice_index si, slice_traversal *st)
{
  boolean const result = true;
  Side * const starter = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",*starter);
  TraceFunctionParamListEnd();

  slices[si].starter = *starter;
  slice_traverse_children(si,st);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* **************** Branch utilities ***************
 */

/* Continue deallocating a branch
 * @param si identifies branch_fork slice
 * @param st structure representing the traversal
 * @return true iff si and its children have been successfully
 *         deallocated
 */
static boolean traverse_and_deallocate(slice_index si, slice_traversal *st)
{
  boolean const result = slice_traverse_children(si,st);
  dealloc_slice_index(si);
  return result;
}

/* Store slice representing play after branch in object representing
 * traversal, then continue deallocating the branch
 * @param si identifies branch_fork slice
 * @param st structure representing the traversal
 */
static boolean traverse_and_deallocate_branch_fork(slice_index si,
                                                   slice_traversal *st)
{
  boolean result;
  slice_index * const to_be_found = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  *to_be_found = slices[si].u.pipe.u.branch.towards_goal;

  result = traverse_slices(slices[si].u.pipe.next,st);
  dealloc_slice_index(si);
  
  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static boolean traverse_and_deallocate_leaf(slice_index si,
                                            slice_traversal *st)
{
  boolean const result = true;
  slice_index * const to_be_found = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  *to_be_found = si;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static slice_operation const slice_to_fork_deallocators[] =
{
  &traverse_and_deallocate,             /* STBranchDirect */
  &traverse_and_deallocate,             /* STBranchDirectDefender */
  &traverse_and_deallocate,             /* STBranchHelp */
  &traverse_and_deallocate,             /* STBranchSeries */
  &traverse_and_deallocate_branch_fork, /* STBranchFork */
  &traverse_and_deallocate_leaf,        /* STLeafDirect */
  &traverse_and_deallocate_leaf,        /* STLeafHelp */
  &traverse_and_deallocate_leaf,        /* STLeafForced */
  0,                                    /* STReciprocal */
  0,                                    /* STQuodlibet */
  0,                                    /* STNot */
  0,                                    /* STMoveInverter */
  &traverse_and_deallocate,             /* STDirectRoot */
  &traverse_and_deallocate,             /* STDirectDefenderRoot */
  &traverse_and_deallocate,             /* STDirectHashed */
  &traverse_and_deallocate,             /* STHelpRoot */
  &traverse_and_deallocate,             /* STHelpAdapter */
  &traverse_and_deallocate,             /* STHelpHashed */
  &traverse_and_deallocate,             /* STSeriesRoot */
  &traverse_and_deallocate,             /* STSeriesAdapter */
  &traverse_and_deallocate,             /* STSeriesHashed */
  &traverse_and_deallocate,             /* STSelfCheckGuard */
  &traverse_and_deallocate,             /* STDirectDefense */
  0,                                    /* STReflexGuard */
  &traverse_and_deallocate,             /* STSelfAttack */
  &traverse_and_deallocate,             /* STSelfDefense */
  0,                                    /* STRestartGuard */
  0,                                    /* STGoalReachableGuard */
  &traverse_and_deallocate,             /* STKeepMatingGuard */
  &traverse_and_deallocate,             /* STMaxFlightsquares */
  &traverse_and_deallocate,             /* STDegenerateTree */
  &traverse_and_deallocate,             /* STMaxNrNonTrivial */
  &traverse_and_deallocate              /* STMaxThreatLength */
};

/* Deallocate a branch
 * @param branch identifies branch
 * @return index of slice representing the play after the branch
 */
slice_index branch_deallocate_to_fork(slice_index branch)
{
  slice_index result;
  slice_traversal st;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",branch);
  TraceFunctionParamListEnd();

  slice_traversal_init(&st,&slice_to_fork_deallocators,&result);
  traverse_slices(branch,&st);
  
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

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  do
  {
    TraceValue("%u\n",slices[result].u.pipe.next);
    result = slices[result].u.pipe.next;
  } while (result!=no_slice
           && result!=si
           && slices[result].type!=type);
  
  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}
