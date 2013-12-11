
#ifndef __GUI_H__
#define __GUI_H__
#include "stdafx.h"
#include "utilities.h"
#include "CIGRuleConfig.h"

//GUI线程令主线程清空数据, 重新开始.
#define WM_RESTART			(WM_USER+1)

//GUI线程给工作线程消息, 可以给我下一步了, 我好根据你的结果画图.
//而工作线程发现当前玩家是人, 通知GUI线程读取走法, 然后传给我数据, 我好给你交差.
//这样设计可扩展, GUI不必知道下一轮是人还是电脑.
#define WM_GET_MOVE 		(WM_USER+2)
#define WM_MOVE_COMPLETE (WM_USER+3)

#define CIG_END			0
#define  CIG_UNDO		1
#define CIG_POINT		2

class CConfigurableIntelligenceGameView;
namespace CIG
{
	class Chessboard;
	class GUI
	{
		public:

			/*************************************************************************
				设置通信接口, 跨平台时需要重写代码.
			**************************************************************************/
			static CEvent drawComplete;
			static CEvent moveComplete;
			static CConfigurableIntelligenceGameView* cigView;
			static void drawBoard(Chessboard* cb = NULL, void* action =NULL);
			static void inform(const string& messsage, bool exit = false);
			static void exit();
			static UINT runThread(LPVOID pParam);
			static void postMessage( UINT msg, WPARAM wp, LPARAM  lp);

			static void askForAction(Chessboard*cb, void* op);
			static void GUI::getInput(PointOrVector&dist, UINT& msg);

			/*************************************************************************
				设置棋盘绘制参数
				先确定棋子大小, 在窗口初始化时绘制棋盘并根据棋盘情况确定客户区大小.
				规定几何坐标轴:
				0----------------->x
				|
				|
				|
				↓
				y
			**************************************************************************/
			typedef float degree;

			static bool roundChessman;					//棋子圆或方
			static bool namedChessman;
			static bool markCrossByCircle;								//是否标记点
			static bool drawLineWhenDrawDot;		//是否画线(若画线, 则一定要画点, 否则点处会有空白)
			static float markCircleRadias;							//棋盘点的半径
			static float additionalPointRadias;
			static degree thetaOfXY;						//棋盘线的夹角
			static degree thetaOfXTop;					//x轴的俯角
			static PointOrVector_Float chessmanRect;		//必须保证棋盘上没有重叠
			static float lengthOfLattticeX;
			static float lengthOfLattticeY;
			static PointOrVector_Float Vx;
			static PointOrVector_Float Vy;
			static CString chessmanName[CIGRuleConfig::CHESSMAN_TYPE_NUM];
			static CString playerName[CIGRuleConfig::PLAYER_NUM];
			static COLORREF playerColor[CIGRuleConfig::PLAYER_NUM];
			static float borderWidth;
			static vector<PointOrVector> LINE_DIRECTION;
			static float boundsOfBoardRelatively[4];
			enum {TOP, BOTTOM, LEFT, RIGHT};
			static void refreshBoardDisplayData();
			static PointOrVector_Float getGeometryCoordination(int x, int y);
			static PointOrVector_Float getGeometryCoordination(PointOrVector p);
			static PointOrVector getLogicalCoordination(PointOrVector_Float p);
			static PointOrVector getLogicalCoordination(float x, float y);
			static PointOrVector_Float boundsOfBoard;
			static PointOrVector_Float coordinateOf00;
			static vector<Line> addtionalLines;
			static vector<PointOrVector> addtionalPoints;
			static int latticePenWidth;
			static PointOrVector guiPoint;
			static CEvent inputGot;
			static UINT msg;
			//判断任意棋盘的边界比较难, 不做了.
			//static int borderPenColor[3];
	};
}

#endif /*__GUI_H_*/

