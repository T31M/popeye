/********************* MODIFICATIONS to pyproof.c ***********************
** 
** Date       Who  What
** 1995/08/10 TLi  Original
**
** 1995/09/28 NG   Bug fixing:
**                 function ProofBlKingMovesNeeded() cast variable
**                 was not set EVERY time, but used !
**                 typo fixed: at the end of function BlPawnMovesNeeded()
**                 (Wh|Bl)PromPieceMovesFromTo(): cenpromsq+-i got out
**                 of range sometimes (example: cenpromsq==272, but there
**                 where 5 captures possible, so cenpromsq+i reached 276)
**                 Also the assignment of  *captures  was different there.
**
** 1995/10/03 NG   ProofFairyImpossible(): test included to check if a
**                 pawn can move from it's original square in AntiCirce,
**                 because it would need at least two captures to bring 
**                 it back.
** 
** 1995/10/06 NG   ProofFairyImpossible(): bug in above test fixed 
**                 and slightly improved.
**
** 1995/10/27 NG   spec[] initilisation needed in ProofInitialise() !
**
** 1995/11/04 NG   Some old (#ifdef NODEF) stuff deleted.
**
** 1995/12/08 NG   Lines 533,542,551 not compilable WITHOUT -DOPTIMIZE
**                 so I did a not usable hack only for compilation.
**
** 1995/12/28 TLi  old orthodox optimization stuff and switch OPTIMIZE
**                 deleted
**                 bug in functions (Wh|Bl)PromPieceMovesFromTo() fixed.
**      
** 1995/12/29 TLi  new condition: GlasgowChess
**
** 1996/06/10 NG   new stipulation: ser-dia (SeriesProofGame)
**
** 1996/12    SE/TLi  new condition: isardam
**
** 1997/01/15 NG   antiandernachchess bug fixed
**
** 1997/02/07 TLi  new restart mechanism
**
** 1997/10/28 SE   proofgame twinning bug fixed.
**		   problems due to BorlandC V3.1 compiler fixed.
**
** 1997/11/08 NG   problems due to compilerbug fix above fixed, ie. 
**                 correction only within define  BC31_SUCKS
**
** 1998/06/05 NG   new option: MaxTime
**
** 1999/01/17 NG   option MaxTime improved
**
** 1999/05/25 TLi  bugfix: condition messigny and proofgames
**
** 2000/10/17 TLi  bugfix: Haaner Chess
**
** 2001/05/20 SE   new stipulation: atob
**
**************************** End of List ******************************/

#include <stdio.h>
#include <stdlib.h>
#ifdef __TURBOC__
# include <mem.h>
#endif
#include "py.h"
#include "pyproc.h"
#include "pydata.h"
#include "pyhash.h"
#include "DHT/dhtbcmem.h"
#include "pyproof.h"
#include "pymsg.h"

/* an array to store the position */
piece ProofPieces[32];
square ProofSquares[32];
smallint ProofNbrAllPieces;
echiquier ProofBoard, PosA;
static  byte buffer[256];
square Proof_rb, Proof_rn, rbA, rnA;
smallint ProofSpec[64], SpecA[64];

smallint xxxxx[fb+fb+1];
#define ProofNbrPiece (xxxxx+fb)

smallint ProofNbrWhitePieces, ProofNbrBlackPieces;

boolean BlockedBishopc1, BlockedBishopf1, BlockedQueend1,
	BlockedBishopc8, BlockedBishopf8, BlockedQueend8,
	CapturedBishopc1, CapturedBishopf1, CapturedQueend1,
	CapturedBishopc8, CapturedBishopf8, CapturedQueend8;

boolean ProofVerifie(void) {
    if (flagfee || PieSpExFlags&(~(BIT(White)+BIT(Black))))
	return VerifieMsg(ProofAndFairyPieces);

    ProofFairy= change_moving_piece || jouegenre ||
		CondFlag[glasgow] ||            /* V3.39  TLi */
		CondFlag[messigny];             /* V3.57  TLi */

    return true;
} /* ProofVerifie */

BCMemValue *ProofEncode(void) {
	byte    *bp, *position, pieces;
	int     i, row, col;
	square  bnp;
	BCMemValue      *bcm;
	boolean even= False;

	bcm= (BCMemValue *)buffer;
	position= bp= bcm->Data;
	/* clear the bits for storing the position of pieces */
	memset(position, 0, 8);
	bp= position+8;

	bnp= bas;
	for (row=0; row<8; row++, bnp+= 16) {
	    for (col=0; col<8; col++, bnp++) {
		piece   p;
		if ((p= e[bnp]) != vide) {
		    if (!even)
			pieces= (byte)(p < vide ? 7-p : p);
		    else {
			*bp++ = pieces+(((byte)(p < vide ? 7-p : p))<<4);
		    }
		    even= !even;
		    position[row] |= BIT(col);
		}
	    }
	}
	if (even)
	    *bp++ = pieces+(15<<4);
	*bp++ = castling_flag[nbply];           /* Castling_Flag */     /* V3.35  NG */
	if (ep[nbply])
		*bp++ = (byte)(ep[nbply] - bas);

	bcm->Leng= bp - bcm->Data;
	return bcm;
}

smallint proofwkm[haut+25-(bas-25)+1];
smallint proofbkm[haut+25-(bas-25)+1];
#if defined(BC31_SUCKS)
smallint *WhKingMoves=proofwkm-(bas-25);
smallint *BlKingMoves=proofbkm-(bas-25);
#else	/* not defined BC31_SUCKS */
#define WhKingMoves  (proofwkm-(bas-25))
#define BlKingMoves  (proofbkm-(bas-25)) 
#endif /* BC31_SUCKS */
/* above changed due to BorlandC V3.1 problems  */	/* V3.50  SE, NG */

void ProofInitialiseKingMoves(square ProofRB, square ProofRN) {
  square *bnp, sq;
  numvec k;
  smallint MoveNbr;
  boolean GoOn;

  /* set all squares to a maximum */
  for (bnp= boardnum; *bnp; bnp++)
    WhKingMoves[*bnp]= BlKingMoves[*bnp]= enonce;

  /* mark squares occupied or garded by immobile pawns */
  /* white pawns */
  for (sq= 224; sq < 232; sq++)
    if (ProofBoard[sq] == pb)
      WhKingMoves[sq]=
      BlKingMoves[sq]= BlKingMoves[sq+23]= BlKingMoves[sq+25]= -1;

  /* black pawns */
  for (sq= 344; sq < 352; sq++)
    if (ProofBoard[sq] == pn)
      BlKingMoves[sq]=
      WhKingMoves[sq]= WhKingMoves[sq-23]= WhKingMoves[sq-25]= -1;

  /* cornered bishops */
  if (BlockedBishopc1)
    WhKingMoves[202]= BlKingMoves[202]= -1;
  if (BlockedBishopf1)
    WhKingMoves[205]= BlKingMoves[205]= -1;
  if (BlockedBishopc8)
    WhKingMoves[370]= BlKingMoves[370]= -1;
  if (BlockedBishopf8)
    WhKingMoves[373]= BlKingMoves[373]= -1;

  /* initialise wh king */
  WhKingMoves[ProofRB]=
  MoveNbr= 0;
  do {
    GoOn= False;
    for (bnp= boardnum; *bnp; bnp++)
      if (WhKingMoves[*bnp] == MoveNbr)
	for (k= 8; k; k--)
	  if (WhKingMoves[*bnp+vec[k]] > MoveNbr) {
	    WhKingMoves[*bnp+vec[k]]= MoveNbr+1;
	    GoOn= True;
	  }
    MoveNbr++;
  } while(GoOn);

  /* initialise bl king */
  BlKingMoves[ProofRN]=
  MoveNbr= 0;
  do {
    GoOn= False;
    for (bnp= boardnum; *bnp; bnp++)
      if (BlKingMoves[*bnp] == MoveNbr)
	for (k= 8; k; k--)
	  if (BlKingMoves[*bnp+vec[k]] > MoveNbr) {
	    BlKingMoves[*bnp+vec[k]]= MoveNbr+1;
	    GoOn= True;
	  }
    MoveNbr++;
  } while(GoOn);
} /* ProofInitialiseKingMoves */

/* a function to store the position and set the PAS */
void ProofInitialise(void) {
  int i;
  piece p;

/*  square ProofRB= rb, ProofRN= rn;            */
  Proof_rb= rb;                              /* V3.50  SE */
  Proof_rn= rn;

  ProofNbrAllPieces=
  ProofNbrWhitePieces=
  ProofNbrBlackPieces= 0;

  for (i= roib; i <= fb; i++) {
    ProofNbrPiece[i]= nbpiece[i];
    ProofNbrWhitePieces+= ProofNbrPiece[i];
    ProofNbrPiece[-i]= nbpiece[-i];
    ProofNbrBlackPieces+= ProofNbrPiece[-i];
  }

  for (i= maxsquare - 1; i >= 0; i--)
    ProofBoard[i]= e[i];

  for (i = 0; i < 64; i++) {
	 ProofSpec[i]=spec[boardnum[i]];   /* V3.50  SE */ /* in case continued
											twinning to other than proof game */
	 p= e[boardnum[i]];
    if (p != vide) {
      ProofPieces[ProofNbrAllPieces]= p;
      ProofSquares[ProofNbrAllPieces++]= boardnum[i];
    }
    CLEARFL(spec[boardnum[i]]);
    p= e[boardnum[i]]= flag_atob ? PosA[boardnum[i]] : PAS[i];
/* We must set spec[] for the PAS. This is used in jouecoup for andernachchess !  V3.37  NG */
    if (p >= roib)
	  SETFLAG(spec[boardnum[i]], White);
    else if (p <= roin)
	  SETFLAG(spec[boardnum[i]], Black);
	 if (flag_atob) 
	 	spec[boardnum[i]]= SpecA[boardnum[i]];
  }

  /* set the king squares */
  rb= 204;      /* white king e1 */
  rn= 372;      /* black king e8 */
	if (flag_atob) 
	{ 
		rb= rbA;
		rn= rnA;
	}
  if (flag_atob)
  {
  		StdString("Initial:");
      WritePosition();
  }

  /* update nbpiece */  
  verifieposition();
  /* store the PAS to be set when linesolution is called */
  StorePosition();

  if (ProofFairy)
    return;

  /* determine pieces blocked */
  BlockedBishopc1=
      ProofBoard[202] == fb && ProofBoard[225] == pb && ProofBoard[227] == pb;
  BlockedBishopf1=
      ProofBoard[205] == fb && ProofBoard[228] == pb && ProofBoard[230] == pb;
  BlockedBishopc8=
      ProofBoard[370] == fn && ProofBoard[345] == pn && ProofBoard[347] == pn;
  BlockedBishopf8=
      ProofBoard[373] == fn && ProofBoard[348] == pn && ProofBoard[350] == pn;
  BlockedQueend1= BlockedBishopc1 && BlockedBishopf1 && ProofBoard[203] == db &&
		  ProofBoard[226] == pb && ProofBoard[229] == pb;
  BlockedQueend8= BlockedBishopc8 && BlockedBishopf8 && ProofBoard[371] == dn &&
		  ProofBoard[346] == pn && ProofBoard[349] == pn;

  /* determine pieces captured */
  CapturedBishopc1=
      ProofBoard[202] != fb && ProofBoard[225] == pb && ProofBoard[227] == pb;
  CapturedBishopf1=
      ProofBoard[205] != fb && ProofBoard[228] == pb && ProofBoard[230] == pb;
  CapturedBishopc8=
      ProofBoard[370] != fn && ProofBoard[345] == pn && ProofBoard[347] == pn;
  CapturedBishopf8=
      ProofBoard[373] != fn && ProofBoard[348] == pn && ProofBoard[350] == pn;
  CapturedQueend1= BlockedBishopc1 && BlockedBishopf1 && ProofBoard[203] != db &&
		   ProofBoard[226] == pb && ProofBoard[229] == pb;
  CapturedQueend8= BlockedBishopc8 && BlockedBishopf8 && ProofBoard[371] != dn &&
		   ProofBoard[346] == pn && ProofBoard[349] == pn;

  /* update castling possibilities */
  if (BlockedBishopc1)
    /* wh long castling impossible -- clear wRa1 */
    castling_flag[0]&= 0x57;
  if (BlockedBishopf1)
    /* wh short castling impossible -- clear wRh1 */
    castling_flag[0]&= 0x67;
  if (BlockedBishopc8)
    /* bl long castling impossible -- clear blRa8 */
    castling_flag[0]&= 0x75;
  if (BlockedBishopf8)
    /* bl short castling impossible -- clear blRh8 */
    castling_flag[0]&= 0x76;
  if (!castling_flag[0]&0x20 && !castling_flag[0]&0x10)
    /* no wh rook can castle, so the wh king cannot either */
    castling_flag[0]&= 0x07;
  if (!castling_flag[0]&0x02 && !castling_flag[0]&0x01)
    /* no bl rook can castle, so the bl king cannot either */
    castling_flag[0]&= 0x70;
  castling_flag[2]= castling_flag[1]= castling_flag[0];

  /* initialise king diff_move arrays */
  ProofInitialiseKingMoves(Proof_rb, Proof_rn);
} /* ProofInitialise */

/* function that compares the current positition with the desired one and
** returns True if they are identical. Otherwise it returns False. */
boolean ProofIdentical(void) {
  int i;

  for (i = 0; i < ProofNbrAllPieces; i++)
    if (ProofPieces[i] != e[ProofSquares[i]]) {
      return False;
    }

  for (i = roib; i <= fb; i++)
    if (ProofNbrPiece[i] != nbpiece[i] ||
	ProofNbrPiece[-i] != nbpiece[-i])
      return False;

  return True;
} /* ProofIdentical */

short ProofKnightMoves[haut-bas+1]= {
  /*   1-  7 */           0,  3,  2,  3,  2,  3,  4,  5,
  /* dummies   8- 16 */  -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*  17- 31 */           4,  3,  4,  3,  2,  1,  2,  3,  2,  1,  2,  3,  4,  3,  4,
  /* dummies  32- 40 */  -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*  41- 55 */           5,  4,  3,  2,  3,  4,  1,  2,  1,  4,  3,  2,  3,  4,  5,
  /* dummies  56- 64 */  -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*  65- 79 */           4,  3,  4,  3,  2,  3,  2,  3,  2,  3,  2,  3,  4,  3,  4,
  /* dummies  80- 88 */  -1, -1, -1, -1, -1, -1, -1, -1,-1,
  /*  89-103 */           5,  4,  3,  4,  3,  2,  3,  2,  3,  2,  3,  4,  3,  4,  5,
  /* dummies 104-112 */  -1, -1, -1, -1, -1, -1, -1, -1,-1,
  /* 113-127 */           4,  5,  4,  3,  4,  3,  4,  3,  4,  3,  4,  3,  4,  5,  4,
  /* dummies 128-136 */  -1, -1, -1, -1, -1, -1, -1, -1,-1,
  /* 137-151 */           5,  4,  5,  4,  3,  4,  3,  4,  3,  4,  3,  4,  5,  4,  5,
  /* dummies 152-160 */  -1, -1, -1, -1, -1, -1, -1, -1,-1,
  /* 161-175 */           6,  5,  4,  5,  4,  5,  4,  5,  4,  5,  4,  5,  4,  5,  6
};

smallint ProofBlKingMovesNeeded(void) {
  smallint needed= BlKingMoves[rn];
  smallint cast;

  if (castling_flag[nbply]&0x04) {
    if (castling_flag[nbply]&0x02) {    /* V3.37  NG */
      /* bl long castling */
      /* BlKingMoves is the number of moves the bl king still needs after castling.
      ** It takes 1 move to castle, but we might save a rook moves */
      cast= BlKingMoves[370];
      if (cast < needed)
	needed= cast;
    }
    if (castling_flag[nbply]&0x01) {    /* V3.37  NG */
      /* bl short castling */
      /* BlKingMoves is the number of moves the bl king still needs after castling.
      ** It takes 1 move to castle, but we might save a rook moves */
      cast= BlKingMoves[374];
      if (cast < needed)
	needed= cast;
    }
  }
  return needed;
} /* ProofBlKingMovesNeeded */

smallint ProofWhKingMovesNeeded(void) {
  smallint needed= WhKingMoves[rb];
  smallint cast;

  if (castling_flag[nbply]&0x40) {
    if (castling_flag[nbply]&0x20) {
      /* wh long castling */
      /* WhKingMoves is the number of moves the wh king still needs after castling.
      ** It takes 1 move to castle, but we might save a rook moves */
      cast= WhKingMoves[202];
      if (cast < needed)
	needed= cast;
    }
    if (castling_flag[nbply]&0x10) {
      /* wh short castling */
      /* WhKingMoves is the number of moves the wh king still needs after castling.
      ** It takes 1 move to castle, but we might save a rook moves */
      cast= WhKingMoves[206];
      if (cast < needed)
	needed= cast;
    }
  }
  return needed;
} /* ProofWhKingMovesNeeded */

void WhPawnMovesFromTo(square from, square to, int *moves, int *captures, int captallowed) {
  int rank_to= to/24;
  int rank_from= from/24;

  /* calculate number of captures */
  *captures= to%24-from%24;
  if (*captures < 0)
    *captures= -*captures;

  /* calculate number of moves */
  *moves= rank_to-rank_from;

  if (*moves < 0 || *moves < *captures || *captures > captallowed)
    *moves= enonce;
  else {
    if (from < bas+24+24 && *captures < *moves-1)
      /* double step possible */
      (*moves)--;
  }
} /* WhPawnMovesFromTo */

void BlPawnMovesFromTo(square from, square to, int *moves, int *captures, int captallowed) {
  int rank_to= to/24;
  int rank_from= from/24;

  /* calculate number of captures */
  *captures= to%24-from%24;
  if (*captures < 0)
    *captures= -*captures;

  /* calculate number of moves */
  *moves= rank_from-rank_to;

  if (*moves < 0 || *moves < *captures || *captures > captallowed)
    *moves= enonce;
  else {
    if (from > haut-24-24 && *captures < *moves-1)
      /* double step possible */
      (*moves)--;
  }
} /* BlPawnMovesFromTo */

smallint WhPawnMovesNeeded(square sq) {
  smallint MovesNeeded, MovesNeeded1;

  /* The first time ProofWhPawnMovesNeeded is called the following test
  ** is always false. It has already been checked in ProofImpossible.
  ** But we need it here for the recursion. */
  if (e[sq] == pb && ProofBoard[sq] != pb)
    return 0;
  if (sq < bas+8+24)
    /* there is no pawn at all that can enter this square */
    return enonce;

  /* double step */
  if (sq/24 == 11 && e[sq-48] == pb && ProofBoard[sq-48] != pb)
    return 1;

  if (e[sq-23] != obs) {
    MovesNeeded= WhPawnMovesNeeded(sq-23);
    if (!MovesNeeded)
      /* There is a free pawn on sq-23
      ** so it takes just 1 move */
      return 1;
  } else
    MovesNeeded= enonce;

  if (e[sq-25] != obs) {
    MovesNeeded1= WhPawnMovesNeeded(sq-25);
    if (!MovesNeeded1)
      /* There is a free pawn on sq-25
      ** so it takes just 1 move */
      return 1;
    if (MovesNeeded1 < MovesNeeded)
      MovesNeeded= MovesNeeded1;
  }

  MovesNeeded1= WhPawnMovesNeeded(sq-24);
  if (MovesNeeded1 < MovesNeeded)
    MovesNeeded= MovesNeeded1;

  return MovesNeeded+1;
} /* WhPawnMovesNeeded */

smallint BlPawnMovesNeeded(square sq) {
  smallint MovesNeeded, MovesNeeded1;

  /* The first time ProofBlPawnMovesNeeded is called the following test
  ** is always false. It has already been checked in ProofImpossible.
  ** But we need it here for the recursion. */
  if (e[sq] == pn && ProofBoard[sq] != pn)
    return 0;

  if (sq > haut-8-24)
    /* there is no pawn at all that can enter this square */
    return enonce;

  /* double step */
  if (sq/24 == 12 && e[sq+48] == pn && ProofBoard[sq+48] != pn)
    return 1;

  if (e[sq+23] != obs) {
    MovesNeeded= BlPawnMovesNeeded(sq+23);
    if (!MovesNeeded)
      /* There is a free pawn on sq+23
      ** so it takes just 1 move */
      return 1;
  } else
    MovesNeeded= enonce;

  if (e[sq+25] != obs) {
    MovesNeeded1= BlPawnMovesNeeded(sq+25);
    if (!MovesNeeded1)
      /* There is a free pawn on sq+25
      ** so it takes just 1 move */
      return 1;
    if (MovesNeeded1 < MovesNeeded)
      MovesNeeded= MovesNeeded1;
  }

  MovesNeeded1= BlPawnMovesNeeded(sq+24);       /* V3.37  NG */
  if (MovesNeeded1 < MovesNeeded)
    MovesNeeded= MovesNeeded1;

  return MovesNeeded+1;
} /* BlPawnMovesNeeded */

#define Blocked(sq) ((e[sq] == pb && ProofBoard[sq] == pb && WhPawnMovesNeeded(sq) >= enonce) || \
		     (e[sq] == pn && ProofBoard[sq] == pn && BlPawnMovesNeeded(sq) >= enonce))

void PieceMovesFromTo(piece p, square from, square to, int *moves) {
  int sqdiff= from-to;
  numvec dir;

  if (sqdiff) {
    switch (abs(p)) {
      case Knight: 
	  *moves= ProofKnightMoves[abs(sqdiff)];
	  if (*moves > 1) {
	    smallint i, j, testmov, testmin= enonce;
	    square sqi, sqj;
	    for (i= 9; i <= 16; i++) {
	      sqi= from+vec[i];
	      if (!Blocked(sqi) && e[sqi] != obs) {
		for (j= 9; j <= 16; j++) {
		  sqj= to+vec[j];
		  if (!Blocked(sqj) && e[sqj] != obs) {
		    testmov= ProofKnightMoves[abs(sqi-sqj)]+2;
		    if (testmov == *moves)
		      return;
		    if (testmov < testmin)
		      testmin= testmov;
		  }
		}
	      }
	    }
	    *moves= testmin;
	  }
	break;
      case Bishop:
	if (SquareCol(from) != SquareCol(to))
	  *moves= enonce;
	else
	  if (dir= CheckDirBishop[sqdiff]) {
	    do {
	      from-= dir;
	    } while (to != from && !Blocked(from));
	    *moves= to == from ? 1 : 3;
	  } else
	    *moves= 2;
	break;
      case Rook:
	if (dir= CheckDirRook[sqdiff]) {
	  do {
	    from-= dir;
	  } while (to != from && !Blocked(from));
	  *moves= to == from ? 1 : 3;
	} else
	  *moves= 2;
	break;
      case Queen:
	if (dir= CheckDirQueen[sqdiff]) {
	  do {
	    from-= dir;
	  } while (to != from && !Blocked(from));
	  *moves= to == from ? 1 : 2;
	} else
	  *moves= 2;
	break;
      default: printf("error in PieceMovesFromTo\n");
	       WritePiece(p); printf("\n");
    }
  } else
    *moves= 0;
} /* PieceMovesFromTo */

void WhPromPieceMovesFromTo(square from, square to, int *moves, int *captures, int captallowed) {
  square cenpromsq= from%24+360;
  int i, mov1, mov2, cap1;
  *moves= enonce;

  WhPawnMovesFromTo(from, cenpromsq, &mov1, &cap1, captallowed);
  PieceMovesFromTo(ProofBoard[to], cenpromsq, to, &mov2);
  if (mov1+mov2 < *moves)
    *moves= mov1+mov2;

  for (i= 1; i <= captallowed; i++) {
    if (cenpromsq+i <= haut) {          /* got out of range sometimes !  V3.37  NG */
      WhPawnMovesFromTo(from, cenpromsq+i, &mov1, &cap1, captallowed);
      PieceMovesFromTo(ProofBoard[to], cenpromsq+i, to, &mov2);
      if (mov1+mov2 < *moves)
	*moves= mov1+mov2;
    }
    if (cenpromsq-i > haut-8) {         /* got out of range sometimes !  V3.37  NG */
      WhPawnMovesFromTo(from, cenpromsq-i, &mov1, &cap1, captallowed);
      PieceMovesFromTo(ProofBoard[to], cenpromsq-i, to, &mov2);
      if (mov1+mov2 < *moves)
	*moves= mov1+mov2;
    }
  }

  /* We cannot say for sure how many captures we really need.
  ** We may need 3 moves and 1 capture or 2 moves and 2 captures.
  ** Therefore zero is returned.   3.38  TLi */
  *captures= 0;
} /* WhPromPieceMovesFromTo */

void BlPromPieceMovesFromTo(square from, square to, int *moves, int *captures, int captallowed) {
  square cenpromsq= from%24+192;
  int i, mov1, mov2, cap1;
  *moves= enonce;

  BlPawnMovesFromTo(from, cenpromsq, &mov1, &cap1, captallowed);
  PieceMovesFromTo(ProofBoard[to], cenpromsq, to, &mov2);
  if (mov1+mov2 < *moves)
    *moves= mov1+mov2;

  for (i= 1; i <= captallowed; i++) {
    if (cenpromsq+i < bas+8) {          /* got out of range sometimes !  V3.37  NG */
      BlPawnMovesFromTo(from, cenpromsq+i, &mov1, &cap1, captallowed);
      PieceMovesFromTo(ProofBoard[to], cenpromsq+i, to, &mov2);
      if (mov1+mov2 < *moves)
	*moves= mov1+mov2;
    }
    if (cenpromsq-i >= bas) {           /* got out of range sometimes !  V3.37  NG */
      BlPawnMovesFromTo(from, cenpromsq-i, &mov1, &cap1, captallowed);
      PieceMovesFromTo(ProofBoard[to], cenpromsq-i, to, &mov2);
      if (mov1+mov2 < *moves)
	*moves= mov1+mov2;
    }
  }

  /* We cannot say for sure how many captures we really need.
  ** We may need 3 moves and 1 capture or 2 moves and 2 captures.
  ** Therefore zero is returned.   3.38  TLi */
  *captures= 0;
} /* BlPromPieceMovesFromTo */

void WhPieceMovesFromTo(square from, square to, int *moves, int *captures,
			int captallowed, int captrequ) {
  piece pfrom= e[from];
  piece pto= ProofBoard[to];

  *moves= enonce;

  switch (pto) {
    case pb:
      if (pfrom == pb)
	WhPawnMovesFromTo(from, to, moves, captures, captallowed);
      break;
    default:
      if (pfrom == pto) {
	PieceMovesFromTo(pfrom, from, to, moves);
	*captures= 0;
      } else if (pfrom == pb)
	WhPromPieceMovesFromTo(from, to, moves, captures, captallowed-captrequ);
  }
} /* WhPieceMovesFromTo */

void BlPieceMovesFromTo(square from, square to, int *moves, int *captures,
			int captallowed, int captrequ) {
  piece pfrom= e[from];
  piece pto= ProofBoard[to];

  *moves= enonce;

  switch (pto) {
    case pn:
      if (pfrom == pn)
	BlPawnMovesFromTo(from, to, moves, captures, captallowed);
      break;
    default:
      if (pfrom == pto) {
	PieceMovesFromTo(pfrom, from, to, moves);
	*captures= 0;
      } else if (pfrom == pn)
	BlPromPieceMovesFromTo(from, to, moves, captures, captallowed-captrequ);
  }
} /* BlPieceMovesFromTo */

typedef struct {
  int  Nbr;
  square sq[16];
} PieceList;

typedef struct {
  int Nbr;
  int moves[16];
  int captures[16];
  int id[16];
} PieceList2;

PieceList ProofWhPawns, CurrentWhPawns,
	  ProofWhPieces, CurrentWhPieces,
	  ProofBlPawns, CurrentBlPawns,
	  ProofBlPieces, CurrentBlPieces;

void PrintPieceList(PieceList *pl) {
  int i;

  for (i= 0; i < pl->Nbr; i++) {
    WriteSquare(pl->sq[i]);
    StdString(" ");
  }
  StdString("\n");
} /* PrintPieceList */

void PrintPieceList2(PieceList2 *pl, int nto) {
  int i, j;

  for (i= 0; i < nto; i++) {
    for (j= 0; j < pl[i].Nbr; j++) {
      printf("%2d %2d %2d   ", pl[i].id[j], pl[i].moves[j], pl[i].captures[j]);
    }
    printf("\n");
  }
} /* PrintPieceList2 */

int ArrangeListedPieces(PieceList2 *pl, int nto, int nfrom, boolean *taken, int CapturesAllowed) {
  int Diff= enonce, Diff2, i, id;

  if (nto == 0)
    return 0;

  for (i= 0; i < pl[0].Nbr; i++) {
    id= pl[0].id[i];
    if (taken[id] || pl[0].captures[i] > CapturesAllowed)
      continue;
    taken[id]= true;
    Diff2= pl[0].moves[i]+ArrangeListedPieces(pl+1, nto-1, nfrom, taken,
					      CapturesAllowed-pl[0].captures[i]);
    if (Diff2 < Diff)
      Diff= Diff2;
    taken[id]= false;
  }
  return Diff;
} /* ArrangeListedPieces */

int ArrangePieces(int CapturesAllowed, couleur camp, int CapturesRequired) {
  int ifrom, ito, moves, captures, Diff;
  PieceList2 pl[16];
  boolean taken[16];
  PieceList *from, *to;

  from= camp == blanc ? &CurrentWhPieces : &CurrentBlPieces;
  to= camp == blanc ? &ProofWhPieces : &ProofBlPieces;

  if (to->Nbr == 0)
    return 0;

  for (ito= 0; ito < to->Nbr; ito++) {
    pl[ito].Nbr= 0;
    for (ifrom= 0; ifrom < from->Nbr; ifrom++) {
      if (camp == blanc)
	WhPieceMovesFromTo(from->sq[ifrom], to->sq[ito], &moves, &captures,
			   CapturesAllowed, CapturesRequired);
      else
	BlPieceMovesFromTo(from->sq[ifrom], to->sq[ito], &moves, &captures,
			   CapturesAllowed, CapturesRequired);
      if (moves < enonce) {
	pl[ito].moves[pl[ito].Nbr]= moves;
	pl[ito].captures[pl[ito].Nbr]= captures;
	pl[ito].id[pl[ito].Nbr]= ifrom;
	pl[ito].Nbr++;
      }
    }
  }

  for (ifrom= 0; ifrom < from->Nbr; ifrom++)
    taken[ifrom]= false;

  /* determine minimal number of moves required */
  Diff= ArrangeListedPieces(pl, to->Nbr, from->Nbr, taken, CapturesAllowed);

  return Diff;
} /* ArrangePieces */

int ArrangePawns(int CapturesAllowed, couleur camp, int *CapturesRequired) {
  int ifrom, ito, moves, captures, Diff;
  PieceList2 pl[8];
  boolean taken[8];
  PieceList *from, *to;

  from= camp == blanc ? &CurrentWhPawns : &CurrentBlPawns;
  to= camp == blanc ? &ProofWhPawns : &ProofBlPawns;

  if (to->Nbr == 0) {
    *CapturesRequired= 0;
    return 0;
  }

  for (ito= 0; ito < to->Nbr; ito++) {
    pl[ito].Nbr= 0;
    for (ifrom= 0; ifrom < from->Nbr; ifrom++) {
      if (camp == blanc)
	WhPawnMovesFromTo(from->sq[ifrom], to->sq[ito], &moves, &captures, CapturesAllowed);
      else
	BlPawnMovesFromTo(from->sq[ifrom], to->sq[ito], &moves, &captures, CapturesAllowed);
      if (moves < enonce) {
	pl[ito].moves[pl[ito].Nbr]= moves;
	pl[ito].captures[pl[ito].Nbr]= captures;
	pl[ito].id[pl[ito].Nbr]= ifrom;
	pl[ito].Nbr++;
      }
    }
  }
  for (ifrom= 0; ifrom < from->Nbr; ifrom++)
    taken[ifrom]= false;

  /* determine minimal number of moves required */
  Diff= ArrangeListedPieces(pl, to->Nbr, from->Nbr, taken, CapturesAllowed);

  if (Diff == enonce)
    return enonce;

  /* determine minimal number of captures required */
  captures= 0;
  while (ArrangeListedPieces(pl, to->Nbr, from->Nbr, taken, captures) == enonce)
    captures++;

  *CapturesRequired= captures;

  return Diff;
} /* ArrangePawns */

boolean ProofFairyImpossible(int MovesAvailable) {
  square *bnp, sq;
  piece p1;

  smallint NbrWh= nbpiece[pb]+nbpiece[cb]+nbpiece[tb]+nbpiece[fb]+nbpiece[db]+1;
  smallint NbrBl= nbpiece[pn]+nbpiece[cn]+nbpiece[tn]+nbpiece[fn]+nbpiece[dn]+1;

  /* not enough time to capture the remaining pieces */
  if (change_moving_piece) {
    if (NbrWh+NbrBl-ProofNbrWhitePieces-ProofNbrBlackPieces > MovesAvailable)
      return true;
    if (CondFlag[andernach]) {     /* V3.37  NG */
      smallint count= 0;
      /* in AndernachChess we need at least 1 capture if a pawn residing at */
      /* his initial square has moved and has to be reestablished via a     */
      /* capture of the opposite side.                                      */

      /* has a white pawn on the second rank moved or has it been captured? */
      for (sq= 224; sq <= 231; sq++)
	  if (e[sq] != pb) {
	      if (ProofBoard[sq] == pb)
			  count++;
	  }     /* e[sq] != pb */
      if ((16 - count) < ProofNbrBlackPieces)
	return true;

      count= 0;
      /* has a black pawn on the seventh rank moved or has it been captured? */
      for (sq= 344; sq <= 351; sq++)
	  if (e[sq] != pn) {
	      if (ProofBoard[sq] == pn) 
		      count++;
	  }     /* e[sq] != pn */
      if ((16 - count) < ProofNbrWhitePieces)
	return true;
    } /* CondFlag[andernach] */
#ifdef NODEF  /* V3.44  NG */
/* seems to be buggy, because during capture there is *no* colour change ! */
    if (CondFlag[antiandernach]) {     /* V3.37  NG */
      smallint count= 0;
      /* in AntiAndernachChess we need at least 1 capture if a pawn residing */
      /* on his initial square has moved at least 2 squares and has to be    */
      /* reestablished via a capture of the opposite side.                   */

      /* has a white pawn on the second rank moved or has it been captured? */
      for (sq= 224; sq <= 231; sq++)
	  if (e[sq] != pb && e[sq+24] != pn ) {
	      if (ProofBoard[sq] == pb)
			  count++;
	  }     /* e[sq] != pb && e[sq+24] != pn */
      if ((16 - count) < ProofNbrWhitePieces)
	return true;

      count= 0;
      /* has a black pawn on the seventh rank moved or has it been captured? */
      for (sq= 344; sq <= 351; sq++)
	  if (e[sq] != pn && e[sq-24] != pb) {
	      if (ProofBoard[sq] == pn) 
		      count++;
	  }     /* e[sq] != pn && e[sq-24] != pb */
      if ((16 - count) < ProofNbrBlackPieces)
	return true;
    } /* CondFlag[antiandernach] */
#endif        /* NODEF */
  } else {
    smallint BlMovesLeft, WhMovesLeft;
    if (FlowFlag(Alternate)) {
        BlMovesLeft= WhMovesLeft= MovesAvailable/2;
        if (MovesAvailable&1)
          if ((flag_atob&&!OptFlag[appseul]) ^ (enonce&1))
	    WhMovesLeft++;
          else
	    BlMovesLeft++;
    } else {				/* ser-dia */	/* V3.42  NG */
	BlMovesLeft= 0;
	WhMovesLeft= MovesAvailable;
    }

    /* not enough time to capture the remaining pieces */
    if (NbrWh-ProofNbrWhitePieces > BlMovesLeft ||
	NbrBl-ProofNbrBlackPieces > WhMovesLeft)
      return true;

    if (!CondFlag[sentinelles]) {     /* V3.37  NG */
      /* note, that we are in the !change_moving_piece section */
      /* too many pawns captured or promoted */
      if (ProofNbrPiece[pb] > nbpiece[pb] ||
	  ProofNbrPiece[pn] > nbpiece[pn])
	return true;
    }   /* !CondFlag[sentinelles] */

    if (CondFlag[anti]) {     /* V3.37  NG */
      /* note, that we are in the !change_moving_piece section */
      smallint count= 0;
      /* in AntiCirce we need at least 2 captures if a pawn residing at */
      /* his initial square has moved and has to be reborn via capture  */
      /* because we need a second pawn to do the same to the other rank */
      /* NOT ALWAYS TRUE ! */
      /* Only if there's no pawn of the same colour on the same rank    */

    /* has a white pawn on the second rank moved or has it been captured? */
    for (sq= 224; sq <= 231; sq++)
	if (e[sq] != pb) {
	    if (ProofBoard[sq] == pb) {
		if (ProofBoard[sq+24] != pb && ProofBoard[sq+48] != pb && 
		    ProofBoard[sq+72] != pb && ProofBoard[sq+96] != pb &&
		    ProofBoard[sq+120] != pb)
			count++;
	    } else if (ProofBoard[sq+24] == pb && e[sq+24] != pb) {
		if (ProofBoard[sq+48] != pb && ProofBoard[sq+72] != pb && 
		    ProofBoard[sq+96] != pb && ProofBoard[sq+120] != pb)
			count++;
	    }   /* ProofBoard[sq] != pb && ProofBoard[sq+24] == pb && e[sq+24] != pb */
	}       /* e[sq] != pb */
    if (count&1)
      count++;
    if ((16 - count) < ProofNbrBlackPieces)
      return true;

    count= 0;
    /* has a black pawn on the seventh rank moved or has it been captured? */
    for (sq= 344; sq <= 351; sq++)
	if (e[sq] != pn) {
	    if (ProofBoard[sq] == pn) {
		if (ProofBoard[sq-24] != pn && ProofBoard[sq-48] != pn && 
		ProofBoard[sq-72] != pn && ProofBoard[sq-96] != pn &&
		ProofBoard[sq-120] != pn)
		    count++;
	    } else if (ProofBoard[sq-24] == pn && e[sq-24] != pn) {
		if (ProofBoard[sq-48] != pn && ProofBoard[sq-72] != pn && 
		    ProofBoard[sq-96] != pn && ProofBoard[sq-120] != pn)
			count++;
	    }   /* ProofBoard[sq] != pn && ProofBoard[sq-24] == pn && e[sq-24] != pn */
	}       /* e[sq] != pn */
    if (count&1)
      count++;
    if ((16 - count) < ProofNbrWhitePieces)
      return true;
    } /* CondFlag[anti] */
  }   /* !change_moving_piece */

#ifdef NODEF	/* I don't understand this !! What is it for ??  V3.42  NG */
#endif	/* NODEF */	/* Now I know that without the line below, Popeye doesn't */
			/* find a solution ... */
  MovesAvailable *= 2;

  for (bnp= boardnum; *bnp; bnp++)
    if ((p1= ProofBoard[*bnp]) != vide)
      if (p1 != e[*bnp])
	MovesAvailable--;

  return MovesAvailable < 0;
} /* ProofFairyImpossible */

boolean ProofImpossible(int MovesAvailable) {
  square *bnp;
  int NbrWh= 0, NbrBl= 0;
  int WhMovesLeft, BlMovesLeft;
  int WhPieToBeCapt, BlPieToBeCapt, WhCapturesRequired, BlCapturesRequired;
  piece p1, p2;
  square sq;

  if (ProofFairy)
    return ProofFairyImpossible(MovesAvailable);

  /* too many pawns captured or promoted */
  if (ProofNbrPiece[pb] > nbpiece[pb] ||
      ProofNbrPiece[pn] > nbpiece[pn])
    return true;

  NbrWh= nbpiece[pb]+nbpiece[cb]+nbpiece[tb]+nbpiece[fb]+nbpiece[db]+1;
  NbrBl= nbpiece[pn]+nbpiece[cn]+nbpiece[tn]+nbpiece[fn]+nbpiece[dn]+1;

  /* to many pieces captured */
  if (NbrWh < ProofNbrWhitePieces ||
      NbrBl < ProofNbrBlackPieces)
    return true;

  /* check if there is enough time left to capture the superfluos pieces */
    if (FlowFlag(Alternate)) {
        BlMovesLeft= WhMovesLeft= MovesAvailable/2;
        if (MovesAvailable&1)
          if ((flag_atob&&!OptFlag[appseul]) ^ (enonce&1))
	    WhMovesLeft++;
          else
	    BlMovesLeft++;
    } else {				/* ser-dia */	/* V3.42  NG */
	BlMovesLeft= 0;
	WhMovesLeft= MovesAvailable;
    }

  /* not enough time to capture the remaining pieces */
  if ((WhPieToBeCapt= NbrWh-ProofNbrWhitePieces) > BlMovesLeft ||
      (BlPieToBeCapt= NbrBl-ProofNbrBlackPieces) > WhMovesLeft)
    return true;

  /* has one of the blocked pieces been captured ? */
  if ((BlockedBishopc1 && ProofBoard[202] != fb) ||
      (BlockedBishopf1 && ProofBoard[205] != fb) ||
      (BlockedBishopc8 && ProofBoard[370] != fn) ||
      (BlockedBishopf8 && ProofBoard[373] != fn) ||
      (BlockedQueend1  && ProofBoard[203] != db) ||
      (BlockedQueend8  && ProofBoard[371] != dn))
    return true;

  /* has a white pawn on the second rank moved or has it been captured? */
  for (sq= 208; sq <= 215; sq++)
    if (ProofBoard[sq] == pb && e[sq] != pb)
      return true;

  /* has a black pawn on the seventh rank moved or has it been captured? */
  for (sq= 344; sq <= 351; sq++)
    if (ProofBoard[sq] == pn && e[sq] != pn)
      return true;

  WhMovesLeft -= ProofWhKingMovesNeeded();
  if (WhMovesLeft < 0)
    return True;

  BlMovesLeft -= ProofBlKingMovesNeeded();
  if (BlMovesLeft < 0)
    return True;

  if (CondFlag[haanerchess])                 /* V3.64  TLi */
    return (ProofBoard[cd[nbcou]] != vide);

  /* collect the pieces for further investigations */
  ProofWhPawns.Nbr= ProofWhPieces.Nbr=
  ProofBlPawns.Nbr= ProofBlPieces.Nbr=
  CurrentWhPawns.Nbr= CurrentWhPieces.Nbr=
  CurrentBlPawns.Nbr= CurrentBlPieces.Nbr= 0;
  for (bnp= boardnum; *bnp; bnp++) {
    p1= ProofBoard[*bnp];
    p2= e[*bnp];

    if (p1 != p2) {
      if (p1 != vide) {
	if (p1 > vide) {  /* it's a white piece */
	  switch (p1) {
	    case roib: break;
	    case pb: ProofWhPawns.sq[ProofWhPawns.Nbr++]= *bnp;
	    	     ProofWhPieces.sq[ProofWhPieces.Nbr++]= *bnp;
		     break;
	    default: ProofWhPieces.sq[ProofWhPieces.Nbr++]= *bnp;
		     break;
	  }
	} else {  /* it's a black piece */
	  switch (p1) {
	    case roin: break;
	    case pn: ProofBlPawns.sq[ProofBlPawns.Nbr++]= *bnp;
	    	     ProofBlPieces.sq[ProofBlPieces.Nbr++]= *bnp;
		     break;
	    default: ProofBlPieces.sq[ProofBlPieces.Nbr++]= *bnp;
		     break;
	  }
	}
      } /* p1 != vide */

      if (p2 != vide) {
	if (p2 > vide) {  /* it's a white piece */
	  switch (p2) {
	    case roib: break;
	    case pb: CurrentWhPawns.sq[CurrentWhPawns.Nbr++]= *bnp;
		     CurrentWhPieces.sq[CurrentWhPieces.Nbr++]= *bnp;
		     break;
	    default: if (!(CapturedBishopc1 && *bnp == 202 && p2 == fb) &&
			 !(CapturedBishopf1 && *bnp == 205 && p2 == fb) &&
			 !(CapturedQueend1 && *bnp == 203 && p2 == db))
				CurrentWhPieces.sq[CurrentWhPieces.Nbr++]= *bnp;
		     break;
	  }
	} else {  /* it's a black piece */
	  switch (p2) {
	    case roin: break;
	    case pn: CurrentBlPawns.sq[CurrentBlPawns.Nbr++]= *bnp;
		     CurrentBlPieces.sq[CurrentBlPieces.Nbr++]= *bnp;
		     break;
	    default: if (!(CapturedBishopc1 && *bnp == 202 && p2 == fn) &&
			 !(CapturedBishopf1 && *bnp == 205 && p2 == fn) &&
			 !(CapturedQueend1 && *bnp == 203 && p2 == dn))
		     		CurrentBlPieces.sq[CurrentBlPieces.Nbr++]= *bnp;
		     break;
	  }
	}
      } /* p2 != vide */
    } /* p1 != p2 */
  } /* for (bnp... */

  if (ArrangePawns(BlPieToBeCapt, blanc, &BlCapturesRequired) > WhMovesLeft) {
    return True;
  }

  if (ArrangePawns(WhPieToBeCapt, noir, &WhCapturesRequired) > BlMovesLeft) {
    return True;
  }

  if (ArrangePieces(BlPieToBeCapt, blanc, BlCapturesRequired) > WhMovesLeft) {
    return True;
  }

  if (ArrangePieces(WhPieToBeCapt, noir, WhCapturesRequired) > BlMovesLeft) {
    return True;
  }

  return false;
} /* ProofImpossible */

boolean ProofSeriesImpossible(int MovesAvailable) {		/* V3.42  NG */
  square *bnp;
  int NbrBl= 0;
  int WhMovesLeft= MovesAvailable;
  int BlPieToBeCapt, BlCapturesRequired;
  piece p1, p2;
  square sq;

  if (ProofFairy)
    return ProofFairyImpossible(MovesAvailable);

  /* too many pawns captured or promoted */
  if (ProofNbrPiece[pb] > nbpiece[pb] ||
      ProofNbrPiece[pn] > nbpiece[pn])
    return true;

  NbrBl= nbpiece[pn]+nbpiece[cn]+nbpiece[tn]+nbpiece[fn]+nbpiece[dn]+1;

  /* to many pieces captured  or */
  /* not enough time to capture the remaining pieces */
  if ((BlPieToBeCapt= NbrBl-ProofNbrBlackPieces) < 0 || BlPieToBeCapt > WhMovesLeft)
    return true;

  /* has a white pawn on the second rank moved ? */
  for (sq= 208; sq <= 215; sq++)
    if (ProofBoard[sq] == pb && e[sq] != pb)
      return true;

  /* has a black pawn on the seventh rank been captured ? */
  for (sq= 344; sq <= 351; sq++)
    if (ProofBoard[sq] == pn && e[sq] != pn)
      return true;

  /* has a black piece on the eigth rank been captured ? */
  for (sq= 368; sq <= 375; sq++)
    if (ProofBoard[sq] < roin && ProofBoard[sq] != e[sq])
      return true;

  WhMovesLeft -= ProofWhKingMovesNeeded();
  if (WhMovesLeft < 0)
    return true;

  /* collect the pieces for further investigations */
  ProofWhPawns.Nbr= ProofWhPieces.Nbr=
  CurrentWhPawns.Nbr= CurrentWhPieces.Nbr= 0;

  for (bnp= boardnum; *bnp; bnp++) {
    p1= ProofBoard[*bnp];
    p2= e[*bnp];

    if (p1 != p2) {
	if (p1 > vide) {  /* it's a white piece */
	  switch (p1) {
	    case roib: break;
	    case pb: ProofWhPawns.sq[ProofWhPawns.Nbr++]= *bnp;
	    	     ProofWhPieces.sq[ProofWhPieces.Nbr++]= *bnp;
		     break;
	    default: ProofWhPieces.sq[ProofWhPieces.Nbr++]= *bnp;
		     break;
	  }
	} /* p1 > vide */

	if (p2 > vide) {  /* it's a white piece */
	  switch (p2) {
	    case roib: break;
	    case pb: CurrentWhPawns.sq[CurrentWhPawns.Nbr++]= *bnp;
	    	     CurrentWhPieces.sq[CurrentWhPieces.Nbr++]= *bnp;
		     break;
	    default: CurrentWhPieces.sq[CurrentWhPieces.Nbr++]= *bnp;
		     break;
	  }
      } /* p2 > vide */
    } /* p1 != p2 */
  } /* for (bnp... */

  if (ArrangePawns(BlPieToBeCapt, blanc, &BlCapturesRequired) > WhMovesLeft) {
    return true;
  }

  if (ArrangePieces(BlPieToBeCapt, blanc, BlCapturesRequired) > WhMovesLeft) {
    return true;
  }

  return false;
} /* ProofSeriesImpossible */

boolean ProofSol(couleur camp, smallint n, boolean restartenabled) {
                                        /* V3.44 TLi */
  boolean flag= false;

  couleur ad= advers(camp);

  if ((OptFlag[maxsols] && (solutions >= maxsolutions)) ||
      FlagTimeOut)		/* V3.54  NG */
	return false;

  /* Let us check whether the position is already in the
  ** hash table and marked unsolvable.
  */
  if (inhash(camp == blanc ? WhHelpNoSucc : BlHelpNoSucc, n))
    return false;

  n--;
  genmove(camp);
  while (encore()){
    if (jouecoup() && !(restartenabled && MoveNbr < RestartNbr)) {
                            /* V3.44  SE/TLi */
        if (n ? (!ProofImpossible(n) && !echecc(camp) && ProofSol(ad, n, False)) :
	        (ProofIdentical() && !echecc(camp))) {
          flag= true;
          if (!n)
	    linesolution();
        }
    }
    if (restartenabled)    /* V3.44  TLi */
        IncrementMoveNbr();
    repcoup();
  }
  finply();
  n++;

  /* Add the position to the hash table if it has no solutions */
  if (!flag)
    addtohash(camp == blanc ? WhHelpNoSucc : BlHelpNoSucc, n);

  return flag;
} /* ProofSol */

boolean SeriesProofSol(smallint n, boolean restartenabled) {
                      /* V3.42  NG, V3.44  LTi  */
/* no camp, because we play always with white ! */
  boolean flag= false;

  if ((OptFlag[maxsols] && (solutions >= maxsolutions)) ||
      FlagTimeOut)		/* V3.54  NG */
	return false;

  /* Let us check whether the position is already in the
  ** hash table and marked unsolvable.
  */
  if (inhash(SerNoSucc, n))
    return false;

  n--;
  genmove(blanc);
  while (encore()){
    if (jouecoup() && !(restartenabled && MoveNbr < RestartNbr)) {  /* V3.44  TLi */
        if (n ? (!ProofSeriesImpossible(n) && !echecc(noir) &&
                 !echecc(blanc) && SeriesProofSol(n, False)) :
	        (ProofIdentical() && !echecc(blanc))) {
          flag= true;
          if (!n)
	    linesolution();
        }
    }
    if (restartenabled)            /* V3.44  TLi */
        IncrementMoveNbr();
    repcoup();
  }
  finply();

  /* Add the position to the hash table if it has no solutions */
  if (!flag)
    addtohash(SerNoSucc, n+1);

  return flag;
} /* SeriesProofSol */


