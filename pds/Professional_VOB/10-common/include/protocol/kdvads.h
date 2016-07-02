#ifndef _KDVADS_H_
#define _KDVADS_H_

/*
    ADS (Abstract Data Service) 抽象数据服务
    提供数据结构操作的类库
*/

#include "Kdvtype.h"
#include "osp.h"

#ifndef STK_API
#define STK_API         OSP_API
#endif


#ifndef KDVERROR
#define KDVERROR        -1
#endif

#ifndef MEMALLOC
#define MEMALLOC(nSize) malloc(nSize)
#define MEMFREE(p)      free(p)
#endif

#define ADS_LOG         1
#define ADSDEBUG

//对齐大小
#define ALIGNSIZE       sizeof(void*)

#define ALIGN(n)        (((n)+ALIGNSIZE-1)/ALIGNSIZE * ALIGNSIZE)


typedef enum
{
    LOGINFO     =   (u32)0, 
    LOGDEBUG    =   (u32)1,
    LOGWARN     =   (u32)2,
    LOGERROR    =   (u32)3,
    LOGEXCEPT   =   (u32)4,   
    LOGALLWAYS  =   (u32)5,
    LAST_LEVEL
} ELogLevel;


void LogPrintf(s32 eModule, ELogLevel eLevel, const s8* pszformat, ...);


/*-----------------------------   CAdsArray    ----------------------------------
    CAdsArray（数组对象）实现对静态数组的管理，为用户提供固定大小结构块的申请与释放，
用于取代动态内存分配。在此基础上，可以进一步实现链表、缓冲POOL、树等对象，以进一步增
强对数据块的管理
    CAdsArray主要提供增加、删除、存取等操作。
---------------------------------------------------------------------------------*/

//数组节点用户数据单元
typedef void* PArrayElement;

/*
    元素比较函数原型
    如果匹配，则返回TRUE, 否则返回FALSE
    该函数保存在对象中。
*/
typedef BOOL32 (*IsElemMatchingT)(void* pElement, void *pParam);

/*
    元素处理函数原型
    该函数在循环调用中，每次调用结束，参数pParam置成返回值，并带入下一次调用
    该函数保存在对象中。
*/
typedef void* (*ProcessElemT)(void* pElement, void *pParam);


/*统计信息结构*/
typedef struct tagAdsStats
{
    u32      m_dwCur;           //当前使用数量
    u32      m_dwMaxUsage;      //最大的使用数量
    u32      m_dwMax;           //最大容量
} TAdsStats;


class STK_API CAdsArray
{
public:
    CAdsArray();
    virtual ~CAdsArray();

    /*====================================================================
    函数名      ：Create(s32 nElemSize, s32 nMaxElemNum, const s8* pchName)
    功能		：根据输入参数，分配Array内存，并设置相应的参数
    输入参数说明：nElemSize     元素大小
                  nMaxElemNum   最大元素数量
                  logMgr        日志句柄
                  pchName       Array名字
    返回值说明  ：失败：FALSE
                  成功：TRUE
    ====================================================================*/    
    BOOL32 Create(s32 nElemSize, s32 nMaxElemNum, const s8* pchName=NULL);

    //释放占用的空间
    void Close();

    //判断对象是否有效
    BOOL32 IsValid(); 
    
    //分配一个新的单元。返回单元的指针。（用于不关心元素Id的场合）
    PArrayElement Add();

    /*====================================================================
    函数名      ：Add(PArrayElement pElem)
    功能		：把pElem元素加入数组
    输入参数说明：pElem---指向用户元素的指针，元素大小固定
    返回值说明  ：失败：返回-1
                  成功：返回在数组中的位置
    ====================================================================*/
    s32 Add(PArrayElement pElem);

    /*====================================================================
    函数名      ：AddExt(PArrayElement* ppOutElem)
    功能		：取得一个空的Elem，并由ppOutElem指针带出。
    输入参数说明：ppOutElem---用于保存返回的指针
    返回值说明  ：失败：返回-1;
                  成功：返回在数组中的位置；
    ====================================================================*/
    s32 AddExt(PArrayElement* ppOutElem);
    
    /*====================================================================
    函数名      ：GetByPtr(void *pPtr)
    功能		：通过元素指针得到该元素的位置
    输入参数说明：pPtr---指向Array单元的指针
    返回值说明  ：失败：返回-1
                  成功：返回在数组中的位置
    ====================================================================*/
    s32 GetByPtr(PArrayElement pPtr);

    /*====================================================================
    函数名      ：GetNext(s32 nCur)
    功能		：返回当前位置nCur的下一个有效单元的位置
    输入参数说明：nCur---当前的起始位置
    返回值说明  ：失败：返回-1
                  成功：返回在数组中的位置
    ====================================================================*/
    s32 GetNext(s32 nCur);
    
    //清空数组
    void Clear();

    //删除指定的元素
    BOOL32 Delete(PArrayElement pArrayElement);

    /*====================================================================
    函数名      ：Delete(s32 nLocation)
    功能		：删除指定元素
    输入参数说明：nLocation---删除单元的位置号
    返回值说明  ：失败：FALSE
                  成功：TRUE
    ====================================================================*/
    BOOL32 Delete(s32 nLocation);

    /*====================================================================
    函数名      ：Find(void* pParam)
    功能		：根据内部比较函数，寻找适配的第一个元素
    输入参数说明：pParam---比较函数的参数，进行比较的关键值
    返回值说明  ：失败：-1
                  成功：找到的单元的位置号
    ====================================================================*/
    s32 Find(void* pParam);

    /*====================================================================
    函数名      ：GetElem(s32 nLocation)
    功能		：取得指定位置上的元素
    输入参数说明：nLocation----单元的位置号
    返回值说明  ：失败：NULL
                  成功：单元的指针
    ====================================================================*/
    PArrayElement GetElem(s32 nLocation);
    
    //取得Array的统计信息，错误返回FALSE
    BOOL32 GetStatistics(TAdsStats* pStats);
    BOOL32 GetStatistics(s32* pnTotalNum, s32* pnMaxUsage, s32* pnCurNum);

    //取得第一个节点的位置，错误返回KDVERROR
    s32 GetFirstLocation();

    //设置第一个节点的位置，错误返回KDVERROR
    s32 SetFirstLocation(s32 nLlocation);

    //取得最后一个节点的位置，错误返回KDVERROR
    s32 GetLastLocation();

    //设置最后一个节点的位置，错误返回KDVERROR
    s32 SetLastLocation(s32 nLlocation);

    //取得当前元素的数量，错误返回KDVERROR
    s32 GetCurNum();

    //取得当前空单元的数量，错误返回KDVERROR
    s32 GetFreeNum();

    //取得Array的最大容量，错误返回KDVERROR
    s32 GetMaxNum();

    //判断指定位置上的元素是否为空, 为空返回TRUE
    BOOL32 IsElemVacant(s32 nLocation);

    //取得Array的名字，错误返回NULL
    const s8* GetName();

    //取得峰值使用数量，错误返回KDVERROR
    s32 GetMaxUsage();

    //取得元素的大小，错误返回KDVERROR
    s32 GetElemSize();

    //取得Array的比较函数
    IsElemMatchingT GetCompareFunc();

    //设置单元处理回调函数
    void SetCompareFunc(IsElemMatchingT Func);
    
    //得到单元处理回调函数
    ProcessElemT GetProcElemFunc();

    //设置打印函数
    void SetProcElemFunc(ProcessElemT Func);

    //对所有单元调用默认单元处理函数进行处理
    //pParam是单元处理函数的参数，每次调用单元处理函数后，pParam置为返回值
    void DoAll(void *pParam);

private:
    //取得分配内存的数量
    s32 GetAllocationSize(s32 nElemSize, s32 m_nMaxNumOfElements);

    //设置指定单元的内容
    void SetElem(s32 nLocation, PArrayElement pSrc);
    
private:
    //取得位图起始位置
    #define BITMAP(pHeader)      ((u8 *)(pHeader) + sizeof(TArrayHeader))
    
    //位图大小
    #define BITMAPSIZE(n)   (((n)+7)/8 + (ALIGNSIZE - (((n)+7)/8)%ALIGNSIZE)%ALIGNSIZE)


    //得到指定位置的数据
    #define ELEMDATA(nLoc) \
                    (m_ptHeader->m_pchArrayLocation + (nLoc) * m_ptHeader->m_dwElemSize)

   
private:
    struct tagArrayHeader* m_ptHeader;

};







/*-----------------------------   CAdsList      ----------------------------------
    CAdsList（链表对象）在CAdsArray的基础上进一步增加了结构块的前后链接关系，提供了
一组相应的链表操作方法。
---------------------------------------------------------------------------------*/
//链表节点用户数据单元
typedef void* PListElement;

class STK_API CAdsList : private CAdsArray
{

public:
    CAdsList(){};
    virtual ~CAdsList();

    /**************************创建与删除***********************************/
    //分配空间，创建并初始化List对象
    BOOL32  Create(s32 nElemSize, s32 nMaxElemNum, const char* pchName=NULL);
    //销毁List对象，释放空间
    void    Close();


    /**************************节点和数据***********************************/
    //得到链表的头
    s32 Head() { return CAdsArray::GetFirstLocation(); }

    //得到链表的尾
    s32 Tail() { return CAdsArray::GetLastLocation(); }

    //得到链表的下一个节点
    s32 Next(s32 nLocation);

    //得到链表的前一个节点
    s32 Prev(s32 nLocation);

    //得到链表指定节点的元素
    PListElement GetElem(s32 nLocation);

    //通过元素指针得到链表节点
    s32 GetByPtr(PListElement pListElement);

    //用注册的比较函数在链表中查找匹配的节点
    s32 Find(s32 nLocation, void *pParam);

    //得到元素大小
    s32 GetElemSize()
    {
        return CAdsArray::GetElemSize() - sizeof(s32) - sizeof(s32);
    };


    
    /**************************添加与删除***********************************/
    //把元素加入链表中指定位置之后
    s32     Add(s32 nLocation, PListElement elem); /* after location */
    //删除指定的节点。
    BOOL32  Delete(s32 nLocation);
    //删除所有节点
    BOOL32  DeleteAll();
    //清除节点，重新初始化链表
    void    Clear();

    //加到尾部    
    s32 AddTail(PListElement pElem)
    {
        return Add(Tail(), pElem);
    };

    //加到头部
    s32 AddHead(PListElement pElem)
    {
        return Add(KDVERROR, pElem);
    };

    //删除头节点
    BOOL32 DeleteHead()
    {
        return Delete(Head());
    };

    //删除尾节点
    BOOL32 DeleteTail()
    {
        return Delete(Tail());
    };

    /**************************其他*************************************/

    //依次对链表中的元素做单元处理
    //pParam是单元处理回调函数的参数，并在每次调用后置成回调函数的返回值
    void DoAll(void *pParam);
    

    /*************************输出继承过来的部分私有函数****************************/

    //取得当前元素的数量
    s32 GetCurNum() { return CAdsArray::GetCurNum(); }

    //取得最大容量
    s32 GetMaxNum() { return CAdsArray::GetMaxNum(); }

    //取得比较函数
    IsElemMatchingT GetCompareFunc() { return CAdsArray::GetCompareFunc(); }

    //设置比较函数
    void SetCompareFunc(IsElemMatchingT pFunc) { CAdsArray::SetCompareFunc(pFunc); }

    //判断指定位置上的节点是否为空
    s32 IsElemVacant(s32 nLocation) { return CAdsArray::IsElemVacant(nLocation); }

    //取得最大的元素使用数量
    s32 GetMaxUsage() { return CAdsArray::GetMaxUsage(); };

    //取得统计信息
    s32 GetStatistics(TAdsStats* pStats) { return CAdsArray::GetStatistics(pStats); }

    BOOL32 GetStatistics(s32* pnTotalNum, s32* pnMaxUsage, s32* pnCurNum)
    {
        return CAdsArray::GetStatistics(pnTotalNum, pnMaxUsage, pnCurNum);
    };

    //得到打印函数
    ProcessElemT GetProcElemFunc() { return CAdsArray::GetProcElemFunc(); }
    
    //设置打印函数
    void SetProcElemFunc(ProcessElemT func){ CAdsArray::SetProcElemFunc(func); }


private:
    //把元素加入链表,不进行链表关系修改
	s32 AddNode(PListElement pElem);
};





/*-----------------------------   CAdsTree      ----------------------------------
    CAdsTree（树对象）在CAdsArray的基础上增加了树的管理和操作。
    Tree数据的组织形式：
        A：Tree节点的child指针指向第一个child节点(称为head)；
        B：一个节点的所有child节点通过brother指针构成单向链表;
        C：空的指针都为-1。
    
      遍历方法：
                         (例)          A
                                    /  |  \
                                   B   C   D
                                 /  \  | 
                                E    F G
        
          Preorder遍历： 先子树，再根节点。顺序为：EFBGCDA
          Postorder遍历：先根节点，再子树。顺序为：ABEFCGD
          实现中默认的遍历方法Next采用Postorder顺序          
          
            注意：这里的树不是二叉树，子树没有左右的概念。
---------------------------------------------------------------------------------*/
//树节点用户单元
typedef void* PTreeElement;


class CAdsTree;

/*
    树节点处理函数原型
    在循环调用中，每次调用结束，参数pParam置成返回值，并带入下一次调用
    该函数保存在对象中。
*/
typedef void* (*TreeProcElemFuncT)(CAdsTree* pcTree, s32 nNodeId, void *pParam);

/*
    带层参数的树节点处理函数原型
    在循环调用中，每次调用结束，参数pParam置成返回值，并带入下一次调用
    该函数保存在对象中。
*/
typedef void* (*TreeProcElemFuncLayerT)(CAdsTree* pcTree, s32 nNodeId, s32 nLayer, void *pParam);

/*
    判断两个树节点是否匹配的函数原型
    如果匹配，则返回TRUE, 否则返回FALSE
*/
typedef BOOL32 (*TreeIsTwoMatchingT)(PTreeElement pElem1, PTreeElement pElem2, void *pParam);



class STK_API CAdsTree : private CAdsArray
{
 
public:
    CAdsTree(){};
    virtual ~CAdsTree();

    //创建并初始化Tree对象
    BOOL32 Create(s32 nElemSize, s32 nMaxElemNum, const char* pchName);
    //销毁分配的单元
    void Close();


    /*********************  基本访问操作函数   **************************/

    //得到数据单元指针，失败返回NULL
    PTreeElement GetElem(s32 nLoc);

    //得到根节点，失败返回KDVERROR
    s32 GetRoot(s32 nodeId);

    //得到第N个子节点，失败返回KDVERROR
    s32 GetByIndex(s32 nParent, s32 nIndex);

    //得到父节点，失败返回KDVERROR
    s32 Parent(s32 nNode);

    //得到下一个兄弟节点，失败返回KDVERROR
    s32 Brother(s32 nNode);

    //得到第一个子节点，失败返回KDVERROR
    s32 Head(s32 nParent);

    //得到最后一个子节点，失败返回KDVERROR
    s32 Tail(s32 nParent);

    //得到子节点的序号，失败返回KDVERROR
    s32 Index(s32 nParent, s32 nChild);

    //得到子节点的数目，失败返回KDVERROR
    s32 NumChilds(s32 nParent);

    //得到下一个节点(Postorder顺序)，失败返回KDVERROR
    s32 Next(s32 nRoot, s32 nLocation);

    //得到子树的节点数，失败返回KDVERROR
    s32 TreeSize(s32 nRootId);

        
    /************************    添加    ********************************/

    //加入一个新的根节点，返回新的根节点号，失败返回KDVERROR
    s32 AddRoot(PTreeElement pElem);

    //加入第一个子节点，返回新加入节点的节点号，失败返回KDVERROR
    s32 AddHead(s32 nParent, PTreeElement pElem);

    //加入最后一个子节点，返回新加入节点的节点号，失败返回KDVERROR
    s32 AddTail(s32 nParent, PTreeElement pElem);

    //加入兄弟节点，返回新加入节点的节点号，失败返回KDVERROR
    s32 AddBrother(s32 nBrother, PTreeElement pElem);

    /*====================================================================
    函数名      ：AddTree(s32 nDestParentId, CAdsTree* pcSrcTree, s32 nSrcRootId,
                          TreeProcElemFuncT pAddFunc, void* pParam)
    功能		：把源对象pcSrcTre中指定树nSrcRootId全部复制到本树nDestParentId节点下，
                  成为nDestParentId节点的最后一个child。
                  复制过程中每次加入新节点都调用回调函数pAddFunc。
    输入参数说明：nDestParentId 目标parent节点
                  pcSrcTree     指向源树对象的指针
                  nSrcRootId    源子树的根节点
                  pAddFunc      回调函数
                  pParam        回调函数参数
    返回值说明  ：成功：新子树的根节点
                  失败：KDVERROR
    ====================================================================*/
    s32 Add(s32 nDestParentId, CAdsTree* pcSrcTree, s32 nSrcRootId, TreeProcElemFuncT pAddFunc, void* pParam)
    {
        return AddTree(nDestParentId, pcSrcTree, nSrcRootId, pAddFunc, pParam, TRUE);
    };

    /*====================================================================
    函数名      ：AddChilds(s32 nDestParentId, CAdsTree* pcSrcH, s32 nSrcRootId,
                            TreeProcElemFuncT pAddFunc, void* pParam)
    功能		：把源对象pcSrcTre中指定节点nSrcRootId下的全部子树复制到本树nDestParentId
                  下，复制过程中每次加入新节点都调用回调函数pAddFunc。
    输入参数说明：nDestParentId 目标parent节点
                  pcScRH        指向源树对象的指针
                  nSrcRootId    源子树的根节点
                  pAddFunc      回调函数
                  pParam        回调函数参数
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/
    BOOL32 AddChilds(s32 nDestParentId, CAdsTree* pcSrcH, s32 nSrcRootId,
                     TreeProcElemFuncT pAddFunc, void* pParam);


    /************************    移动   ********************************/

    /*====================================================================
    函数名      ：Set(s32 nDestParentId, CAdsTree* pcSrcH, s32 nSrcRootId, TreeProcElemFuncT pAddFunc,
                      TreeProcElemFuncT fDleteFunc, void* pParam)
    功能		：把源对象pcSrcTree中指定树nSrcRootId全部复制并替代nDestParentId树，
                  复制后nDestParentId子树和nSrcRootId子树相同。但nDestParentId节点的
                  brother和parent关系不变。原来nDestParentId节点下的节点全部删除。
    输入参数说明：nDestParentId 目标parent节点
                  pcScRH        指向源树对象的指针
                  nSrcRootId    源子树的根节点
                  pAddFunc      节点增加时的回调函数
                  pDleteFunc    节点删除时的回调函数
                  pParam        回调函数参数
    返回值说明 ： 成功：TRUE
                  失败：FALSE
    ====================================================================*/
    BOOL32 Set(s32 nDestParentId, CAdsTree* pcSrcH, s32 nSrcRootId, TreeProcElemFuncT pAddFunc,
               TreeProcElemFuncT pDleteFunc, void* pParam);

    /*====================================================================
    函数名      ：Move(s32 nDestNodeId, s32 nSrcRootId, BOOL32 bKeepSrcRoot, 
                       TreeProcElemFuncT pDleteFunc, void* pParam)
    功能		：把指定节点nSrcRootId的子树全部移动到本树nDestParentId下。
                  原nDestParentId下的节点全部删除。允许有包含关系的子树进行移动。
    输入参数说明：nDestParentId 目标parent节点
                  nSrcRootId    源子树的根节点
                  bKeepSrcRoot  是否保留nSrcRootId节点（仅指根节点）
                  pDleteFunc       节点删除时的回调函数
                  pParam        回调函数参数
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/
    BOOL32 Move(s32 nDestNodeId, s32 nSrcRootId, BOOL32 bKeepSrcRoot, 
                TreeProcElemFuncT pDleteFunc, void* pParam);

    /*====================================================================
    函数名      ：AdoptChild(s32 nAdoptedChildId, s32 nNewParentId, s32 nNewBrotherId)
    功能		：把节点移动到新的位置。
    算法实现	：
    引用全局变量：无
    输入参数说明：nAdoptedChildId   移动的节点
                  nNewParentId  新位置的parent
                  nNewBrotherId 新位置的brother
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/
    BOOL32 AdoptChild(s32 nAdoptedChildId, s32 nNewParentId, s32 nNewBrotherId);


    /************************    删除   ********************************/

    //删除子树，成功返回TRUE，失败返回FALSE
    BOOL32 Delete(s32 nRootId, TreeProcElemFuncT pDleteFunc, void *pParam);

    //删除父节点下的全部子树，成功返回TRUE，失败返回FALSE
    BOOL32 DeleteChilds(s32 nRootId, TreeProcElemFuncT pDleteFunc, void *pParam);


    /************************    查询   ********************************/

    //查找子节点，成功返回TRUE，失败返回FALSE
    s32 GetChild(s32 nParent, void *pParam, s32 nIndex);

    /*====================================================================
    函数名      ：Find(s32 nRootId, void *pParam, s32 nIndex)
    功能		：在子树nRootId中查找第nIndex个匹配的节点。
                  遍历顺序采用Postorder。
                  采用注册的比较函数进行比较，无比较函数的情况则直接比较第一个值。
    输入参数说明：nSubTreeRoot  根节点
                  pParam        比较函数的参数
                  nIndex        序号
    返回值说明  ：成功：找到的节点号
                  失败：KDVERROR
    ====================================================================*/
    s32 Find(s32 nRootId, void *pParam, s32 nIndex);

    /*====================================================================
    函数名      ：CompareTrees(s32 nDestRootId, CAdsTree* pcSrcH, s32 nSrcRootId,
                               RTECompareTwo pCompareTwoFunc, void *pParam)
    功能		：比较两棵子树的节点的值，两树都按照next方法遍历。无比较函数的时候
                  采用单元内容比较。
                  注：不能说明两个子树形状相同。
    输入参数说明：nDestRootId        目标parent节点
                  pcSrcH             源树对象
                  nSrcRootId         源子树的根节点
                  pCompareTwoFunc    比较回调函数
                  pParam             回调函数参数
    返回值说明  ：相同：TRUE
                  不同：FALSE
    ====================================================================*/
    BOOL32 CompareTrees(s32 nDestRootId, CAdsTree* pcSrcH, s32 nSrcRootId,
                        TreeIsTwoMatchingT pCompareTwoFunc, void *pParam);

    //对所有根节点执行指定操作
    void DoAllRoot(TreeProcElemFuncT pOperationFunc, void* pParam);

    /*====================================================================
    函数名      ：ProcElemsInDepth(s32 nParent, s32 nMaxLevel, void *pParam);
    功能		：处理指定深度内的所有单元，遍历顺序Preorder。
    输入参数说明：nParent         子树的根
                  nMaxLevel       最大的打印层数，负数表示无限制
                  pParam          回调函数参数
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/
    BOOL32 ProcElemsInDepth(s32 nParent, s32 nMaxLevel, void *pParam);
    
    //得到第一棵树的根，失败返回KDVERROR
    s32 Root(); 
    
    //Preorder顺序下的下一个节点，结束或失败返回KDVERROR
    s32 NextPreorder(s32 nRoot, s32 nLocation);
    
    //得到最深层最左边的节点，失败返回KDVERROR。 该函数可得到preorder遍历的起始节点
    s32 LeftMostChild(s32 nParent);


    /*************************输出继承过来的部分私有函数****************************/

    //清空
    void Clear() { CAdsArray::Clear(); }

    //取得Array的比较函数
    IsElemMatchingT GetCompareFunc() { return CAdsArray::GetCompareFunc(); }

    //设置比较函数
    void SetCompareFunc(IsElemMatchingT pFunc) { CAdsArray::SetCompareFunc(pFunc); }

    //得到单元处理函数
    TreeProcElemFuncLayerT GetProcElemFunc()
    { 
        return (TreeProcElemFuncLayerT)CAdsArray::GetProcElemFunc();
    }

    //设置单元处理函数
    void SetProcElemFunc(TreeProcElemFuncLayerT pFunc)
    { 
        CAdsArray::SetProcElemFunc((ProcessElemT)pFunc);
    }

    //得到当前使用的节点数
    s32 GetCurNum() { return CAdsArray::GetCurNum(); }

    //得到最大容量
    s32 GetMaxNum() { return CAdsArray::GetMaxNum(); }

    //取得当前空单元的数量
    s32 GetFreeNum() { return CAdsArray::GetFreeNum(); }

    //得到最大使用数
    s32 GetMaxUsage() { return CAdsArray::GetMaxUsage(); }

    //判断指定位置上的节点是否为空
    s32 IsElemVacant(s32 nLocation) { return CAdsArray::IsElemVacant(nLocation); }

    //取得统计信息
    s32 GetStatistics(TAdsStats* pStats) { return CAdsArray::GetStatistics(pStats); }

    BOOL32 GetStatistics(s32* pnTotalNum, s32* pnMaxUsage, s32* pnCurNum)
    {
        return CAdsArray::GetStatistics(pnTotalNum, pnMaxUsage, pnCurNum);
    };


private:
    //加入节点
    s32 AddNode(PTreeElement pElem, s32 nParent, s32 nBrother);
    
    //删除节点
    BOOL32 DeleteNode(s32 nLocation, TreeProcElemFuncT pDleteFunc, void *pParam);

    //设置为子节点
    s32 SetChild(s32 nParent, s32 nChild);

    //设置为兄弟节点
    s32 SetBrother(s32 nLocation, s32 nBrother);

    //把Tree对象中的子树加入
    s32 AddTree(s32 nDestParentId, CAdsTree* pcSrcTree, s32 nSrcRootId,
                TreeProcElemFuncT fadd, void* pParam, BOOL32 bCheck);

    //Postorder顺序下的下一个节点
    s32 NextPostorder(s32 nRoot, s32 nLocation);

    /* 移到子树到别处 */
    BOOL32 Move2Other(s32 nDestNodeId, s32 nSrcRootId, BOOL32 bKeepSrcRoot,
                      TreeProcElemFuncT pDleteFunc, void *pParam);

    //处理当前单元，并递归处理下一层单元，遍历顺序：Postorder
    BOOL32 RecureProcElem(s32 nParent, s32 nMaxLevel, void *pParam, 
                          TreeProcElemFuncLayerT pFunc,  u32 dwLayer);

};	








/*-----------------------------   CAdsPool      ----------------------------------
	CAdsPool（缓冲块对象）在CAdsArray的基础上进一步增加了块的分配和管理，使得更适合
用于无结构的内存块操作，以取代动态内存分配和释放。
---------------------------------------------------------------------------------*/
class CAdsPool:private CAdsArray
{
public:
    //构造函数
    CAdsPool(){};
    //析构函数
    virtual ~CAdsPool();

    /*====================================================================
    函数名      ：Create(s32 nElemSize, s32 nMaxNumOfBlocks, const s8*  pszName)
    功能		：根据输入参数，分配Pool内存，根据Pool节点的结构，确定分配
                  单元的大小
    输入参数说明：nElemSize         元素大小
                  nMaxNumOfBlocks   最大元素数量
                  pszName           Pool名字
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/
    BOOL32 Create(s32 nElemSize, s32 nMaxNumOfBlocks, const s8* pszName);
    
    //销毁分配的内存单元
    void Close();
    
    //清除Pool中的所有数据
    void Clear() { CAdsArray::Clear(); }


    /*====================================================================
    函数名      ：Alloc(s32 nSize)
    功能		：从Pool中分配空间，并把分配的空间清零
    输入参数说明：nSize         需要分配的大小
    返回值说明  ：分配的块链的起始块号
                  失败：KDVERROR
    ====================================================================*/
    s32 Alloc(s32 nSize);
    
    /*====================================================================
    函数名      ：AllocCopyExternal(const void* pSrc, s32 nSize)
    功能		：从Pool中分配空间，并用外部数据源填充
    输入参数说明：pSrc          外部数据指针，如果为NULL，用零填充
                  nSize         复制数据的数量
    返回值说明  ：成功：分配的块链的起始块号
                  失败：KDVERROR
    ====================================================================*/
    s32 AllocCopyExternal(const void* pSrc, s32 nSize);

    /*====================================================================
    函数名      ：AllocCopyInternal(CAdsPool* pcSrcPool, const void* pSrc, s32 nSize)
    功能		：从Pool中分配空间，并用指定的Pool中的单元填充。
                  采用整块拷贝。nSize可以大于源，超出部分填0。
    输入参数说明：pcSrcPool     源Pool对象
                  pSrc          源数据单元指针
                  nSize         数据的大小
    返回值说明  ：成功：分配空间的指针
                  失败：NULL
    ====================================================================*/
    s32 AllocCopyInternal(CAdsPool* pcSrcPool, s32 nSrc, s32 nSize);

    //数据块尺寸变化，nLoc起始位置号，nSize新的大小，返回起始位置号
    s32 Realloc(s32 nLoc, s32 nSize);

    //释放块链，nLoc起始位置号，成功返回TRUE，失败返回FALSE
    BOOL32 Delete(s32 nLoc);


    /*====================================================================
    函数名      ：CopyFromExternal(s32 nLoc, const void* pSrc, s32 nShift, s32 nSize)
    功能		：从外部数据源复制数据到指定位置
    输入参数说明：nLoc          第一块的位置
                  pSrc          外部数据单元指针
                  nShift        块链内的起始偏移字节数
                  nSize         要复制数据的大小
    返回值说明  ：实际复制的数据字节数
    ====================================================================*/
    s32 CopyFromExternal(s32 nLoc, const void* pSrc, s32 nShift, s32 nSize);

    /*====================================================================
    函数名      ：CopyToExternal(s32 nLoc, void* pDest, s32 nShift, s32 nSize)
    功能		：从指定位置复制数据到外部数据源
    输入参数说明：nLoc          第一块的位置
                  pDest         外部数据单元指针
                  nShift        块链内的起始偏移字节数
                  nSize         要复制数据的大小
    返回值说明  ：实际复制的数据字节数
    ====================================================================*/
    s32 CopyToExternal(s32 nLoc, void* pDest, s32 nShift, s32 nSize);

    /*====================================================================
    函数名      ：CopyInternal(s32 nDestLoc, s32 nSrcLoc, s32 nSize)
    功能		：内部两个链之间进行复制
                  说明，实际复制数据的大小是按块对齐的。
                  要求nSize是按块对齐的。
    输入参数说明：nDestLoc      目标链的起始块号
                  nSrcLoc       源链的起始块号
                  nSize         复制数据的大小
    返回值说明  ：实际复制的数据字节数
    ====================================================================*/
    s32 CopyInternal(s32 nDestLoc, s32 nSrcLoc, s32 nSize);

    //把指定块数据复制到另一个Pool对象的指定块中
    BOOL32 CopyPoolToPool(CAdsPool* pcPoolDest, s32 nSrcLoc, s32 nDestLoc, s32 nSize);

    //把块链的一段设置成指定值，nSize按块对齐
    BOOL32 Set(s32 nLoc, s8 chValue, s32 nSize);
    
    //与外部数据源进行比较
    s32 CompareExternal(s32 nLoc, void* pSrc, s32 nSize);

    //内部两个链进行比较
    s32 CompareInternal(s32 nLocA, s32 nLocB, s32 nSize);

    //得到统计信息
    BOOL32 Statistics(s32* pPoolSize, s32* pAvailableSpace, s32* pAllocatedSpace);
    
    BOOL32 GetStatistics(s32* pnTotalNum, s32* pnMaxUsage, s32* pnCurNum)
    {
        return CAdsArray::GetStatistics(pnTotalNum, pnMaxUsage, pnCurNum);
    };

    //得到指定的块链的数据大小
    s32 ChunkSize(s32 nLoc);

    //和外部Pool对象中的数据块进行比较
    s32 ComparePoolToPool(CAdsPool* pcPoolDest, s32 nSrcLoc, s32 nDestLoc, s32 nSize);

    /*====================================================================
    函数名      ：GetPtr(s32 nLoc, s32 nOffset, void **ppPointer, s32 nTotalLength)
    功能		：得到实际数据的指针
    输入参数说明：nLoc          块起始位置
                  nOffset       偏移量
                  ppPointer     返回的数据指针的指针
                  nTotalLength  整个数据的长度
    返回值说明  ：从起点开始的连续区域的长度
    ====================================================================*/
    s32 GetPtr(s32 nLoc, s32 nOffset, void **ppPointer, s32 nTotalLength);
    
private:
    BOOL32 INVALID_LOCATION(s32 nLoc);
    struct tagPoolElem* GetNode(s32 nLoc);
};


/*-----------------------------   CAdsHash      ----------------------------------
    CAdsHash（哈希表对象）在CAdsArray的基础上进一步增加了哈希表的管理，以加快索引。
键的形式和生成Hash索引值的函数都由用户提供。
---------------------------------------------------------------------------------*/

/*计算Hash Key的函数*/
typedef u32 (*PHashFuncT)(void *pKey, s32 nKeySize, s32 nHashSize);

/*元素的比较函数，比较键值是否相等*/
typedef BOOL32 (*PHashCompareFuncT)(void *pKey1, void* pKey2, u32 dwKeySize);

class CAdsHash;
//定义单元处理回调函数原型
typedef void* (*HashProcElemT)(CAdsHash* pcHash, void* pElem, void* pParam);


class STK_API CAdsHash : private CAdsArray
{
public:
    CAdsHash();
	~CAdsHash();

    //根据输入参数，分配内存，完成初始化
    BOOL32 Create(s32 nNumOfKeys, s32 nNumOfElems, 
                  PHashFuncT pHashFunc, PHashCompareFuncT pCompareFunc,
                  s32 nKeySize, s32 nElemSize, const s8* pszName=NULL);

    //销毁分配的内存
    void Close();

    //把单元增加到Hash表中
    void* Add(void*  pKey, void*  pUserElem, BOOL32 bSearchBeforeInsert=FALSE);

    //查找第一个匹配的节点
    void* Find(void* pKey);

    //查找下一个匹配的节点
    void* FindNext(void* pKey, void* pNode);

    //由节点位置得到数据元素的指针
    void* GetElement(void* pNode);

    //设置指定节点的单元数据
    BOOL32 SetElement(void* pNode, void* pUserElem);

    //得到指定节点的关键值的指针
    void* GetKey(void* pNode);

    //删除指定的节点
    BOOL32 Delete(void* pNode);
    
    //对所有单元调用默认单元处理函数进行处理，pParam在调用中循环赋值
    BOOL32 DoAll(void* pParam);


    //得到打印函数
    HashProcElemT GetProcElemFunc() 
    { 
        return (HashProcElemT)(CAdsArray::GetProcElemFunc());
    }

    //设置打印函数
    void SetProcElemFunc(HashProcElemT pFunc)
    { 
        CAdsArray::SetProcElemFunc((ProcessElemT)pFunc);
    }

    //默认的比较函数
    static BOOL32 DefaultCompare(void *pKey1, void* pKey2, u32 dwKeySize);

    //默认的Hash Key函数
    static u32 HashStr(void* pParam, s32 nParamSize, s32 nHashSize);

    /*************************输出继承过来的部分私有函数****************************/
    
    //取得当前元素的数量
    s32 GetCurNum() { return CAdsArray::GetCurNum(); }
    //取得最大容量
    s32 GetMaxNum() { return CAdsArray::GetMaxNum(); }
    //取得Array的比较函数
    IsElemMatchingT GetCompareFunc() { return CAdsArray::GetCompareFunc(); }
    //设置比较函数
    void SetCompareFunc(IsElemMatchingT pFunc) { CAdsArray::SetCompareFunc(pFunc); }
    //判断指定位置上的节点是否为空
    s32 IsElemVacant(s32 nLocation) { return CAdsArray::IsElemVacant(nLocation); }
    //取得最大的元素使用数量
    s32 GetMaxUsage() { return CAdsArray::GetMaxUsage(); }
    //取得统计信息
    s32 GetStatistics(TAdsStats* pStats) { return CAdsArray::GetStatistics(pStats); }

private:
    struct tagHashHeader* m_ptHashHeader;
    
};








/*-----------------------------   CAdsHeap      ----------------------------------
    CAdsHeap（堆对象）实现了部分堆排序功能，它按照完全二叉树的形式顺序存取节点，主
要功能是实现最小元素的获取。根节点是堆中的最小元素。
    CAdsHeap实现中已经默认了堆上的节点保存的是32位有符号数，比较函数由用户提供，出
错返回KDVERROR(-1)。
---------------------------------------------------------------------------------*/

typedef s32 (*PCmpareFunc)(s32 nNodeDataA, s32 nNodeDataB, void* pParam);
typedef s32 (*PUpdateFunc)(s32 *pnNodeData, void *pParam);


class STK_API CAdsHeap
{
public:
    CAdsHeap();
    virtual ~CAdsHeap();

    //根据输入参数，分配内存，完成Heap对象的初始化
    BOOL32 Create(s32 nHeapCapacity, PCmpareFunc pCompareFunc,
                  PUpdateFunc pUpdateFunc, void *pParam);

	//根据输入参数，在指定内存区生成Heap对象，并完成初始化
    BOOL32 CreateFrom(s32 nHeapCapacity, s8 *pchBuff, s32 nBuffSize,
                      PCmpareFunc pCompareFunc, PUpdateFunc pUpdateFunc, void *pParam);
	//销毁Heap
    void Close();
    //得到Heap的大小
	s32 GetCurNum();
	//把节点加入Heap
	s32 Insert(s32 nNodeData);
	//返回Heap顶节点，并删除顶节点
	s32 ExtractTop();
	//得到顶部节点
	s32 GetTop();
	//删除节点
	s32 DeleteNode(s32 *pnNodeData);
	//更新节点
	BOOL32 UpdateNode(s32 *pnNodeData);
	//更新堆顶
	BOOL32 UpdateTop();

    s32 Point2Pos(s32* pnNodeData);

    s32* Pos2Point(s32 nLoc);

protected:
private:
    //从结点nLoc开始重新整理heap，根结点为最小值
	void Heapify(u32 nLoc);

    //把点nSrc的值移动到nDest处，并清除点nSrc的值
	void Move(u32 dwDest, u32 dwSrc);

    //交换节点A和B两个节点
	void Swap(u32 dwNodeA, u32 dwNodeB);

    //把点dwLoc的值置为ppNode指向的值，并回调update函数
	void Set(s32 nNodeData, u32 dwLoc);

    //根据位置号得到该点的指针（ppNode）
    s32 NODEDATA(s32 nLoc);


private:
    struct tagHeapHeader* m_ptHeader;

};




/*-----------------------------   CAdsBitBuff    ----------------------------------
    CAdsBitBuff实现位缓冲的管理，只能从尾部进行操作
---------------------------------------------------------------------------------*/

class STK_API CAdsBitBuff
{
public:
    CAdsBitBuff();

    virtual ~CAdsBitBuff();


    /*====================================================================
    函数名      ：Create(s32 nBitBytes)
    功能		：根据输入参数，分配内存，并进行相应初始化
    输入参数说明：nBitBytes     Bitbuf缓冲的字节数
    返回值说明  ：失败：FALSE
                  成功：TRUE
    ====================================================================*/
    BOOL32 Create(s32 nBitBytes);

    /*====================================================================
    函数名      ：CreateFrom(s32 nBitBytes, s8 *pchBuf, s32 nBufSize)
    功能		：在外部缓冲上构建Bitbuf对象
    输入参数说明：nBitBytes     Bitbuf缓冲的字节数
                  pchBuf        外部缓冲
                  nBufSize      外部缓冲的大小
    返回值说明  ：失败：FALSE
                  成功：TRUE
    ====================================================================*/
    BOOL32 CreateFrom(s32 nBitBytes, u8 *pchBuf, s32 nBufSize);

    //关闭位对象，释放已经分配的内存
    void Close();


    /*====================================================================
    函数名      ：SetOctets(s32 nBitBytes, s32 nBitsInUse, u8 *pchBuf)
    功能		：把外部已有的位缓冲（不含头结构）赋给对象，进行管理。
    输入参数说明：nBitBytes     Bitbuf缓冲的字节数
                  nBitsInUse    已经使用的bit数量
                  pchBuf        位缓冲起始地址
    返回值说明  ：失败：FALSE
                  成功：TRUE
    ====================================================================*/
    s32 SetOctets(s32 nBitBytes, s32 nBitsInUse, u8 *pchBuf);

    //把缓冲清空
    void Clear();

    //取得位缓冲的首地址
    u8* GetBitBuff();

    //位缓冲是否是对齐的
    BOOL32 IsAligned();
    
    /*====================================================================
    函数名      ：AlignBits(s32 nLength)
    功能		：字节对齐长度nLength时要补充的位数
    输入参数说明：nLength   位长度
    返回值说明  ：返回要补充的位数
    ====================================================================*/
    s32 AlignBits(s32 nLocation);

    /*====================================================================
    函数名      ：SetAligned()
    功能		：把位缓冲置为对齐的
    返回值说明  ：TRUE:  成功
                  FALSE: 失败
    ====================================================================*/
    BOOL32 SetAligned();

    /*====================================================================
    函数名      ：SetUnaligned()
    功能		：把位缓冲置为非对齐的
    返回值说明  ：TRUE:  成功
                  FALSE: 失败
    ====================================================================*/
    BOOL32 SetUnaligned();

    /*====================================================================
    函数名      ：FreeBits()
    功能		：得到未使用的位数
    引用全局变量：无
    输入参数说明：无
    返回值说明  ：返回未使用的位数，错误返回KDVERROR
    ====================================================================*/
    s32 FreeBits();

    /*====================================================================
    函数名      ：FreeBytes()
    功能		：得到未使用的字节数
    返回值说明  ：返回未使用的字节数，错误返回KDVERROR
    ====================================================================*/
    s32 FreeBytes();
    
    /*====================================================================
    函数名      ：BitsInUse()
    功能		：得到已使用的位数
    返回值说明  ：返回已使用的位数，错误返回KDVERROR
    ====================================================================*/
    s32 BitsInUse();

    /*====================================================================
    函数名      ：BytesInUse()
    功能		：得到已使用的字节数
    返回值说明  ：返回已使用的字节数，错误返回KDVERROR
    ====================================================================*/
    s32 BytesInUse();

    /*====================================================================
    函数名      ：AddTail(u8 *pchSrc, s32 nSrcFrom, s32 nSrcBitsNum, BOOL32 bIsAligned) 
    功能		：从外部数据源复制指定数量的位加到缓冲区的尾部
    输入参数说明：pchSrc             外部数据源首地址
                  nSrcFrom           外部数据位的偏移量
                  nSrcBitsNum        要复制的位数
                  bIsAligned         是否要对齐
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/
    BOOL32 AddTail(u8 *pchSrc, s32 nSrcFrom, s32 nSrcBitsNum, BOOL32 bIsAligned);

    /*====================================================================
    函数名      ：Move(s32 nSrcOffset, s32 nBitsNum, s32 nDestOffset)
    功能		：把指定长度的数据位从一个位置复制到另一个位置，覆盖原来的数据
    输入参数说明：nSrcOffset        源数据的偏移位置
                  nBitsNum          移动的bit数
                  nDestOffset       目标数据的偏移位置
    返回值说明  ：TRUE:  成功
                  FALSE: 失败
    ====================================================================*/
    BOOL32 Move(s32 nSrcOffset, s32 nBitsNum, s32 nDestOffset);

    /*====================================================================
    函数名      ：Set(s32 nFromOffset, s32 nBitsNum, u8 *pchSrc)
    功能		：从外部数据源复制并覆盖到指定位置
    引用全局变量：无
    输入参数说明：nFromOffset       被覆盖区域的偏移位置
                  nBitsNum          覆盖的bit数
                  pchSrc             外部数据的起始地址
    返回值说明  ：TRUE:  成功
                  FALSE: 失败
    ====================================================================*/
    BOOL32 Set(s32 nFromOffset, s32 nBitsNum, u8 *pchSrc);
    
    /*====================================================================
    函数名      ：DelTail(u32 nBitsNum) 
    功能		：从尾部删除指定的位数
    输入参数说明：nBitsNum          删除的bit数
    返回值说明  ：TRUE:  成功
                  FALSE: 失败
    ====================================================================*/
    BOOL32 DelTail(s32 nBitsNum);

    /*====================================================================
    函数名      ：DelHead(s32 nBitsNum) 
    功能		：从头部删除指定的位数
    输入参数说明：dwBitsNum          删除的bit数
    返回值说明  ：TRUE:  成功
                  FALSE: 失败
    ====================================================================*/
    BOOL32 DelHead(s32 nBitsNum);
    
    BOOL32 IsOverflow();

    s32 Display();

    static s32 GetAllocationSize(s32 nTotalBytes)
    {
        return sizeof(TBitBufHeader) + nTotalBytes;
    };

private:
    typedef struct tagBitBufHeader{
        BOOL32 m_bIsAllocated;     //是否分配了内存
        s32    m_nBytesNum;        //BitBuff的字节数
        s32    m_nBitsInUse;       //已使用的bit数
        BOOL32 m_bIsAligned;       //是否是字节对齐的
        u8*    m_pchBits;          //BitBuff的起始地址
        BOOL32 m_bIsOverflow;      //是否溢出
    } TBitBufHeader, *PTBitBufHeader;

private:

    BOOL32 IsValid(){ return ((m_ptHeader == NULL) ? FALSE : TRUE); }
    


private:
    PTBitBufHeader m_ptHeader;
};






/*-----------------------------   CAdsETimer    ----------------------------------
    CAdsETimer类实现定时器的管理。外部周期性地调用check()来检查和处理定时器的到期。
这里的定时器都是循环定时器，到期后自动根据间隔时间再次加入定时器队列。需要手工调用
delete函数进行删除。
---------------------------------------------------------------------------------*/
typedef void (*ETimerHandlerT)(void *param);


class STK_API CAdsETimer
{
public:
    CAdsETimer();
    ~CAdsETimer();

    /*====================================================================
    函数名      ：Create(s32 nTimerNum)
    功能		：创建定时器池
    输入参数说明：nTimerNum     定时器的数量
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/    
    BOOL32  Create(s32 nTimerNum);

    /*====================================================================
    函数名      ：Close()
    功能		：销毁定时器池
    ====================================================================*/
    void    Close();

    /*====================================================================
    函数名      ：Add(ETimerHandler pCallback, void *pParam, u32 dwTimeOut)
    功能		：加入一个定时器
    输入参数说明：pCallback     定时器到期的回调函数
                  pParam        回调函数的参数
                  dwTimeOut     定时器的到期时间
    返回值说明  ：定时器的位置号
                  失败：KDVERROR
    ====================================================================*/    
    s32     Add(ETimerHandlerT pCallback, void *pParam, u32 dwTimeOut);

    /*====================================================================
    函数名      ：Delete(s32 nLoc)
    功能		：删除一个定时器
    输入参数说明：nLoc          定时器的位置号
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/
    BOOL32  Delete(s32 nLoc);

    /*====================================================================
    函数名      ：DeleteByValue(ETimerHandler pCallback, void* pParam)
    功能		：通过定时器的参数删除定时器
    输入参数说明：pCallback    定时器到期的回调函数
                  pParam       回调函数的参数
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/    
    s32     DeleteByValue(ETimerHandlerT pCallback, void* pParam);

    /*====================================================================
    函数名      ：Find(ETimerHandler pCallback, void *pParam)
    功能		：查找一个定时器
    输入参数说明：pCallback          定时器到期的回调函数
                  pParam             回调函数的参数 
    返回值说明  ：找到，返回定时器的位置号
                  没找到：返回KDVERROR
    ====================================================================*/
    s32     Find(ETimerHandlerT pCallback, void *pParam);

    /*====================================================================
    函数名      ：Check()
    功能		：检查定时器链表，对到期的定时器进行处理
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/    
    BOOL32  Check(); 

    /*====================================================================
    函数名      ：GetNextExpiration()
    功能		：取得下一个到期的定时器的绝对到期时间
    返回值说明  ：成功：下一个到期定时器的到期时间
                  失败：KDVERROR
    ====================================================================*/
    u32     GetNextExpiration();

    /*====================================================================
    函数名      ：GetCount()
    功能		：取得已使用定时器的数量
    返回值说明  ：已使用定时器的数量
    ====================================================================*/
    s32     GetCount();

    /*====================================================================
    函数名      ：GetParams(s32 nLoc, ETimerHandler* ppCallback, void** ppParam)
    功能		：取得定时器的参数
    输入参数说明：nLoc          定时器的位置号
                  ppCallback    定时器到期的回调函数的指针（用于返回）
                  ppParam       回调函数的参数的指针（用于返回） 
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/
    BOOL32  GetParams(s32 nLoc, ETimerHandlerT* ppCallback, void** ppParam);
    
    //得到App句柄
    void* GetTimerApp(){ return m_pTimerApp;}
    
    //设置App句柄
    void  SetTimerApp(void* pTimerApp){ m_pTimerApp = pTimerApp;}
    
private:
    /*====================================================================
    函数名      ：GetMinElement()
    功能		：取得最近要到期的定时器
    返回值说明  ：成功：返回定时器的地址
                  失败：NULL
    ====================================================================*/
    struct tagETimerNode* GetMinElement();

    /*====================================================================
    函数名      ：UpdateMinElem()
    功能		：更新定时器序列，重新排序。
    算法实现	：
    引用全局变量：无
    输入参数说明：
    返回值说明  ：成功：TRUE
                  失败：FALSE
    ====================================================================*/
    BOOL32      UpdateMinElem();

private:
    CAdsList    m_tList;
    CAdsHeap    m_tHeap;
    void*       m_pTimerApp;
};





/*-----------------------------   CAdsId     ------------------------------------
    CAdsId实现对一段整数区间的管理，提供分配、释放操作。
    主要用于分配端口号这样的情况。
---------------------------------------------------------------------------------*/
class CAdsId
{
public:
    CAdsId();
    ~CAdsId();
    
    //创建
    BOOL32 Create(s32 nFrom, s32 nTo);
    
    //关闭
    void Close();
    
    //分配一个号码
    s32 New();

    //释放一个号码
    BOOL32 Delete(s32 nId);

    //释放所有的号码
    BOOL32 DeleteAll();

    //判断是否是空闲的
    BOOL32 IsFree(s32 nId);
    
    //得到起始值
    s32 GetFrom();
    
    //得到终止值
    s32 GetTo();

private:
    struct tagIdHeader* m_pcIdHeader;

    void SetId(s32 nId);
    void ClearId(s32 nId);
    s32  GetId(s32 nId);
        

};


#endif  // _KDVADS_H_
