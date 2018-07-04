////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 ASAP S.L. / IT24 OUTSOURCING SRL / COA SA
// LOG generico - Proyecto SIAF-BANSUD originalmente.
//   
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2002.07.11      gsi     Log para SIAF , armado de la clase y metodos principales
// (Inicial)    2002.08.07      mdc     Varias correcciones
// (Inicial)    2002.10.30      mdc     BOOL SetTodayFileName(LPCSTR pszInFName, LPSTR pszOutFName);
// (Inicial)    2003.07.03      mdc     boolean_t SetTodayFileName(....). Otros proyectos.
// [Beta]       2008.08.27      mdc     FIX VISUAL .NET 2005, requiere si o si retornar un tipo de dato especifico o sino VOID
//


#ifndef _GENERIC_LOG_H_
#define _GENERIC_LOG_H_

#include <stdio.h>
#include <qusrinc/alias.h>
#include <qusrinc/typedefs.h>


class GenericLog {
	public:
		// Constructor
		EXPORT_ATTR  GenericLog();
		// Destructor		
		EXPORT_ATTR  ~GenericLog();

		// Metodos generales de la clase
		// FIX VISUAL .NET 2005, requiere si o si retornar un tipo de dato especifico o sino VOID
		void EXPORT_ATTR  Log();		
		int EXPORT_ATTR  Open(const char *logname);
		int EXPORT_ATTR  Put(char *fmt, ...);
		int EXPORT_ATTR  DraftPut(char *fmt, ...);
		void EXPORT_ATTR  Close();

	protected:
		
		boolean_t SetTodayFileName(LPCSTR pszInFName, LPSTR pszOutFName);
		// Variables del archivo Principal
 		FILE *fLog_fd;
};

#endif // GENERICLOG_H



