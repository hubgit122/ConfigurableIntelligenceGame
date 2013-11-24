
// ConfigurableIntelligenceGameApplicationView.h : CConfigurableIntelligenceGameApplicationView 类的接口
//

#pragma once
#include "ConfigurableIntelligenceGameApplicationDoc.h"
#include "Stack.h"
#include "ChessBoard.h"

//GUI线程令主线程清空数据, 重新开始. 
#define WM_RESTART			WM_USER+1							

//GUI线程给工作线程消息, 可以给我下一步了, 我好根据你的结果画图. 
//而工作线程发现当前玩家是人, 通知GUI线程读取走法, 然后传给我数据, 我好给你交差. 
//这样设计可扩展, GUI不必知道下一轮是人还是电脑. 
#define WM_GET_MOVE 		WM_USER+2							
#define WM_MOVE_COMPLETE WM_USER+3

class CConfigurableIntelligenceGameApplicationView : public CView
{
	protected: // 仅从序列化创建
		CConfigurableIntelligenceGameApplicationView();
		DECLARE_DYNCREATE(CConfigurableIntelligenceGameApplicationView)

		// 特性
	public:
		CConfigurableIntelligenceGameApplicationDoc* GetDocument() const;
		CWinThread* m_GameThread;
		CEvent eventThreadMessageOK;
		CIG::OperationStack actionOfThisRound;
		CIG::ChessBoard nowBoard;


		// 操作
	public:

		// 重写
	public:
		virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
		virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
		virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
		virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

		// 实现
	public:
		virtual ~CConfigurableIntelligenceGameApplicationView();
#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
#endif

	protected:

		// 生成的消息映射函数
	protected:
		afx_msg void OnFilePrintPreview();
		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnFileNew();
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // ConfigurableIntelligenceGameApplicationView.cpp 中的调试版本
CConfigurableIntelligenceGameApplicationDoc* CConfigurableIntelligenceGameApplicationView::GetDocument() const
{
	return reinterpret_cast<CConfigurableIntelligenceGameApplicationDoc*>(m_pDocument);
}
#endif

