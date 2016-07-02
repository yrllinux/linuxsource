#ifndef _MakeMtEventDescription_

#ifndef EVENTCOM_H
#define EVENTCOM_H

#include "kdvtype.h"
#include "osp.h"
#include "eventid.h"
//定义消息起始号
const u16 begin_event_value = EV_MT_BGN;
//The end of message value is not using for mt,
//because the message which defined in below is 
//the inner message for mt
const u16 end_event_value   = EV_MT_END + 500; // [Fangtao,2007-01-19]

//定义每段的消息个数
#define begin_define_event_count 
	#define define_event_count(name,count) \
		const u16 _##name##_event_count = count
#define end_define_event_count

	begin_define_event_count
		define_event_count(event_value,1); 
		define_event_count(mtsys,60);		
		define_event_count(user_manager,20);
		define_event_count(file_trans,15);
		define_event_count(mtcall , 35);
		define_event_count(gk,5);	
		define_event_count(conference , 60);
		define_event_count(conferenceex,80);
		define_event_count(vod,20);
		define_event_count(matrix,40);
		define_event_count(camera,25);
		define_event_count(serial,10);
		define_event_count(config,120);
		define_event_count(codec,150);
		define_event_count(alarm,25);
		define_event_count(MC   ,50);
		define_event_count(dhcp ,15);
		define_event_count(pppoe,20);
		define_event_count(sitecall,20); 
		define_event_count(autoupgrade, 20);		
		define_event_count(pcdualvideo, 15);
		define_event_count(batchconfig, 25);
		define_event_count(extend, 100 );
	end_define_event_count

		



//定义消息范围
#define event_count(name) _##name##_event_count

#define begin_define_event_range(base) \
        const u16 _first_event_value_ = begin_##base;
#define define_event_range(base,name) \
		const u16 begin_##name = begin_##base + event_count(base)
#define outerevent_define_event_range(base)\
	     const u16 _ounterevent_event_value_ = begin_##base + event_count(base) +1;
#define end_define_event_range(base)\
        const u16 _last_event_value_ = begin_##base + event_count(base)+1;
#define pcdvevent_define_event_range(base)\
	    const u16 _pcdvevent_event_value_ = begin_##base + event_count(base) + 1;

begin_define_event_range(event_value)
		define_event_range(event_value,mtsys);
	    define_event_range(mtsys,codec);
        define_event_range(codec,pcdualvideo);
pcdvevent_define_event_range(pcdualvideo)
		define_event_range(pcdualvideo,user_manager);
		define_event_range(user_manager,file_trans);
		define_event_range(file_trans,mtcall);
		define_event_range(mtcall,gk);
		define_event_range(gk,conference);
		define_event_range(conference,conferenceex);
		define_event_range(conferenceex,vod);
		define_event_range(vod,matrix);
		define_event_range(matrix,camera);
		define_event_range(camera,serial);
		define_event_range(serial,config);
		define_event_range(config,alarm);
		define_event_range(alarm,MC   );
		define_event_range(MC,dhcp   );	
		define_event_range(dhcp,pppoe);
		define_event_range(pppoe,sitecall);
		define_event_range(sitecall, autoupgrade);
outerevent_define_event_range(autoupgrade);
		define_event_range(autoupgrade,batchconfig);
		define_event_range(batchconfig, extend   );
end_define_event_range( extend )


//正常定义消息体，用enum来实现消息的值的唯一性
#define _ev_segment(val) ev_##val##_begin = begin_##val,
#define _event(val) val,
#define _body(val1 , val2)
#define _ev_end
#define _ev_segment_end(val) ev_##val##_end 

//获取消息段的实际消息个数
#define ev_segment_range(val) (ev_##val##_end - ev_##val##_begin)
//最后一个消息的字面值
#define ev_last_event_val     _last_event_value_
//实际分配的消息总数
#define ev_using_event_space  (_last_event_value_ - _first_event_value_ + 1)
//最后一个外部消息的字面值
#define ev_outerevent_last_event_val _ounterevent_event_value_
#define ev_pcdv_event_val  _pcdvevent_event_value_          

//获取每一段的第一个消息字面值
#define get_first_event_val(val) (ev_##val##_begin)
//获取每一段的最后一个消息字面值
#define get_last_event_val(val)  (ev_##val##_end)
//外部消息定义起始点
const u16  ev_appinner_begin = ev_last_event_val + 1 ;
#endif

#endif

