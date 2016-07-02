create database sus;

use sus;

CREATE TABLE UserTable
(
    UserNo          int                 auto_increment,
    UserName        varchar(32)         not null,
    Password        char(32)            null,
    Type            tinyint             not null,
    FullName        varchar(32)         null,
    Description     varchar(64)         null,

CONSTRAINT PK_UserTable_UserNo      PRIMARY KEY (UserNo)
);

CREATE TABLE LogTable
(
    SerialNo        int                 auto_increment,
    Category        smallint            not null,
    LogLevel        smallint            not null,
    Event           smallint            not null,
    EventSubType    smallint            not null,
    Device          varchar(32)         null,
    HardVer         varchar(32)         null,
    Time            int                 not null,
    ErrorCode       int                 not null,
    SpecialInfo     varchar(64)         null,

CONSTRAINT PK_LogTable_SerialNo     PRIMARY KEY (SerialNo)
);

CREATE TABLE FileTable
(
    FileNo          int                 auto_increment,
    DevVerNo        int                 not null,
    Filename        varchar(64)         not null,
    Type            int                 not null,
    Size            int                 not null,

CONSTRAINT PK_FileTable_FileNo      PRIMARY KEY (FileNo)
);

CREATE TABLE DevVerTable
(
    DevVerNo        int                 auto_increment,
    DeviceType      varchar(32)         not null,
    HardVer         varchar(32)         not null,
    SoftVer         varchar(64)         not null,
    Report          varchar(512)        null,
    IsRecommend     tinyint             not null,

CONSTRAINT PK_DevVerTable_DevVerNo  PRIMARY KEY (DevVerNo)
);



#ALTER TABLE UserTable ADD INDEX UserTable_Index (UserNo);

ALTER TABLE LogTable ADD INDEX LogTable_Index (SerialNo);

#ALTER TABLE FileTable ADD INDEX FileTalbe_Index (FileNo);

#ALTER TABLE DevVerTable ADD INDEX DevVerTable_Index (DevVerNo);

INSERT INTO UserTable( UserName, Password, Type, FullName, Description ) VALUES( 'admin', '21232f297a57a5a743894a0e4a801fc', 1, 'administrator', 'administrator' );

#MT 8220A  Linux
#INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS6610__Linux PowerPC__', '9', '40.30.01.01.070123', 'mt 8220A Linux', 1 );
#SET @DevNo = last_insert_id();
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 );
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 );
#MT 8220A VxRaw
#INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS6610__Vxworks Raw____', '9', '40.30.01.01.070123', 'mt 8220A VxRaw', 1 );
#SET @DevNo = last_insert_id();
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 );
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 );
#MT 8220B  Linux
#INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS6610__Linux PowerPC__', '9', '40.30.01.01.070123', 'mt 8220B Linux', 1 );
#SET @DevNo = last_insert_id();
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 );
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 );
#MT 8220B VxRaw
#INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS6610__Vxworks Raw____', '9', '40.30.01.01.070123', 'mt 8220B VxRaw', 1 );
#SET @DevNo = last_insert_id();
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 );
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 );
#MT 8220C  Linux
#INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS6610__Linux PowerPC__', '9', '40.30.01.01.070123', 'mt 8220C Linux', 1 );
#SET @DevNo = last_insert_id();
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 );
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 );
#MT 8220C VxRaw
#INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS6610__Vxworks Raw____', '9', '40.30.01.01.070123', 'mt 8220C VxRaw', 1 );
#SET @DevNo = last_insert_id();
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 );
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 );
#MT 8620A  Linux
#INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS6610__Linux PowerPC__', '9', '40.30.01.01.070123', 'mt 8620A Linux', 1 );
#SET @DevNo = last_insert_id();
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 );
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 );
#MT 8620A VxRaw
#INSERT INTO DevVerTable( DeviceType, HardVer, SoftVer, Report, IsRecommend ) VALUES( 'mt______TS6610__Vxworks Raw____', '9', '40.30.01.01.070123', 'mt 8620A VxRaw', 1 );
#SET @DevNo = last_insert_id();
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'service', 1, 4437 );
#INSERT INTO FileTable( DevVerNo, Filename, Type, Size ) VALUES( @DevNo, 'console', 2, 4463 );


#CREATE PROCEDURE splitpageqry
#(
#    @qrystr     varchar(5120),
#    @startid    int,
#    @totalnum   int,
#    @userdata   int
#)
#BEGIN
#    DECLARE @rcount         int;
#    DECLARE @execsql        varchar(5376);
#    DECLARE @tempteablename varchar(256);
#
#    SELECT @rcount = @startid + @totalnum - 1;
#    SET rowcount = @rcount;
#    SET @tempteablename = '#temptable' + convert( varchar, @userdata );
#    SET @execsql = stuff( @qrystr, charindex( 'select', @qrystr ), 6,
#                          'SELECT sybid = identity(12), ' );
#    SET @execsql = stuff( @execsql, charindex( 'from', @execsql ), 4,
#                          'into ' + @tempteablename + ' from ' );
#    SET @execsql = @execsql || ' select * from '+ @tempteablename
#                   + ' WHERE sybid > ' || convert( varchar, (@startid - 1) ) || ' and sybid <= ' || convert( varchar, ( @startid + @totalnum - 1) );
#    execute (@execsql);
#    SET rowcount = 0;
#END $$


DELIMITER $$

DROP PROCEDURE IF EXISTS user_getCount $$
CREATE PROCEDURE user_getcount
()
BEGIN
    SELECT Count(*) FROM UserTable;
END $$

DROP PROCEDURE IF EXISTS user_auth $$
CREATE PROCEDURE user_auth
(
    OUT p_Ret           tinyint,
    IN  p_Name          varchar(32),
    IN  p_Pwd           varchar(32)
)
BEGIN
    DECLARE nCount tinyint;
    SET p_Ret = 3;

    SELECT Count(*) INTO nCount FROM UserTable WHERE UserName = p_Name;

    IF ( nCount = 0 ) THEN
        SET p_Ret = 1;
    ELSE
        IF ( p_Pwd is null or p_Pwd = '' ) THEN
            SELECT Count(*) INTO nCount FROM UserTable
                WHERE UserName = p_Name AND (Password = '' or Password is null);
        ELSE
            SELECT Count(*) INTO nCount FROM UserTable
                WHERE UserName = p_Name AND Password = p_Pwd;
        END IF; #  IF ( Pwd is null or Pwd = '' ) THEN

        IF ( nCount = 0 ) THEN
            SET p_Ret = 2;
        ELSE
            SET p_Ret = 0;
        END IF; # IF ( nCount = 0 ) THEN
    END IF; # IF ( nCount = 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS user_getlist $$
CREATE PROCEDURE user_getlist
()
BEGIN
    SELECT UserNo, UserName, Password, Type, FullName, Description FROM UserTable;
END $$

DROP PROCEDURE IF EXISTS user_add $$
CREATE PROCEDURE user_add
(
    OUT p_Ret           tinyint,
    OUT p_UserNo        int,
    IN  p_Name          varchar(32),
    IN  p_Type          tinyint,
    IN  p_Pwd           varchar(32),
    IN  p_FullName      varchar(32),
    IN  p_Desc          varchar(64)
)
BEGIN
    DECLARE nCount tinyint;
    SET p_Ret = 2;
    SET p_UserNo = 0;

    SELECT COUNT(*) INTO nCount FROM UserTable WHERE UserName = p_Name;
    IF ( nCount > 0 ) THEN
        SET p_Ret = 1;
    ELSE
        INSERT INTO UserTable ( UserName, Password, Type, FullName, Description )
            VALUES ( p_Name, p_Pwd, p_Type, p_FullName, p_Desc );
        SET p_UserNo = last_insert_id();
        SET p_Ret = 0;
    END IF; # IF ( nCount > 0 ) THEN
END $$


DROP PROCEDURE IF EXISTS user_edit $$
CREATE PROCEDURE user_edit
(
    OUT p_Ret           tinyint,
    IN  p_UserNo        int,
    IN  p_Name          varchar(32),
    IN  p_Type          tinyint,
    IN  p_Pwd           varchar(32),
    IN  p_FullName      varchar(32),
    IN  p_Desc          varchar(64)
)
BEGIN
    DECLARE nCount tinyint;
    DECLARE OldName varchar(32);

    SET p_Ret = 3;
    SET nCount = 0;

    SELECT COUNT(*) INTO nCount FROM UserTable WHERE UserNo = p_UserNo;

    IF ( nCount <> 1 ) THEN
        SET p_Ret = 1;
    ELSE
        SELECT UserName INTO OldName FROM UserTable WHERE UserNo = p_UserNo;

        IF ( OldName <> p_Name ) THEN
            SET p_Ret = 2;
        ELSE
            UPDATE UserTable
                SET
                    Password    = p_Pwd,
                    Type        = p_Type,
                    FullName    = p_FullName,
                    Description = p_Desc
                WHERE UserNo = p_UserNo;
            SET p_Ret = 0;
        END IF; # IF ( OldName <> p_Name ) THEN
    END IF; # IF ( nCount <> 1 ) THEN
END $$

DROP PROCEDURE IF EXISTS user_del $$
CREATE PROCEDURE user_del
(
    OUT p_Ret        tinyint,
    IN  p_UserNo     int
)
BEGIN
    DECLARE nCount tinyint;
    DECLARE OldName varchar(32);

    SET p_Ret = 3;
    SET nCount = 0;

    SELECT COUNT(*) INTO nCount FROM UserTable WHERE UserNo = p_UserNo;

    IF ( nCount <> 1 ) THEN
        SET p_Ret = 1;
    ELSE
        SELECT UserName INTO OldName FROM UserTable WHERE UserNo = p_UserNo;

        IF ( OldName = 'admin' ) THEN
            SET p_Ret = 2;
        ELSE
            DELETE FROM UserTable WHERE UserNo = p_UserNo;
            SET p_Ret = 0;
        END IF; # IF ( OldName = 'admin' ) THEN
    END IF; # IF ( nCount <> 1 ) THEN
END $$

DROP PROCEDURE IF EXISTS log_add $$
CREATE PROCEDURE log_add
(
    OUT p_SerialNo      int,
    IN  p_Category      smallint,
    IN  p_Level         smallint,
    IN  p_Event         smallint,
    IN  p_EventSubType  smallint,
    IN  p_Time          int,
    IN  p_Device        varchar(32),
    IN  p_HardVer       varchar(32),
    IN  p_ErrorCode     int,
    IN  p_Info          varchar(64)
)
BEGIN
    DECLARE nCount int;
    SELECT COUNT(*) INTO nCount FROM LogTable;

#    IF ( nCount >= 9999 );
#        BEGIN
#            
#        END

    INSERT INTO LogTable ( Category, LogLevel, Event, EventSubType, Device,
                           HardVer, Time, ErrorCode, SpecialInfo )
        VALUES ( p_Category, p_Level, p_Event, p_EventSubType, p_Device, p_HardVer,
                 p_Time, p_ErrorCode, p_Info );
    SET p_SerialNo = last_insert_id();
END $$

DROP PROCEDURE IF EXISTS log_del_one $$
CREATE PROCEDURE log_del_one
(
    IN p_SerialNo       int
)
BEGIN
    DELETE FROM LogTable WHERE SerialNo = p_SerialNo;
END $$

DROP PROCEDURE IF EXISTS log_clear $$
CREATE PROCEDURE log_clear
()
BEGIN
    DELETE FROM LogTable;
END $$

DROP PROCEDURE IF EXISTS log_event_dev_query_condition $$
CREATE PROCEDURE log_event_dev_query_condition
(
    OUT p_QueryCondition    varchar(3840),
    IN  p_EventDevMask      smallint
)
BEGIN
    DECLARE DevCondition   varchar(1024);

    IF ( p_EventDevMask > 0 ) THEN

        IF ( p_EventDevMask >= 4 ) THEN
            SET p_EventDevMask = p_EventDevMask - 4;
            SET DevCondition = 'EventSubType = 4';
        END IF; # IF ( p_EventDevMask >= 4 ) THEN

        IF ( p_EventDevMask >= 2 ) THEN
            SET p_EventDevMask = p_EventDevMask - 2;
            IF ( LEN( DevCondition ) > 0 ) THEN
                SET DevCondition = DevCondition + ' OR ';
            END IF; # IF ( LEN( DevCondition ) > 0 ) THEN
            SET DevCondition = DevCondition + 'EventSubType = 2';
        END IF; # IF ( p_EventDevMask >= 2 ) THEN

        IF ( p_EventDevMask >= 1 ) THEN
            SET p_EventDevMask = 0;
            IF ( LEN( DevCondition ) > 0 ) THEN
                SET DevCondition = DevCondition + ' OR ';
            END IF; # IF ( LEN( DevCondition ) > 0 ) THEN
            SET DevCondition = DevCondition + 'EventSubType = 1';
        END IF; # IF ( p_EventDevMask >= 1 ) THEN

        SET p_QueryCondition = '((Event = 1) AND (' + DevCondition + '))';
    END IF; # IF ( p_EventDevMask > 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS log_event_client_query_condition $$
CREATE PROCEDURE log_event_client_query_condition
(
    OUT p_QueryCondition    varchar(3840),
    IN  p_EventClientMask   smallint
)
BEGIN
    DECLARE ClientCondition    varchar(1024);

    IF ( EventClientMask > 0 ) THEN

        IF ( p_EventClientMask >= 128 ) THEN
            SET p_EventClientMask = p_EventClientMask - 128;
            SET ClientCondition = 'EventSubType = 128';
        END IF; # IF ( p_EventClientMask >= 128 ) THEN

        IF ( p_EventClientMask >= 64 ) THEN
            SET p_EventClientMask = p_EventClientMask - 64;
            IF ( LEN( ClientCondition ) > 0 ) THEN
                SET ClientCondition = ClientCondition + ' OR ';
            END IF; # IF ( LEN( ClientCondition ) > 0 ) THEN
            SET ClientCondition = ClientCondition + 'EventSubType = 64';
        END IF; # IF ( p_EventClientMask >= 64 ) THEN

        IF ( p_EventClientMask >= 32 ) THEN
            SET p_EventClientMask = p_EventClientMask - 32;
            IF ( LEN( ClientCondition ) > 0 ) THEN
                SET ClientCondition = ClientCondition + ' OR ';
            END IF; # IF ( LEN( ClientCondition ) > 0 ) THEN
            SET ClientCondition = ClientCondition + 'EventSubType = 32';
        END IF; # IF ( p_EventClientMask >= 32 ) THEN

        IF ( p_EventClientMask >= 16 ) THEN
            SET p_EventClientMask = p_EventClientMask - 16;
            IF ( LEN( ClientCondition ) > 0 ) THEN
                SET ClientCondition = ClientCondition + ' OR ';
            END IF; # IF ( LEN( ClientCondition ) > 0 ) THEN
            SET ClientCondition = ClientCondition + 'EventSubType = 16';
        END IF; # IF ( p_EventClientMask >= 16 ) THEN

        IF ( p_EventClientMask >= 8 ) THEN
            SET p_EventClientMask = p_EventClientMask - 8;
            IF ( LEN( ClientCondition ) > 0 ) THEN
                SET ClientCondition = ClientCondition + ' OR ';
            END IF; # IF ( LEN( ClientCondition ) > 0 ) THEN
            SET ClientCondition = ClientCondition + 'EventSubType = 8';
        END IF; # IF ( p_EventClientMask >= 8 ) THEN

        IF ( p_EventClientMask >= 4 ) THEN
            SET p_EventClientMask = p_EventClientMask - 4;
            IF ( LEN( ClientCondition ) > 0 ) THEN
                SET ClientCondition = ClientCondition + ' OR ';
            END IF; # IF ( LEN( ClientCondition ) > 0 ) THEN
            SET ClientCondition = ClientCondition + 'EventSubType = 4';
        END IF; # IF ( p_EventClientMask >= 4 ) THEN

        IF ( p_EventClientMask >= 2 ) THEN
            SET p_EventClientMask = p_EventClientMask - 2;
            IF ( LEN( ClientCondition ) > 0 ) THEN
                SET ClientCondition = ClientCondition + ' OR ';
            END IF; # IF ( LEN( ClientCondition ) > 0 ) THEN
            SET ClientCondition = ClientCondition + 'EventSubType = 2';
        END IF; # IF ( p_EventClientMask >= 2 ) THEN

        IF ( p_EventClientMask >= 1 ) THEN
            SET p_EventClientMask = 0;
            IF ( LEN( ClientCondition ) > 0 ) THEN
                SET ClientCondition = ClientCondition + ' OR ';
            END IF; # IF ( LEN( ClientCondition ) > 0 ) THEN
            SET ClientCondition = ClientCondition + 'EventSubType = 1';
        END IF; # IF ( p_EventClientMask >= 1 ) THEN

        SET p_QueryCondition = ' ((Event = 2) AND (' + ClientCondition + '))';
    END IF; # IF ( EventClientMask > 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS log_event_query_condition $$
CREATE PROCEDURE log_event_query_condition
(
    OUT p_QueryCondition    varchar(2048),
    IN  p_EventMask         smallint,
    IN  p_EventDevMask      smallint,
    IN  p_EventClientMask   smallint
)
BEGIN
    DECLARE DevQueryCondition      varchar(1024);
    DECLARE ClientQueryCondition   varchar(1024);
    DECLARE DevLen     int;
    DECLARE ClientLen  int;

    IF ( p_EventMask >= 2 ) THEN
        SET p_EventMask = p_EventMask - 2;
        call log_event_dev_query_condition( DevQueryCondition, p_EventDevMask );
        select DevQueryCondition;
    END IF; # IF ( p_EventMask >= 2 ) THEN

    IF ( p_EventMask >= 1 ) THEN
        SET p_EventMask = 0;
        call log_event_client_query_condition( ClientQueryCondition, p_EventClientMask );
        select ClientQueryCondition;
    END IF; # IF ( p_EventMask >= 1 ) THEN

    SET DevLen = LEN( DevQueryCondition );
    SET ClientLen = LEN( ClientQueryCondition );

    IF ( DevLen > 0 AND ClientLen > 0 ) THEN
        SET p_QueryCondition = '(' + DevQueryCondition + ' OR ' + ClientQueryCondition + ')';
    ELSE
        IF ( DevLen > 0 ) THEN
            SET p_QueryCondition = DevQueryCondition;
        ELSE
            IF ( ClientLen > 0 ) THEN
                SET p_QueryCondition = ClientQueryCondition;
            END IF; # IF ( ClientLen > 0 ) THEN
        END IF; # IF ( DevLen > 0 ) THEN
    END IF; # IF ( DevLen > 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS log_result_query_condition $$
CREATE PROCEDURE log_result_query_condition
(
    OUT p_QueryCondition    varchar(2048),
    IN  p_ResultMask        smallint
)
BEGIN
    DECLARE TempQuery  varchar(1024);

    IF ( p_ResultMask >= 2 ) THEN
        SET p_ResultMask = p_ResultMask - 2;
        SET TempQuery = 'ErrorCode <> 0';
    END IF; # IF ( p_ResultMask >= 2 ) THEN

    IF ( p_ResultMask >= 1 ) THEN
        IF ( LEN( TempQuery ) > 0 ) THEN
            SET TempQuery = TempQuery + ' OR ';
        END IF; # IF ( LEN( TempQuery ) > 0 ) THEN
        SET p_ResultMask = 0;
        SET TempQuery = TempQuery + 'ErrorCode = 0';
    END IF; # IF ( p_ResultMask >= 1 ) THEN

    IF ( LEN( TempQuery ) > 0 ) THEN
        SET p_QueryCondition = '(' + TempQuery + ')';
    END IF; # IF ( LEN( TempQuery ) > 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS log_dev_query_condition $$
CREATE PROCEDURE log_dev_query_condition
(
    OUT p_QueryCondition    varchar(2048),
    IN  p_DevMask           smallint,
    IN  p_DevType           varchar(32),
    IN  p_HardVer           varchar(32)
)
BEGIN
    DECLARE TempQuery  varchar(1024);

    IF ( p_DevMask >= 8 ) THEN
        SET p_DevMask = p_DevMask - 8;
        SET TempQuery = 'HardVer = @HardVer';
    END IF; # IF ( @DevMask >= 8 ) THEN

    IF ( p_DevMask >= 4 ) THEN
        IF ( LEN( TempQuery ) > 0 ) THEN
            SET TempQuery = TempQuery + ' AND ';
        END IF; # IF ( LEN( TempQuery ) > 0 ) THEN
        SET p_DevMask = p_DevMask - 4;
        SET TempQuery = TempQuery + 'substring(Device, 17, 15) = ' + substring(p_DevType, 17, 15);
    END IF; # IF ( p_DevMask >= 4 ) THEN

    IF ( p_DevMask >= 2 ) THEN
        IF ( LEN( TempQuery ) > 0 ) THEN
            SET TempQuery = TempQuery + ' AND ';
        END IF; # IF ( LEN( TempQuery ) > 0 ) THEN
        SET p_DevMask = p_DevMask - 2;
        SET TempQuery = TempQuery + 'substring(Device, 9, 8) = ' + substring(p_DevType, 9, 8);
    END IF; # IF ( p_DevMask >= 2 ) THEN

    IF ( p_DevMask >= 1 ) THEN
        IF ( LEN( TempQuery ) > 0 ) THEN
            SET TempQuery = TempQuery + ' AND ';
        END IF; # IF ( LEN( TempQuery ) > 0 ) THEN
        SET p_DevMask = p_DevMask - 1;
        SET TempQuery = TempQuery + 'substring(Device, 1, 8) = ' + substring(p_DevType, 1, 8);
    END IF; # IF ( p_DevMask >= 1 ) THEN

    IF ( LEN( p_TempQuery ) > 0 ) THEN
        SET p_QueryCondition = '(' + TempQuery + ')';
    END IF; # IF ( LEN( @TempQuery ) > 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS log_time_query_condition $$
CREATE PROCEDURE log_time_query_condition
(
    OUT p_QueryCondition    varchar(2048),
    IN  p_StartTime         int,
    IN  p_EndTime           int
)
BEGIN
    DECLARE TempQuery  varchar(1024);

    IF ( p_StartTime > 0 ) THEN
        SET TempQuery = 'Time > @StartTime';
    END IF; # IF ( p_StartTime > 0 ) THEN

    IF ( p_EndTime > 0 ) THEN
        IF ( LEN( TempQuery ) > 0 ) THEN
            SET TempQuery = TempQuery + ' AND ';
        END IF; # IF ( LEN( TempQuery ) > 0 ) THEN

        SET TempQuery = TempQuery + 'Time < @EndTime';
    END IF; # IF ( p_EndTime > 0 ) THEN

    IF ( LEN( TempQuery ) > 0 ) THEN
        SET p_QueryCondition = '(' + TempQuery + ')';
    END IF; # IF ( LEN( TempQuery ) > 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS log_level_query_condition $$
CREATE PROCEDURE log_level_query_condition
(
    OUT p_QueryCondition    varchar(2048),
    IN  p_LevelMask         smallint
)
BEGIN
    DECLARE TempQuery  varchar(1024);

    IF ( p_LevelMask >= 4 ) THEN
        SET p_LevelMask = p_LevelMask - 4;
        SET TempQuery = 'LogLevel = 4';
    END IF; # IF ( p_LevelMask >= 4 ) THEN

    IF ( p_LevelMask >= 2 ) THEN
        SET p_LevelMask = p_LevelMask - 2;
        IF ( LEN( TempQuery ) > 0 ) THEN
            SET TempQuery = TempQuery + ' OR ';
        END IF; # IF ( LEN( TempQuery ) > 0 ) THEN
        SET TempQuery = TempQuery + 'LogLevel = 2';
    END IF; # IF ( p_LevelMask >= 2 ) THEN

    IF ( p_LevelMask >= 1 ) THEN
        SET p_LevelMask = 0;
        IF ( LEN( TempQuery ) > 0 ) THEN
            SET TempQuery = TempQuery + ' OR ';
        END IF; # IF ( LEN( TempQuery ) > 0 ) THEN
        SET TempQuery = TempQuery + 'LogLevel = 1';
    END IF; # IF ( p_LevelMask >= 1 ) THEN

    IF ( LEN( TempQuery ) > 0 ) THEN
        SET p_QueryCondition = '(' + TempQuery + ')';
    END IF; # IF ( LEN( TempQuery ) > 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS log_category_query_condition $$
CREATE PROCEDURE log_category_query_condition
(
    OUT p_QueryCondition    varchar(2048),
    IN  p_CategoryMask      smallint
)
BEGIN
    DECLARE TempQuery  varchar(1024);

    IF ( p_CategoryMask >= 2 ) THEN
        SET p_CategoryMask = p_CategoryMask - 2;
        SET TempQuery = 'Category = 2';
    END IF; # IF ( p_CategoryMask >= 2 ) THEN

    IF ( p_CategoryMask >= 1 ) THEN
        SET p_CategoryMask = 0;
        IF ( LEN( TempQuery ) > 0 ) THEN
            SET TempQuery = TempQuery + ' OR ';
        END IF; # IF ( LEN( TempQuery ) > 0 ) THEN
        SET TempQuery = TempQuery + 'Category = 1';
    END IF; # IF ( p_CategoryMask >= 1 ) THEN

    IF ( LEN( TempQuery ) > 0 ) THEN
        SET p_QueryCondition = '(' + TempQuery + ')';
    END IF; # IF ( LEN( TempQuery ) > 0 ) THEN
END $$


DROP PROCEDURE IF EXISTS log_query $$
CREATE PROCEDURE log_query
(
    IN  p_MainType          tinyint,
    IN  p_CategoryMask      smallint,
    IN  p_LevelMask         smallint,
    IN  p_ResultMask        smallint,
    IN  p_DevMask           smallint,
    IN  p_EventMask         smallint,
    IN  p_EventDevMask      smallint,
    IN  p_EventClientMask   smallint,
    IN  p_DevType           varchar(32),
    IN  p_DevHardVer        varchar(32),
    IN  p_StartTime         int,
    IN  p_EndTime           int
)
BEGIN
    DECLARE QueryCondition varchar(3840);
    DECLARE TempQuery varchar(2048);
    DECLARE szSql varchar(4096);

    IF ( p_MainType = 0 ) THEN
        SELECT ( SerialNo, Category, LogLevel, Event, EventSubType,
                 Device, HardVer, Time, ErrorCode, SpecialInfo )
            FROM LogTable;
    ELSE
        IF ( p_MainType >= 32 ) THEN
            SET p_MainType = p_MainType - 32;
            call log_event_query_condition( TempQuery, p_EventMask,
                                            p_EventDevMask, p_EventClientMask);
            SELECT TempQuery;
            SET QueryCondition = QueryCondition + TempQuery;
        END IF; # IF ( p_MainType >= 32 ) THEN

        IF ( p_MainType >= 16 ) THEN
            SET p_MainType = p_MainType - 16;
            SET TempQuery = '';
            call log_result_query_condition( TempQuery, p_ResultMask);
            SELECT TempQuery;

            IF ( LEN( QueryCondition ) > 0 AND LEN( TempQuery ) > 0 ) THEN
                SET QueryCondition = QueryCondition + ' AND ';
            END IF; # IF ( LEN( QueryCondition ) > 0 AND LEN( TempQuery ) > 0  ) THEN
            SET QueryCondition = QueryCondition + TempQuery;
        END IF; # IF ( p_MainType >= 16 ) THEN

        IF ( p_MainType >= 8 ) THEN
            SET p_MainType = p_MainType - 8;
            SET TempQuery = '';
            call log_dev_query_condition( TempQuery, p_DevMask, p_DevType, p_DevHardVer );
            SELECT TempQuery;
            IF ( LEN( QueryCondition ) > 0 AND LEN( TempQuery ) > 0 ) THEN
                SET QueryCondition = QueryCondition + ' AND ';
            END IF; # IF ( LEN( QueryCondition ) > 0 AND LEN( TempQuery ) > 0  ) THEN
            SET QueryCondition = QueryCondition + TempQuery;
        END IF; # IF ( p_MainType >= 8 ) THEN

        IF ( p_MainType >= 4 ) THEN
            SET p_MainType = p_MainType - 4;
            SET TempQuery = '';
            call log_time_query_condition( TempQuery, p_StartTime, p_EndTime );
            SELECT TempQuery;
            IF ( LEN( QueryCondition ) > 0 AND LEN( TempQuery ) > 0 ) THEN
                SET QueryCondition = QueryCondition + ' AND ';
            END IF; # IF ( LEN( QueryCondition ) > 0 AND LEN( TempQuery ) > 0  ) THEN
            SET QueryCondition = QueryCondition + TempQuery;
        END IF; # IF ( p_MainType >= 4 ) THEN

        IF ( p_MainType >= 2 ) THEN
            SET p_MainType = p_MainType - 2;
            SET TempQuery = '';
            call log_level_query_condition( TempQuery, p_LevelMask );
            SELECT TempQuery;
            IF ( LEN( QueryCondition ) > 0 AND LEN( TempQuery ) > 0  ) THEN
                SET QueryCondition = QueryCondition + ' AND ';
            END IF; # IF ( LEN( QueryCondition ) > 0 AND LEN( TempQuery ) > 0  ) THEN
            SET QueryCondition = QueryCondition + TempQuery;
        END IF; # IF ( p_MainType >= 2 ) THEN

        IF ( p_MainType >= 1 ) THEN
            SET p_MainType = 0;
            SET TempQuery = '';
            call log_category_query_condition( TempQuery, p_CategoryMask );
            SELECT TempQuery;
            IF ( LEN( QueryCondition ) > 0 AND LEN( TempQuery ) > 0  ) THEN
                SET QueryCondition = QueryCondition + ' AND ';
            END IF; # IF ( LEN( QueryCondition ) > 0 AND LEN( TempQuery ) > 0  ) THEN
            SET QueryCondition = QueryCondition + TempQuery;
        END IF; # IF ( p_MainType >= 1 ) THEN

        SET szSql = 'SELECT SerialNo, Category, LogLevel, Event, EventSubType, Device, HardVer, Time, ErrorCode, SpecialInfo FROM LogTable WHERE'
                    + QueryCondition;
        SET @szSql = szSql;
        PREPARE stmt FROM @szSql;
        execute stmt;
        DEALLOCATE PREPARE stmt;
    END IF; # IF ( p_MainType = 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS dev_getallrecommend_devfile $$
CREATE PROCEDURE dev_getallrecommend_devfile
()
BEGIN
    SELECT DevVerTable.DevVerNo, DevVerTable.DeviceType, DevVerTable.HardVer,
           DevVerTable.Report, DevVerTable.SoftVer, DevVerTable.IsRecommend,
           FileTable.FileNo, FileTable.Filename, FileTable.Type, FileTable.Size
        FROM DevVerTable INNER JOIN FileTable
        On DevVerTable.DevVerNo = FileTable.DevVerNo;
END $$

DROP PROCEDURE IF EXISTS dev_getall_devfile $$
CREATE PROCEDURE dev_getall_devfile
(
    IN  p_DeviceType    varchar(32)
)
BEGIN
    SELECT DevVerTable.DevVerNo, DevVerTable.DeviceType, DevVerTable.HardVer,
           DevVerTable.Report, DevVerTable.SoftVer, DevVerTable.IsRecommend,
           FileTable.FileNo, FileTable.Filename, FileTable.Type, FileTable.Size
        FROM DevVerTable INNER JOIN FileTable
        On DevVerTable.DeviceType = p_DeviceType
           AND DevVerTable.DevVerNo = FileTable.DevVerNo;
END $$

CREATE PROCEDURE dev_add
(
    OUT p_Ret           tinyint,
    OUT p_DevVerNo      int,
    IN  p_DeviceType    varchar(32),
    IN  p_SoftVer       varchar(64),
    IN  p_HardVer       varchar(32),
    IN  p_Report        varchar(512)
)
BEGIN
    DECLARE nCount int;

    SET p_Ret = 2;
    SET p_DevVerNo = 0;

    SELECT COUNT(*) INTO nCount FROM DevVerTable
        WHERE DeviceType = p_DeviceType AND SoftVer = p_SoftVer;

    IF ( nCount > 0 ) THEN
        SET p_Ret = 1;
    ELSE
        INSERT INTO DevVerTable ( DeviceType, HardVer, SoftVer, Report, IsRecommend )
            VALUES ( p_DeviceType, p_HardVer, p_SoftVer, p_Report, 0 );
        SET p_DevVerNo = last_insert_id();
        SET p_Ret = 0;
    END IF; # IF ( nCount > 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS dev_edit $$
CREATE PROCEDURE dev_edit
(
    OUT p_Ret           tinyint,
    IN  p_DevVerNo      int,
    IN  p_HardVer       varchar(32),
    IN  p_Report        varchar(512)
)
BEGIN
    DECLARE nCount int;

    SET p_Ret = 2;
    SELECT COUNT(*) INTO nCount FROM DevVerTable WHERE DevVerNo = p_DevVerNo;

    IF ( nCount = 0 ) THEN
        SET p_Ret = 1;
    ELSE
        UPDATE DevVerTable
            SET
                Report      = p_Report,
                HardVer     = p_HardVer
            WHERE DevVerNo = p_DevVerNo;
        SET p_Ret = 0;
    END IF; # IF ( nCount = 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS dev_edit_ver $$
CREATE PROCEDURE dev_edit_ver
(
    OUT p_Ret           tinyint,
    IN  p_OldNo         int,
    IN  p_DevVerNo      int
)
BEGIN
    DECLARE nCount int;

    SELECT COUNT(*) INTO nCount FROM DevVerTable WHERE DevVerNo = p_DevVerNo;

    SET p_Ret = 2;

    IF ( nCount = 0 ) THEN
        SET p_Ret = 1;
    ELSE
        IF ( p_OldNo <> 0 ) THEN
            UPDATE DevVerTable
                SET
                    IsRecommend = 0
                WHERE DevVerNo = p_OldNo;
        END IF; # ( p_OldNo <> 0 ) THEN

        UPDATE DevVerTable
            SET
                IsRecommend = 1
            WHERE DevVerNo = p_DevVerNo;

        SET p_Ret = 0;
    END IF; # IF ( nCount = 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS dev_get_devfile $$
CREATE PROCEDURE dev_get_devfile
(
    IN  p_DevVerNo      int
)
BEGIN
    SELECT DevVerTable.DevVerNo, DevVerTable.DeviceType, DevVerTable.HardVer,
           DevVerTable.Report, DevVerTable.SoftVer, DevVerTable.IsRecommend,
           FileTable.FileNo, FileTable.Filename, FileTable.Type, FileTable.Size
        FROM DevVerTable INNER JOIN FileTable
        On  DevVerTable.DevVerNo = p_DevVerNo AND FileTable.DevVerNo = p_DevVerNo;
END $$

DROP PROCEDURE IF EXISTS dev_del $$
CREATE PROCEDURE dev_del
(
#    OUT p_Ret           tinyint,
    IN  p_DevVerNo      int
)
BEGIN
#    SET p_Ret = 2
#    DECLARE nCount int
#    SELECT COUNT(*) INTO nCount FROM DevVerTable WHERE DevVerNo = p_DevVerNo

#    IF ( nCount = 0 ) THEN
#        SET p_Ret = 1
#    ELSE
        DELETE FROM FileTable WHERE DevVerNo = p_DevVerNo;
        DELETE FROM DevVerTable WHERE DevVerNo = p_DevVerNo;
#            @Ret = 0
#    END IF # IF ( nCount = 0 ) THEN
END $$

DROP PROCEDURE IF EXISTS dev_get_hardver $$
CREATE PROCEDURE dev_get_hardver
(
    IN  p_DeviceType    varchar(32),
    IN  p_SoftVer       varchar(64)
)
BEGIN
    SELECT HardVer FROM DevVerTable
        WHERE DeviceType = p_DeviceType AND SoftVer = p_SoftVer;
END $$

#CREATE PROCEDURE dev_del_ver_newer_than_recommend
#(
#    @Device     varchar(32),
#    @SoftVer    varchar(64)
#)
#BEGIN
#    DELETE FROM FileTable WHERE
#        DevNo = ( SELECT DevNo FROM DevVerTable WHERE Device = @Device
#                                                   AND SoftVer > @SoftVer );
#    DELETE FROM DevVerTable WHERE Device = @Device AND SoftVer > @SoftVer;
#    EXEC dev_edit_ver @Device, @SoftVer;
#END$

#CREATE PROCEDURE file_query_devfile
#(
#    @DevNo      int
#)
#BEGIN
#    SELECT FileNo, Filename, Type FROM FileTable WHERE DevNo = @DevNo;
#END $$

DROP PROCEDURE IF EXISTS file_add $$
CREATE PROCEDURE file_add
(
    OUT p_Ret           tinyint,
    OUT p_FileNo        int,
    IN  p_DevVerNo      int,
    IN  p_Filename      varchar(64),
    IN  p_Type          int,
    IN  p_Size          int
)
BEGIN
    DECLARE nCountName int;
    DECLARE nCountType int;

    SET p_Ret = 3;

    SELECT COUNT(*) INTO nCountName FROM FileTable
        WHERE ( DevVerNo = p_DevVerNo AND Filename = p_Filename );
    SELECT COUNT(*) INTO nCountType FROM FileTable
        WHERE ( DevVerNo = p_DevVerNo AND Type = p_Type );

    IF ( nCountName > 0 ) THEN
        SET p_Ret = 1;
    ELSE
        IF ( nCountType > 0 ) THEN
            SET p_Ret = 2;
        ELSE
            INSERT INTO FileTable ( DevVerNo, Filename, Type, Size )
                VALUES ( p_DevVerNo, p_Filename, p_Type, p_Size );
            SET p_FileNo = last_insert_id();
            SET p_Ret = 0;
        END IF; # IF ( nCountType > 0 ) THEN
    END IF; # IF ( nCountName > 0 ) THEN
END $$

#CREATE PROCEDURE file_edit
#(
#    @FileNo     int,
#    @DevNo      int,
#    @Filename   varchar(64),
#    @Type       int
#)
#BEGIN
#    UPDATE FileTable
#        SET
#            Filename    = @Filename,
#            Type        = @Type
#        WHERE FileNo = @FileNo AND DevNo = @DevNo;
#END $$

DROP PROCEDURE IF EXISTS file_del $$
CREATE PROCEDURE file_del
(
    IN  p_FileNo        int
)
BEGIN
    DELETE FROM FileTable WHERE FileNo = p_FileNo;
END $$

delimiter ;$$