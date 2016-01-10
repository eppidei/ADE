@ECHO ON

::SET _P_="\"
::home paths
SET ADE_HOME="%HOMEDRIVE%\WCPYS_win\ADE_branch"
SET ADE_MATLAB_INSTALL_HOME="%HOMEDRIVE%\Programmi\MATLAB\R2013A"
::
SET ADE_API_HOME="%ADE_HOME%\API"
SET ADE_SRC_PATH="%ADE_API_HOME%\src"
SET ADE_LIB_PATH="%ADE_API_HOME%\lib"
::headers search directories
SET ADE_INCLUDE_PATH="%ADE_SRC_PATH%"
SET ADE_FFTW_INCLUDE_PATH="%ADE_LIB_PATH%\fftw-3.3.4\api"
SET ADE_MATLAB_INCLUDE_PATH="%ADE_MATLAB_INSTALL_HOME%\extern\include"
::linker search directoties
SET ADE_CBLAS_PATH="%ADE_LIB_PATH%\cblas\CBLAS\lib"
SET ADE_BLAS_PATH="%ADE_LIB_PATH%\blas"\BLAS"
SET ADE_MEX_PATH="%ADE_MATLAB_INSTALL_HOME%\bin\win64"
::set real environment variables
setx ADE_API_HOME "%ADE_API_HOME%"
setx ADE_SRC_PATH "%ADE_SRC_PATH%"
setx ADE_LIB_PATH "%ADE_LIB_PATH%"
::headers search directories
setx ADE_INCLUDE_PATH "%ADE_INCLUDE_PATH%"
setx ADE_FFTW_INCLUDE_PATH "%ADE_FFTW_INCLUDE_PATH%"
setx ADE_MATLAB_INCLUDE_PATH "%ADE_MATLAB_INCLUDE_PATH%"
::linker search directoties
setx ADE_CBLAS_PATH "%ADE_CBLAS_PATH%"
setx ADE_BLAS_PATH "%ADE_BLAS_PATH%"
setx ADE_MEX_PATH "%ADE_MEX_PATH%"


pause