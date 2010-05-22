#include "stipulation/battle_play/defense_move.h"
#include "pydata.h"
#include "pypipe.h"
#include "pyselfcg.h"
#include "stipulation/branch.h"
#include "stipulation/battle_play/branch.h"
#include "stipulation/battle_play/attack_play.h"
#include "stipulation/help_play/move.h"
#include "pyoutput.h"
#include "trace.h"

#include <assert.h>

/* Allocate a STDefenseMove defender slice.
 * @param length maximum number of half-moves of slice (+ slack)
 * @param min_length minimum number of half-moves of slice (+ slack)
 * @return index of allocated slice
 */
slice_index alloc_defense_move_slice(stip_length_type length,
                                     stip_length_type min_length)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",length);
  TraceFunctionParam("%u",min_length);
  TraceFunctionParamListEnd();

  if (min_length<=slack_length_battle)
    min_length += 2;
  assert(min_length>slack_length_battle);
  result = alloc_branch(STDefenseMove,length,min_length);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Produce slices representing set play
 * @param si slice index
 * @param st state of traversal
 */
void defense_move_make_setplay_slice(slice_index si,
                                     stip_structure_traversal *st)
{
  slice_index * const result = st->param;
  stip_length_type const length = slices[si].u.branch.length;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (length>slack_length_battle)
  {
    stip_length_type const length_h = (length-slack_length_battle
                                       +slack_length_help);
    *result = alloc_help_move_slice(length_h,length_h);
    pipe_set_successor(*result,slices[si].u.branch.next);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Find the first postkey slice and deallocate unused slices on the
 * way to it
 * @param si slice index
 * @param st address of structure capturing traversal state
 */
void defense_move_reduce_to_postkey_play(slice_index si,
                                         stip_structure_traversal *st)
{
  slice_index *postkey_slice = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  *postkey_slice = si;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Insert root slices
 * @param si identifies (non-root) slice
 * @param st address of structure representing traversal
 */
void defense_move_insert_root(slice_index si, stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  {
    slice_index * const root = st->param;
    stip_length_type const length = slices[si].u.branch.length;
    stip_length_type const min_length = slices[si].u.branch.min_length;
    *root = alloc_defense_move_slice(length,min_length);
    pipe_set_successor(*root,slices[si].u.pipe.next);
  }
  
  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Detect starter field with the starting side if possible.
 * @param si identifies slice being traversed
 * @param st status of traversal
 */
void defense_move_detect_starter(slice_index si, stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (slices[si].starter==no_side)
  {
    stip_traverse_structure(slices[si].u.pipe.next,st);
    slices[si].starter = advers(slices[slices[si].u.pipe.next].starter);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Try to defend after an attempted key move at root level
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_min minimum number of half-moves of interesting variations
 *              (slack_length_battle <= n_min <= slices[si].u.branch.length)
 * @param max_nr_refutations how many refutations should we look for
 * @return <=n solved  - return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - <=max_nr_refutations refutations found
 *         n+4 refuted - >max_nr_refutations refutations found
 */
stip_length_type defense_move_root_defend(slice_index si,
                                          stip_length_type n,
                                          stip_length_type n_min,
                                          unsigned int max_nr_refutations)
{
  stip_length_type result = 0;
  unsigned int nr_refutations = 0;
  Side const defender = slices[si].starter;
  slice_index const next = slices[si].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_min);
  TraceFunctionParam("%u",max_nr_refutations);
  TraceFunctionParamListEnd();

  assert(n_min>=slack_length_battle);

  if (n_min==slack_length_battle)
    n_min = slack_length_battle+2;

  output_start_defense_level(si);

  move_generation_mode = move_generation_not_optimized;
  TraceValue("->%u\n",move_generation_mode);
  genmove(defender);

  while(encore())
  {
    if (jouecoup(nbply,first_play) && TraceCurrentMove(nbply))
    {
      stip_length_type const nr_moves_needed = attack_solve_in_n(next,
                                                                 n-1,n_min-1);
      if (nr_moves_needed>result)
        result = nr_moves_needed;
      if (nr_moves_needed>n-1)
        ++nr_refutations;
    }

    repcoup();
  }

  finply();

  output_end_defense_level();

  assert(nr_refutations<=max_nr_refutations);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Try to defend after an attempted key move at non-root level.
 * When invoked with some n, the function assumes that the key doesn't
 * solve in less than n half moves.
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_min minimum number of half-moves of interesting variations
 *              (slack_length_battle <= n_min <= slices[si].u.branch.length)
 * @return true iff the defender can defend
 */
stip_length_type defense_move_defend_in_n(slice_index si,
                                          stip_length_type n,
                                          stip_length_type n_min)
{
  stip_length_type result = n_min;
  Side const defender = slices[si].starter;
  slice_index const next = slices[si].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_min);
  TraceFunctionParamListEnd();

  assert(n_min>=slack_length_battle);

  if (n_min==slack_length_battle)
    n_min = slack_length_battle+2;

  output_start_defense_level(si);
  
  move_generation_mode = move_generation_not_optimized;
  TraceValue("->%u\n",move_generation_mode);
  genmove(defender);

  while(encore())
  {
    if (jouecoup(nbply,first_play) && TraceCurrentMove(nbply))
    {
      stip_length_type const nr_moves_needed
          = attack_solve_in_n(next,n-1,n_min-1)+1;
      if (nr_moves_needed>result)
        result = nr_moves_needed;
    }

    repcoup();
  }

  finply();

  output_end_defense_level();

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static unsigned int nr_refutations[maxply+1];

static stip_length_type try_defenses(slice_index si,
                                     stip_length_type n,
                                     stip_length_type n_min,
                                     unsigned int max_nr_refutations)
{
  slice_index const next = slices[si].u.pipe.next;
  stip_length_type result = 0;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_min);
  TraceFunctionParam("%u",max_nr_refutations);
  TraceFunctionParamListEnd();

  while (nr_refutations[nbply]<=max_nr_refutations && encore())
  {
    if (jouecoup(nbply,first_play) && TraceCurrentMove(nbply))
    {
      stip_length_type const length_sol = attack_has_solution_in_n(next,
                                                                   n-1,
                                                                   n_min-1);
      if (length_sol>=n)
      {
        ++nr_refutations[nbply];
        coupfort();
      }

      if (length_sol>result)
        result = length_sol+1;
    }

    repcoup();
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static stip_length_type iterate_other_pieces(slice_index si,
                                             stip_length_type n,
                                             stip_length_type n_min,
                                             square killer_pos,
                                             Side defender,
                                             unsigned int max_nr_refutations)
{
  square const *selfbnp;
  stip_length_type result = 0;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_min);
  TraceSquare(killer_pos);
  TraceEnumerator(Side,defender,"");
  TraceFunctionParamListEnd();

  for (selfbnp = boardnum;
       *selfbnp!=initsquare && nr_refutations[nbply]<=max_nr_refutations;
       ++selfbnp)
    if (*selfbnp!=killer_pos)
    {
      stip_length_type result2;
      piece p = e[*selfbnp];
      if (p!=vide)
      {
        if (TSTFLAG(spec[*selfbnp],Neutral))
          p = -p;
        if (defender==White)
        {
          if (p>obs)
            gen_wh_piece(*selfbnp,p);
        }
        else
        {
          if (p<vide)
            gen_bl_piece(*selfbnp,p);
        }
      }

      result2 = try_defenses(si,n,n_min,max_nr_refutations);
      if (result2>result)
        result = result2;
    }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static
stip_length_type iterate_priorise_killer_piece(slice_index si,
                                               stip_length_type n,
                                               stip_length_type n_min,
                                               Side defender,
                                               square killer_pos,
                                               piece killer,
                                               unsigned int max_nr_refutations)
{
  Side const attacker = advers(defender);
  stip_length_type result;
  stip_length_type result2;
    
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_min);
  TraceEnumerator(Side,defender,"");
  TraceSquare(killer_pos);
  TracePiece(killer);
  TraceFunctionParam("%u",max_nr_refutations);
  TraceFunctionParamListEnd();

  move_generation_mode = move_generation_optimized_by_killer_move;
  TraceValue("->%u\n",move_generation_mode);

  nextply(nbply);
  trait[nbply] = defender;

  if (TSTFLAG(PieSpExFlags,Neutral))
    initneutre(attacker);
  if (TSTFLAG(spec[killer_pos],Neutral))
    killer = -e[killer_pos];

  init_move_generation_optimizer();

  if (defender==White)
  {
    if (killer>obs)
      gen_wh_piece(killer_pos,killer);
  }
  else
  {
    if (killer<-obs)
      gen_bl_piece(killer_pos,killer);
  }

  finish_move_generation_optimizer();

  result = try_defenses(si,n,n_min,max_nr_refutations);

  result2 = iterate_other_pieces(si,
                                 n,n_min,
                                 killer_pos,
                                 defender,
                                 max_nr_refutations);
  if (result2>result)
    result = result2;

  finply();

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static
stip_length_type iterate_last_self_defenses(slice_index si,
                                            stip_length_type n,
                                            stip_length_type n_min,
                                            Side defender,
                                            unsigned int max_nr_refutations)
{
  square const killer_pos = kpilcd[nbply+1];
  piece const killer = e[killer_pos];
  stip_length_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_min);
  TraceEnumerator(Side,defender,"");
  TraceFunctionParam("%u",max_nr_refutations);
  TraceFunctionParamListEnd();

  TraceSquare(killer_pos);
  TracePiece(killer);
  TraceText("\n");

  if (slices[si].u.branch.imminent_goal==goal_ep
      && ep[nbply]==initsquare
      && ep2[nbply]==initsquare)
  {
    /* nothing */
    /* TODO transform to defender filter */
    /* TODO ?create other filters? */
    result = n+4;
  }
  else
  {
    if ((defender==Black ? flagblackmummer : flagwhitemummer)
        || killer==obs || killer==vide)
    {
      move_generation_mode = move_generation_optimized_by_killer_move;
      TraceValue("->%u\n",move_generation_mode);
      genmove(defender);
      result = try_defenses(si,n,n_min,max_nr_refutations);
      finply();
    }
    else
      result = iterate_priorise_killer_piece(si,
                                             n,n_min,
                                             defender,
                                             killer_pos,
                                             killer,
                                             max_nr_refutations);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether there are refutations after an attempted key move
 * at non-root level
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_min minimum number of half-moves of interesting variations
 *              (slack_length_battle <= n_min <= slices[si].u.branch.length)
 * @param max_nr_refutations how many refutations should we look for
 * @return <=n solved  - return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - <=max_nr_refutations refutations found
 *         n+4 refuted - >max_nr_refutations refutations found
 */
stip_length_type defense_move_can_defend_in_n(slice_index si,
                                              stip_length_type n,
                                              stip_length_type n_min,
                                              unsigned int max_nr_refutations)
{
  Side const defender = slices[si].starter;
  stip_length_type max_len_continuation;
  stip_length_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_min);
  TraceFunctionParam("%u",max_nr_refutations);
  TraceFunctionParamListEnd();

  assert(n_min>=slack_length_battle);

  nr_refutations[nbply+1] = 0;

  active_slice[nbply+1] = si;

  /* TODO create a design for representing all these move generation
   * modes
   */
  if (n==slack_length_battle+1 && slices[si].u.branch.imminent_goal!=no_goal)
    max_len_continuation = iterate_last_self_defenses(si,
                                                      n,n_min,
                                                      defender,
                                                      max_nr_refutations);
  else
  {
    if (n_min==slack_length_battle)
      n_min = slack_length_battle+2;

    if (n<=slack_length_battle+3)
    {
      move_generation_mode = move_generation_optimized_by_killer_move;
      TraceValue("->%u\n",move_generation_mode);
      genmove(defender);
    }
    else
    {
      move_generation_mode = move_generation_mode_opti_per_side[defender];
      TraceValue("->%u\n",move_generation_mode);
      genmove(defender);
    }

    max_len_continuation = try_defenses(si,n,n_min,max_nr_refutations);

    finply();
  }

  if (max_len_continuation<n_min)
    result = n+4;
  else if (nr_refutations[nbply+1]>max_nr_refutations)
    result = n+4;
  else if (nr_refutations[nbply+1]>0)
    result = n+2;
  else
    result = max_len_continuation;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}
