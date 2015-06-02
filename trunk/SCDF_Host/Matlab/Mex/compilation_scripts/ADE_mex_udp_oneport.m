clear all;close all;clc

main_c = 'ADE_mex_udp_oneport.cpp';

 

if (isunix || ismac)
     separator='/';  
     this_dir =['.',separator];
 up_dir   =['..',separator];
     path_osc_net = [separator,'ip',separator,'posix'];
     link_lib     = {'rt','gfortran','fftw3','fftw3f','fftw3_threads','fftw3f_threads'};
     lib_search_path = {[up_dir,up_dir,up_dir,up_dir,'API/lib/cblas/CBLAS/lib'],...
                        [up_dir,up_dir,up_dir,up_dir,'API/lib/blas/BLAS'],...
                        [up_dir,up_dir,up_dir,up_dir,'API/lib/fftw-3.3.4/api']};
      obj_extenstion = 'o ';
elseif ispc
     separator='\';
     this_dir =['.',separator];
 up_dir   =['..',separator];
     path_osc_net = [separator,'ip',separator,'win32'];
     link_lib     = {'ws2_32','Winmm'};
     obj_extenstion = 'obj ';
else
    warning('Computer unknown check path separators');
end


 
 mex_main_path   = [up_dir,'src',separator,main_c];
 Cshared_search_path = [up_dir,up_dir,up_dir,'Shared_C_funcs'];
 ade_search_path = [up_dir,up_dir,up_dir,up_dir,'API/src'];
 mex_obj_path    =[up_dir,'obj'];
 mex_exec_path    =[up_dir,'exec'];
 
 rem_obj_str = [mex_obj_path,separator,'*'];
 rem_exec_str = [mex_exec_path,separator,'*'];
 
 delete(rem_obj_str);
 delete(rem_exec_str);
 
 c_2_obj_file = {[separator,'udp_receiver'],[separator,'osc',separator,'OscTypes'],...
     [separator,'osc',separator,'OscReceivedElements'],[path_osc_net,separator,'NetworkingUtils'],[path_osc_net,separator,'UdpSocket'],...
     [separator,'ADE'],[separator,'ADE_complex'],[separator,'ADE_nrutil'],[separator,'ADE_polyfit'],[separator,'ADE_blas_level3'],...
     [separator,'ADE_Utils'],[separator,'ADE_downsampler'],[separator,'ADE_fir'],[separator,'ADE_blas_level1'],[separator,'ADE_iir'],...
     [separator,'ADE_upsampler'],[separator,'ADE_Bench_Utils'],[separator,'ADE_decimator'],[separator,'ADE_interpolator'],[separator,'ADE_Blow'],...
     [separator,'ADE_Error_Handler'],[separator,'ADE_blas_level2'],[separator,'ADE_fft'],[separator,'ADE_Snap']};
 
  obj_2_exec_file = {[separator,'udp_receiver'],[separator,'OscTypes'],...
     [separator,'OscReceivedElements'],[separator,'NetworkingUtils'],[separator,'UdpSocket'],...
     [separator,'ADE'],[separator,'ADE_complex'],[separator,'ADE_nrutil'],[separator,'ADE_polyfit'],[separator,'ADE_blas_level3'],...
     [separator,'ADE_Utils'],[separator,'ADE_downsampler'],[separator,'ADE_fir'],[separator,'ADE_blas_level1'],[separator,'ADE_iir'],...
     [separator,'ADE_upsampler'],[separator,'ADE_Bench_Utils'],[separator,'ADE_decimator'],[separator,'ADE_interpolator'],[separator,'ADE_Blow'],...
     [separator,'ADE_Error_Handler'],[separator,'ADE_blas_level2'],[separator,'ADE_fft'],[separator,'ADE_Snap']};
 
 mex_obj_str = ['mex -c -DADE_TARGET_TYPE=ADE_PC_NORMAL -DADE_FP_PRECISION=ADE_USE_SINGLE_PREC -DADE_BLAS_IMPLEMENTATION=ADE_USE_BLAS_LIB -DOSC_HOST_LITTLE_ENDIAN CFLAGS="\$CFLAGS -std=c99" ',Cshared_search_path,c_2_obj_file{1},'.c ',Cshared_search_path,c_2_obj_file{2},'.cpp ',...
     Cshared_search_path,c_2_obj_file{3},'.cpp ',Cshared_search_path,c_2_obj_file{4},'.cpp ',Cshared_search_path,c_2_obj_file{5},'.cpp ',...
     ade_search_path,c_2_obj_file{6},'.c ',ade_search_path,c_2_obj_file{7},'.c ',ade_search_path,c_2_obj_file{8},'.c ',...
     ade_search_path,c_2_obj_file{9},'.c ',ade_search_path,c_2_obj_file{10},'.c ',ade_search_path,c_2_obj_file{11},'.c ',...
     ade_search_path,c_2_obj_file{12},'.c ',ade_search_path,c_2_obj_file{13},'.c ',ade_search_path,c_2_obj_file{14},'.c ',...
     ade_search_path,c_2_obj_file{15},'.c ',ade_search_path,c_2_obj_file{16},'.c ',ade_search_path,c_2_obj_file{17},'.c ',...
     ade_search_path,c_2_obj_file{18},'.c ',ade_search_path,c_2_obj_file{19},'.c ',ade_search_path,c_2_obj_file{20},'.c ',...
     ade_search_path,c_2_obj_file{21},'.c ',ade_search_path,c_2_obj_file{22},'.c ',ade_search_path,c_2_obj_file{23},'.c ',...
     ade_search_path,c_2_obj_file{24},'.c ',' -I',Cshared_search_path, ' -I',ade_search_path,' -outdir ',mex_obj_path];
 
 
 if (isunix || ismac)
      mex_str = ['mex -DADE_MEX_PRINT -DADE_TARGET_TYPE=ADE_PC_NORMAL ',mex_main_path,' ',mex_obj_path,obj_2_exec_file{1},'.',obj_extenstion,mex_obj_path,obj_2_exec_file{2},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{3},'.',obj_extenstion,mex_obj_path,obj_2_exec_file{4},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{5},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{6},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{7},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{8},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{9},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{10},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{11},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{12},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{13},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{14},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{15},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{16},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{17},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{18},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{19},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{20},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{21},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{22},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{23},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{24},'.',obj_extenstion,...
       '-l:blas_MEX.a ',...
       '-l:cblas_MEX.a ',...
       ' -l',link_lib{1},...
       ' -l',link_lib{2},...
       ' -l',link_lib{3},...
       ' -l',link_lib{4},...
       ' -l',link_lib{5},...
        ' -l',link_lib{6},...
       ' -L',lib_search_path{1},...
       ' -L',lib_search_path{2},...
        ' -L',lib_search_path{3},...
       ' -I',Cshared_search_path, ' -I',ade_search_path,' -outdir ',mex_exec_path];
 elseif ispc
  mex_str = ['mex ',mex_main_path,' ',mex_obj_path,obj_2_exec_file{1},'.',obj_extenstion,mex_obj_path,obj_2_exec_file{2},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{3},'.',obj_extenstion,mex_obj_path,obj_2_exec_file{4},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{5},'.',obj_extenstion,...
        mex_obj_path,obj_2_exec_file{5},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{6},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{7},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{8},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{9},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{10},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{11},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{12},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{13},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{14},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{15},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{16},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{17},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{18},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{19},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{20},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{21},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{22},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{23},'.',obj_extenstion,...
       mex_obj_path,obj_2_exec_file{24},'.',obj_extenstion,...
       '-l',link_lib{1},' -l',link_lib{2},' -I',Cshared_search_path,' -outdir ',mex_exec_path];
 else
     
     warning('Computer unknown!\n');
 end
 
 eval(mex_obj_str);
 eval(mex_str);
