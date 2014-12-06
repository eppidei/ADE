clear all;close all;clc

main_c = 'mex_udp_receiver_oneport.c';
c_2_obj_file = 'udp_receiver';
link_lib     = 'WSock32';

if (isunix || ismac)
     separator='/';  
elseif ispc
     separator='\';
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
 
 mex_obj_str = ['mex -c ',mex_search_path,separator,c_2_obj_file,'.c -I',mex_search_path,' -outdir ',mex_obj_path];
 mex_str = ['mex ',mex_main_path,' ',mex_obj_path,separator,c_2_obj_file,'.obj -l',link_lib,' -I',mex_search_path,' -outdir ',mex_exec_path];
 
eval(mex_obj_str);
eval(mex_str);
