
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

using namespace CIG;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CConfigurableIntelligenceGameView::CConfigurableIntelligenceGameView(): nowBoard(), actionOfLastRound("ActionOfLastRound")
{
	//这里的初始化会乱掉. 所以还是在别处再初始化一次吧.

}

CConfigurableIntelligenceGameView::~CConfigurableIntelligenceGameView()
{
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
	ON_WM_CLOSE()
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
	eventThreadMessageOK.Lock();

	PostMessage(WM_PAINT, 0, 0);
	m_GameThread->PostThreadMessage(WM_GET_MOVE, (WPARAM)&actionOfLastRound, (LPARAM)&nowBoard);
}


afx_msg LRESULT CConfigurableIntelligenceGameView::OnMoveComplete(WPARAM wParam, LPARAM lParam)
{
	GUI::inform("OnMoveComplete");
	PostMessage(WM_PAINT, 0,0);
	m_GameThread->PostThreadMessage(WM_GET_MOVE, (WPARAM)&actionOfLastRound, (LPARAM)&nowBoard);

	return 0;
}

afx_msg LRESULT CConfigurableIntelligenceGameView::OnGetMove(WPARAM wParam, LPARAM lParam)
{
	getAction = true;
	MessageBox((LPCTSTR)_T("假设已经得到走法. "));

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

void CConfigurableIntelligenceGameView::DrawBoard()
{
	CClientDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	CDC memClientDC;
	memClientDC.CreateCompatibleDC(&dc);
	CBitmap memBitmap;
	memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap* oldMemBitmap = memClientDC.SelectObject(&memBitmap);

	memClientDC.SetBkMode(TRANSPARENT);

	//画棋盘
	{
		CDC boardBaseDC;
		CBitmap* pBoardBaseDCBmpOld = NULL;
		GenerateBoardBaseDC(boardBaseDC, pBoardBaseDCBmpOld);
		boardBaseDC.SelectObject(pBoardBaseDCBmpOld) ;//选入原DDB

		memClientDC.BitBlt(0, 0, rect.Width(), rect.Height(), &boardBaseDC, 0, 0, SRCCOPY) ; //将源DC中(0,0,20,20)复制到目的DC
	}

	//画棋子
	{
		CFont font;
		font.CreatePointFont(roundInt(0.5 * 10 * min(GUI::chessmanRect.x[0], GUI::chessmanRect.x[1])), _T("华文行楷"), NULL);
		memClientDC.SetBkMode(TRANSPARENT);
		memClientDC.SetStretchBltMode(COLORONCOLOR);

		CFont* oldFont = memClientDC.SelectObject(&font);

		for (int p = 0; p < CIGRuleConfig::PLAYER_NUM; ++p)
		{

			for (int c = nowBoard.players[p].ownedChessmans.size - 1; c >= 0; --c)
			{
				if (nowBoard.players[p].ownedChessmans[c].status==CIGRuleConfig::CAPTURED)
				{
					continue;
				}
				CDC chessDC;
				chessDC.CreateCompatibleDC(&memClientDC);

				CBitmap* pBoardBaseDCBmpOld = chessDC.SelectObject(&chessmanBaseBitmap);
				PointOrVector_Float xy = GUI::getGeometryCoordination(nowBoard.players[p].ownedChessmans[c].coordinate);

				BITMAP bm;
				chessmanBaseBitmap.GetBitmap(&bm);

				TransparentBlt2(memClientDC.m_hDC, xy.x[0] - roundInt(GUI::chessmanRect.x[0] / 2), xy.x[1] - roundInt(GUI::chessmanRect.x[1] / 2), GUI::chessmanRect.x[0], GUI::chessmanRect.x[1], chessDC.m_hDC, 0, 0, bm.bmWidth, bm.bmHeight, RGB(0, 255, 0));

				memClientDC.SetTextColor(GUI::playerColor[p]);
				memClientDC.DrawText(GUI::chessmanName[nowBoard.players[p].ownedChessmans[c].chessmanType], &CRect(xy.x[0] - roundInt(GUI::chessmanRect.x[0] / 2), roundInt(xy.x[1] - GUI::chessmanRect.x[1] / 2), roundInt(xy.x[0] + GUI::chessmanRect.x[0] / 2), xy.x[1] + GUI::chessmanRect.x[1] / 2), DT_SINGLELINE | DT_CENTER | DT_VCENTER);

				chessDC.SelectObject(oldFont);
			}
		}

		memClientDC.SelectObject(oldFont);
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memClientDC, 0, 0, SRCCOPY);

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

	if (GUI::drawCross && GUI::drawLineWhenDrawDot)
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

	for (unsigned i = 0; i < GUI::addtionalLines.size(); ++i)
	{
		int x = roundInt(GUI::getGeometryCoordination(GUI::addtionalLines[i].p0).x[0]);
		int y = roundInt(GUI::getGeometryCoordination(GUI::addtionalLines[i].p0).x[1]);			//得到几何坐标

		int x_ = roundInt(GUI::getGeometryCoordination(GUI::addtionalLines[i].p1).x[0]);
		int y_ = roundInt(GUI::getGeometryCoordination(GUI::addtionalLines[i].p1).x[1]);

		addtionalDC.MoveTo(x, y);
		addtionalDC.LineTo(x_, y_);
	}

	latticeDC.BitBlt(0, 0, rect.Width(), rect.Height(), &addtionalDC, 0, 0, SRCINVERT);

	if (GUI::drawCross)
	{
		for (int i = 0; i < (1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2); ++i)
		{
			for (int j = 0; j < (1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2); ++j)
			{
				if (CIGRuleConfig::BOARD_RANGE[j][i])			//有这个点
				{
					int x = roundInt(GUI::getGeometryCoordination(i, j).x[0]);
					int y = roundInt(GUI::getGeometryCoordination(i, j).x[1]);			//得到几何坐标

					latticeDC.Ellipse(roundInt(x - GUI::dotRadias), roundInt(y - GUI::dotRadias), roundInt(x + GUI::dotRadias), roundInt(y + GUI::dotRadias));			//画点
				}
			}
		}
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

////图标可以透明. 但是运行时生成图标是不可以的. 为了棋子图片的自动生成, 所以还是用位图吧.
//HICON hIcon1=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//
//dc.DrawIcon(0,0,hIcon1);
//DestroyIcon(hIcon1);


int CConfigurableIntelligenceGameView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// TODO:  在此添加您专用的创建代码CIG::GUI::cigView = this;
	m_GameThread = AfxBeginThread(CIG::GUI::runThread, this);
	eventThreadMessageOK.Lock();
	getAction = false;

	CClientDC memDC(this);
	boardBaseBitmap.LoadBitmap(IDB_BOARD_BASE);
	chessmanBaseBitmap.LoadBitmap((GUI::roundChessman) ? IDB_CHESSMAN_ROUND : IDB_CHESSMAN_RECTANGLE);

	input = (CIGRuleConfig::PLAYER_NAMES)-1;

	return 0;
}


void CConfigurableIntelligenceGameView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

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
	PointOrVector lp = GUI::getLogicalCoordination(point.x, point.y);

	nowBoard.nowTurn = CIGRuleConfig::COMPUTER;

	for (;;)
	{
		MotionGenerator mg(nowBoard);
		mg.generateMotionsAndBoards();

		for (int i = 0; i < mg.chessboardStack.size ; ++i)
		{
			nowBoard = mg.chessboardStack[i];
			PostMessage(WM_PAINT, 0, 0);
			GUI::drawComplete.Lock();
			MessageBox(_T("ok?"));
		}
		nowBoard.nowTurn = (CIGRuleConfig::PLAYER_NAMES)(1-nowBoard.nowTurn);
	}
#endif // DEBUG_GENERATOR

	CWnd::OnLButtonDown(nFlags, point);
}


void CConfigurableIntelligenceGameView::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	m_GameThread->PostThreadMessage(WM_QUIT,0,0);

	CWnd::OnClose();
}
