call C:\CCStudio_v3.3\DosRun.bat

echo "===================== compiling lib [dm647] ================"

cd ..\..\..\..\platform\20-cbb\platform\dm647\driver

call compile.bat

cd ..\..\..\..\..\

rem del /F /Q .\Professional_VOB\10-common\lib\release\ti_d647\*.lib
copy /Y .\platform\10-common\lib\cbb\platform\release\dm647\*.lib .\Professional_VOB\10-common\lib\release\ti_d647\

cd .\Professional_VOB\10-common\build\scripts_plt
