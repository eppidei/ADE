#ifndef _ADE_TESTBENCH_H
#define _ADE_TESTBENCH_H
#include "headers/ADE_typedefs.h"
#include "headers/ADE_SCDF_Interface.h"




#ifdef __cplusplus
    extern "C" {
#endif

/****************************** INIT METHODS ************************/
ADE_API_RET_T ADE_TestBench_Init( ADE_TESTBENCH_T ** dp_tb);
ADE_VOID_T ADE_TestBench_Release(ADE_TESTBENCH_T * p_tb);
/****************************** GET METHODS **************************/
ADE_API_RET_T ADE_TestBench_GetSensorDataPointer(ADE_TESTBENCH_T * p_tb,ADE_SCDF_Input_Int_T **dp_SensorData);
ADE_API_RET_T ADE_TestBench_GetSensorData(ADE_TESTBENCH_T * p_tb,ADE_SCDF_Input_Int_T *p_SensorData);
ADE_API_RET_T ADE_TestBench_GetAdeHandle(ADE_TESTBENCH_T * p_tb,ADE_T **dp_Handle);
ADE_API_RET_T ADE_TestBench_GetNoIterations(ADE_TESTBENCH_T * p_tb,ADE_INT32_T *p_iterations);
ADE_API_RET_T ADE_TestBench_GetSource(ADE_TESTBENCH_T * p_tb,ADE_SOURCES_T **dp_Source);
ADE_API_RET_T ADE_TestBench_GetOutBuff(ADE_TESTBENCH_T * p_tb,ADE_FLOATING_T **dp_buff);

/****************************** SET METHODS ***************************/
ADE_API_RET_T ADE_TestBench_SetMatExePath(ADE_TESTBENCH_T * p_tb,char *p_MatExePath);
ADE_API_RET_T ADE_TestBench_SetMatScriptPath(ADE_TESTBENCH_T * p_tb,char *p_ScriptPath);
ADE_API_RET_T ADE_TestBench_SetMatlabInputVarname(ADE_TESTBENCH_T * p_tb,ADE_CHAR_T* varname);
ADE_API_RET_T ADE_TestBench_SetMatWsPath(ADE_TESTBENCH_T * p_tb,char *p_MatWsPath);
ADE_API_RET_T ADE_TestBench_SetMatlabFsVarname(ADE_TESTBENCH_T * p_tb,ADE_CHAR_T* varname);
ADE_API_RET_T ADE_TestBench_SetMatlabNoColsVarname(ADE_TESTBENCH_T * p_tb,ADE_CHAR_T* varname);
ADE_API_RET_T ADE_TestBench_SetAlgo2Test(ADE_TESTBENCH_T * p_tb,ADE_UINT32_T Sel_Flag_i);
ADE_API_RET_T ADE_TestBench_SetIterations(ADE_TESTBENCH_T * p_tb,ADE_INT32_T n_iterations);
ADE_API_RET_T ADE_TestBench_SetNoCols(ADE_TESTBENCH_T * p_tb,ADE_INT32_T NoCols);
ADE_API_RET_T ADE_TestBench_SetFs(ADE_TESTBENCH_T * p_tb,ADE_FLOATING_T Fs);
ADE_API_RET_T ADE_TestBench_SetFc(ADE_TESTBENCH_T * p_tb,ADE_FLOATING_T Fc);
ADE_API_RET_T ADE_TestBench_SetGain(ADE_TESTBENCH_T * p_tb,ADE_FLOATING_T gain);
ADE_API_RET_T ADE_TestBench_SetSrcType(ADE_TESTBENCH_T * p_tb,ADE_SOURCES_TYPE_T SrcType);
/********************************** Configure Methods             ***********************************/
ADE_API_RET_T ADE_TestBench_Configure(ADE_TESTBENCH_T * p_tb,ADE_SOURCES_TYPE_T SrcType,ADE_INT32_T Algo_flag, int cols, double fs,double gain,...);
ADE_API_RET_T ADE_TestBench_ConfigureMatlab(ADE_TESTBENCH_T * p_tb,ADE_SOURCES_TYPE_T SrcType,ADE_INT32_T Algo_flag,char* ScriptPath, char* WsPath,char *MatExePath ,char *InBuffName,char *colvarname,char *fsvarname);
/********************************* Processing methods ***********************/
ADE_API_RET_T ADE_TestBench_RunTest(ADE_TESTBENCH_T * p_tb);
ADE_API_RET_T ADE_TestBench_Verify(ADE_TESTBENCH_T * p_tb);
#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_TESTBENCH_H
