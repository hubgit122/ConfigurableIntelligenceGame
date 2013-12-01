
// CConfigurableIntelligenceGameView.cpp : CConfigurableIntelligenceGameView 类的实现
//

#include "stdafx.h"
#include "ConfigurableIntelligenceGame.h"
#include "CConfigurableIntelligenceGameView.h"
#include "GUI.h"
#include "Player.h"
#include "Chessman.h"
#include "Chessboard.h"
#include "utilities.h"
#include "MotionGenerator.h"

#define DEBUG_GENERATOR

using namespace CIG;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CConfigurableIntelligenceGameView::CConfigurableIntelligenceGameView(): nowBoard(),actionOfLastRound()
{
	//这里的初始化会乱掉. 所以还是在别处再初始化一次吧.

}

int CConfigurableIntelligenceGameView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// TODO:  在此添加您专用的创建代码CIG::GUI::cigView = this;
	m_GameThread = AfxBeginThread(CIG::GUI::runThread, this);
	workThreadOK.Lock();
	getAction = false;

	CClientDC memDC(this);
	boardBaseBitmap.LoadBitmap(IDB_BOARD_BASE);
	chessmanBaseBitmap.LoadBitmap((GUI::roundChessman) ? IDB_CHESSMAN_ROUND_NAMED : IDB_CHESSMAN_RECTANGLE);
	//input = (CIGRuleConfig::PLAYER_NAMES)-1;

	return 0;
}

CConfigurableIntelligenceGameView::~CConfigurableIntelligenceGameView()
{
	GUI::cigView = NULL;
	if (m_GameThread)
	{
		m_GameThread->PostThreadMessage(WM_QUIT, 0, 0);
	}
}


BEGIN_MESSAGE_MAP(CConfigurableIntelligenceGameView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_COMMAND(ID_GAME_NEW, &CConfigurableIntelligenceGameView::OnGameNew)
	ON_MESSAGE(WM_MOVE_COMPLETE, &CConfigurableIntelligenceGameView::OnMoveComplete)
	ON_MESSAGE(WM_GET_MOVE, &CConfigurableIntelligenceGameView::OnGetMove)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CConfigurableIntelligenceGameView 消息处理程序

BOOL CConfigurableIntelligenceGameView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
	{
		return FALSE;
	}

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
									   ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

	return TRUE;
}

void CConfigurableIntelligenceGameView::OnPaint()
{
	CPaintDC memClientDC(this); // 用于绘制的设备上下文
	// TODO: 在此处添加消息处理程序代码

	DrawBoard();

	GUI::drawComplete.SetEvent();
}

void CConfigurableIntelligenceGameView::OnGameNew()
{
	// TODO: 在此添加命令处理程序代码

	m_GameThread->PostThreadMessage(WM_RESTART, 0, 0);
	workThreadOK.Lock();

	PostMessage(WM_PAINT, 0, 0);
	m_GameThread->PostThreadMessage(WM_GET_MOVE, (WPARAM)&actionOfLastRound, (LPARAM)&nowBoard);
}

afx_msg LRESULT CConfigurableIntelligenceGameView::OnMoveComplete(WPARAM wParam, LPARAM lParam)
{
	DrawBoard();
	//MessageBox(_T("test"));

	if (nowBoard.gameOver())
	{
		MessageBox(_T("胜负已分! "));
	}
	else
	{
		m_GameThread->PostThreadMessage(WM_GET_MOVE, 0, 0); 
	}

	return 0;
}

afx_msg LRESULT CConfigurableIntelligenceGameView::OnGetMove(WPARAM wParam, LPARAM lParam)
{
	getAction = true;
	
	GUI::moveComplete.SetEvent();
	return 0;
}

void TransparentBlt2( HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, UINT crTransparent )
{
	HDC hImageDC, hMaskDC;
	HBITMAP hOldImageBMP, hImageBMP, hOldMaskBMP, hMaskBMP;
	hImageBMP = CreateCompatibleBitmap(hdcDest, nWidthDest, nHeightDest);
	hMaskBMP = CreateBitmap(nWidthDest, nHeightDest, 1, 1, NULL);
	hImageDC = CreateCompatibleDC(hdcDest);
	hMaskDC = CreateCompatibleDC(hdcDest);
	hOldImageBMP = (HBITMAP) SelectObject(hImageDC, hImageBMP);
	hOldMaskBMP = (HBITMAP) SelectObject(hMaskDC, hMaskBMP);

	SetStretchBltMode(hdcDest, COLORONCOLOR);
	SetStretchBltMode(hImageDC, COLORONCOLOR);
	SetStretchBltMode(hMaskDC, COLORONCOLOR);

	if (nWidthDest == nWidthSrc && nHeightDest == nHeightSrc)
	{
		BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest,
			   hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
	}
	else
	{
		StretchBlt(hImageDC, 0, 0, nWidthDest, nHeightDest,
				   hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);
	}

	SetBkColor(hImageDC, crTransparent);
	BitBlt(hMaskDC, 0, 0, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCCOPY);
	SetBkColor(hImageDC, RGB(0, 0, 0));
	SetTextColor(hImageDC, RGB(255, 255, 255));
	BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);
	SetBkColor(hdcDest, RGB(255, 255, 255));
	SetTextColor(hdcDest, RGB(0, 0, 0));
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
		   hMaskDC, 0, 0, SRCAND);
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
		   hImageDC, 0, 0, SRCPAINT);

	SelectObject(hImageDC, hOldImageBMP);
	DeleteDC(hImageDC);
	SelectObject(hMaskDC, hOldMaskBMP);
	DeleteDC(hMaskDC);
	DeleteObject(hImageBMP);
	DeleteObject(hMaskBMP);
}

bool yaheiInstalled()
{
	LONG   ires;
	HKEY   hMyKey;
	DWORD   Type   =   REG_SZ;
	DWORD   count =   256;
	byte   mstr[256] = "";
	ires = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"),
						0, KEY_READ, &hMyKey);

	if(ERROR_SUCCESS == ires)
	{
		ires = RegQueryValueEx(hMyKey, _T("微软雅黑 Bold (TrueType)"), 0, &Type, mstr, &count);
	}

	return (mstr[0] != '\0');
}

void CConfigurableIntelligenceGameView::WrapChessWithFrame(CDC& dc, PointOrVector logicCoo, COLORREF color /*= RGB(255,0,0)*/, bool rectangleNotCircle/* = true*/, bool fill/* = false*/)
{
	PointOrVector_Float xy = GUI::getGeometryCoordination(logicCoo);
	CBrush* oldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	CPen pen(PS_SOLID, GUI::latticePenWidth, color);
	CPen* oldPen = dc.SelectObject(&pen);

	dc.Rectangle(roundInt(xy.x[0] - GUI::chessmanRect.x[0] / 2), roundInt(xy.x[1] - GUI::chessmanRect.x[1] / 2),  roundInt(xy.x[0] +GUI::chessmanRect.x[0] / 2), roundInt(xy.x[1] + GUI::chessmanRect.x[1] / 2) );
	
	dc.SelectObject(oldBrush);
	dc.SelectObject(oldPen);
}

void CConfigurableIntelligenceGameView::DrawBoard()
{
	CClientDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	CDC memClientDC;			//暂存dc, 双缓冲绘图. 
	memClientDC.CreateCompatibleDC(&dc);
	CBitmap memBitmap;
	memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap* oldMemBitmap = memClientDC.SelectObject(&memBitmap);

	//画棋盘
	{
		CDC boardBaseDC;
		CBitmap* pBoardBaseDCBmpOld = NULL;
		GenerateBoardBaseDC(boardBaseDC, pBoardBaseDCBmpOld);			//绘图函数

		memClientDC.BitBlt(0, 0, rect.Width(), rect.Height(), &boardBaseDC, 0, 0, SRCCOPY) ; //将源DC中(0,0,20,20)复制到目的DC
		
		boardBaseDC.SelectObject(pBoardBaseDCBmpOld) ;//选入原DDB
	}

	//画棋子
	{
		CFont* pOldFont = NULL;
		CFont font;
		if (GUI::namedChessman)
		{
			font.CreatePointFont(roundInt(0.5 * 10 * min(GUI::chessmanRect.x[0], GUI::chessmanRect.x[1])), _T("华文行楷"), NULL);
			pOldFont = memClientDC.SelectObject(&font);
			memClientDC.SetStretchBltMode(COLORONCOLOR);			// 调整像素模式, 让贴图好看一点
		}
		memClientDC.SetBkMode(TRANSPARENT);

		for (int p = 0; p < CIGRuleConfig::PLAYER_NUM; ++p)				//每个玩家
		{
			for (int c = nowBoard.players[p].ownedChessmans.size - 1; c >= 0; --c)		//每个棋子
			{
				if (nowBoard.players[p].ownedChessmans[c].status==CIGRuleConfig::CAPTURED)		//被吃不画
				{
					continue;
				}
				CDC chessDC;
				chessDC.CreateCompatibleDC(&memClientDC);

				PointOrVector_Float xy = GUI::getGeometryCoordination(nowBoard.players[p].ownedChessmans[c].coordinate);

				if (GUI::namedChessman)
				{
					CBitmap* pBoardBaseDCBmpOld = chessDC.SelectObject(&chessmanBaseBitmap);
					BITMAP bm;
					chessmanBaseBitmap.GetBitmap(&bm);
					TransparentBlt2(memClientDC.m_hDC, roundInt(xy.x[0] - GUI::chessmanRect.x[0] / 2), roundInt(xy.x[1] - GUI::chessmanRect.x[1] / 2), roundInt(GUI::chessmanRect.x[0]), roundInt(GUI::chessmanRect.x[1]), chessDC.m_hDC, 0, 0, bm.bmWidth, bm.bmHeight, RGB(0, 255, 0));

					memClientDC.SetTextColor(GUI::playerColor[p]);
					memClientDC.DrawText(GUI::chessmanName[nowBoard.players[p].ownedChessmans[c].chessmanType], &CRect(xy.x[0] - roundInt(GUI::chessmanRect.x[0] / 2), roundInt(xy.x[1] - GUI::chessmanRect.x[1] / 2), roundInt(xy.x[0] + GUI::chessmanRect.x[0] / 2), xy.x[1] + GUI::chessmanRect.x[1] / 2), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
					chessDC.SelectObject(pBoardBaseDCBmpOld);
				}
				else
				{
					CBrush brush(GUI::playerColor[p]);
					CBrush* pOldChessBrush = memClientDC.SelectObject(&brush);
					memClientDC.Ellipse(&CRect(xy.x[0] - roundInt(GUI::chessmanRect.x[0] / 2), roundInt(xy.x[1] - GUI::chessmanRect.x[1] / 2), roundInt(xy.x[0] + GUI::chessmanRect.x[0] / 2), xy.x[1] + GUI::chessmanRect.x[1] / 2));
					memClientDC.SelectObject(pOldChessBrush);
				}
			}
		}

		//标记上次走法
		for (int i = actionOfLastRound.size-1; i>=0; --i)
		{
			if ((actionOfLastRound[i].operation == CIGRuleConfig::PUT)||(actionOfLastRound[i].operation == CIGRuleConfig::PICK))
			{
				ChessmanIndex& ci = actionOfLastRound[i].chessmanIndex;
				
				if (actionOfLastRound[i].distination != PointOrVector(-1,-1))
				{
					WrapChessWithFrame(memClientDC, actionOfLastRound[i].distination);
				}
			}
		}
		if (GUI::namedChessman)
		{
			memClientDC.SelectObject(pOldFont);
		}
	}// 画棋子

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memClientDC, 0, 0, SRCCOPY);			//双缓冲绘图

	memClientDC.SelectObject(oldMemBitmap);
}

void CConfigurableIntelligenceGameView::GenerateChessmanDC(CDC& chessmanDC, CBitmap* pChessmanDCBmpOld)
{
}

void CConfigurableIntelligenceGameView::GenerateBoardBaseDC(CDC& boardBaseDC, CBitmap* pBoardBaseDCBmpOld)
{
	boardBaseDC.CreateCompatibleDC(&CClientDC(this));		//创建DC

	CRect rect;
	GetClientRect(&rect);

	CDC latticeDC, addtionalDC;
	latticeDC.CreateCompatibleDC(&boardBaseDC);
	addtionalDC.CreateCompatibleDC(&boardBaseDC);

	CBitmap latticeBitmap, addtionalBitmap;
	latticeBitmap.CreateCompatibleBitmap(&boardBaseDC, rect.Width(), rect.Height());
	addtionalBitmap.CreateCompatibleBitmap(&boardBaseDC, rect.Width(), rect.Height());
	CBitmap* oldLatticeBitmap = latticeDC.SelectObject(&latticeBitmap),
			 *oldAdditionalBitmap = addtionalDC.SelectObject(&addtionalBitmap);

	latticeDC.StretchBlt(0, 0, rect.Width(), rect.Height(), NULL, 0, 0, 0, 0, BLACKNESS);		//纯黑
	addtionalDC.StretchBlt(0, 0, rect.Width(), rect.Height(), NULL, 0, 0, 0, 0, BLACKNESS);		//纯黑

	CPen pen(PS_SOLID, GUI::latticePenWidth, RGB(255, 255, 255));					//用于画格点的画笔.
	CPen* oldLatticePen = latticeDC.SelectObject(&pen),
		  *oldAdditionalPen = addtionalDC.SelectObject(&pen);

	latticeDC.SelectStockObject(NULL_BRUSH);
	addtionalDC.SelectStockObject(NULL_BRUSH);

	if (GUI::markCrossByCircle && GUI::drawLineWhenDrawDot)
	{
		for (int i = 0; i < (1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2); ++i)
		{
			for (int j = 0; j < (1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2); ++j)
			{
				if (CIGRuleConfig::BOARD_RANGE[j][i])			//有这个点
				{
					int x = roundInt(GUI::getGeometryCoordination(i, j).x[0]);
					int y = roundInt(GUI::getGeometryCoordination(i, j).x[1]);			//得到几何坐标

					for(unsigned k = 0 ; k < GUI::LINE_DIRECTION.size(); ++k)			//在所有要划线的方向上
					{
						latticeDC.MoveTo(x, y);
						int x_ = i + roundInt(GUI::LINE_DIRECTION[k].x[0]);
						int y_ = j + roundInt(GUI::LINE_DIRECTION[k].x[1]);

						if (CIGRuleConfig::BOARD_RANGE[y_][x_])				//对应线前方有点
						{
							PointOrVector_Float p = GUI::getGeometryCoordination(x_, y_);
							latticeDC.LineTo(roundInt(p.x[0]), roundInt(p.x[1]));		//画线
						}
					}
				}
			}
		}
	}

	// 在附加平面上画附加线
	for (unsigned i = 0; i < GUI::addtionalLines.size(); ++i)
	{
		int x = roundInt(GUI::getGeometryCoordination(GUI::addtionalLines[i].p0).x[0]);
		int y = roundInt(GUI::getGeometryCoordination(GUI::addtionalLines[i].p0).x[1]);			//得到几何坐标

		int x_ = roundInt(GUI::getGeometryCoordination(GUI::addtionalLines[i].p1).x[0]);
		int y_ = roundInt(GUI::getGeometryCoordination(GUI::addtionalLines[i].p1).x[1]);

		addtionalDC.MoveTo(x, y);
		addtionalDC.LineTo(x_, y_);
	}

	latticeDC.BitBlt(0, 0, rect.Width(), rect.Height(), &addtionalDC, 0, 0, SRCINVERT);		//异或得到画好特殊线的棋盘

	//重新覆盖点, 防止交叉点被异或
	if (GUI::markCrossByCircle)
	{
		for (int i = 0; i < (1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2); ++i)
		{
			for (int j = 0; j < (1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2); ++j)
			{
				if (CIGRuleConfig::BOARD_RANGE[j][i])			//有这个点
				{
					int x = roundInt(GUI::getGeometryCoordination(i, j).x[0]);
					int y = roundInt(GUI::getGeometryCoordination(i, j).x[1]);			//得到几何坐标

					latticeDC.Ellipse(roundInt(x - GUI::markCircleRadias), roundInt(y - GUI::markCircleRadias), roundInt(x + GUI::markCircleRadias), roundInt(y + GUI::markCircleRadias));			//画点
				}
			}
		}
	}

	// 在棋盘平面上画附加点
	for (unsigned i = 0; i < GUI::addtionalPoints.size(); ++i)
	{
		int x = roundInt(GUI::getGeometryCoordination(GUI::addtionalPoints[i]).x[0]);
		int y = roundInt(GUI::getGeometryCoordination(GUI::addtionalPoints[i]).x[1]);			//得到几何坐标

		latticeDC.Ellipse(roundInt(x - GUI::additionalPointRadias), roundInt(y - GUI::additionalPointRadias), roundInt(x + GUI::additionalPointRadias), roundInt(y + GUI::additionalPointRadias));			//画点
	}

	latticeDC.StretchBlt(0, 0, rect.Width(), rect.Height(), NULL, 0, 0, rect.Width(), rect.Height(), DSTINVERT);

	pBoardBaseDCBmpOld = boardBaseDC.SelectObject(&boardBaseBitmap) ;//保存原有DDB，并选入新DDB入DC,

	boardBaseDC.StretchBlt(0, 0, rect.Width(), rect.Height(), &latticeDC, 0, 0, rect.Width(), rect.Height(), SRCAND);
	nBoardBaseDC = boardBaseDC.SaveDC();
	latticeDC.SelectObject(oldLatticePen);
	addtionalDC.SelectObject(oldAdditionalPen);
	latticeDC.SelectObject(oldLatticeBitmap);
	addtionalDC.SelectObject(oldAdditionalBitmap);
}


void CConfigurableIntelligenceGameView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	PointOrVector lp = GUI::getLogicalCoordination(point.x, point.y);

	if (getAction)
	{




		getAction = false;
	}

#ifdef _DEBUG_POINT
	ostringstream oss;
	oss << "lp= (" << lp.x[0] << " , " << lp.x[1] << ")";
	GUI::inform(oss.str());
#endif // _DEBUG_POINT
#ifdef DEBUG_GENERATOR

	for (int j =0;j<CIGRuleConfig::PLAYER_NUM;++j)
	{
		MotionGenerator mg(nowBoard);
		mg.generateActions();

		for (int i = 0; i < mg.actionStack.size ; ++i)
		{
			nowBoard.onActionIntent(mg.actionStack[i],true);
			DrawBoard();
			MessageBox(_T("ok?"));
			nowBoard.undoAction(mg.actionStack[i],true);
		}
		nowBoard.onChangeTurn();
	}
#endif // DEBUG_GENERATOR

	CWnd::OnLButtonDown(nFlags, point);
}
