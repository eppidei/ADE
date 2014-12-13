matdir=/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/glnxa64
matinc=/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/extern/include
	

#gcc main.c -o test_mat -I$matinc -leng -lmx -lut -lmat -lmwcpp11compat -lmwresource_core -lmwi18n -lmwmfl_scalar -lmwfl -lmwMATLAB_res -L$matdir
Debug:
	gcc main.c -g -o ./bin/Debug/matlab_test -I$(matinc) -leng -lmx -Xlinker -rpath-link $(matdir) -L$(matdir)