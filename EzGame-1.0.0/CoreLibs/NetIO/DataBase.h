////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 파일이름 : DataBase.h
//
// 릴 리 즈 : 2003 . 6.
//
// 제 작 자 : 박 종 찬
//
// 설    명 : MS-SQL 스토어 프로시저 처리 루틴 
//
// 저 작 권 : Copyright SUNWOO Entertainment Co. 2003.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include <oledb.h>
#include <oledberr.h>
#include <MAPIDBG.H>

// 데이터 베이스 연결 설정
#define MAX_PARAM					 100
#define MAX_COL_SIZE				 5000
#define MAX_NAME_LEN				 256

#define MAX_ROWS                     10
#define MAX_DISPLAY_SIZE             20
#define MIN_DISPLAY_SIZE			 3
#define MAX_DATA_SIZE				 1000
#define RECORD_EOF				     0
#define RECORD_NOTEOF				 1


typedef struct tagDBPropert
{
    WCHAR *wDataSource;
	WCHAR *wCatalog;
	WCHAR *wUserID;
	WCHAR *wPassWord;

} DBPropert;

#define __LONGSTRING(string) L##string
#define LONGSTRING(string) __LONGSTRING(string)
// Goes to CLEANUP on Failure_
#define CHECK_HR(hr)            \
   if(FAILED(hr))               \
   {                            \
      goto CLEANUP;            \
   }

#define CHECK_MEMORY(hr, pv)   \
{                              \
   if(!pv)                     \
   {                           \
      hr = E_OUTOFMEMORY;      \
      CHECK_HR(hr);            \
   }                           \
}


// ROUNDUP on all platforms pointers must be aligned properly
#define ROUNDUP_AMOUNT            8
#define ROUNDUP_(size,amount) (((ULONG)(size)+((amount)-1))&~((amount)-1))
#define ROUNDUP(size)             ROUNDUP_(size, ROUNDUP_AMOUNT)


enum
{
   // Connecting
   USE_PROMPTDATASOURCE      = 0x0001,
   USE_ENUMERATOR            = 0x0002,

   // Rowset
   USE_COMMAND               = 0x0010,

   // Storage objects
   USE_ISEQSTREAM            = 0x0100,

   // Display options
   DISPLAY_METHODCALLS       = 0x1000,
   DISPLAY_INSTRUCTIONS      = 0x2000,
};


class CDataBase
{
public:
	CDataBase(void);
	~CDataBase(void);
	HRESULT EstablishConnection(WCHAR *wDataSource, WCHAR *wCatalog, WCHAR *wUserID, WCHAR *wPassWord);
	static const ULONG	nInitProps			= 4;
	static const ULONG	nPropSet			= 1;
    HRESULT CreateCommandText(ICommandText** pICommandText);
	void Close();

protected:
	IDBInitialize*			m_pIDBInitialize;
	IDBCreateSession*		m_pIDBCreateSession;
	IDBCreateCommand*		m_pIDBCreateCommand;
	HRESULT CreateSession(void);

	static DBPROP			m_InitProperties[4];
	static DBPROPSET		m_rgInitPropSet[1];
};


class CDBRecordSet
{

public:
	CDBRecordSet(void)
	{
		m_pBind = NULL;
		m_pIRowset = NULL;
	}
	~CDBRecordSet(void)
	{
		Release();
	}


protected:
    typedef struct TagBIND
	{
		WCHAR   szValue[MAX_DATA_SIZE];
		DWORD   dwStatus;
	} BIND;

	
protected:
	WCHAR			   m_wCmdString[1024];
	CDataBase*		   m_pDataBase;
	ULONG			   m_iCount;
	ULONG              m_iMaxRows;
    ULONG              m_iColum;
	ULONG			   m_iRow;
	LONG               m_cRows;


	IRowset*			m_pIRowset;
	BIND*                m_pBind;

	HRESULT			GetRowset(ULONG iRowParam);
	HRESULT			SetupBindings(ULONG* pcBindings, DBBINDING** prgBindings, ULONG* pcbRowSize);
	HRESULT			GetRow(ULONG iRow, ULONG cBindings, DBBINDING* rgBindings, void* pData);
	void					FreeBindings(ULONG cBindings, DBBINDING* rgBindings);


public:
	bool ObjRs(int row, int column, char *pData);
	void ObjRs(int row, int column, char *pData, int iSize);
	int ObjRsInt(int row, int column);
	float ObjRsFloat(int row, int column);
	INT64 ObjRsInt64(int row, int column);
	BYTE ObjRsByte(int row, int column);
	bool ObjRsTime(int row, int column, LPSYSTEMTIME lpTime);

	inline void SetCommand(WCHAR* Cmd)
	{
		wcscpy(m_wCmdString, Cmd);
	}

	inline void SetDataBase(CDataBase*	pDataBase)
	{
		m_pDataBase = pDataBase;
		delete [] m_pBind;

	}

	HRESULT	Execute(ULONG iRow);
	int MoveNext();
	void Release(void);

	inline ULONG GetColumMax()
	{
		return m_iColum;

	}

	inline int GetMaxRows()
	{
		return m_iMaxRows;
	}
};



class CDBStoredProc
{

public:
	typedef struct TagBIND
	{
		WCHAR   szValue[MAX_DATA_SIZE];
		DWORD   dwStatus;
		int		iSize;
	} BIND;

	typedef struct TagPROCPARAMS
	{
		long  iReturn;
		char  ParamValue[5000];

	} PROCPARAMS;
	

public:
	CDBStoredProc(void);
	~CDBStoredProc(void);
	void SetParam(WCHAR* pName, DBTYPEENUM type, DBPARAMFLAGSENUM ParamFlagseNum, char* value);
	void SetCmdString(CDataBase* pDataBase, WCHAR* cmd);
    char* GetParam(LPCWSTR pwszName);
	long GetReturn();
	void Release(void);
	bool ObjRs(int row, int column, char *pData);
	void ObjRs(int row, int column, char *pData, int iSize);
	int ObjRsInt(int row, int column);
	float ObjRsFloat(int row, int column);
	INT64 ObjRsInt64(int row, int column);
	bool ObjRsTime(int row, int column, LPSYSTEMTIME lpTime);
	BYTE ObjRsByte(int row, int column);
	int MoveNext();
	HRESULT	Execute(ULONG iRow);


	inline ULONG GetColumMax()
	{
		return m_iColum;
	}

	inline ULONG GetMaxRows()
	{
		return m_iMaxRows;
	}



	
protected:
	HRESULT	GetRow(ULONG iRow, ULONG cBindings, DBBINDING* rgBindings, void* pData);
//	HRESULT	GetProperty(REFIID riid, DBPROPID dwPropertyID, REFGUID guidPropertySet, BOOL* pbValue);
    HRESULT	SetupBindings(ULONG* pcBindings, DBBINDING** prgBindings, ULONG* pcbRowSize);
	HRESULT	GetRowset();
	HRESULT	FindColumn(LPCWSTR pwszName, LONG *plIndex);
	void			FreeBindings(ULONG cBindings, DBBINDING* rgBindings);


protected:
	WCHAR					m_wCmdString[1024];
	DBPARAMBINDINFO	m_pParamBindInfo[MAX_PARAM];
	ULONG					m_pParamOrdinals[MAX_PARAM];
	DBBINDING				m_pACDBBinding[MAX_PARAM];
	DBBINDSTATUS			m_pACDBBindStatus[MAX_PARAM];

	ULONG					m_nParam;
	LONG					m_cNumRows;
	ULONG					m_nSizeParam;
	CDataBase*				m_pDataBase;
	ULONG					m_iCount;
    IRowset*				m_pIRowset;
	LONG					m_cRows;
	ULONG					m_iMaxRows;
    ULONG					m_iColum;
	PROCPARAMS				m_SetParam;

	BIND*						m_pBind;

	// 파라미터 셋업을 위해 필요한값
	int								m_n;
	DBBYTEOFFSET			m_ParamOffSet;

};


