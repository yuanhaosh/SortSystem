#include "StdAfx.h"
#include "ImageProcess.h"


CImageProcess::CImageProcess(void)
{
	//sImagePara* sImageParamter;
	m_iRingDeep = 30;

}

CImageProcess::CImageProcess(sImagePara* sImageParamter)
{
	m_iRingDeep = sImageParamter->RingDeep;
	memset(m_cFulePath, 0, sizeof(m_cFulePath));
	strncpy_s(m_cFulePath, sImageParamter->FulePath, strlen(sImageParamter->FulePath));

	memset(m_cEmptyPath, 0, sizeof(m_cEmptyPath));
	strncpy_s(m_cEmptyPath, sImageParamter->EmptyPath, strlen(sImageParamter->EmptyPath));
}

CImageProcess::~CImageProcess(void)
{
}

int CImageProcess::ReadTemplateImage()
{
	//char* strTemplateDirFuleBall =  ".\\TempletPicture\\FuleBall.jpg";  //参数化路径
   // char* strTemplateDirEmptyBall = ".\\TempletPicture\\EmptyBall.jpg";
	FuleBallPic = cvLoadImage( m_cFulePath, CV_LOAD_IMAGE_GRAYSCALE); assert(FuleBallPic);
	EmptyBallPic = cvLoadImage( m_cEmptyPath, CV_LOAD_IMAGE_GRAYSCALE); assert(EmptyBallPic);

	return 0;
}


//图像都要转成与模板等尺寸的大小
int CImageProcess::ReadChkImage(IplImage* SrcPic)
{
	if(SrcPic == NULL )return -1;
	
	//IplImage* SrcPic = cvLoadImage( ucChkFileName, CV_LOAD_IMAGE_GRAYSCALE ); assert(SrcPic);
	
	if( SrcPic->nSize == 0)
	{ 
		//m_listbox_sysinfo.AddString("图像为空，请检查。");
		return -1;
	}
	else if(SrcPic->width > 0  && SrcPic->height / SrcPic->width == 1)
	{
		if(SrcPic->height != 512)
		{
		    CvSize cvDstSize = cvSize (FuleBallPic->width, FuleBallPic->height);
	        ChkBallPic =  cvCreateImage(cvDstSize, IPL_DEPTH_8U, 1);
		    cvResize( SrcPic, ChkBallPic);
		}
		else
		{
			ChkBallPic = cvCloneImage(SrcPic);
		}
	}
	else
	{
		//m_listbox_sysinfo.AddString("图像比例不符合要求。");
		return -1;
	}
	return 0;
}

//
int CImageProcess::ClassifyImage()
{
	int ret = 0;
	sCircle sCircleParam;
	memset( &sCircleParam, 0, sizeof(sCircleParam) );

	ret = SimilarityDetection();
	if(ret == -1) {return -1;}
	else if(ret == 1)//燃料球
	{
		int iBlobNum = 0;
		LocateCircle(&sCircleParam);    //定位圆坐标

	    iBlobNum = BoundaryImpurityDetection(&sCircleParam);  //外层杂质检测
		if(iBlobNum == 0)
		{
			//QuarterCicleAndCalcSimilarity(&sCircleParam);  //后续增添 球内心颗粒分布是否均匀
			return 1; //1: 合格
		} 
		else
		{return 2;}//2：不合格
	}
	else
	{   //检测球或者白球
		LocateCircle(&sCircleParam);
		EdgeDetection();
		//GrayRingDetection(&sCircleParam);
		ret = FindMaxConnetionZoneInCircle(&sCircleParam); //3：检测球 4：白球
		return ret;
	}
	return 0;
}

int CImageProcess::SimilarityDetection()
{
	int ret = m_CImageAlgorithm.MatchTemplateSimilarityValue(ChkBallPic, FuleBallPic, EmptyBallPic); //与模板的相似度匹配
	return ret;
}

/**************************************************************************************
平均灰度法确定图像二值化阀值，抽样检测检测球，燃料球(合格与不合格)效果比较好

这个函数有两个方法 1. canny边缘检测 used
                   2. 常规的膨胀腐蚀sobel检测
***************************************************************************************/
int CImageProcess::LocateCircle(sCircle* sCircleParam)
{
		//利用平均灰度值法二值化
	IplImage* PreEdgePic = cvCreateImage(cvGetSize(ChkBallPic), IPL_DEPTH_8U, 1);
	m_CImageAlgorithm.MeanThresholdBinaryzation(ChkBallPic, PreEdgePic);

	//利用最小二乘法两次拟合圆 0.138997s 0.115137
	m_CImageAlgorithm.CollectPointAndFitCircle(PreEdgePic, sCircleParam);

	/*CvFont font; 
	cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX, 0.5, 0.5, 1, 2, 8);
	cvCircle(ChkBallPic, sCircleParam->CirclePoint, sCircleParam->iRadius, cvScalar(0, 0, 255));
	cvPutText(ChkBallPic, "LMS + 平均灰度值法", cvPoint(5,30),  &font, cvScalar(0, 0, 255));
	m_CImageAlgorithm.ShowDebugPhoto(ChkBallPic); */
	return 0;
}

int CImageProcess::EdgeDetection()
{
	EdgePic = cvCreateImage(cvGetSize(ChkBallPic), IPL_DEPTH_8U, 1);

	cvCanny(ChkBallPic, EdgePic, 50, 50 * 3, 3);  
	//cvCircle( EdgePic, (sCircleParam->CirclePoint), sCircleParam->iRadius, Scalar(100, 100, 100));  //用于察看canny边缘检测与拟合圆重合效果
	
	cvShowImage("canny", ChkBallPic);
	cv::waitKey(0);
	
	//m_CImageAlgorithm.EdgeAndThreshold(ChkBallPic, EdgePic);
	//cvErode(EdgePic, EdgePic);

	//TransformIplImageToBmp(ChkBallPic);             //
	//DrawImage(ChkBallPic->width, ChkBallPic->height); //显示到PictureBox控件中
	return 0;
}

int CImageProcess::BoundaryImpurityDetection(sCircle* sCircleParam)
{
	int iRingLength = m_iRingDeep;    //参数 圆环的宽度
    int iBlobNum = m_CImageAlgorithm.BoundaryImpurityDetection(ChkBallPic, sCircleParam, iRingLength);
	if(iBlobNum > 0)
	{
		keypoints.clear();
		keypoints = m_CImageAlgorithm.GetKeypoints();
	}
	return iBlobNum;
}


//返回值为3:检测球 返回值4: 白球
int CImageProcess::FindMaxConnetionZoneInCircle(sCircle* sCircleParam)
{
	//标记指定区域中的连通域，找出最大与阀值比较
	int result = m_CImageAlgorithm.LableConnectedDomainInSpecifiedZone(EdgePic, sCircleParam );
	if(result > 3)
	{ 
		return 3; //m_listbox_sysinfo.AddString(_TEXT("待测球为检测球。"));
	}
	else 	
	{
			return 4; //m_listbox_sysinfo.AddString(_TEXT("待测球为白球。"));
	}
}
