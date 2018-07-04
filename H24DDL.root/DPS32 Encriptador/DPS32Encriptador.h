// DPS32ENC.h : main header file for the DPS32ENC application
//

#if !defined(AFX_DPS32ENC_H__DEADECD8_B48E_492D_9EA9_6A67E25B314C__INCLUDED_)
#define AFX_DPS32ENC_H__DEADECD8_B48E_492D_9EA9_6A67E25B314C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDPS32ENCApp:
// See DPS32ENC.cpp for the implementation of this class
//

class CDPS32ENCApp : public CWinApp
{
public:
	CDPS32ENCApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDPS32ENCApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDPS32ENCApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DPS32ENC_H__DEADECD8_B48E_492D_9EA9_6A67E25B314C__INCLUDED_)
