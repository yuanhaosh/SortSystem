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

	if (FAILED(::CoInitialize(NULL))) 	/*��ʼ��COM��*/
	{
		AfxMessageBox("Error during CoInitialize", MB_OK );
        return FALSE;
	}
	
	hr = CLSIDFromProgID( strServer.AllocSysString(), &clsid ); 	/*����OPC����*/
	if( FAILED(hr))
	{
        AfxMessageBox("Error during CLSIDFromProgID", MB_OK);
        return FALSE;
	}
	

	LPUNKNOWN pUnkn = NULL;   	/*����OPC����������*/
	hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER , IID_IOPCServer, (void**)&m_IOPCServer); //CLSCTX_LOCAL_SERVER CLSCTX_ALL
	if( FAILED(hr) || m_IOPCServer == NULL)
	{
        AfxMessageBox("Error during CoCreateInstance", MB_OK);
        return FALSE;
	}

	FLOAT PercentDeadband = 0.0; /*����鵽OPC������*/
	DWORD RevisedUpdateRate;
	hr = m_IOPCServer->AddGroup(
		L"group",                     //[in]  ����
        TRUE,                         //[in]  �״̬
        500,                          //[in]  ����������������ˢ����
        1,                            //[in]  �ͻ��˵Ĳ������
        NULL,                         //[in]  ���׼ʱ���У��ֵ
        &PercentDeadband,             //[in]  Ҫ����������	
        0,                            //[in]  ������ʹ�õ�����
        &m_GrpSrvHandle,              //[out] ������Ժ���������ص�����
        &RevisedUpdateRate,           //[out] ������������ˢ����  
        IID_IOPCItemMgt,              //[in]  �����Ľӿ�����
        (LPUNKNOWN*)&m_IOPCItemMgt);  //[out] ���������صĽӿڶ���ָ��
	if( FAILED(hr) )
	{  
		LPWSTR pErrString;
        AfxMessageBox("Error during AddGroup", MB_OK);
		hr = m_IOPCServer->GetErrorString(hr, LOCALE_SYSTEM_DEFAULT, &pErrString);
		if(SUCCEEDED(hr))
		{
			//���������Ϣ
		}
		else
		{
			//�����ʧ��;
		}
		m_IOPCServer->Release();
		m_IOPCServer=NULL;
		CoUninitialize();
        return FALSE;
	}
	
	Item* pcItem = new Item[COUNT + 1];
	OPCITEMDEF *m_Items = new OPCITEMDEF[COUNT + 1];                        //��Ĵ�ȡ·��, ����ͱ�������������
	CString csItemName[]= {"MB0", "MB1", "VB0", "VB1", "VB2", "VB3", "VB4", "VB5", "VB6", "VB7"};  
	for(int i = 0; i < COUNT; i++)
	{
		
		pcItem[i].quality = QUAL_BAD;
	//	pcItem->name = _T("" + TableStr[i]);                             //����opc�ڵ� Microwin.NewPLC.group.
		pcItem[i].name = _T("Microwin.NewPLC.group.") + csItemName[i];     //������ַ�����д���item����
		m_Items[i].szItemID = pcItem[i].name.AllocSysString();
		m_Items[i].dwBlobSize = 0;
		m_Items[i].pBlob = NULL;
		m_Items[i].bActive = TRUE;
		m_Items[i].hClient = (OPCHANDLE)(pcItem + i);
		m_Items[i].szAccessPath = pcItem[i].cAccessPath.AllocSysString();; //pcItem->cAccessPath.AllocSysString();
		m_Items[i].vtRequestedDataType = VT_EMPTY;                       //VT_EMPTY;
	}
    OPCITEMRESULT *pOPCResults = NULL;   //��2������Ϊ˫ָ�� �Ƿ���Ҫ�����ڴ� ����
	HRESULT *pOPCErrors = NULL;
    hr = m_IOPCItemMgt->AddItems(COUNT, m_Items, &pOPCResults, &pOPCErrors);
	if(FAILED(hr))
	{
		LPWSTR pErrString;
        AfxMessageBox("Error during AddItems", MB_OK);
		hr = m_IOPCServer->GetErrorString(hr, LOCALE_SYSTEM_DEFAULT, &pErrString);
		if(SUCCEEDED(hr))
		{
			//���������Ϣ
		}
		else
		{
			//pErrString = "�����ʧ��.";
		}
		m_IOPCServer->Release();
		m_IOPCServer=NULL;
		CoUninitialize();
        return FALSE;
	}
	
	/*����item��Ӧ�ķ�����*/
	pdwServerHandles = new DWORD[COUNT];
	for(int i = 0; i < COUNT; i++)
	{
		pdwServerHandles[i] = pOPCResults[i].hServer; //ֱ�ӻ�ȡ��� pOPCErrors�Ƿ���Ҫ��ʼ������
        if(pOPCErrors[i]= S_OK)
		{
			pdwServerHandles[i] = pOPCResults[i].hServer;  
		}
	}

	/*��ȡͬ��IO��*/
	hr = m_IOPCItemMgt->QueryInterface(IID_IOPCSyncIO, (void**)&m_IOPCSyncIO);
    if(FAILED(hr))
	{
		LPWSTR pErrString;
        AfxMessageBox("��ȡͬ��IO��ʧ��", MB_OK);
		hr = m_IOPCServer->GetErrorString(hr, LOCALE_SYSTEM_DEFAULT, &pErrString);
		if(SUCCEEDED(hr))
		{
			//���������Ϣ
		}
		else
		{
			//pErrString = "��ȡIO��ʧ��.";
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
	return TRUE; 	//��ʼ����ɣ����Կ�ʼ��ѯ.
}

/**************************************************************************************************
*  ��ȡOPC�������е�״̬��Ϣ
*  phServe:  ��ȡ��ľ�� 
*  dwSource: ״̬��ȡ��Դ�����桢�ڴ� 
****************************************************************************************************/
BOOL COPCAction::ReadOPCServe(OPCITEMSTATE **pValues, OPCHANDLE *phServe, OPCDATASOURCE dwSource)
{
	if(IsOPCOnline())
	{
		return FALSE;
	}

	HRESULT hr = S_OK;
    OPCHANDLE *phserve = phServe;

	LPWSTR pErrString; //��¼������Ϣ�ַ���
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
			//pErrString���������Ϣ
            AfxMessageBox("��ȡ״̬ʧ��.");
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
* д��OPC�������е�״̬��Ϣ
* ������Ҫ��д OPCITEMSTATE **pValues, OPCHANDLE *phServe, OPCDATASOURCE dwSource
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
			//pErrString���������Ϣ
		}
		else
		{
			//pErrString = "д��ʧ��.";
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
 * ���״̬λ�Ƿ�����
 * pValues: ���ڴ��ݴ�OPC Server��õ�״̬��Ϣ
 * type:    ��������λ���ֽ�״̬��Ϣ 1:λ 0:�ֽ�
 * bit��    ����ָʾλ״̬��Ϣ�Ĳ���λ eg��M1��M1.x;
            8:�����ֽ�״̬, 0-7����λ״̬
******************************************************************************************/
BOOL COPCAction::ChkState(OPCITEMSTATE* pValues, int type, int bit)
{
    BYTE state = 0; 
	if(pValues->wQuality == QUAL_BAD)
	{
		//��¼log ����������
		return FALSE;
	}
	else if(VT_EMPTY == V_VT(&pValues->vDataValue))
	{
		//û�л�ȡ����Ч������
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
	    //�������������Ƿ���ȷ
	    return FALSE;
	}
}


/*****************************************************************************************
* �ͷ���Դ
******************************************************************************************/
void COPCAction::ReleaseSource()
{
	if(pValues != NULL)
	{
		delete pValues;
		pValues = NULL;
	}

	//	�ͷ�ͬ���ӿ�
	if(m_IOPCSyncIO != NULL)
	{
		m_IOPCSyncIO->Release();
		m_IOPCSyncIO = NULL;
	}
	//	�ͷ�item����ӿ�
	if(m_IOPCItemMgt != NULL)
	{
		m_IOPCItemMgt->Release();
		m_IOPCItemMgt = NULL;
	}
	
	//	�ͷ� OPC������
	if(m_IOPCServer != NULL)
	{
		m_IOPCServer->Release();
		m_IOPCServer = NULL;
	}
			
	//�ر�COM��
	::CoUninitialize();
	m_bOPCOnline  = false;
}


/***************************************************************************************
���M0.0����Ϊ1 �򷵻�1   PLC��������1
          ��Ϊ0 �򷵻�0  ��������0
		  �쳣����-1     �����쳣����-1
*****************************************************************************************/
int COPCAction::CheckPLCStateM0(BYTE bit = 0)
{
	BOOL result;
	if(bit > 8 ) return -1;

	if(pValues == NULL)
	{
		pValues = new OPCITEMSTATE(); //�����ȡ��״ֵ̬
		memset(pValues, 0, sizeof(pValues));
	}
	else
	{
		memset(pValues, 0, sizeof(pValues));
	}

	//OPCDATASOURCE dwSource	= bDeviceRead ? OPC_DS_DEVICE : OPC_DS_CACHE;
	result = ReadOPCServe(&pValues, &pdwServerHandles[0], OPC_DS_DEVICE /*OPC_DS_CACHE*/); //pValues��ʼ����������ַ��һ�� OPC_DS_DEVICE : OPC_DS_CACHE
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
		pValues = new OPCITEMSTATE[1]; //�����ȡ��״ֵ̬
		memset(pValues, 0, sizeof(pValues));
	}
	else
	{
		memset(pValues, 0, sizeof(pValues));
	}
	//OPCDATASOURCE dwSource	= bDeviceRead ? OPC_DS_DEVICE : OPC_DS_CACHE;
	result = ReadOPCServe(&pValues, &pdwServerHandles[1], OPC_DS_CACHE); //pValues��ʼ����������ַ��һ�� OPC_DS_DEVICE : OPC_DS_CACHE
	if(result == FALSE)
	{
		return -1;
	}

	for(int i = 0; i < sizeof(BYTE); i++)
	{
		BYTE mask = 1 << i;
		if((pValues->vDataValue.bVal) & mask == 1)
		{
			//PLC ���洰��ʾ
		}

	}
}

/***************************************************************************************
����ǰ�Լ�ȷ���й�������������0
					  �쳣����-1
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