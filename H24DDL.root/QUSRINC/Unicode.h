//
// IT24 SISTEMAS S.A.
// AlphaUnicode Utilities
// 1998.12.07 mdc Base
// 
//

#ifndef UNICODE_H
#define UNICODE_H

// Alias de Extensiones Propietarias (segun cada compilador)
#ifdef EXPORT_PROC
#undef EXPORT_PROC
#undef IMPORT_PROC
#endif // EXPORT
#ifdef _WIN32
#define EXPORT_PROC    __stdcall  // MS VC/C++ 5.0
#else
#define EXPORT_PROC    __export   // MS C/C++ 1.5
#endif

#if defined(__cplusplus)
#define EXTERN extern "C"
#else
#define EXTERN extern 
#endif // __cplusplus

EXTERN int EXPORT_PROC ALPHA_UNICODE(char *,int,char *,int *) ;
EXTERN int EXPORT_PROC ALPHA_REV_UNICODE(char *sstr,int,char *,int *);
EXTERN int EXPORT_PROC ALPHA_REV_UNICODE_BIN(char *,int,long *);

#endif // UNICODE_H