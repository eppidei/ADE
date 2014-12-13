ifeq ($(OS),Windows_NT)
PRJPATH:=..
CP:=-copy
MV:=-move
RM_DIR:=-rd /S /Q
RM:=-erase /Q
MKDIR:=-mkdir
else
PRJPATH:=$(CURDIR)/..
CP:=cp
MV:=mv
RM_DIR:=rm -f -r
RM:=rm -f
MKDIR:=mkdir -p
endif
NAME:=main
PRGNAME:=ADE
TARGET:=$(PRJPATH)/bin/Debug/$(PRGNAME)
BLASLIB:=blas_LINUX
BLASPATH:=$(PRJPATH)/lib/blas/BLAS
ADELIB:=ADE
ADEPATH:=$(PRJPATH)/lib/ADE
SRCDIR:=$(PRJPATH)/src
LDFLAGS:=-lm -l:$(BLASLIB).a -l$(ADELIB)
LPATH:=$()


ifeq ($(MAKECMDGOALS),debug)
 FLAGS:= -g -Wall -Wextra -pedantic
 INSTDIR=Debug
else ifeq ($(MAKECMDGOALS),release)
 FLAGS:= -Wall -Wextra -pedantic -O2
 INSTDIR=Release
else ifeq ($(MAKECMDGOALS),)
 FLAGS:= -Wall -Wextra -pedantic -O2
 INSTDIR=Release
else ifeq ($(MAKECMDGOALS),all)
 FLAGS:= -Wall -Wextra -pedantic -O2
 INSTDIR=Release
else ifeq ($(MAKECMDGOALS),clean)
else
 $(error "Avaiable target: debug release all clean")
endif



	
$(PRJPATH)/obj/$(INSTDIR)/%.o: $(PRJPATH)/prjs/codeblocks/blas_test/%.c 
	gcc $(FLAGS) -I$(SRCDIR) $< -o $@ -c
$(PRJPATH)/obj/$(INSTDIR)/%.o: $(PRJPATH)/src/%.c 
	gcc $(FLAGS) -I$(SRCDIR) $< -o $@ -c	
	
all : release

MAIN_TEST=$(PRJPATH)/obj/$(INSTDIR)/$(NAME).o

OBJS=   $(PRJPATH)/obj/$(INSTDIR)/ADE_polyfit.o \
	$(PRJPATH)/obj/$(INSTDIR)/ADE_iir.o \
	$(PRJPATH)/obj/$(INSTDIR)/ADE_fir.o \
	$(PRJPATH)/obj/$(INSTDIR)/ADE_blas_level1.o \
	$(PRJPATH)/obj/$(INSTDIR)/ADE_Blow.o \
	$(PRJPATH)/obj/$(INSTDIR)/ADE.o

$(TARGET): $(OBJS)
	gcc -o $@ $(MAIN_TEST) $^ -lm

install-dirs:
	$(MKDIR) "$(PRJPATH)/bin/$(INSTDIR)"
	$(MKDIR) "$(PRJPATH)/obj/$(INSTDIR)"
	
install-lib-dir:
	$(MKDIR) "$(ADEPATH)"


adelib : install-lib-dir $(OBJS)
	ar -rc $(ADEPATH)/libADE.a $(OBJS)
	ranlib $(ADEPATH)/libADE.a
	
debug: install-dirs $(MAIN_TEST) $(TARGET)
	
release : install-dirs adelib $(OBJS) $(MAIN_TEST)
	gcc -o $(PRJPATH)/bin/Release/$(PRGNAME) $(PRJPATH)/obj/Release/$(NAME).o -L$(BLASPATH) -L$(ADEPATH) $(LDFLAGS)

# Clean
	
cleanDebug :
	$(RM_DIR) "$(PRJPATH)/bin/Debug"
	$(RM_DIR) "$(PRJPATH)/obj/Debug"
	
cleanRelease :
	$(RM_DIR) "$(PRJPATH)/bin/Release"
	$(RM_DIR) "$(PRJPATH)/obj/Release"
	$(RM_DIR) "$(ADEPATH)"

clean : cleanDebug cleanRelease
	$(RM_DIR) "$(PRJPATH)/obj"
	$(RM_DIR) "$(PRJPATH)/bin"


 
