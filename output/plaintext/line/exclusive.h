#if !defined(OUTPUT_PLAINTEXT_LINE_EXCLUSIV_H)
#define OUTPUT_PLAINTEXT_LINE_EXCLUSIV_H

/* This module helps informing the user about undecidable moves in Exclusive Chess
 */

#include "solving/solve.h"

/* Allocate a STExclusiveChessUndecidableWriterLine slice
 * @return identifier of the allocated slice
 */
slice_index alloc_exclusive_chess_undecidable_writer_line_slice(void);

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
stip_length_type exclusive_chess_undecidable_writer_line_solve(slice_index si,
                                                               stip_length_type n);

#endif