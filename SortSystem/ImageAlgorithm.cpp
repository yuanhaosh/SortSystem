/****************************************************************************
   CImageAlgorithm.cpp
   
   实现了图像匹配中的算法


   袁浩 2015/03/12


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
函数名: DisplayOpenCVInfo
参  数: void
功  能: 显示OpenCV安装信息及IPP信息
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
函数名: ShowDebugPhoto
参  数: IplImage* img
功  能: 将img图像显示出来
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
函数名: PaintCircle
参  数: IplImage* img
        sCircle*  sCircle1
功  能: 在图像中画一个黑色的圆
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
函数名: MatchTemplateSimilarityValue
参  数: IplImage* ChkBallPic
        IplImage* FuleBallPic
		IplImage* EmptyBallPic
功  能: 如果燃料球的相似度大于白球且大于相似度最低阀值SimilarThreshold则为燃料球。
        若燃料球大于白球相似率但小于最低阀值SimilarThreshold则提示匹配失败。
返回值: 1:  检测为燃料球
        0:  检测球或者白球
	   -1: 检测失败
*************************************************************************/
int CImageAlgorithm::MatchTemplateSimilarityValue(IplImage* ChkBallPic, IplImage* FuleBallPic, IplImage* EmptyBallPic )
{
    //待测球
    double dChkPicNormVerticalShadow[VALUE128];
	double dChkPicNormHorizontalShadow[VALUE128];
    GetNormShadowValue(ChkBallPic, dChkPicNormVerticalShadow, dChkPicNormHorizontalShadow);
	
	//燃料球
	double dFulePicNormVerticalShadow[VALUE128];
	double dFulePicNormHorizontalShadow[VALUE128];
    GetNormShadowValue(FuleBallPic, dFulePicNormVerticalShadow, dFulePicNormHorizontalShadow);
	
	//白球
	double dEmptyPicNormVerticalShadow[VALUE128];
	double dEmptyPicNormHorizontalShadow[VALUE128];
    GetNormShadowValue(EmptyBallPic, dEmptyPicNormVerticalShadow, dEmptyPicNormHorizontalShadow);
	
	//计算横纵相关系数
	double dVerticalCorrValue1, dHorizontalCorrValue1; //燃料球
	dVerticalCorrValue1 = Corrcoef(dChkPicNormVerticalShadow, dFulePicNormVerticalShadow, VALUE128);
	dHorizontalCorrValue1 = Corrcoef(dChkPicNormHorizontalShadow, dFulePicNormHorizontalShadow, VALUE128);

	double dVerticalCorrValue2, dHorizontalCorrValue2; //白球
	dVerticalCorrValue2 = Corrcoef(dChkPicNormVerticalShadow, dEmptyPicNormVerticalShadow, VALUE128);
	dHorizontalCorrValue2 = Corrcoef(dChkPicNormHorizontalShadow, dEmptyPicNormHorizontalShadow, VALUE128);
	printf("待测球与燃料球相似度:\n");
	printf("垂直相似度为:%f, 水平相似度为:%f\n", dVerticalCorrValue1, dHorizontalCorrValue1);

	//比较相关系数返回有效值
	double dCorrValue1, dCorrValue2;
	dCorrValue1 = dVerticalCorrValue1 + dHorizontalCorrValue1;
	dCorrValue2 = dVerticalCorrValue2 + dHorizontalCorrValue2;
	printf("待测球与白球相似度:\n");
	printf("垂直相似度为:%f, 水平相似度为:%f\n", dVerticalCorrValue2, dHorizontalCorrValue2);
	
	int ret = 0;
	if (dCorrValue1 > dCorrValue2 && dCorrValue1 > SimilarThreshold)
	{
		//printf("垂直相似度为:%f, 水平相似度为:%f\n", dVerticalCorrValue1, dHorizontalCorrValue1);
		ret = 1; //燃料球
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
函数名: GetNormShadowValue
参  数: IplImage* Image                源图像
		double* dNormVerticalShadow    投影垂直归一化数组
		double* dNormHorizontalShadow  投影水平归一化数组
功  能: 首先缩小图像的尺寸，提高运算速度
        然后计算横纵方向投影
		最后归一化横纵方向投影
备  注: 包含子函数 CalcColumnShadow, NormalizedShadowDivDCComponet
**********************************************************************/
int CImageAlgorithm::GetNormShadowValue(IplImage* Image, double* dNormVerticalShadow, double* dNormHorizontalShadow)
{
    int iWidth = 0;                //standard size
	int iHeight = 0;
	IplImage* cvTImage = 0;      //用于存储转置后的图像
	CvSize cvDstSize;
	double dImageVeriticalShadow[VALUE128];  //用于存储横纵投影值
	double dImageHorizontalShadow[VALUE128];

	assert(Image);
	iWidth = Image->width;   
	iHeight = Image->height;
	cvDstSize.height = iHeight / NARROWSIZE;
	cvDstSize.width = iWidth / NARROWSIZE;
    
	{
		//图像缩小4倍，提高运算效率，且与用原图效果差异不大 
		//tips：图像缩小四倍，这个函数针对每个函数，所以不想用放在main函数中
		IplImage* NarrowPic =  cvCreateImage(cvDstSize, IPL_DEPTH_8U, 1);
		cvResize( Image, NarrowPic);
	
		//缩小图纵向投影
		memset(dImageVeriticalShadow, 0 , VALUE128 * sizeof(double));
		CalcColumnShadow(NarrowPic, dImageVeriticalShadow, cvDstSize.height);

		//缩小图横向投影
		cvTImage = cvCloneImage(NarrowPic);
		cvT(NarrowPic, cvTImage);
		memset(dImageHorizontalShadow, 0, VALUE128 * sizeof(double));
		CalcColumnShadow(cvTImage, dImageHorizontalShadow, cvDstSize.width);

		//投影去除直流分量，归一化
		memset(dNormVerticalShadow, 0, VALUE128 * sizeof(double));
		NormalizedShadowDivDCComponet(dImageVeriticalShadow, dNormVerticalShadow);

		memset(dNormHorizontalShadow, 0, VALUE128 * sizeof(double));
		NormalizedShadowDivDCComponet(dImageHorizontalShadow,  dNormHorizontalShadow);
	}
	return 0;
}

/*********************************************************************\
函数名：CalcColumnShadow
参数：  IplImage*      image
        unsigned int*  uiValue
		int            iShadowAxisValue
功能：   计算图像每列和，then除以列/行数求投影

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
函数名：NormalizedShadowDivDCComponet
参数:   double* dSrcValue
        double* dDstValue
功能：  图像投影除以直流分量，计算归一化投影
**************************************************************************/
int CImageAlgorithm::NormalizedShadowDivDCComponet(double* dSrcValue, double* dDstValue )
{
	double dSumValue = 0;
	double dTempAverageValue = 0;
	double dMaxValue = 0;
	
	//求投影数组的和，并找出最大值
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
函数名: Corrcoef
参  数: double* x
        double* y
		int ArraySize
功  能: 计算数组x,y两个数组的相关系数,并返回该值
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
函数名: Corrcoef
参  数: double* x
        double* y
		int ArraySize
功  能: 计算数组x,y两个数组的相关系数,并返回该值
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
函数名: EdgeAndThreshold
参  数：IplImage* src
        IplImage* dst
功  能: 使用边缘检测对灰度图像进行检测,并且使用阀值得到二值化图像
****************************************************************************/
int CImageAlgorithm::EdgeAndThreshold(IplImage* src, IplImage* dst)
{
	IplImage* soble16S = cvCreateImage(cvGetSize(src), IPL_DEPTH_16S, 1);

	 //预处理在后
	cvSobel(src, soble16S, 1, 1, 1); 
	cvConvertScaleAbs(soble16S, dst, 1, 0 );
	cvThreshold(dst, dst, (SOBELTHRESHOLD * 255), 255, CV_THRESH_BINARY);
	cvSmooth( dst, dst, CV_MEDIAN ); //去除椒盐噪声，使用中值滤波
	cvSmooth(dst,dst,CV_GAUSSIAN,7,7);
	cvThreshold(dst, dst, (SOBELTHRESHOLD * 255), 255, CV_THRESH_BINARY);

	/* 预处理在前
	cvSmooth( src, dst, CV_MEDIAN ); //去除椒盐噪声，使用中值滤波
	cvSmooth(dst,dst,CV_GAUSSIAN);

	cvSobel(dst, soble16S, 1, 1, 1); 
 	cvConvertScaleAbs(soble16S, dst, 1, 0 );
	cvThreshold(dst, dst, (SOBELTHRESHOLD * 255), 255, CV_THRESH_BINARY);

	ShowDebugPhoto(dst);
	*/
	return 0;
}

/***************************************************************************
函数名: FindCircleRadius
参  数: IplImage* src
        sCircle* sCircle1
功  能：利用灰度图像在X,Y轴上的投影计算，圆的半径和圆心  
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
函数名: LableConnectedDomainInSpecifiedZone
参  数: IplImage* src
        sCircle*  sCircle1 //指定区域
功  能: 找出指定区域中的连通域
****************************************************************************/
int CImageAlgorithm::LableConnectedDomainInSpecifiedZone( IplImage* src, sCircle*  sCircle1 )
{
	assert(src);
	double dPintY = (double)sCircle1->CirclePoint.y;
	double dPintX = (double)sCircle1->CirclePoint.x;
	double dCircleRadius = (double)sCircle1->iRadius;
	IplImage* img = cvCloneImage(src);
	//将指定区域划分出来 
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
	//标记连通域
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
	//找出最大连通域
	return maxArea;

}

/***************************************************************************
函数名: FindPointsFromUpToDown  
参  数: IplImage* src     
        int iNum  采样点数
		CvPoint point[] 用于存储采集的拟合点
功  能: 从图像的上方向下寻找非零点  1
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
函数名: FindPointsFromRightToLeft
参  数: IplImage* src     
        int iNum  采样点数
		CvPoint point[] 用于存储采集的拟合点
功  能: 从图像的右方向左寻找非零点  2
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
函数名: FindPointsFromDownToUp
参  数: IplImage* src     
        int iNum  采样点数
		CvPoint point[] 用于存储采集的拟合点
功  能: 从图像的下方向上寻找非零点  3
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
函数名: FindPointsFromLeftToRight
参  数: IplImage* src     
        int iNum  采样点数
		CvPoint point[] 用于存储采集的拟合点
功  能: 从图像的右方向左寻找非零点  4
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
函数名: GetFittingPoint
参  数: IplImage* src
        int iDirection    用来标出采样方向,上右下左分别为 1, 2, 3, 4        
        int iNum          采样点数
		CvPoint point[]   用于存储采集的拟合点
功  能: 用来采集指定图像方向的拟合点
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
函数名： LSMFitCircle
参  数： double x
         double y 
		 int n    采集到的点数
         sCircle* sCircle1
功  能： 利用最小二乘法拟合圆
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
函数名： CollectPointAndFitCircle
参  数： IplImage* src
         sCircle* sCircle1
功  能： 从上下左右四个方向对二值化图像采样获取边界点，
         将这些点利用最小二乘法拟合成圆
*******************************************************************************/
int CImageAlgorithm::CollectPointAndFitCircle(IplImage* src, sCircle* sCircle1 )
{
    iRealCount = 0;
	CvPoint point[FITTINGPOINT];
	memset(point, 0, FITTINGPOINT * sizeof(CvPoint));
	memset(dPointX, 0, VALUE128 * sizeof(double));
	memset(dPointY, 0, VALUE128 * sizeof(double));
	
	GetFittingPointFromDir(src, 1, FITTINGPOINT, point);  //上右下左分别为 1, 2, 3, 4  
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
函数名： ChangePiexlValue
参  数： IplImage* src
         int x[] 
		 int y[]
		 int iNum    采集到的点数
功  能： 将图像中指定点修改为黑色
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
函数名： RemoveNoise
参  数： sCircle* sCircle1
         double x[] 
		 double y[]
		 int iNum    采集到的点数
功  能： 计算采集到的坐标到圆点的距离,计算平均值,小于平均值的被剔出掉.(后续得补充自适应剔出)
返回值： 去除后的有效坐标个数
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
函数名： CalcHist
参  数： IplImage *src
         int hist[]     直方图
功  能： 计算图像的直方图
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
			hist[p[i]] += 1; //统计直方图
		}
	}
	return 0;
}

/********************************************************************************
函数名： ImageBinaryzation
参  数： IplImage *src
         int iThresholdValue
         IplImage *dst
功  能： 图像二值化
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
函数名： GetOtsuThreshold
参  数： int *hist   传入图像的直方图数据
功  能： 使用Otsu算法,计算出直方图阀值
返回值:  返回阀值
*********************************************************************************/
int CImageAlgorithm::GetOtsuThreshold(int *hist) //compute the threshold
{
	float u0 = 0, u1 = 0;
	float w0 = 0, w1 = 0;
	int count0 = 0;
	int t =0, maxT = 0;
	float devi = 0, maxDevi = 0; //方差及最大方差
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
       //阈值为t时，c0组的均值及产生的概率
		for (i = 0; i <= t; i++)
		{
			u0 += i * hist[i];
			count0 += hist[i];
		}
		u0 = u0 / count0;
		w0 = (float)count0/sum;
        //阈值为t时，c1组的均值及产生的概率
		u1 = 0;
		for (i = t + 1; i < 256; i++)
		{
			u1 += i * hist[i];
		}
		u1 = u1 / (sum - count0);
		w1 = 1 - w0;
        //两类间方差
		devi = w0 * w1 * (u1 - u0) * (u1 - u0);
        //记录最大的方差及最佳位置
		if (devi > maxDevi)
		{
			maxDevi = devi;
			maxT = t;
		}
	}
	return maxT;
}

/********************************************************************************
函数名： OTSU
参  数： IplImage *src
         IplImage *dst
功  能： 使用Otsu算法,计算出直方图阀值,然后进行二值化
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
函数名: GetMeanThreshold
参  数: IplImage *src
        IplImage *dst
功  能：使用灰度平均值，二值化图像
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
函数名: GetMeanThreshold
参  数: int hist[]
功  能：用整幅图像的灰度平均值作为二值化的阈值
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
函数名: BoundaryImpurityDetection
参  数: IplImage* src
        IplImage* dst
        sCircle* sCircleParam
        int iRingPos
		int iRingLength = 10    默认为10
功  能: 在圆的外边界,检测是否存在颗粒(斑点)
**********************************************************************************/
int CImageAlgorithm::BoundaryImpurityDetection(IplImage *src, sCircle* sCircleParam, int iRingLength)
{
	int iRadiusLength = sCircleParam->iRadius; 
	//先切除圆环
	IplImage* RingPic = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	//cvFlip(src);      //图像数据排列坐标系是左下角，而显示坐标系是左上角
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

	//下面这段代码显示找到的斑点，用于debug
	/*cv::Mat output_img;  
	cv::drawKeypoints( RingPic, keypoints, output_img, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );  
    cv::namedWindow("SimpleBlobDetector");  
    cv::imshow("SimpleBlobDetector", output_img);  
    cv::waitKey(0);  
	*/
	return ret;
}

/*********************************************************************************
函数名: SimpleBlobDetectorInRing 作废 
参  数: IplImage* RingPic
功  能: 使用opencv自带方法检测斑点
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

	//下面这段代码显示找到的斑点，用于debug
	/*cv::Mat output_img;  
	cv::drawKeypoints( RingPic, key_points, output_img, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );  
    cv::namedWindow("SimpleBlobDetector");  
    cv::imshow("SimpleBlobDetector", output_img);  
    cv::waitKey(0);  
*/
	return iBlob;
}

/*********************************************************************************
函数名: GeneralMeasuer
参  数: IplImage* RingPic
功  能: 使用opencv自带方法检测斑点
**********************************************************************************/
int CImageAlgorithm::GeneralMeasuer(IplImage* RingPic)
{
	cvSmooth(RingPic,RingPic,CV_GAUSSIAN,3,3);
	cvErode(RingPic, RingPic, 0, 1);
	cvCanny(RingPic, RingPic, 15, 80, 3);

	IplConvKernel * myModel;
    myModel=cvCreateStructuringElementEx( //自定义5*5,参考点（3,3）的矩形模板
        3,3,1,1,CV_SHAPE_ELLIPSE );

    cvDilate(RingPic, RingPic, myModel, 1);
	cvShowImage( "General Measure",RingPic);
	return 0;
}


/*********************************************************************************
函数名: CutRingInCircle
参  数: IplImage* src
        IplImage* dst
        sCircle* sCircleParam
        int iRingPos
		int iRingLength = 10    默认为10
功  能: 图像中圆在指定半径处向圆心方向切出指定宽度的圆环，结果保存在dst中
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
函数名: IsInDesignatedArea
参  数: double x
		double y
		sCircle* sCircleParam
		double iRingPos
		double iRingLength
功  能: 判断坐标点是否则圆环中
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
函数名: QuarterCicle
参  数: IplImage* src
        IplImage** dst
        sCircle* sCircleParam
功  能: 将图像中的圆四等分，保存在**dst中
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
函数名: HoughCircles
参  数: IplImage* src
        IplImage** dst
功  能: 检测src图像中的圆，结果保存在临时变量dst中并用白线标记出来圆
**********************************************************************************/
int CImageAlgorithm::HoughCircles(IplImage* src)
{
	IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	CvMemStorage* storage = cvCreateMemStorage(0);  
	//cvSmooth( src, dst, CV_GAUSSIAN, 5, 5 );  //降噪  
	CvSeq* results = cvHoughCircles(  //cvHoughCircles函数需要估计每一个像素梯度的方向，  
											  //因此会在内部自动调用cvSobel,而二值边缘图像的处理是比较难的  
				src,  
				storage,  
				CV_HOUGH_GRADIENT,  
				1,  //累加器图像的分辨率  
				5  
				);  
	for( int i = 0; i < results->total; i++ )  
	{  
		float* p = ( float* )cvGetSeqElem( results, i );  //霍夫圆变换  
		CvPoint pt = cvPoint( cvRound( p[0] ), cvRound( p[1] ) );  
		cvCircle(  src,  
				   pt,  //确定圆心  
				   cvRound( p[2] ),  //确定半径  
				   CV_RGB( 0xff, 0xff, 0xff )  );  //画圆函数  
     }  
	return 0;
}

