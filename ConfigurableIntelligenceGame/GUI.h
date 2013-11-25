
#ifndef __GUI_H__
#define __GUI_H__
#include "stdafx.h"
#include "utilities.h"
#include "CIGRuleConfig.h"

//GUI�߳������߳��������, ���¿�ʼ.
#define WM_RESTART			(WM_USER+1)

//GUI�̸߳������߳���Ϣ, ���Ը�����һ����, �Һø�����Ľ����ͼ.
//�������̷߳��ֵ�ǰ�������, ֪ͨGUI�̶߳�ȡ�߷�, Ȼ�󴫸�������, �Һø��㽻��.
//������ƿ���չ, GUI����֪����һ�����˻��ǵ���.
#define WM_GET_MOVE 		(WM_USER+2)
#define WM_MOVE_COMPLETE (WM_USER+3)

class CConfigurableIntelligenceGameView;
namespace CIG
{
	class GUI
	{
		public:

			/*************************************************************************
				����ͨ�Žӿ�, ��ƽ̨ʱ��Ҫ��д����.
			**************************************************************************/
			static CEvent drawComplete;
			static CEvent moveComplete;
			static CConfigurableIntelligenceGameView* cigView;
			static void drawBoard();
			static void inform(const string& messsage, bool exit = false);
			static void exit();
			static UINT runThread(LPVOID pParam);
			static void postMessage( UINT msg, WPARAM wp, LPARAM  lp);

			static void askForAction(void* op);

			/*************************************************************************
				�������̻��Ʋ���
				��ȷ�����Ӵ�С, �ڴ��ڳ�ʼ��ʱ�������̲������������ȷ���ͻ�����С.
				�涨����������:
				0----------------->x
				|
				|
				|
				��
				y
			**************************************************************************/
			typedef float degree;

			static bool roundChessman;					//����Բ��
			static bool drawCross;								//�Ƿ��ǵ�
			static bool drawLineWhenDrawDot;		//�Ƿ���(������, ��һ��Ҫ����, ����㴦���пհ�)
			static float dotRadias;							//���̵�İ뾶
			static degree thetaOfXY;						//�����ߵļн�
			static degree thetaOfXTop;					//x��ĸ���
			static PointOrVector_Float chessmanRect;		//���뱣֤������û���ص�
			static float lengthOfLattticeX;
			static float lengthOfLattticeY;
			static PointOrVector_Float Vx;
			static PointOrVector_Float Vy;
			static CString chessmanName[CIGRuleConfig::CHESSMAN_TYPE_NUM];
			static CString playerName[CIGRuleConfig::PLAYER_NUM];
			static COLORREF playerColor[CIGRuleConfig::PLAYER_NUM];
			static float borderWidth;
			static vector<PointOrVector_Float> LINE_DIRECTION;
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
			static int latticePenWidth;
			//�ж��������̵ı߽�Ƚ���, ������.
			//static int borderPenColor[3];
	};
}

#endif /*__GUI_H_*/
