/*************************************************************************
 *   $cours: lex/yacc
 * $section: projet
 *      $Id: pdt.h 440 2017-10-20 16:34:42Z ia $
 * $HeadURL: svn://lunix120.ensiie.fr/ia/cours/lex-yacc/src/labyrinthe/labgen/pdt.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
*************************************************************************/
#ifndef FILE_PDT_H
#define FILE_PDT_H
/*======================================================================*/

#include "vars.h"
#include "points.h"
#include "lds.h"

#define PDT_FVARSSIZE 100

/*======================================================================*/
/*= Parser DaTa                                                        =*/

struct _Tpdt {
    Tvars *vars;
    char *fvars_names[PDT_FVARSSIZE]; // we need an array to store in order
                                     // the variables names
    int fvars_index;
    Tpoints* in;
    Tpoints* out;
    Tpoint3s *frgs;
    // worm holes
    int whnb;
    Tpoints* wh; // points that have a worm hole
    // magic doors
    Tpoints*md; // points that have a magic door.
                // associated data are stored in the lds structure
};

/*======================================================================*/
/*= constructor/destructor                                             =*/

extern Tpdt * pdt_new ();
extern void   pdt_free(Tpdt *ds);

/*======================================================================*/
/*= variable handler                                                   =*/

extern void pdt_var_chgOrAddEated (Tpdt* pdt, char*vn, int v);
extern void pdt_var_chgOrAddCloned(Tpdt* pdt, Cstr vn, int v);

// returns 0 on success and puts the value vn variable into *v.
// Otherwise (vn variable does not exist) it returns 1.
extern int  pdt_var_get(Tpdt* pdt, Cstr vn, int*v);

/*======================================================================*/
/*= miscellaneous                                                      =*/

// if the the worm hole src --> dest exists the function return dest.
// Otherwise it returns the NULL pointer.
Tpoint* pdt_wormhole_get(const Tpdt*pdt, Tpoint src);
void    pdt_wormhole_create(Tpdt*pdt, Tlds*ds, Tpoint src, Tpoint dest);

// Returns the md parameters of the pt square.
// If it not yet exists it is created and attached to the pt square.
// So a successive calls with the same pt return the same object.
Tpoint* pdt_magicdoor_get(const Tpdt*pdt, Tpoint src);
Tsqmd*  pdt_magicdoor_getcreate(Tpdt*pdt,Tlds*ds,Tpoint pt);

/*======================================================================*/
#endif // FILE_PDT_H
/*======================================================================*/
