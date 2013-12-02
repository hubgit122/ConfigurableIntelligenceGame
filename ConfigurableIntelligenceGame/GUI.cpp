#include "stdafx.h"
#include "Operation.h"
#include "GUI.h"
#include "Chessman.h"
#include "Game.h"
#include "CConfigurableIntelligenceGameView.h"
#include "Player.h"
#include <math.h>
#include "Stack.h"
#include "MotionGenerator.h"

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

	void CIG::GUI::drawBoard(Chessboard* cb)
	{
		cigView->DrawBoard(cb);
	}

	UINT GUI::runThread( LPVOID pParam )
	{
		cigView = (CConfigurableIntelligenceGameView*)pParam;

		MSG msg;
		PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
		//通知其它线程消息队列已创建好
		SetEvent(cigView->workThreadOK);

		while(true)
		{
			GetMessage(&msg, NULL, 0, 0);

			switch(msg.message)
			{
				case WM_QUIT:
					return 0;

				case WM_RESTART:
					if (cigView)
					{
						cigView->nowBoard =Chessboard();
						cigView->workThreadOK.SetEvent();
					}
					break;

				case WM_GET_MOVE:
					Chessboard chessboard = cigView->nowBoard;
					Action action;
					Player& nowPlayer = chessboard.players[chessboard.nowTurn];
					nowPlayer.makeBestAction(&chessboard, &action);			//这个函数要执行很长时间, 返回时cigView可能已经是NULLL了. 

					if (cigView)
					{
						if (chessboard.canMakeWholeAction(action))
						{
							chessboard.onWholeActionIntent(action,true);
							cigView->nowBoard = chessboard;
							cigView->actionOfLastRound = action;
							GUI::postMessage(WM_MOVE_COMPLETE,0,0);
						}
						else
						{
							ostringstream oss;
							oss<<"走法不正确, 请重新走或重新配置. \n";
							oss << (chessboard);oss << (action);
							GUI::inform(oss.str());
						}
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
	bool GUI::namedChessman = false;

	bool GUI::markCrossByCircle = true;
	float GUI::additionalPointRadias = 3;

	bool GUI::drawLineWhenDrawDot = false;

	float GUI::thetaOfXTop = 0;
	float GUI::thetaOfXY = 120;

	float GUI::lengthOfLattticeX = 40;
	float GUI::lengthOfLattticeY = lengthOfLattticeX;
	float GUI::borderWidth = lengthOfLattticeX;
	int GUI::latticePenWidth = 2;

	float GUI::markCircleRadias = lengthOfLattticeX/2*0.9;

	PointOrVector_Float GUI::chessmanRect(0.9*lengthOfLattticeX, 0.9*lengthOfLattticeY);

	PointOrVector_Float GUI::Vy;
	PointOrVector_Float GUI::Vx;

	PointOrVector_Float GUI::coordinateOf00;
	PointOrVector_Float GUI::boundsOfBoard;

	vector<PointOrVector> GUI::LINE_DIRECTION;
	float GUI::boundsOfBoardRelatively[4];

	vector<Line> GUI::addtionalLines;
	vector<PointOrVector> GUI::addtionalPoints;

	CString GUI::playerName[CIGRuleConfig::PLAYER_NUM] = {_T("A"), _T("B"), _T("C"), _T("D"), _T("E"), _T("F")};

	CString GUI::chessmanName[CIGRuleConfig::CHESSMAN_TYPE_NUM] = {_T(" ") };

	COLORREF GUI::playerColor[CIGRuleConfig::PLAYER_NUM] = {RGB(255, 255, 255), RGB(0, 0, 0), RGB(255,0,0), RGB(0,255,0), RGB(0,0,255), RGB(255,0,255)};

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
		{
			/*************************************************************************
				检查棋子是否太大而导致棋盘放不下.
				检查边和对角线上的投影.
			**************************************************************************/
			if (roundInt(chessmanRect * Vx) > roundInt(Vx * Vx))
			{
				inform("棋子在X方向太大", true);
			}

			if (roundInt(chessmanRect * Vy) > roundInt(Vy * Vy))
			{
				inform("棋子在Y方向太大", true);
			}
	
			if (!roundChessman)
			{
				if (roundInt(chessmanRect * (Vx + Vy)) > roundInt((Vx + Vy) * (Vx + Vy)))
				{
					inform("棋子在对角线方向太大", true);
				}
	
				if (roundInt(chessmanRect * (Vx - Vy)) > roundInt((Vx - Vy) * (Vx - Vy)))
				{
					inform("棋子在副对角线方向太大", true);
				}
			} 
			else
			{
				if (roundInt(chessmanRect * chessmanRect /2) > roundInt((Vx + Vy) * (Vx + Vy)))
				{
					inform("棋子在对角线方向太大", true);
				}

				if (roundInt(chessmanRect * chessmanRect /2) > roundInt((Vx - Vy) * (Vx - Vy)))
				{
					inform("棋子在副对角线方向太大", true);
				}
			}
		}
		LINE_DIRECTION.clear();
		LINE_DIRECTION.push_back(PointOrVector(1, 0));
		LINE_DIRECTION.push_back(PointOrVector(0, 1));
		LINE_DIRECTION.push_back(PointOrVector(1, 1));

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
		// addtionalLines.clear();
		
		// 添加几个特殊点
		//addtionalPoints.clear();
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

	void GUI::askForAction(Chessboard*cb, void* op)
	{
		MotionGenerator mg(*cb);
		mg.generateActions(true);
		if (mg.actionStack.size)
		{
			*(Action*)op = mg.actionStack.top();
		}
	}

	CEvent GUI::pointGot;
	PointOrVector GUI::guiPoint;
	void GUI::getPoint( PointOrVector&dist )
	{
		postMessage(WM_GET_MOVE,0,0);
		pointGot.Lock();
		dist = guiPoint;
	}
}
