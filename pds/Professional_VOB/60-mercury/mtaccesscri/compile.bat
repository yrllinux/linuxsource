@rem rem call D:\tornado\host\x86-win32\bin\torvars.bat

@rem cd prj_vx8260

@rem set PRJ_DIR=.\

@rem @echo ***********************  make 8260 debug mtAccess.a **************************

@rem set BUILD_SPEC=MtAccessCriDebug
@rem make clean
@rem make -e -S mtaccesscri.out > ..\..\..\10-common/version/CompileInfo/mtaccesscri_d.txt
@rem copy /Y mtaccesscri.out ..\..\..\10-Common\version\debug\8260\kdvcri.out
@rem deflate <mtaccesscri.out> ..\..\..\10-common\version\debug\8260\mtaccesscri.z
@rem del mtaccesscri.out

@echo ***********************  make 8260 release mtAccesscri.out **************************

@rem set BUILD_SPEC=MtAccessCriRelease
@rem make clean
@rem make -e -S mtaccesscri.out > ..\..\..\10-common/version/CompileInfo/mtaccesscri_r.txt
@rem copy /Y mtaccesscri.out ..\..\..\10-Common\version\release\8260\kdvcri.out
@rem deflate <mtaccesscri.out> ..\..\..\10-common\version\release\8260\mtaccesscri.z
@rem del mtaccesscri.out
@rem make clean

@rem set BUILD_SPEC=

@rem cd ..\







