
// ConfigurableIntelligenceGameApplicationDoc.cpp : CConfigurableIntelligenceGameApplicationDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "ConfigurableIntelligenceGameApplication.h"
#endif

#include "ConfigurableIntelligenceGameApplicationDoc.h"
#include "GUI.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CConfigurableIntelligenceGameApplicationDoc

IMPLEMENT_DYNCREATE(CConfigurableIntelligenceGameApplicationDoc, CDocument)

BEGIN_MESSAGE_MAP(CConfigurableIntelligenceGameApplicationDoc, CDocument)
END_MESSAGE_MAP()


// CConfigurableIntelligenceGameApplicationDoc ����/����

CConfigurableIntelligenceGameApplicationDoc::CConfigurableIntelligenceGameApplicationDoc()
{
	// TODO: �ڴ����һ���Թ������
}

CConfigurableIntelligenceGameApplicationDoc::~CConfigurableIntelligenceGameApplicationDoc()
{
}

BOOL CConfigurableIntelligenceGameApplicationDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
	{
		return FALSE;
	}

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CConfigurableIntelligenceGameApplicationDoc ���л�

void CConfigurableIntelligenceGameApplicationDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CConfigurableIntelligenceGameApplicationDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// ������������֧��
void CConfigurableIntelligenceGameApplicationDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:  strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CConfigurableIntelligenceGameApplicationDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl* pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);

		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CConfigurableIntelligenceGameApplicationDoc ���

#ifdef _DEBUG
void CConfigurableIntelligenceGameApplicationDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CConfigurableIntelligenceGameApplicationDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CConfigurableIntelligenceGameApplicationDoc ����
