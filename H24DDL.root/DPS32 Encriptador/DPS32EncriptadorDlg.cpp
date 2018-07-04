///////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A.
// Dialogo del ENCorizador DPS24
//
// Tarea        Fecha           ENCor   Observaciones
// (1.0.1.0)    2004.07.05      mdc     BASE, como ejemplo al BISEL.
//
// DPS32ENCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DPS32Encriptador.h"
#include "DPS32EncriptadorDlg.h"
#include "Crypt.c"

// Std Library
#include <stdlib.h>
#include <stdio.h>
// Header Local Settings
#include <locale.h>


#include ".\dps32Encriptadordlg.h"


/**********************************************************************************/
// Application instance
extern CDPS32ENCApp  theApp;
/**********************************************************************************/
// Window Proc
extern LONG PASCAL LocalTPWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) ;
/**********************************************************************************/
extern VOID CALLBACK DLGTIMERPROC(HWND hWindow, UINT uTimer, UINT uInt, DWORD uDWord);
/**********************************************************************************/

#ifndef _DPS_ERROR_BLOQUEO_EXCEDE_SALDO_PARA_GIRAR_
#define _DPS_ERROR_BLOQUEO_EXCEDE_SALDO_PARA_GIRAR_ (-1)
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDPS32ENCDlg dialog

CDPS32ENCDlg::CDPS32ENCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDPS32ENCDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDPS32ENCDlg)
	m_usuario = _T("");
	m_password = _T("");
	m_confirmpass = _T("");
	//}}AFX_DATA_INIT
}

void CDPS32ENCDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDPS32ENCDlg)
    DDX_Control(pDX, IDOK, m_buttonOK);
    DDX_Control(pDX, IDCANCEL, m_buttonCancel);
    DDX_Text(pDX, IDC_EDIT_USER, m_usuario);
    DDX_Text(pDX, IDC_EDIT_PASSWORD, m_password);
    DDX_Text(pDX, IDC_EDIT_PASSCONFIRM, m_confirmpass);
    DDX_Text(pDX, IDC_EDIT_USER2, m_usuarioWS);
    DDX_Text(pDX, IDC_EDIT_PASSWORD2, m_passwordWS);
    DDX_Text(pDX, IDC_EDIT_PASSCONFIRM2, m_confirmpassWS);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDPS32ENCDlg, CDialog)
	//{{AFX_MSG_MAP(CDPS32ENCDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDPS32ENCDlg message handlers

BOOL CDPS32ENCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this ENComatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	//////////////////////////////////////////////////////////
	// Posicionar el dialogo en forma aleatoria en la pantalla
	CRect   rect;	
	GetClientRect(&rect);		
	srand( (unsigned int)this->m_hWnd );
	MoveWindow( rect.top  + rand() % rect.right,
				rect.left + rand() % rect.bottom,
				rect.right, 
				rect.bottom 
				+ 20 );/* + title bar */
	//////////////////////////////////////////////////////////

    
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDPS32ENCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is ENComatically done for you by the framework.

void CDPS32ENCDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDPS32ENCDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDPS32ENCDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CDPS32ENCDlg::OnOk() 
{
    // Mensaje MS-Windows
	MSG	msg;

	UpdateData( TRUE );

	if (m_usuario.IsEmpty())
	{
		/* Logout OK si habia sesion previa */
		AfxMessageBox("Debe ingresar un usuario");
		return;
	}

	if (m_password.Compare(m_confirmpass) != 0
		|| m_password.IsEmpty())
	{
		/* Logout OK si habia sesion previa */
		AfxMessageBox("Las contraseñas no coinciden");
		return;
	}

	if (m_usuarioWS.IsEmpty())
	{
		/* Logout OK si habia sesion previa */
		AfxMessageBox("Debe ingresar un usuario del Web Service");
		return;
	}

	if (m_passwordWS.Compare(m_confirmpassWS) != 0
		|| m_passwordWS.IsEmpty())
	{
		/* Logout OK si habia sesion previa */
		AfxMessageBox("Las contraseñas del Web Service no coinciden");
		return;
	}

	
	// Variables locales
	char datos[512]		    = {""};
	char datosEnc[512]     = {""};
	unsigned long cryptLen = 0;

	strcpy(datos, m_usuario.GetBuffer());
	strcat(datos, "/");
	strcat(datos, m_password.GetBuffer());
	strcat(datos, ":");
	strcat(datos, m_usuarioWS.GetBuffer());
	strcat(datos, "/");
	strcat(datos, m_passwordWS.GetBuffer());

	if( ProtectStringValue(datos, datosEnc, &cryptLen) == 1)
	{
		// Abrir archivo de conexion encriptada
		FILE *enc = fopen("conexion.enc", "wb");
		// Escribir
		fwrite(datosEnc, cryptLen, 1, enc);
		// Cerrar
		fclose(enc);
	}
	else
	{
		/* Logout OK si habia sesion previa */
		AfxMessageBox("Fallo la encripcion con estca credencial");
		return;
	}



	if( FALSE ) 
		UnprotectStringValue(datosEnc, cryptLen);

	OnCancel();
}

