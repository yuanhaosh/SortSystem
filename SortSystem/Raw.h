// CRaw.h: 根据RAW图像的有效精度将图像数据转化成可以被处理的bmp 数据体
//           并不对其添加bmp数据头
//
//   
//      
//        
// 袁浩 2015.05.29
//////////////////////////////////////////////////////////////////////


#pragma once

// 有效数据掩模，与原始数据相与得到有效数据
typedef enum tagHCPVALIDBITSMASK
{
	ValidBits12	= 0x0FFF,				// 低12位有效
	ValidBits14 = 0x3FFF,
	ValidBits16 = 0xFFFF
	
} VALIDBITSMASK;


class CRaw
{
public:
	CRaw(void);
	CRaw(  WORD* PtrRawBits,  unsigned char* pBmpBits, int iValidDataBit, int iWidth, int iHeight);
	~CRaw(void);

	BOOL IplImageToBMP();
	BOOL GetImgDataAddr();
	BOOL InvalidBitsAndFindMValue();
	BOOL DirectConversionToEightBit();
	BOOL RangeConversionToEightBit();
	BOOL SpecialRangeConversionToEightBit(int start, int end);
	BOOL StatisticsPiexlsInfo();

private:

	WORD*  m_PtrRawBits;   //接收高精度16位图像数据
	LPBYTE m_pBmpBits;
	int    m_iValidDataBit; //高精度图像每个象素实际用n位表达
	int    m_iWidth;
	int    m_iHeight;

	WORD   m_iMinPixel;
	WORD   m_iMaxPixel;
};

