#include "solving/observation.h"
#include "conditions/monochrome.h"
#include "conditions/bichrome.h"
#include "conditions/grid.h"
#include "conditions/edgemover.h"
#include "conditions/annan.h"
#include "conditions/phantom.h"
#include "conditions/marscirce/plus.h"
#include "conditions/marscirce/marscirce.h"
#include "conditions/singlebox/type3.h"
#include "conditions/sting.h"
#include "conditions/transmuting_kings/transmuting_kings.h"
#include "conditions/transmuting_kings/reflective_kings.h"
#include "conditions/vaulting_kings.h"
#include "solving/find_square_observer_tracking_back_from_target.h"
#include "solving/single_move_generator.h"
#include "stipulation/has_solution_type.h"
#include "stipulation/stipulation.h"
#include "stipulation/pipe.h"
#include "stipulation/branch.h"
#include "stipulation/temporary_hacks.h"
#include "optimisations/orthodox_square_observation.h"
#include "debugging/trace.h"
#include "pydata.h"

#include <assert.h>

boolean (*next_observation_validator)(square sq_observer, square sq_landing, square sq_observee) = &validate_observation;

vec_index_type interceptable_observation_vector_index[maxply+1];
unsigned int observation_context = 0;

enum
{
  observation_validators_capacity = 10
};

static void insert_slice(slice_index testing,
                         slice_type type,
                         slice_index const rank_order[],
                         unsigned int nr_rank_order)
{
  slice_index const prototype = alloc_pipe(type);
  stip_structure_traversal st;
  branch_slice_insertion_state_type state =
  {
    &prototype,1,
    rank_order, nr_rank_order,
    branch_slice_rank_order_nonrecursive,
    0,
    testing,
    0
  };

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",testing);
  TraceEnumerator(slice_type,type,"");
  TraceFunctionParamListEnd();

  assert(slices[testing].type==rank_order[0]);

  state.base_rank = get_slice_rank(slices[testing].type,&state);
  assert(state.base_rank!=no_slice_rank);
  init_slice_insertion_traversal(&st,&state,stip_traversal_context_intro);
  stip_traverse_structure_children_pipe(testing,&st);
  dealloc_slice(prototype);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

boolean validate_observation_geometry_recursive(slice_index si,
                                                square sq_observer,
                                                square sq_landing,
                                                square sq_observee)
{
  boolean result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceSquare(sq_target);
  TraceFunctionParamListEnd();

  TraceEnumerator(slice_type,slices[si].type,"\n");

  switch (slices[si].type)
  {
    case STTestObservationGeometryMonochrome:
      result = monochrome_validate_observation_geometry(si,
                                                        sq_observer,
                                                        sq_landing,
                                                        sq_observee);
      break;

    case STTestObservationGeometryBichrome:
      result = bichrome_validate_observation_geometry(si,
                                                      sq_observer,
                                                      sq_landing,
                                                      sq_observee);
      break;

    case STTestObservationGeometryGridChess:
      result = grid_validate_observation_geometry(si,
                                                  sq_observer,
                                                  sq_landing,
                                                  sq_observee);
      break;

    case STTestObservationGeometryEdgeMover:
      result = edgemover_validate_observation_geometry(si,
                                                       sq_observer,
                                                       sq_landing,
                                                       sq_observee);
      break;

    case STTestObservationGeometryByPlayingMove:
    {
      init_single_move_generator(sq_observer,sq_landing,sq_observee);
      if (solve(slices[temporary_hack_king_capture_legality_tester[trait[nbply]]].next2,
                length_unspecified)
          ==next_move_has_solution)
        result = validate_observation_geometry_recursive(slices[si].next1,
                                                         sq_observer,
                                                         sq_landing,
                                                         sq_observee);
      else
        result = false;
      break;
    }

    case STTrue:
      result = true;
      break;

    case STFalse:
      result = false;
      break;

    default:
      assert(0);
      break;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Validate an observation
 * @param sq_observer position of the observer
 * @param sq_landing landing square of the observer (normally==sq_observee)
 * @param sq_observee position of the piece to be observed
 * @return true iff the observation is valid
 */
boolean validate_observation_geometry(square sq_observer,
                                      square sq_landing,
                                      square sq_observee)
{
  boolean result;

  TraceFunctionEntry(__func__);
  TraceSquare(sq_observer);
  TraceSquare(sq_landing);
  TraceSquare(sq_observee);
  TraceFunctionParamListEnd();

  result = validate_observation_geometry_recursive(slices[temporary_hack_observation_geometry_tester[trait[nbply]]].next2,
                                                   sq_observer,
                                                   sq_landing,
                                                   sq_observee);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static slice_index const observation_geometry_slice_rank_order[] =
{
    STTestingObservationGeometry,
    STTestObservationGeometryMonochrome,
    STTestObservationGeometryBichrome,
    STTestObservationGeometryGridChess,
    STTestObservationGeometryEdgeMover,
    STTestObservationGeometryByPlayingMove,
    STTrue
};

enum
{
  nr_observation_geometry_slice_rank_order_elmts = sizeof observation_geometry_slice_rank_order / sizeof observation_geometry_slice_rank_order[0]
};

static void observation_geometry_testing_insert_slice(slice_index testing,
                                                      slice_type type)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",testing);
  TraceEnumerator(slice_type,type,"");
  TraceFunctionParamListEnd();

  insert_slice(testing,
               type,
               observation_geometry_slice_rank_order,
               nr_observation_geometry_slice_rank_order_elmts);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

typedef struct
{
    Side side;
    slice_type type;
} observation_geometry_instrumentation_type;

static void instrument_observation_geometry_testing(slice_index si,
                                                    stip_structure_traversal *st)
{
  observation_geometry_instrumentation_type const * const it = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children_pipe(si,st);

  if (it->side==nr_sides || it->side==slices[si].starter)
    observation_geometry_testing_insert_slice(si,it->type);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Instrument square observation testing with a slice type
 * @param identifies where to start instrumentation
 * @param side for which side (pass nr_sides to indicate both sides)
 * @param type type of slice with which to instrument moves
 */
void stip_instrument_observation_geometry_testing(slice_index si,
                                                  Side side,
                                                  slice_type type)
{
  observation_geometry_instrumentation_type it = { side, type };
  stip_structure_traversal st;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceEnumerator(Side,side,"");
  TraceEnumerator(slice_type,type,"");
  TraceFunctionParamListEnd();

  stip_structure_traversal_init(&st,&it);
  stip_structure_traversal_override_single(&st,
                                           STTestingObservationGeometry,
                                           &instrument_observation_geometry_testing);
  stip_traverse_structure(si,&st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static evalfunction_t *observer_validators[observation_validators_capacity];
static unsigned int nr_observer_validators;

/* Forget about the observer validators registered in a previous round of
 * solving.
 */
void reset_observer_validators(void)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  nr_observer_validators = 0;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Register an observer validator for the next round of solving
 * @param validator validator to be registered
 */
void register_observer_validator(evalfunction_t *validator)
{
  assert(nr_observer_validators<observation_validators_capacity);
  observer_validators[nr_observer_validators] = validator;
  ++nr_observer_validators;
}

/* Validate an observation
 * @param sq_observer position of the observer
 * @param sq_landing landing square of the observer (normally==sq_observee)
 * @param sq_observee position of the piece to be observed
 * @return true iff the observation is valid
 */
boolean validate_observer(square sq_observer,
                          square sq_landing,
                          square sq_observee)
{
  boolean result = true;
  unsigned int i;

  TraceFunctionEntry(__func__);
  TraceSquare(sq_observer);
  TraceSquare(sq_landing);
  TraceSquare(sq_observee);
  TraceFunctionParamListEnd();

  TraceValue("%u\n",nr_observer_validators);

  next_observation_validator = &validate_observation_geometry;

  for (i = 0; i!=nr_observer_validators; ++i)
    if (!(*observer_validators[i])(sq_observer,sq_landing,sq_observee))
    {
      result = false;
      break;
    }

  next_observation_validator = &validate_observer;

  if (result)
    result = validate_observation_geometry(sq_observer,sq_landing,sq_observee);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}


static evalfunction_t *observation_validators[observation_validators_capacity];
static unsigned int nr_observation_validators;

/* Forget about the observation validators registered in a previous round of
 * solving.
 */
void reset_observation_validators(void)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  nr_observation_validators = 0;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Register an observation validator for the next round of solving
 * @param validator validator to be registered
 */
void register_observation_validator(evalfunction_t *validator)
{
  assert(nr_observation_validators<observation_validators_capacity);
  observation_validators[nr_observation_validators] = validator;
  ++nr_observation_validators;
}

/* Retrieve the number of observation validators registered since program start
 * or the last invokation of reset_observation_validators()
 * @return number of registered observation validators
 */
unsigned int get_nr_observation_validators(void)
{
  return nr_observation_validators;
}

/* Validate an observation
 * @param sq_observer position of the observer
 * @param sq_landing landing square of the observer (normally==sq_observee)
 * @param sq_observee position of the piece to be observed
 * @return true iff the observation is valid
 */
boolean validate_observation(square sq_observer,
                             square sq_landing,
                             square sq_observee)
{
  boolean result = true;
  unsigned int i;

  TraceFunctionEntry(__func__);
  TraceSquare(sq_observer);
  TraceSquare(sq_landing);
  TraceSquare(sq_observee);
  TraceFunctionParamListEnd();

  TraceValue("%u\n",nr_observation_validators);

  next_observation_validator = &validate_observer;

  for (i = 0; i!=nr_observation_validators; ++i)
    if (!(*observation_validators[i])(sq_observer,sq_landing,sq_observee))
    {
      result = false;
      break;
    }

  next_observation_validator = &validate_observation;

  if (result)
    result = validate_observer(sq_observer,sq_landing,sq_observee);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

boolean is_square_observed_recursive(slice_index si,
                                     square sq_target,
                                     evalfunction_t *evaluate)
{
  boolean result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceSquare(sq_target);
  TraceFunctionParamListEnd();

  TraceEnumerator(slice_type,slices[si].type,"\n");

  switch (slices[si].type)
  {
    case STIsSquareObservedOrtho:
      if (evaluate==&validate_observation)
        result = is_square_observed_ortho(sq_target);
      else
        result = is_square_observed_recursive(slices[si].next1,sq_target,evaluate);
      break;

    case STSingleBoxType3IsSquareObserved:
      result = singleboxtype3_is_square_observed(si,sq_target,evaluate);
      break;

    case STAnnanIsSquareObserved:
      result = annan_is_square_observed(si,sq_target,evaluate);
      break;

    case STPhantomIsSquareObserved:
      result = phantom_is_square_observed(si,sq_target,evaluate);
      break;

    case STPlusIsSquareObserved:
      result = plus_is_square_observed(si,sq_target,evaluate);
      break;

    case STMarsIsSquareObserved:
      result = marscirce_is_square_observed(si,sq_target,evaluate);
      break;

    case STStingIsSquareObserved:
      result = sting_is_square_observed(si,sq_target,evaluate);
      break;

    case STVaultingKingIsSquareObserved:
      result = vaulting_king_is_square_observed(si,sq_target,evaluate);
      break;

    case STTransmutingKingIsSquareObserved:
      result = transmuting_king_is_square_observed(si,sq_target,evaluate);
      break;

    case STReflectiveKingIsSquareObserved:
      result = reflective_king_is_square_observed(si,sq_target,evaluate);
      break;

    case STFindSquareObserverTrackingBackKing:
      result = find_square_observer_tracking_back_from_target_king(si,
                                                                   sq_target,
                                                                   evaluate);
      break;

    case STFindSquareObserverTrackingBack:
      result = find_square_observer_tracking_back_from_target_non_king(si,
                                                                       sq_target,
                                                                       evaluate);
      break;

    case STFindSquareObserverTrackingBackFairy:
      result = find_square_observer_tracking_back_from_target_fairy(si,
                                                                    sq_target,
                                                                    evaluate);
      break;

    case STTrue:
      result = true;
      break;

    case STFalse:
      result = false;
      break;

    default:
      assert(0);
      break;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

boolean is_square_observed(square sq_target, evalfunction_t *evaluate)
{
  return is_square_observed_recursive(slices[temporary_hack_is_square_observed[trait[nbply]]].next2,
                                      sq_target,
                                      evaluate);
}

static slice_index const is_square_observed_slice_rank_order[] =
{
    STTestingIfSquareIsObserved,
    STIsSquareObservedOrtho,
    STSingleBoxType3IsSquareObserved,
    STAnnanIsSquareObserved,
    STPhantomIsSquareObserved,
    STPlusIsSquareObserved,
    STMarsIsSquareObserved,
    STStingIsSquareObserved,
    STVaultingKingIsSquareObserved,
    STTransmutingKingIsSquareObserved,
    STReflectiveKingIsSquareObserved,
    STFindSquareObserverTrackingBackKing,
    STFindSquareObserverTrackingBack,
    STFindSquareObserverTrackingBackFairy,
    STFalse
};

enum
{
  nr_is_square_observed_slice_rank_order_elmts = sizeof is_square_observed_slice_rank_order / sizeof is_square_observed_slice_rank_order[0]
};

/* Instrument a particular square observation testing branch with a slice type
 * @param testing identifies STTestingIfSquareIsObserved at entrance of branch
 * @param type type of slice to insert
 */
void is_square_observed_insert_slice(slice_index testing, slice_type type)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",testing);
  TraceEnumerator(slice_type,type,"");
  TraceFunctionParamListEnd();

  insert_slice(testing,
               type,
               is_square_observed_slice_rank_order,
               nr_is_square_observed_slice_rank_order_elmts);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

typedef struct
{
    Side side;
    slice_type type;
} square_observed_instrumentation_type;

static void instrument_square_observed_testing(slice_index si,
                                               stip_structure_traversal *st)
{
  square_observed_instrumentation_type const * const it = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children_pipe(si,st);

  if (it->side==nr_sides || it->side==slices[si].starter)
    is_square_observed_insert_slice(si,it->type);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Instrument square observation testing with a slice type
 * @param identifies where to start instrumentation
 * @param side for which side (pass nr_sides to indicate both sides)
 * @param type type of slice with which to instrument moves
 */
void stip_instrument_is_square_observed_testing(slice_index si,
                                                Side side,
                                                slice_type type)
{
  square_observed_instrumentation_type it = { side, type };
  stip_structure_traversal st;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceEnumerator(Side,side,"");
  TraceEnumerator(slice_type,type,"");
  TraceFunctionParamListEnd();

  stip_structure_traversal_init(&st,&it);
  stip_structure_traversal_override_single(&st,
                                           STTestingIfSquareIsObserved,
                                           &instrument_square_observed_testing);
  stip_traverse_structure(si,&st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static void optimise_side(slice_index si, Side side)
{
  slice_index const proxy = slices[temporary_hack_is_square_observed[side]].next2;
  slice_index const testing = slices[proxy].next1;
  slice_index const tracking_back_king = slices[testing].next1;
  slice_index const landing = slices[tracking_back_king].next1;
  slice_index const tracking_back = slices[landing].next1;
  slice_index const leaf = slices[tracking_back].next1;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceEnumerator(Side,side,"");
  TraceFunctionParamListEnd();

  TraceEnumerator(slice_type,slices[proxy].type,"");
  TraceEnumerator(slice_type,slices[testing].type,"");
  TraceEnumerator(slice_type,slices[tracking_back_king].type,"");
  TraceEnumerator(slice_type,slices[tracking_back].type,"");
  TraceEnumerator(slice_type,slices[landing].type,"");
  TraceEnumerator(slice_type,slices[leaf].type,"\n");
  if (slices[testing].type==STTestingIfSquareIsObserved
      && slices[tracking_back_king].type==STFindSquareObserverTrackingBackKing
      && slices[landing].type==STLandingAfterFindSquareObserverTrackingBackKing
      && slices[tracking_back].type==STFindSquareObserverTrackingBack
      && slices[leaf].type==STFalse)
    stip_instrument_is_square_observed_testing(si,side,STIsSquareObservedOrtho);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Optimise the square observation machinery if possible
 * @param si identifies the root slice of the solving machinery
 */
void optimise_is_square_observed(slice_index si)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (nr_observation_validators==0
      && nr_observer_validators==0)
  {
    if (slices[slices[slices[slices[temporary_hack_observation_geometry_tester[White]].next2].next1].next1].type==STTrue)
      optimise_side(si,White);
    if (slices[slices[slices[slices[temporary_hack_observation_geometry_tester[Black]].next2].next1].next1].type==STTrue)
      optimise_side(si,Black);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
