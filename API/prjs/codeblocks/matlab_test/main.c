#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#define MAXCHAR (256)
#define WAIT printf("Hit return to continue\n\n");fgetc(stdin);
#define PRINT_ARRAY(x,idx,len,format) fprintf(stdout,"\n"); for(idx=0;idx<len;idx++) { fprintf(stdout, #x "[%u] =" format "\n",idx,x[idx]); };


int main()
{
	Engine *ep;
	char *matpath ="/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab";
	char *scriptpath="/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/iir/";
	char *script_name="iir_test.m";
	mxArray *mextest = NULL, *scalevals=NULL;
	//char buffer[BUFSIZE+1];
	//double time[10] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };
	FILE *script_fid=NULL;
	char filename[MAXCHAR];
    char temp_str[MAXCHAR];
    double gettest[2],*getpr;
    unsigned int i=0;

	memset(filename,'\0',sizeof(filename));
	memset(temp_str,'\0',sizeof(temp_str));
	strcpy(filename,scriptpath);
	strcat(filename,script_name);


	/*
	 * Call engOpen with a NULL string. This starts a MATLAB process
     * on the current host using the command "matlab".
	 */
	if (!(ep = engOpen(matpath))) {
		fprintf(stderr, "\nCan't start MATLAB engine\n");
		return EXIT_FAILURE;
	}


	script_fid=fopen(filename,"r");

	if (script_fid==NULL)
	{
	    return -2;
	}

	while(feof(script_fid)==0)
	{
	    fgets(temp_str,MAXCHAR,script_fid);
	    engEvalString(ep,temp_str);
	    memset(temp_str,'\0',sizeof(temp_str));
	}

	fclose(script_fid);




	mextest = engGetVariable(ep,"sosvec");

	if (mxIsStruct(mextest))
	{
	    scalevals = mxGetField(mextest, 0,"ScaleValues");

        printf("%s\n",scalevals);
	}
	else if (mextest==NULL)
	{
	    printf("ngul\n");
	}

 printf(" %s\n",mxGetClassName(mextest));
getpr=(double*)mxGetData(mextest);
memcpy(gettest,getpr,2*sizeof(double));

PRINT_ARRAY(gettest,i,2,"%f");

	WAIT;

	mxDestroyArray(mextest);





	/*
	 * PART I
	 *
	 * For the first half of this demonstration, send data
	 * to MATLAB, analyze the data, and plot the result.
	 */

	/*
	 * Create a variable for the data
	 */
//	T = mxCreateDoubleMatrix(1, 10, mxREAL);
//	memcpy((void *)mxGetPr(T), (void *)time, sizeof(time));
//	/*
//	 * Place the variable T into the MATLAB workspace
//	 */
//	engPutVariable(ep, "T", T);
//
//	/*
//	 * Evaluate a function of time, distance = (1/2)g.*t.^2
//	 * (g is the acceleration due to gravity)
//	 */
//	engEvalString(ep, "D = .5.*(-9.8).*T.^2;");
//
//	/*
//	 * Plot the result
//	 */
//	engEvalString(ep, "plot(T,D);");
//	engEvalString(ep, "title('Position vs. Time for a falling object');");
//	engEvalString(ep, "xlabel('Time (seconds)');");
//	engEvalString(ep, "ylabel('Position (meters)');");
//
//	/*
//	 * use fgetc() to pause long enough to be
//	 * able to see the plot
//	 */
//	printf("Hit return to continue\n\n");
//	fgetc(stdin);
//	/*
//	 * We're done for Part I! Free memory, close MATLAB figure.
//	 */
//	printf("Done for Part I.\n");
//	mxDestroyArray(T);
//	engEvalString(ep, "close;");
//
//
//	/*
//	 * PART II
//	 *
//	 * For the second half of this demonstration, we will request
//	 * a MATLAB string, which should define a variable X.  MATLAB
//	 * will evaluate the string and create the variable.  We
//	 * will then recover the variable, and determine its type.
//	 */
//
//	/*
//	 * Use engOutputBuffer to capture MATLAB output, so we can
//	 * echo it back.  Ensure first that the buffer is always NULL
//	 * terminated.
//	 */
//
//	buffer[BUFSIZE] = '\0';
//	engOutputBuffer(ep, buffer, BUFSIZE);
//	while (result == NULL) {
//	    char str[BUFSIZE+1];
//	    /*
//	     * Get a string input from the user
//	     */
//	    printf("Enter a MATLAB command to evaluate.  This command should\n");
//	    printf("create a variable X.  This program will then determine\n");
//	    printf("what kind of variable you created.\n");
//	    printf("For example: X = 1:5\n");
//	    printf(">> ");
//
//	    fgets(str, BUFSIZE, stdin);
//
//	    /*
//	     * Evaluate input with engEvalString
//	     */
//	    engEvalString(ep, str);
//
//	    /*
//	     * Echo the output from the command.
//	     */
//	    printf("%s", buffer);
//
//	    /*
//	     * Get result of computation
//	     */
//	    printf("\nRetrieving X...\n");
//	    if ((result = engGetVariable(ep,"X")) == NULL)
//	      printf("Oops! You didn't create a variable X.\n\n");
//	    else {
//		printf("X is class %s\t\n", mxGetClassName(result));
//	    }
//	}
//
//	/*
//	 * We're done! Free memory, close MATLAB engine and exit.
//	 */
//	printf("Done!\n");
//	mxDestroyArray(result);
	engClose(ep);

	return EXIT_SUCCESS;
}
