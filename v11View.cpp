#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "v11.h"
#endif

#include "v11Doc.h"
#include "v11View.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cv11View

IMPLEMENT_DYNCREATE(Cv11View, CView)

BEGIN_MESSAGE_MAP(Cv11View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Cv11View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_COLOR, &Cv11View::OnColor)
	ON_COMMAND(ID_SHAPE, &Cv11View::OnShape)
	ON_REGISTERED_MESSAGE(AFX_WM_ON_HIGHLIGHT_RIBBON_LIST_ITEM, HighlightPreview)
END_MESSAGE_MAP()

// Cv11View construction/destruction

Cv11View::Cv11View() {
	this->shape = 0;
	this->color = 0;
	this->shape_prev = 0;
	this->c_prev = 0;
}

Cv11View::~Cv11View()
{
}

BOOL Cv11View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cv11View drawing

void Cv11View::OnDraw(CDC* pDC)
{
	HPEN pen = CreatePen(PS_SOLID, 3, this->color);
	pDC->SelectObject(pen);
	switch (this->shape)
	{
	case 0:
		pDC->Rectangle(this->cr);
		break;
	case 1:
		pDC->Ellipse(this->cr);
		break;
	case 2:
		POINT rrCorners;
		rrCorners.x = 20;
		rrCorners.y = 20;
		pDC->RoundRect(this->cr, rrCorners);
	}
	DeleteObject(pen);
}


// Cv11View printing


void Cv11View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Cv11View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Cv11View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Cv11View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void Cv11View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Cv11View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Cv11View diagnostics

#ifdef _DEBUG
void Cv11View::AssertValid() const
{
	CView::AssertValid();
}

void Cv11View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cv11Doc* Cv11View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cv11Doc)));
	return (Cv11Doc*)m_pDocument;
}
#endif //_DEBUG


// Cv11View message handlers



void Cv11View::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);
	CRectTracker tr;
	tr.TrackRubberBand(this, point, true);
	cr = tr.m_rect;
	Invalidate(true);
}





void Cv11View::OnColor()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_COLOR, arr);
	CMFCRibbonColorButton* pColor = (CMFCRibbonColorButton*)arr.GetAt(0);
	this->color = pColor->GetColor();
	this->c_prev = pColor->GetColor();
	Invalidate(true);
}


void Cv11View::OnShape()
{
	
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
	((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_SHAPE, arr);
	CMFCRibbonGallery* pGallery = (CMFCRibbonGallery*)arr.GetAt(0);
	this->shape = pGallery->GetSelectedItem();
	this->shape_prev = pGallery->GetSelectedItem();
	Invalidate(true);
}

LRESULT Cv11View::HighlightPreview(WPARAM wparam, LPARAM lparam)
{
	int index = (int)wparam;
	CMFCRibbonBaseElement* pElem = (CMFCRibbonBaseElement*)lparam;
	CMFCRibbonColorButton* pColor = (CMFCRibbonColorButton*)lparam;

	switch (pElem->GetID())
	{
	case ID_SHAPE:
		if (index != -1) 
			this->shape = index;
		else 
			this->shape = this->shape_prev;
		break;

	case ID_COLOR:
		if (index != -1) 
			this->color = pColor->GetHighlightedColor();
		else 
			this->color = this->c_prev;
		break;
	}

	Invalidate();
	return 0;
}
