#ifndef _LINKLIST_H_
#define _LINKLIST_H_

#ifdef __cplusplus
extern "C" {
#endif


/*NOINC*/


typedef struct _LIST_ENTRY {
    struct _LIST_ENTRY* Flink;
    struct _LIST_ENTRY* Blink;
} LIST_ENTRY;
typedef LIST_ENTRY* PLIST_ENTRY;


/*
 * Linked List Manipulation Functions - from NDIS.H
 */

/*
 * Calculate the address of the base of the structure given its type, and an
 * address of a field within the structure. - from NDIS.H
 */
#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) ((type *)( \
                          (unsigned char*)(address) - \
                          (unsigned char*)(&((type *)0)->field)))
#endif  /* CONTAINING_RECORD */

/*
 *  Doubly-linked list manipulation routines.  Implemented as macros
 */
#define InitializeListHead(ListHead) \
    ((ListHead)->Flink = (ListHead)->Blink = (ListHead) )

#define IsListEmpty(ListHead) \
    (( ((ListHead)->Flink == (ListHead)) ? TRUE : FALSE ) )

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {\
    PLIST_ENTRY FirstEntry;\
    FirstEntry = (ListHead)->Flink;\
    FirstEntry->Flink->Blink = (ListHead);\
    (ListHead)->Flink = FirstEntry->Flink;\
    }

#define RemoveEntryList(Entry) do {\
    PLIST_ENTRY _EX_Entry;\
    _EX_Entry = (Entry);\
    _EX_Entry->Blink->Flink = _EX_Entry->Flink;\
    _EX_Entry->Flink->Blink = _EX_Entry->Blink;\
    } while(0)

static inline PLIST_ENTRY RemoveTailList(PLIST_ENTRY ListHead)
{
    PLIST_ENTRY _Tail_Entry;
    _Tail_Entry = ListHead->Blink;
    RemoveEntryList(_Tail_Entry);
    return _Tail_Entry;
}

#define InsertTailList(ListHead,Entry) do {\
    (Entry)->Flink = (ListHead);\
    (Entry)->Blink = (ListHead)->Blink;\
    (ListHead)->Blink->Flink = (Entry);\
    (ListHead)->Blink = (Entry);\
    } while(0)

#define InsertHeadList(ListHead,Entry) do {\
    (Entry)->Flink = (ListHead)->Flink;\
    (Entry)->Blink = (ListHead);\
    (ListHead)->Flink->Blink = (Entry);\
    (ListHead)->Flink = (Entry);\
    } while (0)

/*INC*/

/* the added macros to extend the linklist operation */

/* insert a new entry before a list entry */
#define InsertBeforeList(ListEntry,Entry) do {\
    (Entry)->Flink = (ListEntry);\
    (Entry)->Blink = (ListEntry)->Blink;\
    (ListEntry)->Blink->Flink = (Entry);\
    (ListEntry)->Blink = (Entry);\
    } while (0)

/* insert a new entry after a list entry */
#define InsertAfterList(ListEntry,Entry) do {\
    (Entry)->Flink = (ListEntry)->Flink;\
    (Entry)->Blink = (ListEntry);\
    (ListEntry)->Flink->Blink = (Entry);\
    (ListEntry)->Flink = (Entry);\
    } while (0)

/*
 * List access methods.
 */
#define ListFor(ListEntry,ListHead)  \
	for(ListEntry=(ListHead)->Flink;\
         ListEntry!=ListHead;ListEntry=ListEntry->Flink)

/************************************************************
 *
 *  Doubly-linked list manipulation routines.  Implemented as macros
 *  but logically these are procedures. 
 *  The following is copied from ntddk.h, but names are changed.
 */

/*
    VOID
    lstInitializeHead(
        PLIST_ENTRY ListHead
        );
*/

#define lstInitializeHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

/*
    BOOLEAN
    lstIsEmpty(
        PLIST_ENTRY ListHead
        );
*/

#define lstIsEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

/*
    PLIST_ENTRY
    lstRemoveHead(
        PLIST_ENTRY ListHead
        );
*/

#define lstRemoveHead(ListHead) \
    (ListHead)->Flink;\
    {lstRemoveEntry((ListHead)->Flink)}

/*
    PLIST_ENTRY
    lstRemoveTail(
        PLIST_ENTRY ListHead
        );
*/

#define lstRemoveTail(ListHead) \
    (ListHead)->Blink;\
    {lstRemoveEntry((ListHead)->Blink)}

/*
    VOID
    lstRemoveEntry(
        PLIST_ENTRY Entry
        );
*/

#define lstRemoveEntry(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

/*
    VOID
    lstInsertTail(
        PLIST_ENTRY ListHead,
        PLIST_ENTRY Entry
        );
*/

#define lstInsertTail(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

/*
    VOID
    lstInsertHead(
        PLIST_ENTRY ListHead,
        PLIST_ENTRY Entry
        );
*/

#define lstInsertHead(ListHead,Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }

/************************************************************
 *
 *  The following is appended according the requirement of our invok.
 */
 
/*
    PLIST_ENTRY
    lstNextEntry(
        PLIST_ENTRY Entry
        );
*/

 #define lstNextEntry(Entry) \
     (Entry)->Flink
     
/*
    PLIST_ENTRY
    lstPreviousEntry(
        PLIST_ENTRY Entry
        );
*/

#define lstPreviousEntry(Entry) \
    (Entry)->Blink

/*
    PLIST_ENTRY
    lstFirstEntry(
        PLIST_ENTRY ListHead
        );
*/

 #define lstFirstEntry(ListHead) \
     (ListHead)->Flink
     
/*
    PLIST_ENTRY
    lstLastEntry(
        PLIST_ENTRY ListHead
        );
*/

#define lstLastEntry(ListHead) \
    (ListHead)->Blink

/*
    VOID
    lstMerge(
        PLIST_ENTRY ListHeadDst,
        PLIST_ENTRY ListHeadSrc
    );
*/

#define lstConCat(ListHeadDst,ListHeadSrc) {\
    PLIST_ENTRY _EX_ListHead1;\
    PLIST_ENTRY _EX_ListHead2;\
    _EX_ListHead1 = (ListHeadDst);\
    _EX_ListHead2 = (ListHeadSrc);\
    if (!lstIsEmpty(_EX_ListHead2))\
    {\
        PLIST_ENTRY _EX_Blink1;\
        PLIST_ENTRY _EX_Flink2;\
        PLIST_ENTRY _EX_Blink2;\
        _EX_Blink1 = _EX_ListHead1->Blink;\
        _EX_Flink2 = _EX_ListHead2->Flink;\
        _EX_Blink2 = _EX_ListHead2->Blink;\
        _EX_Flink2->Blink = _EX_Blink1;\
        _EX_Blink2->Flink = _EX_ListHead1;\
        _EX_Blink1->Flink = _EX_Flink2;\
        _EX_ListHead1->Blink = _EX_Blink2;\
        lstInitializeHead(_EX_ListHead2);\
    }\
}
#ifdef __cplusplus
}
#endif


#endif  /* _LINKLIST_H_ */

