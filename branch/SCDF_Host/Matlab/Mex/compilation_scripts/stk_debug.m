clear all;close all;clc

main_c = 'stk_debug.cpp';


if (isunix || ismac)
     separator='/';  
     path_stk_src = [separator,'stk',separator,'src'];
     path_stk_include = [separator,'stk',separator,'include'];
     link_lib     = {''};
elseif ispc
     separator='\';
     path_stk_src = [separator,'stk',separator,'src'];
     path_stk_include = [separator,'stk',separator,'include'];
     link_lib     = {'Winmm'};
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
 
 c_2_obj_file = {[path_stk_src,separator,'Stk'],[path_stk_src,separator,'Clarinet'],[path_stk_src,separator,'DelayL'],...
     [path_stk_src,separator,'OneZero'],[path_stk_src,separator,'Envelope'],[path_stk_src,separator,'SineWave'],...
      [path_stk_src,separator,'Noise']};
 
  obj_2_exec_file = {[separator,'Stk'],[separator,'Clarinet'],[separator,'DelayL'],[separator,'OneZero'],[separator,'Envelope'],[separator,'SineWave'],...
      [separator,'Noise']};
 
 mex_obj_str = ['mex -c -DWIN32 -DNDEBUG -D_CONSOLE -D__WINDOWS_DS__ -D__WINDOWS_MM__ ',mex_search_path,c_2_obj_file{1},'.cpp ',mex_search_path,c_2_obj_file{2},'.cpp ',...
 mex_search_path,c_2_obj_file{3},'.cpp ',...
 mex_search_path,c_2_obj_file{4},'.cpp ',...
  mex_search_path,c_2_obj_file{5},'.cpp ',...
 mex_search_path,c_2_obj_file{6},'.cpp ',...
 mex_search_path,c_2_obj_file{7},'.cpp ',...
 ' -I',[mex_search_path,path_stk_include],' -outdir ',mex_obj_path];
 mex_str = ['mex ',mex_main_path,' ',mex_obj_path,obj_2_exec_file{1},'.obj ',mex_obj_path,obj_2_exec_file{2},'.obj ',...
     mex_obj_path,obj_2_exec_file{3},'.obj ',...
     mex_obj_path,obj_2_exec_file{4},'.obj ',...
     mex_obj_path,obj_2_exec_file{5},'.obj ',...
     mex_obj_path,obj_2_exec_file{6},'.obj ',...
     mex_obj_path,obj_2_exec_file{7},'.obj ',...
     '-l',link_lib{1},' -I',mex_search_path,' -outdir ',mex_exec_path];
 
 eval(mex_obj_str);
  eval(mex_str);
 
 




