BLASSRC:=cblas
PRJPATH:=$(CURDIR)
ifeq ($(BLASSRC),blas)
	BLAS_LSEARCHDIR:=-L/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/API/lib/blas/BLAS
	BLASLIB:=-l:blas_LINUX.a
	BLASIMPLDEF:=-DADE_BLAS_IMPLEMENTATION=1
else
	ifeq ($(BLASSRC),cblas)
		BLAS_LSEARCHDIR:=-L/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/API/lib/cblas/CBLAS/lib -L/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/API/lib/blas/BLAS
		BLASLIB:=-l:cblas_LINUX.a -l:blas_LINUX.a
		BLASIMPLDEF:=-DADE_BLAS_IMPLEMENTATION=2
	else
		ifeq ($(BLASSRC),openblas)
			BLAS_ISEARCHDIR:=-I/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/API/lib/xianyi-OpenBLAS-v0.2.13-0-g51ce5ef/xianyi-OpenBLAS-51ce5ef
			BLAS_LSEARCHDIR:=-L/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/API/lib/xianyi-OpenBLAS-v0.2.13-0-g51ce5ef/xianyi-OpenBLAS-51ce5ef
			BLASLIB:=-l:libopenblas.a
			BLASIMPLDEF:=-DADE_BLAS_IMPLEMENTATION=3
		endif
	endif
endif
COMMONLIBS:=-lgfortran -lm -lrt
ADE_ISEARCHDIR:=/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/API/src
CP:=cp
MV:=mv
RM_DIR:=rm -f -r
RM:=rm -f
MKDIR:=mkdir -p
CC:=gcc
CCFLAGS:=-g -Wall -Wextra
MAIN:=main.c
PROJECT_NAME:=Blas_benchmark
	
OBJ=    $(PRJPATH)/obj/Debug/ADE_Bench_Utils.o \
	$(PRJPATH)/obj/Debug/ADE_Utils.o \
	$(PRJPATH)/obj/Debug/ADE_blas_level1.o \
	$(PRJPATH)/obj/Debug/ADE_blas_level2.o \
	$(PRJPATH)/obj/Debug/ADE_blas_level3.o \
	$(PRJPATH)/obj/Debug/ADE_complex.o

$(PRJPATH)/obj/Debug/%.o: $(ADE_ISEARCHDIR)/%.c 
	$(CC) $(CCFLAGS) $(BLASIMPLDEF) -I$(ADE_ISEARCHDIR) $(BLAS_ISEARCHDIR) $< -o $@ -c -lm
create_dir_debug :
	$(MKDIR) $(PRJPATH)/bin/Debug
	$(MKDIR) $(PRJPATH)/obj/Debug
delete_dir_debug :
	$(RM_DIR) $(PRJPATH)/bin/Debug
	$(RM_DIR) $(PRJPATH)/obj/Debug
go: $(OBJ)
	$(CC) $(MAIN) $(CCFLAGS) $(BLASIMPLDEF) -o $(PRJPATH)/bin/Debug/$(PROJECT_NAME) $^ -I$(ADE_ISEARCHDIR) $(BLASLIB) $(BLAS_LSEARCHDIR) $(COMMONLIBS)
Debug: delete_dir_debug create_dir_debug go

clean : delete_dir_debug
	
	
