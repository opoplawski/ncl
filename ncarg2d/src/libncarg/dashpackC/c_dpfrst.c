/*
 *	$Id: c_dpfrst.c,v 1.1 1997-04-11 17:41:42 haley Exp $
 */
#include <ncarg/ncargC.h>

void c_dpfrst
#ifdef NeedFuncProto
(
    float xcpu,
    float ycpu
)
#else
(xcpu,ycpu)
    float xcpu;
    float ycpu;
#endif
{
    float xcpu2, ycpu2;

    xcpu2 = xcpu;
    ycpu2 = ycpu;
    NGCALLF(dpfrst,DPFRST)(&xcpu2,&ycpu2);
}
