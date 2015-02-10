#ifndef _ADE_COMPLEX_H
#define _ADE_COMPLEX_H
#include "headers/ADE_typedefs.h"

ADE_CPLX_T ADE_csum(ADE_CPLX_T add1,ADE_CPLX_T add2);
ADE_CPLX_T ADE_cset(ADE_FLOATING_T initreal,ADE_FLOATING_T initimag);
ADE_CPLX_T ADE_cdiff(ADE_CPLX_T add1,ADE_CPLX_T add2);
ADE_CPLX_T ADE_cmult(ADE_CPLX_T fatt1,ADE_CPLX_T fatt2);
ADE_FLOATING_T ADE_creal(ADE_CPLX_T val);
ADE_FLOATING_T ADE_cimag(ADE_CPLX_T val);
ADE_FLOATING_T ADE_cabs(ADE_CPLX_T val);


#endif // _ADE_COMPLEX_H
