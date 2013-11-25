
// CConfigurableIntelligenceGameView.h : CConfigurableIntelligenceGameView ��Ľӿ�
//


#pragma once

#include "Stack.h"
#include "Chessboard.h"
#include "CIGRuleConfig.h"
using namespace CIG;

// CConfigurableIntelligenceGameView ����

class CConfigurableIntelligenceGameView : public CWnd
{
		// ����
	public:
		CConfigurableIntelligenceGameView();

		// ����
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

		CIGRuleConfig::PLAYER_NAMES input;				//��Ϊ˭�������豸. 

		// ����
	public:
		void DrawBoard();

		void GenerateBoardBaseDC(CDC& boardBaseDC, CBitmap* pBoardBaseDCBmpOld);

		void GenerateChessmanDC(CDC& chessmanDC, CBitmap* pChessmanDCBmpOld);

		// ��д
	protected:
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

		// ʵ��
	public:
		virtual ~CConfigurableIntelligenceGameView();

		// ���ɵ���Ϣӳ�亯��
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

