/****************************************************************************
模块名      ：崩溃信息捕获模块
文件名      ：exceptioncatch.h
相关文件    ：exceptioncatch.cpp
文件实现功能：崩溃捕获
作者        ：刘晓静
版本        ：V1.0 Copyright(C) 2009-2009 KDC, All rights reserved.
备注        ：只适用于Windows OS 
用法        ：1. 在你的cpp文件中include "exceptioncatch.h" 
              2. 在主线程入口调用CExceptionCatch::Start()加载崩溃信息捕获模块
              3. 崩溃文件在程序执行目录下的KDCException.txt文件，也可以自定义输出文件CExceptionCatch::LogToFile("文件全路径")
              4. 调用CExceptionCatch::Free()卸载崩溃信息模块
-----------------------------------------------------------------------------
修改记录：
日期       版本    修改人    修改内容 
2009/03/16 1.0     刘晓静    创建
2013/07/17 2.0     邹俊龙    修改以支持UNICODE
*****************************************************************************/

#if !defined(EXCEPTION_CATCH_H)
#define EXCEPTION_CATCH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "stdafx.h"

#include <imagehlp.h>
#include <stdio.h>
#include <assert.h>
#include "unicode.h"

#pragma comment (lib, "imagehlp")

//崩溃输出文件路径
static TCHAR m_pszLogFile[MAX_PATH] = {0};

//崩溃信息Exception Filter
static LPTOP_LEVEL_EXCEPTION_FILTER m_pPreExcepAddress;

//崩溃信息输出类
class CExceptionOutput  
{
public:
	CExceptionOutput()
	{
			ZeroMemory ( m_abyFilePath, sizeof(m_abyFilePath) );
			_tcscpy( m_abyFilePath, _T("Output.txt") );
	}
	virtual ~CExceptionOutput(){}
	CExceptionOutput( LPCTSTR pch )
	{
		assert ( pch != NULL );
		_tcscpy ( m_abyFilePath, pch );
	}
	
	//设置输出文件名
	void SetFileName(LPCTSTR pch)
	{
		assert (pch != NULL);
		_tcscpy ( m_abyFilePath, pch );
	}

	LPCTSTR GetFileName()
	{
		return m_abyFilePath;
	}
	
	//输出行
	void WriteString( LPCTSTR pstr )
	{
	#ifdef UNICODE
		CreateFile();
	#endif
		FILE *fp = _tfopen( m_abyFilePath, _T("ab") );
		if ( fp != NULL )
		{
			fwrite( pstr, sizeof(TCHAR), _tcslen(pstr), fp );
			fclose( fp );
		}
	}
	
	//新增一行
	void NewLine()
	{
	#ifdef UNICODE
		CreateFile();
	#endif
		LPCTSTR lpszRtn = _T("\n");
		FILE *fp = _tfopen( m_abyFilePath, _T("ab") );
		if ( fp != NULL )
		{
			fwrite( lpszRtn, sizeof(TCHAR), _tcslen(lpszRtn), fp );
			fclose( fp );
		}
	}
	
	//获得程序执行路径
	static CString GetModuleFullPath(void)
	{
		TCHAR achDllName[MAX_PATH] = _T("");
		CString csPath = _T("");
		
		GetModuleFileName(AfxGetInstanceHandle(), achDllName, sizeof (achDllName));
		
		csPath = achDllName;
		csPath = csPath.Left( csPath.ReverseFind('\\') + 1 );
		
		return csPath;
	}

#ifdef UNICODE
	void CreateFile()
	{
		FILE *fp = _tfopen( m_abyFilePath, _T("rb+") );
		if ( fp != NULL )
		{
			fseek( fp, 0, SEEK_END );
			size_t dwSize = ftell( fp );
			fseek( fp, 0, SEEK_SET );
			if ( dwSize > 0 )
			{
				TCHAR byHead = 0;
				fread( &byHead, sizeof(TCHAR), 1, fp );
				if ( byHead != 0xFEFF )
				{
					fclose( fp );
					fp = _tfopen( m_abyFilePath, _T("wb+") );
					if ( fp )
					{
						dwSize = 0;
					}
				}			
			}
			if ( dwSize == 0 && fp )
			{
				TCHAR byUtf16Head = 0xFEFF;
				fwrite( &byUtf16Head, sizeof(TCHAR), 1, fp );
				fclose( fp );
			}
		}
	}
#endif
private:
	TCHAR m_abyFilePath[MAX_PATH];
};

//崩溃信息捕获类
class CExceptionCatch
{
public:
	static void Start()
	{
		if (NULL == m_pPreExcepAddress) 
		{
			SetErrorMode (SEM_NOGPFAULTERRORBOX);
			m_pPreExcepAddress = SetUnhandledExceptionFilter (CallBackDebugInfo);
		}
	}
	
	
	static void Free()
	{
		SetErrorMode (0);
		SetUnhandledExceptionFilter (m_pPreExcepAddress);
	}
	
	static void LogToFile( LPCTSTR pszFile)
	{
		_tcsncpy( m_pszLogFile, pszFile, _countof( m_pszLogFile ) );
	}
protected:
	static LONG WINAPI CallBackDebugInfo(EXCEPTION_POINTERS *pException)
	{
		// 得到异常的信息
		PEXCEPTION_RECORD pExcp = pException->ExceptionRecord;
		while (NULL != pExcp) 
		{
			DealExcep (pExcp);
			pExcp = pExcp->ExceptionRecord;
		}
		
		//打印堆栈信息
		ImgStackWalk (pException->ContextRecord);
		if (m_pPreExcepAddress) 
		{
			return m_pPreExcepAddress (pException);
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}
	static void	DealExcep(PEXCEPTION_RECORD pExcp)
	{
		TCHAR szFileInfo[3 * MAX_PATH] = {0};
		TCHAR szExceptionInfo[MAX_PATH] = {0};
		
		// 打印提示头
		_stprintf( szExceptionInfo, _T("======================== 崩溃信息 ==========================\n") );
		_tcscpy (szFileInfo, szExceptionInfo);
		
		// 获得当前时间
		SYSTEMTIME st;
		GetLocalTime(&st);  
		_stprintf(szExceptionInfo, _T("崩溃时间： %4d/%.2d/%.2d %.2d:%.2d:%.2d\n"),
			st.wYear, st.wMonth,  st.wDay,
			st.wHour, st.wMinute, st.wSecond);
		_tcscat(szFileInfo, szExceptionInfo);
		
		_tcscat(szFileInfo, _T("崩溃原因： "));
		
		// 判断异常原因
		switch (pExcp->ExceptionCode)
		{
		case STATUS_ACCESS_VIOLATION:
			_stprintf(szExceptionInfo, _T("非法内存操作\n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			_stprintf(szExceptionInfo, _T("数组访问越界\n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_BREAKPOINT:
			_stprintf(szExceptionInfo, _T("遇到breakpoint\n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			_stprintf(szExceptionInfo, _T("The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			_stprintf(szExceptionInfo, _T("One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO: 
			_stprintf(szExceptionInfo, _T("浮点除0. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_INEXACT_RESULT: 
			_stprintf(szExceptionInfo, _T("The result of a floating-point operation cannot be represented exactly as a decimal fraction. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			_stprintf(szExceptionInfo, _T("This exception represents any floating-point exception not included in this list. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_OVERFLOW :
			_stprintf(szExceptionInfo, _T("The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_STACK_CHECK :
			_stprintf(szExceptionInfo, _T("The stack overflowed or underflowed as the result of a floating-point operation. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_UNDERFLOW :
			_stprintf(szExceptionInfo, _T("The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION :
			_stprintf(szExceptionInfo, _T("The thread tried to execute an invalid instruction. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_IN_PAGE_ERROR :
			_stprintf(szExceptionInfo, _T("The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_INT_OVERFLOW: 
			_stprintf(szExceptionInfo, _T("The result of an integer operation caused a carry out of the most significant bit of the result. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_INVALID_DISPOSITION :
			_stprintf(szExceptionInfo, _T("(在异常处理过程中系统使用的代码)An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION :
			_stprintf(szExceptionInfo, _T("(不可持续异常,程序无法恢复执行,异常处理例程不应处理这个异常)The thread tried to continue execution after a noncontinuable exception occurred. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_PRIV_INSTRUCTION :
			_stprintf(szExceptionInfo, _T("The thread tried to execute an instruction whose operation is not allowed in the current machine mode. \n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_SINGLE_STEP :
			_stprintf(szExceptionInfo, _T("A trace trap or other single-instruction mechanism signaled that one instruction has been executed.	\n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
//		case STATUS_STACK_OVERFLOW:
/////		case EXCEPTION_STACK_OVERFLOW:
//			_stprintf (szExceptionInfo, _T("堆栈溢出\n异常代码 = %8x\n"), pExcp->ExceptionCode);
// 			break;
		case STATUS_INTEGER_DIVIDE_BY_ZERO:
			_stprintf (szExceptionInfo, _T("整型除数为0\n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		default:
			_stprintf (szExceptionInfo, _T("其它异常\n异常代码 = %8x\n"), pExcp->ExceptionCode);
			break;
		}
		
		// 异常地址
		lstrcat (szFileInfo, szExceptionInfo);
		_stprintf (szExceptionInfo, _T("\n异常地址 = 0x%.8x \n异常模块: "), pExcp->ExceptionAddress);
		lstrcat (szFileInfo, szExceptionInfo);
		
		// 得到异常所在的module
		MEMORY_BASIC_INFORMATION mem;
		VirtualQuery (pExcp->ExceptionAddress, &mem, sizeof (MEMORY_BASIC_INFORMATION));
		GetModuleFileName ((HMODULE)mem.AllocationBase, szExceptionInfo, sizeof (szExceptionInfo));
		lstrcat (szFileInfo, szExceptionInfo);
		
		// 定位异常的偏移位置(相对地址)
		PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)(mem.AllocationBase);
		PIMAGE_NT_HEADERS pNts = (PIMAGE_NT_HEADERS)((PBYTE)pDos + pDos->e_lfanew);
		PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION (pNts);
		
		// 获得偏移位置(相对地址)
		DWORD dRva = (DWORD)pExcp->ExceptionAddress - (DWORD)mem.AllocationBase;
		
		//  跟踪每个section.
		for (WORD wCnt = 0; wCnt < pNts->FileHeader.NumberOfSections; ++wCnt) 
		{
			DWORD dStart = pSection->VirtualAddress;
			DWORD dEnd = dStart + max (pSection->SizeOfRawData, pSection->Misc.VirtualSize);
			if (dRva >= dStart && dRva <= dEnd) 
			{
				_stprintf (szExceptionInfo, _T("\nSection name: %s - offset(rva) : 0x%.8x\n"), CA2T( LPCSTR( pSection->Name ) ), dRva - dStart);
				_tcscat (szFileInfo, szExceptionInfo);
				break;
			}
			++pSection;
		}
		
		// 如果没有设置文件名，赋默认值
		if ( _tcslen(m_pszLogFile) == 0 ) 
		{
			_tcscpy(m_pszLogFile,  CExceptionOutput::GetModuleFullPath() + _T("KDCException.txt"));
		} 
		
		// 输出打印到文件
		CExceptionOutput out(m_pszLogFile);
		out.WriteString (szFileInfo);
		out.NewLine();
		out.NewLine();
    }

	static void	ImgStackWalk(PCONTEXT pCon)
	{
		TCHAR szFileInfo[MAX_PATH * 2] = {0};
		TCHAR szExceptionInfo[MAX_PATH] = {0};
	
		// 先获取模块名字
		MEMORY_BASIC_INFORMATION mem;
		VirtualQuery((PVOID)pCon->Eip, &mem, sizeof (MEMORY_BASIC_INFORMATION));
		GetModuleFileName( (HMODULE)mem.AllocationBase, szExceptionInfo, _countof(szExceptionInfo) );
		_tcscpy(szFileInfo, szExceptionInfo);
		
		
		if (SymInitialize(GetCurrentProcess (), NULL, TRUE)) 
		{
			STACKFRAME sf;
			memset(&sf, 0x00, sizeof (STACKFRAME));
			
			// 初始化STACKFRAME（表示当前线程的调用堆栈中的一个函数调用）结构，只适用于Intel CPU
			sf.AddrPC.Offset       = pCon->Eip;
			sf.AddrPC.Mode         = AddrModeFlat;
			sf.AddrStack.Offset    = pCon->Esp;
			sf.AddrStack.Mode      = AddrModeFlat;
			sf.AddrFrame.Offset    = pCon->Ebp;
			sf.AddrFrame.Mode      = AddrModeFlat;
			
			
			while (true) 
			{
				if (!StackWalk (IMAGE_FILE_MACHINE_I386,
					GetCurrentProcess (),
					GetCurrentThread (),
					&sf,
					pCon,
					NULL,
					SymFunctionTableAccess,
					SymGetModuleBase,
					NULL))
				{
					break;
				}
				
				
				// 检查堆栈帧，不合理退出
				if ( 0 == sf.AddrFrame.Offset ) 
				{
					break;
				}
				
				// 申请image缓冲
				BYTE imgBuf[sizeof (IMAGEHLP_SYMBOL) + 512];
				PIMAGEHLP_SYMBOL pSymbol = reinterpret_cast <PIMAGEHLP_SYMBOL> (imgBuf);
				pSymbol->SizeOfStruct = sizeof (IMAGEHLP_SYMBOL);
				pSymbol->MaxNameLength = 512;
				DWORD dLen = 0;
				if (SymGetSymFromAddr (GetCurrentProcess(), sf.AddrPC.Offset,
					&dLen, pSymbol))
				{
					_stprintf (szExceptionInfo, _T("\nname : %s - location: %x"), pSymbol->Name, dLen);
					_tcscat (szFileInfo, szExceptionInfo);
				}
			}
			SymCleanup (GetCurrentProcess ());
		}
		
		//如果没有设置文件名，赋默认值
		if ( _tcslen( m_pszLogFile ) == 0 ) 
		{
			_tcscpy( m_pszLogFile, CExceptionOutput::GetModuleFullPath() + _T("KDCException.txt") );
		} 
		
		//输出到文件
		CExceptionOutput out(m_pszLogFile);
		out.WriteString( _T("---------------------- Trips of Stack ----------------------\n") );
		out.WriteString( szFileInfo );
		out.NewLine();
		out.NewLine();
	}
protected:
	///static	LPTOP_LEVEL_EXCEPTION_FILTER m_pPreExcepAddress;
	///static	char	m_pszLogFile[MAX_PATH];
	
};

#endif // !defined(EXCEPTION_CATCH_H)
