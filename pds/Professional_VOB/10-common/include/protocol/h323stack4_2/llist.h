/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

/* Linked lists (for internal use only!) */
#ifndef _LList_h_
#define _LList_h_

#ifdef _cplusplus
extern "C" {
#endif /* _cplusplus*/


/*  Doubly linked list structure.  Can be used as either a list head, or
  as link words.*/

typedef struct _RV_LIST_ENTRY {
   struct _RV_LIST_ENTRY* Flink;
   struct _RV_LIST_ENTRY* Blink;
} RV_LIST_ENTRY, *RV_PLIST_ENTRY;


#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

#define GetLastList(ListHead) \
    (IsListEmpty(ListHead) ? NULL : (ListHead)->Blink)

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}

#define RemoveEntryList(Entry) {\
    RV_PLIST_ENTRY _EX_Blink;\
    RV_PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

#define InsertTailList(ListHead,Entry) {\
    RV_PLIST_ENTRY _EX_Blink;\
    RV_PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

#define InsertHeadList(ListHead,Entry) {\
    RV_PLIST_ENTRY _EX_Flink;\
    RV_PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }

#ifdef _cplusplus
}
#endif /* _cplusplus */

#endif /* _LList_h_*/
