
#include "HcpAction.h"
#include "Raw.h"

//#pragma pack(push,8)

// 像素格式
typedef enum tagXRayPixelFormat
{
	eXRayUnknown	= -1,				// 未知
	eXRayGrey8	    = 0,				// 8位灰度图像
	eXRayRGB24	    = 1,				// 24位彩色图像
	eXRayRGBA32     = 2,				// 32位彩色图像
	eXRayRGBA16     = 3                 // 16位彩色图像
} XRayPixelFormat;
// 采集是否成功的状态标志
typedef enum tagXRayStatus
{
	eXRayGrabOK		    = 0,			// 采集成功
	eXRayGrabFailed	    = 1,			// 采集失败
	eXRayGrabTimeout	= 2,			// 采集超时
} XRayStatus;

// 采集线程工作模式
typedef enum tagHCPWORKMODE
{
	eXRayIdle = -1,					// 空闲
	eXRaySnap = 0,					// 单帧
	eXRayGrab = 1,				   // 连续
} HCPWorkMode;

// 回调函数类型声明，当采集完成后调用该回调函数；
// 参数说明：
//		pOwner:		主调用对象的指针，与SetOwner的输入参数pOwner一致；
//		status:		采集状态枚举：成功、出错或超时
//		pImage:		图像数据指针
//		lWidth:		图像的宽
//		lHeight:	图像的高
//		pixelFormat:图像的像素格式
// 注：
//		如果采集图像成功，则pImage、lWidth、lHeight和pixelFormat为图像相关的有效数据；
//		如果采集图像出错，则pImage、lWidth、lHeight和pixelFormat为无效值，一般为0；
//		如果采集图像超时，则pImage、lWidth、lHeight和pixelFormat为无效值，一般为0；

typedef void (WINAPI *XRayGrabbedCallback)(void* pOwner, XRayStatus status, unsigned char* pImage, long lWidth, long lHeight, XRayPixelFormat pixelFormat);


class CGrabImage
{
public:
	CGrabImage(void);
	~CGrabImage(void);

	public:	
	////////////////////////////////////设置回调函数////////////////////////////////
	void SetOwner(void* pOwner, XRayGrabbedCallback funcCallback);							// 设置调用者和回调函数
	void SetSendCallbackEnable(bool bEnable);												// 设置是否调用回调函数
	bool GetSendCallbackEnable();	

	////////////////////////////////////图像采集设备函数////////////////////////////
	BOOL InitHCPFromIndex(sHcpParam* sHcpParamter);                                         //根据模式初始化HCP
	BOOL InitHCP(sHcpParam* sHcpParamter);                                                 //初始化HCP
    BOOL CreateGrabThread();																// 创建采集线程，一个采集对象只能有一个采集线程	
	BOOL TerminateGrabThread();                                                            //终止采集线程
	BOOL Destroy();                                                                        //终止采集线程并释放内存
	static unsigned int GrabThread(LPVOID lpParam);	
	XRayStatus AcquireImage();
	int TransfRawToByte();                            
	//XRayStatus AcquireImage();
	void GrabbedImageSendCallbackToDisplay(XRayStatus status);                             // 获取到图片回调函数在界面进行图片显示  
	bool SnapImage();                                                                      //单帧采集
	bool IsAcquireSuccess();                                                               //是否采集成功
	bool Freeze();																		   // 结束连续采集，需要已经创建采集线程

	bool Grab();                                                                           //连续采集
	bool IsGrabbing();                                                                     //是否正在连续采集
	bool IsHCPOnline();                                                                    //相机是否连接


public:

	///////////////////////////////回调函数相关参数//////////////////////////////
	void*	             m_pMainFrm;														// 回调函数主窗口指针
	XRayGrabbedCallback	 m_funcCallback;													// 回调函数	
	bool				 m_bSendCallbackEnable;												// 是否允许回调	


	////////////////HCP参数//////////////////////////////////////////////
	cHcpAction           *CHcp;
	int                  result;
	int                  Frm;
	int                  Buf;
	int                  Mode;
	char                 FrameReadyName[MAX_STR];                                          //至少32个字节，保存的字符串与frame同步等作用
	WORD*                m_RawPtr;                                                         //在堆上开辟内存放置RAW数据
	bool				 m_bHCPOnline;														// HCP连接成功标志位
	bool				 m_bHCPThreadIsActive;												// HCP采集的线程已经待命，准备接受采集信号

	bool				 m_bTerminate;														// 退出实时显示标志
	///////////////线程采集参数///////////////////////////////////
private:
	HANDLE				 m_hEventSnapped;	
	HANDLE				 m_hEventToRun;														// 采集线程处于暂停时，发出消息开始采集事件句柄
	HANDLE				 m_hEventExit;			  											// 子线程通知主线程已结束事件句柄
	CWinThread*	         m_pWinThread;														// 连续采集线程
	HCPWorkMode          m_HCPWorkMode;  													// 采集线程所处的工作模式
	bool				 m_bHaveStopped;													// 是否已经退出实时采集显示
	bool	             m_bIsGrabbing;														// 是否正在连续采集图像



	////////////////图像采集过程中使用的变量//////////////////////////////////////////////
public:
	CRITICAL_SECTION	 m_criticalSection;													// 临界区，控制图像数据的读写	
	bool                 m_bAcquireSuceess;                                     
	unsigned char*       m_pGrabImage;														// 采集到的图像数据
	long	             m_lWidth;															// 图像的宽
	long	             m_lHeight;															// 图像的高
	XRayPixelFormat		 m_pixelFormat;														// 图像的像素格式				
	CRaw*                m_pRaw;                                                            //将高精度图像转化成能被识别的格式



};
//#pragma pack(pop)