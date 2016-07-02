#pragma once
//-----------------------------------------------------------------------//
// This is a part of the KdvLib MFC Extention.							 //	
// Autor  :  Francisco Campos											 //
// (C) 2002 Francisco Campos <www.beyondata.com> All rights reserved     //
// This code is provided "as is", with absolutely no warranty expressed  //
// or implied. Any use is at your own risk.								 //		
// You must obtain the author's consent before you can include this code //
// in a software library.												 //
// If the source code in  this file is used in any application			 //
// then acknowledgement must be made to the author of this program		 //	
// fcampos@tutopia.com													 //
//-----------------------------------------------------------------------//

//从网上下载的开源代码的文件

#import "C:\Program Files\Common Files\System\ADO\msado15.dll" rename_namespace("ADOCG") rename("EOF", "EndOfFile")

using namespace ADOCG;


#include "icrsint.h"
//------------------------------------------------------------------------------------------------
class CKdvRecordSet;
class CKdvConnection
{
public:
	CKdvConnection(void);
	~CKdvConnection(void);

protected:
	_ConnectionPtr m_pConn;
public:
	BOOL Open(LPCTSTR ConnectionString,LPCTSTR UID=_T(""),LPCTSTR PWD=_T(""),long Options=-1);
	CKdvRecordSet* Execute(LPCTSTR CommandText,long Options=-1);
	long BeginTrans();
	void RollbackTrans();
	void CommitTrans();
	void Cancel();
	void Close();
	BOOL IsConnect();
	void SetConnectionTimeout(long ConnectionTimeout);
	void SetConectionString( LPCTSTR ConnectionString);
	void SetCommandTimeout(long CommandTimeout);
	long GetCommandTimeout();
	long GetConnectionTimeout();
	_ConnectionPtr GetConecction(){return m_pConn;};
	LPCTSTR GetConectionString();
	long GetState();	
};

//---------------------------------------------------------------------------------------
class CKdvField
{
protected:
	FieldPtr field;
public:
	CKdvField(void);
	~CKdvField(void);
public:

	FieldPtr GetFieldPtr(){return field;};
	void Attach(FieldPtr mField){field=mField;};
	CString GetName(){ CString Name=(LPCTSTR)field->GetName(); return Name; };
	short   GetType(){return field->GetType();};
	long    GetActualSize(){return field->GetActualSize();};
	long    GetDefinedSize(){return field->GetDefinedSize();};
	long    GetAttributes(){return field->GetAttributes();};
	CString GetOriginalVal(){_variant_t vt=field->GetOriginalValue(); return (CString) vt.bstrVal;};

	BOOL SetValue(bool boolVal);
	BOOL GetValue(bool& boolVal);

	BOOL SetValue(unsigned char bVal);
	BOOL GetValue(unsigned char& bVal);

	BOOL SetValue(unsigned short uiVal);
	BOOL GetValue(unsigned short& uiVal);

	BOOL SetValue(unsigned long ulVal);
	BOOL GetValue(unsigned long& ulVal);

	BOOL SetValue(CString strVal);
	BOOL GetValue(CString& strVal);

	BOOL SetValue(COleDateTime dtVal);
	BOOL GetValue(COleDateTime& dtVal);

	BOOL SetValue(_variant_t vt);
	BOOL GetValue(_variant_t& vt);

	CString GetUnderlyingValue() { _variant_t vt=field->UnderlyingValue; return (CString)vt.bstrVal;};

};

//--------------------------------------------------------------------------------------
class CKdvCommand;
class CKdvRecordSet
{
public:
	CKdvRecordSet(void);
	CKdvRecordSet(CKdvConnection* conn);
	~CKdvRecordSet(void);

protected:
	_RecordsetPtr m_rs;
	CKdvConnection* m_pCon;
public:
//	BOOL Open(_ConnectionPtr ActiveConnection,LPCTSTR Source= _T(""), ADOCG::CursorTypeEnum CursorType = adOpenUnspecified,ADOCG::LockTypeEnum LockType= adLockUnspecified, long Options = -1);
	BOOL Open(_ConnectionPtr ActiveConnection,LPCTSTR Source= _T(""), ADOCG::CursorTypeEnum CursorType = adOpenKeyset, ADOCG::LockTypeEnum LockType= adLockOptimistic, long Options = adCmdUnknown);
	BOOL Open(LPCTSTR Source= _T(""), ADOCG::CursorTypeEnum CursorType = adOpenKeyset, ADOCG::LockTypeEnum LockType= adLockOptimistic, long Options = adCmdUnknown);
	BOOL ReQuery();
	void MoveFirst();
	void MoveLast();
	void MoveNext();
	void MovePrevious();
	void Cancel();
	void CancelUpdate(); 
	void Close();
	BOOL IsEOF();
	BOOL IsBOF();
	long GetRecordCount();
	long GetAbsolutePage();
	void SetAbsolutePage(int nPage);
	long GetPageCount();
	long GetPageSize();
	void SetPageSize(int nSize);
	long GetAbsolutePosition();
	void SetAbsolutePosition(int nPosition);
	BOOL Find(LPCTSTR Criteria , long SkipRecords=0 , SearchDirectionEnum SearchDirection= adSearchForward,_variant_t Start=vtMissing);
	long GetNumFields();
	_RecordsetPtr GetAdoRecordSet();
	//void SetBookmark(){m_rs->Bookmark = m_rs->GetBookmark();};

	BOOL CKdvRecordSet::SetFieldValue(LPCTSTR lpName, _variant_t vtField);
	BOOL GetFormatDate(LPCTSTR lpField,CString& m_szDate, CString Format=_T("%d/%m/%Y"));
	CKdvField GetField(LPCTSTR lpField);
	CKdvField GetField(int Index);
	
	BOOL SetValue(LPCTSTR lpName, bool boolVal);
	BOOL GetValue(LPCTSTR lpName, bool& boolVal);

	BOOL SetValue(LPCTSTR lpName, unsigned char bVal);
	BOOL GetValue(LPCTSTR lpName, unsigned char& bVal);

	BOOL SetValue(LPCTSTR lpName, unsigned short uiVal);
	BOOL GetValue(LPCTSTR lpName, unsigned short& uiVal);

	BOOL SetValue(LPCTSTR lpName, unsigned long ulVal);
	BOOL GetValue(LPCTSTR lpName, unsigned long& ulVal);

	BOOL SetValue(LPCTSTR lpName, CString strVal);
	BOOL GetValue(LPCTSTR lpName, CString& strVal);

	BOOL SetValue(LPCTSTR lpName, COleDateTime dtVal);
	BOOL GetValue(LPCTSTR lpName, COleDateTime& dtVal);

	BOOL SetValue(LPCTSTR lpName, _variant_t ulVal);
	BOOL GetValue(LPCTSTR lpName, _variant_t& ulVal);

	BOOL Supports( CursorOptionEnum CursorOptions ) ;
	BOOL FindFirst(LPCTSTR Criteria);
	BOOL FindNext();
	void CKdvRecordSet::Move(long NumRecords , _variant_t Start);
	void Attach(_RecordsetPtr m_prs );
	void CancelBatch(AffectEnum AffectRecords= adAffectAll);
	void SetCacheSize(long lSizeCache){m_rs->put_CacheSize(lSizeCache);};
	void GetCacheSize() {m_rs->GetCacheSize();};
	CKdvRecordSet*  Clone(ADOCG::LockTypeEnum LockType= adLockUnspecified);
	BOOL Clone(CKdvRecordSet* pcRs, ADOCG::LockTypeEnum LockType= adLockUnspecified);
	CKdvRecordSet* NextRecordset(long RecordsAffected) ;
	BOOL AddNew();
	BOOL Update();
	BOOL Delete();
	BOOL IsOpen();
	BOOL SetFilter(LPCTSTR lpFilter);
	BOOL SetSort(LPCTSTR lpSort);
	ADOCG::EditModeEnum GetEditMode(){return m_rs->EditMode;}; 
protected:
	BOOL m_bIsOpen;
	BOOL m_bIsAddNew;
	BOOL m_bIsDelete;
	_variant_t vtPointer;
	LPCTSTR m_Criteria;
};

//----------------------------------------------------------------------------------------------

class CKdvParameter
{
public:
	CKdvParameter();
	~CKdvParameter();
protected:
	_ParameterPtr pParam;
public:
	void SetAttributes(long Attributes);
	void SetDirection(ADOCG::ParameterDirectionEnum Direction=adParamUnknown);
	void SetName(LPCTSTR szName);
	void SetNumericScale(unsigned char NumericScale);
	void SetPrecision(unsigned char Precision);
	void SetSize(long Size);
	void SetType(ADOCG::DataTypeEnum Type);
	
	BOOL SetValue(bool boolVal);
	BOOL GetValue(bool& boolVal);

	BOOL SetValue(unsigned char bVal);
	BOOL GetValue(unsigned char& bVal);

	BOOL SetValue(unsigned short uiVal);
	BOOL GetValue(unsigned short& uiVal);

	BOOL SetValue(unsigned long ulVal);
	BOOL GetValue(unsigned long& ulVal);	

	BOOL SetValue(CString strVal);
	BOOL GetValue(CString& strVal);

	BOOL SetValue(COleDateTime dtVal);
	BOOL GetValue(COleDateTime& dtVal);

	BOOL SetValue(_variant_t vt);
	BOOL GetValue(_variant_t& vt);
	
	BOOL GetFormatDate(CString& m_szDate, CString Format=_T("%d/%m/%Y"));
	_ParameterPtr GetParameter(){return pParam;};
	void Attach(_ParameterPtr param);

};

//----------------------------------------------------------------------------------------------

class CKdvCommand
{
public:
	CKdvCommand();
	~CKdvCommand();
protected:
	_CommandPtr	pCommand;
	CKdvConnection m_pCon;
public:
	void SetActiveConnection(CKdvConnection* pCon);
	void SetActiveConnection(LPCTSTR szconnec);
	void Cancel();
	void SetCommandText(LPCTSTR lpCommand,CommandTypeEnum cmdType=adCmdText);
	void SetCommandTimeout(long CommandTimeout);
	void SetPrepared(BOOL prepared);
	long GetState();
	
	BOOL Execute(CKdvRecordSet* pcRs, long Options=-1);
	BOOL Execute(long Options);
	BOOL CreateParameter(CKdvParameter* pParam, CString Name ,long Size ,ADOCG::DataTypeEnum Type= adEmpty, 
					ADOCG::ParameterDirectionEnum Direction= adParamInput) ;
	BOOL Append(CKdvParameter* pParam);
	_CommandPtr GetCommand(){return pCommand;};
	void Attach(_CommandPtr	Command) {pCommand=Command;};	
};
