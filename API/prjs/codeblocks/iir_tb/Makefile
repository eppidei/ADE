matdir:=/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/glnxa64
matinc:=/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/extern/include
adeinc:=/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/API/src
# adelink:=/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/API/lib/ADE
blasdir:=/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/API/lib/blas/BLAS
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
	
OBJ=    $(PRJPATH)/obj/Debug/ADE_iir.o \
	$(PRJPATH)/obj/Debug/ADE_blas_level1.o \
	$(PRJPATH)/obj/Debug/ADE_Matlab.o

$(PRJPATH)/obj/Debug/%.o: $(adeinc)/%.c 
	$(CC) $(CCFLAGS) -I$(adeinc) -I$(matinc) $< -o $@ -c
create_dir_debug :
	$(MKDIR) $(PRJPATH)/bin/Debug
	$(MKDIR) $(PRJPATH)/obj/Debug
delete_dir_debug :
	$(RM_DIR) $(PRJPATH)/bin/Debug
	$(RM_DIR) $(PRJPATH)/obj/Debug
go: $(OBJ)
	$(CC) $(MAIN) $(CCFLAGS) -o $(PRJPATH)/bin/Debug/$(PROJECT_NAME) $^ -I$(matinc) -I$(adeinc) -l:blas_LINUX.a -leng -lmx -lmat -Xlinker -rpath-link $(matdir) -L$(matdir) -L$(blasdir) 
Debug: delete_dir_debug create_dir_debug go

clean : delete_dir_debug
	
	
