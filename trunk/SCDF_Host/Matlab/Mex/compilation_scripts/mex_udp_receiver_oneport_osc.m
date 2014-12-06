clear all;close all;clc

main_c = 'mex_udp_receiver_oneport_osc.cpp';


if (isunix || ismac)
     separator='/';  
     path_osc_net = [separator,'ip',separator,'posix'];
%      link_lib     = {'socket'};
      lib_extenstion = 'o ';
elseif ispc
     separator='\';
     path_osc_net = [separator,'ip',separator,'win32'];
     link_lib     = {'ws2_32','Winmm'};
     lib_extenstion = 'obj ';
else
    warning('Computer unknown check path separators');
end

 this_dir =['.',separator];
 up_dir   =['..',separator];
 
 mex_main_path   = [up_dir,'src',separator,main_c];
 mex_search_path = [up_dir,up_dir,up_dir,'Shared_C_funcs'];
 mex_obj_path    =[up_dir,'obj'];
 mex_exec_path    =[up_dir,'exec'];
 
 rem_obj_str = [mex_obj_path,separator,'*'];
 rem_exec_str = [mex_exec_path,separator,'*'];
 
 delete(rem_obj_str);
 delete(rem_exec_str);
 
 c_2_obj_file = {[separator,'udp_receiver'],[separator,'osc',separator,'OscTypes'],...
     [separator,'osc',separator,'OscReceivedElements'],[path_osc_net,separator,'NetworkingUtils'],[path_osc_net,separator,'UdpSocket']};
 
  obj_2_exec_file = {[separator,'udp_receiver'],[separator,'OscTypes'],...
     [separator,'OscReceivedElements'],[separator,'NetworkingUtils'],[separator,'UdpSocket']};
 
 mex_obj_str = ['mex -c -DOSC_HOST_LITTLE_ENDIAN ',mex_search_path,c_2_obj_file{1},'.c ',mex_search_path,c_2_obj_file{2},'.cpp ',...
     mex_search_path,c_2_obj_file{3},'.cpp ',mex_search_path,c_2_obj_file{4},'.cpp ',mex_search_path,c_2_obj_file{5},'.cpp ',...
     ' -I',mex_search_path,' -outdir ',mex_obj_path];
 if (isunix || ismac)
      mex_str = ['mex ',mex_main_path,' ',mex_obj_path,obj_2_exec_file{1},'.',lib_extenstion,mex_obj_path,obj_2_exec_file{2},'.',lib_extenstion,...
       mex_obj_path,obj_2_exec_file{3},'.',lib_extenstion,mex_obj_path,obj_2_exec_file{4},'.',lib_extenstion,...
       mex_obj_path,obj_2_exec_file{5},'.',lib_extenstion,' -I',mex_search_path,' -outdir ',mex_exec_path];
 elseif ispc
  mex_str = ['mex ',mex_main_path,' ',mex_obj_path,obj_2_exec_file{1},'.',lib_extenstion,mex_obj_path,obj_2_exec_file{2},'.',lib_extenstion,...
       mex_obj_path,obj_2_exec_file{3},'.',lib_extenstion,mex_obj_path,obj_2_exec_file{4},'.',lib_extenstion,...
       mex_obj_path,obj_2_exec_file{5},'.',lib_extenstion,'-l',link_lib{1},' -l',link_lib{2},' -I',mex_search_path,' -outdir ',mex_exec_path];
 else
     
     warning('Computer unknown!\n');
 end
 
 eval(mex_obj_str);
  eval(mex_str);
