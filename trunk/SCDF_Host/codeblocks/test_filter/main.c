#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int filter_DII_T (double *in, double *out, double *a, unsigned int order, double *b, double *state,unsigned int len_frame)
{
    unsigned int i=0,k=0;

    for (k=0;k<len_frame;k++)
    {


    out[k] = b[0]*(in[k])+state[0];

        for (i=0;i<(order);i++)
        {
            state[i]=b[i+1]*(in[k])+state[i+1]-a[i+1]*(out[k]);
        }

    }

    return 0;

}


int main()
{

    double a[3]={1.0,0.99978,-0.9898};
    double b[3]={0.878,0.888,0.456};
    double state[3]={0.0,0.0,0.0};
    double in[12]={1,2,3,4,5,6,7,8,9,0.1,-0.1,0.1};
    double out[12];

    memset(out,0,sizeof(out));

    unsigned int frame_len = 12,i=0;

//    for(i=0;i<3;i++)
//    {
//        filter_DII_T (&in[i*frame_len], &out[i*frame_len], a, 2, b, state,frame_len);
//    }

     filter_DII_T (in, out, a, 2, b, state,frame_len);

    for(i=0;i<12;i++)
    {
        printf("%f\n",out[i]);
    }
    return 0;
}
