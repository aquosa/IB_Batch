
// DPS32Feriados.h: archivo de encabezado principal para la aplicación PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "incluir 'stdafx.h' antes de incluir este archivo para PCH"
#endif

#include "resource.h"		// Símbolos principales


// CDPS32FeriadosApp:
// Consulte la sección DPS32Feriados.cpp para obtener información sobre la implementación de esta clase
//

class CDPS32FeriadosApp : public CWinApp
{
public:
	CDPS32FeriadosApp();

// Reemplazos
public:
	virtual BOOL InitInstance();

// Implementación

	DECLARE_MESSAGE_MAP()
};

extern CDPS32FeriadosApp theApp;