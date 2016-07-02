@rem call D:\tornado\host\x86-win32\bin\torvars.bat

@rem cd prj_vx8260

@rem set PRJ_DIR=.\

@rem @echo   *********************************************
@rem @echo   ******Make Debug Target For Board MMP********
@rem @echo   *********************************************
@rem set BUILD_SPEC=MmpDebug
@rem make clean
@rem make -e -S mmp.out
@rem copy /Y .\mmp.out ..\..\..\10-common\version\debug\8260\mcueqp_mmp.out
@rem deflate <mmp.out> ..\..\..\10-common\version\debug\8260\mcueqp_mmp.z
@rem make clean

@rem @echo   *********************************************
@rem @echo   ******Make Release Target For Board MMP******
@rem @echo   *********************************************
@rem set BUILD_SPEC=MmpRelease
@rem make clean
@rem make -e -S mmp.out
@rem copy /Y .\mmp.out ..\..\..\10-common\version\release\8260\kdvvpu.out
@rem copy /Y .\mmp.out ..\..\..\10-common\version\release\8260\kdvtvwall.out
@rem deflate <mmp.out> ..\..\..\10-common\version\release\8260\kdvvpu.z
@rem copy /Y ..\..\..\10-common\version\release\8260\kdvvpu.z ..\..\..\10-common\version\release\8260\kdvtvwall.z
@rem make clean

@rem @echo   *********************************************
@rem @echo   ******Make Debug Target For Board APU********
@rem @echo   *********************************************
@rem set BUILD_SPEC=ApuDebug
@rem make clean
@rem make -e -S mmp.out
@rem copy /Y .\mmp.out ..\..\..\10-common\version\debug\8260\mcueqp_apu.out
@rem deflate <mmp.out> ..\..\..\10-common\version\debug\8260\mcueqp_apu.z
@rem make clean

@rem @echo   *********************************************
@rem @echo   ******Make Release Target For Board APU******
@rem @echo   *********************************************
@rem set BUILD_SPEC=ApuRelease
@rem make clean
@rem make -e -S mmp.out
@rem copy /Y .\mmp.out ..\..\..\10-common\version\release\8260\kdvapu.out
@rem deflate <mmp.out> ..\..\..\10-common\version\release\8260\kdvapu.z
@rem make clean

@rem set BUILD_SPEC=

@rem cd ..

@rem @if not exist "..\..\10-common\version\config\mmp/" mkdir "..\..\10-common\version\config\mmp"
@rem copy /Y .\conf\brdcfg.ini ..\..\10-common\version\config\mmp\brdcfg.ini
@rem copy /Y .\conf\mcueqp.ini ..\..\10-common\version\config\mmp\mcueqp.ini