/****************************************************************************
   CImageAlgorithm.cpp
   
   ʵ����ͼ��ƥ���е��㷨


   Ԭ�� 2015/03/12


*****************************************************************************/
#include "stdafx.h"
#include "ImageAlgorithm.h"




CImageAlgorithm::CImageAlgorithm()
{
	//keypoints(10) {};
}

CImageAlgorithm::~CImageAlgorithm()
{

}

/************************************************************************
������: DisplayOpenCVInfo
��  ��: void
��  ��: ��ʾOpenCV��װ��Ϣ��IPP��Ϣ
*************************************************************************/
int CImageAlgorithm::DisplayOpenCVInfo(void)
{
	const char* libraries;
	const char* modules;
	cvGetModuleInfo(0, &libraries, &modules);
	printf("OpenCV Librayies: %s\nModules: %s\n", libraries, modules);
	return 0;
}

/************************************************************************
������: ShowDebugPhoto
��  ��: IplImage* img
��  ��: ��imgͼ����ʾ����
*************************************************************************/
int CImageAlgorithm::ShowDebugPhoto(IplImage* img)
{
	cvNamedWindow("img",CV_WINDOW_AUTOSIZE);
	cvShowImage("img", img);
	cvWaitKey(0);
	//cvReleaseImage(&img);
	cvDestroyWindow("img");
	return 0;
}

/************************************************************************
������: PaintCircle
��  ��: IplImage* img
        sCircle*  sCircle1
��  ��: ��ͼ���л�һ����ɫ��Բ
*************************************************************************/
int CImageAlgorithm::PaintCircle(IplImage* img, sCircle*  sCircle1)
{
	/*CvFont font; 
    cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX, 0.5, 0.5, 1, 2, 8); */
	cvCircle(img, sCircle1->CirclePoint, sCircle1->iRadius, cvScalar(255, 255, 255)); 
	ShowDebugPhoto(img);
	return 0;
}

/************************************************************************
������: MatchTemplateSimilarityValue
��  ��: IplImage* ChkBallPic
        IplImage* FuleBallPic
		IplImage* EmptyBallPic
��  ��: ���ȼ��������ƶȴ��ڰ����Ҵ������ƶ���ͷ�ֵSimilarThreshold��Ϊȼ����
        ��ȼ������ڰ��������ʵ�С����ͷ�ֵSimilarThreshold����ʾƥ��ʧ�ܡ�
����ֵ: 1:  ���Ϊȼ����
        0:  �������߰���
	   -1: ���ʧ��
*************************************************************************/
int CImageAlgorithm::MatchTemplateSimilarityValue(IplImage* ChkBallPic, IplImage* FuleBallPic, IplImage* EmptyBallPic )
{
    //������
    double dChkPicNormVerticalShadow[VALUE128];
	double dChkPicNormHorizontalShadow[VALUE128];
    GetNormShadowValue(ChkBallPic, dChkPicNormVerticalShadow, dChkPicNormHorizontalShadow);
	
	//ȼ����
	double dFulePicNormVerticalShadow[VALUE128];
	double dFulePicNormHorizontalShadow[VALUE128];
    GetNormShadowValue(FuleBallPic, dFulePicNormVerticalShadow, dFulePicNormHorizontalShadow);
	
	//����
	double dEmptyPicNormVerticalShadow[VALUE128];
	double dEmptyPicNormHorizontalShadow[VALUE128];
    GetNormShadowValue(EmptyBallPic, dEmptyPicNormVerticalShadow, dEmptyPicNormHorizontalShadow);
	
	//����������ϵ��
	double dVerticalCorrValue1, dHorizontalCorrValue1; //ȼ����
	dVerticalCorrValue1 = Corrcoef(dChkPicNormVerticalShadow, dFulePicNormVerticalShadow, VALUE128);
	dHorizontalCorrValue1 = Corrcoef(dChkPicNormHorizontalShadow, dFulePicNormHorizontalShadow, VALUE128);

	double dVerticalCorrValue2, dHorizontalCorrValue2; //����
	dVerticalCorrValue2 = Corrcoef(dChkPicNormVerticalShadow, dEmptyPicNormVerticalShadow, VALUE128);
	dHorizontalCorrValue2 = Corrcoef(dChkPicNormHorizontalShadow, dEmptyPicNormHorizontalShadow, VALUE128);
	printf("��������ȼ�������ƶ�:\n");
	printf("��ֱ���ƶ�Ϊ:%f, ˮƽ���ƶ�Ϊ:%f\n", dVerticalCorrValue1, dHorizontalCorrValue1);

	//�Ƚ����ϵ��������Чֵ
	double dCorrValue1, dCorrValue2;
	dCorrValue1 = dVerticalCorrValue1 + dHorizontalCorrValue1;
	dCorrValue2 = dVerticalCorrValue2 + dHorizontalCorrValue2;
	printf("��������������ƶ�:\n");
	printf("��ֱ���ƶ�Ϊ:%f, ˮƽ���ƶ�Ϊ:%f\n", dVerticalCorrValue2, dHorizontalCorrValue2);
	
	int ret = 0;
	if (dCorrValue1 > dCorrValue2 && dCorrValue1 > SimilarThreshold)
	{
		//printf("��ֱ���ƶ�Ϊ:%f, ˮƽ���ƶ�Ϊ:%f\n", dVerticalCorrValue1, dHorizontalCorrValue1);
		ret = 1; //ȼ����
	}
	else if(dCorrValue1 > dCorrValue2 && dCorrValue1 < SimilarThreshold)
	{
		ret =  -1;
	}
	else
	{
		ret = 0;
	}

    return ret;
}

/**********************************************************************
������: GetNormShadowValue
��  ��: IplImage* Image                Դͼ��
		double* dNormVerticalShadow    ͶӰ��ֱ��һ������
		double* dNormHorizontalShadow  ͶӰˮƽ��һ������
��  ��: ������Сͼ��ĳߴ磬��������ٶ�
        Ȼ�������ݷ���ͶӰ
		����һ�����ݷ���ͶӰ
��  ע: �����Ӻ��� CalcColumnShadow, NormalizedShadowDivDCComponet
**********************************************************************/
int CImageAlgorithm::GetNormShadowValue(IplImage* Image, double* dNormVerticalShadow, double* dNormHorizontalShadow)
{
    int iWidth = 0;                //standard size
	int iHeight = 0;
	IplImage* cvTImage = 0;      //���ڴ洢ת�ú��ͼ��
	CvSize cvDstSize;
	double dImageVeriticalShadow[VALUE128];  //���ڴ洢����ͶӰֵ
	double dImageHorizontalShadow[VALUE128];

	assert(Image);
	iWidth = Image->width;   
	iHeight = Image->height;
	cvDstSize.height = iHeight / NARROWSIZE;
	cvDstSize.width = iWidth / NARROWSIZE;
    
	{
		//ͼ����С4�����������Ч�ʣ�������ԭͼЧ�����첻�� 
		//tips��ͼ����С�ı�������������ÿ�����������Բ����÷���main������
		IplImage* NarrowPic =  cvCreateImage(cvDstSize, IPL_DEPTH_8U, 1);
		cvResize( Image, NarrowPic);
	
		//��Сͼ����ͶӰ
		memset(dImageVeriticalShadow, 0 , VALUE128 * sizeof(double));
		CalcColumnShadow(NarrowPic, dImageVeriticalShadow, cvDstSize.height);

		//��Сͼ����ͶӰ
		cvTImage = cvCloneImage(NarrowPic);
		cvT(NarrowPic, cvTImage);
		memset(dImageHorizontalShadow, 0, VALUE128 * sizeof(double));
		CalcColumnShadow(cvTImage, dImageHorizontalShadow, cvDstSize.width);

		//ͶӰȥ��ֱ����������һ��
		memset(dNormVerticalShadow, 0, VALUE128 * sizeof(double));
		NormalizedShadowDivDCComponet(dImageVeriticalShadow, dNormVerticalShadow);

		memset(dNormHorizontalShadow, 0, VALUE128 * sizeof(double));
		NormalizedShadowDivDCComponet(dImageHorizontalShadow,  dNormHorizontalShadow);
	}
	return 0;
}

/*********************************************************************\
��������CalcColumnShadow
������  IplImage*      image
        unsigned int*  uiValue
		int            iShadowAxisValue
���ܣ�   ����ͼ��ÿ�кͣ�then������/������ͶӰ

**************************************************************************/
int CImageAlgorithm::CalcColumnShadow(IplImage* image, double* dValue, int iShadowAxisValue)
{
	if(iShadowAxisValue <= 0)
	{
		return -1;
	}
    if(iShadowAxisValue != 1)
	{
		for(int i = 0; i < image->height; i++)
		{
			for (int j = 0; j < image->width; j++)
			{
				*(dValue + i) += (BYTE) *((image->imageData) + i * image->widthStep + j);
			}
			*(dValue + i) /= iShadowAxisValue;
		}
	}
	if(iShadowAxisValue == 1)
	{
		for(int i = 0; i < image->height; i++)
		{
			for (int j = 0; j < image->width; j++)
			{
				*(dValue + i) += (BYTE) *((image->imageData) + i * image->widthStep + j);
			}
		}
	}
	return 0;
}

/*********************************************************************\
��������NormalizedShadowDivDCComponet
����:   double* dSrcValue
        double* dDstValue
���ܣ�  ͼ��ͶӰ����ֱ�������������һ��ͶӰ
**************************************************************************/
int CImageAlgorithm::NormalizedShadowDivDCComponet(double* dSrcValue, double* dDstValue )
{
	double dSumValue = 0;
	double dTempAverageValue = 0;
	double dMaxValue = 0;
	
	//��ͶӰ����ĺͣ����ҳ����ֵ
	for(int j = 0; j < VALUE128; j++)
	{
		dSumValue += *(dSrcValue + j);
		if( dMaxValue < *(dSrcValue + j) )
		{
			dMaxValue = *(dSrcValue + j);
		}
	}
	dTempAverageValue = dSumValue / VALUE128;

	for(int i = 0; i < VALUE128; i++)
	{
        *(dDstValue + i) = ( *(dSrcValue + i) - dTempAverageValue) / dMaxValue;
    
	}
	return 0;
}

/************************************************************
������: Corrcoef
��  ��: double* x
        double* y
		int ArraySize
��  ��: ��������x,y������������ϵ��,�����ظ�ֵ
*************************************************************/
double CImageAlgorithm::Corrcoef(const double* x, const double* y, int iArraySize)
{
	double r = 0.0, r1 = 0.0, r2 = 0.0;
    double dsumX = 0.0;
	double dsumY = 0.0;
	double dsumXY = 0.0;
	double dsumXX = 0.0;
	double dsumYY = 0.0;
	for(int i = 0; i < iArraySize; i++)
	{
		dsumX += *(x + i);
		dsumY += *(y + i);
		dsumXY += (*(x + i)) * (*(y + i));
		dsumXX += (*(x + i)) * (*(x + i));
		dsumYY += (*(y + i)) * (*(y + i));;
	}
	r1 = dsumXY - dsumX * dsumY / iArraySize;
	r2 = sqrt( (dsumXX - dsumX * dsumX / iArraySize) * (dsumYY - dsumY * dsumY / iArraySize) );
	r = r1 / r2;

	return r;
}

/************************************************************
������: Corrcoef
��  ��: double* x
        double* y
		int ArraySize
��  ��: ��������x,y������������ϵ��,�����ظ�ֵ
*************************************************************/
double CImageAlgorithm::Corrcoef(const int* x, const int* y, int iArraySize)
{
	double r = 0.0, r1 = 0.0, r2 = 0.0;
    double dsumX = 0.0;
	double dsumY = 0.0;
	double dsumXY = 0.0;
	double dsumXX = 0.0;
	double dsumYY = 0.0;
	for(int i = 0; i < iArraySize; i++)
	{
		dsumX += *(x + i);
		dsumY += *(y + i);
		dsumXY += (*(x + i)) * (*(y + i));
		dsumXX += (*(x + i)) * (*(x + i));
		dsumYY += (*(y + i)) * (*(y + i));;
	}
	r1 = dsumXY - dsumX * dsumY / iArraySize;
	r2 = sqrt( (dsumXX - dsumX * dsumX / iArraySize) * (dsumYY - dsumY * dsumY / iArraySize) );
	r = r1 / r2;

	return r;
}

/****************************************************************************
������: EdgeAndThreshold
��  ����IplImage* src
        IplImage* dst
��  ��: ʹ�ñ�Ե���ԻҶ�ͼ����м��,����ʹ�÷�ֵ�õ���ֵ��ͼ��
****************************************************************************/
int CImageAlgorithm::EdgeAndThreshold(IplImage* src, IplImage* dst)
{
	IplImage* soble16S = cvCreateImage(cvGetSize(src), IPL_DEPTH_16S, 1);

	 //Ԥ�����ں�
	cvSobel(src, soble16S, 1, 1, 1); 
	cvConvertScaleAbs(soble16S, dst, 1, 0 );
	cvThreshold(dst, dst, (SOBELTHRESHOLD * 255), 255, CV_THRESH_BINARY);
	cvSmooth( dst, dst, CV_MEDIAN ); //ȥ������������ʹ����ֵ�˲�
	cvSmooth(dst,dst,CV_GAUSSIAN,7,7);
	cvThreshold(dst, dst, (SOBELTHRESHOLD * 255), 255, CV_THRESH_BINARY);

	/* Ԥ������ǰ
	cvSmooth( src, dst, CV_MEDIAN ); //ȥ������������ʹ����ֵ�˲�
	cvSmooth(dst,dst,CV_GAUSSIAN);

	cvSobel(dst, soble16S, 1, 1, 1); 
 	cvConvertScaleAbs(soble16S, dst, 1, 0 );
	cvThreshold(dst, dst, (SOBELTHRESHOLD * 255), 255, CV_THRESH_BINARY);

	ShowDebugPhoto(dst);
	*/
	return 0;
}

/***************************************************************************
������: FindCircleRadius
��  ��: IplImage* src
        sCircle* sCircle1
��  �ܣ����ûҶ�ͼ����X,Y���ϵ�ͶӰ���㣬Բ�İ뾶��Բ��  
****************************************************************************/
int CImageAlgorithm::FindCircleRadius(IplImage* src, sCircle* sCircle1)
{
	int iLineLeftPoint = 0, iLineRightPoint = 0;
	double dArray[VALUE512];
	memset(dArray, 0, sizeof(double) * VALUE512);
	CalcColumnShadow(src, dArray, 1);
	for(int i = 0; i < VALUE512; i++)
	{
		if( *(dArray + i) != 0 && *(dArray + i + 1) != 0  )
		{ 
			iLineLeftPoint = i; break;
		}
	}

	for(int i = VALUE512; i > 0; i--)
	{
		if(*(dArray + i) != 0 && *(dArray + i -1) != 0 )
		{
			iLineRightPoint =i; break;
		}
	}
	sCircle1->iRadius = (iLineRightPoint - iLineLeftPoint) / 2;
	sCircle1->CirclePoint.x = sCircle1->iRadius + iLineLeftPoint;

	
	IplImage* cvTImage = cvCloneImage(src);
	cvT(src, cvTImage);

	memset(dArray, 0, sizeof(double) * VALUE512);
	CalcColumnShadow(cvTImage, dArray, 1);
	for(int i = 0; i < VALUE512; i++)
	{
		if( *(dArray + i) != 0 && *(dArray + i + 1) != 0  )
		{ 
			iLineLeftPoint = i; break;
		}
	}

	for(int i = VALUE512; i > 0; i--)
	{
		if(*(dArray + i) != 0 && *(dArray + i -1) != 0 )
		{
			iLineRightPoint =i; break;
		}
	}

	if( sCircle1->iRadius >  ((iLineRightPoint - iLineLeftPoint) / 2) )
	{
	    sCircle1->iRadius = (iLineRightPoint - iLineLeftPoint) / 2;
	}

	sCircle1->CirclePoint.y = (iLineRightPoint - iLineLeftPoint) / 2 + iLineLeftPoint;

	return 0;
}

/***************************************************************************
������: LableConnectedDomainInSpecifiedZone
��  ��: IplImage* src
        sCircle*  sCircle1 //ָ������
��  ��: �ҳ�ָ�������е���ͨ��
****************************************************************************/
int CImageAlgorithm::LableConnectedDomainInSpecifiedZone( IplImage* src, sCircle*  sCircle1 )
{
	assert(src);
	double dPintY = (double)sCircle1->CirclePoint.y;
	double dPintX = (double)sCircle1->CirclePoint.x;
	double dCircleRadius = (double)sCircle1->iRadius;
	IplImage* img = cvCloneImage(src);
	//��ָ�����򻮷ֳ��� 
	for(int i = 0; i < img->height; i++)
	{
		for(int j = 0; j < img->width; j++)
		{
			if(pow( dPintY - i, 2) + pow( dPintX - j, 2) > pow( dCircleRadius - 10, 2 ))
			{
				ELEM(img, i, j) = 0;
			}
		}
	}
	//�����ͨ��
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	CvSeq* contmax = 0;
	int area = 0, maxArea = CONTOUR_THRESHOLD_AREA;
	cvFindContours( img, storage, &contour, sizeof(CvContour),  CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
	for( ; contour != 0; contour = contour->h_next )    
	{
		area = (int)fabs( cvContourArea( contour));
		if(area >= maxArea)
		{
			contmax = contour;
			maxArea = area;
		}
	}
	//�ҳ������ͨ��
	return maxArea;

}

/***************************************************************************
������: FindPointsFromUpToDown  
��  ��: IplImage* src     
        int iNum  ��������
		CvPoint point[] ���ڴ洢�ɼ�����ϵ�
��  ��: ��ͼ����Ϸ�����Ѱ�ҷ����  1
****************************************************************************/
int CImageAlgorithm::FindPointsFromUpToDown(IplImage* src, int iNum, CvPoint point[])
{
	int iSelectedCloumn = 0;
	for(int i = 0; i < iNum; i++)
	{
		for(int j = 0; j < src->height; j++)
		{
			if( 0 != CV_IMAGE_ELEM(src, uchar, j, iSelectedCloumn ) && 0 != CV_IMAGE_ELEM(src, uchar, j + 1, iSelectedCloumn ))
			{
				point[i].x = iSelectedCloumn;
				point[i].y = j;
				break;
			}
		}
		iSelectedCloumn += (src->width) / iNum;
		
	}
	return 0;
}

/***************************************************************************
������: FindPointsFromRightToLeft
��  ��: IplImage* src     
        int iNum  ��������
		CvPoint point[] ���ڴ洢�ɼ�����ϵ�
��  ��: ��ͼ����ҷ�����Ѱ�ҷ����  2
****************************************************************************/
int CImageAlgorithm::FindPointsFromRightToLeft(IplImage* src, int iNum, CvPoint point[])
{
	int iSelectedRow = 0;
	for(int i = 0; i < iNum; i++) //H
	{
		for(int j = src->width; j > 0; j--)
		{
			if( 0 != CV_IMAGE_ELEM(src, uchar, iSelectedRow, j ) && 0 != CV_IMAGE_ELEM(src, uchar, iSelectedRow, j - 1 ))
			{
				point[i].x = j;
				point[i].y = iSelectedRow;
				break;
			}
		}
		iSelectedRow += (src->width) / iNum;
	}
	return 0;
}

/***************************************************************************
������: FindPointsFromDownToUp
��  ��: IplImage* src     
        int iNum  ��������
		CvPoint point[] ���ڴ洢�ɼ�����ϵ�
��  ��: ��ͼ����·�����Ѱ�ҷ����  3
****************************************************************************/
int CImageAlgorithm::FindPointsFromDownToUp(IplImage* src, int iNum, CvPoint point[])
{
	int iSelectedCloumn = 0;
	for(int i = 0; i < iNum; i++)
	{
		for(int j = src->height; j > 0; j--)
		{
			if( 0 != CV_IMAGE_ELEM(src, uchar, j, iSelectedCloumn ) && 0 != CV_IMAGE_ELEM(src, uchar, j - 1, iSelectedCloumn ))
			{
				point[i].x = iSelectedCloumn;
				point[i].y = j;
				break;
			}
		}
		iSelectedCloumn += (src->width) / iNum;
	}
	return 0;
}

/***************************************************************************
������: FindPointsFromLeftToRight
��  ��: IplImage* src     
        int iNum  ��������
		CvPoint point[] ���ڴ洢�ɼ�����ϵ�
��  ��: ��ͼ����ҷ�����Ѱ�ҷ����  4
****************************************************************************/
int CImageAlgorithm::FindPointsFromLeftToRight(IplImage* src, int iNum, CvPoint point[])
{
	int iSelectedRow = 0;
	for(int i = 0; i < iNum; i++) //H
	{
		for(int j = 0; j < src->width; j++)
		{
			if( 0 != CV_IMAGE_ELEM(src, uchar, iSelectedRow, j ) && 0 != CV_IMAGE_ELEM(src, uchar, iSelectedRow, j + 1 ))
			{
				point[i].x = j;
				point[i].y = iSelectedRow;
				break;
			}
		}
		iSelectedRow += (src->width) / iNum;
	}
	return 0;
}


/***************************************************************************
������: GetFittingPoint
��  ��: IplImage* src
        int iDirection    ���������������,��������ֱ�Ϊ 1, 2, 3, 4        
        int iNum          ��������
		CvPoint point[]   ���ڴ洢�ɼ�����ϵ�
��  ��: �����ɼ�ָ��ͼ�������ϵ�
****************************************************************************/
int CImageAlgorithm::GetFittingPointFromDir(IplImage* src, int iDirection, int iNum, CvPoint point[])
{
	switch(iDirection)
	{
		case 1: FindPointsFromUpToDown(src, iNum, point);    break;
		case 2: FindPointsFromRightToLeft(src, iNum, point); break;
		case 3: FindPointsFromDownToUp(src, iNum, point);    break;
		case 4: FindPointsFromLeftToRight(src, iNum, point); break;
		default: return -1;
	}
	return 0;
}

/******************************************************************************
�������� LSMFitCircle
��  ���� double x
         double y 
		 int n    �ɼ����ĵ���
         sCircle* sCircle1
��  �ܣ� ������С���˷����Բ
*******************************************************************************/
int CImageAlgorithm::LSMFitCircle(double x[], double y[], int n, sCircle* sCircle1)
{

	int i=0;
    double X1,Y1,X2,Y2,Z1,X3,Y3,Z2,Z3;
    double p;
   	double *tp=NULL;
	//tp= (double*)malloc((unsigned)(n * sizeof(double)));
	tp= (double*)malloc((unsigned int)(n * sizeof(double)));
    X1=Average(x,n);
	Y1=Average(y,n);
	
	for(i=0;i<n;i++)tp[i]=y[i]*y[i];	
	Y2=Average(tp,n);

    for(i=0;i<n;i++)tp[i]=x[i]*x[i];
	X2=Average(tp,n);
        
    for(i=0;i<n;i++)tp[i]=x[i]*y[i];
	Z1=Average(tp,n);

	for(i=0;i<n;i++)tp[i]=x[i]*x[i]*y[i];
	Z2=Average(tp,n);
    
    for(i=0;i<n;i++)tp[i]=x[i]*x[i]*x[i];
	X3=Average(tp,n);
    
	for(i=0;i<n;i++)tp[i]=y[i]*y[i]*y[i];
	Y3=Average(tp,n);

	for(i=0;i<n;i++)tp[i]=y[i]*y[i]*x[i];
	Z3=Average(tp,n);

	p=(double)2*(X1*X1-X2)*(Y1*Y1-Y2)-(double)2*(X1*Y1-Z1)*(X1*Y1-Z1);
	
	double a, b, r;
	a=((X2*X1+X1*Y2-X3-Z3)*(Y1*Y1-Y2)-(X2*Y1+Y1*Y2-Z2-Y3)*(X1*Y1-Z1))/p;
	
	b=((X2*Y1+Y1*Y2-Z2-Y3)*(X1*X1-X2)-(X2*X1+X1*Y2-X3-Z3)*(X1*Y1-Z1))/p;	
    r=sqrt(a*a-(double)2*X1*a+b*b-(double)2*Y1*b+X2+Y2);
	sCircle1->CirclePoint.x = (int)a;
	sCircle1->CirclePoint.y = (int)b;
	sCircle1->iRadius = (int)r;

	free(tp);
	return 0;
}


double CImageAlgorithm::Average(double A[],int n)
{
	int i;
	double tmp=0;
	for (i=0;i<n;i++)tmp+=A[i];
	tmp = tmp / (int)n;
	return tmp;
}

/******************************************************************************
�������� CollectPointAndFitCircle
��  ���� IplImage* src
         sCircle* sCircle1
��  �ܣ� �����������ĸ�����Զ�ֵ��ͼ�������ȡ�߽�㣬
         ����Щ��������С���˷���ϳ�Բ
*******************************************************************************/
int CImageAlgorithm::CollectPointAndFitCircle(IplImage* src, sCircle* sCircle1 )
{
    iRealCount = 0;
	CvPoint point[FITTINGPOINT];
	memset(point, 0, FITTINGPOINT * sizeof(CvPoint));
	memset(dPointX, 0, VALUE128 * sizeof(double));
	memset(dPointY, 0, VALUE128 * sizeof(double));
	
	GetFittingPointFromDir(src, 1, FITTINGPOINT, point);  //��������ֱ�Ϊ 1, 2, 3, 4  
	for(int i = 0; i < FITTINGPOINT; i++ )
	{
		if(point[i].x != 0 && point[i].y != 0)
		{
			dPointX[iRealCount] = point[i].x;
			dPointY[iRealCount++] = point[i].y;
		}
	}

	GetFittingPointFromDir(src, 2, FITTINGPOINT, point);
	for(int i = 0; i < FITTINGPOINT; i++ )
	{
		if(point[i].x != 0 && point[i].y != 0)
		{
			dPointX[iRealCount] = point[i].x;
			dPointY[iRealCount++] = point[i].y;
		}
	}

	GetFittingPointFromDir(src, 3, FITTINGPOINT, point);
	for(int i = 0; i < FITTINGPOINT; i++ )
	{
		if(point[i].x != 0 && point[i].y != 0)
		{
			dPointX[iRealCount] = point[i].x;
			dPointY[iRealCount++] = point[i].y;
		}
	}

	GetFittingPointFromDir(src, 4, FITTINGPOINT, point);
	for(int i = 0; i < FITTINGPOINT; i++ )
	{
		if(point[i].x != 0 && point[i].y != 0)
		{
			dPointX[iRealCount] = point[i].x;
			dPointY[iRealCount++] = point[i].y;
		}
	}
    LSMFitCircle( dPointX, dPointY, iRealCount - 1, sCircle1);

	return 0;
}

/********************************************************************************
�������� ChangePiexlValue
��  ���� IplImage* src
         int x[] 
		 int y[]
		 int iNum    �ɼ����ĵ���
��  �ܣ� ��ͼ����ָ�����޸�Ϊ��ɫ
*********************************************************************************/
int CImageAlgorithm::ChangePiexlValue(IplImage* src, int x[], int y[], int iNum)
{
	for(int i = 0; i < iNum; i++)
	{
		CV_IMAGE_ELEM(src, uchar, y[i], x[i]) = 255;
		//CV_IMAGE_ELEM(src, uchar, y[i], 3 * x[i] + 1 ) = 255;
		//CV_IMAGE_ELEM(src, uchar, y[i], 3 * x[i] + 2) = 0; 
	}
	
	return 0;
}

/********************************************************************************
�������� RemoveNoise
��  ���� sCircle* sCircle1
         double x[] 
		 double y[]
		 int iNum    �ɼ����ĵ���
��  �ܣ� ����ɼ��������굽Բ��ľ���,����ƽ��ֵ,С��ƽ��ֵ�ı��޳���.(�����ò�������Ӧ�޳�)
����ֵ�� ȥ�������Ч�������
*********************************************************************************/
int CImageAlgorithm::RemoveNoise(sCircle* sCircle1, double x[], double y[], int iNum)
{
	int iCount = 0;
	double dSamplePointToCircleCenterDistance[VALUE128];
	double dAverageValue = 0;
	for(int i = 0; i < iNum; i++)
	{
		dSamplePointToCircleCenterDistance[i] = sqrt( pow((double)(sCircle1->CirclePoint.x) - x[i], 2) + pow((double)sCircle1->CirclePoint.y - y[i], 2) );
		//TRACE("%f \n", dSamplePointToCircleCenterDistance[i]);
	}

	dAverageValue = Average(dSamplePointToCircleCenterDistance, iNum - 1);
	for(int i = 0; i < iNum; i++ )
	{
		if(dAverageValue < dSamplePointToCircleCenterDistance[i] )
		{
			x[iCount] = x[i];
			y[iCount] = y[i];
			iCount++;
		}
	}
	return iCount - 1;
}

/********************************************************************************
�������� CalcHist
��  ���� IplImage *src
         int hist[]     ֱ��ͼ
��  �ܣ� ����ͼ���ֱ��ͼ
********************************************************************************/
int CImageAlgorithm::CalcHist(IplImage *src, int hist[])
{
	int wide = src->widthStep;
	int high = src->height;
	//int hist[256] = {0};
	unsigned char *p = NULL, *q = NULL;  //CV_IMAGE_ELEM(src, uchar, j, iSelectedCloumn )
	for (int j = 0; j < high; j ++)
	{
		p = (unsigned char *)(src->imageData + j * wide);
		for (int i = 0; i < wide; i++)
		{
			hist[p[i]] += 1; //ͳ��ֱ��ͼ
		}
	}
	return 0;
}

/********************************************************************************
�������� ImageBinaryzation
��  ���� IplImage *src
         int iThresholdValue
         IplImage *dst
��  �ܣ� ͼ���ֵ��
********************************************************************************/
int CImageAlgorithm::ImageBinaryzation(IplImage *src, int iThresholdValue, IplImage *dst)
{
	int wide = src->widthStep;
	int high = src->height;
	int t = iThresholdValue;
	unsigned char *p, *q;  //CV_IMAGE_ELEM(src, uchar, j, iSelectedCloumn )
	for (int j = 0; j < high; j ++)
	{
		q = (unsigned char *)(dst->imageData + j * wide);
		p = (unsigned char *)(src->imageData + j * wide);
		for (int i = 0; i < wide; i++)
		{
			q[i] = p[i] >= t ? 0 : 255; //
		}
	}
	return 0;
}

/********************************************************************************
�������� GetOtsuThreshold
��  ���� int *hist   ����ͼ���ֱ��ͼ����
��  �ܣ� ʹ��Otsu�㷨,�����ֱ��ͼ��ֵ
����ֵ:  ���ط�ֵ
*********************************************************************************/
int CImageAlgorithm::GetOtsuThreshold(int *hist) //compute the threshold
{
	float u0 = 0, u1 = 0;
	float w0 = 0, w1 = 0;
	int count0 = 0;
	int t =0, maxT = 0;
	float devi = 0, maxDevi = 0; //�����󷽲�
	int i = 0;
	int sum = 0;
	for (i = 0; i < 256; i++)
	{
		sum = sum + hist[i];
	}
	for (t = 0; t < 255; t++)
	{
		u0 = 0;
		count0 = 0;
       //��ֵΪtʱ��c0��ľ�ֵ�������ĸ���
		for (i = 0; i <= t; i++)
		{
			u0 += i * hist[i];
			count0 += hist[i];
		}
		u0 = u0 / count0;
		w0 = (float)count0/sum;
        //��ֵΪtʱ��c1��ľ�ֵ�������ĸ���
		u1 = 0;
		for (i = t + 1; i < 256; i++)
		{
			u1 += i * hist[i];
		}
		u1 = u1 / (sum - count0);
		w1 = 1 - w0;
        //����䷽��
		devi = w0 * w1 * (u1 - u0) * (u1 - u0);
        //��¼���ķ�����λ��
		if (devi > maxDevi)
		{
			maxDevi = devi;
			maxT = t;
		}
	}
	return maxT;
}

/********************************************************************************
�������� OTSU
��  ���� IplImage *src
         IplImage *dst
��  �ܣ� ʹ��Otsu�㷨,�����ֱ��ͼ��ֵ,Ȼ����ж�ֵ��
*********************************************************************************/
void CImageAlgorithm::Otsu(IplImage *src, IplImage *dst)
{
	int wide = src->widthStep;
	int high = src->height;
	int hist[256] = {0};
	int t;
	//unsigned char *p = NULL, *q = NULL;  //CV_IMAGE_ELEM(src, uchar, j, iSelectedCloumn )

	CalcHist(src, hist);
	
	t = GetOtsuThreshold(hist);

	ImageBinaryzation(src, t, dst);

	return ;
}

/********************************************************************************
������: GetMeanThreshold
��  ��: IplImage *src
        IplImage *dst
��  �ܣ�ʹ�ûҶ�ƽ��ֵ����ֵ��ͼ��
**********************************************************************************/
int CImageAlgorithm::MeanThresholdBinaryzation(IplImage *src, IplImage *dst)
{
	 int wide = src->widthStep;
	int high = src->height;
	int hist[256] = {0};
	int t;
	//unsigned char *p, *q;  //CV_IMAGE_ELEM(src, uchar, j, iSelectedCloumn )

	CalcHist(src, hist);

	t = GetMeanThreshold(hist);

	ImageBinaryzation(src, t, dst);
	return 0;
}

/********************************************************************************
������: GetMeanThreshold
��  ��: int hist[]
��  �ܣ�������ͼ��ĻҶ�ƽ��ֵ��Ϊ��ֵ������ֵ
**********************************************************************************/
int CImageAlgorithm::GetMeanThreshold(int hist[])
{
	int Sum = 0, Amount = 0;
    for (int Y = 0; Y < 256; Y++)
    {
        Amount += hist[Y];
        Sum += Y * hist[Y];
    }
    return Sum / Amount;
}


/*********************************************************************************
������: BoundaryImpurityDetection
��  ��: IplImage* src
        IplImage* dst
        sCircle* sCircleParam
        int iRingPos
		int iRingLength = 10    Ĭ��Ϊ10
��  ��: ��Բ����߽�,����Ƿ���ڿ���(�ߵ�)
**********************************************************************************/
int CImageAlgorithm::BoundaryImpurityDetection(IplImage *src, sCircle* sCircleParam, int iRingLength)
{
	int iRadiusLength = sCircleParam->iRadius; 
	//���г�Բ��
	IplImage* RingPic = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	//cvFlip(src);      //ͼ��������������ϵ�����½ǣ�����ʾ����ϵ�����Ͻ�
	CutRingInCircle(src, RingPic, sCircleParam, iRadiusLength, iRingLength);
	//cvSaveImage("BoundaryRing.bmp", RingPic); debug

	 //SimpleBlobDetector
	cv::SimpleBlobDetector::Params params;    
	
	//change thresholds
	params.minThreshold = 15;    
	params.maxThreshold = 80;    
	params.thresholdStep = 5;    
	
	//filter by circularity
	params.minArea = 5; 
	params.maxArea = 20; 

	params.minConvexity = .04f;    
	params.minInertiaRatio = .05f;    

  
	cv::SimpleBlobDetector detector(params);  
	detector.create("SimpleBlob");
	//std::vector<cv::KeyPoint> key_points(10);   
	keypoints.clear();
	detector.detect(RingPic, keypoints);
	int ret = keypoints.size();

	//������δ�����ʾ�ҵ��İߵ㣬����debug
	/*cv::Mat output_img;  
	cv::drawKeypoints( RingPic, keypoints, output_img, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );  
    cv::namedWindow("SimpleBlobDetector");  
    cv::imshow("SimpleBlobDetector", output_img);  
    cv::waitKey(0);  
	*/
	return ret;
}

/*********************************************************************************
������: SimpleBlobDetectorInRing ���� 
��  ��: IplImage* RingPic
��  ��: ʹ��opencv�Դ��������ߵ�
**********************************************************************************/
int CImageAlgorithm::SimpleBlobDetectorInRing(IplImage* RingPic)
{
    //SimpleBlobDetector
	cv::SimpleBlobDetector::Params params;    
	
	//change thresholds
	params.minThreshold = 15;    
	params.maxThreshold = 80;    
	params.thresholdStep = 5;    
	
	//filter by circularity
	params.minArea = 5; 
	params.maxArea = 30; 

	params.minConvexity = .05f;    
	params.minInertiaRatio = .05f;    

  
	cv::SimpleBlobDetector detector(params);  
	detector.create("SimpleBlob");
	cv::vector<cv::KeyPoint> key_points(10);   

	detector.detect(RingPic,key_points);
	
	int iBlob = key_points.size();
	if(iBlob != 0)
	{
	}

	//������δ�����ʾ�ҵ��İߵ㣬����debug
	/*cv::Mat output_img;  
	cv::drawKeypoints( RingPic, key_points, output_img, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );  
    cv::namedWindow("SimpleBlobDetector");  
    cv::imshow("SimpleBlobDetector", output_img);  
    cv::waitKey(0);  
*/
	return iBlob;
}

/*********************************************************************************
������: GeneralMeasuer
��  ��: IplImage* RingPic
��  ��: ʹ��opencv�Դ��������ߵ�
**********************************************************************************/
int CImageAlgorithm::GeneralMeasuer(IplImage* RingPic)
{
	cvSmooth(RingPic,RingPic,CV_GAUSSIAN,3,3);
	cvErode(RingPic, RingPic, 0, 1);
	cvCanny(RingPic, RingPic, 15, 80, 3);

	IplConvKernel * myModel;
    myModel=cvCreateStructuringElementEx( //�Զ���5*5,�ο��㣨3,3���ľ���ģ��
        3,3,1,1,CV_SHAPE_ELLIPSE );

    cvDilate(RingPic, RingPic, myModel, 1);
	cvShowImage( "General Measure",RingPic);
	return 0;
}


/*********************************************************************************
������: CutRingInCircle
��  ��: IplImage* src
        IplImage* dst
        sCircle* sCircleParam
        int iRingPos
		int iRingLength = 10    Ĭ��Ϊ10
��  ��: ͼ����Բ��ָ���뾶����Բ�ķ����г�ָ����ȵ�Բ�������������dst��
**********************************************************************************/
int CImageAlgorithm::CutRingInCircle(IplImage* src, IplImage* dst, sCircle* sCircleParam, int iRingPos, int iRingLength)
{
	assert(src);
	if(iRingPos < iRingLength)
	{
		return -1;
	}
	if(iRingPos > sCircleParam->iRadius)
	{
		return -1;
	}

	for(int i = 0; i < src->height; i++ )
	{
		for(int j = 0; j < src->width; j++)
		{
			int ret = IsInDesignatedArea(j, i, sCircleParam, (double)iRingPos, (double)iRingLength);
			if( 0 == ret)
			{
				ELEM(dst, i, j) = ELEM(src, i, j);
			}
			else
			{
				ELEM(dst, i, j) = 255;
			}
		}
	}
	return 0;
}


/*********************************************************************************
������: IsInDesignatedArea
��  ��: double x
		double y
		sCircle* sCircleParam
		double iRingPos
		double iRingLength
��  ��: �ж�������Ƿ���Բ����
**********************************************************************************/
inline int CImageAlgorithm::IsInDesignatedArea(double x, double y, sCircle* sCircleParam, double iRingPos, double iRingLength)
{

	double dPintY = (double)sCircleParam->CirclePoint.y;
	double dPintX = (double)sCircleParam->CirclePoint.x;
	double dCircleRadius = (double)sCircleParam->iRadius;

	if(pow( dPintY - y, 2) + pow( dPintX - x, 2) <= pow( iRingPos, 2 ))
	{
		if(pow( dPintY - y, 2) + pow( dPintX - x, 2) >= pow( iRingPos - iRingLength, 2 ) )
		{
			return 0;
		}
	}
	return -1;
}

/*********************************************************************************
������: QuarterCicle
��  ��: IplImage* src
        IplImage** dst
        sCircle* sCircleParam
��  ��: ��ͼ���е�Բ�ĵȷ֣�������**dst��
**********************************************************************************/
int CImageAlgorithm::QuarterCicle(IplImage* src, IplImage** dst, sCircle* sCircleParam)
{
	int iHeight = sCircleParam->CirclePoint.y;
	int iWidth = sCircleParam->CirclePoint.x;
	int iLength = sCircleParam->iRadius;
	int dstindex = 0;
	cvFlip(src);

	for(int i = 0; i < src->height; i++ )
	{
		for(int j = 0; j < src->width; j++)
		{
			if(i < iHeight)
			{
				if(j < iWidth)
				{
					dstindex = 0;
				}
				else
				{
					dstindex = 1;
				}
			}
			else
			{
				if(j < iWidth)
				{
					dstindex = 2;
				}
				else
				{
					dstindex = 3;
				}
			}

			if(!IsInDesignatedArea(j, i, sCircleParam, iLength, iLength))
			{
				ELEM( *(dst+dstindex), i, j) = ELEM(src, i, j);
			}
			else
			{
				ELEM( *(dst+dstindex), i, j) = 0;
			}
		}
	}

	return 0;
}

/*********************************************************************************
������: HoughCircles
��  ��: IplImage* src
        IplImage** dst
��  ��: ���srcͼ���е�Բ�������������ʱ����dst�в��ð��߱�ǳ���Բ
**********************************************************************************/
int CImageAlgorithm::HoughCircles(IplImage* src)
{
	IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	CvMemStorage* storage = cvCreateMemStorage(0);  
	//cvSmooth( src, dst, CV_GAUSSIAN, 5, 5 );  //����  
	CvSeq* results = cvHoughCircles(  //cvHoughCircles������Ҫ����ÿһ�������ݶȵķ���  
											  //��˻����ڲ��Զ�����cvSobel,����ֵ��Եͼ��Ĵ����ǱȽ��ѵ�  
				src,  
				storage,  
				CV_HOUGH_GRADIENT,  
				1,  //�ۼ���ͼ��ķֱ���  
				5  
				);  
	for( int i = 0; i < results->total; i++ )  
	{  
		float* p = ( float* )cvGetSeqElem( results, i );  //����Բ�任  
		CvPoint pt = cvPoint( cvRound( p[0] ), cvRound( p[1] ) );  
		cvCircle(  src,  
				   pt,  //ȷ��Բ��  
				   cvRound( p[2] ),  //ȷ���뾶  
				   CV_RGB( 0xff, 0xff, 0xff )  );  //��Բ����  
     }  
	return 0;
}

