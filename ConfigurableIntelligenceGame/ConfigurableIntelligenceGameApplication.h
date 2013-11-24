
// ConfigurableIntelligenceGameApplication.h : ConfigurableIntelligenceGameApplication 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号

// CConfigurableIntelligenceGameApplicationApp:
// 有关此类的实现，请参阅 ConfigurableIntelligenceGameApplication.cpp
//

class CConfigurableIntelligenceGameApplicationApp : public CWinAppEx
{
	public:
		CConfigurableIntelligenceGameApplicationApp();


		// 重写
	public:
		virtual BOOL InitInstance();

		// 实现
		UINT  m_nAppLook;
		BOOL  m_bHiColorIcons;

		virtual void PreLoadState();
		virtual void LoadCustomState();
		virtual void SaveCustomState();

		afx_msg void OnAppAbout();
		DECLARE_MESSAGE_MAP()
};

extern CConfigurableIntelligenceGameApplicationApp theApp;
extern CEvent g_EventInformTurn;