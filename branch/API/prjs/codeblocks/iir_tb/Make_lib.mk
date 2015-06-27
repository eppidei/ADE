matdir:=/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/glnxa64
matinc:=/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/extern/include
adeinc:=/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/API/src
adelink:=/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/API/lib/ADE
PRJPATH:=$(CURDIR)
CP:=cp
MV:=mv
RM_DIR:=rm -f -r
RM:=rm -f
MKDIR:=mkdir -p
CC:=gcc
CCFLAGS:=-g -Wall -Wextra
MAIN:=main.c
PROJECT_NAME:=iir_tb
	
create_dir_debug :
	$(MKDIR) $(PRJPATH)/bin/Debug
	$(MKDIR) $(PRJPATH)/obj/Debug
#gcc main.c -o test_mat -I$matinc -leng -lmx -lut -lmat -lmwcpp11compat -lmwresource_core -lmwi18n -lmwmfl_scalar -lmwfl -lmwMATLAB_res -L$matdir
Debug: create_dir_debug
	$(CC) $(MAIN) $(CCFLAGS) -o $(PRJPATH)/bin/Debug/$(PROJECT_NAME) -I$(matinc) -I$(adeinc) -leng -lmx -lADE -Xlinker -rpath-link $(matdir) -L$(matdir) -L$(adelink)