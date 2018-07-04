// FuegoDPS.h: archivo de encabezado principal para la aplicación PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error incluye 'stdafx.h' antes de incluir este archivo para PCH
#endif

#include "resource.h"		// Símbolos principales


// CFuegoDPSApp:
// Consulte la sección FuegoDPS.cpp para obtener información sobre la implementación de esta clase
//

class CFuegoDPSApp : public CWinApp
{
public:
	CFuegoDPSApp();

// Reemplazos
	public:
	virtual BOOL InitInstance();

// Implementación

	DECLARE_MESSAGE_MAP()
};

extern CFuegoDPSApp theApp;
