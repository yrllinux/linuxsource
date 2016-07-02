@rem rem call D:\tornado\host\x86-win32\bin\torvars.bat

@rem cd prj_vx8260

@rem set PRJ_DIR=.\

@rem @rem @echo ***********************  make 8260 debug mtaccessdri.a **************************

@rem @rem set BUILD_SPEC=MpDriDebug
@rem @rem make  clean
@rem @rem make  -e -S mtaccessdri.out > ..\..\..\10-common/version/CompileInfo/mtaccessdri_d.txt
@rem @rem copy /Y mtaccessdri.out ..\..\..\10-Common\version\debug\8260\kdvdri.out
@rem @rem deflate <mtaccessdri.out> ..\..\..\10-common\version\debug\8260\mtaccessdri.z
@rem @rem del mtaccessdri.out


@rem @echo ***********************  make 8260 release mtaccessdri.a **************************

@rem set BUILD_SPEC=MpDriRelease
@rem make clean
@rem make -e -S mtaccessdri.out > ..\..\..\10-common/version/CompileInfo/mtaccessdri_r.txt
@rem copy /Y mtaccessdri.out ..\..\..\10-Common\version\release\8260\kdvdri.out
@rem deflate <mtaccessdri.out> ..\..\..\10-common\version\release\8260\kdvdri.z
@rem del mtaccessdri.out
@rem make clean

@rem set BUILD_SPEC=

@rem cd ..\
