@echo 不再编译mtadplib单元测试
@goto Stop

cd SimMcu\\prj_win32
call compile
cd..
cd..

cd SimMt\\prj_win32
call compile
cd..
cd..

cd SimSMcu\\prj_win32
call compile
cd..
cd..

:Stop
