set Export_LintFile=export.lnt
rem set Output_win32=pclint.log
rem set Output_linux=pclint_linux.log
set PclintFullName=%PCLINT_ROOT%\lint-nt.exe

for %%c in (*.dsp ) do %PclintFullName% +linebuf %%c >%Export_LintFile% && set Output=pclint(%%c).log

%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std.lnt env-vc6.lnt %Export_LintFile% > %Output% 
rem %PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std_linux.lnt env-vc6.lnt %Export_LintFile% > %Output_linux%

copy /Y %Output% ..\..\..\10-common\version\pclintinfo\%Output%

rem @echo 按任一键退出
rem pause>nul

