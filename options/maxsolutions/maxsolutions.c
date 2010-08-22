#include "optimisations/maxsolutions/maxsolutions.h"
#include "pypipe.h"
#include "optimisations/maxsolutions/root_solvable_filter.h"
#include "optimisations/maxsolutions/solvable_filter.h"
#include "optimisations/maxsolutions/root_defender_filter.h"
#include "optimisations/maxsolutions/help_filter.h"
#include "optimisations/maxsolutions/series_filter.h"
#include "trace.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

/* number of solutions found in the current phase */
static unsigned int nr_solutions_found_in_phase;

/* maximum number of allowed solutions found in the current phase */
static unsigned int max_nr_solutions_per_phase;

/* has the maximum number of allowed solutions been reached? */
static boolean allowed_nr_solutions_reached;

/* Reset the value of the maxsolutions option
 */
void reset_max_solutions(void)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  max_nr_solutions_per_phase = UINT_MAX;
  allowed_nr_solutions_reached = false;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Read the value of the maxsolutions option
 * @return true iff the value could be successfully read
 */
boolean read_max_solutions(char const *token)
{
  boolean result;
  char *end;
  unsigned long const value = strtoul(token,&end,10);

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%s",token);
  TraceFunctionParamListEnd();

  if (*end==0 && value<UINT_MAX)
  {
    max_nr_solutions_per_phase = (unsigned int)value;
    result = true;
  }
  else
    result = false;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Have we found the maxmimum allowed number of solutions since the
 * last invokation of reset_max_solutions()/read_max_solutions()?
 * @true iff we have found the maxmimum allowed number of solutions
 */
boolean max_solutions_reached(void)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",allowed_nr_solutions_reached);
  TraceFunctionResultEnd();
  return allowed_nr_solutions_reached;
}

/* Reset the number of found solutions
 */
void reset_nr_found_solutions_per_phase(void)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  nr_solutions_found_in_phase = 0;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Increase the number of found solutions by 1
 */
void increase_nr_found_solutions(void)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  ++nr_solutions_found_in_phase;
  TraceValue("->%u\n",nr_solutions_found_in_phase);

  if (max_nr_solutions_found_in_phase())
    allowed_nr_solutions_reached = true;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Have we found the maximum allowed number of solutions since the
 * last invokation of reset_nr_found_solutions()? 
 * @return true iff the allowed maximum number of solutions have been found
 */
boolean max_nr_solutions_found_in_phase(void)
{
  boolean result;

  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  result = nr_solutions_found_in_phase>=max_nr_solutions_per_phase;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}


/* Insert a STMaxSolutionsHelpFilter slice after a STHelpMove slice
 */
static void insert_maxsolutions_help_filter(slice_index si,
                                            stip_structure_traversal *st)
{
  slice_index const next = slices[si].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children(si,st);

  if (slices[next].prev==si)
    /* we are part of a loop
     */
    pipe_append(si,alloc_maxsolutions_help_filter());
  else
  {
    slice_index const next_pred = slices[next].prev;
    if (slices[next_pred].type==STMaxSolutionsHelpFilter)
      /* we are attached to a loop; a STMaxSolutionsHelpFilter slice
       * has been inserted in the loop before next; attach to it
       */
      pipe_set_successor(si,next_pred);
    else
      /* we are attached to something else; e.g. the help move is at
       * the beginning of set play in series self play
       */
      pipe_append(si,alloc_maxsolutions_help_filter());
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Insert a STMaxSolutionsSeriesFilter slice after a STSeriesMove slice
 */
static void insert_maxsolutions_series_filter(slice_index si,
                                              stip_structure_traversal *st)
{
  slice_index const next = slices[si].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children(si,st);

  if (slices[next].prev==si)
    /* we are part of a loop
     */
    pipe_append(si,alloc_maxsolutions_series_filter());
  else
  {
    /* we are attached to a loop
     */
    slice_index const next_pred = slices[next].prev;
    assert(slices[next_pred].type==STMaxSolutionsSeriesFilter);

    /* a STMaxSolutionsSeriesFilter slice has been inserted in the
     * loop before next; attach to it
     */
    pipe_set_successor(si,next_pred);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Insert a STMaxSolutionsRootDefenderFilter slice after a STAttackRoot slice
 */
static
void insert_maxsolutions_root_defender_filter(slice_index si,
                                              stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  pipe_append(si,alloc_maxsolutions_root_defender_filter());

  /* don't recurse further; we don't want to instrument leaves */

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Insert a STMaxSolutionsSolvableFilter before a STLeaf slice
 */
static void insert_maxsolutions_solvable_filter(slice_index si,
                                                stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  pipe_append(slices[si].prev,alloc_maxsolutions_solvable_filter());

  /* don't recurse further; we don't want to instrument leaves */

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static structure_traversers_visitors maxsolutions_filter_inserters[] =
{
  { STHelpMove,         &insert_maxsolutions_help_filter          },
  { STHelpMoveToGoal,   &insert_maxsolutions_help_filter          },
  { STSeriesMove,       &insert_maxsolutions_series_filter        },
  { STSeriesMoveToGoal, &insert_maxsolutions_series_filter        },
  { STLeaf,             &insert_maxsolutions_solvable_filter      },
  { STAttackRoot,       &insert_maxsolutions_root_defender_filter },
  { STAttackMoveToGoal, &insert_maxsolutions_root_defender_filter }
};

enum
{
  nr_maxsolutions_filter_inserters =
  (sizeof maxsolutions_filter_inserters
   / sizeof maxsolutions_filter_inserters[0])
};

/* Insert a STMaxSolutionsRootSolvableFilter slice before some slice
 */
static void insert_root_solvable_filter(slice_index si,
                                        stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  pipe_append(slices[si].prev,alloc_maxsolutions_root_solvable_filter());

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static structure_traversers_visitors maxsolutions_initialiser_inserters[] =
{
  { STReadyForAttack,   &insert_root_solvable_filter },
  { STHelpRoot,         &insert_root_solvable_filter },
  { STSeriesRoot,       &insert_root_solvable_filter }
};

enum
{
  nr_maxsolutions_initialiser_inserters =
  (sizeof maxsolutions_initialiser_inserters
   / sizeof maxsolutions_initialiser_inserters[0])
};

/* Instrument a stipulation with STMaxSolutions*Filter slices
 * @param si identifies slice where to start
 */
void stip_insert_maxsolutions_filters(slice_index si)
{
  stip_structure_traversal st;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceStipulation(si);

  stip_structure_traversal_init(&st,0);
  stip_structure_traversal_override(&st,
                                    maxsolutions_filter_inserters,
                                    nr_maxsolutions_filter_inserters);
  stip_traverse_structure(si,&st);

  stip_structure_traversal_init(&st,0);
  stip_structure_traversal_override(&st,
                                    maxsolutions_initialiser_inserters,
                                    nr_maxsolutions_initialiser_inserters);
  stip_traverse_structure(si,&st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
