
// ConfigurableIntelligenceGameApplicationView.cpp : CConfigurableIntelligenceGameApplicationView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "ConfigurableIntelligenceGameApplication.h"
#endif

#include "ConfigurableIntelligenceGameApplicationDoc.h"
#include "ConfigurableIntelligenceGameApplicationView.h"
#include "GUI.h"
#include "Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CConfigurableIntelligenceGameApplicationView

IMPLEMENT_DYNCREATE(CConfigurableIntelligenceGameApplicationView, CView)

BEGIN_MESSAGE_MAP(CConfigurableIntelligenceGameApplicationView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CConfigurableIntelligenceGameApplicationView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FILE_NEW, &CConfigurableIntelligenceGameApplicationView::OnFileNew)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CConfigurableIntelligenceGameApplicationView ����/����
CConfigurableIntelligenceGameApplicationView* CIG::GUI::cigView = NULL;

CConfigurableIntelligenceGameApplicationView::CConfigurableIntelligenceGameApplicationView():nowBoard(),actionOfThisRound("actionOfThisRound")
{
	// TODO: �ڴ˴���ӹ������
}

CConfigurableIntelligenceGameApplicationView::~CConfigurableIntelligenceGameApplicationView()
{
	if (m_GameThread)
	{
		m_GameThread->PostThreadMessage(WM_QUIT, 0, 0);
	}
}

BOOL CConfigurableIntelligenceGameApplicationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CConfigurableIntelligenceGameApplicationView ����

void CConfigurableIntelligenceGameApplicationView::OnDraw(CDC* pDC)
{
	CConfigurableIntelligenceGameApplicationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc)
	{
		return;
	}

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���

	pDC->TextOut(0, 0, CString("test"));

	CIG::GUI::drawComplete.SetEvent();
}


// CConfigurableIntelligenceGameApplicationView ��ӡ


void CConfigurableIntelligenceGameApplicationView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CConfigurableIntelligenceGameApplicationView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CConfigurableIntelligenceGameApplicationView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CConfigurableIntelligenceGameApplicationView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CConfigurableIntelligenceGameApplicationView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CConfigurableIntelligenceGameApplicationView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
#endif
}


// CConfigurableIntelligenceGameApplicationView ���

#ifdef _DEBUG
void CConfigurableIntelligenceGameApplicationView::AssertValid() const
{
	CView::AssertValid();
}

void CConfigurableIntelligenceGameApplicationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CConfigurableIntelligenceGameApplicationDoc* CConfigurableIntelligenceGameApplicationView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CConfigurableIntelligenceGameApplicationDoc)));
	return (CConfigurableIntelligenceGameApplicationDoc*)m_pDocument;
}
#endif //_DEBUG


// CConfigurableIntelligenceGameApplicationView ��Ϣ�������


void CConfigurableIntelligenceGameApplicationView::OnFileNew()
{
	m_GameThread->PostThreadMessage(WM_RESTART, 0, 0);
	eventThreadMessageOK.Lock();
	
	m_GameThread->PostThreadMessage(WM_GET_MOVE,(WPARAM)&actionOfThisRound,(LPARAM)&nowBoard);
}


int CConfigurableIntelligenceGameApplicationView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	CIG::GUI::cigView = this;
	m_GameThread = AfxBeginThread(CIG::GUI::runThread,this);
	eventThreadMessageOK.Lock();

	return 0;
}
