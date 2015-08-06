// MyDib.cpp: implementation of the CMyDib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyDib.h"

#include <windowsx.h>
#include <afxadv.h>
#include <io.h>
#include <errno.h>



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyDib::CMyDib()
{

}

CMyDib::~CMyDib()
{

}

// 从图像文件中读取数据
// 2005.12.9
BOOL CMyDib::ReadBMPFile(LPCTSTR lpszPathName)
{
	DWORD dWord = 0;
    CFile file;
    //打开文件失败
    if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite))
    {
        ASSERT("打开文件错误!");
        return FALSE;
    }
    dWord = Read(file);
	
    file.Close();

	return dWord>0?TRUE:FALSE;
}

// 保存数据到图像文件
// 2005.12.9
BOOL CMyDib::SaveToFile(LPCTSTR lpszPathName)
{
	DWORD dWord = 0;
    CFile file;
    //打开文件
    if (!file.Open(lpszPathName, CFile::modeCreate |
        CFile::modeReadWrite | CFile::shareExclusive))
    {
        return FALSE;
    }

    TRY
    {
        //保存为DIB图像
        dWord = Save(file);
        
        file.Close();
    }
    //保存失败
    CATCH (CException, eSave)
    {
        file.Abort();
        return FALSE;
    }
    END_CATCH
	return dWord>0?TRUE:FALSE;
}

/*************************************************************************
 *
 * Paint()
 *
 * Parameters:
 *
 * HDC hDC          - DC to do output to
 *
 * LPRECT lpDCRect  - rectangle on DC to do output to
 *
 * LPRECT lpDIBRect - rectangle of DIB to output into lpDCRect
 *
 * CPalette* pPal   - pointer to CPalette containing DIB's palette
 *
 * Return Value:
 *
 * BOOL             - TRUE if DIB was drawn, FALSE otherwise
 *
 * Description:
 *   Painting routine for a DIB.  Calls StretchDIBits() or
 *   SetDIBitsToDevice() to paint the DIB.  The DIB is
 *   output to the specified DC, at the coordinates given
 *   in lpDCRect.  The area of the DIB to be output is
 *   given by lpDIBRect.
 *
 ************************************************************************/
//2005.12.15

BOOL CMyDib::Paint(HDC hDC, LPRECT lpDCRect, LPRECT lpDIBRect) const
{
    //想重载一个函数以避免闪烁。目前还没有。
    return CDib::Paint(hDC,lpDCRect, lpDIBRect);

}

BOOL CMyDib::Paint(CDC* pDC, LPRECT lpDCRect, LPRECT lpViewRect, LPRECT lpDIBRect) const
{
	//重载一个函数以避免闪烁
	if (!m_pBMI || pDC == NULL)
		return FALSE;

	HDC hDC = pDC->GetSafeHdc();
	HPALETTE hPal = NULL;           // Our DIB's palette
	HPALETTE hOldPal = NULL;        // Previous palette

	// Get the DIB's palette, then select it into DC
	if (m_pPalette != NULL)
	{
		hPal = (HPALETTE) m_pPalette->m_hObject;

		// Select as background since we have
		// already realized in foreground if needed
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}

	/* Make sure to use the stretching mode best for color pictures */
	::SetStretchBltMode(hDC, COLORONCOLOR);

	CDC* pMemDC = new CDC();
	CBitmap bm,*pOldBM;
	pMemDC->CreateCompatibleDC(pDC);
	bm.CreateCompatibleBitmap(pDC,RECTWIDTH(lpDCRect),RECTHEIGHT(lpDCRect));
	pOldBM = pMemDC->SelectObject(&bm);
	
	CBrush BlueBrush;
	BlueBrush.CreateSolidBrush(RGB(10,10,100));	
	CBrush* pOldBrush = (CBrush* )pMemDC->SelectObject(&BlueBrush);
	pMemDC->Rectangle(CRect(lpDCRect->left,lpDCRect->top,RECTWIDTH(lpDCRect),RECTHEIGHT(lpDCRect)));
	pMemDC->SelectObject(pOldBrush);

	pMemDC->SetStretchBltMode(HALFTONE);
	SetBrushOrgEx(pMemDC->GetSafeHdc(),0,0,NULL);


	BOOL bSuccess;
	if ((RECTWIDTH(lpViewRect)  == RECTWIDTH(lpDIBRect)) &&
	   (RECTHEIGHT(lpViewRect) == RECTHEIGHT(lpDIBRect)))
		bSuccess = ::SetDIBitsToDevice(pMemDC->GetSafeHdc(),   // hDC
								   lpViewRect->left,             // DestX
								   lpViewRect->top,              // DestY
								   RECTWIDTH(lpViewRect),        // nDestWidth
								   RECTHEIGHT(lpViewRect),       // nDestHeight
								   lpDIBRect->left,            // SrcX
								   (int)Height() -
									  lpDIBRect->top -
									  RECTHEIGHT(lpDIBRect),     // SrcY
								   0,                          // nStartScan
								   (WORD)Height(),             // nNumScans
								   m_pBits,                    // lpBits
								   m_pBMI,                     // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage
	else
		bSuccess = ::StretchDIBits(pMemDC->GetSafeHdc(),     // hDC
							   lpViewRect->left,             // DestX
							   lpViewRect->top,              // DestY
							   RECTWIDTH(lpViewRect),        // nDestWidth
							   RECTHEIGHT(lpViewRect),       // nDestHeight
							   lpDIBRect->left,              // SrcX
							   lpDIBRect->top,               // SrcY
							   RECTWIDTH(lpDIBRect),         // wSrcWidth
							   RECTHEIGHT(lpDIBRect),        // wSrcHeight
							   m_pBits,                      // lpBits
							   m_pBMI,                       // lpBitsInfo
							   DIB_RGB_COLORS,               // wUsage
							   SRCCOPY);                     // dwROP
	pDC->BitBlt(0,0,RECTWIDTH(lpDCRect),RECTHEIGHT(lpDCRect),pMemDC,0,0,SRCCOPY);

	pMemDC->SelectObject(pOldBM);
	bm.DeleteObject();
	
	if(pMemDC != NULL)
	{
		pMemDC->DeleteDC();
		delete pMemDC;
		pMemDC = NULL;
	}

	/* Reselect old palette */
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}

	return bSuccess;
    //return CDib::Paint(pDC, lpDCRect, lpViewRect, lpDIBRect);
}


/*************************************************************************
  函数名称: BmpToSaperaFmt()
  功能描述: 将图像数据从BMP格式转换到Sapera格式

  输入参数:         
            无 
            
  输出参数:
            无 

  返 回 值:
            TRUE：转换成功 FALSE：转换失败(不支持的数据格式)

  其它说明:
            BMP格式数据组织方式和Sapera的不同，BMP格式数据从下到上排列，且每行
            字节数为4的倍数，Sapera格式数据从上到下，每行字节数为实际宽度对应
            字节数。但GDI显示时只支持BMP格式，为了便于其他项目组直接使用算法库，
            故作此转换。此外，Sapera格式数据也是算法库采用的格式。    

  作    者: 周明才
  编写时间: 2005.12.16
*************************************************************************/
BOOL CMyDib::BmpToSaperaFmt()
{
	WORD wBitCount;  // DIB bit count
	wBitCount = m_pBMI->bmiHeader.biBitCount;

	switch (wBitCount) //只处理8位、24位和32位图像
	{
		case 8:
		case 24:
		case 32:
			break;
		default:
			return FALSE;
	}

    int     iWidth  = Width();
    int     iHeight = Height();
    int     iLineBytes;                 //每行字节数（BMP格式）
    int     iWidthBytes;                //每行字节数（Sapera格式）
    UINT    nCount;                     //BMP格式数据大小（BMP格式）
    LPBYTE  pbyDataCpy, pbyTmpRes, pbyTmpDst;

    // Calculate the number of bytes per line
    iLineBytes  = WIDTHBYTES(wBitCount * Width());
	nCount      = iLineBytes * iHeight;

    iWidthBytes = (wBitCount * Width()) / 8 ;

    pbyDataCpy  = new BYTE[nCount];
    memcpy(pbyDataCpy, m_pBits, nCount);

    int j;

    pbyTmpDst   = m_pBits;
    pbyTmpRes   = pbyDataCpy + nCount - iLineBytes;

    //从pbyDataCpy的顶部开始取各行数据，对m_pBits从底部开始填充。
    for (j=0; j<iHeight; j++) 
    {
        memcpy(pbyTmpDst, pbyTmpRes, iWidthBytes);
        pbyTmpDst += iWidthBytes;
        pbyTmpRes -= iLineBytes;
    }

    delete [] pbyDataCpy;
    return TRUE;
}

/*************************************************************************
  函数名称: SaperaToBmpFmt()
  功能描述: 将图像数据从Sapera格式转换到BMP格式

  输入参数:         
            无 
            
  输出参数:
            无 

  返 回 值:
            TRUE：转换成功 FALSE：转换失败(不支持的数据格式)

  其它说明:
            BMP格式数据组织方式和Sapera的不同，BMP格式数据从下到上排列，且每行
            字节数为4的倍数，Sapera格式数据从上到下，每行字节数为实际宽度对应
            字节数。但GDI显示时只支持BMP格式，为了便于其他项目组直接使用算法库，
            故作此转换。此外，Sapera格式数据也是算法库采用的格式。    

  作    者: 周明才
  编写时间: 2005.12.16
*************************************************************************/
BOOL CMyDib::SaperaToBmpFmt()
{
	WORD wBitCount;  // DIB bit count
	wBitCount = m_pBMI->bmiHeader.biBitCount;

	switch (wBitCount) //只处理8位、24位和32位图像
	{
		case 8:
		case 24:
		case 32:
			break;
		default:
			return FALSE;
	}

    int     iWidth  = Width();
    int     iHeight = Height();
    int     iLineBytes;          //每行字节数（BMP格式）
    int     iWidthBytes;         //每行字节数（Sapera格式）
    UINT    nCount;              //BMP格式数据大小（BMP格式）
    LPBYTE  pbyDataCpy, pbyTmpRes, pbyTmpDst;

    // Calculate the number of bytes per line
    iLineBytes  = WIDTHBYTES(wBitCount * Width());
	nCount      = iLineBytes * iHeight;

    iWidthBytes = (wBitCount * Width()) / 8;

    pbyDataCpy  = new BYTE[nCount];
    memcpy(pbyDataCpy, m_pBits, nCount);        //为简单起见，做一份拷贝，

    int j;

    pbyTmpDst   = m_pBits + nCount - iLineBytes;
    pbyTmpRes   = pbyDataCpy;

    //从pbyDataCpy的底部开始取各行数据，对m_pBits从顶部开始填充。
    for (j=0; j<iHeight; j++) 
    {
        memcpy(pbyTmpDst, pbyTmpRes, iWidthBytes);
        pbyTmpDst -= iLineBytes;
        pbyTmpRes += iWidthBytes;
    }

    delete [] pbyDataCpy;
    return TRUE;
}

/*************************************************************************
  函数名称: Create()
  功能描述: 根据参数创建图像
  说    明: BOOL bIsRawData用于指明pbyDataAddr是草稿数据还是做了4字节对齐的数据
            (TRUE: 草稿数据  FALSE: 4字节对齐的数据)
  作    者: 周明才
  编写时间: 2005.12.30
*************************************************************************/
void CMyDib:: Create(int iWidth, int iHeight, Format format, LPBYTE pbyDataAddr, 
                    BOOL bIsRawData)
{
    // Ensures no memory leaks will occur
    Free();

    if (iWidth < 0 || iHeight < 0) {
        iWidth  = 50;
        iHeight = 50;
    }

	int     bitCount, i;
    UINT    n;

    //目前只支持8位灰度图的创建
    switch(format) {
        case FormatUByte:
            bitCount = 8;
    	    break;
		case FormatRgb:
			bitCount=24;
			break;
        default:
            return;
            break;
    }

	//为位图信息头分配内存
	m_pBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD));
	if (m_pBMI == NULL)
		return;

    //设置位图信息头结构
    m_pBMI->bmiHeader.biSize            = sizeof(BITMAPINFOHEADER); 
    m_pBMI->bmiHeader.biWidth           = iWidth;           
    m_pBMI->bmiHeader.biHeight          = iHeight;          
    m_pBMI->bmiHeader.biPlanes          = 1;                        
    m_pBMI->bmiHeader.biBitCount        = bitCount;                 
    m_pBMI->bmiHeader.biCompression     = BI_RGB;                   
    m_pBMI->bmiHeader.biSizeImage       = 0;                        
    m_pBMI->bmiHeader.biXPelsPerMeter   = 0;                        
    m_pBMI->bmiHeader.biYPelsPerMeter   = 0;                        
    m_pBMI->bmiHeader.biClrUsed         = 0;                        
    m_pBMI->bmiHeader.biClrImportant    = 0;                        

	//填充调色板
	for(i=0; i<256; ++i) 
    {
		m_pBMI->bmiColors[i].rgbRed     = i;
		m_pBMI->bmiColors[i].rgbGreen   = i;
		m_pBMI->bmiColors[i].rgbBlue    = i;
    }
	
	DWORD dwBmBitsSize;
	dwBmBitsSize = WIDTHBYTES(bitCount * iWidth) * iHeight;

	//为位图内容分配内存
	m_pBits = (LPBYTE)GlobalAllocPtr(GHND, dwBmBitsSize);
	if (m_pBits == NULL)
	{
		GlobalFreePtr(m_pBMI);
		m_pBMI = NULL;
		return;
	}

    if (pbyDataAddr == NULL)
    {
        for (n=0; n<=dwBmBitsSize-1; ++n)
		{
            m_pBits[n] = 128;
        }
    }
    else
    {
        //2006.01.10 增加一个参数bIsRawData指明传入的数据组织形式
        //草稿数据
        if (bIsRawData == TRUE) 
        {
            dwBmBitsSize = (bitCount * iWidth) * iHeight / 8;
            memcpy(m_pBits, pbyDataAddr, dwBmBitsSize * sizeof(BYTE) );
            SaperaToBmpFmt();   //数据要重组
        }
        //4字节对齐的数据
        else
        {
            memcpy(m_pBits, pbyDataAddr, dwBmBitsSize * sizeof(BYTE) );
        }
    }

	CreatePalette();
}
// 重载拷贝构造函数
// 2005.12.30
CMyDib::CMyDib(int iWidth, int iHeight, Format format, LPBYTE pbyDataAddr, BOOL bIsRawData)
{
   Create(iWidth, iHeight, format, pbyDataAddr, bIsRawData);
}


// 重载拷贝构造函数
// 2005.12.30
CMyDib::CMyDib(const CMyDib& mydib)
{
   Format   format;
   BOOL     bIsRawData = FALSE;
   //只处理256色灰度图
   if (mydib.NumColors() == 256) 
        format = FormatUByte;
   else
        format = FormatUnknown;

   Create(mydib.Width(), mydib.Height(), FormatUByte, mydib.GetImgDataAddr(), bIsRawData);
}
