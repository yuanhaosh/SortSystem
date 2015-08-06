// MyDib.h: interface for the CMyDib class.
//
//   ˵����
//       1��CMyDib���Ƕ�CDib�����չ
//       2����չ��λͼ�ļ����뵼������
//        
//                              ������ 2005.12.12
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYDIB_H__37DACCF5_E5C1_4F74_9F14_D8E8FD6651EF__INCLUDED_)
#define AFX_MYDIB_H__37DACCF5_E5C1_4F74_9F14_D8E8FD6651EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "LLVProDemoDef.h"

#include "DIB.H"

class CMyDib : public CDib  
{
public:
	enum Format
	{
        FormatUnknown   = -1,
        FormatByte      = 6,
        FormatInt       = 8,
        FormatUByte     = 10,
        FormatUInt      = 12,
        FormatRgb       = 19
	};

public:
	CMyDib();
    CMyDib(int iWidth, int iHeight, Format format, LPBYTE pbyDataAddr = NULL, 
                    BOOL bIsRawData = FALSE); 

    CMyDib(const CMyDib& mydib);

    void Create(int iWidth, int iHeight, Format format, LPBYTE pbyDataAddr = NULL, 
                    BOOL bIsRawData = FALSE);

    ~CMyDib();

    //��ȡͼ�����ݵ�ַ
    LPBYTE  GetImgDataAddr()    const  { return m_pBits; }
    //��ȡͼ��λ��
    int  NumBitCount()          const  { return m_pBMI->bmiHeader.biBitCount; }  

public:

    // ��ͼ���ļ��ж�ȡ����
    BOOL ReadBMPFile(LPCTSTR lpszPathName);
    // �������ݵ�ͼ���ļ�
    BOOL SaveToFile(LPCTSTR lpszPathName);
    //���ػ��ƺ���
    BOOL Paint(HDC hDC, LPRECT lpDCRect, LPRECT lpDIBRect) const;
	BOOL Paint(CDC* pDC, LPRECT lpDCRect, LPRECT lpViewRect, LPRECT lpDIBRect) const;

    //��BMP��ʽ(���µ��ϣ�ÿ���ֽ���Ϊ4�ı���)ת����Sapera��ʽ(���ϵ��£�ÿ��ʵ�ʿ�ȶ�ӡ�ֽ���)������Ҫ���顣
    BOOL BmpToSaperaFmt();
    //��Sapera��ʽת����BMP��ʽ������Ҫ���顣
    BOOL SaperaToBmpFmt();

};

#endif // !defined(AFX_MYDIB_H__37DACCF5_E5C1_4F74_9F14_D8E8FD6651EF__INCLUDED_)
