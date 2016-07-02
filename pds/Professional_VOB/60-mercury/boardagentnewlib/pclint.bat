echo off
cd basicboardagent
call pclint.bat

cd ..\..
cd cri_agent
call pclint.bat

cd ..\..
cd dri_agent
call pclint.bat


cd ..\..
cd imt_agent
call pclint.bat


cd ..\..
cd mmp_agent
call pclint.bat


echo on