create database t2test;

use t2test;

CREATE TABLE configtable
(
    DeviceType      int                 NOT NULL,
    FirstIP         varchar(32)         not null,
    FirstMACOne     varchar(32)         not null,
    FirstMacTwo     varchar(32)         default null,
    `Count`         int                 NOT NULL,
    SerialLength    int                 default 9,
    ProductDate     date                NOT NULL,

CONSTRAINT PK_configtable_DeviceType    PRIMARY KEY (DeviceType)
);

INSERT INTO configtable VALUES (116,'122.16.5.69','08:00:22:33:55:99','',20,9,'2007-1-1');


CREATE TABLE deviceassigntable
(
    SerialNo        varchar(32)         NOT NULL,
    DeviceType      int                 NOT NULL,
    IP              varchar(32)         NOT NULL,
    MACOne          varchar(32)         NOT NULL,
    MACTwo          varchar(32)         default NULL,
    TestDateTime    datetime            NOT NULL,

CONSTRAINT PK_deviceassigntable_SerialNo    PRIMARY KEY (SerialNo)
);


CREATE TABLE deviceitemtable (
    DeviceType      int                  NOT NULL,
    TestItem        int                  NOT NULL
);

INSERT INTO deviceitemtable VALUES (112,1),(112,2),(112,3),(112,4),(112,5),(112,6),(112,7),(112,8),(112,9),(112,10),(112,13),(112,14),(112,15),(160,2),(160,3),(160,4),(160,5),(160,6),(160,7),(160,10),(160,13),(160,14),(160,15),(113,2),(113,3),(113,4),(113,5),(113,6),(113,7),(113,8),(113,9),(113,10),(113,11),(113,13),(113,14),(113,15),(114,2),(114,3),(114,4),(114,5),(114,9),(114,10),(114,11),(114,13),(114,14),(114,15),(115,1),(115,2),(115,3),(115,4),(115,5),(115,6),(115,7),(115,8),(115,9),(115,10),(115,12),(115,13),(115,14),(115,15),(116,1),(116,2),(116,3),(116,4),(116,5),(116,6),(116,7),(116,8),(116,9),(116,10),(116,13),(116,14),(116,15);


CREATE TABLE devicetable (
    DeviceType      int                 NOT NULL,
    DeviceDesc      varchar(32)         default NULL,
    HardwareVersion int                 NOT NULL,
    E2promInfo      int                 NOT NULL,
    E2promFlag      int                 NOT NULL,
    Abort           int                 default 0,
    ServerPath      varchar(100)        default 'c:\\',
    OperatorPath    varchar(100)        default NULL,
    TestDelay       int                 default 2,
    `Resume`        int                 default 1,

CONSTRAINT PK_devicetable_DeviceType    PRIMARY KEY (DeviceType)
);

INSERT INTO devicetable VALUES (112,'TS6610',1,3,0,0,'C:\\','',2,1),(113,'TS5210',1,3,0,0,'C:\\','',2,1),(114,'V5',1,3,0,0,'C:\\','',2,1),(115,'TS6610E',1,3,0,0,'C:\\','',2,1),(116,'TS6210',1,3,0,0,'C:\\','',2,1),(160,'TS3210',1,3,0,0,'C:\\','',2,1);


CREATE TABLE itemtable (
    TestItem        int                 NOT NULL,
    ItemDesc        varchar(32)         default NULL,

CONSTRAINT PK_itemtable_TestItem    PRIMARY KEY (TestItem)
);

INSERT INTO itemtable VALUES (1,'FXO环路端口测试'),(2,'串口测试'),(3,'以太网口测试'),(4,'USB测试'),(5,'音频测试'),(6,'外置摄像头输入VGA输出测试'),(7,'外置摄像头输入C端子输出测试'),(8,'内置摄像头输入VGA输出测试'),(9,'内置摄像头输入C端子输出测试'),(10,'扬声器'),(11,'麦克风'),(12,'摄像头转动测试'),(13,'时钟芯片测试'),(14,'指示灯测试'),(15,'红外线测试');

CREATE TABLE loadfiletable (
    DeviceType      int                 NOT NULL,
    LoadFileType    int                 NOT NULL,
    FileName        varchar(32)         NOT NULL
);

INSERT INTO loadfiletable VALUES (160,1,'ts3210.bin'),(113,1,'ts5210.bin'),(112,1,'ts6610.bin'),(114,1,'v5.bin'),(115,1,'ts6610e.bin'),(116,1,'ts6610.bin');


CREATE TABLE loadfiletypetable (
    LoadFileType    int                 NOT NULL,
    FileDesc        varchar(32)         default NULL,

CONSTRAINT PK_loadfiletypetable_LoadFileType    PRIMARY KEY (LoadFileType)
);

INSERT INTO loadfiletypetable VALUES (1,'SysFile');


CREATE TABLE proceduretable (
    `Procedure`     int                 NOT NULL,
    TestItem        int                 NOT NULL,
    `Order`         int                 NOT NULL
);

INSERT INTO proceduretable VALUES (1,0,0),(2,1,1),(2,2,2),(2,3,3),(2,4,4),(2,5,5),(2,6,6),(2,7,7),(2,8,8),(2,9,9),(2,10,10),(2,11,11),(2,12,12),(2,13,13),(2,14,14),(2,15,15);


CREATE TABLE testloginnotetable (
    LoginNo         int                 auto_increment,
    `Procedure`     int                 NOT NULL,
    EmployeeNo      varchar(32)         NOT NULL,
    LoginInDateTime datetime            default NULL,
    LoginOutDateTime datetime           default NULL,

CONSTRAINT PK_testloginnotetable_LoginNo    PRIMARY KEY (LoginNo)
);


CREATE TABLE testresulttable (
    NoteNo          int                 auto_increment,
    SerialNo        varchar(32)         NOT NULL,
    TestItem        int                 NOT NULL,
    Result          int                 NOT NULL,

CONSTRAINT PK_testresulttable_NoteNo    PRIMARY KEY (NoteNo)
);



DELIMITER $$

DROP PROCEDURE IF EXISTS add_deviceAssign $$
CREATE PROCEDURE add_deviceAssign
(
  in p_serialno     varchar(32),
  in p_devicetype   int,
  in p_ip           varchar(32),
  in p_macone       varchar(32),
  in p_mactwo       varchar(32),
  in p_testdatetime datetime
)
BEGIN
  insert into deviceassigntable (SerialNo,DeviceType,IP,MACOne,MACTwo,TestDateTime)
      values(p_serialno,p_devicetype,p_ip,p_macone,p_mactwo,p_testdatetime);
END $$

DROP PROCEDURE IF EXISTS add_loginUser $$
CREATE PROCEDURE add_loginUser
(
  in  p_procedure           int,
  in  p_employeeno          varchar(32),
  in  p_loginintime         datetime
)
BEGIN
  insert into testloginnotetable (testloginnotetable.Procedure, EmployeeNo, LoginInDateTime)
      values(p_procedure,p_employeeno,p_loginintime);
END $$

DROP PROCEDURE IF EXISTS add_procedure $$
CREATE PROCEDURE add_procedure
(
  in p_procedure    int,
  in p_testitem     int,
  in p_order        int
)
BEGIN
  insert into proceduretable(proceduretable.Procedure,TestItem,proceduretable.Order) values(p_procedure,p_testitem,p_order);
END $$

DROP PROCEDURE IF EXISTS add_result $$
CREATE PROCEDURE add_result
(
    in p_serialNo       varchar(32),
    in p_testItem       int,
    in p_result         int
)
BEGIN
    INSERT INTO testresulttable(SerialNo,TestItem,Result) VALUES(p_serialNo,p_testItem,p_result);
END $$

DROP PROCEDURE IF EXISTS end_loginUser $$
CREATE PROCEDURE end_loginUser
(
  in  p_employeeno          varchar(32),
  in  p_loginouttime        datetime
)
BEGIN
  update testloginnotetable set LoginOutDateTime=p_loginouttime where LoginOutDateTime is NULL and EmployeeNo=p_employeeno;
END $$

DROP PROCEDURE IF EXISTS get_configInfo $$
CREATE PROCEDURE get_configInfo()
BEGIN
   select * from configtable;
END $$

DROP PROCEDURE IF EXISTS get_deviceAssign $$
CREATE PROCEDURE get_deviceAssign
(
  in p_condition int,
  in p_starttime datetime,
  in p_endtime   datetime,
  in p_devicetype int,
  in p_testresult int
)
BEGIN
  if p_condition=0 then
    select * from deviceassigntable where TestDateTime>=p_starttime and TestDateTime<=p_endtime order by TestDateTime;
  elseif p_condition=1 then
    select * from deviceassigntable where DeviceType=p_devicetype and TestDateTime>=p_starttime and TestDateTime<=p_endtime order by TestDateTime;
  elseif p_condition=2 then
    if p_testresult=1 then
      select distinct deviceassigntable.* from deviceassigntable,testresulttable where deviceassigntable.SerialNo=testresulttable.SerialNo
      and testresulttable.Result=p_testresult and deviceassigntable.TestDateTime>=p_starttime and deviceassigntable.TestDateTime<=p_endtime
      and deviceassigntable.SerialNo not in (select distinct deviceassigntable.SerialNo from deviceassigntable,testresulttable where
      deviceassigntable.SerialNo=testresulttable.SerialNo and testresulttable.Result=2 and deviceassigntable.TestDateTime>=p_starttime
      and deviceassigntable.TestDateTime<=p_endtime) order by TestDateTime;
    elseif p_testresult=2 then
      select distinct deviceassigntable.* from deviceassigntable,testresulttable where deviceassigntable.SerialNo=testresulttable.SerialNo
      and testresulttable.Result=p_testresult and deviceassigntable.TestDateTime>=p_starttime and deviceassigntable.TestDateTime<=p_endtime order by TestDateTime;
    end if;
  elseif p_condition=3 then
    if p_testresult=1 then
      select distinct deviceassigntable.* from deviceassigntable,testresulttable where deviceassigntable.DeviceType=p_devicetype
      and deviceassigntable.SerialNo=testresulttable.SerialNo and testresulttable.Result=p_testresult and deviceassigntable.TestDateTime>=p_starttime
      and deviceassigntable.TestDateTime<=p_endtime and deviceassigntable.SerialNo not in
      (select distinct deviceassigntable.SerialNo from deviceassigntable,testresulttable where deviceassigntable.SerialNo=testresulttable.SerialNo
      and testresulttable.Result=2 and deviceassigntable.TestDateTime>=p_starttime and deviceassigntable.TestDateTime<=p_endtime) order by TestDateTime;
    elseif p_testresult=2 then
      select distinct deviceassigntable.* from deviceassigntable,testresulttable where deviceassigntable.DeviceType=p_devicetype and deviceassigntable.SerialNo=testresulttable.SerialNo
      and testresulttable.Result=p_testresult and deviceassigntable.TestDateTime>=p_starttime and deviceassigntable.TestDateTime<=p_endtime order by TestDateTime;
    end if;
  end if;
END $$

DROP PROCEDURE IF EXISTS get_deviceInfo $$
CREATE PROCEDURE get_deviceInfo
(
  in p_deviceType   int
)
BEGIN
  select * from devicetable where devicetable.DeviceType=p_deviceType;
END $$

DROP PROCEDURE IF EXISTS get_loadFile $$
CREATE PROCEDURE get_loadFile
(
  in  p_deviceType   int
)
BEGIN
  select * from loadfiletable where loadfiletable.DeviceType=p_deviceType;
END $$

DROP PROCEDURE IF EXISTS get_peopleInTest $$
CREATE PROCEDURE get_peopleInTest()
BEGIN
  select * from testloginnotetable where LoginOutDateTime is NULL;
END $$

DROP PROCEDURE IF EXISTS get_procedureInfo $$
CREATE PROCEDURE get_procedureInfo
(
  in p_procedure  int
)
BEGIN
    select * from proceduretable where proceduretable.Procedure=p_procedure;
END $$

DROP PROCEDURE IF EXISTS get_testResult $$
CREATE PROCEDURE get_testResult
(
  in p_serialno varchar(32)
)
BEGIN
  select * from testresulttable where SerialNo=p_serialno;
END $$

DROP PROCEDURE IF EXISTS get_testResultNum $$
CREATE PROCEDURE get_testResultNum
(
  in p_condition int,
  in p_starttime datetime,
  in p_endtime   datetime,
  in p_devicetype int,
  in p_testresult int
)
BEGIN
  if p_condition=0 then
    select count(*) from deviceassigntable where TestDateTime>=p_starttime and TestDateTime<=p_endtime;
  elseif p_condition=1 then
    select count(*) from deviceassigntable where DeviceType=p_devicetype and TestDateTime>=p_starttime and TestDateTime<=p_endtime;
  elseif p_condition=2 then
    if p_testresult=1 then
      select count(distinct deviceassigntable.SerialNo) from deviceassigntable,testresulttable where 
      testresulttable.Result=p_testresult and deviceassigntable.TestDateTime>=p_starttime and deviceassigntable.TestDateTime<=p_endtime
      and deviceassigntable.SerialNo not in (select distinct deviceassigntable.SerialNo from deviceassigntable,testresulttable where
      deviceassigntable.SerialNo=testresulttable.SerialNo and testresulttable.Result=2 and deviceassigntable.TestDateTime>=p_starttime
      and deviceassigntable.TestDateTime<=p_endtime);
    elseif p_testresult=2 then
      select count(distinct deviceassigntable.SerialNo) from deviceassigntable,testresulttable where 
      testresulttable.Result=p_testresult and deviceassigntable.TestDateTime>=p_starttime and deviceassigntable.TestDateTime<=p_endtime;
    end if;
  elseif p_condition=3 then
    if p_testresult=1 then
      select count(distinct deviceassigntable.SerialNo) from deviceassigntable,testresulttable where deviceassigntable.DeviceType=p_devicetype
      and deviceassigntable.SerialNo=testresulttable.SerialNo and testresulttable.Result=p_testresult and deviceassigntable.TestDateTime>=p_starttime
      and deviceassigntable.TestDateTime<=p_endtime and deviceassigntable.SerialNo not in
      (select distinct deviceassigntable.SerialNo from deviceassigntable,testresulttable where deviceassigntable.SerialNo=testresulttable.SerialNo
      and testresulttable.Result=2 and deviceassigntable.TestDateTime>=p_starttime and deviceassigntable.TestDateTime<=p_endtime);
    elseif p_testresult=2 then
      select count(distinct deviceassigntable.SerialNo) from deviceassigntable,testresulttable where deviceassigntable.DeviceType=p_devicetype and deviceassigntable.SerialNo=testresulttable.SerialNo
      and testresulttable.Result=p_testresult and deviceassigntable.TestDateTime>=p_starttime and deviceassigntable.TestDateTime<=p_endtime;
    end if;
  end if;
END $$

DROP PROCEDURE IF EXISTS update_config $$
CREATE PROCEDURE update_config
(
  in p_devicetype      int,
  in p_firstip         varchar(32),
  in p_firstmacone     varchar(32),
  in p_firstmactwo     varchar(32),
  in p_count           int,
  in p_seriallength    int,
  in p_productdate     date
)
BEGIN
  update configtable set DeviceType=p_devicetype,FirstIP=p_firstip,FirstMACOne=p_firstmacone,FirstMACTwo=p_firstmactwo,
      configtable.Count=p_count,SerialLength=p_seriallength,ProductDate=p_productdate;
END $$

DROP PROCEDURE IF EXISTS update_device $$
CREATE PROCEDURE update_device
(
  in p_hardwareversion     int,
  in p_e2prominfo          int,
  in p_e2promflag          int,
  in p_abort               int,
  in p_serverpath          varchar(32),
  in p_operatorpath        varchar(32),
  in p_testdelay           int,
  in p_resume              int,
  in p_devicetype          int
)
BEGIN
  update devicetable set HardwareVersion=p_hardwareversion,E2promInfo=p_e2prominfo,E2promFlag=p_e2promflag,
      Abort=p_abort,ServerPath=p_serverpath,OperatorPath=p_operatorpath,TestDelay=p_testdelay,devicetable.Resume=p_resume where DeviceType=p_devicetype;
END $$

DROP PROCEDURE IF EXISTS update_loadFile $$
CREATE PROCEDURE update_loadFile
(
  in p_filename     varchar(32),
  in p_devicetype   int,
  in p_loadfiletype int
)
BEGIN
  update loadfiletable set FileName=p_filename where DeviceType=p_devicetype and LoadFileType=p_loadfiletype;
END $$

DELIMITER ;$$