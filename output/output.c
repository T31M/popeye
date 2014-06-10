#include "output/output.h"
#include "stipulation/pipe.h"
#include "stipulation/slice_insertion.h"
#include "output/plaintext/plaintext.h"
#include "debugging/trace.h"

char versionString[100];

#ifdef _SE_
#include "se.h"
#endif

boolean flag_regression;

/* Allocate an STOutputModeSelector slice
 * @param mode output mode to be selected by the allocated slice
 * @return identifier of the allocated slice
 */
slice_index alloc_output_mode_selector(output_mode mode)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceEnumerator(output_mode,mode,"");
  TraceFunctionParamListEnd();

  result = alloc_pipe(STOutputModeSelector);
  slices[result].u.output_mode_selector.mode = mode;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Instrument the stipulation structure with slices that implement
 * the selected output mode.
 * @param si identifies slice where to start
 */
void solving_insert_output_slices(slice_index si)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceStipulation(si);

  output_plaintext_instrument_solving(si);
  output_latex_instrument_solving(si);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
