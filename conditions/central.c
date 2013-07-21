#include "conditions/central.h"
#include "solving/move_generator.h"
#include "solving/observation.h"
#include "stipulation/stipulation.h"
#include "debugging/trace.h"
#include "pydata.h"

static boolean can_piece_move_from(square sq_departure);

static boolean central_test_supporter(square sq_departure,
                                      square sq_arrival,
                                      square sq_capture)
{
  boolean result;

  TraceFunctionEntry(__func__);
  TraceSquare(sq_departure);
  TraceSquare(sq_arrival);
  TraceSquare(sq_capture);
  TraceFunctionParamListEnd();

  if (validate_observer(sq_departure,sq_arrival,sq_capture))
    result = can_piece_move_from(sq_departure);
  else
    result = false;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static boolean can_piece_move_from(square sq_departure)
{
  boolean result;

  TraceFunctionEntry(__func__);
  TraceSquare(sq_departure);
  TraceFunctionParamListEnd();

  if (sq_departure==king_square[trait[nbply]])
    result = true;
  else
    result = is_square_observed(sq_departure,&central_test_supporter);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static boolean is_observer_supported(square sq_observer,
                                     square sq_landing,
                                     square sq_observee)
{
  boolean result;

  TraceFunctionEntry(__func__);
  TraceSquare(sq_observer);
  TraceSquare(sq_landing);
  TraceSquare(sq_observee);
  TraceFunctionParamListEnd();

  result = can_piece_move_from(sq_observer);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Generate moves for a single piece
 * @param identifies generator slice
 * @param sq_departure departure square of generated moves
 * @param p walk to be used for generating
 */
void central_generate_moves_for_piece(slice_index si,
                                      square sq_departure,
                                      PieNam p)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceSquare(sq_departure);
  TracePiece(p);
  TraceFunctionParamListEnd();

  if (can_piece_move_from(sq_departure))
    generate_moves_for_piece(slices[si].next1,sq_departure,p);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Inialise the solving machinery with Central Chess
 * @param si identifies root slice of solving machinery
 */
void central_initialise_solving(slice_index si)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  solving_instrument_move_generation(si,nr_sides,STCentralMovesForPieceGenerator);

  register_observation_validator(&is_observer_supported);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
