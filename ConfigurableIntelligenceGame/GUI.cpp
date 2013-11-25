#include "stdafx.h"
#include "Operation.h"
#include "GUI.h"
#include "Chessman.h"
#include "Game.h"
#include "CConfigurableIntelligenceGameView.h"
#include "Player.h"
#include <math.h>
#include "IntellegenceEngine.h"

namespace CIG
{
	CConfigurableIntelligenceGameView* GUI::cigView = NULL;

	CEvent GUI::drawComplete;
	CEvent GUI::moveComplete;

	std::wstring s2ws(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}

	void CIG::GUI::inform( const string& messsage, bool exit)
	{
#ifdef UNICODE
		std::wstring stemp = s2ws(messsage); // Temporary buffer is required
		LPCWSTR result = stemp.c_str();
#else
		LPCWSTR result = s.c_str();
#endif
		cigView->MessageBox((LPCTSTR)result);			// TO-DO 这里会不会对话框没返回就退出了呢? 模态对话框就行了.

		if (exit)
		{
			GUI::exit();
		}
	}

	void CIG::GUI::drawBoard()
	{
		cigView->PostMessage(WM_PAINT);
		drawComplete.Lock();
	}

	UINT GUI::runThread( LPVOID pParam )
	{
		Game* game = NULL;
		cigView = (CConfigurableIntelligenceGameView*)pParam;

		MSG msg;
		PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
		//通知其它线程消息队列已创建好
		SetEvent(cigView->eventThreadMessageOK);

		while(true)
		{
			GetMessage(&msg, NULL, 0, 0);

			switch(msg.message)
			{
				case WM_QUIT:
					Game::deleteInstance(game);
					return 0;

				case WM_RESTART:
					Game::deleteInstance(game);
					game = new Game();
					cigView->nowBoard = game->chessBoard;
					SetEvent(cigView->eventThreadMessageOK);
					break;

				case WM_GET_MOVE:
					OperationStack* pAction = (OperationStack*)msg.wParam;
					Chessboard* pChessboard = (Chessboard*)msg.lParam;
					IntellegenceEngine& nowPlayer = (IntellegenceEngine&)game->chessBoard.players[game->chessBoard.nowTurn];
					nowPlayer.makeBestAction(*pAction);

					if (game->chessBoard.canMakeAction(*pAction))
					{
						if (!game->chessBoard.onChangeTurn())
						{
							ostringstream oss;
							oss<<"走法不正确, 请重新走或检查规则配置. \n";
							oss << (game->chessBoard) << (*pAction);
							GUI::inform(oss.str());
						}
						else
						{
							game->chessBoard.makeAction(*pAction);
							*pChessboard = Chessboard(game->chessBoard);
							GUI::postMessage(WM_MOVE_COMPLETE,(WPARAM)pAction,(LPARAM)pChessboard);
						}
					}
					else
					{
						ostringstream oss;
						oss<<"走法不正确, 请重新走或重新配置. \n";
						oss << (game->chessBoard) << (*pAction);
						GUI::inform(oss.str());
					}
					break;
			}
		}

		return 0;
	}

	void CIG::GUI::exit()
	{
		cigView->PostMessage(WM_QUIT);
	}

	void GUI::postMessage( UINT msg, WPARAM wp, LPARAM  lp)
	{
		cigView->PostMessage(msg, wp, lp);
	}

	bool GUI::roundChessman = true;

	bool GUI::drawCross = true;
	float GUI::dotRadias = 1;
	bool GUI::drawLineWhenDrawDot = true;

	float GUI::thetaOfXTop = 0;
	float GUI::thetaOfXY = 90;

	float GUI::lengthOfLattticeX = 70;
	float GUI::lengthOfLattticeY = lengthOfLattticeX;
	float GUI::borderWidth = 50.0;
	int GUI::latticePenWidth = 2;

	PointOrVector_Float GUI::chessmanRect(lengthOfLattticeX, lengthOfLattticeY);

	PointOrVector_Float GUI::Vy;
	PointOrVector_Float GUI::Vx;

	PointOrVector_Float GUI::coordinateOf00;
	PointOrVector_Float GUI::boundsOfBoard;

	vector<PointOrVector_Float> GUI::LINE_DIRECTION;
	float GUI::boundsOfBoardRelatively[4];

	vector<Line> GUI::addtionalLines;

	CString GUI::playerName[CIGRuleConfig::PLAYER_NUM] = {_T("石守谦"), _T("电脑")};

	CString GUI::chessmanName[CIGRuleConfig::CHESSMAN_TYPE_NUM] = {_T("将"), _T("士"), _T("象"), _T("马"), _T("车"), _T("炮"), _T("兵") };

	COLORREF GUI::playerColor[CIGRuleConfig::PLAYER_NUM] = {RGB(235, 0, 0), RGB(0, 0, 0)};

	//得到格点相对的0,0的几何坐标.
	//以格点的0,0 的坐标为(0,0)
	PointOrVector_Float GUI::getGeometryCoordination(int x, int y)
	{
		return x * Vx + y * Vy + coordinateOf00;
	}

	PointOrVector_Float GUI::getGeometryCoordination( PointOrVector p )
	{
		return getGeometryCoordination(p.x[0], p.x[1]);
	}

	void GUI::refreshBoardDisplayData()
	{
		/*************************************************************************
			检查棋子是否太大而导致棋盘放不下.
			检查边和对角线上的投影.
		**************************************************************************/
		if (chessmanRect * Vx > Vx * Vx)
		{
			inform("棋子在X方向太大", true);
		}

		if (chessmanRect * Vy > Vy * Vy)
		{
			inform("棋子在Y方向太大", true);
		}

		if (chessmanRect * (Vx + Vy) > (Vx + Vy) * (Vx + Vy))
		{
			inform("棋子在对角线方向太大", true);
		}

		if (PointOrVector_Float(chessmanRect.x[0], - chessmanRect.x[1]) * (Vx - Vy) > (Vx - Vy) * (Vx - Vy))
		{
			inform("棋子在副对角线方向太大", true);
		}

		/*************************************************************************
			设置棋盘显示参数
			先确定棋子大小, 在窗口初始化时绘制棋盘并根据棋盘情况确定客户区大小.
		**************************************************************************/
		const long double PI = acos((long double) - 1.0);
		typedef float rad;
		rad theta1 = thetaOfXTop / 180.0 * PI,
			theta2 = thetaOfXY / 180.0 * PI;
		//支持动态调整棋盘, 所以不是常量, 可以调整.
		Vx = PointOrVector_Float(lengthOfLattticeX * cos(theta1) , lengthOfLattticeX * sin(theta1) );
		Vy = PointOrVector_Float(lengthOfLattticeX * cos(theta1 + theta2) , lengthOfLattticeX * sin(theta1 + theta2));

		LINE_DIRECTION.push_back(PointOrVector_Float(1, 0));
		LINE_DIRECTION.push_back(PointOrVector_Float(0, 1));

		//设置界限初值
		boundsOfBoardRelatively[RIGHT] = -1E7;
		boundsOfBoardRelatively[LEFT] = 1E7;
		boundsOfBoardRelatively[TOP] = 1E7;
		boundsOfBoardRelatively[BOTTOM] = -1E7;

		coordinateOf00 = PointOrVector_Float(0, 0);			//先求相对位置

		for (int i = 0; i < (1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2); ++i)
		{
			for (int j = 0; j < (1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2); j++)
			{
				if (CIGRuleConfig::BOARD_RANGE[j][i])
				{
					PointOrVector_Float temp = getGeometryCoordination(i, j);

					if (temp.x[0] > boundsOfBoardRelatively[RIGHT])
					{
						boundsOfBoardRelatively[RIGHT] = temp.x[0];
					}

					if (temp.x[0] < boundsOfBoardRelatively[LEFT])
					{
						boundsOfBoardRelatively[LEFT] = temp.x[0];
					}

					if (temp.x[1] < boundsOfBoardRelatively[TOP])
					{
						boundsOfBoardRelatively[TOP] = temp.x[1];
					}

					if (temp.x[1] > boundsOfBoardRelatively[BOTTOM])
					{
						boundsOfBoardRelatively[BOTTOM] = temp.x[1];
					}
				}
			}
		}

		//确定棋盘矩形的大小.
		boundsOfBoard.x[0] = borderWidth * 2 + boundsOfBoardRelatively[RIGHT] - boundsOfBoardRelatively[LEFT];
		boundsOfBoard.x[1] = borderWidth * 2 + boundsOfBoardRelatively[BOTTOM] - boundsOfBoardRelatively[TOP];

		//原点重新定位
		coordinateOf00 = PointOrVector_Float( borderWidth - boundsOfBoardRelatively[LEFT], borderWidth - boundsOfBoardRelatively[TOP]);

		//添加几条特殊线
		addtionalLines.push_back(Line(6, 3, 8, 5));
		addtionalLines.push_back(Line(8, 3, 6, 5));
		addtionalLines.push_back(Line(6, 12, 8, 10));
		addtionalLines.push_back(Line(8, 12, 6, 10));

		for (int i = 4; i <= 10; ++i)
		{
			addtionalLines.push_back(Line(i, 7, i, 8));
		}
	}

	PointOrVector GUI::getLogicalCoordination( float x, float y )
	{
		return getLogicalCoordination(PointOrVector_Float(x, y));
	}

	PointOrVector GUI::getLogicalCoordination( PointOrVector_Float p )
	{
		p = p - coordinateOf00;
		float delt_ = 1 / (Vx.x[0] * Vy.x[1] - Vx.x[1] * Vy.x[0]);

		int x = roundInt( delt_ *  (p.x[0] * Vy.x[1] - p.x[1] * Vy.x[0])),
			y = roundInt( delt_ *  (Vx.x[0] * p.x[1] - Vx.x[1] * p.x[0]));

		return PointOrVector(x, y);
	}

	void GUI::askForAction(void* op)
	{
#ifdef DEBUG_MESSAGE
		inform("假设我已经走完一步"/*"Assume that we have mede a move there. "*/);
#else
		postMessage(WM_GET_MOVE, (WPARAM)&op, 0);
		moveComplete.Lock();
#endif
	}
}
