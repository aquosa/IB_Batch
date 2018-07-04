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
// FuegoDPS.cpp : define los comportamientos de las clases para la aplicación.
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


// Construcción de CFuegoDPSApp

CFuegoDPSApp::CFuegoDPSApp()
{
	// TODO: agregar aquí el código de construcción,
	// Colocar toda la inicialización importante en InitInstance
}


// El único objeto CFuegoDPSApp

CFuegoDPSApp theApp;


// Inicialización de CFuegoDPSApp

BOOL CFuegoDPSApp::InitInstance()
{
	// Windows XP requiere InitCommonControls() si un manifiesto de 
	// aplicación especifica el uso de ComCtl32.dll versión 6 o posterior para habilitar
	// estilos visuales. De lo contrario, se generará un error al crear ventanas.
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
		// TODO: insertar aquí el código para controlar 
		//  cuándo se descarta el cuadro de diálogo con Aceptar
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: insertar aquí el código para controlar 
		//  cuándo se descarta el cuadro de diálogo con Cancelar
	}

	// Dado que el cuadro de diálogo se ha cerrado, devolver FALSE para salir
	//  de la aplicación en vez de iniciar el suministro de mensajes de dicha aplicación.
	return FALSE;
}
