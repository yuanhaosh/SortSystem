#include "stdafx.h"
#include "DBAdo.h"

CDBAdo::CDBAdo(void)
{
	m_strConnect	= _T("");
	m_strErrorMsg	= _T("");

	//初始化 COM
	CoInitialize(NULL);
}

CDBAdo::~CDBAdo(void)
{
	//关闭连接
	CloseConnection();

	//释放对象
	if(m_ptrCommand!=NULL)
		m_ptrCommand.Release();
	if(m_ptrRecordset!=NULL)
		m_ptrRecordset.Release();
	if(m_ptrConnection!=NULL)
		m_ptrConnection.Release();
}

bool CDBAdo::CreateInstance()
{
	//创建对象
	m_ptrCommand.CreateInstance(__uuidof(Command));
	m_ptrRecordset.CreateInstance(__uuidof(Recordset));
	m_ptrConnection.CreateInstance(__uuidof(Connection));

	if(m_ptrCommand==NULL)
	{
		m_strErrorMsg	= _T("数据库命令对象创建失败");
		return	false;
	}
	if(m_ptrRecordset==NULL)
	{
		m_strErrorMsg	= _T("数据库记录集对象创建失败");
		return	false;
	}
	if(m_ptrConnection==NULL)
	{
		m_strErrorMsg	= _T("数据库连接对象创建失败");
		return	false;
	}

	//设置变量
	m_ptrCommand->CommandType	= adCmdStoredProc;
	return	true;
}

void CDBAdo::DetectResult(HRESULT hResult)
{
	if(FAILED(hResult))
		_com_issue_error(hResult);
}

void CDBAdo::RecordErrorMsg(_com_error comError)
{
	_bstr_t	bstrDescribe(comError.Description());

	m_strErrorMsg.Format(TEXT("ADO 错误：0x%8x，%s"), comError.Error(), (LPCTSTR)bstrDescribe);
}

bool CDBAdo::SetConnectionString(CString strDriver, CString strIP, WORD wPort, CString strCatalog, CString strUserID, CString strPassword)
{
	CString	strProvider, strPWD, strUID, strData, strDataSrc;

	strProvider.Format(_T("Provider=%s;"), strProvider);
	strPWD.Format(_T("Password=%s;"), strPassword);
	strUID.Format(_T("User ID=%s;"), strUserID);
	strData.Format(_T("Initial Catalog=%s;"), strCatalog);
	strDataSrc.Format(_T("Data Source=%s,%ld;"), strIP, wPort);

	//构造连接字符串
	m_strConnect	= strProvider+strPWD+_T("Persist Security Info=True;")+strUID+strData+strDataSrc;

	return true;
}

bool CDBAdo::SetConnectionString(CString strDriver, CString strDataSrc, CString strPassword)
{
	CString	strProvider, strDataSource, strPWD;

	strProvider.Format(_T("Provider=%s;"), strDriver);
	strDataSource.Format(_T("Data Source=%s;"), strDataSrc);
	strPWD.Format(_T("Jet OLEDB:DataBase Password=%s;"), strPassword);

	//构造连接字符串
	m_strConnect	= strProvider+_T("User ID=Admin;")+strDataSource+strPWD;
	return true;
}
bool CDBAdo::SetConnectionString()
{
	m_strConnect = _T("Provider=SQLOLEDB.1;Password=xupt;Persist Security Info=True;User ID=sa;Initial Catalog=SortSystem;Data Source=YUANHAO-PC\\SQLEXPRESS");
	return true;
}


bool CDBAdo::OpenConnection()
{
	try
	{
		//关闭连接
		CloseConnection();

		//连接数据库
		DetectResult(m_ptrConnection->Open(_bstr_t(m_strConnect), "", "", adModeUnknown));
		m_ptrConnection->CursorLocation	= adUseClient;
		m_ptrCommand->ActiveConnection	= m_ptrConnection;

		return true;
	}
	catch(_com_error& comError) 
	{
		RecordErrorMsg(comError);
	}

	return false;
}

bool CDBAdo::CloseConnection()
{
	try
	{
		CloseRecordset();
		if((m_ptrConnection!=NULL)&&(m_ptrConnection->GetState()!=adStateClosed))
			DetectResult(m_ptrConnection->Close());

		return true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return false;
}

bool CDBAdo::IsConnecting()
{
	try 
	{
		//状态判断
		if(m_ptrConnection==NULL)
			return	false;
		if(m_ptrConnection->GetState()==adStateClosed)
			return	false;

		//参数判断
		long	lErrorCount	= m_ptrConnection->Errors->Count;
		if(lErrorCount>0L)
		{
	        ErrorPtr	pError	 = NULL;
			for(long i=0; i<lErrorCount; i++)
			{
				pError	= m_ptrConnection->Errors->GetItem(i);
				if(pError->Number==0x80004005)
					return	false;
			}
		}

		return true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return false;
}

bool CDBAdo::OpenRecordset(char* szSQL)
{
	try
	{
		//关闭记录集
		CloseRecordset();

		m_ptrRecordset->Open(szSQL, m_ptrConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);

		return true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}

bool CDBAdo::CloseRecordset()
{
	try
	{
		if(IsRecordsetOpened())
			DetectResult(m_ptrRecordset->Close());
		return true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return false;
}

bool CDBAdo::IsRecordsetOpened()
{
	if(m_ptrRecordset==NULL)
		return	false;
	if(m_ptrRecordset->GetState()==adStateClosed)
		return	false;

	return true;
}

bool CDBAdo::IsEndRecordset()
{
	try 
	{
		return (m_ptrRecordset->EndOfFile==VARIANT_TRUE);
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return true;
}

void CDBAdo::MoveToNext()
{
	try 
	{ 
		m_ptrRecordset->MoveNext(); 
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}
}

void CDBAdo::MoveToFirst()
{
	try 
	{ 
		m_ptrRecordset->MoveFirst(); 
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}
}

void CDBAdo::MoveToLast()
{
	try 
	{ 
		m_ptrRecordset->MoveLast(); 
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}
}

long CDBAdo::GetRecordCount()
{
	try
	{
		if(m_ptrRecordset==NULL)
			return	0;
		return	m_ptrRecordset->GetRecordCount();
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return 0;
}

void CDBAdo::ClearAllParameters()
{
	try 
	{
		long	lParamCount	= m_ptrCommand->Parameters->Count;
		if(lParamCount>0L)
		{
			for(long i=lParamCount; i>0; i--)
			{
				_variant_t	vtIndex;

				vtIndex.intVal	= i-1;
				m_ptrCommand->Parameters->Delete(vtIndex);
			}
		}
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}
}

void CDBAdo::AddParamter(LPCTSTR lpcsrName, /*ADOWE::*/ParameterDirectionEnum Direction, /*ADOWE::*/DataTypeEnum Type, long lSize, _variant_t & vtValue)
{
	ASSERT(lpcsrName!=NULL);
	try 
	{
        _ParameterPtr	Parameter	= m_ptrCommand->CreateParameter(lpcsrName, Type, Direction, lSize, vtValue);
		m_ptrCommand->Parameters->Append(Parameter);
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}
}

void CDBAdo::SetSPName(LPCTSTR lpcsrSPName)
{
	try 
	{ 
		m_ptrCommand->CommandText	= lpcsrSPName; 
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}
}

bool CDBAdo::ExecuteCommand(bool bIsRecordset)
{
	try 
	{
		//关闭记录集
		CloseRecordset();
		//执行命令
		if(bIsRecordset)
		{
			m_ptrRecordset->PutRefSource(m_ptrCommand);
			m_ptrRecordset->CursorLocation	= adUseClient;
			DetectResult(m_ptrRecordset->Open((IDispatch*)m_ptrCommand, vtMissing, adOpenForwardOnly, adLockReadOnly, adOptionUnspecified));
		}else 
		{
			m_ptrConnection->CursorLocation	= adUseClient;
			DetectResult(m_ptrCommand->Execute(NULL, NULL, adExecuteNoRecords));
		}
		return	true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}

bool CDBAdo::Execute(LPCTSTR lpcsrCommand)
{
	try
	{
		m_ptrConnection->CursorLocation	= adUseClient;
		m_ptrConnection->Execute(lpcsrCommand, NULL, adExecuteNoRecords);
		return	true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}

long CDBAdo::GetReturnValue()
{
	try 
	{
        _ParameterPtr	ptrParam;
		long			lParameterCount	= m_ptrCommand->Parameters->Count;
		for(long i=0; i<lParameterCount; i++)
		{
			ptrParam	= m_ptrCommand->Parameters->Item[i];
			if(ptrParam->Direction==adParamReturnValue)
				return	ptrParam->Value.lVal;
		}
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	0;
}

bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, WORD& wValue)
{
	wValue	= 0L;
	try
	{
		_variant_t	vtFld	= m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;
		if((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY))
			wValue	= (WORD)vtFld.ulVal;

		return true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return false;
}

bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, CString& strValue)
{
	try
	{
		_variant_t vtFld	= m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;
		if(vtFld.vt==VT_BSTR)
		{
			strValue	= (char*)_bstr_t(vtFld);
			strValue.TrimLeft();
			return	true;
		}
		return	false;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}

bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, INT& nValue)
{
	nValue	= 0;
	try
	{
		_variant_t vtFld = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_BOOL:
			{
				nValue	= vtFld.boolVal;
				break;
			}
		case VT_I2:
		case VT_UI1:
			{
				nValue	= vtFld.iVal;
				break;
			}
		case VT_NULL:
		case VT_EMPTY:
			{
				nValue	= 0;
				break;
			}
		default: nValue	= vtFld.iVal;
		}	
		return true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return false;
}

bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, BYTE& bValue)
{
	bValue	= 0;
	try
	{
		_variant_t vtFld	= m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_BOOL:
			{
				bValue	= (vtFld.boolVal!=0)?1:0;
				break;
			}
		case VT_I2:
		case VT_UI1:
			{
				bValue	= (vtFld.iVal>0)?1:0;
				break;
			}
		case VT_NULL:
		case VT_EMPTY:
			{
				bValue	= 0;
				break;
			}
		default: bValue	= (BYTE)vtFld.iVal;
		}	
		return true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, UINT& ulValue)
{
	ulValue	= 0L;
	try
	{
		_variant_t	vtFld	= m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;
		if((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY))
			ulValue	= vtFld.lVal;
		return	true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, DOUBLE& dbValue)
{
	dbValue=0.0L;
	try
	{
		_variant_t	vtFld	= m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_R4:dbValue	= vtFld.fltVal;break;
		case VT_R8:dbValue	= vtFld.dblVal;break;
		case VT_DECIMAL:
			{
				dbValue	 = vtFld.decVal.Lo32;
				dbValue	*= (vtFld.decVal.sign==128)?-1:1;
				dbValue	/= pow((float)10,vtFld.decVal.scale);
			}
			break;
		case VT_UI1:dbValue	= vtFld.iVal;break;
		case VT_I2:
		case VT_I4:dbValue	= vtFld.lVal;break;
		case VT_NULL:
		case VT_EMPTY:dbValue	= 0.0L;break;
		default:dbValue	= vtFld.dblVal;
		}
		return true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, LONG& lValue)
{
	lValue	= 0L;
	try
	{
		_variant_t	vtFld	= m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;
		if((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY))
			lValue	= vtFld.lVal;
		return	true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, DWORD& dwValue)
{
	dwValue	= 0L;
	try
	{
		_variant_t	vtFld	= m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;
		if((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY))
			dwValue	= vtFld.ulVal;
		return	true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, __int64& llValue)
{
	llValue	= 0L;
	try
	{
		_variant_t	vtFld	= m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;
		if((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY))
			llValue=vtFld.lVal;

		return	true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, COleDateTime& Time)
{
	try
	{
		_variant_t	vtFld	= m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_DATE:
			{
				COleDateTime	TempTime(vtFld);
				Time	= TempTime;
			}break;
		case VT_EMPTY:
		case VT_NULL:Time.SetStatus(COleDateTime::null);break;
		default: return false;
		}
		return	true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}

bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, bool& bValue)
{
	try
	{
		_variant_t	vtFld	= m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_BOOL:bValue=(vtFld.boolVal==0)?false:true;break;
		case VT_EMPTY:
		case VT_NULL:bValue = false;break;
		default:return false;
		}

		return	true;
	}
	catch(_com_error& comError)
	{
		RecordErrorMsg(comError);
	}

	return	false;
}