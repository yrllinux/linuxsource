use t2test;

delete from configtable;
delete from deviceassigntable;
delete from deviceitemtable;
delete from devicetable;
delete from itemtable;
delete from loadfiletable;
delete from loadfiletypetable;
delete from proceduretable;
delete from testloginnotetable;
delete from testresulttable;

drop table configtable;
drop table deviceassigntable;
drop table deviceitemtable;
drop table devicetable;
drop table itemtable;
drop table loadfiletable;
drop table loadfiletypetable;
drop table proceduretable;
drop table testloginnotetable;
drop table testresulttable;


drop procedure add_deviceAssign;
drop procedure add_loginUser;
drop procedure add_procedure;
drop procedure add_result;
drop procedure end_loginUser;
drop procedure get_configInfo;
drop procedure get_deviceAssign;
drop procedure get_deviceInfo;
drop procedure get_loadFile;
drop procedure get_peopleInTest;
drop procedure get_procedureInfo;
drop procedure get_testResult;
drop procedure get_testResultNum;
drop procedure update_config;
drop procedure update_device;
drop procedure update_loadFile;


use mysql;
drop database t2test;