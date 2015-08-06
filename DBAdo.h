#pragma once

#include "math.h"

#include<windows.h>
#include <afxdb.h>// AfxMessageBox������Ҫ
//#import "C:\Program Files\Common Files\System\ado\msado15.dll"\no_namespace rename("EOF","adoEOF")
#import "C:\Program Files\common files\system\ado\msado15.dll" no_namespace rename("EOF","EndOfFile") rename("BOF","FirstOfFile")

//using namespace ADOWE;

class CDBAdo
{
public:
	_CommandPtr		m_ptrCommand;		//�������
	_RecordsetPtr	m_ptrRecordset;		//��¼������
	_ConnectionPtr	m_ptrConnection;	//���ݿ����
	
	CString			m_strConnect,		//�����ַ���
					m_strErrorMsg;		//������Ϣ

public:
	CDBAdo(void);
	~CDBAdo(void);

	void	DetectResult(HRESULT hResult);
	void	RecordErrorMsg(_com_error comError);
	CString	GetLastError(){return m_strErrorMsg;}

	bool	CreateInstance();
	bool	SetConnectionString(CString strDriver, CString strIP, WORD wPort, CString strCatalog, CString strUserID, CString strPassword);
	bool	SetConnectionString(CString strDriver, CString strDataSrc, CString strPassword);
	bool    SetConnectionString();
	bool	OpenConnection();
	bool	CloseConnection();
	bool	IsConnecting();

	void	ClearAllParameters();
	void	AddParamter(LPCTSTR lpcsrName, /*ADOWE::*/ParameterDirectionEnum Direction, /*ADOWE::*/DataTypeEnum Type, long lSize, _variant_t & vtValue);
	void	SetSPName(LPCTSTR lpcsrSPName);
	bool	ExecuteCommand(bool bIsRecordset);
	bool	Execute(LPCTSTR lpcsrCommand);
	long	GetReturnValue();

	bool	OpenRecordset(char* szSQL);
	bool	CloseRecordset();
	bool	IsRecordsetOpened();
	bool	IsEndRecordset();
	void	MoveToNext();
	void	MoveToFirst();
	void	MoveToLast();
	long	GetRecordCount();

	bool	GetFieldValue(LPCTSTR lpcsrFieldName, WORD& wValue);
	bool	GetFieldValue(LPCTSTR lpcsrFieldName, CString& strValue);
	bool	GetFieldValue(LPCTSTR lpcsrFieldName, INT& nValue);
	bool	GetFieldValue(LPCTSTR lpcsrFieldName, BYTE& bValue);
	bool	GetFieldValue(LPCTSTR lpcsrFieldName, LONG& lValue);
	bool	GetFieldValue(LPCTSTR lpcsrFieldName, DWORD& dwValue);
	bool	GetFieldValue(LPCTSTR lpcsrFieldName, UINT& ulValue);
	bool	GetFieldValue(LPCTSTR lpcsrFieldName, DOUBLE& dbValue);
	bool	GetFieldValue(LPCTSTR lpcsrFieldName, __int64& llValue);
	bool	GetFieldValue(LPCTSTR lpcsrFieldName, COleDateTime& Time);
	bool	GetFieldValue(LPCTSTR lpcsrFieldName, bool& bValue);
};