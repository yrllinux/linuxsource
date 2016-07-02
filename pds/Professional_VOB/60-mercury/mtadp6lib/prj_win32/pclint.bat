@echo off
rem ++++++++++++++++++++++++++++++++++
rem    					mtadplib
rem ++++++++++++++++++++++++++++++++++

set Export_LintFile=export.lnt
set PclintFullName=%PCLINT_ROOT%\lint-nt.exe
set Extra_option=extra_options.lnt
set Output=mtadp6lib_pclint.log

@for %%c in (*.dsp ) do %PclintFullName% +linebuf %%c >%Export_LintFile%

@if exist %Extra_option% goto extra
%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std.lnt env-vc6.lnt %Export_LintFile% > %Output% 
goto cpproc

:extra
%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std.lnt env-vc6.lnt %Export_LintFile% %Extra_option% > %Output% 

:cpproc
FOR /F "tokens=1,2,7" %%i in  (';findstr /C:"error 900"  %Output%';) do if "%%k" == "0" (goto end)

copy /Y %Output% ..\..\..\10-common\version\pclintinfo\%Output%

:end
echo on

