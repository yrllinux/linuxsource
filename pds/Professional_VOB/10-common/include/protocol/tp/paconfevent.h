#ifndef  PA_CONFEVENT_H
#define   PA_CONFEVENT_H
#include "padefine.h"
#include "pastruct.h"
#ifndef _PA_EventDescription_
enum EnumPAConfEvent
{
#endif
	_paev_segment( paconfevent )

	_paevent(  ev_PAFastUpdate  )
	_paev_end

	_paevent(  ev_PAFlowCtrl  )
	_pabody( u16       ,1  ) 
	_paev_end

	_paevent(  ev_PAYouAreSeeing  )
	_pabody( TYouAreSeeing       ,1  ) 
	_paev_end

 	_paevent(  ev_PAMeetingSpeaker  )
	_pabody( s32       ,1  ) 
	_paev_end

	_paevent(  ev_PABeChairman  )
	_pabody( BOOL32       ,1  ) 
	_paev_end

	_paevent(  ev_PACascadingSpeaker  )
	_pabody( TPACascadingSpeaker   ,1  ) 
	_paev_end

	_paevent(  ev_PAStartPoll  )
	_pabody( TPAPollCmd       ,1  )
	_paev_end

	_paevent(  ev_PAAdjustFrameRate  )
	_pabody( TPAAdjustFrameRate       ,1  )
	_paev_end

	_paevent(  ev_PAAdjustVidRes  )
	_pabody( TPAAdjustVidRes       ,1  )
	_paev_end

	_paevent(  ev_PAStartPollAck  )
	_pabody( TPAPollAck       ,1  )
	_paev_end

	_paevent(  ev_PAEndPoll  )
	_pabody( BOOL32		,1 )//是否暂停
	_paev_end


	_paevent(  ev_PAPollFastUpdate  )
	_pabody( u16       ,1 )//reserved无意义用于xml编码
	_paev_end

	_paevent(  ev_PAConfStatusIndicate  )
	_pabody( TPAConfStatusInd      ,1  )
	_paev_end

	_paevent(  ev_PAConfPollNodeChange  )
	_pabody( TPAPollNodeChange      ,1  )
	_paev_end

	_paevent(  ev_PAConfPeerMuteCmd  )
	_pabody( TPAPeerMuteCmd      ,1  )
	_paev_end

	_paevent(  ev_PAConfPeerMuteInd  )
	_pabody( TPAPeerMuteInd      ,1  )
	_paev_end

	_paevent(  ev_PAConfCallCascadingOff  )
	_pabody( u16      ,1  )
	_pabody( TPAAlias ,1  )
	_paev_end

	_paevent(  ev_PAConfHangUpCascadingConf  )
	_pabody( THangupCascadingConf      ,1  )
	_paev_end

	_paevent(  ev_PAConfNodeUpdateTrList  )
	_pabody( TPAConfNodeUpdateTrList      ,1  )
	_paev_end

	_paevent(  ev_PAConfNodeUpdateB2NewTrList  )
	_pabody( TPAConfNodeUpdateB2NEWTrList      ,1  )
	_paev_end

	_paevent(  ev_PAConfNodeDelTrList  )
	_pabody( TPAConfNodeDelTrList      ,1  )
	_paev_end


	_paevent( ev_PAConfAudMixEpCmd)  // 上级通知下级混音某个端点
	_pabody( TPAAudMixEpCmdTr      ,1  )
	_paev_end

	
	_paevent( ev_PAConfAudMixEpAck)// 下级通知上级混音某个端点的结果
	_pabody( TPAAudMixEpAckTr      ,1  )
	_paev_end

	_paevent( ev_PAConfAudMixEpStop)// 上级级通知下级停止某个端点的混音
	_pabody( u16      ,1  )   //端点ID
	_paev_end


	_paevent( ev_PAConfAudMixOpenNCmd)// 上级通知下级打开N模式通道
	_pabody( TPAAudMixEpCmdTr      ,1  )
	_paev_end

	_paevent( ev_PAConfAudMixOpenNAck)// 下级通知上级打开N模式通道
	_pabody( TPAAudMixEpAckTr      ,1  )
	_paev_end

	_paevent( ev_PAConfPeerSoundOffCmd )
	_pabody( TPAPeerMuteCmd      ,1  )
	_paev_end

	_paevent( ev_PAConfPeerSoundOffInd )
	_pabody( TPAPeerMuteInd      ,1  )
	_paev_end


	_paevent( ev_PARequestDualStreamCmd )
	_pabody( TDualRequest      ,1  )//reserved
	_paev_end

	_paevent( ev_PARequestDualStreamInd )
	_pabody( BOOL32      ,1  )
	_pabody( EmDualReason      ,1  )
	_pabody( TDualRequest,  1 )
	_paev_end


	_paevent( ev_PASendDualStreamCmd )
	_pabody( TDualInfo      ,1  )
	_paev_end

	_paevent( ev_PASendDualStreamAck )
	_pabody( TDualInfoAck      ,1  )
	_paev_end

	_paevent( ev_PAStopDualStream )
	_pabody( TPAEpKey      ,1  )//reserved 
	_paev_end

	_paevent( ev_PADualStreamFastUpdate )
	_pabody( BOOL32      ,1  ) 
	_paev_end

	_paevent( ev_PAViewLocal )
	_pabody( u32      ,1  )//reserved 
	_paev_end

	_paevent( ev_PACancelViewLocal )
	_pabody( u32      ,1  )//reserved 
	_paev_end

	_paevent( ev_PAStartRcvDualInd )
	_pabody( TPAEpKey      ,1  )//TPAEpKey 
	_paev_end

	_paevent( ev_PAStopRcvDualInd )
	_pabody( EmDualReason      ,1  ) 
	_paev_end


	_paevent( ev_PAAddTvMonitorCmd )
	_pabody( TPATvMonitorCmd      ,1  ) 
	_paev_end

	_paevent( ev_PAAddTvMonitorInd )
	_pabody( TPATvMonitorInd      ,1  ) 
	_paev_end

	_paevent( ev_PAStopMonitorCmd )
	_pabody( u16      ,1  ) 
	_paev_end

	_paevent( ev_PAAskKeyByScreen )
	_pabody( TPAAskKeyByScreenTr      ,1  ) 
	_paev_end

	_paevent( ev_PAPollDstNameInd )
	_pabody( TPAPollDstName      ,1  ) 
	_paev_end

	_paevent( ev_PASrcDualNameInd )
	_pabody( TPAName      ,1  ) 
	_pabody( TPAEpKey      ,1  ) 
	_paev_end

	_paevent( ev_PAChairCapNotify )
	_pabody( TPANodeCapsetTr      ,1  ) 
	_paev_end

	_paevent ( ev_PAConfCnsList )
	_pabody (TPAConfCnsList,    1)
    _paev_end 
	
	_paevent (ev_PAConfStartPolling) 
	_pabody (u32 , 1)
	_paev_end
	
	_paevent (ev_PAConfStartPollingRes)
	_pabody (u32 , 1)
	_paev_end
	
    _paevent ( ev_PAConfClosePolling ) 
	_pabody (u32,  1)
    _paev_end
	
    _paevent ( ev_PAConfClosePollingRes ) 
	_pabody (u32 , 1)
    _paev_end
    
    _paevent ( ev_PAConfNotifyPollingRes )
	_pabody ( BOOL32 ,  1)
	_paev_end

	_paevent ( ev_PAChanSelecteCascad )
	_pabody (TPAChanSelecteCascad, 1)
	_paev_end
	
    _paevent( ev_PAChanSelecteCascadRes)
	_pabody(TPATmpChanSelecteCascadRes, 1)
	_paev_end
	
	_paevent(ev_PAChanSelecteCascadCancel)
	_pabody(s32, 1)
	_paev_end
	
	_paevent(ev_PAChanSelecteCascadCancelRes)
	_pabody(TPAChanSelecteCascadCancelRes,1)
	_paev_end
	
    _paevent(ev_PAChanSeeLocal)
	_pabody (s32, 1)
	_paev_end

	_paevent(ev_PAChanSeeLocalCancel)
	_pabody (s32, 1)
	_paev_end

    _paevent(ev_PACascadeStartViewcmd)
	_pabody(TPAViewCmd, 1)
	_paev_end
    
	_paevent(ev_PACascadeStartViewInd)
	_pabody(TPAViewAck, 1)
	_paev_end
   
	_paevent(ev_PACascadeStopView_cmd)
	_pabody(u16, 1)
	_paev_end

	_paevent(ev_PACascadeGetVidcmd)
	_pabody(TPAViewCmd, 1)
	_paev_end
    
	_paevent(ev_PACascadeGetVidInd)
	_pabody(TPAViewAck, 1)
	_paev_end
	
	_paevent(ev_PACascadeStopGetVidcmd)
	_pabody(u16, 1)
	_paev_end

	_paevent(ev_PACascadeFlowCtrl)
	_pabody(TPAFlowControl, 1)
	_paev_end

	_paevent(ev_PAOpenAudChanCmd)
	_pabody(TPAAudChanCmd, 1)
	_paev_end

	_paevent(ev_PAOpenAudChanAck)
	_pabody(TPAAudChanAck, 1)
	_paev_end

	_paevent(ev_PAOpenVidChanCmd)
	_pabody(TPAVidChanCmd, 1)
	_paev_end

	_paevent(ev_PAOpenVidChanAck)
	_pabody(TPAVidChanAck, 1)
	_paev_end

	_paevent(ev_PACloseVidChanCmd)
	_pabody(u16, 1)
	_paev_end
	
	_paevent(ev_PATppConfAMixUpdate_Cmd)
	_pabody(TPAConfAuxMixInfo, 1)
	_paev_end
	
	_paevent(ev_PATppConfAMixUpdate_Ind)
	_pabody(TPAConfAuxMixInfo, 1)
	_paev_end
	
	_paevent(ev_PATppConfAMixUpdate_Nty)
	_pabody(TPAConfAuxMixInfo, 1)
	_paev_end

	_paevent(ev_PATppConfApplySpeaker_Cmd)
	_pabody(TPAConfEpID, 1)
	_paev_end


	_paevent(ev_PATppConfApplySpeaker_Ind)
	_pabody(TPAConfEpID, 1)
	_pabody(EmPATpApplySpeakerRet, 1)
	_paev_end


	_paevent(ev_PATppConfApplySpeaker_Nty)
	_pabody(TPAConfEpID, 1)
	_paev_end

	_paevent(ev_PATppConfMute_Cmd)
	_pabody(TPAConfEpID, 1)
	_pabody(BOOL, 1)
	_paev_end


	_paevent(ev_PATppConfMute_Ind)
	_pabody(TPAConfEpID, 1)
	_pabody(BOOL, 1)
	_paev_end
	 	
	_paevent(ev_PATppConfMute_Nty)
	_pabody(TPAConfEpID, 1)
	_pabody(BOOL, 1)
	_paev_end

	_paevent(ev_PATppConfQuiet_Cmd)
	_pabody(TPAConfEpID, 1)
	_pabody(BOOL, 1)
	_paev_end

	_paevent(ev_PATppConfQuiet_Ind)
	_pabody(TPAConfEpID, 1)
	_pabody(BOOL, 1)
	_paev_end

	_paevent(ev_PATppConfQuiet_Nty)
	_pabody(TPAConfEpID, 1)
	_pabody(BOOL, 1)
	_paev_end

	_paevent(ev_PATppConfStartDual_Cmd)
	_pabody(TPAConfEpID, 1)
	_paev_end

	_paevent(ev_PATppConfStartDual_Ind)
	_pabody(TPAConfEpID, 1)
	_pabody(BOOL, 1)
	_paev_end
	
	_paevent(ev_PATppConfStopDual_Cmd)
	_pabody(TPAConfEpID, 1)
	_paev_end

	_paevent(ev_PATppConfStopDual_Ind)
	_pabody(TPAConfEpID, 1)
	_pabody(BOOL, 1)
	_paev_end

    _paevent(ev_PATppConfDualEp_Nty)
	_pabody(TPAConfEpID, 1)
	_paev_end


	_paevent(ev_PATppInviteCnsByAlias_Cmd)
	_pabody(TPAAlias, 1)
	_paev_end

	_paevent(ev_PATppInviteCnsByAlias_Ind)
	_pabody(TPAAlias, 1)
	_pabody(EmPACnsCallReason, 1)
	_paev_end


	_paevent(ev_PATppInviteCnsByEpId_Cmd)
	_pabody(u16, 1)
	_paev_end

	_paevent(ev_PATppInviteCnsByEpId_Ind)
	_pabody(u16, 1)
	_pabody(EmPACnsCallReason, 1)
	_paev_end

	_paevent(ev_PATppHangupCns_Cmd)
	_pabody(u16, 1)
	_paev_end
	
	_paevent(ev_PATppHangupCns_Ind)
	_pabody(u16, 1)
	_pabody(EmPACnsCallReason, 1)
	_paev_end

	_paevent(ev_PATppHangupConf_Cmd)
	_pabody(u32, 1)
	_paev_end

	_paevent(ev_PATppUpdateConfTurnlist_Nty)
	_pabody(TPAConfTurnInfo, 1)
	_paev_end


    _paevent(ev_PATppCallEPResult_Ind)
	_pabody(TPAConfCallEpAddr, 1)
	_pabody(EmPACnsCallReason, 1)
	_paev_end

	_paevent(ev_PATppChairConfInfo_Nty)
	_pabody(TPAChairConfInfo, 1)
	_paev_end


	_paevent(ev_PATppAudMixStatus_Nty)
	_pabody(TPAAudMixStat, 1)
	_paev_end

	_paevent(ev_PAUmsDiscussScreenStat_Notify)
	_pabody(TPADisScreenStat, 1)
	_paev_end
	
	_paevent(ev_PAUmsCnsSpeakerIndxAdjust_Notify)
	_pabody(TPACascadingSpeaker, 1)
	_paev_end

	_paevent( ev_PAUmsDiscussFastUpdate_Req )
	_pabody(TPADisAskKeyFrame, 1)
	_paev_end
	
	_paevent( ev_PAUmsAudMixDisListOpr_Cmd )
	_pabody(TPADisListOpr, 1)
	_paev_end
	
	_paevent( ev_PAUmsAudMixDisListOpr_Ind )
	_pabody(TPADisListOpr, 1)
	_paev_end

	_paevent( ev_PAUmsCascadeOpenChairChan_Cmd )
	_pabody(TPACascadeOpenChanCmd, 1)
	_paev_end

	_paevent( ev_PAUmsCascadeOpenChairChan_Ack )
	_pabody(TPACascadeOpenChanAck, 1)
	_paev_end

	// added by ganxiebin 20120808 >>>
	_paevent(  ev_PAConfStdJoinConf  )
	_paev_end

	_paevent(  ev_PAConfStdNotifyTerJoin  )
	_paev_end
	
	_paevent(  ev_PAConfStdNotifyTerQuit  )
	_paev_end
	
	_paevent(  ev_PAConfStdYouAreSeeing  )
	_pabody( TYouAreSeeing        ,1  ) 
	_paev_end	

	_paevent(  ev_PAConfStdTerLabelListReq  )
	_paev_end

	_paevent(  ev_PAConfStdTerLabelListRsp  )
	_paev_end
	
	_paevent(  ev_PAConfStdTerIDListReq  )
	_paev_end

	_paevent(  ev_PAConfStdTerIDListRsp  )
	_pabody( TConfCnsList       ,1  ) 
	_paev_end
	// added by ganxiebin 20120808 <<<


	// added by ganxiebin 20120814 >>>
	_paevent(  ev_PAConfMultipleConference  )   // eventID: 1435
	_pabody( u32       ,1 )//reserved无意义用于xml编码
	_paev_end
	
	_paevent(  ev_PAConfCancelMultipleConference  )
	_pabody( u32       ,1 )//reserved无意义用于xml编码
	_paev_end
	
	_paevent(  ev_PAConfTerminalNumberAssign  )
	_pabody( TPATerminalLabel       ,1 )
	_paev_end
	
	_paevent(  ev_PAConfTerminalYouAreSeeing  )
	_pabody( TYouAreSeeing        ,1  ) 
	_paev_end	
	
	_paevent(  ev_PAConfSeenByAll  )
	_pabody( u32       ,1 )//reserved无意义用于xml编码
	_paev_end
	
	_paevent(  ev_PAConfCancelSeenByAll  )
	_pabody( u32       ,1 )//reserved无意义用于xml编码
	_paev_end

	_paevent(  ev_PAConfTerminalListRequest  )
	_pabody( u32       ,1 )//reserved无意义用于xml编码
	_paev_end
	
	_paevent(  ev_PAConfTerminalListResponce  )
	_pabody( TPATerLabelList       ,1 )
	_paev_end

	_paevent(  ev_PAConfTerminalIDListRequest  )
	_pabody( u32       ,1 )//reserved无意义用于xml编码
	_paev_end
	
	_paevent(  ev_PAConfTerminalIDListResponse  )
	_pabody( TConfCnsList       ,1  ) 
	_paev_end
	
	_paevent(  ev_PAConfTerminalJoinConf  )
	_pabody( TPATerminalInfo       ,1 )
	_paev_end
	
	_paevent(  ev_PAConfTerminalLeftConf  )
	_pabody( TPATerminalInfo       ,1 )
	_paev_end
	// added by ganxiebin 20120814 <<<


    _paevent( ev_PAUmsCommonReasonToUI_Ind )
    _pabody(TPAUmsReasonInd, 1)
    _paev_end

    _paevent( ev_PAUmsConfRecPlayState_Nty )
    _pabody(TPAUmsUpRecPlayState, 1)
    _paev_end
    
    _paevent( ev_PAUmsConfStopRecPlay_Req )
    _pabody(u32, 1)
    _paev_end
    

#ifndef _PA_EventDescription_
	_paev_segment_end( paconfevent )
};
#endif

#endif


