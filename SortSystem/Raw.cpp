// CRaw.cpp: 根据RAW图像的有效精度将图像数据转化成可以被处理的bmp 数据体
//           并不对其添加bmp数据头
//
//   
//      
//        
// 袁浩 2015.05.29
//////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "Raw.h"

#include <windowsx.h>
#include <afxadv.h>
#include <io.h>
#include <errno.h>

CRaw::CRaw(void)
{
	 m_PtrRawBits = NULL;   //接收高精度16位图像数据
	 m_pBmpBits = NULL;
	 m_iValidDataBit = 12;  //默认有效精度为12
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
		return TRUE; //16位有效数据不与掩模处理
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

//高精度图像直接转化 
BOOL CRaw::DirectConversionToEightBit()
{
	InvalidBitsAndFindMValue();  //屏蔽掉无效位

	//使用查表法赋值应该会比计算赋值快很多
	LPBYTE Tab = new BYTE[m_iWidth * m_iHeight + 1]; //数组值直接对应转化后的象素值，加速
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
		Tab[i] = piexl; //检查时，表内从0开始
	}

	for(int i = 0; i < m_iWidth * m_iHeight; i++ )
	{
		 *(m_pBmpBits + i) = Tab[*(m_PtrRawBits + i)];
	}
	delete Tab;
	Tab = NULL;

	return TRUE;
}

//高精度图像转化到最大最小值区域
BOOL CRaw::RangeConversionToEightBit()
{
	InvalidBitsAndFindMValue();     //1.首先屏蔽掉无效位找出最大最小值

	if(m_iMaxPixel == 0 && m_iMinPixel == 0)
	{
		return FALSE;
	}

	//使用查表法赋值应该会比计算赋值快很多
	LPBYTE Tab = new BYTE[m_iWidth * m_iHeight + 1]; //数组值直接对应转化后的象素值，加速
	if(Tab == NULL) return FALSE;

	int iRangeLength = 0;
	int piexl = 0;
	iRangeLength = (m_iMaxPixel - m_iMinPixel)/256; //每个区间的步长
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
		Tab[i] = piexl; //驱动表 从一开始设置区间段已经在初始化时将两头的区间设0和255
	}

	for(int i = 0; i < m_iWidth * m_iHeight; i++ )
	{
		*(m_pBmpBits + i) = Tab[*(m_PtrRawBits + i)];
	}

	delete Tab;
	Tab = NULL;
	return TRUE;
}

//将高精度图像转换到指定区域
BOOL CRaw::SpecialRangeConversionToEightBit(int start, int end)
{
	if(start == 0 && end == 0)
	{
		return FALSE;
	}

	InvalidBitsAndFindMValue(); //屏蔽掉无效位

	//使用查表法赋值应该会比计算赋值快很多
	LPBYTE Tab = new BYTE[m_iWidth * m_iHeight + 1]; //数组值直接对应转化后的象素值，加速
	if(Tab == NULL) return FALSE;

	int iRangeLength = 0;
	int piexl = 0;
	iRangeLength = (end - start)/256; //每个区间的步长
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
		Tab[i] = piexl; //驱动表 从一开始设置区间段已经在初始化时将两头的区间设0和255
	}

	for(int i = 0; i < m_iWidth * m_iHeight; i++ )
	{
		*(m_pBmpBits + i) = Tab[*(m_PtrRawBits + i)];
	}

	delete Tab;
	Tab = NULL;

	return TRUE;
}