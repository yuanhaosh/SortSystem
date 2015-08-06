// CRaw.h: ����RAWͼ�����Ч���Ƚ�ͼ������ת���ɿ��Ա������bmp ������
//           �����������bmp����ͷ
//
//   
//      
//        
// Ԭ�� 2015.05.29
//////////////////////////////////////////////////////////////////////


#pragma once

// ��Ч������ģ����ԭʼ��������õ���Ч����
typedef enum tagHCPVALIDBITSMASK
{
	ValidBits12	= 0x0FFF,				// ��12λ��Ч
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

	WORD*  m_PtrRawBits;   //���ո߾���16λͼ������
	LPBYTE m_pBmpBits;
	int    m_iValidDataBit; //�߾���ͼ��ÿ������ʵ����nλ���
	int    m_iWidth;
	int    m_iHeight;

	WORD   m_iMinPixel;
	WORD   m_iMaxPixel;
};

