
// CConfigurableIntelligenceGameView.cpp : CConfigurableIntelligenceGameView ���ʵ��
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
	//����ĳ�ʼ�����ҵ�. ���Ի����ڱ��ٳ�ʼ��һ�ΰ�.

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



// CConfigurableIntelligenceGameView ��Ϣ��������

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
	CPaintDC memClientDC(this); // ���ڻ��Ƶ��豸������

	// TODO: �ڴ˴�������Ϣ�����������

	DrawBoard();

	GUI::drawComplete.SetEvent();
}

void CConfigurableIntelligenceGameView::OnGameNew()
{
	// TODO: �ڴ�����������������

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
	MessageBox((LPCTSTR)_T("�����Ѿ��õ��߷�. "));

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
		ires = RegQueryValueEx(hMyKey, _T("΢���ź� Bold (TrueType)"), 0, &Type, mstr, &count);
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

	//������
	{
		CDC boardBaseDC;
		CBitmap* pBoardBaseDCBmpOld = NULL;
		GenerateBoardBaseDC(boardBaseDC, pBoardBaseDCBmpOld);
		boardBaseDC.SelectObject(pBoardBaseDCBmpOld) ;//ѡ��ԭDDB

		memClientDC.BitBlt(0, 0, rect.Width(), rect.Height(), &boardBaseDC, 0, 0, SRCCOPY) ; //��ԴDC��(0,0,20,20)���Ƶ�Ŀ��DC
	}

	//������
	{
		CFont font;
		font.CreatePointFont(roundInt(0.5 * 10 * min(GUI::chessmanRect.x[0], GUI::chessmanRect.x[1])), _T("�����п�"), NULL);
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
	boardBaseDC.CreateCompatibleDC(&CClientDC(this));		//����DC

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

	latticeDC.StretchBlt(0, 0, rect.Width(), rect.Height(), NULL, 0, 0, 0, 0, BLACKNESS);		//����
	addtionalDC.StretchBlt(0, 0, rect.Width(), rect.Height(), NULL, 0, 0, 0, 0, BLACKNESS);		//����

	CPen pen(PS_SOLID, GUI::latticePenWidth, RGB(255, 255, 255));					//���ڻ����Ļ���.
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
				if (CIGRuleConfig::BOARD_RANGE[j][i])			//�������
				{
					int x = roundInt(GUI::getGeometryCoordination(i, j).x[0]);
					int y = roundInt(GUI::getGeometryCoordination(i, j).x[1]);			//�õ���������

					for(unsigned k = 0 ; k < GUI::LINE_DIRECTION.size(); ++k)			//������Ҫ���ߵķ�����
					{
						latticeDC.MoveTo(x, y);
						int x_ = i + roundInt(GUI::LINE_DIRECTION[k].x[0]);
						int y_ = j + roundInt(GUI::LINE_DIRECTION[k].x[1]);

						if (CIGRuleConfig::BOARD_RANGE[y_][x_])				//��Ӧ��ǰ���е�
						{
							PointOrVector_Float p = GUI::getGeometryCoordination(x_, y_);
							latticeDC.LineTo(roundInt(p.x[0]), roundInt(p.x[1]));		//����
						}
					}
				}
			}
		}
	}

	for (unsigned i = 0; i < GUI::addtionalLines.size(); ++i)
	{
		int x = roundInt(GUI::getGeometryCoordination(GUI::addtionalLines[i].p0).x[0]);
		int y = roundInt(GUI::getGeometryCoordination(GUI::addtionalLines[i].p0).x[1]);			//�õ���������

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
				if (CIGRuleConfig::BOARD_RANGE[j][i])			//�������
				{
					int x = roundInt(GUI::getGeometryCoordination(i, j).x[0]);
					int y = roundInt(GUI::getGeometryCoordination(i, j).x[1]);			//�õ���������

					latticeDC.Ellipse(roundInt(x - GUI::dotRadias), roundInt(y - GUI::dotRadias), roundInt(x + GUI::dotRadias), roundInt(y + GUI::dotRadias));			//����
				}
			}
		}
	}

	latticeDC.StretchBlt(0, 0, rect.Width(), rect.Height(), NULL, 0, 0, rect.Width(), rect.Height(), DSTINVERT);

	pBoardBaseDCBmpOld = boardBaseDC.SelectObject(&boardBaseBitmap) ;//����ԭ��DDB����ѡ����DDB��DC,

	boardBaseDC.StretchBlt(0, 0, rect.Width(), rect.Height(), &latticeDC, 0, 0, rect.Width(), rect.Height(), SRCAND);
	nBoardBaseDC = boardBaseDC.SaveDC();
	latticeDC.SelectObject(oldLatticePen);
	addtionalDC.SelectObject(oldAdditionalPen);
	latticeDC.SelectObject(oldLatticeBitmap);
	addtionalDC.SelectObject(oldAdditionalBitmap);
}

////ͼ�����͸��. ��������ʱ����ͼ���ǲ����Ե�. Ϊ������ͼƬ���Զ�����, ���Ի�����λͼ��.
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

	// TODO:  �ڴ�������ר�õĴ�������CIG::GUI::cigView = this;
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
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ

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
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
	
	m_GameThread->PostThreadMessage(WM_QUIT,0,0);

	CWnd::OnClose();
}