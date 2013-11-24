
// ConfigurableIntelligenceGameApplicationView.cpp : CConfigurableIntelligenceGameApplicationView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CConfigurableIntelligenceGameApplicationView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FILE_NEW, &CConfigurableIntelligenceGameApplicationView::OnFileNew)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CConfigurableIntelligenceGameApplicationView 构造/析构
CConfigurableIntelligenceGameApplicationView* CIG::GUI::cigView = NULL;

CConfigurableIntelligenceGameApplicationView::CConfigurableIntelligenceGameApplicationView():nowBoard(),actionOfThisRound("actionOfThisRound")
{
	// TODO: 在此处添加构造代码
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
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CConfigurableIntelligenceGameApplicationView 绘制

void CConfigurableIntelligenceGameApplicationView::OnDraw(CDC* pDC)
{
	CConfigurableIntelligenceGameApplicationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc)
	{
		return;
	}

	// TODO: 在此处为本机数据添加绘制代码

	pDC->TextOut(0, 0, CString("test"));

	CIG::GUI::drawComplete.SetEvent();
}


// CConfigurableIntelligenceGameApplicationView 打印


void CConfigurableIntelligenceGameApplicationView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CConfigurableIntelligenceGameApplicationView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CConfigurableIntelligenceGameApplicationView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CConfigurableIntelligenceGameApplicationView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
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


// CConfigurableIntelligenceGameApplicationView 诊断

#ifdef _DEBUG
void CConfigurableIntelligenceGameApplicationView::AssertValid() const
{
	CView::AssertValid();
}

void CConfigurableIntelligenceGameApplicationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CConfigurableIntelligenceGameApplicationDoc* CConfigurableIntelligenceGameApplicationView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CConfigurableIntelligenceGameApplicationDoc)));
	return (CConfigurableIntelligenceGameApplicationDoc*)m_pDocument;
}
#endif //_DEBUG


// CConfigurableIntelligenceGameApplicationView 消息处理程序


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

	// TODO:  在此添加您专用的创建代码

	CIG::GUI::cigView = this;
	m_GameThread = AfxBeginThread(CIG::GUI::runThread,this);
	eventThreadMessageOK.Lock();

	return 0;
}
