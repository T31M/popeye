#include "conditions/andernach.h"
#include "pydata.h"
#include "stipulation/pipe.h"
#include "stipulation/has_solution_type.h"
#include "stipulation/stipulation.h"
#include "stipulation/move.h"
#include "solving/move_effect_journal.h"
#include "debugging/trace.h"

#include <assert.h>
#include <stdlib.h>

/* Let the arriving piece of a move assume a side
 * @param side side to assume
 */
void andernach_assume_side(Side side)
{
  move_effect_journal_index_type const base = move_effect_journal_top[nbply-1];
  move_effect_journal_index_type const movement = base+move_effect_journal_index_offset_movement;
  square const sq_arrival = move_effect_journal[movement].u.piece_movement.to;
  PieceIdType const moving_id = GetPieceId(move_effect_journal[movement].u.piece_movement.movingspec);

  square const pos = move_effect_journal_follow_piece_through_other_effects(nbply,
                                                        moving_id,
                                                        sq_arrival);
  if (pos!=initsquare)
    move_effect_journal_do_side_change(move_effect_reason_andernach_chess,
                                       pos,
                                       side);
}

/* Try to solve in n half-moves.
 * @param si slice index
 * @param n maximum number of half moves
 * @return length of solution found and written, i.e.:
 *            previous_move_is_illegal the move just played (or being played)
 *                                     is illegal
 *            immobility_on_next_move  the moves just played led to an
 *                                     unintended immobility on the next move
 *            <=n+1 length of shortest solution found (n+1 only if in next
 *                                     branch)
 *            n+2 no solution found in this branch
 *            n+3 no solution found in next branch
 */
stip_length_type andernach_side_changer_solve(slice_index si,
                                               stip_length_type n)
{
  move_effect_journal_index_type const base = move_effect_journal_top[nbply-1];
  move_effect_journal_index_type const capture = base+move_effect_journal_index_offset_capture;
  move_effect_journal_index_type const movement = base+move_effect_journal_index_offset_movement;
  square const sq_departure = move_effect_journal[movement].u.piece_movement.from;
  stip_length_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  if (move_effect_journal[capture].type==move_effect_piece_removal
      && sq_departure!=prev_king_square[Black][nbply]
      && sq_departure!=prev_king_square[White][nbply])
    andernach_assume_side(advers(slices[si].starter));

  result = solve(slices[si].next1,n);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Instrument slices with move tracers
 */
void stip_insert_andernach(slice_index si)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  stip_instrument_moves(si,STAndernachSideChanger);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
