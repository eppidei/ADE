#!/bin/bash
ADE_ISUNIX='true';
_P_=/;
if [ $ADE_ISUNIX = 'false' ];
then
  _P_=\;
fi
#home paths
export ADE_HOME=/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2
export ADE_MATLAB_INSTALL_HOME=/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A
#
export ADE_API_HOME=${ADE_HOME}${_P_}API
export ADE_SRC_PATH=${ADE_API_HOME}${_P_}src
export ADE_LIB_PATH=${ADE_API_HOME}${_P_}lib
#headers search directories
export ADE_INCLUDE_PATH=${ADE_SRC_PATH}
export ADE_FFTW_INCLUDE_PATH=${ADE_LIB_PATH}${_P_}fftw-3.3.4${_P_}api
export ADE_MATLAB_INCLUDE_PATH=${ADE_MATLAB_INSTALL_HOME}${_P_}extern${_P_}include
#linker search directoties
export ADE_CBLAS_PATH=${ADE_LIB_PATH}${_P_}cblas${_P_}CBLAS${_P_}lib${_P_}cblas_LINUX.a
export ADE_CBLAS_PATH=${ADE_LIB_PATH}${_P_}blas${_P_}BLAS${_P_}blas_LINUX.a
export ADE_MEX_PATH=${ADE_MATLAB_INSTALL_HOME}${_P_}bin${_P_}glnxa64
