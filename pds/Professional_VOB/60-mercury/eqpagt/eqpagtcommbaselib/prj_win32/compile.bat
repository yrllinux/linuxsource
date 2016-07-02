@echo ***********************  make win32 debug eqpagtcommbase.lib **************************

@rem If /I  "%1" == "" Or "%1" == "clean" 
nmake /NOLOGO /A /B /f eqpagtcommbase.mak CFG="eqpagtcommbase - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f eqpagtcommbase.mak CFG="eqpagtcommbase - Win32 Debug" /x nmakeError.log


@echo ***********************  make win32 release eqpagtcommbase.lib **************************

@rem If /I  "%1" == "" Or "%1" == "clean" 
nmake /NOLOGO /A /B /f eqpagtcommbase.mak CFG="eqpagtcommbase - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f eqpagtcommbase.mak CFG="eqpagtcommbase - Win32 Release" /x nmakeError.log

cd ..