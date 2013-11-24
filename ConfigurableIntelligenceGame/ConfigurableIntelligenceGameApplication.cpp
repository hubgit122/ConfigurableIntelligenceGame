
// ConfigurableIntelligenceGameApplication.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "ConfigurableIntelligenceGameApplication.h"
#include "MainFrm.h"

#include "ConfigurableIntelligenceGameApplicationDoc.h"
#include "ConfigurableIntelligenceGameApplicationView.h"

#include "Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CConfigurableIntelligenceGameApplicationApp

BEGIN_MESSAGE_MAP(CConfigurableIntelligenceGameApplicationApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CConfigurableIntelligenceGameApplicationApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CConfigurableIntelligenceGameApplicationApp ����

CConfigurableIntelligenceGameApplicationApp::CConfigurableIntelligenceGameApplicationApp()
{
	m_bHiColorIcons = TRUE;

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("ConfigurableIntelligenceGameApplication.AppID.NoVersion"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CConfigurableIntelligenceGameApplicationApp ����

CConfigurableIntelligenceGameApplicationApp theApp;


// CConfigurableIntelligenceGameApplicationApp ��ʼ��

BOOL CConfigurableIntelligenceGameApplicationApp::InitInstance()
{
	CWinAppEx::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
			RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CConfigurableIntelligenceGameApplicationDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CConfigurableIntelligenceGameApplicationView));

	if (!pDocTemplate)
	{
		return FALSE;
	}

	AddDocTemplate(pDocTemplate);


	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
	{
		return FALSE;
	}

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

// CConfigurableIntelligenceGameApplicationApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
	public:
		CAboutDlg();

		// �Ի�������
		enum { IDD = IDD_ABOUTBOX };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

		// ʵ��
	protected:
		DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CConfigurableIntelligenceGameApplicationApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CConfigurableIntelligenceGameApplicationApp �Զ������/���淽��

void CConfigurableIntelligenceGameApplicationApp::PreLoadState()
{
}

void CConfigurableIntelligenceGameApplicationApp::LoadCustomState()
{
}

void CConfigurableIntelligenceGameApplicationApp::SaveCustomState()
{
}

// CConfigurableIntelligenceGameApplicationApp ��Ϣ�������



