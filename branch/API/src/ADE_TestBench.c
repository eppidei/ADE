#include "headers/ADE_TestBench.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_Error_Handler.h"
#include "headers/ADE.h"
#include "headers/ADE_Blow.h"
#include "headers/ADE_Snap.h"
#include "headers/ADE_Sources.h"
#include "headers/ADE_Matlab.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

struct ADE_TESTBENCH_S
{
    /*configuration params*/
    ADE_INT32_T Algo2Test;
    ADE_SOURCES_TYPE_T SrcType;
    ADE_INT32_T n_iterations;
    /* Per standard --test queste info potrebbero anche non essere salvate e passate solo per configurare oggetti interni*/
    ADE_INT32_T n_cols;
    ADE_FLOATING_T fs;
    ADE_FLOATING_DP_T gain;
    ADE_FLOATING_DP_T fc;
    /* Per matlab test ---- queste info potrebbero anche non essere salvate e passate solo per configurare oggetti interni*/
    char p_MatExePath[128];/* ="/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab";*/
    char p_ScriptPath[128];/*="/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/Main_scripts/blow_fine_control_swlike.m";*/
    char p_MatWsPath[128];/*="./blow_config_ws.mat";*/
    char p_input_matvarname[32];
    char n_cols_varname[32];
    char Fsampling_varname[32];
    ADE_MATLAB_T *p_mat;
    ADE_T *p_ade_handle;
    ADE_SOURCES_T  * p_stimulus;
    /* internal data */
    ADE_FLOATING_T *p_total_out_buffer;
    ADE_SCDF_Input_Int_T sensor_data;

};

/************************************ STATIC HEADERS **********************************/
static ADE_API_RET_T ADE_TestBench_SnapPostProc(ADE_TESTBENCH_T * p_tb,ADE_INT32_T n_max_indexes_i,ADE_FLOATING_T *p_pos_event_idx,ADE_FLOATING_T *p_neg_event_idx);


/************************************* INIT METHODS IMPLEMENTATION *******************************/
/*var_args char* filename, char* p_matfname,char *p_matpath */
ADE_API_RET_T ADE_TestBench_Init( ADE_TESTBENCH_T ** dp_tb)
{
    ADE_TESTBENCH_T * p_this=NULL;
    ADE_SIZE_T max_buff_dim= ADE_TB_MAX_OUT_TOTAL_BUFFER;
    ADE_API_RET_T ret_src=ADE_RET_ERROR;
     ADE_API_RET_T ret_ade=ADE_RET_ERROR;
     ADE_API_RET_T ret=ADE_RET_ERROR;



      ADE_BLOW_T *p_blow=NULL;
      ADE_SNAP_T *p_snap=NULL;

    p_this=calloc(1,sizeof(ADE_TESTBENCH_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_TESTBENCH,ADE_METHOD_Init,p_this);

    ret_src=ADE_Sources_Init(&(p_this->p_stimulus));
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Init,ret_src);

    /* ret_ade = ADE_Init(&(p_this->p_ade_handle), Sel_Flag_i);
     ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Init,ret_ade);*/


    p_this->p_total_out_buffer=calloc(1,max_buff_dim);
    ADE_CHECK_MEMALLOC(ADE_CLASS_TESTBENCH,ADE_METHOD_Init,p_this->p_total_out_buffer);

     /*#ifdef ADE_CONFIGURATION_INTERACTIVE

    va_start ( arguments, Sel_Flag_i );
    scriptname=va_arg(arguments ,ADE_CHAR_T*);
    wsname=va_arg(arguments ,ADE_CHAR_T*);
    matpath=va_arg(arguments ,ADE_CHAR_T*);

    va_end(arguments);

     ret= ADE_Matlab_Init(&(p_this->p_mat),scriptname,wsname,matpath );
     ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Init,ret);

     if (Sel_Flag_i==BLOW_FLAG)
     {
        ret=  ADE_GetBlow(p_this->p_ade_handle,&p_blow);
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Init,ret);
        ret= ADE_Blow_SetMatlab(p_blow, p_blow->p_mat);
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Init,ret);
     }
     else if (Sel_Flag_i==SNAP_FLAG)
     {
        ret= ADE_GetBlow(p_this->p_ade_handle, &p_snap);
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Init,ret);
        ret= ADE_Snap_SetMatlab( p_snap, p_snap->p_mat);
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Init,ret);
     }


     #endif */


    *dp_tb=p_this;

    return ADE_RET_SUCCESS;

}

ADE_VOID_T ADE_TestBench_Release(ADE_TESTBENCH_T * p_tb)
{

    ADE_FLOATING_T *p_buff=NULL;
    ADE_API_RET_T ret_buff;
    ADE_SOURCES_T  *p_source=NULL;
    ADE_API_RET_T ret_src;
    ADE_T *p_ade=NULL;
    ADE_API_RET_T ret_ade;
    ADE_API_RET_T ret_alg;
    ADE_UINT32_T active_alg=0;

    //ret_ade=ADE_TestBench_GetAdeHandle(p_tb,&p_ade);

    ret_alg=ADE_GetActiveAlgos(p_tb->p_ade_handle,&active_alg);
    //ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Release,ret_alg);

    //ret_buff=ADE_TestBench_GetOutBuff(p_tb,&p_buff);
    //ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Release,ret_buff);

    //ret_src=ADE_TestBench_GetSource(p_tb,&p_source);
    //ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Release,ret_src);


    //ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Release,ret_ade);

    ADE_CHECKNFREE(p_tb->p_total_out_buffer);
    ADE_Sources_Release(p_tb->p_stimulus);
    ADE_Release(p_tb->p_ade_handle,active_alg);
    if (p_tb->SrcType==ADE_SOURCES_MATLAB)
    {
        ADE_Matlab_Release(p_tb->p_mat);
    }
    ADE_CHECKNFREE(p_tb);
}

/* POLICY 2 extend to old blocks :
1) Get/Set Methods have input checks
2) All accesses to members are made by get/set , if not comment why
3) Processing methods don't check input ,only return codes,as along as they use get/set
4) Il controllo input sul set va fatto solo se il valore è utilizzato direttamente dal metodo, se viene passato solo ad altre classi no
?????? da decidere 5) Il get di membri annidati per ragioni di efficienza viene fatto con una funzione statica di get che però wrappa solo e prende
    il valore indirizzando esplicitamente all'interno delle strutture per evitare troppi get annidati
*/

/************************************CONFIGURE METHODS *******************************************/
/*var_args char* filename, char* p_matfname,char *p_matpath ,cha+ *colvarname,char *fs varname*/
/*cols,fs,gain,fc*/
ADE_API_RET_T ADE_TestBench_ConfigureMatlab(ADE_TESTBENCH_T * p_tb,ADE_SOURCES_TYPE_T SrcType,ADE_INT32_T Algo_flag,char* ScriptPath, char* WsPath,char *MatExePath ,char *InBuffName,char *colvarname,char *fsvarname)
{

    ADE_API_RET_T ret =ADE_RET_ERROR;
    ADE_BLOW_T *p_blow=NULL;
    ADE_SNAP_T *p_snap=NULL;
    ADE_UINT32_T buff_len;
    double n_cols;
    double fs;
     ADE_INT32_T dimallocated ;
    ADE_INT32_T maxdim =ADE_TB_MAX_OUT_TOTAL_BUFFER;
    ADE_INT32_T dec_factor=0;
    ADE_FLOATING_T Fs1,Fs2;
    ADE_SCDF_Input_Int_T *p_sens_data;
    ADE_INT32_T outdim;



    ret = ADE_TestBench_SetAlgo2Test(p_tb,Algo_flag);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
    /*******************INIZIALIZZA ADE **************************/

    /* in realtà andrebbe fatto in init (qui solamente il configure) ma c'è il flag */
    ret  = ADE_Init(&(p_tb->p_ade_handle), Algo_flag);
     ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret );

     /******************************************************/

    ret =ADE_TestBench_SetSrcType(p_tb,SrcType);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);

    if (SrcType==ADE_SOURCES_MATLAB)
    {

        /************************ INIZIALIZZA MATLAB ***************************/

        ret = ADE_TestBench_SetMatExePath( p_tb,MatExePath);
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
        ret = ADE_TestBench_SetMatScriptPath(p_tb,ScriptPath);
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
        ret = ADE_TestBench_SetMatWsPath(p_tb,WsPath);
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
        ret =  ADE_TestBench_SetMatlabInputVarname(p_tb,InBuffName);
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
        ret =   ADE_TestBench_SetMatlabNoColsVarname(p_tb,colvarname);
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
        ret =   ADE_TestBench_SetMatlabFsVarname(p_tb,fsvarname);
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);


        /********************************* INIZIALIZZA E CONFIGURA MATLAB *******************/
             /* in realtà andrebbe fatto in init (qui solamente il configure) ma ci sono i patrametri TO IMPROVE*/
        ret= ADE_Matlab_Init(&(p_tb->p_mat),ScriptPath,WsPath,MatExePath );
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);

        /************************** CONFIGURA LA SORGENT SOURCE  *************/
        ret= ADE_Sources_ConfigureFromMatlab(p_tb->p_stimulus,p_tb->p_mat,p_tb->SrcType,p_tb->p_input_matvarname,p_tb->n_cols_varname,p_tb->Fsampling_varname,NULL);
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
        /****************************** SETTA MATLAB SULLA ALGORITMO *******************/
         ret= ADE_Matlab_GetSize(p_tb->p_mat, p_tb->p_input_matvarname,&buff_len);
            ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);

            ret= ADE_Matlab_GetScalar(p_tb->p_mat, p_tb->n_cols_varname,&n_cols);
            ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);

        if (Algo_flag==BLOW_FLAG)
        {
            ret=ADE_GetBlow(p_tb->p_ade_handle,&p_blow);
             ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
            ret= ADE_Blow_SetMatlab(p_blow,p_tb->p_mat);
             ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);

             /**** questa configurazione mi serve per ora solo per valuatre il dec factor */
                  ret=ADE_TestBench_GetSensorDataPointer(p_tb,&p_sens_data);
                ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);

                /* Get sensor data info from simulated source */
                ret= Ade_Sources_FillSensorData(p_tb->p_stimulus,p_sens_data);
                ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
             ret= ADE_Configure(p_tb->p_ade_handle,BLOW_FLAG,&(p_tb->sensor_data));
             ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
             /***********/

             ret= ADE_Blow_GetFs(p_blow, &Fs1);
             ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);

            ret= ADE_Blow_GetFsOut(p_blow, &Fs2);
            ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);

            dec_factor=Fs2/Fs1;
        }
        else if (Algo_flag==SNAP_FLAG)
        {
            ret=ADE_GetSnap(p_tb->p_ade_handle,&p_snap);
             ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
            ret= ADE_Snap_SetMatlab(p_snap,p_tb->p_mat);
             ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
        }



    }
    else
    {
            ADE_LOG(stderr," Source must be MATLAB\n");
           return ADE_RET_ERROR;

    }

    outdim=(ADE_INT32_T)buff_len/dec_factor*sizeof(ADE_FLOATING_T);

    if (outdim<=maxdim)
    {

        ret= ADE_TestBench_SetIterations(p_tb,(outdim/sizeof(ADE_FLOATING_T)));
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
    }
    else
    {
        ret= ADE_TestBench_SetIterations(p_tb,(ADE_INT32_T)(maxdim/n_cols/sizeof(ADE_FLOATING_T)));
        ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_ConfigureMatlab,ret);
        ADE_LOG(stderr,"Warning :  Allocated buffer dim %d for Matlab Testbench not big enough (required %d) , reducing simulating cycles to %d \n",maxdim,outdim,p_tb->n_iterations);

    }


   return ADE_RET_SUCCESS;

}


ADE_API_RET_T ADE_TestBench_Configure(ADE_TESTBENCH_T * p_tb,ADE_SOURCES_TYPE_T SrcType,ADE_INT32_T Algo_flag, int cols, double fs,double gain,...)
{

    ADE_API_RET_T ret =ADE_RET_ERROR;
    va_list arguments;

    double Fc=0.0;



    ret = ADE_TestBench_SetAlgo2Test(p_tb,Algo_flag);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Configure,ret);
    /*******************INIZIALIZZA ADE **************************/

    /* in realtà andrebbe fatto in init (qui solamente il configure) ma c'è il flag */
    ret  = ADE_Init(&(p_tb->p_ade_handle), Algo_flag);
     ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Configure,ret );

     /******************************************************/

    ret =ADE_TestBench_SetSrcType(p_tb,SrcType);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Configure,ret);


            va_start ( arguments, gain );

            Fc=va_arg(arguments ,double);
            va_end(arguments);

           ret=  ADE_TestBench_SetNoCols(p_tb, cols);
           ret=  ADE_TestBench_SetFs(p_tb,(ADE_FLOATING_T) fs);
           ret=  ADE_TestBench_SetGain(p_tb, (ADE_FLOATING_T)gain);
           if (Fc!=0.0)/* NON FUNZIONA CAPIRE SE POSSIBILE EVITARE FALSI*/
           {
            ret=  ADE_TestBench_SetFc(p_tb, (ADE_FLOATING_T)Fc);
            ret=  ADE_Sources_ConfigureParams(p_tb->p_stimulus,p_tb->SrcType,p_tb->n_cols,p_tb->fs,p_tb->gain,p_tb->fc );
            ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Configure,ret);
           }
           else
           {

              ret=  ADE_Sources_ConfigureParams(p_tb->p_stimulus,p_tb->SrcType,p_tb->n_cols,p_tb->fs,p_tb->gain );
            ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Configure,ret);
            }


    return ADE_RET_SUCCESS;
}
/************************************* GET METHODS IMPLEMENTATION *******************************/
ADE_API_RET_T ADE_TestBench_GetSensorDataPointer(ADE_TESTBENCH_T * p_tb,ADE_SCDF_Input_Int_T **dp_SensorData)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetSensorDataPointer,p_tb);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetSensorDataPointer,dp_SensorData);

    *dp_SensorData=&(p_tb->sensor_data);

    return ADE_RET_SUCCESS;
}
ADE_API_RET_T ADE_TestBench_GetSensorData(ADE_TESTBENCH_T * p_tb,ADE_SCDF_Input_Int_T *p_SensorData)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetSensorData,p_tb);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetSensorData,p_SensorData);

    *p_SensorData=p_tb->sensor_data;

    return ADE_RET_SUCCESS;
}
ADE_API_RET_T ADE_TestBench_GetAdeHandle(ADE_TESTBENCH_T * p_tb,ADE_T **dp_Handle)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetAdeHandle,p_tb);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetAdeHandle,dp_Handle);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetAdeHandle,p_tb->p_ade_handle);

    *dp_Handle=p_tb->p_ade_handle;

    return ADE_RET_SUCCESS;
}
ADE_API_RET_T ADE_TestBench_GetNoIterations(ADE_TESTBENCH_T * p_tb,ADE_INT32_T *p_iterations)
{
    ADE_INT32_T val0=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetNoIterations,p_tb);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetNoIterations,p_iterations);
    ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_TESTBENCH,ADE_METHOD_GetNoIterations,p_tb->n_iterations,"%d",val0);

    *p_iterations=p_tb->n_iterations;
    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_TestBench_GetSource(ADE_TESTBENCH_T * p_tb,ADE_SOURCES_T **dp_Source)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetSource,p_tb);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetSource,dp_Source);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetSource,p_tb->p_stimulus);

    *dp_Source=p_tb->p_stimulus;

    return ADE_RET_SUCCESS;
}
ADE_API_RET_T ADE_TestBench_GetOutBuff(ADE_TESTBENCH_T * p_tb,ADE_FLOATING_T **dp_buff)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetOutBuff,p_tb);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetOutBuff,dp_buff);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_GetOutBuff,p_tb->p_total_out_buffer);

    *dp_buff=p_tb->p_total_out_buffer;

    return ADE_RET_SUCCESS;
}


/************************************* SET METHODS IMPLEMENTATION *******************************/


ADE_API_RET_T ADE_TestBench_SetMatExePath(ADE_TESTBENCH_T * p_tb,char *p_MatExePath)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatExePath,p_tb);
      ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatExePath,p_MatExePath);


    strncpy(p_tb->p_MatExePath,p_MatExePath,sizeof(p_tb->p_MatExePath));
    return ADE_RET_SUCCESS;
}
ADE_API_RET_T ADE_TestBench_SetMatScriptPath(ADE_TESTBENCH_T * p_tb,char *p_ScriptPath)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatScriptPath,p_tb);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatScriptPath,p_ScriptPath);

     strncpy(p_tb->p_ScriptPath,p_ScriptPath,sizeof(p_tb->p_ScriptPath));
    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_TestBench_SetMatWsPath(ADE_TESTBENCH_T * p_tb,char *p_MatWsPath)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatWsPath,p_tb);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatWsPath,p_MatWsPath);

     strncpy(p_tb->p_MatWsPath,p_MatWsPath,sizeof(p_tb->p_ScriptPath));
    return ADE_RET_SUCCESS;
}


ADE_API_RET_T ADE_TestBench_SetAlgo2Test(ADE_TESTBENCH_T * p_tb,ADE_UINT32_T Sel_Flag_i)
{


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetAlgo2Test,p_tb);


   p_tb->Algo2Test=Sel_Flag_i;


    return ADE_RET_SUCCESS;
}



//Varargs ADE_INT32_T num_frames,ADE_SensorType_T,ADE_INT32_T rate;
ADE_API_RET_T ADE_TestBench_SetIterations(ADE_TESTBENCH_T * p_tb,ADE_INT32_T n_iterations)
{

    ADE_INT32_T val0=0;
    ADE_API_RET_T ret = ADE_RET_ERROR;
    double n_cols;
    ADE_INT32_T dimallocated ;
    ADE_INT32_T maxdim =ADE_TB_MAX_OUT_TOTAL_BUFFER;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetIterations,p_tb);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_TESTBENCH,ADE_METHOD_SetIterations,n_iterations,"%d",val0);


    if (p_tb->SrcType!=ADE_SOURCES_MATLAB)
    {
            dimallocated=p_tb->n_cols*sizeof(ADE_FLOATING_T)*n_iterations;
             ADE_CHECK_INTERVAL_G_MIN_LE_MAX(ADE_CLASS_TESTBENCH,ADE_METHOD_SetIterations,dimallocated,"%d",val0,maxdim);
    }


    p_tb->n_iterations=n_iterations;

    return ADE_RET_SUCCESS;


}

ADE_API_RET_T ADE_TestBench_SetSrcType(ADE_TESTBENCH_T * p_tb,ADE_SOURCES_TYPE_T SrcType)
{


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetSrcType,p_tb);

    p_tb->SrcType=SrcType;

    return ADE_RET_SUCCESS;


}

ADE_API_RET_T ADE_TestBench_SetMatlabInputVarname(ADE_TESTBENCH_T * p_tb,ADE_CHAR_T* varname)
{


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatlabInputVarname,p_tb);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatlabInputVarname,varname);

    strncpy(p_tb->p_input_matvarname,varname,sizeof(p_tb->p_input_matvarname));

    return ADE_RET_SUCCESS;


}

ADE_API_RET_T ADE_TestBench_SetMatlabNoColsVarname(ADE_TESTBENCH_T * p_tb,ADE_CHAR_T* varname)
{


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatlabNoColsVarname,p_tb);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatlabNoColsVarname,varname);

    strncpy(p_tb->n_cols_varname,varname,sizeof(p_tb->n_cols_varname));

    return ADE_RET_SUCCESS;


}

ADE_API_RET_T ADE_TestBench_SetMatlabFsVarname(ADE_TESTBENCH_T * p_tb,ADE_CHAR_T* varname)
{


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatlabFsVarname,p_tb);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetMatlabFsVarname,varname);

    strncpy(p_tb->Fsampling_varname,varname,sizeof(p_tb->Fsampling_varname));

    return ADE_RET_SUCCESS;


}

ADE_API_RET_T ADE_TestBench_SetNoCols(ADE_TESTBENCH_T * p_tb,ADE_INT32_T NoCols)
{
    ADE_INT32_T val0 =0;
   /* ADE_INT32_T dimallocated =NoCols*sizeof(ADE_FLOATING_T);
    ADE_INT32_T maxdim =ADE_TB_MAX_OUT_TOTAL_BUFFER;*/

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetNoCols,p_tb);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_TESTBENCH,ADE_METHOD_SetNoCols,NoCols,"%d",val0);
   /* ADE_CHECK_INTERVAL_G_MIN_LE_MAX(ADE_CLASS_TESTBENCH,ADE_METHOD_SetNoCols,dimallocated,"%d",val0,maxdim);*/

    p_tb->n_cols=NoCols;

    return ADE_RET_SUCCESS;


}

ADE_API_RET_T ADE_TestBench_SetFs(ADE_TESTBENCH_T * p_tb,ADE_FLOATING_T Fs)
{

    ADE_FLOATING_T val0 =0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetFs,p_tb);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_TESTBENCH,ADE_METHOD_SetFs,Fs,"%f",val0);

    p_tb->fs=Fs;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_TestBench_SetFc(ADE_TESTBENCH_T * p_tb,ADE_FLOATING_T Fc)
{

    ADE_FLOATING_T val0 =0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetFc,p_tb);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_TESTBENCH,ADE_METHOD_SetFc,Fc,"%f",val0);

    p_tb->fc=Fc;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_TestBench_SetGain(ADE_TESTBENCH_T * p_tb,ADE_FLOATING_T gain)
{

    ADE_FLOATING_T val0 =0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_TESTBENCH,ADE_METHOD_SetGain,p_tb);

    p_tb->gain=gain;

    return ADE_RET_SUCCESS;

}

/*********************Configure Methods*******************/

//ADE_API_RET_T ADE_TestBench_ConfigureStimulus(ADE_TESTBENCH_T * p_tb,ADE_SOURCES_TYPE_T SrcType,ADE_INT32_T n_cols,ADE_FLOATING_T Fsampling,ADE_FLOATING_DP_T gain, ...)
//{
//    va_list arguments;
//    ADE_FLOATING_DP_T Fperiod,FC0=0.0;
//
//    ADE_API_RET_T ret1,ret2,ret3 = ADE_RET_ERROR;
//    ADE_SOURCES_T *p_source = NULL;
//   // ADE_INT32_T n_frames;//inrealtà sono il numero di campioni
//
//    ret3=ADE_TestBench_GetSource(p_tb,&p_source);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_SetStimulus,ret3);
//
//    if ( (SrcType==ADE_SOURCES_SINE) || (SrcType==ADE_SOURCES_COSINE) )
//    {
//        va_start ( arguments, gain );
//        Fperiod=va_arg(arguments ,ADE_FLOATING_DP_T);
//
//
//       ret1= ADE_Sources_ConfigureParams(p_source,SrcType,n_cols,Fsampling,gain,Fperiod);/* Bisogna passare i double sempre al va_arg*/
//       ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_SetStimulus,ret1);
//    }
//    else
//    {
//         ret2= ADE_Sources_ConfigureParams(p_source,SrcType,n_cols,Fsampling,gain,FC0);/* Bisogna passare i double sempre al va_arg*/
//         ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_SetStimulus,ret2);
//    }
//    va_end(arguments);
//
//    return ADE_RET_SUCCESS;
//}
//
//ADE_API_RET_T ADE_TestBench_ConfigureStimulusFromMatlab(ADE_TESTBENCH_T * p_tb)
//{
//
//    ADE_API_RET_T ret1,ret2,ret3 = ADE_RET_ERROR;
//    ADE_SOURCES_T *p_source = NULL;
//    ADE_CHAR_T *Fperiod_varname;
//   // ADE_INT32_T n_frames;//inrealtà sono il numero di campioni
//
//    ret3=ADE_TestBench_GetSource(p_tb,&p_source);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_SetStimulusFromMatlab,ret3);
//
//    ret = ADE_Sources_SetMatlab(p_sources,p_tb->p_mat);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_SetStimulusFromMatlab,ret);
//
//    ADE_API_RET_T ADE_Sources_SetMatlabInBuffName(ADE_SOURCES_T * p_sources,ADE_CHAR_T* varname)
//    ADE_API_RET_T ADE_Sources_SetMatlabInLenVarname(ADE_SOURCES_T * p_sources,ADE_CHAR_T* varname)
//    ADE_API_RET_T ADE_Sources_SetMatlabFsamplingVarname(ADE_SOURCES_T * p_sources,ADE_CHAR_T* varname)
//
//    ret2= ADE_Sources_SetConfigFromMatlab(p_source,p_tb->p_mat,p_tb->SrcType,p_tb->n_cols_varname,p_tb->Fsampling_varname,(double)1.0);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_SetStimulusFromMatlab,ret2);
//
//    return ADE_RET_SUCCESS;
//}

/********************************* Processing methods ***********************/

ADE_API_RET_T ADE_TestBench_RunTest(ADE_TESTBENCH_T * p_tb)
{

    ADE_INT32_T n_simul_cycles;
    ADE_INT32_T i=0,val0=0;
    ADE_API_RET_T ret_snap=ADE_RET_ERROR;
    ADE_API_RET_T ret_blow=ADE_RET_ERROR;
    ADE_API_RET_T ret_gsd=ADE_RET_ERROR;
    ADE_API_RET_T ret_src=ADE_RET_ERROR;
    ADE_API_RET_T ret_src2=ADE_RET_ERROR;
    ADE_API_RET_T ret_ade=ADE_RET_ERROR;
    ADE_API_RET_T ret_ite=ADE_RET_ERROR;
    ADE_API_RET_T ret_alg=ADE_RET_ERROR;
    ADE_API_RET_T ret_conf=ADE_RET_ERROR;
    ADE_API_RET_T ret_sensdata=ADE_RET_ERROR;
    ADE_UINT32_T active_alg;
     ADE_SOURCES_T *p_source = NULL;
  ADE_SCDF_Input_Int_T *p_SensData;
  ADE_SCDF_Output_Int_T *p_out_data;
     ADE_T *p_ade;


   // ADE_INT32_T n_frames;//inrealtà sono il numero di campioni

    ret_src=ADE_TestBench_GetSource(p_tb,&p_source);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_RunTest,ret_src);

    ret_sensdata=ADE_TestBench_GetSensorDataPointer(p_tb,&p_SensData);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_RunTest,ret_sensdata);

    /* Get sensor data info from simulated source */
    ret_gsd= Ade_Sources_FillSensorData(p_source,p_SensData);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_RunTest,ret_gsd);

    ret_ade=ADE_TestBench_GetAdeHandle(p_tb,&p_ade);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_RunTest,ret_ade);

    ret_alg=ADE_GetActiveAlgos(p_ade,&active_alg);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_RunTest,ret_alg);

    /* Configure ADE algo pointers based on sensor data info */
     if (active_alg==SNAP_FLAG)
   {
     ret_conf = ADE_Configure(p_ade,SNAP_FLAG,p_SensData);
     ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_RunTest,ret_conf);
     }
     else if (active_alg==BLOW_FLAG)
     {
        ret_conf = ADE_Configure(p_ade,BLOW_FLAG,p_SensData);
     ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_RunTest,ret_conf);
     }



     ret_ite=ADE_TestBench_GetNoIterations(p_tb,&n_simul_cycles);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_RunTest,ret_ite);




for (i=0;i<n_simul_cycles;i++)
{
   ret_src2 = Ade_Sources_Step(p_source);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_RunTest,ret_src2);

   if (active_alg==SNAP_FLAG)
   {
    ret_snap=ADE_Step(p_ade,SNAP_FLAG,p_SensData);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_RunTest,ret_snap);
    }
    /* con "else" esegue un algo alla volta anche se potenzialmnete possono essere runnati indipendentemente*/
    else if (active_alg==BLOW_FLAG)
    {
    ret_blow=ADE_Step(p_ade,BLOW_FLAG,p_SensData);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_RunTest,ret_blow);
     p_out_data=ADE_GetOutBuff(p_ade,active_alg);
     memcpy(&(p_tb->p_total_out_buffer[i*p_out_data->n_data]),p_out_data->p_data,p_out_data->n_data*sizeof(ADE_FLOATING_T));
   /*  verificare se conviene lasciare buff out len
     ricordarsi che il calcolo del limite simul cycle sul buff allocato di uscita dipende anche dal downsampling*/


    }
    else
    {
        ADE_LOG(stderr,"Unknown algorithm in ADE_TestBench_RunTest\n");
    }



}

 return ADE_RET_SUCCESS;
}




ADE_API_RET_T ADE_TestBench_Verify(ADE_TESTBENCH_T * p_tb)
{

ADE_FLOATING_T *p_pos_event_idx=NULL;
ADE_FLOATING_T *p_neg_event_idx=NULL;
ADE_INT32_T n_max_indexes_i=2;
ADE_T *p_ade;
ADE_UINT32_T active_alg;
ADE_API_RET_T ret_alg=ADE_RET_ERROR;
ADE_API_RET_T ret_ade=ADE_RET_ERROR;
ADE_BLOW_T *p_blow;
ADE_SNAP_T *p_snap;
ADE_API_RET_T ret_blow_mat=ADE_RET_ERROR;
ADE_API_RET_T ret_blow=ADE_RET_ERROR;
ADE_API_RET_T ret_snap=ADE_RET_ERROR;
ADE_MATLAB_T *p_mat_blow=NULL;
ADE_MATLAB_T *p_mat_snap=NULL;
ADE_API_RET_T ret_buff=ADE_RET_ERROR;
ADE_FLOATING_T *p_out_buff=NULL;
ADE_API_RET_T ret_ite=ADE_RET_ERROR;
ADE_INT32_T n_simul_cycles;
ADE_API_RET_T ret_sensdata=ADE_RET_ERROR;
ADE_SCDF_Input_Int_T SensData;
ADE_API_RET_T ret_mat1=ADE_RET_ERROR;
ADE_API_RET_T ret_mat2=ADE_RET_ERROR;
ADE_API_RET_T ret_mat3=ADE_RET_ERROR;
ADE_API_RET_T ret_mat4=ADE_RET_ERROR;
ADE_API_RET_T ret_mat5=ADE_RET_ERROR;
ADE_API_RET_T ret_pp=ADE_RET_ERROR;


ret_ade=ADE_TestBench_GetAdeHandle(p_tb,&p_ade);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_ade);

ret_alg=ADE_GetActiveAlgos(p_ade,&active_alg);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_alg);

ret_sensdata=ADE_TestBench_GetSensorData(p_tb,&SensData);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_sensdata);

 ret_ite=ADE_TestBench_GetNoIterations(p_tb,&n_simul_cycles);
    ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_ite);

#ifdef ADE_CONFIGURATION_INTERACTIVE

if ( ADE_IsBlow(active_alg))
{

ret_alg=ADE_GetBlow(p_ade,&p_blow);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_alg);

ret_buff= ADE_TestBench_GetOutBuff(p_tb,&p_out_buff);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_buff);

ret_blow=ADE_Blow_GetMatlab(p_blow, &p_mat_blow);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_blow);



ret_mat1=ADE_Matlab_PutVarintoWorkspace(p_mat_blow, p_out_buff, "outt", 1, SensData.num_frames*n_simul_cycles, ADE_MATH_REAL);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_mat1);
/*** Run Matlab algorithm****/
ret_mat2=ADE_Matlab_launch_script_segment(p_mat_blow,"Blow");
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_mat2);
ret_mat3=ADE_Matlab_Evaluate_StringnWait(p_mat_blow, "figure;hold on;plot(outt,'or');plot(expanded_pow_iir,'+b');hold off;");
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_mat3);
}
else if (ADE_IsSnap(active_alg))
{

p_pos_event_idx=(ADE_FLOATING_T *)calloc(SensData.num_frames*n_max_indexes_i,sizeof(ADE_FLOATING_T));
p_neg_event_idx=(ADE_FLOATING_T *)calloc(SensData.num_frames*n_max_indexes_i,sizeof(ADE_FLOATING_T));

ret_alg=ADE_GetSnap(p_ade,&p_snap);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_alg);

ret_snap=ADE_Blow_GetMatlab(p_blow, &p_mat_snap);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_snap);

ret_pp=ADE_TestBench_SnapPostProc(p_ade,n_max_indexes_i,p_pos_event_idx,p_neg_event_idx);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_pp);

ret_mat1=ADE_Matlab_PutVarintoWorkspace(p_mat_snap, p_pos_event_idx, "pos_event_idx_C", 1, n_max_indexes_i*n_simul_cycles, ADE_MATH_REAL);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_mat1);
ret_mat2=ADE_Matlab_PutVarintoWorkspace(p_mat_snap, p_neg_event_idx, "neg_event_idx_C", 1, n_max_indexes_i*n_simul_cycles, ADE_MATH_REAL);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_mat2);
/*** Run Matlab algorithm****/
ret_mat3=ADE_Matlab_launch_script_segment(p_mat_snap,"Snap");
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_mat3);
ret_mat4=ADE_Matlab_Evaluate_StringnWait(p_mat_snap, "figure;hold on;plot(pos_event_idx_C,'or');plot(positive_events_idx,'+b');hold off;");
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_mat4);
ret_mat5=ADE_Matlab_Evaluate_StringnWait(p_mat_snap, "figure;hold on;plot(neg_event_idx_C,'or');plot(negative_events_idx,'+b');hold off;");
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_mat5);

free(p_pos_event_idx);
free(p_neg_event_idx);
}

#endif // ADE_CONFIGURATION_INTERACTIVE



return ADE_RET_SUCCESS;

}

/************************************* STATIC METHODS IMPLEMENTATION *******************************/

static ADE_API_RET_T ADE_TestBench_SnapPostProc(ADE_TESTBENCH_T * p_tb,ADE_INT32_T n_max_indexes_i,ADE_FLOATING_T *p_pos_event_idx,ADE_FLOATING_T *p_neg_event_idx)
{

ADE_INT32_T i=0,pos_cnt=0,neg_cnt=0;
ADE_INT32_T cycle_idx=0;
ADE_SNAP_T *p_snap_int=NULL;
ADE_API_RET_T ret_snap=ADE_RET_ERROR;
ADE_API_RET_T ret_ade=ADE_RET_ERROR;
ADE_API_RET_T ret_snap2=ADE_RET_ERROR;
ADE_API_RET_T ret_ite=ADE_RET_ERROR;
ADE_API_RET_T ret_idx=ADE_RET_ERROR;
ADE_API_RET_T ret_blen=ADE_RET_ERROR;
ADE_API_RET_T ret_n_idx=ADE_RET_ERROR;
ADE_INT32_T n_simul_cycles;
ADE_INT32_T *p_snap_idx=NULL;
ADE_INT32_T buff_len ;
ADE_INT32_T  n_found_indexes;
ADE_BOOL_T *p_snaps=NULL;
ADE_T *p_ade;


ret_ade=ADE_TestBench_GetAdeHandle(p_tb,&p_ade);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_Verify,ret_ade);

ret_snap=ADE_GetSnap(p_ade,&p_snap_int);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_SnapPostProc,ret_snap);

ret_ite=ADE_TestBench_GetNoIterations(p_tb,&n_simul_cycles);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_SnapPostProc,ret_ite);


ret_idx=ADE_Snap_GetIndexes(p_snap_int,&p_snap_idx);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_SnapPostProc,ret_idx);

ret_blen= ADE_Snap_GetBuffLength(p_snap_int,&buff_len);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_SnapPostProc,ret_blen);

ret_n_idx= ADE_Snap_GetNoFoundIndexes(p_snap_int,&n_found_indexes);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_SnapPostProc,ret_n_idx);

ret_snap2 = ADE_Snap_GetSnaps(p_snap_int,&p_snaps);
ADE_CHECK_ADERETVAL(ADE_CLASS_TESTBENCH,ADE_METHOD_GetSnaps,ret_snap2);
/* TO DO add INCHECKS */

    for (cycle_idx=0;cycle_idx<n_simul_cycles;cycle_idx++)
    {

        /********SNAP CASE ************/
        for (i=0;i<n_max_indexes_i;i++)
        {
            if (n_found_indexes>0)
            {
                if (p_snaps[i]==true)
                {
                    p_pos_event_idx[pos_cnt]=p_snap_idx[i]+cycle_idx*buff_len;
                    pos_cnt++;
                }
                else if (p_snaps[i]==false)
                {
                    p_neg_event_idx[neg_cnt]=p_snap_idx[i]+cycle_idx*buff_len;
                    neg_cnt++;
                }
            }
        }

    }

    return ADE_RET_SUCCESS;

}



