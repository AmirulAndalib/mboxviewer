//
//////////////////////////////////////////////////////////////////
//
//  Windows Mbox Viewer is a free tool to view, search and print mbox mail archives.
//
// Source code and executable can be downloaded from
//  https://sourceforge.net/projects/mbox-viewer/  and
//  https://github.com/eneam/mboxviewer
//
//  Copyright(C) 2019  Enea Mansutti, Zbigniew Minciel
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the version 3 of GNU Affero General Public License
//  as published by the Free Software Foundation; 
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU
//  Library General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public
//  License along with this program; if not, write to the
//  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
//  Boston, MA  02110 - 1301, USA.
//
//////////////////////////////////////////////////////////////////
//

// Browser.cpp : implementation file
//

#include "stdafx.h"
#include "Browser.h"
#include "mainfrm.h"
#include "resource.h"
#include "FileUtils.h"
#include "HtmlUtils.h"
#include <atlconv.h>
#include "mboxview.h"

#ifdef _DEBUG
#undef THIS_FILE
#define THIS_FILE __FILE__
#define new DEBUG_NEW
#endif

void Com_Initialize();

void SaveIESettings(bool save = true);
/////////////////////////////////////////////////////////////////////////////
// CBrowser

CBrowser::CBrowser()
{
	m_hWndParent = NULL;
	m_bDidNavigate = false;
	m_bDops = false;
	m_bNavigateComplete = TRUE;
}

BEGIN_MESSAGE_MAP(CBrowser, CWnd)
	//{{AFX_MSG_MAP(CBrowser)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_CMD_PARAM_ON_SWITCH_WINDOW_MESSAGE, &CBrowser::OnCmdParam_OnSwitchWindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBrowser message handlers

BOOL CBrowser::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, //|CS_PARENTDC, 
		//::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);

	return TRUE;
}

int CBrowser::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;

	//Com_Initialize();
	
	if (!m_ie.Create(NULL, WS_VISIBLE | WS_CHILD, CRect(), this, IDC_EXPLORER))
		return -1;

	// It appears m_ie.Create  calls CoInitilize
	//Com_Initialize();

	return 0;
}

void CBrowser::OnSize(UINT nType, int cx, int cy) 
{
	CWnd ::OnSize(nType, cx, cy);
	
	m_ie.MoveWindow( 0, 0, cx, cy );
}

void CBrowser::Navigate(LPCWSTR url, DWORD flags)
{
	COleVariant* pvarURL = new COleVariant( url );
	COleVariant* pvarEmpty = new COleVariant;
	COleVariant* pvarFlags = new COleVariant((long)(flags), VT_I4);
	_ASSERTE(pvarURL);
	_ASSERTE(pvarEmpty);
	_ASSERTE(pvarFlags);
	m_url = url;
	TRACE(L"CBrowser::Navigate(%s)... m_bNavigateComplete(%d)\n", url, m_bNavigateComplete);
	m_bNavigateComplete = FALSE;
	//m_ie.Stop();
	m_ie.Navigate2( pvarURL, pvarFlags, pvarEmpty, pvarEmpty, pvarEmpty );
	CString loc = m_ie.GetLocationURL();
	if (loc.CompareNoCase(url))
		int deb = 1;
	TRACE(L"Done.\n");
	delete pvarURL;
	delete pvarEmpty;
	delete pvarFlags;
	return;
}

BEGIN_EVENTSINK_MAP(CBrowser, CWnd)
    //{{AFX_EVENTSINK_MAP(CBrowser)
	ON_EVENT(CBrowser, IDC_EXPLORER, 259 /* DocumentComplete */, OnDocumentCompleteExplorer, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CBrowser, IDC_EXPLORER, 250 /* BeforeNavigate */, BeforeNavigate, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()
// DISPID_DOWNLOADCOMPLETE

void CBrowser::OnDocumentCompleteExplorer(LPDISPATCH pDisp, VARIANT FAR* URL) 
{
	IUnknown*  pUnk;
	LPDISPATCH lpWBDisp;
	HRESULT    hr;

	CString strURL = V_BSTR(URL);
	CString locURL = m_ie.GetLocationURL();
	
	TRACE(L"CBrowser::OnDocumentCompleteExplorer(%s)... m_bNavigateComplete(%d) currURL(%s) locURL(%s)\n",
		strURL, m_bNavigateComplete, m_url, locURL);

	pUnk = m_ie.GetControlUnknown();
	_ASSERTE(pUnk);

	hr = pUnk->QueryInterface(IID_IDispatch, (void**)&lpWBDisp);
	_ASSERTE(SUCCEEDED(hr));

	if (lpWBDisp) {
		if (pDisp == lpWBDisp)
		{

		}
		lpWBDisp->Release();
	}
	m_bNavigateComplete = TRUE;

	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	if (!pFrame)
		return;
	if (!::IsWindow(pFrame->m_hWnd) || !pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)))
		return;
	NMsgView *pMsgView = pFrame->GetMsgView();
	if (!pMsgView || !::IsWindow(pMsgView->m_hWnd))
		return;

	NListView *pListView = pFrame->GetListView();
	if (!pListView)
		return;

	if (pListView->m_bHighlightAllSet) {
		pMsgView->FindStringInIHTMLDocument(pListView->m_searchString, pListView->m_bWholeWord, pListView->m_bCaseSens);
	}
	pListView->m_bHighlightAllSet = FALSE;
#if 0
	// Already set by Command UI in void CMainFrame::OnUpdateMailDownloadStatus(CCmdUI *pCmdUI)
	if (pFrame) {
		CString sText = L"Ready";
		int paneId = 0;
		pFrame->SetStatusBarPaneText(paneId, sText, FALSE);
	}
#endif

#if 0
	// Best if user usees CTL+mouse scroll to control font size
	static int done = 0;
	if (!done)
	{
		int fontSize = 160;
		HtmlUtils::SetFontSize_Browser(*this, fontSize);
		done = 1;
	}
#endif
}

void CBrowser::BeforeNavigate(LPDISPATCH pDisp /* pDisp */, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName,
		VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	_ASSERTE(V_VT(URL) == VT_BSTR);
	_ASSERTE(Cancel != NULL);
	USES_CONVERSION;

	CString strURL = V_BSTR(URL);
	CString locURL = m_ie.GetLocationURL();

	TRACE(L"CBrowser::BeforeNavigate(%s)... m_bNavigateComplete(%d) currURL(%s) locURL(%s)\n",
		strURL, m_bNavigateComplete, m_url, locURL);

	m_bDidNavigate = true;
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	if( ! pFrame )
		return;
	if( ! ::IsWindow(pFrame->m_hWnd) || ! pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) )
		return;
	NMsgView *pMsgView = pFrame->GetMsgView();
	if( ! pMsgView || ! ::IsWindow(pMsgView->m_hWnd))
		return;
	pMsgView->PostMessage(WM_PAINT);
	if (Cancel)
		*Cancel = FALSE;

	// Open link clicked by a user in the external browser
	// TODO: Best effort. Microsoft recommded solution not clear.
	// May need to intercept mouse click, update doc to attach action events to all links ?

	CString url = L"about:blank";
	DWORD color = CMainFrame::m_ColorStylesDB.m_colorStyles.GetColor(ColorStyleConfig::MailMessage);

	{
		CString colorStr;
		int retC2A = NListView::Color2Str(color, colorStr);

		url = L"about:<html><head><style>body{background-color: #";
		url.Append(colorStr);
		url.Append(L";}</style></head><body></body></html><br>");
	}

	if ((strURL.CompareNoCase(url) != 0) 
		&& !FileUtils::PathFileExist(strURL)) {
		if (Cancel)
			*Cancel = TRUE;
		//IDispatch *api = pDisp;
		//m_ie.Stop();
		ShellExecute(0, NULL, strURL, NULL, NULL, SW_SHOWDEFAULT);
	}

	return;
}

#include <mshtmdid.h>


BOOL CBrowser::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar) 
{
USES_CONVERSION;
	// Change download properties - no java, no scripts...
	if (dispid == DISPID_AMBIENT_DLCONTROL)
	{
		pvar->vt = VT_I4;
		pvar->lVal = DLCTL_DLIMAGES | DLCTL_VIDEOS | DLCTL_BGSOUNDS | DLCTL_NO_SCRIPTS | DLCTL_NO_JAVA | DLCTL_NO_DLACTIVEXCTLS | DLCTL_NO_RUNACTIVEXCTLS | DLCTL_SILENT;

		return TRUE;
	}

	return CWnd::OnAmbientProperty(pSite, dispid, pvar);
}

LRESULT CBrowser::OnCmdParam_OnSwitchWindow(WPARAM wParam, LPARAM lParam)
{
	CMainFrame::SetWindowFocus(&m_ie);
	CmboxviewApp::wndFocus = &m_ie;
	return 0;
}
