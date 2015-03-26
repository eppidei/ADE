#ifndef ADE_NR_UTILS_H_
#define ADE_NR_UTILS_H_
#include "headers/ADE_typedefs.h"

/*static ADE_FLOATING_SP_T sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)
static ADE_FLOATING_DP_T dsqrarg;
#define DSQR(a) ((dsqrarg=(a)) == 0.0 ? 0.0 : dsqrarg*dsqrarg)
static ADE_FLOATING_DP_T dmaxarg1,dmaxarg2;
#define DMAX(a,b) (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ?\
(dmaxarg1) : (dmaxarg2))
static ADE_FLOATING_DP_T dminarg1,dminarg2;
#define DMIN(a,b) (dminarg1=(a),dminarg2=(b),(dminarg1) < (dminarg2) ?\
(dminarg1) : (dminarg2))
static ADE_FLOATING_SP_T maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
(maxarg1) : (maxarg2))
static ADE_FLOATING_SP_T minarg1,minarg2;
#define FMIN(a,b) (minarg1=(a),minarg2=(b),(minarg1) < (minarg2) ?\
(minarg1) : (minarg2))
static ADE_LONG_T lmaxarg1,lmaxarg2;
#define LMAX(a,b) (lmaxarg1=(a),lmaxarg2=(b),(lmaxarg1) > (lmaxarg2) ?\
(lmaxarg1) : (lmaxarg2))
static ADE_LONG_T lminarg1,lminarg2;
#define LMIN(a,b) (lminarg1=(a),lminarg2=(b),(lminarg1) < (lminarg2) ?\
(lminarg1) : (lminarg2))
static ADE_INT32_T imaxarg1,imaxarg2;
#define IMAX(a,b) (imaxarg1=(a),imaxarg2=(b),(imaxarg1) > (imaxarg2) ?\
(imaxarg1) : (imaxarg2))
static ADE_INT32_T iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
(iminarg1) : (iminarg2))
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))*/

ADE_VOID_T nrerror(ADE_CHAR_T error_text[]);
//ADE_FLOATING_SP_T *vector(ADE_LONG_T nl, ADE_LONG_T nh);
ADE_INT32_T *ivector(ADE_LONG_T nl, ADE_LONG_T nh);
/*unsigned ADE_CHAR_T *cvector(ADE_LONG_T nl, ADE_LONG_T nh);
unsigned ADE_LONG_T *lvector(ADE_LONG_T nl, ADE_LONG_T nh);
ADE_FLOATING_DP_T *dvector(ADE_LONG_T nl, ADE_LONG_T nh);
ADE_FLOATING_SP_T **matrix(ADE_LONG_T nrl, ADE_LONG_T nrh, ADE_LONG_T ncl, ADE_LONG_T nch);
ADE_FLOATING_DP_T **dmatrix(ADE_LONG_T nrl, ADE_LONG_T nrh, ADE_LONG_T ncl, ADE_LONG_T nch);
ADE_INT32_T **imatrix(ADE_LONG_T nrl, ADE_LONG_T nrh, ADE_LONG_T ncl, ADE_LONG_T nch);
ADE_FLOATING_SP_T **submatrix(ADE_FLOATING_SP_T **a, ADE_LONG_T oldrl, ADE_LONG_T oldrh, ADE_LONG_T oldcl, ADE_LONG_T oldch,
ADE_LONG_T newrl, ADE_LONG_T newcl);
ADE_FLOATING_SP_T **convert_matrix(ADE_FLOATING_SP_T *a, ADE_LONG_T nrl, ADE_LONG_T nrh, ADE_LONG_T ncl, ADE_LONG_T nch);
ADE_FLOATING_SP_T ***f3tensor(ADE_LONG_T nrl, ADE_LONG_T nrh, ADE_LONG_T ncl, ADE_LONG_T nch, ADE_LONG_T ndl, ADE_LONG_T ndh);*/

//ADE_VOID_T free_vector(ADE_FLOATING_SP_T *v, ADE_LONG_T nl, ADE_LONG_T nh);
ADE_VOID_T free_ivector(ADE_INT32_T *v, ADE_LONG_T nl, ADE_LONG_T nh);
/*ADE_VOID_T free_cvector(unsigned ADE_CHAR_T *v, ADE_LONG_T nl, ADE_LONG_T nh);
ADE_VOID_T free_lvector(unsigned ADE_LONG_T *v, ADE_LONG_T nl, ADE_LONG_T nh);
ADE_VOID_T free_dvector(ADE_FLOATING_DP_T *v, ADE_LONG_T nl, ADE_LONG_T nh);
ADE_VOID_T free_matrix(ADE_FLOATING_SP_T **m, ADE_LONG_T nrl, ADE_LONG_T nrh, ADE_LONG_T ncl, ADE_LONG_T nch);
ADE_VOID_T free_dmatrix(ADE_FLOATING_DP_T **m, ADE_LONG_T nrl, ADE_LONG_T nrh, ADE_LONG_T ncl, ADE_LONG_T nch);
ADE_VOID_T free_imatrix(ADE_INT32_T **m, ADE_LONG_T nrl, ADE_LONG_T nrh, ADE_LONG_T ncl, ADE_LONG_T nch);
ADE_VOID_T free_submatrix(ADE_FLOATING_SP_T **b, ADE_LONG_T nrl, ADE_LONG_T nrh, ADE_LONG_T ncl, ADE_LONG_T nch);
ADE_VOID_T free_convert_matrix(ADE_FLOATING_SP_T **b, ADE_LONG_T nrl, ADE_LONG_T nrh, ADE_LONG_T ncl, ADE_LONG_T nch);
ADE_VOID_T free_f3tensor(ADE_FLOATING_SP_T ***t, ADE_LONG_T nrl, ADE_LONG_T nrh, ADE_LONG_T ncl, ADE_LONG_T nch,
ADE_LONG_T ndl, ADE_LONG_T ndh);*/

#endif /* _NR_UTILS_H_ */


