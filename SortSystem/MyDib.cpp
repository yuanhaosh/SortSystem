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

// ��ͼ���ļ��ж�ȡ����
// 2005.12.9
BOOL CMyDib::ReadBMPFile(LPCTSTR lpszPathName)
{
	DWORD dWord = 0;
    CFile file;
    //���ļ�ʧ��
    if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite))
    {
        ASSERT("���ļ�����!");
        return FALSE;
    }
    dWord = Read(file);
	
    file.Close();

	return dWord>0?TRUE:FALSE;
}

// �������ݵ�ͼ���ļ�
// 2005.12.9
BOOL CMyDib::SaveToFile(LPCTSTR lpszPathName)
{
	DWORD dWord = 0;
    CFile file;
    //���ļ�
    if (!file.Open(lpszPathName, CFile::modeCreate |
        CFile::modeReadWrite | CFile::shareExclusive))
    {
        return FALSE;
    }

    TRY
    {
        //����ΪDIBͼ��
        dWord = Save(file);
        
        file.Close();
    }
    //����ʧ��
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
    //������һ�������Ա�����˸��Ŀǰ��û�С�
    return CDib::Paint(hDC,lpDCRect, lpDIBRect);

}

BOOL CMyDib::Paint(CDC* pDC, LPRECT lpDCRect, LPRECT lpViewRect, LPRECT lpDIBRect) const
{
	//����һ�������Ա�����˸
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
  ��������: BmpToSaperaFmt()
  ��������: ��ͼ�����ݴ�BMP��ʽת����Sapera��ʽ

  �������:         
            �� 
            
  �������:
            �� 

  �� �� ֵ:
            TRUE��ת���ɹ� FALSE��ת��ʧ��(��֧�ֵ����ݸ�ʽ)

  ����˵��:
            BMP��ʽ������֯��ʽ��Sapera�Ĳ�ͬ��BMP��ʽ���ݴ��µ������У���ÿ��
            �ֽ���Ϊ4�ı�����Sapera��ʽ���ݴ��ϵ��£�ÿ���ֽ���Ϊʵ�ʿ�ȶ�Ӧ
            �ֽ�������GDI��ʾʱֻ֧��BMP��ʽ��Ϊ�˱���������Ŀ��ֱ��ʹ���㷨�⣬
            ������ת�������⣬Sapera��ʽ����Ҳ���㷨����õĸ�ʽ��    

  ��    ��: ������
  ��дʱ��: 2005.12.16
*************************************************************************/
BOOL CMyDib::BmpToSaperaFmt()
{
	WORD wBitCount;  // DIB bit count
	wBitCount = m_pBMI->bmiHeader.biBitCount;

	switch (wBitCount) //ֻ����8λ��24λ��32λͼ��
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
    int     iLineBytes;                 //ÿ���ֽ�����BMP��ʽ��
    int     iWidthBytes;                //ÿ���ֽ�����Sapera��ʽ��
    UINT    nCount;                     //BMP��ʽ���ݴ�С��BMP��ʽ��
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

    //��pbyDataCpy�Ķ�����ʼȡ�������ݣ���m_pBits�ӵײ���ʼ��䡣
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
  ��������: SaperaToBmpFmt()
  ��������: ��ͼ�����ݴ�Sapera��ʽת����BMP��ʽ

  �������:         
            �� 
            
  �������:
            �� 

  �� �� ֵ:
            TRUE��ת���ɹ� FALSE��ת��ʧ��(��֧�ֵ����ݸ�ʽ)

  ����˵��:
            BMP��ʽ������֯��ʽ��Sapera�Ĳ�ͬ��BMP��ʽ���ݴ��µ������У���ÿ��
            �ֽ���Ϊ4�ı�����Sapera��ʽ���ݴ��ϵ��£�ÿ���ֽ���Ϊʵ�ʿ�ȶ�Ӧ
            �ֽ�������GDI��ʾʱֻ֧��BMP��ʽ��Ϊ�˱���������Ŀ��ֱ��ʹ���㷨�⣬
            ������ת�������⣬Sapera��ʽ����Ҳ���㷨����õĸ�ʽ��    

  ��    ��: ������
  ��дʱ��: 2005.12.16
*************************************************************************/
BOOL CMyDib::SaperaToBmpFmt()
{
	WORD wBitCount;  // DIB bit count
	wBitCount = m_pBMI->bmiHeader.biBitCount;

	switch (wBitCount) //ֻ����8λ��24λ��32λͼ��
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
    int     iLineBytes;          //ÿ���ֽ�����BMP��ʽ��
    int     iWidthBytes;         //ÿ���ֽ�����Sapera��ʽ��
    UINT    nCount;              //BMP��ʽ���ݴ�С��BMP��ʽ��
    LPBYTE  pbyDataCpy, pbyTmpRes, pbyTmpDst;

    // Calculate the number of bytes per line
    iLineBytes  = WIDTHBYTES(wBitCount * Width());
	nCount      = iLineBytes * iHeight;

    iWidthBytes = (wBitCount * Width()) / 8;

    pbyDataCpy  = new BYTE[nCount];
    memcpy(pbyDataCpy, m_pBits, nCount);        //Ϊ���������һ�ݿ�����

    int j;

    pbyTmpDst   = m_pBits + nCount - iLineBytes;
    pbyTmpRes   = pbyDataCpy;

    //��pbyDataCpy�ĵײ���ʼȡ�������ݣ���m_pBits�Ӷ�����ʼ��䡣
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
  ��������: Create()
  ��������: ���ݲ�������ͼ��
  ˵    ��: BOOL bIsRawData����ָ��pbyDataAddr�ǲݸ����ݻ�������4�ֽڶ��������
            (TRUE: �ݸ�����  FALSE: 4�ֽڶ��������)
  ��    ��: ������
  ��дʱ��: 2005.12.30
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

    //Ŀǰֻ֧��8λ�Ҷ�ͼ�Ĵ���
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

	//Ϊλͼ��Ϣͷ�����ڴ�
	m_pBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD));
	if (m_pBMI == NULL)
		return;

    //����λͼ��Ϣͷ�ṹ
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

	//����ɫ��
	for(i=0; i<256; ++i) 
    {
		m_pBMI->bmiColors[i].rgbRed     = i;
		m_pBMI->bmiColors[i].rgbGreen   = i;
		m_pBMI->bmiColors[i].rgbBlue    = i;
    }
	
	DWORD dwBmBitsSize;
	dwBmBitsSize = WIDTHBYTES(bitCount * iWidth) * iHeight;

	//Ϊλͼ���ݷ����ڴ�
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
        //2006.01.10 ����һ������bIsRawDataָ�������������֯��ʽ
        //�ݸ�����
        if (bIsRawData == TRUE) 
        {
            dwBmBitsSize = (bitCount * iWidth) * iHeight / 8;
            memcpy(m_pBits, pbyDataAddr, dwBmBitsSize * sizeof(BYTE) );
            SaperaToBmpFmt();   //����Ҫ����
        }
        //4�ֽڶ��������
        else
        {
            memcpy(m_pBits, pbyDataAddr, dwBmBitsSize * sizeof(BYTE) );
        }
    }

	CreatePalette();
}
// ���ؿ������캯��
// 2005.12.30
CMyDib::CMyDib(int iWidth, int iHeight, Format format, LPBYTE pbyDataAddr, BOOL bIsRawData)
{
   Create(iWidth, iHeight, format, pbyDataAddr, bIsRawData);
}


// ���ؿ������캯��
// 2005.12.30
CMyDib::CMyDib(const CMyDib& mydib)
{
   Format   format;
   BOOL     bIsRawData = FALSE;
   //ֻ����256ɫ�Ҷ�ͼ
   if (mydib.NumColors() == 256) 
        format = FormatUByte;
   else
        format = FormatUnknown;

   Create(mydib.Width(), mydib.Height(), FormatUByte, mydib.GetImgDataAddr(), bIsRawData);
}
