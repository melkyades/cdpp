PROMPT $p$g
SET TEMP=C:\WINDOWS\TEMP
set FLATDEBUG=1

PATH=C:\WINDOWS;C:\WINDOWS\COMMAND;c:\Progra~1\gcc\bin;
shell=c:\command.com /E:5500
set GCC_EXEC_PREFIX=c:\Progra~1\gcc\lib\gcc-lib
set COMPILER_PATH=c:\Progra~1\gcc\bin;c:\Progra~1\gcc\lib\gcc-lib\i386-mingw32\2.8.1
set C_INCLUDE_PATH=c:\Progra~1\gcc\include
set CPLUS_INCLUDE_PATH=c:\Progra~1\gcc\include\g++;c:\Progra~1\gcc\lib\gcc-lib\i386-mingw32\2.8.1\include;c:\Progra~1\gcc\include
set LIBRARY_PATH=c:\Progra~1\gcc\lib;c:\Progra~1\gcc\lib\gcc-lib\i386-mingw32\2.8.1

del gram.c
make -f makefile.w95 %1 %2 %3 %4 %5 %6 %7 %8 %9
