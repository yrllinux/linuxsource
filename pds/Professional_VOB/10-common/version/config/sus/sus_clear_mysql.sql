use sus;

delete from UserTable;
delete from LogTable;
delete from FileTable;
delete from DevVerTable;

drop table UserTable;
drop table LogTable;
drop table FileTable;
drop table DevVerTable;

drop procedure user_getcount;
drop procedure user_auth;
drop procedure user_add;
drop procedure user_edit;
drop procedure user_del;
drop procedure log_add;
drop procedure log_del_one;
drop procedure log_clear;
drop procedure log_query;
drop procedure log_category_query_condition;
drop procedure log_level_query_condition;
drop procedure log_time_query_condition;
drop procedure log_dev_query_condition;
drop procedure log_result_query_condition;
drop procedure log_event_query_condition;
drop procedure log_event_client_query_condition;
drop procedure log_event_dev_query_condition;
drop procedure dev_getallrecommend_devfile;
drop procedure dev_getall_devfile;
drop procedure dev_add;
drop procedure dev_edit;
drop procedure dev_edit_ver;
drop procedure dev_get_devfile;
drop procedure dev_del;
drop procedure dev_get_hardver;
drop procedure file_add;
drop procedure file_del;


use mysql;
drop database sus;