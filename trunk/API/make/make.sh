makedir=`pwd`

rm -fr $makedir/../obj
rm -fr $makedir/../bin
mkdir $makedir/../obj
mkdir $makedir/../bin

srcdir=$makedir/../src
echo "source directory $srcdir"
objdir=$makedir/../obj
echo "obj directory $objdir"
headdir=$srcdir/headers
echo "header directory $headdir"
bindir=$makedir/../bin
prjdir=$makedir/../prjs
libdir=$makedir/../lib
echo "lib directory $libdir"

gcc -c $srcdir/ADE_polyfit.c -I$srcdir -Wall -Wextra -O1 -o $objdir/ADE_polyfit.o
gcc -c $srcdir/ADE_iir.c -I$srcdir -Wall -Wextra -O1 -o $objdir/ADE_iir.o
gcc -c $srcdir/ADE_fir.c -I$srcdir -Wall -Wextra -O1 -o $objdir/ADE_fir.o
gcc -c $srcdir/ADE_Blow.c -I$srcdir -Wall -Wextra -O1 -o $objdir/ADE_Blow.o
gcc -c $srcdir/ADE_blas_level1.c -I$srcdir -Wall -Wextra -O1 -o $objdir/ADE_blas_level1.o
gcc -c $srcdir/ADE.c -I$srcdir -Wall -Wextra -O1 -o $objdir/ADE.o
gcc $prjdir/codeblocks/blas_test/main.c -lm -I$srcdir -L$libdir/blas/BLAS -l:blas_LINUX.a -Wall -Wextra -O3 -o $bindir/ade $objdir/ADE_polyfit.o $objdir/ADE_iir.o $objdir/ADE_fir.o $objdir/ADE_Blow.o $objdir/ADE_blas_level1.o $objdir/ADE.o