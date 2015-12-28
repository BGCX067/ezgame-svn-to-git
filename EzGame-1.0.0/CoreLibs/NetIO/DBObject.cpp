/*=======================================================================================
//
// Purpose  : 데이터 베이스를 처리하는 객체
//
// $Workfile: DBObject.cpp
// $Date    : 2003. 12. 23
// $NoKeywords: 
======================================================================================*/

#include "stdafx.h"
#include "DBObject.h"


CDBObject* CDBObject::m_pGobalDB   = NULL;


CDBObject::CDBObject(void)
{
    m_pCurrentDB = NULL;
	m_cmd_type = SQL ;
	m_pGobalDB = this;
}

CDBObject::~CDBObject(void)
{
	m_pCurrentDB = NULL;
}

bool CDBObject::DBConnect(int iDbHandle, string DataSource, string Catalog, string UserID, string PassWord)
{
	CDataBase*  pDB = new CDataBase;
	HRESULT  hr;

	WCHAR wDataSource[500], wCatalog[500], wUserID[500], wPassWord[500];
	int nCount;

	ZeroMemory(wDataSource, 500);
	ZeroMemory(wCatalog, 500);
	ZeroMemory(wUserID, 500);
	ZeroMemory(wPassWord, 500);


	nCount =  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
		DataSource.c_str(), 
		(int) DataSource.length(), 
		wDataSource, (int) DataSource.length() * 2); 


	nCount =  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
		Catalog.c_str(), 
		(int) Catalog.length(), 
		wCatalog, (int) DataSource.length() * 2); 
	

	nCount =  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
		UserID.c_str(), 
		(int) UserID.length(), 
		wUserID, (int) UserID.length()); 
	

	nCount =  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
		PassWord.c_str(), 
		(int) PassWord.length(), 
		wPassWord, (int) PassWord.length() * 2); 
	


	

	hr = pDB->EstablishConnection(wDataSource, wCatalog, wUserID,  wPassWord); 
	
	if(SUCCEEDED(hr))
	{
		m_DBList.insert(MAP_DBLIST::value_type(iDbHandle, pDB));
		return true;
	}

	else
	{
				
		printf("adas = %ld", hr);
		delete pDB;
        return false;
	}


		return true;
}


////////////////////////////////////////////////////////////////////////
// 디비 접속을 해제 한다.
////////////////////////////////////////////////////////////////////////
bool CDBObject::DBClose(int iDbHandle)
{
	MAP_DBLIST_IT	Iter;
	CDataBase	*	pDB;

    Iter = m_DBList.find(iDbHandle);

	if (Iter != m_DBList.end() ) 
	{
        pDB = (*Iter).second ;
		m_DBList.erase(Iter);
		pDB->Close();

		delete pDB;

		return true;
	}
    else  
		return false;
}


long CDBObject::Excute(ULONG uRow)
{
	HRESULT  hr;

	switch(m_cmd_type)
	{
        case SQL:
			hr = m_Record.Execute(uRow);
		
			if(FAILED(hr))
                return -1;

			return m_Record.GetMaxRows();
		
		break;
		
		case STORED_PROC:
            hr = m_StoredProc.Execute(uRow);

			if(FAILED(hr))
			{
                return -1;
			}

			return m_StoredProc.GetMaxRows();

		break;
	}


	return 0;
}

void CDBObject::SetCmd(int iDbHandle, EXCUITPARAM cmd_type, char* cmd)
{
    m_cmd_type = cmd_type;

	MAP_DBLIST_IT	Iter;
	CDataBase*	pDB;

    Iter = m_DBList.find(iDbHandle);

	if (Iter != m_DBList.end() ) 
        pDB = (*Iter).second ;
    else  
		return;

	WCHAR SQL_cmd[1024];
	int nCount =  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
		cmd, 
		(int) strlen(cmd), 
		SQL_cmd, (int) strlen(cmd)); 
	
	SQL_cmd[nCount] = 0; 


	switch(m_cmd_type)
	{
	case SQL:
		m_Record.SetDataBase(pDB);
		m_Record.SetCommand(SQL_cmd);
		break;
		
	case STORED_PROC:
		m_Record.SetDataBase(pDB);
        m_StoredProc.SetCmdString(pDB, SQL_cmd);
		break;
	}
}

bool CDBObject::ObjRs(int row, int column, char *pData)
{
		
	switch(m_cmd_type)
	{
	case SQL:
		return m_Record.ObjRs(row, column, pData);
		break;
		
	case STORED_PROC:
        return  m_StoredProc.ObjRs(row, column, pData);
		break;
	}

	return NULL;
}


bool CDBObject::ObjRsTime(int row, int column, LPSYSTEMTIME lpTime)
{
	switch(m_cmd_type)
	{
	case SQL:
		return m_Record.ObjRsTime(row, column, lpTime);
	
	case STORED_PROC:
        return m_StoredProc.ObjRsTime(row, column, lpTime);
	}

	return false;
}

BYTE CDBObject::ObjRsByte(int row, int column)
{
	switch(m_cmd_type)
	{
	case SQL:
		return m_Record.ObjRsByte(row, column);
	
	case STORED_PROC:
        return m_StoredProc.ObjRsByte(row, column);
	}

	return 0;
}


void CDBObject::ObjRs(int row, int column, char *pData, int iSize)
{
	ZeroMemory(pData, iSize);
		
	switch(m_cmd_type)
	{
	case SQL:
		m_Record.ObjRs(row, column, pData, iSize);
		break;
		
	case STORED_PROC:
        m_StoredProc.ObjRs(row, column, pData, iSize);
		break;
	}

}

int CDBObject::ObjRsInt(int row, int column)
{
		
	switch(m_cmd_type)
	{
	case SQL:
		return m_Record.ObjRsInt(row, column);
	
	case STORED_PROC:
        return m_StoredProc.ObjRsInt(row, column);
	}

	return 0;

}

float CDBObject::ObjRsFloat(int row, int column)
{
		
	switch(m_cmd_type)
	{
	case SQL:
		return m_Record.ObjRsFloat(row, column);
	
	case STORED_PROC:
        return m_StoredProc.ObjRsFloat(row, column);
	}

	return 0;

}

INT64 CDBObject::ObjRsInt64(int row, int column)
{
		
	switch(m_cmd_type)
	{
	case SQL:
		return m_Record.ObjRsInt64(row, column);
	
	case STORED_PROC:
        return m_StoredProc.ObjRsInt64(row, column);
	}

	return 0;

}



int CDBObject::MoveNext()
{
	switch(m_cmd_type)
	{
	case SQL:
		return m_Record.MoveNext();
		
	case STORED_PROC:
        return m_StoredProc.MoveNext();
	}

	return 0;
}

void CDBObject::Release(void)
{
	switch(m_cmd_type)
	{
	case SQL:
		m_Record.Release();
		break;
		
	case STORED_PROC:
        m_StoredProc.Release();
		break;
	}
}
