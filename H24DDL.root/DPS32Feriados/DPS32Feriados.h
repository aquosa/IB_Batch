
// DPS32Feriados.h: archivo de encabezado principal para la aplicaci�n PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "incluir 'stdafx.h' antes de incluir este archivo para PCH"
#endif

#include "resource.h"		// S�mbolos principales


// CDPS32FeriadosApp:
// Consulte la secci�n DPS32Feriados.cpp para obtener informaci�n sobre la implementaci�n de esta clase
//

class CDPS32FeriadosApp : public CWinApp
{
public:
	CDPS32FeriadosApp();

// Reemplazos
public:
	virtual BOOL InitInstance();

// Implementaci�n

	DECLARE_MESSAGE_MAP()
};

extern CDPS32FeriadosApp theApp;