#if !defined(OUTPUT_PLAINTEXT_PLAINTEXT_H)
#define OUTPUT_PLAINTEXT_PLAINTEXT_H

#include "py.h"
#include "stipulation/goals/goals.h"

/* Write the current move of a ply
 * @param ply_id identifies the ply of which to write the current move
 */
void output_plaintext_write_move(ply ply_id);

/* Determine whether a goal writer slice should replace the check writer slice
 * which would normally following the possible check deliverd by the move just
 * played (i.e. if both a possible check and the symbol for the reached goal
 * should be written).
 * @param goal goal written by goal writer
 * @return true iff the check writer should be replaced by the goal writer
 */
boolean output_plaintext_goal_writer_replaces_check_writer(goal_type goal);

#endif
