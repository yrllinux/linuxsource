cd clientapp\ospclient
call compile.bat %1
cd ..\..

cd serverapp\ospserver
call compile.bat %1
cd ..\..

pause