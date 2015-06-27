#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>


#define N_SAMPLES (20*2)

static unsigned long mt[MT_N]; /* the array for the state vector  */
static int mti=MT_N+1; /* mti==MT_N+1 means mt[MT_N] is not initialized */

/* initializes mt[MT_N] with a seed */
void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<MT_N; mti++) {
        mt[mti] =
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (MT_N>key_length ? MT_N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=MT_N) { mt[0] = mt[MT_N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=MT_N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=MT_N) { mt[0] = mt[MT_N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MT_MATRIX_A};
    /* mag01[x] = x * MT_MATRIX_A  for x=0,1 */

    if (mti >= MT_N) { /* generate MT_N words at one time */
        int kk;

        if (mti == MT_N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<MT_N-MT_M;kk++) {
            y = (mt[kk]&MT_UPPER_MASK)|(mt[kk+1]&MT_LOWER_MASK);
            mt[kk] = mt[kk+MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<MT_N-1;kk++) {
            y = (mt[kk]&MT_UPPER_MASK)|(mt[kk+1]&MT_LOWER_MASK);
            mt[kk] = mt[kk+(MT_M-MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[MT_N-1]&MT_UPPER_MASK)|(mt[0]&MT_LOWER_MASK);
        mt[MT_N-1] = mt[MT_M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long genrand_int31(void)
{
    return (long)(genrand_int32()>>1);
}

/* generates a random number on [0,1]-real-interval */
double genrand_real1(void)
{
    return genrand_int32()*(1.0/4294967295.0);
    /* divided by 2^32-1 */
}

/* generates a random number on [0,1)-real-interval */
double genrand_real2(void)
{
    return genrand_int32()*(1.0/4294967296.0);
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double genrand_real3(void)
{
    return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0);
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53(void)
{
    unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6;
    return(a*67108864.0+b)*(1.0/9007199254740992.0);
}
/* These real versions are due to Isaku Wada, 2002/01/09 added */

int main(void)
{
    int i;
    unsigned long mask_msb = 0x80000000;
    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    double dres=0;
    long res = 0;
    unsigned long seed= 5489;
  //  init_by_array(init, length);



//    printf("%u outputs of genrand_int32()\n",N_SAMPLES);
//
//    for (i=0; i<N_SAMPLES; i++) {
//        res = genrand_int32();
//      //printf("%8.8X\n", res);
//      if ( (mask_msb&res) >>31 == 0)
//      {
//          printf("0\n");
//      }
//       else if ( (mask_msb&res) >>31 == 1)
//       {
//           printf("1\n");
//       }
//
//      if (i%5==4) printf("\n");
//    }
init_genrand(seed);
printf("state vector 1:10\n");
for(i=0;i<10;i++)
{

    printf("%u\n ", mt[i]);
}

//    printf("\n%u outputs of genrand_real2()\n",N_SAMPLES);
//    for (i=0; i<N_SAMPLES; i++) {
//        dres = genrand_real2();
//        if ( (i%2)==0 )
//        {
//             printf("%10.8f    %d\n ",dres ,i%2);
//        }
//
//      if (i%5==4) printf("\n");
//    }

// init_genrand(seed);
//    printf("\n%u outputs of genrand_real1()\n",N_SAMPLES);
//    for (i=0; i<N_SAMPLES; i++) {
//        if ( (i%2)==0 )
//        {
//            printf("%10.8f\n ", genrand_real1());
//        }

      //if (i%5==4) printf("\n");
  //  }

    init_genrand(seed);
    printf("\n%u outputs of genrand_int32()>>31\n",N_SAMPLES);
    for (i=0; i<N_SAMPLES; i++) {
        res = genrand_int32();
        if ( (i%2)==0 )
        {
            printf("%u %d\n ", res>>31,i%2);
        }

      //if (i%5==4) printf("\n");
    }
    return 0;
}
