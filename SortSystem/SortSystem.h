
// SortSystem.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSortSystemApp:
// �йش����ʵ�֣������ SortSystem.cpp
//

class CSortSystemApp : public CWinApp
{
public:
	CSortSystemApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSortSystemApp theApp;