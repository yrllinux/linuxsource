@echo off
@rem =============vxWorks 8260 version===============

@rem call D:\tornado\host\x86-win32\bin\torvars.bat

@rem set PRJ_DIR=.\
@rem set BUILD_SPEC=PPCEC603gnu
@rem make -e -S h320mtadp.out > ..\..\..\10-common\version\CompileInfo\H320MtAdp_8260_D.txt 
@rem move /Y h320mtadp.out ..\..\..\10-common\version\debug\8260
@rem make clean

set BUILD_SPEC=Release
make -e -S h320mtadp.out > ..\..\..\10-common\version\CompileInfo\H320MtAdp_8260_R.txt 
move /Y h320mtadp.out ..\..\..\10-common\version\Release\8260
make clean

cd ..

echo on