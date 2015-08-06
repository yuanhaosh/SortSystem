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

#define COUNT 10 //opc 参数


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
    DWORD           *pdwServerHandles; //handle顺序参考TableStr定义顺序
	bool            m_bOPCOnline;

	OPCITEMSTATE *pValues; //

	BOOL InitOPCServer(); //初始化OPC服务器
	BOOL ReadOPCServe(OPCITEMSTATE **pValues, OPCHANDLE *phServe, OPCDATASOURCE dwSource); //读取OPC服务器中的状态值
	BOOL WriteOPCServe(OPCHANDLE *phServe,  BYTE Values) ;
	BOOL ChkState(OPCITEMSTATE *pValues, int type, int bit);
	void ReleaseSource(); //释放资源
	int CheckPLCStateM0(BYTE bit);
	int ShowPLCStateM1();
	int ValidityTest();
	bool IsOPCOnline();
};

