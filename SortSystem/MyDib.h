// MyDib.h: interface for the CMyDib class.
//
//   说明：
//       1、CMyDib类是对CDib类的扩展
//       2、扩展了位图文件导入导出函数
//        
//                              周明才 2005.12.12
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

    //获取图像数据地址
    LPBYTE  GetImgDataAddr()    const  { return m_pBits; }
    //获取图像位数
    int  NumBitCount()          const  { return m_pBMI->bmiHeader.biBitCount; }  

public:

    // 从图像文件中读取数据
    BOOL ReadBMPFile(LPCTSTR lpszPathName);
    // 保存数据到图像文件
    BOOL SaveToFile(LPCTSTR lpszPathName);
    //重载绘制函数
    BOOL Paint(HDC hDC, LPRECT lpDCRect, LPRECT lpDIBRect) const;
	BOOL Paint(CDC* pDC, LPRECT lpDCRect, LPRECT lpViewRect, LPRECT lpDIBRect) const;

    //从BMP格式(从下到上，每行字节数为4的倍数)转换到Sapera格式(从上到下，每行实际宽度对印字节数)，数据要重组。
    BOOL BmpToSaperaFmt();
    //从Sapera格式转换到BMP格式，数据要重组。
    BOOL SaperaToBmpFmt();

};

#endif // !defined(AFX_MYDIB_H__37DACCF5_E5C1_4F74_9F14_D8E8FD6651EF__INCLUDED_)
