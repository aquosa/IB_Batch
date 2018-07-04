///////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A.
// Dialogo del Autorizador DPS de Banco Macro-Bansud
//
// Tarea        Fecha           Autor   Observaciones
// (1.0.1.0)    2007.08.15      mdc     BASE, como ejemplo al BISEL

//
// DPS32ENCDlg.cpp : implementation file
//
// DPS32ENCDlg.h : header file
//
//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#if !defined(AFX_DPS32ENCDLG_H__ABF838EB_E5CD_42FC_AAC5_D87FDF327EE2__INCLUDED_)
#define AFX_DPS32ENCDLG_H__ABF838EB_E5CD_42FC_AAC5_D87FDF327EE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDPS32ENCDlg dialog

class CDPS32ENCDlg : public CDialog
{
// Construction
public:
	CDPS32ENCDlg(CWnd* pParent = NULL);	// standard constructor

	/**********************************************************************************/
	VOID CALLBACK TimerProc(HWND hWindow, UINT uTimer, UINT uInt, DWORD uDWord);
	/**********************************************************************************/

// Dialog Data
	//{{AFX_DATA(CDPS32ENCDlg)
	enum { IDD = IDD_DPS32ENC_DIALOG };
	CDateTimeCtrl	m_timeCutOver;
	CButton	m_buttonOK;
	CButton	m_buttonCancel;
	CString	m_usuario;
	CString	m_password;
	CString m_confirmpass;
	CString	m_usuarioWS;
	CString	m_passwordWS;
	CString m_confirmpassWS;

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDPS32ENCDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	HICON m_hIcon;

    // Generated message map functions
	//{{AFX_MSG(CDPS32ENCDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOk();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedCheckLogin();
    long m_nSafDia;
    long m_nSafDiferido;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DPS32ENCDLG_H__ABF838EB_E5CD_42FC_AAC5_D87FDF327EE2__INCLUDED_)
