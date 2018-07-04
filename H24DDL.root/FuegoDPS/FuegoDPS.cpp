/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// DATANET-BATCH D.O.S. program for TDF - Batch interfases to INTERBANKING
//
// Tarea        Fecha           Autor   Observaciones
// [ALFA]       2006.09.18      mdc     Inicial basado en Finansur Batch DPS.
// [ALFA]       2012.12.04      mdc     FIX .NET 2010, STACK CORRUPTED
//
//
// FuegoDPS.cpp : define los comportamientos de las clases para la aplicaci�n.
//

#include "stdafx.h"
#include "FuegoDPS.h"
#include "FuegoDPSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFuegoDPSApp

BEGIN_MESSAGE_MAP(CFuegoDPSApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// Construcci�n de CFuegoDPSApp

CFuegoDPSApp::CFuegoDPSApp()
{
	// TODO: agregar aqu� el c�digo de construcci�n,
	// Colocar toda la inicializaci�n importante en InitInstance
}


// El �nico objeto CFuegoDPSApp

CFuegoDPSApp theApp;


// Inicializaci�n de CFuegoDPSApp

BOOL CFuegoDPSApp::InitInstance()
{
	// Windows XP requiere InitCommonControls() si un manifiesto de 
	// aplicaci�n especifica el uso de ComCtl32.dll versi�n 6 o posterior para habilitar
	// estilos visuales. De lo contrario, se generar� un error al crear ventanas.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	//////////////////////////////////////////////////////////////////////////////
	m_pszAppName = strdup("DPS32"); /* strdup */
	SetRegistryKey(_T("DATANET"));		
	if(m_pMainWnd)
		WriteProfileInt("DATANET","HWND"     , (DWORD)m_pMainWnd->m_hWnd );
	WriteProfileInt("DATANET","ProcessID", GetCurrentProcessId() );
	WriteProfileInt("DATANET","ThreadID" , GetCurrentThreadId() );
	//////////////////////////////////////////////////////////////////////////////

	CFuegoDPSDlg dlg;
	m_pMainWnd = &dlg;


	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: insertar aqu� el c�digo para controlar 
		//  cu�ndo se descarta el cuadro de di�logo con Aceptar
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: insertar aqu� el c�digo para controlar 
		//  cu�ndo se descarta el cuadro de di�logo con Cancelar
	}

	// Dado que el cuadro de di�logo se ha cerrado, devolver FALSE para salir
	//  de la aplicaci�n en vez de iniciar el suministro de mensajes de dicha aplicaci�n.
	return FALSE;
}
