/*************************************************************************
 *   $cours: lex/yacc
 * $section: projet
 *      $Id: wr.c 433 2017-10-18 20:37:37Z ia $
 * $HeadURL: svn://lunix120.ensiie.fr/ia/cours/lex-yacc/src/labyrinthe/labgen/wr.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "top.h"

/*======================================================================*/

TwrInfo wri_t[] = {
  //  name     move         ew1          ew2
    { "N"  , { 0 , -1} , {-1 , -1} , {-1 , -1} },
    { "NE" , { 1 , -1} , { 0 , -1} , {-1 ,  0} },
    { "E"  , { 1 ,  0} , {-1 , -1} , {-1 , -1} },
    { "SE" , { 1 ,  1} , { 0 ,  1} , { 1 ,  0} },
    { "S"  , { 0 ,  1} , {-1 , -1} , {-1 , -1} },
    { "SW" , {-1 ,  1} , { 0 ,  1} , {-1 ,  0} },
    { "W"  , {-1 ,  0} , {-1 , -1} , {-1 , -1} },
    { "NW" , {-1 , -1} , { 0 , -1} , {-1 ,  0} },
};

/*======================================================================*/

extern Cstr   wr_uname(Twr wrd)
{
    if ( 0<=wrd && wrd<LG_WrNb )
        return wri_t[wrd].name;
    else
        return "??";
}

extern Twr wr_fromstr(char *str) {
    Twr v = LG_WrUU;

    if(strcmp(str, "N") == 0) {
        v = LG_WrNN;
    } else if(strcmp(str, "S") == 0) {
        v = LG_WrSS;
    } else if(strcmp(str, "E") == 0) {
        v = LG_WrEE;
    } else if(strcmp(str, "W") == 0) {
        v = LG_WrWW;
    } else if(strcmp(str, "NE") == 0) {
        v = LG_WrNE;
    } else if(strcmp(str, "NW") == 0) {
        v = LG_WrNW;
    } else if(strcmp(str, "SE") == 0) {
        v = LG_WrSE;
    } else if(strcmp(str, "SW") == 0) {
        v = LG_WrSW;
    }

    return v;
}

extern Tpoint wr_next(Twr wrd, Tpoint from)
{
#if 1
    if ( !(0<=wrd && wrd<LG_WrNb) ) ASSERT_MESS();
#endif
    Tpoint to;
    to.x = from.x + wri_t[wrd].move.x;
    to.y = from.y + wri_t[wrd].move.y;
    return to;
}

/*======================================================================*/
