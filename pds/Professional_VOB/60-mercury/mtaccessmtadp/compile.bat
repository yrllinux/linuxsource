@rem rem call D:\tornado\host\x86-win32\bin\torvars.bat

@rem cd prj_vx8260

@rem set PRJ_DIR=.\

@rem @echo ***********************  make 8260 debug mtAccess.a **************************

@rem set BUILD_SPEC=mtaccessmtadpDebug
@rem make clean
@rem make -e -S mtaccessmtadp.out > ..\..\..\10-common/version/CompileInfo/mtaccessmtadp_d.txt
@rem copy /Y mtaccessmtadp.out ..\..\..\10-Common\version\debug\8260\kdvcri.out
@rem deflate <mtaccessmtadp.out> ..\..\..\10-common\version\debug\8260\mtaccessmtadp.z
@rem del mtaccessmtadp.out

@echo ***********************  make 8260 release mtaccessmtadp.out **************************

@rem set BUILD_SPEC=mtaccessmtadpRelease
@rem make clean
@rem make -e -S mtaccessmtadp.out > ..\..\..\10-common/version/CompileInfo/mtaccessmtadp_r.txt
@rem copy /Y mtaccessmtadp.out ..\..\..\10-Common\version\release\8260\kdvcri.out
@rem deflate <mtaccessmtadp.out> ..\..\..\10-common\version\release\8260\mtaccessmtadp.z
@rem del mtaccessmtadp.out
@rem make clean

@rem set BUILD_SPEC=

@rem cd ..\







