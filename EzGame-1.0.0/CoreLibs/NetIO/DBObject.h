/*=========== (C) Copyright 2003 SUNWOO Entertainment All rights reserved. ===========
//
// Purpose  : 데이터 베이스를 처리하는 객체
//
// $Workfile: DBObject.h
// $Date    : 2003. 12. 23
// $NoKeywords: 
=============================================================================*/

#pragma once

#include <map>
#include "database.h"

using namespace std;



#define MAP_DBLIST				map<int, CDataBase*>
#define MAP_DBLIST_IT	  		map<int, CDataBase*>::iterator


enum EXCUITPARAM
{
	SQL,
	STORED_PROC
};

class CDBObject
{
public:
	CDBObject(void);
	~CDBObject(void);



protected:
	map<int, CDataBase*>	m_DBList;
	CDataBase*				m_pCurrentDB;
	CDBStoredProc			m_StoredProc;
	CDBRecordSet			m_Record;
	EXCUITPARAM				m_cmd_type;
	static CDBObject*		m_pGobalDB;

public:
	bool DBConnect(int iDbHandle, string DataSource, string Catalog, string UserID, string PassWord);
	long Excute(ULONG uRow);
    void SetCmd(int iDbHandle, EXCUITPARAM cmd_type, char* cmd);
		bool ObjRs(int row, int column, char *pData);
	void ObjRs(int row, int column, char *pData, int iSize);
	int ObjRsInt(int row, int column);
	float ObjRsFloat(int row, int column);
	INT64 ObjRsInt64(int row, int column);
	BYTE ObjRsByte(int row, int column);
	bool ObjRsTime(int row, int column, LPSYSTEMTIME lpTime);
	int MoveNext();
	void Release(void);
	bool DBClose(int iDbHandle);
	inline static CDBObject* GetDB() { return m_pGobalDB; }

	inline void AddParam(WCHAR *pName, DBTYPEENUM type, DBPARAMFLAGSENUM ParamFlagseNum, char* value)
	{
		m_StoredProc.SetParam(pName, type, ParamFlagseNum, value);
	}

	inline CDBStoredProc* GetStoredProc()
	{
		return &m_StoredProc;
	}

};

inline CDBObject* GetDB()
{
	static CDBObject Global_DB;

	return CDBObject::GetDB();
}

