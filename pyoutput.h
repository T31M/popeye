#if !defined(PYOUTPUT_H)
#define PYOUTPUT_H

#include "py.h"
#include "pygoal.h"
#include "pystip.h"
#include "pytable.h"

#define ENUMERATION_TYPENAME output_mode
#define ENUMERATORS \
  ENUMERATOR(output_mode_tree), \
    ENUMERATOR(output_mode_line), \
    ENUMERATOR(output_mode_none)

#define ENUMERATION_DECLARE

#include "pyenum.h"

/* Set the output mode to be used for the current stipulation
 */
void set_output_mode(output_mode mode);

/* Instrument the stipulation structure with slices that implement
 * the selected output mode.
 */
void stip_insert_output_slices(void);

/* Write the appropriate amount of whitespace for the following output
 * to be correctely indented.
 */
void write_indentation(void);


/* Start a new output level with inverted side at move
 */
void output_start_move_inverted_level(void);

/* End the inner-most output level inverted side at move
 */
void output_end_move_inverted_level(void);

/* End half duplex
 */
void output_end_half_duplex(void);


/* Start a new output level consisting of threats
 */
void output_start_threat_level(void);

/* End the inner-most output level (which consists of threats)
 */
void output_end_threat_level(slice_index si, boolean is_zugzwang);


/* Start a new level of moves
 */
void output_start_move_level(slice_index si);


/* Initialize based on the stipulation
 * @param si index of root of subtree that we initialise output for
 */
void init_output(slice_index si);

/* Write a move in battle play
 */
void write_battle_move(void);

/* Write the decoration (! or ?) for the first move if appropriate
 * @param current_ply identifies ply in which move was played
 * @param type identifies decoration to be added if move_depth==1
 */
void write_battle_move_decoration(ply current_ply, attack_type type);

void write_goal(Goal goal);

/* Mark the defense about to be written as refutation
 */
void write_refutation_mark(void);

/* Write the refutations stored in a table
 * @param refutations table containing refutations
 */
void write_refutations_intro(void);

/* Write the end of a solution
 */
void write_end_of_solution(void);

/* Write the end of a solution phase
 */
void write_end_of_solution_phase(void);

void ecritcoup(ply ply_id);

#endif
