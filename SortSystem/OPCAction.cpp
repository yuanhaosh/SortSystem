#include "StdAfx.h"
#include "OPCAction.h"


COPCAction::COPCAction(void)
{
	pValues = NULL;
	m_bOPCOnline = false;
	pdwServerHandles = NULL;
	m_bOPCOnline = false;
}


COPCAction::~COPCAction(void)
{
}

BOOL COPCAction::InitOPCServer()
{
    CLSID clsid;
	HRESULT hr = S_OK;
	CString strServer = "S7200.OPCServer";  //OPC.SimaticNET S7200.OPCServer

	if (FAILED(::CoInitialize(NULL))) 	/*初始化COM库*/
	{
		AfxMessageBox("Error during CoInitialize", MB_OK );
        return FALSE;
	}
	
	hr = CLSIDFromProgID( strServer.AllocSysString(), &clsid ); 	/*查找OPC服务*/
	if( FAILED(hr))
	{
        AfxMessageBox("Error during CLSIDFromProgID", MB_OK);
        return FALSE;
	}
	

	LPUNKNOWN pUnkn = NULL;   	/*创建OPC服务器对象*/
	hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER , IID_IOPCServer, (void**)&m_IOPCServer); //CLSCTX_LOCAL_SERVER CLSCTX_ALL
	if( FAILED(hr) || m_IOPCServer == NULL)
	{
        AfxMessageBox("Error during CoCreateInstance", MB_OK);
        return FALSE;
	}

	FLOAT PercentDeadband = 0.0; /*添加组到OPC服务器*/
	DWORD RevisedUpdateRate;
	hr = m_IOPCServer->AddGroup(
		L"group",                     //[in]  组名
        TRUE,                         //[in]  活动状态
        500,                          //[in]  向服务器发送请求的刷新率
        1,                            //[in]  客户端的操作句柄
        NULL,                         //[in]  与标准时间的校正值
        &PercentDeadband,             //[in]  要舍弃的数据	
        0,                            //[in]  服务器使用的语言
        &m_GrpSrvHandle,              //[out] 添加组以后服务器返回的组句柄
        &RevisedUpdateRate,           //[out] 服务器的数据刷新率  
        IID_IOPCItemMgt,              //[in]  添加组的接口类型
        (LPUNKNOWN*)&m_IOPCItemMgt);  //[out] 服务器返回的接口对象指针
	if( FAILED(hr) )
	{  
		LPWSTR pErrString;
        AfxMessageBox("Error during AddGroup", MB_OK);
		hr = m_IOPCServer->GetErrorString(hr, LOCALE_SYSTEM_DEFAULT, &pErrString);
		if(SUCCEEDED(hr))
		{
			//输出错误信息
		}
		else
		{
			//添加组失败;
		}
		m_IOPCServer->Release();
		m_IOPCServer=NULL;
		CoUninitialize();
        return FALSE;
	}
	
	Item* pcItem = new Item[COUNT + 1];
	OPCITEMDEF *m_Items = new OPCITEMDEF[COUNT + 1];                        //项的存取路径, 定义和被请求的数据类等
	CString csItemName[]= {"MB0", "MB1", "VB0", "VB1", "VB2", "VB3", "VB4", "VB5", "VB6", "VB7"};  
	for(int i = 0; i < COUNT; i++)
	{
		
		pcItem[i].quality = QUAL_BAD;
	//	pcItem->name = _T("" + TableStr[i]);                             //设置opc节点 Microwin.NewPLC.group.
		pcItem[i].name = _T("Microwin.NewPLC.group.") + csItemName[i];     //这里的字符串填写你的item就行
		m_Items[i].szItemID = pcItem[i].name.AllocSysString();
		m_Items[i].dwBlobSize = 0;
		m_Items[i].pBlob = NULL;
		m_Items[i].bActive = TRUE;
		m_Items[i].hClient = (OPCHANDLE)(pcItem + i);
		m_Items[i].szAccessPath = pcItem[i].cAccessPath.AllocSysString();; //pcItem->cAccessPath.AllocSysString();
		m_Items[i].vtRequestedDataType = VT_EMPTY;                       //VT_EMPTY;
	}
    OPCITEMRESULT *pOPCResults = NULL;   //这2个参数为双指针 是否需要开辟内存 ？？
	HRESULT *pOPCErrors = NULL;
    hr = m_IOPCItemMgt->AddItems(COUNT, m_Items, &pOPCResults, &pOPCErrors);
	if(FAILED(hr))
	{
		LPWSTR pErrString;
        AfxMessageBox("Error during AddItems", MB_OK);
		hr = m_IOPCServer->GetErrorString(hr, LOCALE_SYSTEM_DEFAULT, &pErrString);
		if(SUCCEEDED(hr))
		{
			//输出错误信息
		}
		else
		{
			//pErrString = "添加组失败.";
		}
		m_IOPCServer->Release();
		m_IOPCServer=NULL;
		CoUninitialize();
        return FALSE;
	}
	
	/*保存item对应的服务句柄*/
	pdwServerHandles = new DWORD[COUNT];
	for(int i = 0; i < COUNT; i++)
	{
		pdwServerHandles[i] = pOPCResults[i].hServer; //直接获取句柄 pOPCErrors是否需要初始化？？
        if(pOPCErrors[i]= S_OK)
		{
			pdwServerHandles[i] = pOPCResults[i].hServer;  
		}
	}

	/*获取同步IO口*/
	hr = m_IOPCItemMgt->QueryInterface(IID_IOPCSyncIO, (void**)&m_IOPCSyncIO);
    if(FAILED(hr))
	{
		LPWSTR pErrString;
        AfxMessageBox("获取同步IO口失败", MB_OK);
		hr = m_IOPCServer->GetErrorString(hr, LOCALE_SYSTEM_DEFAULT, &pErrString);
		if(SUCCEEDED(hr))
		{
			//输出错误信息
		}
		else
		{
			//pErrString = "获取IO口失败.";
		}
		m_IOPCServer->Release();
		m_IOPCServer=NULL;
		CoUninitialize();
        return FALSE;
	}
	
	delete[] pdwServerHandles; 
	delete[] m_Items;
	delete[] pcItem;
	m_bOPCOnline  = true;
	return TRUE; 	//初始化完成，可以开始查询.
}

/**************************************************************************************************
*  读取OPC服务器中的状态信息
*  phServe:  读取项的句柄 
*  dwSource: 状态获取来源，缓存、内存 
****************************************************************************************************/
BOOL COPCAction::ReadOPCServe(OPCITEMSTATE **pValues, OPCHANDLE *phServe, OPCDATASOURCE dwSource)
{
	if(IsOPCOnline())
	{
		return FALSE;
	}

	HRESULT hr = S_OK;
    OPCHANDLE *phserve = phServe;

	LPWSTR pErrString; //记录错误信息字符串
	HRESULT *pErrors;// = new HRESULT;


	hr = m_IOPCSyncIO->Read(
		dwSource,               // OPC_DS_CACHE, Source (device or cache)
		1,      	            // Item count
		(OPCHANDLE*)phServe,	// Array of server handles for items
		pValues,	            // Array of values
		&pErrors);	            // Array of errors
    if(SUCCEEDED(hr))
	{	    
        AfxMessageBox("Error during ReadOPC", MB_OK);
		hr = m_IOPCServer->GetErrorString(hr, LOCALE_SYSTEM_DEFAULT, &pErrString);
		if(SUCCEEDED(hr))
		{
			//pErrString输出错误信息
            AfxMessageBox("读取状态失败.");
		}
		else
		{
				
			AfxMessageBox("");
		}
	/*	m_IOPCServer->Release();
		m_IOPCServer=NULL;
		CoUninitialize();*/
        return FALSE;
	}

	return  TRUE;	
}


/*****************************************************************************************
* 写入OPC服务器中的状态信息
* 后续需要再写 OPCITEMSTATE **pValues, OPCHANDLE *phServe, OPCDATASOURCE dwSource
******************************************************************************************/
BOOL COPCAction::WriteOPCServe(OPCHANDLE *phServe,  BYTE Values) 
{
	if(IsOPCOnline())
	{
		return FALSE;
	}

	HRESULT hr = S_OK;
	VARIANT *pItemValues = new VARIANT[1];
	OPCHANDLE *phserve = phServe;
	HRESULT *pErrors;//= new HRESULT;
	LPWSTR pErrString;

    VariantClear(pItemValues);
	memset(pItemValues, 0, sizeof(VARIANT));
    pItemValues->vt = VT_I2;
	BYTE value = Values;
    pItemValues->bVal = value;

	
    hr = m_IOPCSyncIO->Write(1,
                            phserve,
                            pItemValues,
                            &pErrors);
	if(SUCCEEDED(hr))
	{	    
		hr = m_IOPCServer->GetErrorString(hr, LOCALE_SYSTEM_DEFAULT, &pErrString);
		if(SUCCEEDED(hr))
		{
			//pErrString输出错误信息
		}
		else
		{
			//pErrString = "写入失败.";
		}
		/*m_IOPCServer->Release();
		m_IOPCServer=NULL;
		CoUninitialize();*/
        return FALSE;
	}
	delete[] pItemValues;
	
	return TRUE;
}


/***************************************************************************************
 * 检查状态位是否正常
 * pValues: 用于传递从OPC Server获得的状态信息
 * type:    用于区别位和字节状态信息 1:位 0:字节
 * bit：    用于指示位状态信息的操作位 eg：M1的M1.x;
            8:代表字节状态, 0-7代表位状态
******************************************************************************************/
BOOL COPCAction::ChkState(OPCITEMSTATE* pValues, int type, int bit)
{
    BYTE state = 0; 
	if(pValues->wQuality == QUAL_BAD)
	{
		//记录log 数据质量差
		return FALSE;
	}
	else if(VT_EMPTY == V_VT(&pValues->vDataValue))
	{
		//没有获取到有效地数据
		return FALSE;
	}
	else
	{}

    switch(bit)
	{
        case 0:  state = pValues->vDataValue.boolVal & 1;   break;
        case 1:  state = pValues->vDataValue.boolVal & 2;   break;
		case 2:  state = pValues->vDataValue.boolVal & 4;   break;
		case 3:  state = pValues->vDataValue.boolVal & 8;   break;
		case 4:  state = pValues->vDataValue.boolVal & 16;  break;
		case 5:  state = pValues->vDataValue.boolVal & 32;  break;
		case 6:  state = pValues->vDataValue.boolVal & 64;  break;
		case 7:  state = pValues->vDataValue.boolVal & 128; break;	
        default: state = pValues->vDataValue.bVal;          break;	
    }	
    
    if(type == 1) //BIT
	{
	    
	    return  (state == 1) ? TRUE : FALSE;
	}
	else if(type = 2) //BYTE
	{
	    return (state == 0xFF) ? TRUE : FALSE;

	}
	else
	{
	    //请检查输入类型是否正确
	    return FALSE;
	}
}


/*****************************************************************************************
* 释放资源
******************************************************************************************/
void COPCAction::ReleaseSource()
{
	if(pValues != NULL)
	{
		delete pValues;
		pValues = NULL;
	}

	//	释放同步接口
	if(m_IOPCSyncIO != NULL)
	{
		m_IOPCSyncIO->Release();
		m_IOPCSyncIO = NULL;
	}
	//	释放item管理接口
	if(m_IOPCItemMgt != NULL)
	{
		m_IOPCItemMgt->Release();
		m_IOPCItemMgt = NULL;
	}
	
	//	释放 OPC服务器
	if(m_IOPCServer != NULL)
	{
		m_IOPCServer->Release();
		m_IOPCServer = NULL;
	}
			
	//关闭COM库
	::CoUninitialize();
	m_bOPCOnline  = false;
}


/***************************************************************************************
检查M0.0，若为1 则返回1   PLC报警返回1
          若为0 则返回0  正常返回0
		  异常返回-1     程序异常返回-1
*****************************************************************************************/
int COPCAction::CheckPLCStateM0(BYTE bit = 0)
{
	BOOL result;
	if(bit > 8 ) return -1;

	if(pValues == NULL)
	{
		pValues = new OPCITEMSTATE(); //保存读取的状态值
		memset(pValues, 0, sizeof(pValues));
	}
	else
	{
		memset(pValues, 0, sizeof(pValues));
	}

	//OPCDATASOURCE dwSource	= bDeviceRead ? OPC_DS_DEVICE : OPC_DS_CACHE;
	result = ReadOPCServe(&pValues, &pdwServerHandles[0], OPC_DS_DEVICE /*OPC_DS_CACHE*/); //pValues初始化与操作后地址不一致 OPC_DS_DEVICE : OPC_DS_CACHE
	if(result == FALSE)
	{
		return -1;
	}

	BYTE mask = 1 << (bit);
	if(pValues->wQuality == QUAL_BAD || VT_EMPTY == V_VT(&pValues->vDataValue))
	{
		return -1;
	}
	else if( ((pValues->vDataValue.bVal) & mask)  == 1)
	{
		return 1;
	}
	return 0;
}

int COPCAction::ShowPLCStateM1()
{
	BOOL result;
	
	if(pValues == NULL)
	{
		pValues = new OPCITEMSTATE[1]; //保存读取的状态值
		memset(pValues, 0, sizeof(pValues));
	}
	else
	{
		memset(pValues, 0, sizeof(pValues));
	}
	//OPCDATASOURCE dwSource	= bDeviceRead ? OPC_DS_DEVICE : OPC_DS_CACHE;
	result = ReadOPCServe(&pValues, &pdwServerHandles[1], OPC_DS_CACHE); //pValues初始化与操作后地址不一致 OPC_DS_DEVICE : OPC_DS_CACHE
	if(result == FALSE)
	{
		return -1;
	}

	for(int i = 0; i < sizeof(BYTE); i++)
	{
		BYTE mask = 1 << i;
		if((pValues->vDataValue.bVal) & mask == 1)
		{
			//PLC 警告窗提示
		}

	}
}

/***************************************************************************************
运行前自检确保有工件，正常返回0
					  异常返回-1
*****************************************************************************************/
int COPCAction::ValidityTest()
{
	if(CheckPLCStateM0(4) == 1)
	{
		return 0;
		
	}
	else
	{
		if(CheckPLCStateM0(2) == 1)
		{
			return 2;
		}
		else
		{
			return 3;
		}
	}
	return 0;
}

bool COPCAction::IsOPCOnline()
{
	return m_bOPCOnline;
}