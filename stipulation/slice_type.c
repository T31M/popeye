#define ENUMERATION_TYPENAME slice_type
#define ENUMERATORS \
  ENUMERATOR(STProxy),                                                  \
  ENUMERATOR(STTemporaryHackFork),                                      \
  /* battle play structural slices */                                 \
    ENUMERATOR(STAttackAdapter),   /* switch from generic play to attack play */ \
    ENUMERATOR(STDefenseAdapter),  /* switch from generic play to defense play */ \
    ENUMERATOR(STReadyForAttack),     /* proxy mark before we start playing attacks */ \
    ENUMERATOR(STReadyForDefense),     /* proxy mark before we start playing defenses */ \
    ENUMERATOR(STNotEndOfBranchGoal),     /* proxy mark after testing whether the branch is ended because it reached a goal */ \
    ENUMERATOR(STNotEndOfBranch),     /* proxy mark after testing whether the branch is ended */ \
    ENUMERATOR(STMinLengthOptimiser), /* don't even try attacks in less than min_length moves */ \
    /* help play structural slices */                                   \
    ENUMERATOR(STHelpAdapter), /* switch from generic play to help play */ \
    ENUMERATOR(STReadyForHelpMove),                                     \
    /* other structural slices */                                       \
    ENUMERATOR(STSetplayFork),                                          \
    ENUMERATOR(STEndOfBranch), /* end of branch, general case (not reflex, not goal) */ \
    ENUMERATOR(STEndOfBranchForced),  /* side at the move is forced to solve fork if possible */ \
    ENUMERATOR(STEndOfBranchGoal), /* end of branch leading to immediate goal */ \
    ENUMERATOR(STEndOfBranchTester), /* test for end of branch only */ \
    ENUMERATOR(STEndOfBranchGoalTester), /* test for end of branch leading to immediate goal only */ \
    ENUMERATOR(STAvoidUnsolvable), /* avoid solving branches that we know are unsolvable */ \
    ENUMERATOR(STResetUnsolvable), /* reset knowledge about unsolvability */ \
    ENUMERATOR(STLearnUnsolvable), /* update knowledge about unsolvability */ \
    ENUMERATOR(STConstraintSolver),  /* stop unless some condition is met */ \
    ENUMERATOR(STConstraintTester),  /* stop unless some condition is met */ \
    ENUMERATOR(STGoalConstraintTester),  /* stop unless some goal has (not) been reached */ \
    ENUMERATOR(STEndOfRoot), /* proxy slice marking the end of the root branch */ \
    ENUMERATOR(STEndOfIntro), /* proxy slice marking the end of the intro branch */ \
    ENUMERATOR(STDeadEnd), /* stop solving if there are no moves left to be played */ \
    ENUMERATOR(STMove),                                                \
    ENUMERATOR(STForEachMove), /* iterate over each generated move */  \
    ENUMERATOR(STFindMove), /* find the first move to satisfy some condition */ \
    ENUMERATOR(STMovePlayed),                                          \
    ENUMERATOR(STHelpMovePlayed),                                      \
    ENUMERATOR(STDummyMove),    /* dummy move */                       \
    ENUMERATOR(STReadyForDummyMove),                                   \
    ENUMERATOR(STShortSolutionsStart), /* proxy slice marking where we start looking for short battle solutions in line mode */ \
    ENUMERATOR(STCheckZigzagJump),                                     \
    ENUMERATOR(STCheckZigzagLanding),                                  \
    ENUMERATOR(STGoalReachedTester), /* proxy slice marking the start of goal testing */ \
    ENUMERATOR(STGoalMateReachedTester), /* tests whether a mate goal has been reached */ \
    ENUMERATOR(STGoalStalemateReachedTester), /* tests whether a stalemate goal has been reached */ \
    ENUMERATOR(STGoalDoubleStalemateReachedTester), /* tests whether a double stalemate goal has been reached */ \
    ENUMERATOR(STGoalTargetReachedTester), /* tests whether a target goal has been reached */ \
    ENUMERATOR(STGoalCheckReachedTester), /* tests whether a check goal has been reached */ \
    ENUMERATOR(STGoalCaptureReachedTester), /* tests whether a capture goal has been reached */ \
    ENUMERATOR(STGoalSteingewinnReachedTester), /* tests whether a steingewinn goal has been reached */ \
    ENUMERATOR(STGoalEnpassantReachedTester), /* tests whether an en passant goal has been reached */ \
    ENUMERATOR(STGoalDoubleMateReachedTester), /* tests whether a double mate goal has been reached */ \
    ENUMERATOR(STGoalCounterMateReachedTester), /* tests whether a counter-mate goal has been reached */ \
    ENUMERATOR(STGoalCastlingReachedTester), /* tests whether a castling goal has been reached */ \
    ENUMERATOR(STGoalAutoStalemateReachedTester), /* tests whether an auto-stalemate goal has been reached */ \
    ENUMERATOR(STGoalCircuitReachedTester), /* tests whether a circuit goal has been reached */ \
    ENUMERATOR(STGoalExchangeReachedTester), /* tests whether an exchange goal has been reached */ \
    ENUMERATOR(STGoalCircuitByRebirthReachedTester), /* tests whether a circuit by rebirth goal has been reached */ \
    ENUMERATOR(STGoalExchangeByRebirthReachedTester), /* tests whether an "exchange B" goal has been reached */ \
    ENUMERATOR(STGoalAnyReachedTester), /* tests whether an any goal has been reached */ \
    ENUMERATOR(STGoalProofgameReachedTester), /* tests whether a proof game goal has been reached */ \
    ENUMERATOR(STGoalAToBReachedTester), /* tests whether an "A to B" goal has been reached */ \
    ENUMERATOR(STGoalMateOrStalemateReachedTester), /* just a placeholder - we test using the mate and stalemate testers */ \
    ENUMERATOR(STGoalChess81ReachedTester), /* tests whether an Chess81 goal has been reached */ \
    ENUMERATOR(STGoalImmobileReachedTester), /* auxiliary slice testing whether a side is immobile */ \
    ENUMERATOR(STGoalNotCheckReachedTester), /* auxiliary slice enforcing that a side is not in check */ \
    /* boolean logic */                                                 \
    ENUMERATOR(STTrue),            /* true leaf slice */                \
    ENUMERATOR(STFalse),           /* false leaf slice */               \
    ENUMERATOR(STAnd),      /* logical AND */                           \
    ENUMERATOR(STOr),       /* logical OR */                            \
    ENUMERATOR(STNot),             /* logical NOT */                    \
    /* auxiliary slices */                                              \
    ENUMERATOR(STCheckDetector), /* detect check delivered by previous move */ \
    ENUMERATOR(STSelfCheckGuard),  /* stop when a side has exposed its king */ \
    ENUMERATOR(STOhneschachCheckGuard),  /* stop when a side has attacked the opponent's king */ \
    ENUMERATOR(STMoveInverter),    /* inverts side to move */           \
    ENUMERATOR(STMinLengthGuard), /* make sure that the minimum length of a branch is respected */  \
    ENUMERATOR(STForkOnRemaining),     /* fork depending on the number of remaining moves */ \
    /* solver slices */                                                 \
    ENUMERATOR(STFindShortest), /* find the shortest continuation(s) */                                  \
    ENUMERATOR(STFindByIncreasingLength), /* find all solutions */      \
    ENUMERATOR(STGeneratingMoves),  /* proxy marking start of move generation */ \
    ENUMERATOR(STMoveGenerator), /* unoptimised move generator */       \
    ENUMERATOR(STKingMoveGenerator), /* move generator for king moves */ \
    ENUMERATOR(STNonKingMoveGenerator), /* move generator for other moves */ \
    ENUMERATOR(STCastlingIntermediateMoveGenerator), /* generates intermediate castling moves */ \
    ENUMERATOR(STCastlingIntermediateMoveLegalityTester), /* tests the legality of intermediate castling moves */ \
    ENUMERATOR(STRefutationsAllocator), /* (de)allocate the table holding the refutations */ \
    ENUMERATOR(STRefutationsSolver), /* find battle play refutations */ \
    ENUMERATOR(STRefutationsFilter), /* only consider refutations */ \
    ENUMERATOR(STEndOfRefutationSolvingBranch), /* end of special branch */ \
    ENUMERATOR(STPlaySuppressor), /* suppresses play */                 \
    ENUMERATOR(STContinuationSolver), /* solves battle play continuations */ \
    ENUMERATOR(STSolvingContinuation), /* proxy slice representing STContinuationSolver in testing mode */ \
    ENUMERATOR(STThreatSolver), /* solves threats */                    \
    ENUMERATOR(STThreatEnforcer), /* filters out defense that don't defend against the threat(s) */ \
    ENUMERATOR(STThreatStart), /* proxy slice marking where to start solving threats */ \
    ENUMERATOR(STThreatEnd), /* proxy slice marking where to end solving threats */ \
    ENUMERATOR(STThreatCollector), /* collects threats */               \
    ENUMERATOR(STThreatDefeatedTester), /* detect whether threats have been defeated  */ \
    ENUMERATOR(STRefutationsCollector), /* collects refutations */      \
    ENUMERATOR(STRefutationsAvoider), /* aovids refutations while solving variations */      \
    ENUMERATOR(STLegalMoveCounter), /* counts legal moves */            \
    ENUMERATOR(STAnyMoveCounter), /* counts moves whether legal or not */            \
    ENUMERATOR(STCaptureCounter), /* counts captures */            \
    /* slices enforcing prerequisites of some stipulations */           \
    ENUMERATOR(STTestingPrerequisites),  /* proxy marking start of prerequisites tests */ \
    ENUMERATOR(STDoubleMateFilter),  /* enforces precondition for doublemate */ \
    ENUMERATOR(STCounterMateFilter),  /* enforces precondition for counter-mate */ \
    ENUMERATOR(STPrerequisiteOptimiser), /* optimise if prerequisites are not met */ \
    /* slices implementing user options */                              \
    ENUMERATOR(STNoShortVariations), /* filters out short variations */ \
    ENUMERATOR(STRestartGuard),    /* write move numbers */             \
    ENUMERATOR(STRestartGuardIntelligent), /* filter out too short solutions */ \
    ENUMERATOR(STIntelligentTargetCounter), /* count target positions */ \
    ENUMERATOR(STMaxTimeGuard), /* deals with option maxtime */         \
    ENUMERATOR(STMaxSolutionsInitialiser), /* initialise solution counter for option maxsolutions */  \
    ENUMERATOR(STMaxSolutionsGuard), /* deals with option maxsolutions */  \
    ENUMERATOR(STMaxSolutionsCounter), /* count solutions for option maxsolutions */  \
    /* slices implementing optimisations */                             \
    ENUMERATOR(STEndOfBranchGoalImmobile), /* end of branch leading to "immobile goal" (#, =, ...) */ \
    ENUMERATOR(STDeadEndGoal), /* like STDeadEnd, but all ends are goals */ \
    ENUMERATOR(STOrthodoxMatingMoveGenerator),                          \
    ENUMERATOR(STKillerMoveCollector), /* remember killer moves */      \
    ENUMERATOR(STKillerMoveFinalDefenseMove), /* priorise killer move */ \
    ENUMERATOR(STEnPassantFilter),  /* enforces precondition for goal ep */ \
    ENUMERATOR(STCastlingFilter),  /* enforces precondition for goal castling */ \
    ENUMERATOR(STAttackHashed),    /* hash table support for attack */  \
    ENUMERATOR(STAttackHashedTester),  /* attack play with hash table */      \
    ENUMERATOR(STHelpHashed),      /* help play with hash table */      \
    ENUMERATOR(STHelpHashedTester),      /* help play with hash table */      \
    ENUMERATOR(STIntelligentMovesLeftInitialiser), /* count the moves left for both sides */ \
    ENUMERATOR(STIntelligentMateFilter), /* solve mate problems in intelligent mode */ \
    ENUMERATOR(STIntelligentStalemateFilter), /* solve stalemate problems in intelligent mode */ \
    ENUMERATOR(STIntelligentProof), /* solve proof games and A=>B in intelligent mode */ \
    ENUMERATOR(STGoalReachableGuardFilterMate), /* goal still reachable in intelligent mode? */ \
    ENUMERATOR(STGoalReachableGuardFilterStalemate), /* goal still reachable in intelligent mode? */ \
    ENUMERATOR(STGoalReachableGuardFilterProof), /* goal still reachable in intelligent mode? */ \
    ENUMERATOR(STGoalReachableGuardFilterProofFairy), /* goal still reachable in intelligent mode? */ \
    ENUMERATOR(STIntelligentSolutionsPerTargetPosCounter), /* count solutions per target position */ \
    ENUMERATOR(STIntelligentLimitNrSolutionsPerTargetPos), /* limit number of solutions per target position */ \
    ENUMERATOR(STIntelligentDuplicateAvoider), /* avoid double solutions in intelligent mode */ \
    ENUMERATOR(STIntelligentImmobilisationCounter), /* determine how immobile the side to be stalemated is already */ \
    ENUMERATOR(STKeepMatingFilter), /* deals with option KeepMatingPiece */ \
    ENUMERATOR(STMaxFlightsquares), /* deals with option MaxFlightsquares */ \
    ENUMERATOR(STFlightsquaresCounter), /* deals with option MaxFlightsquares */ \
    ENUMERATOR(STDegenerateTree),  /* degenerate tree optimisation */   \
    ENUMERATOR(STMaxNrNonTrivial), /* deals with option NonTrivial */   \
    ENUMERATOR(STMaxNrNonTrivialCounter), /* deals with option NonTrivial */ \
    ENUMERATOR(STMaxThreatLength), /* deals with option Threat */       \
    ENUMERATOR(STMaxThreatLengthStart), /* where should STMaxThreatLength start looking for threats */ \
    ENUMERATOR(STStopOnShortSolutionsInitialiser), /* intialise stoponshortsolutions machinery */  \
    ENUMERATOR(STStopOnShortSolutionsFilter), /* enforce option stoponshortsolutions */  \
    ENUMERATOR(STAmuMateFilter), /* detect whether AMU prevents a mate */ \
    ENUMERATOR(STUltraschachzwangGoalFilter), /* suspend Ultraschachzwang when testing for mate */ \
    ENUMERATOR(STCirceSteingewinnFilter), /* is 'won' piece reborn? */ \
    ENUMERATOR(STCirceCircuitSpecial), /* has reborn capturee made a circuit? */ \
    ENUMERATOR(STCirceExchangeSpecial), /* has reborn capturee made an exchange? */ \
    ENUMERATOR(STAnticirceTargetSquareFilter), /* target square is not reached by capture */ \
    ENUMERATOR(STAnticirceCircuitSpecial), /* special circuit by rebirth */ \
    ENUMERATOR(STAnticirceExchangeSpecial), /* special exchange by rebirth */ \
    ENUMERATOR(STAnticirceExchangeFilter), /* only apply special test after capture in Anticirce */ \
    ENUMERATOR(STPiecesParalysingMateFilter), /* goal not reached because of special rule? */ \
    ENUMERATOR(STPiecesParalysingMateFilterTester), /* goal not reached because of special rule? */ \
    ENUMERATOR(STPiecesParalysingStalemateSpecial), /* stalemate by special rule? */ \
    ENUMERATOR(STPiecesKamikazeTargetSquareFilter), /* target square not reached because of capture by Kamikaze piece? */ \
    ENUMERATOR(STImmobilityTester), \
    ENUMERATOR(STOpponentMovesCounterFork), \
    ENUMERATOR(STOpponentMovesCounter), \
    /* other slices related to fairy chess */ \
    ENUMERATOR(STOhneschachSuspender), \
    ENUMERATOR(STExclusiveChessMatingMoveCounter), \
    ENUMERATOR(STExclusiveChessUnsuspender), \
    ENUMERATOR(STMaffImmobilityTesterKing), \
    ENUMERATOR(STOWUImmobilityTesterKing), \
    ENUMERATOR(STSingleMoveGeneratorWithKingCapture),                          \
    ENUMERATOR(STBrunnerDefenderFinder), \
    ENUMERATOR(STIsardamDefenderFinder), \
    ENUMERATOR(STCageCirceNonCapturingMoveFinder), \
    ENUMERATOR(STSinglePieceMoveGenerator),                          \
    ENUMERATOR(STSingleMoveGenerator), \
    ENUMERATOR(STMaximummerCandidateMoveTester), \
    ENUMERATOR(STBGLFilter), \
    /* output slices */                                                 \
    ENUMERATOR(STOutputModeSelector), /* select an output mode for the subsequent play */ \
    ENUMERATOR(STIllegalSelfcheckWriter), /* indicate illegal self-check in the diagram position */ \
    ENUMERATOR(STEndOfPhaseWriter), /* write the end of a phase */      \
    ENUMERATOR(STEndOfSolutionWriter), /* write the end of a solution */  \
    ENUMERATOR(STThreatWriter), /* writes "threat:" */ \
    ENUMERATOR(STMoveWriter), /* writes moves */ \
    ENUMERATOR(STKeyWriter), /* write battle play keys */               \
    ENUMERATOR(STTryWriter), /* write "but" */                          \
    ENUMERATOR(STZugzwangWriter), /* writes zugzwang if appropriate */  \
    ENUMERATOR(STTrivialEndFilter), /* don't write trivial variations */  \
    ENUMERATOR(STRefutingVariationWriter), /* writes refuting variations */ \
    ENUMERATOR(STRefutationsIntroWriter), /* write "But" */  \
    ENUMERATOR(STRefutationWriter), /* writes refutations */  \
    ENUMERATOR(STOutputPlaintextTreeCheckWriter), /* plain text output, tree mode: write checks by the previous move */  \
    ENUMERATOR(STOutputPlaintextLineLineWriter), /* plain text output, line mode: write a line */  \
    ENUMERATOR(STOutputPlaintextTreeGoalWriter), /* plain text output, tree mode: write the reached goal */  \
    ENUMERATOR(STOutputPlaintextMoveInversionCounter), /* plain text output: count move inversions */  \
    ENUMERATOR(STOutputPlaintextLineEndOfIntroSeriesMarker), /* handles the end of the intro series */  \
    /* debugging slices */                                              \
    ENUMERATOR(STMoveTracer),                                           \
    ENUMERATOR(nr_slice_types),                                         \
    ASSIGNED_ENUMERATOR(no_slice_type = nr_slice_types)

#define ENUMERATION_MAKESTRINGS
#include "utilities/enumeration.h"