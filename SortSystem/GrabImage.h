
#include "HcpAction.h"
#include "Raw.h"

//#pragma pack(push,8)

// ���ظ�ʽ
typedef enum tagXRayPixelFormat
{
	eXRayUnknown	= -1,				// δ֪
	eXRayGrey8	    = 0,				// 8λ�Ҷ�ͼ��
	eXRayRGB24	    = 1,				// 24λ��ɫͼ��
	eXRayRGBA32     = 2,				// 32λ��ɫͼ��
	eXRayRGBA16     = 3                 // 16λ��ɫͼ��
} XRayPixelFormat;
// �ɼ��Ƿ�ɹ���״̬��־
typedef enum tagXRayStatus
{
	eXRayGrabOK		    = 0,			// �ɼ��ɹ�
	eXRayGrabFailed	    = 1,			// �ɼ�ʧ��
	eXRayGrabTimeout	= 2,			// �ɼ���ʱ
} XRayStatus;

// �ɼ��̹߳���ģʽ
typedef enum tagHCPWORKMODE
{
	eXRayIdle = -1,					// ����
	eXRaySnap = 0,					// ��֡
	eXRayGrab = 1,				   // ����
} HCPWorkMode;

// �ص������������������ɼ���ɺ���øûص�������
// ����˵����
//		pOwner:		�����ö����ָ�룬��SetOwner���������pOwnerһ�£�
//		status:		�ɼ�״̬ö�٣��ɹ��������ʱ
//		pImage:		ͼ������ָ��
//		lWidth:		ͼ��Ŀ�
//		lHeight:	ͼ��ĸ�
//		pixelFormat:ͼ������ظ�ʽ
// ע��
//		����ɼ�ͼ��ɹ�����pImage��lWidth��lHeight��pixelFormatΪͼ����ص���Ч���ݣ�
//		����ɼ�ͼ�������pImage��lWidth��lHeight��pixelFormatΪ��Чֵ��һ��Ϊ0��
//		����ɼ�ͼ��ʱ����pImage��lWidth��lHeight��pixelFormatΪ��Чֵ��һ��Ϊ0��

typedef void (WINAPI *XRayGrabbedCallback)(void* pOwner, XRayStatus status, unsigned char* pImage, long lWidth, long lHeight, XRayPixelFormat pixelFormat);


class CGrabImage
{
public:
	CGrabImage(void);
	~CGrabImage(void);

	public:	
	////////////////////////////////////���ûص�����////////////////////////////////
	void SetOwner(void* pOwner, XRayGrabbedCallback funcCallback);							// ���õ����ߺͻص�����
	void SetSendCallbackEnable(bool bEnable);												// �����Ƿ���ûص�����
	bool GetSendCallbackEnable();	

	////////////////////////////////////ͼ��ɼ��豸����////////////////////////////
	BOOL InitHCPFromIndex(sHcpParam* sHcpParamter);                                         //����ģʽ��ʼ��HCP
	BOOL InitHCP(sHcpParam* sHcpParamter);                                                 //��ʼ��HCP
    BOOL CreateGrabThread();																// �����ɼ��̣߳�һ���ɼ�����ֻ����һ���ɼ��߳�	
	BOOL TerminateGrabThread();                                                            //��ֹ�ɼ��߳�
	BOOL Destroy();                                                                        //��ֹ�ɼ��̲߳��ͷ��ڴ�
	static unsigned int GrabThread(LPVOID lpParam);	
	XRayStatus AcquireImage();
	int TransfRawToByte();                            
	//XRayStatus AcquireImage();
	void GrabbedImageSendCallbackToDisplay(XRayStatus status);                             // ��ȡ��ͼƬ�ص������ڽ������ͼƬ��ʾ  
	bool SnapImage();                                                                      //��֡�ɼ�
	bool IsAcquireSuccess();                                                               //�Ƿ�ɼ��ɹ�
	bool Freeze();																		   // ���������ɼ�����Ҫ�Ѿ������ɼ��߳�

	bool Grab();                                                                           //�����ɼ�
	bool IsGrabbing();                                                                     //�Ƿ����������ɼ�
	bool IsHCPOnline();                                                                    //����Ƿ�����


public:

	///////////////////////////////�ص�������ز���//////////////////////////////
	void*	             m_pMainFrm;														// �ص�����������ָ��
	XRayGrabbedCallback	 m_funcCallback;													// �ص�����	
	bool				 m_bSendCallbackEnable;												// �Ƿ�����ص�	


	////////////////HCP����//////////////////////////////////////////////
	cHcpAction           *CHcp;
	int                  result;
	int                  Frm;
	int                  Buf;
	int                  Mode;
	char                 FrameReadyName[MAX_STR];                                          //����32���ֽڣ�������ַ�����frameͬ��������
	WORD*                m_RawPtr;                                                         //�ڶ��Ͽ����ڴ����RAW����
	bool				 m_bHCPOnline;														// HCP���ӳɹ���־λ
	bool				 m_bHCPThreadIsActive;												// HCP�ɼ����߳��Ѿ�������׼�����ܲɼ��ź�

	bool				 m_bTerminate;														// �˳�ʵʱ��ʾ��־
	///////////////�̲߳ɼ�����///////////////////////////////////
private:
	HANDLE				 m_hEventSnapped;	
	HANDLE				 m_hEventToRun;														// �ɼ��̴߳�����ͣʱ��������Ϣ��ʼ�ɼ��¼����
	HANDLE				 m_hEventExit;			  											// ���߳�֪ͨ���߳��ѽ����¼����
	CWinThread*	         m_pWinThread;														// �����ɼ��߳�
	HCPWorkMode          m_HCPWorkMode;  													// �ɼ��߳������Ĺ���ģʽ
	bool				 m_bHaveStopped;													// �Ƿ��Ѿ��˳�ʵʱ�ɼ���ʾ
	bool	             m_bIsGrabbing;														// �Ƿ����������ɼ�ͼ��



	////////////////ͼ��ɼ�������ʹ�õı���//////////////////////////////////////////////
public:
	CRITICAL_SECTION	 m_criticalSection;													// �ٽ���������ͼ�����ݵĶ�д	
	bool                 m_bAcquireSuceess;                                     
	unsigned char*       m_pGrabImage;														// �ɼ�����ͼ������
	long	             m_lWidth;															// ͼ��Ŀ�
	long	             m_lHeight;															// ͼ��ĸ�
	XRayPixelFormat		 m_pixelFormat;														// ͼ������ظ�ʽ				
	CRaw*                m_pRaw;                                                            //���߾���ͼ��ת�����ܱ�ʶ��ĸ�ʽ



};
//#pragma pack(pop)