#include <stdio.h>
#include <math.h>
#include <io.h>
#include "cv.h"
#include "highgui.h"
#include <vector>
#include <cmath>
#include "opencv2/core/core.hpp"   
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/features2d/features2d.hpp"  
#include <opencv2/highgui/highgui.hpp>
//#include "opencv2/nonfree/nonfree.hpp"

#define ELEM(img, r, c) (CV_IMAGE_ELEM(img, uchar, r, c)) 

#define VALUE512 512
#define VALUE128 128
#define NARROWSIZE 4 //��ͼ��СN������������ٶ�
#define SOBELTHRESHOLD 0.0493
#define SimilarThreshold 0.98
#define FITTINGPOINT 20
#define NOISEDISTANCE 30 //��һ�����Բ�ĳ���������������Ϊ���ŵ�
#define CONTOUR_THRESHOLD_AREA 2 //��ͨ����������С��ֵ���������ֵ��ʾû�м�⵽����

using namespace cv;
using namespace std;
typedef struct CIRCLE
{
	CvPoint CirclePoint;
	int iRadius;

} 
sCircle;


class CImageAlgorithm
{
public:
	CImageAlgorithm();
	~CImageAlgorithm();

	int DisplayOpenCVInfo(void);

	int ShowDebugPhoto(IplImage* img);
	int PaintCircle(IplImage* img, sCircle*  sCircle1);

	int NormalizedShadowDivDCComponet(double* dSrcValue, double* dDstValue );

	int CalcColumnShadow(IplImage* image, double* dValue, int iShadowAxisValue);

	int GetNormShadowValue(IplImage* Image, double* dNormVerticalShadow, double* dNormHorizontalShadow);

	int MatchTemplateSimilarityValue(IplImage* ChkBallPic, IplImage* FuleBallPic, IplImage* EmptyBallPic );

	double Corrcoef(const double* x, const double* y, int iArraySize);
	double Corrcoef(const int* x, const int* y, int iArraySize);

	int EdgeAndThreshold(IplImage* src, IplImage* dst);

	int FindCircleRadius(IplImage* src, sCircle* sCircle1);

	int LableConnectedDomainInSpecifiedZone( IplImage* src, sCircle*  sCircle1 );

    int FindPointsFromUpToDown(IplImage* src, int iNum, CvPoint point[]);

	int FindPointsFromRightToLeft(IplImage* src, int iNum, CvPoint point[]);

	int FindPointsFromDownToUp(IplImage* src, int iNum, CvPoint point[]);

	int FindPointsFromLeftToRight(IplImage* src, int iNum, CvPoint point[]);

	int GetFittingPointFromDir(IplImage* src, int iDirection, int iNum, CvPoint point[]);

	int LSMFitCircle(double x[], double y[], int n, sCircle* sCircle1);

	int CollectPointAndFitCircle(IplImage* src, sCircle* sCircle1);

	double CImageAlgorithm::Average(double A[],int n);

	int ChangePiexlValue(IplImage* src, int x[], int y[], int iNum);

	int RemoveNoise(sCircle* sCircle1, double x[], double y[], int iNum);

	int CalcHist(IplImage *src, int hist[]);

	int ImageBinaryzation(IplImage *src, int iThresholdValue, IplImage *dst);

	void Otsu(IplImage *src, IplImage *dst);

	int GetOtsuThreshold(int *hist);

	int GetMeanThreshold(int hist[]);

	int MeanThresholdBinaryzation(IplImage *src, IplImage *dst);

	int BoundaryImpurityDetection(IplImage *src, sCircle* sCircleParam, int iRingLength);
	int SimpleBlobDetectorInRing(IplImage* RingPic);  //���ߵ�����ɷ���
	int GeneralMeasuer(IplImage* RingPic);

	int CutRingInCircle(IplImage* src, IplImage* dst, sCircle* sCircleParam, int iRingPos, int iRingLength );

	int QuarterCicle(IplImage* src, IplImage** dst, sCircle* sCircleParam);
	int HoughCircles(IplImage* src);
	inline int IsInDesignatedArea(double x, double y, sCircle* sCircleParam, double iRingPos, double iRingLength);



private:
	double dPointX[VALUE128];
	double dPointY[VALUE128];
	int iRealCount; //��¼dPointX/Y�洢ʵ�ʸ���

public:
   vector<KeyPoint> keypoints;
   vector<KeyPoint> GetKeypoints()const { return keypoints; };
};