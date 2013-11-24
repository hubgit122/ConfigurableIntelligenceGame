
// ConfigurableIntelligenceGameApplicationView.h : CConfigurableIntelligenceGameApplicationView ��Ľӿ�
//

#pragma once
#include "ConfigurableIntelligenceGameApplicationDoc.h"
#include "Stack.h"
#include "ChessBoard.h"

//GUI�߳������߳��������, ���¿�ʼ. 
#define WM_RESTART			WM_USER+1							

//GUI�̸߳������߳���Ϣ, ���Ը�����һ����, �Һø�����Ľ����ͼ. 
//�������̷߳��ֵ�ǰ�������, ֪ͨGUI�̶߳�ȡ�߷�, Ȼ�󴫸�������, �Һø��㽻��. 
//������ƿ���չ, GUI����֪����һ�����˻��ǵ���. 
#define WM_GET_MOVE 		WM_USER+2							
#define WM_MOVE_COMPLETE WM_USER+3

class CConfigurableIntelligenceGameApplicationView : public CView
{
	protected: // �������л�����
		CConfigurableIntelligenceGameApplicationView();
		DECLARE_DYNCREATE(CConfigurableIntelligenceGameApplicationView)

		// ����
	public:
		CConfigurableIntelligenceGameApplicationDoc* GetDocument() const;
		CWinThread* m_GameThread;
		CEvent eventThreadMessageOK;
		CIG::OperationStack actionOfThisRound;
		CIG::ChessBoard nowBoard;


		// ����
	public:

		// ��д
	public:
		virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
		virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
		virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
		virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

		// ʵ��
	public:
		virtual ~CConfigurableIntelligenceGameApplicationView();
#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
#endif

	protected:

		// ���ɵ���Ϣӳ�亯��
	protected:
		afx_msg void OnFilePrintPreview();
		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnFileNew();
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // ConfigurableIntelligenceGameApplicationView.cpp �еĵ��԰汾
CConfigurableIntelligenceGameApplicationDoc* CConfigurableIntelligenceGameApplicationView::GetDocument() const
{
	return reinterpret_cast<CConfigurableIntelligenceGameApplicationDoc*>(m_pDocument);
}
#endif

