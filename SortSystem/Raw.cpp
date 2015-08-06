// CRaw.cpp: ����RAWͼ�����Ч���Ƚ�ͼ������ת���ɿ��Ա������bmp ������
//           �����������bmp����ͷ
//
//   
//      
//        
// Ԭ�� 2015.05.29
//////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "Raw.h"

#include <windowsx.h>
#include <afxadv.h>
#include <io.h>
#include <errno.h>

CRaw::CRaw(void)
{
	 m_PtrRawBits = NULL;   //���ո߾���16λͼ������
	 m_pBmpBits = NULL;
	 m_iValidDataBit = 12;  //Ĭ����Ч����Ϊ12
}


CRaw::CRaw( WORD* PtrRawBits,  unsigned char* pBmpBits, int iValidDataBit, int iWidth, int iHeight)
{
	if(PtrRawBits == NULL || pBmpBits == NULL)
	{
		return ;
	}
	
	m_pBmpBits = pBmpBits;
	m_iValidDataBit = iValidDataBit;
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	m_iMaxPixel = 0;
	m_iMinPixel = 0;


	m_PtrRawBits = (WORD* )GlobalAllocPtr(GHND, m_iWidth * m_iHeight *sizeof(WORD));
	if(m_PtrRawBits == NULL)
	{
		return ;
	}
	else 
	{
		memcpy(m_PtrRawBits, PtrRawBits, m_iWidth * m_iHeight *sizeof(WORD));
	}
}

CRaw::~CRaw(void)
{
	if(m_PtrRawBits != NULL)
	{
	    GlobalFree(m_PtrRawBits); //GlobalFreePtr
		m_PtrRawBits = NULL;
	}

}

BOOL CRaw::InvalidBitsAndFindMValue()
{
	if(m_PtrRawBits == NULL || m_pBmpBits == NULL)
	{
		return TRUE;
	}
	int iMask ;
	switch(m_iValidDataBit)
	{
		case 12: iMask = ValidBits12; break;
		case 14: iMask = ValidBits14; break;
		case 16: iMask = ValidBits16; break;
		default:;
	}
	if(iMask == ValidBits16)
	{
		return TRUE; //16λ��Ч���ݲ�����ģ����
	}

	m_iMaxPixel = 0;
	m_iMinPixel =  iMask;
	for(int i = 0; i < m_iWidth * m_iHeight; i++)
	{
	    
		*(m_PtrRawBits + i) = *(m_PtrRawBits + i) & iMask; //m_PtrRawBits[i]


		if(m_iMaxPixel < *(m_PtrRawBits + i))
		{
			m_iMaxPixel = *(m_PtrRawBits + i);
		}

		if(m_iMinPixel > *(m_PtrRawBits + i))
		{
			m_iMinPixel = *(m_PtrRawBits + i);
		}
		
	}
	return TRUE;
}

BOOL CRaw::StatisticsPiexlsInfo()
{
	return true;
}

//�߾���ͼ��ֱ��ת�� 
BOOL CRaw::DirectConversionToEightBit()
{
	InvalidBitsAndFindMValue();  //���ε���Чλ

	//ʹ�ò����ֵӦ�û�ȼ��㸳ֵ��ܶ�
	LPBYTE Tab = new BYTE[m_iWidth * m_iHeight + 1]; //����ֱֵ�Ӷ�Ӧת���������ֵ������
	if(Tab == NULL) return FALSE;

	int level = 0;
	int piexl = 0;
	switch(m_iValidDataBit)
	{
		case 12: level = (ValidBits12 + 1) / 256; break;
		case 14: level = (ValidBits14 + 1) / 256; break;
		case 16: level = (ValidBits16 + 1) / 256; break;
		default: ;
	}

	for( int i = 1; i <= m_iWidth * m_iHeight; i++)
	{
		
		if(i % level == 0)
		{
			piexl += 1;
		}
		Tab[i] = piexl; //���ʱ�����ڴ�0��ʼ
	}

	for(int i = 0; i < m_iWidth * m_iHeight; i++ )
	{
		 *(m_pBmpBits + i) = Tab[*(m_PtrRawBits + i)];
	}
	delete Tab;
	Tab = NULL;

	return TRUE;
}

//�߾���ͼ��ת���������Сֵ����
BOOL CRaw::RangeConversionToEightBit()
{
	InvalidBitsAndFindMValue();     //1.�������ε���Чλ�ҳ������Сֵ

	if(m_iMaxPixel == 0 && m_iMinPixel == 0)
	{
		return FALSE;
	}

	//ʹ�ò����ֵӦ�û�ȼ��㸳ֵ��ܶ�
	LPBYTE Tab = new BYTE[m_iWidth * m_iHeight + 1]; //����ֱֵ�Ӷ�Ӧת���������ֵ������
	if(Tab == NULL) return FALSE;

	int iRangeLength = 0;
	int piexl = 0;
	iRangeLength = (m_iMaxPixel - m_iMinPixel)/256; //ÿ������Ĳ���
	memset(Tab, 0, m_iWidth * m_iHeight * sizeof(WORD));
	for( int i = 1; i <= m_iWidth * m_iHeight; i++)
	{
	    if(m_iMinPixel + iRangeLength> i )
		{
			Tab[i] = 0;
			continue;
		}
		else if(m_iMaxPixel - iRangeLength < i)
		{
			Tab[i] = 255;
			continue;
		}

		if( (i - m_iMinPixel) %  iRangeLength == 0 )
		{
			piexl += 1; 
		}
		Tab[i] = piexl; //������ ��һ��ʼ����������Ѿ��ڳ�ʼ��ʱ����ͷ��������0��255
	}

	for(int i = 0; i < m_iWidth * m_iHeight; i++ )
	{
		*(m_pBmpBits + i) = Tab[*(m_PtrRawBits + i)];
	}

	delete Tab;
	Tab = NULL;
	return TRUE;
}

//���߾���ͼ��ת����ָ������
BOOL CRaw::SpecialRangeConversionToEightBit(int start, int end)
{
	if(start == 0 && end == 0)
	{
		return FALSE;
	}

	InvalidBitsAndFindMValue(); //���ε���Чλ

	//ʹ�ò����ֵӦ�û�ȼ��㸳ֵ��ܶ�
	LPBYTE Tab = new BYTE[m_iWidth * m_iHeight + 1]; //����ֱֵ�Ӷ�Ӧת���������ֵ������
	if(Tab == NULL) return FALSE;

	int iRangeLength = 0;
	int piexl = 0;
	iRangeLength = (end - start)/256; //ÿ������Ĳ���
	memset(Tab, 0, m_iWidth * m_iHeight * sizeof(DWORD));
	for( int i = 1; i <= m_iWidth * m_iHeight; i++)
	{
	    if(m_iMinPixel + iRangeLength> i )
		{
			Tab[i] = 0;
			continue;
		}
		else if(m_iMaxPixel - iRangeLength < i)
		{
			Tab[i] = 255;
			continue;
		}

		if( (i - m_iMinPixel) %  iRangeLength == 0 )
		{
			piexl += 1; 
		}
		Tab[i] = piexl; //������ ��һ��ʼ����������Ѿ��ڳ�ʼ��ʱ����ͷ��������0��255
	}

	for(int i = 0; i < m_iWidth * m_iHeight; i++ )
	{
		*(m_pBmpBits + i) = Tab[*(m_PtrRawBits + i)];
	}

	delete Tab;
	Tab = NULL;

	return TRUE;
}