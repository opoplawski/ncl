/*
 *	$Id: c_pcsetc.c,v 1.1 1997-04-11 17:43:53 haley Exp $
 */
#include <ncarg/ncargC.h>

void c_pcsetc 
#ifdef NeedFuncProto
(
    char *whch,
    char *cval
)
#else
 (whch, cval)
    char *whch;
    char *cval;
#endif
{
    NGstring whch2;
    NGstring cval2;
    int len1, len2;
/*
 * Make sure parameter name is not NULL
 */
    if( !whch ) {
        fprintf( stderr, "c_pcsetc:  illegal parameter name (NULL)\n" );
        return;
    }
    len1 = NGSTRLEN(whch);
    len2 = NGSTRLEN(cval);
    whch2 = NGCstrToFstr(whch,len1);
    cval2 = NGCstrToFstr(cval,len2);
    NGCALLF(pcsetc,PCSETC)(whch2,cval2,len1,len2);
}
