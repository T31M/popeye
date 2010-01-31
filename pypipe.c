#include "pypipe.h"
#include "stipulation/proxy.h"
#include "trace.h"

#include <assert.h>

/* Allocate a new pipe and make an existing pipe its successor
 * @param type which slice type
 * @return newly allocated slice
 */
slice_index alloc_pipe(SliceType type)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceEnumerator(SliceType,type,"");
  TraceFunctionParamListEnd();

  result = alloc_slice(type);
  slices[result].u.pipe.next = no_slice;
  slices[result].prev = no_slice;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

#if !defined(NDEBUG)
/* Does a slice have a successor
 * @param si identifies slice
 * @return true iff si identifies a slice that has a .next member
 */
static boolean has_successor(slice_index si)
{
  return (slices[si].type!=STLeafDirect
          && slices[si].type!=STLeafHelp
          && slices[si].type!=STLeafForced
          && slices[si].type!=STQuodlibet
          && slices[si].type!=STReciprocal);
}
#endif

/* Make a slice the successor of a pipe
 * @param pipe identifies the pipe
 * @param succ slice to be made the successor of pipe
 */
void pipe_set_successor(slice_index pipe, slice_index succ)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",pipe);
  TraceFunctionParam("%u",succ);
  TraceFunctionParamListEnd();

  assert(has_successor(pipe));
  slices[pipe].u.pipe.next = succ;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Establish a link between a branch slice and its successor
 * @param branch identifies branch slice
 * @param succ identifies branch to become the successor
 */
void pipe_link(slice_index branch, slice_index succ)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",branch);
  TraceFunctionParam("%u",succ);
  TraceFunctionParamListEnd();

  pipe_set_successor(branch,succ);
  slice_set_predecessor(succ,branch);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Detect starter field with the starting side if possible.
 * @param pipe identifies slice being traversed
 * @param st status of traversal
 * @return true iff slice has been successfully traversed
 */
boolean pipe_detect_starter(slice_index pipe, slice_traversal *st)
{
  boolean result;
  slice_index const next = slices[pipe].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",pipe);
  TraceFunctionParamListEnd();

  if (slices[pipe].starter==no_side)
  {
    result = traverse_slices(slices[pipe].u.pipe.next,st);
    slices[pipe].starter = slices[next].starter;
  }
  else
    result = true;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Impose the starting side on a stipulation
 * @param pipe identifies branch
 * @param st address of structure that holds the state of the traversal
 * @return true iff the operation is successful in the subtree of
 *         which si is the root
 */
boolean pipe_impose_starter(slice_index pipe, slice_traversal *st)
{
  boolean const result = true;
  Side const * const starter = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",pipe);
  TraceFunctionParam("%u",*starter);
  TraceFunctionParamListEnd();

  slices[pipe].starter = *starter;
  traverse_slices(slices[pipe].u.pipe.next,st);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Impose the starting side on a stipulation. Impose the inverted
 * starter on the slice's successor. 
 * @param pipe identifies branch
 * @param st address of structure that holds the state of the traversal
 * @return true iff the operation is successful in the subtree of
 *         which si is the root
 */
boolean pipe_impose_inverted_starter(slice_index pipe, slice_traversal *st)
{
  boolean const result = true;
  Side * const starter = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",pipe);
  TraceFunctionParam("%u",*starter);
  TraceFunctionParamListEnd();

  slices[pipe].starter = *starter;

  *starter = advers(*starter);
  traverse_slices(slices[pipe].u.pipe.next,st);
  *starter = slices[pipe].starter;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Traverse the sub-graph starting at the successor slice of a pipe
 * slice (but don't traverse possible other children of the pipe
 * slice)
 * @param pipe identifies pipe slice
 * @return true iff the sub-graph has been successfully traversed
 */
boolean pipe_traverse_next(slice_index pipe, slice_traversal *st)
{
  boolean const result = true;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",pipe);
  TraceFunctionParamListEnd();

  traverse_slices(slices[pipe].u.pipe.next,st);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Substitute links to proxy slices by the proxy's target
 * @param si root of sub-tree where to resolve proxies
 * @param st address of structure representing the traversal
 * @return true iff slice si has been successfully traversed
 */
boolean pipe_resolve_proxies(slice_index si, slice_traversal *st)
{
  boolean const result = true;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  slice_traverse_children(si,st);
  if (slices[si].u.pipe.next!=no_slice)
    proxy_slice_resolve(&slices[si].u.pipe.next);
  
  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether a slice has a solution
 * @param pipe slice index
 * @return whether there is a solution and (to some extent) why not
 */
has_solution_type pipe_has_solution(slice_index pipe)
{
  has_solution_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",pipe);
  TraceFunctionParamListEnd();

  result = slice_has_solution(slices[pipe].u.pipe.next);

  TraceFunctionExit(__func__);
  TraceEnumerator(has_solution_type,result,"");
  TraceFunctionResultEnd();
  return result;
}
