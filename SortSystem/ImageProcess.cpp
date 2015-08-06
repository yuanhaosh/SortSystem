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
	//char* strTemplateDirFuleBall =  ".\\TempletPicture\\FuleBall.jpg";  //������·��
   // char* strTemplateDirEmptyBall = ".\\TempletPicture\\EmptyBall.jpg";
	FuleBallPic = cvLoadImage( m_cFulePath, CV_LOAD_IMAGE_GRAYSCALE); assert(FuleBallPic);
	EmptyBallPic = cvLoadImage( m_cEmptyPath, CV_LOAD_IMAGE_GRAYSCALE); assert(EmptyBallPic);

	return 0;
}


//ͼ��Ҫת����ģ��ȳߴ�Ĵ�С
int CImageProcess::ReadChkImage(IplImage* SrcPic)
{
	if(SrcPic == NULL )return -1;
	
	//IplImage* SrcPic = cvLoadImage( ucChkFileName, CV_LOAD_IMAGE_GRAYSCALE ); assert(SrcPic);
	
	if( SrcPic->nSize == 0)
	{ 
		//m_listbox_sysinfo.AddString("ͼ��Ϊ�գ����顣");
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
		//m_listbox_sysinfo.AddString("ͼ�����������Ҫ��");
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
	else if(ret == 1)//ȼ����
	{
		int iBlobNum = 0;
		LocateCircle(&sCircleParam);    //��λԲ����

	    iBlobNum = BoundaryImpurityDetection(&sCircleParam);  //������ʼ��
		if(iBlobNum == 0)
		{
			//QuarterCicleAndCalcSimilarity(&sCircleParam);  //�������� �����Ŀ����ֲ��Ƿ����
			return 1; //1: �ϸ�
		} 
		else
		{return 2;}//2�����ϸ�
	}
	else
	{   //�������߰���
		LocateCircle(&sCircleParam);
		EdgeDetection();
		//GrayRingDetection(&sCircleParam);
		ret = FindMaxConnetionZoneInCircle(&sCircleParam); //3������� 4������
		return ret;
	}
	return 0;
}

int CImageProcess::SimilarityDetection()
{
	int ret = m_CImageAlgorithm.MatchTemplateSimilarityValue(ChkBallPic, FuleBallPic, EmptyBallPic); //��ģ������ƶ�ƥ��
	return ret;
}

/**************************************************************************************
ƽ���Ҷȷ�ȷ��ͼ���ֵ����ֵ�������������ȼ����(�ϸ��벻�ϸ�)Ч���ȽϺ�

����������������� 1. canny��Ե��� used
                   2. ��������͸�ʴsobel���
***************************************************************************************/
int CImageProcess::LocateCircle(sCircle* sCircleParam)
{
		//����ƽ���Ҷ�ֵ����ֵ��
	IplImage* PreEdgePic = cvCreateImage(cvGetSize(ChkBallPic), IPL_DEPTH_8U, 1);
	m_CImageAlgorithm.MeanThresholdBinaryzation(ChkBallPic, PreEdgePic);

	//������С���˷��������Բ 0.138997s 0.115137
	m_CImageAlgorithm.CollectPointAndFitCircle(PreEdgePic, sCircleParam);

	/*CvFont font; 
	cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX, 0.5, 0.5, 1, 2, 8);
	cvCircle(ChkBallPic, sCircleParam->CirclePoint, sCircleParam->iRadius, cvScalar(0, 0, 255));
	cvPutText(ChkBallPic, "LMS + ƽ���Ҷ�ֵ��", cvPoint(5,30),  &font, cvScalar(0, 0, 255));
	m_CImageAlgorithm.ShowDebugPhoto(ChkBallPic); */
	return 0;
}

int CImageProcess::EdgeDetection()
{
	EdgePic = cvCreateImage(cvGetSize(ChkBallPic), IPL_DEPTH_8U, 1);

	cvCanny(ChkBallPic, EdgePic, 50, 50 * 3, 3);  
	//cvCircle( EdgePic, (sCircleParam->CirclePoint), sCircleParam->iRadius, Scalar(100, 100, 100));  //���ڲ쿴canny��Ե��������Բ�غ�Ч��
	
	cvShowImage("canny", ChkBallPic);
	cv::waitKey(0);
	
	//m_CImageAlgorithm.EdgeAndThreshold(ChkBallPic, EdgePic);
	//cvErode(EdgePic, EdgePic);

	//TransformIplImageToBmp(ChkBallPic);             //
	//DrawImage(ChkBallPic->width, ChkBallPic->height); //��ʾ��PictureBox�ؼ���
	return 0;
}

int CImageProcess::BoundaryImpurityDetection(sCircle* sCircleParam)
{
	int iRingLength = m_iRingDeep;    //���� Բ���Ŀ��
    int iBlobNum = m_CImageAlgorithm.BoundaryImpurityDetection(ChkBallPic, sCircleParam, iRingLength);
	if(iBlobNum > 0)
	{
		keypoints.clear();
		keypoints = m_CImageAlgorithm.GetKeypoints();
	}
	return iBlobNum;
}


//����ֵΪ3:����� ����ֵ4: ����
int CImageProcess::FindMaxConnetionZoneInCircle(sCircle* sCircleParam)
{
	//���ָ�������е���ͨ���ҳ�����뷧ֵ�Ƚ�
	int result = m_CImageAlgorithm.LableConnectedDomainInSpecifiedZone(EdgePic, sCircleParam );
	if(result > 3)
	{ 
		return 3; //m_listbox_sysinfo.AddString(_TEXT("������Ϊ�����"));
	}
	else 	
	{
			return 4; //m_listbox_sysinfo.AddString(_TEXT("������Ϊ����"));
	}
}
