#if !defined(CONDITIONS_ANNAN_H)
#define CONDITIONS_ANNAN_H

#include "py.h"
#include "position/board.h"
#include "utilities/boolean.h"

/* This module implements the condition Annan Chess */

typedef enum
{
  annan_type_A,
  annan_type_B,
  annan_type_C,
  annan_type_D
} annan_type_type;

extern annan_type_type annan_type;

/* Determine whether a piece annanises another
 * @param side side of potentially annanised piece
 * @param rear potential annaniser
 * @param front potential annanisee
 */
boolean annanises(Side side, square rear, square front);

#endif