#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if (ADE_TARGET_TYPE==ADE_PC_MATLAB)
#include "engine.h"
#include "matrix.h"
#endif
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Utils.h"
#include "headers/ADE_Matlab.h"
#include "headers/ADE.h"
#include "headers/ADE_Blow.h"
#include "headers/ADE_Snap.h"
#include "headers/ADE_SCDF_Interface.h"
#include "headers/ADE_TestBench.h"
#include "headers/ADE_Error_Handler.h"
#include "headers/ADE_TestBench.h"
#include <math.h>



int main()
{
FILE *p_fid=NULL;
ADE_UINT32_T algo_flag=BLOW_FLAG;
ADE_T *p_ade=NULL;
ADE_BLOW_T *p_blow=NULL;
ADE_SNAP_T *p_snap=NULL;
ADE_MATLAB_T *p_mat=NULL;
ADE_INT32_T audio_buff_len,n_simul_cycles;
ADE_FLOATING_T audio_fs;
ADE_CHAR_T frame_len_name[32];
ADE_CHAR_T fs_name[32];
ADE_CHAR_T gain_varname[32];
#ifdef ADE_CONFIGURATION_INTERACTIVE
ADE_SOURCES_TYPE_T SrcType=ADE_SOURCES_MATLAB;
char* BlowScriptPath="/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/Main_scripts/blow_fine_control_swlike.m";
char* BlowWsPath="./blow_config_ws.mat";
char* SnapScriptPath=ADE_SNAP_SCRIPT;
char* SnapWsPath=ADE_SNAP_WS;
char *MatExePath=ADE_MATLAB_EXE;
char *BlowInBuffName="audio_left";
char *Blowlenname="Frame_len";
char *BlowFsname="Fs";
char *SnapInBuffName="actual_samples";
char *Snaplenname="frame_len";
char *SnapFsname="Fs";
#else
ADE_SOURCES_TYPE_T SrcType=ADE_SOURCES_SINE;
audio_buff_len=256;
audio_fs=44100;
n_simul_cycles=100;
#endif

ADE_API_RET_T ret;
ADE_TESTBENCH_T *p_tb;

p_fid=fopen("debug.txt","w");

ret=ADE_TestBench_Init(&p_tb);
ADE_CHECK_RET_FAST(ret);

#ifdef ADE_CONFIGURATION_INTERACTIVE
if (algo_flag==BLOW_FLAG)
{
ret= ADE_TestBench_ConfigureMatlab(p_tb,SrcType,algo_flag,BlowScriptPath, BlowWsPath,MatExePath ,BlowInBuffName,Blowlenname,BlowFsname);
ADE_CHECK_RET_FAST(ret);
}
else if (algo_flag==SNAP_FLAG)
{
ret= ADE_TestBench_ConfigureMatlab(p_tb,SrcType,algo_flag,SnapScriptPath, SnapWsPath,MatExePath ,SnapInBuffName,Snaplenname,SnapFsname);
ADE_CHECK_RET_FAST(ret);
}
#else
ret= ADE_TestBench_Configure(p_tb,SrcType,algo_flag, audio_buff_len, audio_fs,(double)1.0 );
ADE_CHECK_RET_FAST(ret);
ret=ADE_TestBench_SetIterations(p_tb,n_simul_cycles);
ADE_CHECK_RET_FAST(ret);
#endif



ret=ADE_TestBench_RunTest(p_tb);
ADE_CHECK_RET_FAST(ret);
ret=ADE_TestBench_Verify(p_tb);
ADE_CHECK_RET_FAST(ret);

if (algo_flag==BLOW_FLAG)
{
ret=ADE_TestBench_GetAdeHandle(p_tb,&p_ade);
ADE_CHECK_RET_FAST(ret);
ret=ADE_GetBlow(p_ade,&p_blow);
ADE_CHECK_RET_FAST(ret);
 ret=ADE_Blow_Print(p_blow, p_fid,"BLOW", "TB");
 ADE_CHECK_RET_FAST(ret);
}

ADE_TestBench_Release(p_tb);
fclose(p_fid);

return 0;
}
