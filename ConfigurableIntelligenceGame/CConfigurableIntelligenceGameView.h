
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
		CEvent eventThreadMessageOK;
		CIG::OperationStack actionOfLastRound;
		CIG::Chessboard nowBoard;
		bool getAction;
		int nBoardBaseDC;
		int nChessmanDC[CIGRuleConfig::PLAYER_NUM][CIGRuleConfig::CHESSMAN_TYPE_NUM];
		CBitmap boardBaseBitmap;
		CBitmap chessmanBaseBitmap;

		CIGRuleConfig::PLAYER_NAMES input;				//作为谁的输入设备. 

		// 操作
	public:
		void DrawBoard();

		void GenerateBoardBaseDC(CDC& boardBaseDC, CBitmap* pBoardBaseDCBmpOld);

		void GenerateChessmanDC(CDC& chessmanDC, CBitmap* pChessmanDCBmpOld);

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
		afx_msg void OnClose();
};

