#pragma once
#include <list>
#include <array>
#include "WINDOWSX.H"
#include "highgui.h"
#include "cv.h"
#include "afxwin.h"
#include "ImageAlgorithm.h"
#include "opencv2/core/core.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
class CImageProcess
{
public:
	CImageProcess(void);
	CImageProcess(sImagePara* sImageParamter);
	~CImageProcess(void);

	int ReadTemplateImage();
	int ReadChkImage(IplImage* SrcPic);
	int ClassifyImage();
	int SimilarityDetection();
	int LocateCircle(sCircle* sCircleParam);
	int EdgeDetection();
	int BoundaryImpurityDetection(sCircle* sCircleParam);



	int FindMaxConnetionZoneInCircle(sCircle* sCircleParam);
	vector<KeyPoint> GetKeypoints()const { return keypoints; };


private:
	  IplImage* FuleBallPic;
	  IplImage* EmptyBallPic;
	  IplImage* PicTest;   //��ȡԭͼ
	  IplImage* ChkBallPic; //��ȡ�Ҷ�ͼ

	  IplImage* EdgePic;  
	  int m_iRingDeep;   //�߽����ʣ��߽���
	  char m_cFulePath[256];
	  char m_cEmptyPath[256];

	  vector<KeyPoint> keypoints;

	  CImageAlgorithm m_CImageAlgorithm;
	
};

