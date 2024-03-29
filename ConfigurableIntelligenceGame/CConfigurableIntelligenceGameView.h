
// CConfigurableIntelligenceGameView.h : CConfigurableIntelligenceGameView 类的接口
//


#pragma once

#include "Stack.h"
#include "Chessboard.h"
#include "CIGRuleConfig.h"
using namespace CIG;

// CConfigurableIntelligenceGameView 窗口

class CConfigurableIntelligenceGameView : public CWnd
{
		// 构造
	public:
		CConfigurableIntelligenceGameView();

		// 特性
	public:
		CWinThread* m_GameThread;
		CEvent workThreadOK;
		CEvent moveComplete;
		CIG::Move moveOfLastRound;
		CIG::Chessboard nowBoard;
		bool getMove;
		int nBoardBaseDC;
		int nChessmanDC[CIGRuleConfig::PLAYER_NUM][CIGRuleConfig::CHESSMAN_TYPE_NUM];
		CBitmap boardBaseBitmap;
		CBitmap chessmanBaseBitmap;
        Stack<Move, 20, 0> history;

		// 操作
	public:
		void DrawBoard();
		void WrapChessWithFrame(CDC& dc, PointOrVector logicCoo, COLORREF color = RGB(255, 0, 0), bool rectangleNotCircle = true, bool fill = false);

		void GenerateBoardBaseDC(CDC& boardBaseDC, CBitmap* pBoardBaseDCBmpOld);

		// 重写
	protected:
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

		// 实现
	public:
		virtual ~CConfigurableIntelligenceGameView();

		// 生成的消息映射函数
	protected:
		afx_msg void OnPaint();
		DECLARE_MESSAGE_MAP()
	public:
		//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnGameNew();
	protected:
		afx_msg LRESULT OnMoveComplete(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnGetMove(WPARAM wParam, LPARAM lParam);
	public:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		//		afx_msg void OnClose();
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	protected:
		afx_msg LRESULT OnOperationText(WPARAM wParam, LPARAM lParam);
//        afx_msg LRESULT OnDrawBoard(WPARAM wParam, LPARAM lParam);
    public:
    afx_msg void OnEditUndo();
};

