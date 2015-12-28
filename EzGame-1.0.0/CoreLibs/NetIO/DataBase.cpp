////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 파일이름 : DataBase.cpp
//
// 릴 리 즈 : 2003 . 6.
//
// 제 작 자 : 박 종 찬
//
// 설    명 : MS-SQL 스토어 프로시저 처리 루틴 
//
//            MSDN의 루틴을 나의 루틴으로 클레스화  자세한 동작법은 (MADC 부분참고)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"


#define DBINITCONSTANTS
#define INITGUID
#include <SQLOLEDB.h>
#include "database.h"




DBPROP CDataBase::m_InitProperties[4];
DBPROPSET CDataBase::m_rgInitPropSet[1];




CDataBase::CDataBase(void)
{
	m_pIDBInitialize			= NULL;
	m_pIDBCreateSession         = NULL;
	m_pIDBCreateCommand         = NULL;
}

CDataBase::~CDataBase(void)
{



}

void CDataBase::Close()
{
	m_pIDBCreateCommand->Release();
	m_pIDBCreateSession->Release();

	if(m_pIDBInitialize != NULL)
	{

		if(FAILED(m_pIDBInitialize->Uninitialize()))
		{
//			error1("Problem uninitializing.");
		} //endif

		m_pIDBInitialize->Release();
	}
}

HRESULT CDataBase::EstablishConnection(WCHAR *wDataSource, WCHAR *wCatalog, WCHAR *wUserID, WCHAR *wPassWord)
{
	HRESULT				hr;
	int					i;
	IDBProperties*		pIDBProperties = NULL;

    //SQLOLEDB provider 접근 
    CHECK_HR(hr = CoCreateInstance(
                    CLSID_SQLOLEDB, 
                    NULL, 
                    CLSCTX_INPROC_SERVER,
                    IID_IDBInitialize, 
                    (void **) &m_pIDBInitialize));

    /*
    Initialize the property values needed
    to establish the connection.
    */
    for(i = 0; i < nInitProps; i++)
        VariantInit(&m_InitProperties[i].vValue);
    
	
    //Specify server name.
    m_InitProperties[0].dwPropertyID = DBPROP_INIT_DATASOURCE;
    m_InitProperties[0].vValue.vt = VT_BSTR;
    m_InitProperties[0].vValue.bstrVal = 
								SysAllocString(wDataSource);
    m_InitProperties[0].dwOptions = DBPROPOPTIONS_REQUIRED;
    m_InitProperties[0].colid = DB_NULLID;

    //Specify database name.
    m_InitProperties[1].dwPropertyID = DBPROP_INIT_CATALOG;
    m_InitProperties[1].vValue.vt = VT_BSTR;
    m_InitProperties[1].vValue.bstrVal =
								SysAllocString(wCatalog);
    m_InitProperties[1].dwOptions = DBPROPOPTIONS_REQUIRED;
    m_InitProperties[1].colid = DB_NULLID;

    //Specify username (login).
    m_InitProperties[2].dwPropertyID = DBPROP_AUTH_USERID; 
    m_InitProperties[2].vValue.vt = VT_BSTR;
    m_InitProperties[2].vValue.bstrVal = 
								SysAllocString(wUserID);
    m_InitProperties[2].dwOptions = DBPROPOPTIONS_REQUIRED;
    m_InitProperties[2].colid = DB_NULLID;

    //Specify password.
    m_InitProperties[3].dwPropertyID = DBPROP_AUTH_PASSWORD;
    m_InitProperties[3].vValue.vt = VT_BSTR;
    m_InitProperties[3].vValue.bstrVal =
								SysAllocString(wPassWord);
    m_InitProperties[3].dwOptions = DBPROPOPTIONS_REQUIRED;
    m_InitProperties[3].colid = DB_NULLID;

    m_rgInitPropSet[0].guidPropertySet = DBPROPSET_DBINIT;
    m_rgInitPropSet[0].cProperties = 4;
    m_rgInitPropSet[0].rgProperties = m_InitProperties;

	// properties 초기화.
    CHECK_HR(hr = m_pIDBInitialize->QueryInterface(
                                IID_IDBProperties, 
                                (void **)&pIDBProperties));

    CHECK_HR(hr = pIDBProperties->SetProperties(
                                nPropSet, 
                                m_rgInitPropSet));

	pIDBProperties->Release();
   
    //Now establish a connection to the data source.
    CHECK_HR( hr = m_pIDBInitialize->Initialize());


	CHECK_HR( hr = CreateSession());

 	
	return hr;
		
CLEANUP:		

	if(m_pIDBInitialize != NULL)
        m_pIDBInitialize->Release();
    
	return hr;
	}


/////////////////////////////////////////////////////////
// 세션 연결 확립
////////////////////////////////////////////////////////
HRESULT CDataBase::CreateSession(void)
{

	HRESULT hr;
    //Create a new activity from the data source object.
    CHECK_HR( hr = m_pIDBInitialize->QueryInterface(
                                    IID_IDBCreateSession,
                                    (void**) &m_pIDBCreateSession));

	CHECK_HR( hr = m_pIDBCreateSession->CreateSession(
                                     NULL, 
                                     IID_IDBCreateCommand, 
                                     (IUnknown**) &m_pIDBCreateCommand));

	return hr;

CLEANUP:
	m_pIDBCreateSession->Release();
	m_pIDBInitialize->Release();
	return hr;
}


HRESULT CDataBase::CreateCommandText(ICommandText** pICommandText)
{
	HRESULT hr;

    //Create a Command object.
    CHECK_HR(hr = m_pIDBCreateCommand->CreateCommand(
                                        NULL, 
                                        IID_ICommandText, 
                                        (IUnknown**) pICommandText));

	return hr;

CLEANUP:
	m_pIDBCreateCommand->Release();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////

CDBStoredProc::CDBStoredProc(void)
{
	m_pBind             = NULL;
	m_pIRowset          = NULL;
	m_nParam            = 0;
	m_nSizeParam        = 0;
	m_cNumRows		    = 0;
	m_cRows             = 0;
	m_iMaxRows          = 0;
	m_iColum            = 0;
}

CDBStoredProc::~CDBStoredProc(void)
{
	Release();
}

BYTE CDBStoredProc::ObjRsByte(int row, int column)
{
	WCHAR*	pwstr;
	char Temp[10];
    int n;

	ZeroMemory(Temp, 10);

	pwstr = m_pBind[row * m_iColum + column].szValue;
	
    n = WideCharToMultiByte(CP_ACP, 0, pwstr, 10, Temp, 10, NULL, NULL);

	return Temp[0];
}


bool CDBStoredProc::ObjRsTime(int row, int column, LPSYSTEMTIME lpTime)
{
	char TimeStmp[30];
	char Buff[10];
	int Start = 0;
	int iIndex = 0;

	ZeroMemory(lpTime, sizeof(SYSTEMTIME));

	if(ObjRs(row, column, TimeStmp))
	{
		// 년도

		for(int n = 0; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == '-')
			{
				Buff[iIndex] = NULL;
				lpTime->wYear = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		// 월
		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == '-')
			{
				Buff[iIndex] = NULL;
				lpTime->wMonth = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		// 일
		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == ' ')
			{
				Buff[iIndex] = NULL;
				lpTime->wDay = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == ':')
			{
				Buff[iIndex] = NULL;
				lpTime->wHour = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == ':')
			{
				Buff[iIndex] = NULL;
				lpTime->wMinute = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == '.')
			{
				Buff[iIndex] = NULL;
				lpTime->wSecond = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == NULL)
			{
				Buff[iIndex] = NULL;
				lpTime->wMilliseconds = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		return true;

	}

	return false;

}

 bool CDBStoredProc::ObjRs(int row, int column, char *pData)
{
	WCHAR*	pwstr;
	int n;


	if(m_pBind == NULL)
		return false;

	pwstr = m_pBind[(row * m_iColum) + column].szValue;

	n = WideCharToMultiByte(CP_ACP, 0, pwstr, (int) wcslen(pwstr), pData,  m_pBind[(row * m_iColum) + column].iSize, NULL, NULL);

	pData[n] = '\0';

	return true;
}

void CDBStoredProc::ObjRs(int row, int column, char *pData, int iSize)
{
	WCHAR*	pwstr;
	int n;

	if(m_pBind == NULL)
		return;

	ZeroMemory(pData, iSize);

	pwstr = m_pBind[(row * m_iColum) + column].szValue;

    n = WideCharToMultiByte(CP_ACP, 0, pwstr, iSize, pData,  iSize, NULL, NULL);

}

int CDBStoredProc::ObjRsInt(int row, int column)
{
	WCHAR*	pwstr;
	char Temp[50];
	ZeroMemory(Temp, 50);
    int n;

	pwstr = m_pBind[row * m_iColum + column].szValue;
	
    n = WideCharToMultiByte(CP_ACP, 0, pwstr, 50, Temp, 50, NULL, NULL);

	return atoi(Temp);
}

float CDBStoredProc::ObjRsFloat(int row, int column)
{
	WCHAR*	pwstr;
	char Temp[50];
	ZeroMemory(Temp, 50);
    int n;

	pwstr = m_pBind[row * m_iColum + column].szValue;
	
    n = WideCharToMultiByte(CP_ACP, 0, pwstr, 50, Temp, 50, NULL, NULL);

	return atof(Temp);
}

INT64 CDBStoredProc::ObjRsInt64(int row, int column)
{
	WCHAR*	pwstr;
	char Temp[50];
	ZeroMemory(Temp, 50);
    int n;

	pwstr = m_pBind[row * m_iColum + column].szValue;
	
    n = WideCharToMultiByte(CP_ACP, 0, pwstr, 50, Temp, 50, NULL, NULL);

	return _atoi64(Temp);
}



int CDBStoredProc::MoveNext()
{
	if(S_OK != GetRowset())
		return RECORD_EOF;
	
	else 
		return m_cRows;
}


////////////////////////////////////////////////////////////////////
// 스토어 프로시저 명령어 실행
/////////////////////////////////////////////////////////////////////
HRESULT CDBStoredProc::Execute(ULONG iRow)
{
	HRESULT						hr;
	DBPARAMS					Params;
	HACCESSOR				    hAccessor			 = DB_NULL_HACCESSOR;
	ICommandText*				pICommandText        = NULL;
	IAccessor*					pIAccessor           = NULL;
	ICommandWithParameters*		pICommandWithParams  = NULL;


	//Create a Command object.
	CHECK_HR(hr = m_pDataBase->CreateCommandText(&pICommandText));

    
	//Set the command text.
	CHECK_HR(hr = pICommandText->SetCommandText(DBGUID_DBSQL, m_wCmdString));

    //Set the parameters information.
    CHECK_HR(hr = pICommandText->QueryInterface(
                                    IID_ICommandWithParameters,
                                    (void**)&pICommandWithParams));


    CHECK_HR(hr = pICommandWithParams->SetParameterInfo(
										m_nParam, 
										m_pParamOrdinals, 
										m_pParamBindInfo));
	
    //Create an accessor from the above set of bindings.
    CHECK_HR(hr = pICommandWithParams->QueryInterface(
                                    IID_IAccessor, 
                                    (void**)&pIAccessor));

    
	CHECK_HR(hr = pIAccessor->CreateAccessor(
                            DBACCESSOR_PARAMETERDATA,
							m_nParam,
							m_pACDBBinding,
                            m_nSizeParam, 
                            &hAccessor,
							m_pACDBBindStatus));


	// 스토어 프로시저 파라 미터 값을 지정한다.
    Params.pData = &m_SetParam;			// 파라미터 구조체
    Params.cParamSets = 1;                              
    Params.hAccessor = hAccessor;
    
    //Execute the command.
    CHECK_HR(hr = pICommandText->Execute(
                                    NULL, 
                                    IID_IRowset, 
                                    &Params, 
                                    &m_cNumRows, 
                                    (IUnknown **) &m_pIRowset));

	if(m_pIRowset == NULL)
	{
		m_cRows =0;
		m_iMaxRows = 0;
    	goto CLEANUP;
	}

	if(iRow > 0 )
	{
		m_iCount = 0;
		m_cRows = iRow;
		GetRowset();
	}

CLEANUP:
	if(pIAccessor != NULL)
	{
        pIAccessor->ReleaseAccessor(hAccessor, NULL);
	}

	if(pICommandWithParams != NULL)
        pICommandWithParams->Release();

	if(pICommandText != NULL)
        pICommandText->Release();

	if(pIAccessor != NULL)
        pIAccessor->Release();
	return hr;
}


///////////////////////////////////////////////////////////////////////////////////////
// 레코드를 읽은 후 생성된 BIND 객체와 연결한다.
//////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBStoredProc::GetRow(ULONG iRow, ULONG cBindings, DBBINDING* rgBindings, void* pData)
{
   HRESULT               hr = S_OK;
   DBSTATUS              dwStatus;
   ULONG                 ulLength;
   void *                pvValue = NULL;
   ULONG                 iCol;
   ULONG                 cbRead;
   ISequentialStream *   pISeqStream = NULL;
   
   
   // For each column that we have bound, display the data.
   for( iCol = 0; iCol < cBindings; iCol++ )
   {
      // We have bound status, length, and the data value for all
      // columns, so we know that these can all be used.
      dwStatus   = *(DBSTATUS *)((BYTE *)pData + rgBindings[iCol].obStatus);
      ulLength   = *(ULONG *)((BYTE *)pData + rgBindings[iCol].obLength);
      pvValue    = (BYTE *)pData + rgBindings[iCol].obValue;

      // Check the status of this column. This decides
      // exactly what will be displayed for the column.
      switch( dwStatus )
      {
         // The data is NULL, so don't try to display it.
         case DBSTATUS_S_ISNULL:
            wcscpy(m_pBind[m_iCount].szValue, L"");
			m_pBind[m_iCount].iSize = 0;
            break;

         // The data was fetched, but may have been truncated.
         // Display string data for this column to the user.
         case DBSTATUS_S_TRUNCATED:
         case DBSTATUS_S_OK:
         case DBSTATUS_S_DEFAULT:
         {
            // We have bound the column either as a Unicode string
            // (DBTYPE_WSTR) or as an ISequentialStream object
            // (DBTYPE_IUNKNOWN), and we have to do different processing
            // for each one of these possibilities.
            switch( rgBindings[iCol].wType )
            {
               case DBTYPE_WSTR:
               {   
                  // Copy the string data.
				  wcsncpy(m_pBind[m_iCount].szValue, (WCHAR *)pvValue, ulLength);
                  m_pBind[m_iCount].szValue[ulLength] = L'\0';
				  m_pBind[m_iCount].iSize = ulLength;
                  break;
               }

               case DBTYPE_IUNKNOWN:
               {
                  // We've bound this as an ISequentialStream object,
                  // therefore the data in our buffer is a pointer
                  // to the object's ISequentialStream interface.
                  pISeqStream = *(ISequentialStream**)pvValue;
                  
				          
                  hr = pISeqStream->Read(
                           &m_pBind[m_iCount].szValue,                     // pBuffer
                           MAX_DISPLAY_SIZE,              // cBytes
                           &cbRead                        // pcBytesRead
                           );
                  
                  m_pBind[m_iCount].szValue[cbRead / sizeof(WCHAR)] = L'\0';
				  m_pBind[m_iCount].iSize = ulLength;

                  // Release the stream object, now that we're done.
                  pISeqStream->Release();
                  pISeqStream = NULL;
                  break;
               }
            }
            break;
         }

         // This is an error status, so don't try to display the data.
         default:
             wcscpy(m_pBind[m_iCount].szValue, L"(error status)");
			 break;
      }

	  m_iCount++;
   }

   
   
   if( pISeqStream )
      pISeqStream->Release();

   // Print the row separator.
   return hr;
}


//////////////////////////////////////////////////////////////////
// 디비 프로퍼티 얻기
/////////////////////////////////////////////////////////////////
/*
HRESULT CDBStoredProc::GetProperty(REFIID riid, DBPROPID 
                      dwPropertyID, REFGUID guidPropertySet, BOOL* 
                      pbValue)
{
   HRESULT                hr;
   DBPROPID               rgPropertyIDs[1];
   DBPROPIDSET            rgPropertyIDSets[1];
   
   ULONG                  cPropSets        = 0;
   DBPROPSET *            rgPropSets       = NULL;
   IDBProperties *        pIDBProperties   = NULL;
   ISessionProperties *   pISesProps       = NULL;
   ICommandProperties *   pICmdProps       = NULL;
   IRowsetInfo *          pIRowsetInfo     = NULL;


   // Initialize the output value
   *pbValue = FALSE;

   // Set up the property ID array
   rgPropertyIDs[0] = dwPropertyID;
   
   // Set up the Property ID Set
   rgPropertyIDSets[0].rgPropertyIDs     = rgPropertyIDs;
   rgPropertyIDSets[0].cPropertyIDs      = 1;
   rgPropertyIDSets[0].guidPropertySet   = guidPropertySet;

   if( riid == IID_IDBProperties )
   {
      CHECK_HR(hr = m_pIRowset->QueryInterface(IID_IDBProperties,
               (void**)&pIDBProperties));
      CHECK_HR(hr = pIDBProperties->GetProperties(
               1,                                 // cPropertyIDSets
               rgPropertyIDSets,                  // rgPropertyIDSets
               &cPropSets,                        // pcPropSets
               &rgPropSets                        // prgPropSets
               ));
   }
   else if( riid == IID_ISessionProperties )
   {
      CHECK_HR(hr = m_pIRowset->QueryInterface(IID_ISessionProperties,
               (void**)&pISesProps));
      
	  CHECK_HR(hr = pISesProps->GetProperties(
               1,                                 // cPropertyIDSets
               rgPropertyIDSets,                  // rgPropertyIDSets
               &cPropSets,                        // pcPropSets
               &rgPropSets                        // prgPropSets
               ));
   }
   else if( riid == IID_ICommandProperties )
   {
      CHECK_HR(hr = m_pIRowset->QueryInterface(IID_ICommandProperties,
               (void**)&pICmdProps));
      CHECK_HR(hr = pICmdProps->GetProperties(
               1,                                 // cPropertyIDSets
               rgPropertyIDSets,                  // rgPropertyIDSets
               &cPropSets,                        // pcPropSets
               &rgPropSets                        // prgPropSets
               ));
   }
   else
   {
      CHECK_HR(hr = m_pIRowset->QueryInterface(IID_IRowsetInfo,
               (void**)&pIRowsetInfo));
      CHECK_HR(hr = pIRowsetInfo->GetProperties(
               1,                                 // cPropertyIDSets
               rgPropertyIDSets,                  // rgPropertyIDSets
               &cPropSets,                        // pcPropSets
               &rgPropSets                        // prgPropSets
               ));
   }

   if( V_VT(&rgPropSets[0].rgProperties[0].vValue) == VT_BOOL )
      *pbValue = V_BOOL(&rgPropSets[0].rgProperties[0].vValue);


   ULONG iPropSet;
   ULONG iProp;

   Assert((rgPropSets != NULL) || (cPropSets == 0));

   if( rgPropSets )
   {
      for(iPropSet = 0; iPropSet < cPropSets; iPropSet++)
      {
         Assert((rgPropSets[iPropSet].cProperties == 0) ||
               (rgPropSets[iPropSet].rgProperties != NULL));
         if(rgPropSets[iPropSet].rgProperties)
         {
            for(iProp = 0;iProp < rgPropSets[iPropSet].cProperties; iProp++)
            {
                VariantClear(&(rgPropSets[iPropSet].rgProperties[iProp].vValue));
            }
			CoTaskMemFree(rgPropSets[iPropSet].rgProperties);
         }
      }
		CoTaskMemFree(rgPropSets);

   }


CLEANUP:
   SAFE_RELEASE(pIDBProperties )
   SAFE_RELEASE(pISesProps )
   SAFE_RELEASE(pICmdProps )
   SAFE_RELEASE(pIRowsetInfo )
   return hr;
}
*/

/////////////////////////////////
// 메모리 확인
////////////////////////////////
HRESULT CDBStoredProc::SetupBindings(ULONG* pcBindings, DBBINDING** prgBindings, ULONG* pcbRowSize)
{
   HRESULT          hr;
   ULONG            cColumns;
   DBCOLUMNINFO *   rgColumnInfo    = NULL;
   LPWSTR           pStringBuffer   = NULL;
   IColumnsInfo *   pIColumnsInfo   = NULL;

   ULONG            iCol;
   ULONG            dwOffset        = 0;
   DBBINDING *      rgBindings      = NULL;
   
   ULONG            cStorageObjs    = 0;
   BOOL             fMultipleObjs   = FALSE;

   // Obtain the column information for the rowset; from this, we can
   // find out the following information that we need to construct the
   // bindings array:
   // - the number of columns
   // - the ordinal of each column
   // - the precision and scale of numeric columns
   // - the OLE DB data type of the column
   // See IColumnsInfo
   hr = m_pIRowset->QueryInterface(
            IID_IColumnsInfo, (void**)&pIColumnsInfo);
   hr = pIColumnsInfo->GetColumnInfo(
            &cColumns,                            // pcColumns
            &rgColumnInfo,                        // prgColumnInfo
            &pStringBuffer                        // ppStringBuffer
            );

   // Allocate memory for the bindings array; there is a one-to-one
   // mapping between the columns returned from GetColumnInfo and our
   // bindings.
   rgBindings = (DBBINDING*)CoTaskMemAlloc(cColumns * sizeof(DBBINDING));
   CHECK_MEMORY(hr, rgBindings);
   memset(rgBindings, 0, cColumns * sizeof(DBBINDING));

//   GetProperty(IID_IRowset, DBPROP_MULTIPLESTORAGEOBJECTS,
//      DBPROPSET_ROWSET, &fMultipleObjs);

   // Construct the binding array element for each column.
   for( iCol = 0; iCol < cColumns; iCol++ )
   {
      // This binding applies to the ordinal of this column.
      rgBindings[iCol].iOrdinal = rgColumnInfo[iCol].iOrdinal;
      rgBindings[iCol].dwPart = DBPART_VALUE|DBPART_LENGTH|DBPART_STATUS;
      rgBindings[iCol].obStatus   = dwOffset;
      rgBindings[iCol].obLength   = dwOffset + sizeof(DBSTATUS);
      rgBindings[iCol].obValue    = dwOffset + sizeof(DBSTATUS) + sizeof(ULONG);
      rgBindings[iCol].dwMemOwner   = DBMEMOWNER_CLIENTOWNED;
      rgBindings[iCol].eParamIO   = DBPARAMIO_NOTPARAM;
      rgBindings[iCol].bPrecision   = rgColumnInfo[iCol].bPrecision;
      rgBindings[iCol].bScale       = rgColumnInfo[iCol].bScale;
      rgBindings[iCol].wType = DBTYPE_WSTR;
      rgBindings[iCol].cbMaxLen = 0;
      switch( rgColumnInfo[iCol].wType )
      {
         case DBTYPE_NULL:
         case DBTYPE_EMPTY:
         case DBTYPE_I1:
         case DBTYPE_I2:
         case DBTYPE_I4:
         case DBTYPE_UI1:
         case DBTYPE_UI2:
         case DBTYPE_UI4:
         case DBTYPE_R4:
         case DBTYPE_BOOL:
         case DBTYPE_I8:
         case DBTYPE_UI8:
         case DBTYPE_R8:
         case DBTYPE_CY:
         case DBTYPE_ERROR:
            // When the above types are converted to a string, they
            // will all fit into 25 characters, so use that plus space
            // for the NULL terminator.
            rgBindings[iCol].cbMaxLen = (25 + 1) * sizeof(WCHAR);
            break;

         case DBTYPE_DECIMAL:
         case DBTYPE_NUMERIC:
         case DBTYPE_DATE:
         case DBTYPE_DBDATE:
         case DBTYPE_DBTIMESTAMP:
         case DBTYPE_GUID:
            // Converted to a string, the above types will all fit into
            // 50 characters, so use that plus space for the terminator.
            rgBindings[iCol].cbMaxLen = (50 + 1) * sizeof(WCHAR);
            break;
         
         case DBTYPE_BYTES:
            rgBindings[iCol].cbMaxLen =
               (rgColumnInfo[iCol].ulColumnSize * 2 + 1) * sizeof(WCHAR);
            break;

         case DBTYPE_STR:
         case DBTYPE_WSTR:
         case DBTYPE_BSTR:
            rgBindings[iCol].cbMaxLen =
               (rgColumnInfo[iCol].ulColumnSize + 1) * sizeof(WCHAR);
            break;

         default:
            rgBindings[iCol].cbMaxLen = MAX_COL_SIZE;
            break;
      };
      
      if( (rgColumnInfo[iCol].wType == DBTYPE_IUNKNOWN ||
          ((rgColumnInfo[iCol].dwFlags & DBCOLUMNFLAGS_ISLONG) &&
          (USE_PROMPTDATASOURCE | DISPLAY_METHODCALLS & USE_ISEQSTREAM))) &&
          (fMultipleObjs || !cStorageObjs) )
      {
         rgBindings[iCol].wType = DBTYPE_IUNKNOWN;

         rgBindings[iCol].cbMaxLen = sizeof(ISequentialStream *);

         rgBindings[iCol].pObject =
                        (DBOBJECT *)CoTaskMemAlloc(sizeof(DBOBJECT));
         CHECK_MEMORY(hr, rgBindings[iCol].pObject);

         rgBindings[iCol].pObject->iid = IID_ISequentialStream;

         rgBindings[iCol].pObject->dwFlags = STGM_READ;

         cStorageObjs++;
      }   

      rgBindings[iCol].cbMaxLen   
         = min(rgBindings[iCol].cbMaxLen, MAX_COL_SIZE);
      dwOffset = rgBindings[iCol].cbMaxLen + rgBindings[iCol].obValue;
      dwOffset = ROUNDUP(dwOffset);
   }

   *pcbRowSize    = dwOffset;
   *pcBindings    = cColumns;
   *prgBindings   = rgBindings;


CLEANUP:
   CoTaskMemFree(rgColumnInfo);
   rgColumnInfo = NULL;
   CoTaskMemFree(pStringBuffer);
   pStringBuffer = NULL;
   pIColumnsInfo->Release();
   return hr;
}


void CDBStoredProc::SetCmdString(CDataBase* pDataBase, WCHAR* cmd)
{
	m_pDataBase = pDataBase;
    wcscpy(m_wCmdString, cmd);

	m_pParamBindInfo[0].pwszDataSourceType = L"DBTYPE_I4";
	m_pParamBindInfo[0].pwszName = L"@RETURN_VALUE"; //return value from sp
    m_pParamBindInfo[0].ulParamSize = sizeof(long);
    m_pParamBindInfo[0].dwFlags = DBPARAMFLAGS_ISOUTPUT;
    m_pParamBindInfo[0].bPrecision = 11;
    m_pParamBindInfo[0].bScale = 0;
    m_pParamOrdinals[0] = 1;


	m_pACDBBinding[0].obLength = 0;
    m_pACDBBinding[0].obStatus = 0;
    m_pACDBBinding[0].pTypeInfo = NULL;
    m_pACDBBinding[0].pObject = NULL;
    m_pACDBBinding[0].pBindExt = NULL;
    m_pACDBBinding[0].dwPart = DBPART_VALUE;
    m_pACDBBinding[0].dwMemOwner = DBMEMOWNER_CLIENTOWNED;
    m_pACDBBinding[0].dwFlags = 0;
    m_pACDBBinding[0].bScale = 0;

    m_pACDBBinding[0].iOrdinal = 1;
	m_pACDBBinding[0].obValue = 0;
    m_pACDBBinding[0].eParamIO = DBPARAMIO_OUTPUT;
    m_pACDBBinding[0].cbMaxLen = sizeof(long);
    m_pACDBBinding[0].wType = DBTYPE_I4;
    m_pACDBBinding[0].bPrecision = 11;

	m_n = 1;
	m_ParamOffSet = 4;

}


/*
void CDBStoredProc::Create(CDataBase* pDataBase)
{

	m_nParam = iParamNum;


	ZeroMemory(m_pPROCPARAMS, 5000);

	m_pParamBindInfo = new DBPARAMBINDINFO[m_nParam];  
	m_pParamOrdinals = new ULONG[m_nParam];
	m_pACDBBinding = new DBBINDING[m_nParam];
	m_pACDBBindStatus = new DBBINDSTATUS[m_nParam];


}

*/
void CDBStoredProc::SetParam(WCHAR* pName, DBTYPEENUM type, DBPARAMFLAGSENUM ParamFlagseNum, char* value)
{
	int iSize;

	switch(type)
	{
	case DBTYPE_I2:
		m_pParamBindInfo[m_n].pwszDataSourceType = L"DBTYPE_I2";
		iSize = sizeof(INT16);
		break;
	case DBTYPE_I4:
		m_pParamBindInfo[m_n].pwszDataSourceType = L"DBTYPE_I4";
		iSize = sizeof(INT32);
		break;
	case DBTYPE_BOOL:
		m_pParamBindInfo[m_n].pwszDataSourceType = L"DBTYPE_BOOL";
		iSize = 1;
		break;
	case DBTYPE_UI1:
		m_pParamBindInfo[m_n].pwszDataSourceType = L"DBTYPE_UI1";
		iSize = 1;
		break;
	case DBTYPE_I1:
		m_pParamBindInfo[m_n].pwszDataSourceType = L"DBTYPE_I1";
		iSize = 1;
		break;
	case DBTYPE_UI2:
		m_pParamBindInfo[m_n].pwszDataSourceType = L"DBTYPE_UI2";
		iSize = sizeof(INT16);
		break;
	case DBTYPE_UI4:
		m_pParamBindInfo[m_n].pwszDataSourceType = L"DBTYPE_UI4";
		iSize = sizeof(INT32);
		break;
	case DBTYPE_I8:
		m_pParamBindInfo[m_n].pwszDataSourceType = L"DBTYPE_I8";
		iSize = sizeof(INT64);
		break;
	case DBTYPE_UI8:
		m_pParamBindInfo[m_n].pwszDataSourceType = L"DBTYPE_UI8";
		iSize = sizeof(INT64);
		break;
	case DBTYPE_STR:
		m_pParamBindInfo[m_n].pwszDataSourceType = L"DBTYPE_STR";
		iSize = strlen(value);
		break;
	}
	
	memcpy(&m_SetParam.ParamValue[m_ParamOffSet - 4], value, iSize);

    m_pParamBindInfo[m_n].pwszName = pName;
    m_pParamBindInfo[m_n].ulParamSize = iSize;
    m_pParamBindInfo[m_n].dwFlags = ParamFlagseNum;
    m_pParamBindInfo[m_n].bPrecision = 11;
    m_pParamBindInfo[m_n].bScale = 0;
    m_pParamOrdinals[m_n] = m_n + 1;

	m_pACDBBinding[m_n].obLength = 0;
    m_pACDBBinding[m_n].obStatus = 0;
    m_pACDBBinding[m_n].pTypeInfo = NULL;
    m_pACDBBinding[m_n].pObject = NULL;
    m_pACDBBinding[m_n].pBindExt = NULL;
    m_pACDBBinding[m_n].dwPart = DBPART_VALUE;
    m_pACDBBinding[m_n].dwMemOwner = DBMEMOWNER_CLIENTOWNED;
    m_pACDBBinding[m_n].dwFlags = 0;
    m_pACDBBinding[m_n].bScale = 0;

	m_pACDBBinding[m_n].iOrdinal = m_n + 1;
	m_pACDBBinding[m_n].obValue = m_ParamOffSet;
    m_pACDBBinding[m_n].eParamIO = ParamFlagseNum;
    m_pACDBBinding[m_n].cbMaxLen = iSize;
    m_pACDBBinding[m_n].wType = type;
    m_pACDBBinding[m_n].bPrecision = 11;

	m_n++;
	m_ParamOffSet += iSize;
	m_nParam = m_n;
}


char* CDBStoredProc::GetParam(LPCWSTR pwszName)
{

	DBBYTEOFFSET	OffSet;
	ULONG n, iSize;
	static char Temp[100];
	ZeroMemory(Temp, 100);

	for(n = 0; n < m_nParam; n++)
	{
        if(wcscmp(m_pParamBindInfo[n].pwszName, pwszName) == 0)
		{
			OffSet = m_pACDBBinding[n].obValue;
			iSize = m_pACDBBinding[n].cbMaxLen;;

			memcpy(Temp, &m_SetParam.ParamValue[OffSet], iSize);

			return Temp;
		}
	}

	return NULL;

}
long CDBStoredProc::GetReturn()
{
	return m_SetParam.iReturn;
}


HRESULT CDBStoredProc::FindColumn(LPCWSTR pwszName, LONG *plIndex)
{
   HRESULT          hr;
   IColumnsInfo *   pIColumnsInfo    = NULL;
   ULONG            cColumns;
   DBCOLUMNINFO *   rgColumnInfo     = NULL;
   OLECHAR *        pStringsBuffer   = NULL;
   ULONG            iCol;

   // Get the IColumnsInfo interface.
   // See IColumnsInfo
   CHECK_HR(hr = m_pIRowset->QueryInterface(
            IID_IColumnsInfo, (void**)&pIColumnsInfo));

   // Get the columns information.
   CHECK_HR(hr = pIColumnsInfo->GetColumnInfo(
            &cColumns,                            // pcColumns
            &rgColumnInfo,                        // prgColumnInfo
            &pStringsBuffer                       // ppStringBuffer
            ));

   // Assume that we'll find the column.
   hr = S_OK;

   // Search for the column we need.
   for( iCol = 0; iCol < cColumns; iCol++ )
   {
      // If the column name matches, we've found the column....
      if( rgColumnInfo[iCol].pwszName &&
         !wcscmp(pwszName, rgColumnInfo[iCol].pwszName) )
      {
         *plIndex = iCol;
          goto CLEANUP;
      }
   }

   // If we didn't find the column, we'll return S_FALSE.
   hr = S_FALSE;

CLEANUP:

   CoTaskMemFree(rgColumnInfo);
   CoTaskMemFree(pStringsBuffer);
   pIColumnsInfo->Release();

   return hr;
}


void CDBStoredProc::FreeBindings(ULONG cBindings, DBBINDING* rgBindings)
{
   ULONG         iBind;

   if(rgBindings == NULL)
	   return;

   // Free any memory used by DBOBJECT structures in the array.
   for( iBind = 0; iBind < cBindings; iBind++ )
   {
	   if(rgBindings[iBind].pObject != NULL)
           CoTaskMemFree(rgBindings[iBind].pObject);
   }

   // Now free the bindings array itself.
   if(rgBindings != NULL)
       CoTaskMemFree(rgBindings);
}


////////////////////////////////////////////////////////////////////////////////////
// 레코드 값을 읽어 온다.
///////////////////////////////////////////////////////////////////////////////////
HRESULT CDBStoredProc::GetRowset()
{
   HRESULT       hr;
   ULONG         cBindings;
   HACCESSOR     hAccessor              = DB_NULL_HACCESSOR;
   ULONG         cbRowSize;
   DBBINDING *   rgBindings             = NULL;
   void*         pData                  = NULL;
   ULONG*        rgDispSize             = NULL;
   HROW*         rghRows                = NULL;
   void*         pCurData				= NULL;
   DBCOUNTITEM   cRowsObtained;
   ULONG         iRow;
   LONG          iRetCol                = -1;
//   BOOL          fCanFetchBackwards;
   ULONG         iIndex;
   IAccessor *   pIAccessor			= NULL; 

 /*  CHECK_HR(hr = GetProperty(IID_IRowset, DBPROP_CANFETCHBACKWARDS,
      DBPROPSET_ROWSET, &fCanFetchBackwards));
*/
 
   CHECK_HR(hr = SetupBindings(&cBindings, &rgBindings,
      &cbRowSize));
  
   CHECK_HR(hr = m_pIRowset->QueryInterface(
            IID_IAccessor, (void**)&pIAccessor));
  
   
   CHECK_HR(hr = pIAccessor->CreateAccessor(
            DBACCESSOR_ROWDATA,                  // dwAccessorFlags
            cBindings,                         // cBindings
            rgBindings,                        // rgBindings
            0,                                 // cbRowSize
            &hAccessor,                        // phAccessor
            NULL                               // rgStatus
            ));

   m_iColum = cBindings;


   
   pData = CoTaskMemAlloc(cbRowSize * m_cRows);
   CHECK_MEMORY(hr, pData);

   m_iCount = 0;
   // Clear the maximum display size array.
   // Attempt to get cRows row handles from the provider.
   CHECK_HR(hr = m_pIRowset->GetNextRows(
             DB_NULL_HCHAPTER,                      // hChapter
             0,                                     // lOffset
             m_cRows,                               // cRows
             &cRowsObtained,                        // pcRowsObtained
             &rghRows                               // prghRows
             ));

      // 현재 얻어온 row까지 루핑
   for( iRow = 0; iRow < cRowsObtained; iRow++ )
   {
       iIndex     = m_cRows > 0 ? iRow : cRowsObtained - iRow - 1;
       pCurData   = (BYTE*)pData + (cbRowSize * iIndex);
        
       hr = m_pIRowset->GetData(
              rghRows[iRow],                          // hRow
              hAccessor,                              // hAccessor
              pCurData                                // pData
              );
   }

   if( cRowsObtained )
   {
         // Release the row handles that we obtained.
       CHECK_HR(hr = m_pIRowset->ReleaseRows(
                cRowsObtained,                        // cRows
                rghRows,                              // rghRows
                NULL,                                 // rgRowOptions
                NULL,                                 // rgRefCounts
                NULL                                  // rgRowStatus
                ));
   }

   m_pBind = new CDBStoredProc::BIND [cBindings * iRow];
   ZeroMemory(m_pBind, cBindings * iRow);

   // For each row that we obtained the data for, display this data.
   for( iRow = 0; iRow < cRowsObtained; iRow++ )
   {
      // Get a pointer to the data for this row.
      pCurData = (BYTE*)pData + (cbRowSize* iRow);
		 
      // And display the row data.
      hr = GetRow(iRow, cBindings, rgBindings, pCurData);
   }

CLEANUP:
    CoTaskMemFree(rghRows);
    rghRows = NULL;
    if(pIAccessor != NULL)
		pIAccessor->ReleaseAccessor(hAccessor, NULL);
	pIAccessor->Release();
	CoTaskMemFree(pData);
	FreeBindings(cBindings, rgBindings);

	m_iMaxRows = cRowsObtained;
	return hr;

}


void CDBStoredProc::Release(void)
{
	if(m_pIRowset != NULL)
	{
        m_pIRowset->Release();
		delete [] m_pBind;
		m_pBind = NULL;
	}
}



//////////////////////////////


/////
////////////////////////////////////////////////////////////////////
// 스토어 프로시저 명령어 실행
/////////////////////////////////////////////////////////////////////
HRESULT CDBRecordSet::Execute(ULONG iRow)
{
	HRESULT				hr;
	ICommandText*		pICommandText        = NULL;
	LONG                cNumRows;			// 수정된 수


	//Create a Command object.
	CHECK_HR(hr = m_pDataBase->CreateCommandText(&pICommandText));

    
	//Set the command text.
	CHECK_HR(hr = pICommandText->SetCommandText(DBGUID_DBSQL, m_wCmdString));

    //Execute the command.
    CHECK_HR(hr = pICommandText->Execute(
                                    NULL, 
                                    IID_IRowset, 
                                    NULL, 
                                    &cNumRows, 
                                    (IUnknown **) &m_pIRowset));

	if(m_pIRowset == NULL)
		goto CLEANUP;

	if(iRow > 0 )
	{
		m_iCount = 0;
		GetRowset(iRow);
		m_iRow = iRow;

	}

CLEANUP:
	pICommandText->Release();
	return hr;
}

////////////////////////////////////////////////////////////////////////////////////
// 레코드 값을 읽어 온다.
///////////////////////////////////////////////////////////////////////////////////
HRESULT CDBRecordSet::GetRowset(ULONG iRowParam)
{
   HRESULT       hr;
   ULONG         cBindings;
   HACCESSOR     hAccessor              = DB_NULL_HACCESSOR;
   ULONG         cbRowSize;
   DBBINDING *   rgBindings             = NULL;
   void*         pData                  = NULL;
   ULONG*        rgDispSize             = NULL;
   HROW*         rghRows                = NULL;
   void*         pCurData				= NULL;
   DBCOUNTITEM   cRowsObtained;
   ULONG         iRow;
   LONG          iRetCol                = -1;
   BOOL          fCanFetchBackwards;
   ULONG         iIndex;
   IAccessor *   pIAccessor			= NULL; 

/*   CHECK_HR(hr = GetProperty(IID_IRowset, DBPROP_CANFETCHBACKWARDS,
      DBPROPSET_ROWSET, &fCanFetchBackwards));
*/

   fCanFetchBackwards = FALSE;
 
   CHECK_HR(hr = SetupBindings(&cBindings, &rgBindings,
      &cbRowSize));
  
   CHECK_HR(hr = m_pIRowset->QueryInterface(
            IID_IAccessor, (void**)&pIAccessor));
  
   
   CHECK_HR(hr = pIAccessor->CreateAccessor(
            DBACCESSOR_ROWDATA,                  // dwAccessorFlags
            cBindings,                         // cBindings
            rgBindings,                        // rgBindings
            0,                                 // cbRowSize
            &hAccessor,                        // phAccessor
            NULL                               // rgStatus
            ));

   m_iColum = cBindings;


   
   pData = CoTaskMemAlloc(cbRowSize * iRowParam);
   CHECK_MEMORY(hr, pData);

   m_iCount = 0;
   // Clear the maximum display size array.
   // Attempt to get cRows row handles from the provider.
   CHECK_HR(hr = m_pIRowset->GetNextRows(
             DB_NULL_HCHAPTER,                      // hChapter
             0,                                     // lOffset
             iRowParam,                               // cRows
             &cRowsObtained,                        // pcRowsObtained
             &rghRows                               // prghRows
             ));

      // 현재 얻어온 row까지 루핑
   for( iRow = 0; iRow < cRowsObtained; iRow++ )
   {
       iIndex     = iRowParam > 0 ? iRow : cRowsObtained - iRow - 1;
       pCurData   = (BYTE*)pData + (cbRowSize * iIndex);
        
       hr = m_pIRowset->GetData(
              rghRows[iRow],                          // hRow
              hAccessor,                              // hAccessor
              pCurData                                // pData
              );
   }

   if( cRowsObtained )
   {
         // Release the row handles that we obtained.
       CHECK_HR(hr = m_pIRowset->ReleaseRows(
                cRowsObtained,                        // cRows
                rghRows,                              // rghRows
                NULL,                                 // rgRowOptions
                NULL,                                 // rgRefCounts
                NULL                                  // rgRowStatus
                ));
   }

   if(!m_pBind)
   {
	   delete [] m_pBind;
   }

	m_pBind = new CDBRecordSet::BIND [cBindings * iRow ];

   // For each row that we obtained the data for, display this data.
   for( iRow = 0; iRow < cRowsObtained; iRow++ )
   {
      // Get a pointer to the data for this row.
      pCurData = (BYTE*)pData + (cbRowSize* iRow);
		 
      // And display the row data.
      hr = GetRow(iRow, cBindings, rgBindings, pCurData);
   }

CLEANUP:
    CoTaskMemFree(rghRows);
    rghRows = NULL;
    if(pIAccessor != NULL)
		pIAccessor->ReleaseAccessor(hAccessor, NULL);
	
	pIAccessor->Release();
	CoTaskMemFree(pData);
	FreeBindings(cBindings, rgBindings);

	m_iMaxRows = cRowsObtained;
	return hr;

}

/////////////////////////////////
// 메모리 확인
////////////////////////////////

void CDBRecordSet::Release(void)
{
	if(m_pIRowset != NULL)
	{
        m_pIRowset->Release();
		m_pIRowset = NULL;

	}

	if(m_pBind != NULL)
	{
        delete [] m_pBind;
		m_pBind = NULL;
	}

}

HRESULT CDBRecordSet::SetupBindings(ULONG* pcBindings, DBBINDING** prgBindings, ULONG* pcbRowSize)
{
   HRESULT          hr;
   ULONG            cColumns;
   DBCOLUMNINFO *   rgColumnInfo    = NULL;
   LPWSTR           pStringBuffer   = NULL;
   IColumnsInfo *   pIColumnsInfo   = NULL;

   ULONG            iCol;
   ULONG            dwOffset        = 0;
   DBBINDING *      rgBindings      = NULL;
   
   ULONG            cStorageObjs    = 0;
   BOOL             fMultipleObjs   = FALSE;


   hr = m_pIRowset->QueryInterface(
            IID_IColumnsInfo, (void**)&pIColumnsInfo);
   hr = pIColumnsInfo->GetColumnInfo(
            &cColumns,                            // pcColumns
            &rgColumnInfo,                        // prgColumnInfo
            &pStringBuffer                        // ppStringBuffer
            );

   // Allocate memory for the bindings array; there is a one-to-one
   // mapping between the columns returned from GetColumnInfo and our
   // bindings.
   rgBindings = (DBBINDING*)CoTaskMemAlloc(cColumns * sizeof(DBBINDING));
   CHECK_MEMORY(hr, rgBindings);
   memset(rgBindings, 0, cColumns * sizeof(DBBINDING));

//   GetProperty(IID_IRowset, DBPROP_MULTIPLESTORAGEOBJECTS,
//      DBPROPSET_ROWSET, &fMultipleObjs);

   // Construct the binding array element for each column.
   for( iCol = 0; iCol < cColumns; iCol++ )
   {
      // This binding applies to the ordinal of this column.
      rgBindings[iCol].iOrdinal = rgColumnInfo[iCol].iOrdinal;
      rgBindings[iCol].dwPart = DBPART_VALUE|DBPART_LENGTH|DBPART_STATUS;
      rgBindings[iCol].obStatus   = dwOffset;
      rgBindings[iCol].obLength   = dwOffset + sizeof(DBSTATUS);
      rgBindings[iCol].obValue    = dwOffset + sizeof(DBSTATUS) + sizeof(ULONG);
      rgBindings[iCol].dwMemOwner   = DBMEMOWNER_CLIENTOWNED;
      rgBindings[iCol].eParamIO   = DBPARAMIO_NOTPARAM;
      rgBindings[iCol].bPrecision   = rgColumnInfo[iCol].bPrecision;
      rgBindings[iCol].bScale       = rgColumnInfo[iCol].bScale;
      rgBindings[iCol].wType = DBTYPE_WSTR;
      rgBindings[iCol].cbMaxLen = 0;
      switch( rgColumnInfo[iCol].wType )
      {
         case DBTYPE_NULL:
         case DBTYPE_EMPTY:
         case DBTYPE_I1:
         case DBTYPE_I2:
         case DBTYPE_I4:
         case DBTYPE_UI1:
         case DBTYPE_UI2:
         case DBTYPE_UI4:
         case DBTYPE_R4:
         case DBTYPE_BOOL:
         case DBTYPE_I8:
         case DBTYPE_UI8:
         case DBTYPE_R8:
         case DBTYPE_CY:
         case DBTYPE_ERROR:
            // When the above types are converted to a string, they
            // will all fit into 25 characters, so use that plus space
            // for the NULL terminator.
            rgBindings[iCol].cbMaxLen = (25 + 1) * sizeof(WCHAR);
            break;

         case DBTYPE_DECIMAL:
         case DBTYPE_NUMERIC:
         case DBTYPE_DATE:
         case DBTYPE_DBDATE:
         case DBTYPE_DBTIMESTAMP:
         case DBTYPE_GUID:
            // Converted to a string, the above types will all fit into
            // 50 characters, so use that plus space for the terminator.
            rgBindings[iCol].cbMaxLen = (50 + 1) * sizeof(WCHAR);
            break;
         
         case DBTYPE_BYTES:
            rgBindings[iCol].cbMaxLen =
               (rgColumnInfo[iCol].ulColumnSize * 2 + 1) * sizeof(WCHAR);
            break;

         case DBTYPE_STR:
         case DBTYPE_WSTR:
         case DBTYPE_BSTR:
            rgBindings[iCol].cbMaxLen =
               (rgColumnInfo[iCol].ulColumnSize + 1) * sizeof(WCHAR);
            break;

         default:
            rgBindings[iCol].cbMaxLen = MAX_COL_SIZE;
            break;
      };
      
      if( (rgColumnInfo[iCol].wType == DBTYPE_IUNKNOWN ||
          ((rgColumnInfo[iCol].dwFlags & DBCOLUMNFLAGS_ISLONG) &&
          (USE_PROMPTDATASOURCE | DISPLAY_METHODCALLS & USE_ISEQSTREAM))) &&
          (fMultipleObjs || !cStorageObjs) )
      {
         rgBindings[iCol].wType = DBTYPE_IUNKNOWN;

         rgBindings[iCol].cbMaxLen = sizeof(ISequentialStream *);

         rgBindings[iCol].pObject =
                        (DBOBJECT *)CoTaskMemAlloc(sizeof(DBOBJECT));
         CHECK_MEMORY(hr, rgBindings[iCol].pObject);

         rgBindings[iCol].pObject->iid = IID_ISequentialStream;

         rgBindings[iCol].pObject->dwFlags = STGM_READ;

         cStorageObjs++;
      }   

      rgBindings[iCol].cbMaxLen   
         = min(rgBindings[iCol].cbMaxLen, MAX_COL_SIZE);
      dwOffset = rgBindings[iCol].cbMaxLen + rgBindings[iCol].obValue;
      dwOffset = ROUNDUP(dwOffset);
   }

   *pcbRowSize    = dwOffset;
   *pcBindings    = cColumns;
   *prgBindings   = rgBindings;


CLEANUP:
   CoTaskMemFree(rgColumnInfo);
   rgColumnInfo = NULL;
   CoTaskMemFree(pStringBuffer);
   pStringBuffer = NULL;
   pIColumnsInfo->Release();
   return hr;
}


void CDBRecordSet::FreeBindings(ULONG cBindings, DBBINDING* rgBindings)
{
   ULONG         iBind;

   if(rgBindings == NULL)
	   return;

   // Free any memory used by DBOBJECT structures in the array.
   for( iBind = 0; iBind < cBindings; iBind++ )
   {
	   if(rgBindings[iBind].pObject != NULL)
           CoTaskMemFree(rgBindings[iBind].pObject);
   }

   // Now free the bindings array itself.
   if(rgBindings != NULL)
       CoTaskMemFree(rgBindings);
}



///////////////////////////////////////////////////////////////////////////////////////
// 레코드를 읽은 후 생성된 BIND 객체와 연결한다.
//////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBRecordSet::GetRow(ULONG iRow, ULONG cBindings, DBBINDING* rgBindings, void* pData)
{
   HRESULT               hr = S_OK;
   DBSTATUS              dwStatus;
   ULONG                 ulLength;
   void *                pvValue = NULL;
   ULONG                 iCol;
   ULONG                 cbRead;
   ISequentialStream *   pISeqStream = NULL;
   
   
   // For each column that we have bound, display the data.
   for( iCol = 0; iCol < cBindings; iCol++ )
   {
      // We have bound status, length, and the data value for all
      // columns, so we know that these can all be used.
      dwStatus   = *(DBSTATUS *)((BYTE *)pData + rgBindings[iCol].obStatus);
      ulLength   = *(ULONG *)((BYTE *)pData + rgBindings[iCol].obLength);
      pvValue    = (BYTE *)pData + rgBindings[iCol].obValue;

      // Check the status of this column. This decides
      // exactly what will be displayed for the column.
      switch( dwStatus )
      {
         // The data is NULL, so don't try to display it.
         case DBSTATUS_S_ISNULL:
            wcscpy(m_pBind[m_iCount].szValue, L"(null)");
            break;

         // The data was fetched, but may have been truncated.
         // Display string data for this column to the user.
         case DBSTATUS_S_TRUNCATED:
         case DBSTATUS_S_OK:
         case DBSTATUS_S_DEFAULT:
         {
            // We have bound the column either as a Unicode string
            // (DBTYPE_WSTR) or as an ISequentialStream object
            // (DBTYPE_IUNKNOWN), and we have to do different processing
            // for each one of these possibilities.
            switch( rgBindings[iCol].wType )
            {
               case DBTYPE_WSTR:
               {   
                  // Copy the string data.
				  wcsncpy(m_pBind[m_iCount].szValue, (WCHAR *)pvValue, ulLength);
                  m_pBind[m_iCount].szValue[ulLength] = L'\0';
                  break;
               }

               case DBTYPE_IUNKNOWN:
               {
                  // We've bound this as an ISequentialStream object,
                  // therefore the data in our buffer is a pointer
                  // to the object's ISequentialStream interface.
                  pISeqStream = *(ISequentialStream**)pvValue;
                  
				          
                  hr = pISeqStream->Read(
                           &m_pBind[m_iCount].szValue,                     // pBuffer
                           MAX_DISPLAY_SIZE,              // cBytes
                           &cbRead                        // pcBytesRead
                           );
                  
                  m_pBind[m_iCount].szValue[cbRead / sizeof(WCHAR)] = L'\0';

                  // Release the stream object, now that we're done.
                  pISeqStream->Release();
                  pISeqStream = NULL;
                  break;
               }
            }
            break;
         }

         // This is an error status, so don't try to display the data.
         default:
             wcscpy(m_pBind[m_iCount].szValue, L"(error status)");
			 break;
      }

	  m_iCount++;
   }

   
   
   if( pISeqStream )
      pISeqStream->Release();

   return hr;
}

int CDBRecordSet::MoveNext()
{
	if(S_OK != GetRowset(m_iRow))
		return RECORD_EOF;
	
	else 
		return m_iMaxRows;
}


bool CDBRecordSet::ObjRsTime(int row, int column, LPSYSTEMTIME lpTime)
{
	char TimeStmp[30];
	char Buff[10];
	int Start = 0;
	int iIndex = 0;

	ZeroMemory(lpTime, sizeof(SYSTEMTIME));

	if(ObjRs(row, column, TimeStmp))
	{
		// 년도

		for(int n = 0; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == '-')
			{
				Buff[iIndex] = NULL;
				lpTime->wYear = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		// 월
		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == '-')
			{
				Buff[iIndex] = NULL;
				lpTime->wMonth = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		// 일
		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == ' ')
			{
				Buff[iIndex] = NULL;
				lpTime->wDay = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == ':')
			{
				Buff[iIndex] = NULL;
				lpTime->wHour = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == ':')
			{
				Buff[iIndex] = NULL;
				lpTime->wMinute = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == '.')
			{
				Buff[iIndex] = NULL;
				lpTime->wSecond = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		iIndex = 0;

		for(int n = Start; n < strlen(TimeStmp); n++)
		{
			Start++;

			if(TimeStmp[n] == NULL)
			{
				Buff[iIndex] = NULL;
				lpTime->wMilliseconds = atoi(Buff);
				break;
			}
			else
			{
				Buff[iIndex] = TimeStmp[n];
			}

			iIndex++;
		}

		return true;

	}

	return false;


}

bool CDBRecordSet::ObjRs(int row, int column, char *pData)
{
	WCHAR*	pwstr;
	int n;

	pwstr = m_pBind[row * m_iColum + column].szValue;
	
    n = WideCharToMultiByte(CP_ACP, 0, pwstr,(int) wcslen(pwstr), pData, (int) wcslen(pwstr), NULL, NULL);
	pData[n] = NULL;


	return true;
}

int CDBRecordSet::ObjRsInt(int row, int column)
{
	WCHAR*	pwstr;
	char Temp[10];
    int n;

	ZeroMemory(Temp, 10);

	pwstr = m_pBind[row * m_iColum + column].szValue;
	
    n = WideCharToMultiByte(CP_ACP, 0, pwstr, 10, Temp, 10, NULL, NULL);
	
	return atoi(Temp);
}

float CDBRecordSet::ObjRsFloat(int row, int column)
{
	WCHAR*	pwstr;
	char Temp[10];
    int n;

	ZeroMemory(Temp, 10);

	pwstr = m_pBind[row * m_iColum + column].szValue;
	
    n = WideCharToMultiByte(CP_ACP, 0, pwstr, 10, Temp, 10, NULL, NULL);
	
	return atof(Temp);
}


INT64 CDBRecordSet::ObjRsInt64(int row, int column)
{
	WCHAR*	pwstr;
	char Temp[50];
    int n;

	ZeroMemory(Temp, 50);

	pwstr = m_pBind[row * m_iColum + column].szValue;
	
    n = WideCharToMultiByte(CP_ACP, 0, pwstr, 50, Temp, 50, NULL, NULL);
	
	return _atoi64(Temp);

}

BYTE CDBRecordSet::ObjRsByte(int row, int column)
{
	WCHAR*	pwstr;
	char Temp[10];
    int n;

	ZeroMemory(Temp, 10);

	pwstr = m_pBind[row * m_iColum + column].szValue;
	
    n = WideCharToMultiByte(CP_ACP, 0, pwstr, 10, Temp, 10, NULL, NULL);

	return Temp[0];
}

void CDBRecordSet::ObjRs(int row, int column, char *pData, int iSize)
{
	WCHAR*	pwstr;
	int n;

	pwstr = m_pBind[row * m_iColum + column].szValue;
	
    n = WideCharToMultiByte(CP_ACP, 0, pwstr, iSize, pData, iSize, NULL, NULL);
	pData[iSize] = NULL;
}
