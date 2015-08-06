#pragma once

#include "opcda.h"
#include "opccomn.h"
#include "opcerror.h"

class Item : public CObject
{
public:
	Item(){
		name.Empty();
		quality=0;
		value.Clear();
		dwLastError=0;
		boActual=FALSE;
	}
	CString       name;
	CString       cAccessPath;
	COleVariant   value;
	WORD          quality;
	DWORD         dwLastError;
	BOOL          boActual;
};

#define COUNT 10 //opc ����


class COPCAction
{
public:
	COPCAction(void);
	~COPCAction(void);


	IOPCServer		*m_IOPCServer;
    OPCHANDLE		m_GrpSrvHandle;
	IOPCItemMgt		*m_IOPCItemMgt;
    //OPCITEMRESULT   *pOPCResults;
	IOPCSyncIO		*m_IOPCSyncIO;
    DWORD           *pdwServerHandles; //handle˳��ο�TableStr����˳��
	bool            m_bOPCOnline;

	OPCITEMSTATE *pValues; //

	BOOL InitOPCServer(); //��ʼ��OPC������
	BOOL ReadOPCServe(OPCITEMSTATE **pValues, OPCHANDLE *phServe, OPCDATASOURCE dwSource); //��ȡOPC�������е�״ֵ̬
	BOOL WriteOPCServe(OPCHANDLE *phServe,  BYTE Values) ;
	BOOL ChkState(OPCITEMSTATE *pValues, int type, int bit);
	void ReleaseSource(); //�ͷ���Դ
	int CheckPLCStateM0(BYTE bit);
	int ShowPLCStateM1();
	int ValidityTest();
	bool IsOPCOnline();
};

