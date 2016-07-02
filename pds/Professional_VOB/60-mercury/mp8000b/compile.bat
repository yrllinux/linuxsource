@rem call D:\tornado\host\x86-win32\bin\torvars.bat

@rem cd prj_vx8260

@rem set PRJ_DIR=.\

@rem @echo ***********************  make 8260 debug mp8000b **************************

@rem set PRJ_DIR=.\
@rem set BUILD_SPEC=Mp8000bDebug
@rem make  clean
@rem make  -e -S mp8000b.out
@rem copy /Y mp8000b.out ..\..\..\10-Common\version\debug\8260
@rem deflate <mp8000b.out> ..\..\..\10-common\version\debug\8260\mp8000b.z

@rem @echo ***********************  make 8260 release mp8000b **************************

@rem set BUILD_SPEC=Mp8000bRelease
@rem make clean
@rem make -e -S mp8000b.out
@rem copy /Y mp8000b.out ..\..\..\10-Common\version\release\8260
@rem deflate <mp8000b.out> ..\..\..\10-common\version\release\8260\mp8000b.z
@rem make clean
@rem set BUILD_SPEC=
@rem set PRJ_DIR=

@rem cd ..






