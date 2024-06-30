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

#if !defined(AFX_FINDDLG_H__139BBE54_571D_4C79_A9CD_B6035D054686__INCLUDED_)
#define AFX_FINDDLG_H__139BBE54_571D_4C79_A9CD_B6035D054686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindDlg.h : header file
//

#include "ATLComTime.h"
#include "MyCTime.h"


/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog

struct CFindDlgParams
{
	CFindDlgParams();
	~CFindDlgParams();

	CString	m_string;
	CStringA	m_stringA;
	BOOL	m_bWholeWord;
	BOOL	m_bCaseSensitive;

	COleDateTime m_startDate;
	COleDateTime m_endDate;
	BOOL m_filterDates;

	BOOL m_bFindNext;
	BOOL m_bFrom;
	BOOL m_bTo;
	BOOL m_bSubject;
	BOOL m_bContent;
	BOOL m_bCC;
	BOOL m_bBCC;
	BOOL m_bAttachments;
	BOOL m_bAttachmentName;
	BOOL m_bHighlightAll;
	BOOL m_bFindAll;
	BOOL m_bFindAllMailsThatDontMatch;  // find all mails that didn't match

	// Run time args
	// Following vars are not set or updated by FindDlg dialog
	// They are to help to maintain proper state
	BOOL m_bNeedToFindMailMinMaxTime;
	MyCTime m_mboxMailStartDate;
	MyCTime m_mboxMailEndDate;
	//
	BOOL m_needToRestoreArchiveListDateTime;
	MyCTime m_lastStartDate;
	MyCTime m_lastEndDate;
	CString m_dateTimeFormat;

	void SetDflts();
	void Copy(CFindDlgParams &src);
	void ResetFilterDates();
};

class CFindDlg : public CDialog
{
// Construction
public:
	CFindDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindDlg)
	enum { IDD = IDD_FIND };

	struct CFindDlgParams m_params;

	COLORREF m_dflBkColor;
	COLORREF m_checkedColor;
	CBrush m_brBkDate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedFilterDates();
	afx_msg void OnBnClickedCheckFindAll();
	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonHelp();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDLG_H__139BBE54_571D_4C79_A9CD_B6035D054686__INCLUDED_)
