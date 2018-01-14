/*************************************************************************
 *   $cours: lex/yacc
 * $section: projet
 *      $Id: pdt.c 440 2017-10-20 16:34:42Z ia $
 * $HeadURL: svn://lunix120.ensiie.fr/ia/cours/lex-yacc/src/labyrinthe/labgen/pdt.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
*************************************************************************/

#include <stdlib.h>

#include "top.h"
#include "pdt.h"

/*======================================================================*/
/*= constructor/destructor                                             =*/

extern Tpdt * pdt_new ()
{
    Tpdt* pdt = u_malloc0( sizeof(*pdt) );
    pdt->vars = vars_new();
    pdt->fvars = vars_new();
    pdt->fvars_index = 0;
	pdt->frgs = pt3s_new();
    pdt->out  = pts_new();
    pdt->md   = pts_new();
    pdt->wh   = pts_new();
    return pdt;
}

extern void   pdt_free(Tpdt *pdt)
{
    for(int i = 0; i < PDT_FVARSSIZE && pdt->fvars_names[i] != NULL; i++) {
        free(pdt->fvars_names[i]);
    }

    vars_free( pdt->vars );
    vars_free( pdt->fvars );
	pt3s_free( pdt->frgs );
    pts_free( pdt->in );
    pts_free( pdt->out );
    pts_free( pdt->md );
    pts_free( pdt->wh );
    free( pdt );
}

/*======================================================================*/
/*= variable handler                                                   =*/

extern void pdt_var_chgOrAddEated(Tpdt* pdt, char*vn, int v)
{ vars_chgOrAddEated(pdt->vars,vn,v); }

extern void pdt_var_chgOrAddCloned(Tpdt* pdt, Cstr vn, int v)
{ vars_chgOrAddCloned(pdt->vars,vn,v); }

// returns 0 on sucess and puts the value vn variable into *v.
// Otherwise (vn variable does not exist) it returns 1.
extern int  pdt_var_get(Tpdt* pdt, Cstr vn, int*v)
{
    Tvar* found = vars_get(pdt->vars,vn);
    if ( found!=0 ) {
        *v = found->val;
    } 
    return found==0;
}

/*======================================================================*/
/*= miscellaneous                                                      =*/


/* Returns NULL or a pointer to the existing source point of the wh */
Tpoint* pdt_wormhole_get(const Tpdt*pdt, Tpoint src) {
    int i;
    Tpoint *curr;

    for(i=0 ; i<pdt->wh->nb ; i+=1) {
        curr = &pdt->wh->t[i];
        if(pt_cmp(curr, &src) == 0)
            return curr;
    }

    return NULL;
}

void pdt_wormhole_create(Tpdt*pdt, Tlds*ds, Tpoint src, Tpoint dest)
{
    Tpoint *p;

    if(!(p = pdt_wormhole_get(pdt, src))) {
        ds->squares[src.x][src.y].opt    = LDS_OptWH;
        pts_app_pt( pdt->wh, src);
        p = &src;
    }

    /* We finally update the destination */
    ds->squares[p->x][p->y].sq_whd = dest;
}

/* Returns NULL or a pointer to the existing source point of the magic door */
Tpoint *pdt_magicdoor_get(const Tpdt*pdt, Tpoint src) {
    int i;
    Tpoint *curr;

    for(i=0 ; i<pdt->md->nb ; i+=1) {
        curr = &pdt->md->t[i];
        if(pt_cmp(curr, &src) == 0)
            return curr;
    }

    return NULL;
}
// Returns the md parameters of the pt square.
// If it not yet exists it is created and attached to the pt square.
// So a successive calls with the same pt return the same object.
Tsqmd*  pdt_magicdoor_getcreate(Tpdt*pdt,Tlds*ds,Tpoint pt)
{
    Tpoint *p;

    if(!(p = pdt_magicdoor_get(pdt, pt))) {
        ds->squares[pt.x][pt.y].opt    = LDS_OptMD;
        ds->squares[pt.x][pt.y].sq_mdp = lds_sqmd_new(pt);
        pts_app_pt( pdt->md, pt);

        p = &pt;
    }

    return ds->squares[p->x][p->y].sq_mdp;
}

/*======================================================================*/
