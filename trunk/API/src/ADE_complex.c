#include "headers/ADE_complex.h"
#include <math.h>

ADE_CPLX_T ADE_csum(ADE_CPLX_T add1,ADE_CPLX_T add2)
{
    ADE_CPLX_T temp;

    temp.realpart=add1.realpart+add2.realpart;
    temp.imagpart=add1.imagpart+add2.imagpart;

    return temp;

}

ADE_CPLX_T ADE_cdiff(ADE_CPLX_T add1,ADE_CPLX_T add2)
{
    ADE_CPLX_T temp;

    temp.realpart=add1.realpart-add2.realpart;
    temp.imagpart=add1.imagpart-add2.imagpart;

    return temp;

}

ADE_FLOATING_T ADE_cabs(ADE_CPLX_T val)
{
    return sqrt(val.realpart*val.realpart+val.imagpart*val.imagpart);

}

ADE_FLOATING_T ADE_creal(ADE_CPLX_T val)
{
    return val.realpart;
}

ADE_FLOATING_T ADE_cimag(ADE_CPLX_T val)
{
    return val.imagpart;
}

ADE_CPLX_T ADE_cset(ADE_FLOATING_T initreal,ADE_FLOATING_T initimag)
{
     ADE_CPLX_T temp;

    temp.realpart=initreal;
    temp.imagpart=initimag;

    return temp;

}

ADE_CPLX_T ADE_cmult(ADE_CPLX_T fatt1,ADE_CPLX_T fatt2)
{
    ADE_CPLX_T temp;

    temp.realpart=(fatt1.realpart*fatt2.realpart)-(fatt1.imagpart*fatt2.imagpart);
    temp.imagpart=(fatt1.realpart*fatt2.imagpart)+(fatt1.imagpart*fatt2.realpart);

    return temp;

}
