// FuegoDPS.h: archivo de encabezado principal para la aplicaci�n PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error incluye 'stdafx.h' antes de incluir este archivo para PCH
#endif

#include "resource.h"		// S�mbolos principales


// CFuegoDPSApp:
// Consulte la secci�n FuegoDPS.cpp para obtener informaci�n sobre la implementaci�n de esta clase
//

class CFuegoDPSApp : public CWinApp
{
public:
	CFuegoDPSApp();

// Reemplazos
	public:
	virtual BOOL InitInstance();

// Implementaci�n

	DECLARE_MESSAGE_MAP()
};

extern CFuegoDPSApp theApp;
