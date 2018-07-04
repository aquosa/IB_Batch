//
// IT24 SISTEMAS S.A.
// C/C++ Reserved Word Set Aliases
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1997.07.24      mdc     Armado inicial
// (Inicial)    1997.08.04      mdc     Agregado VIRTUAL
// (Inicial)    1997.08.05      mdc     Agregado EXPORT
// (Inicial)    1997.08.07      mdc     Agregado INLINE
// (WindowsNT)  1997.09.05      mdc     _WIN32
// (Version)    1997.09.16      mdc     Agregado FRIEND
// (Version)    1997.10.24      mdc     Agregado CONST
//

#ifndef _ALIAS_H_
#define _ALIAS_H_

// Alias de Palabras del Lenguaje ANSI C++
#define CLASS     class
#define PUBLIC    public
#define PROTECTED protected
#define PRIVATE   private
#define VIRTUAL   virtual
#define INLINE    inline          
#define FRIEND    friend

// Alias de Palabras del Lenguaje ANSI C
#define STRUCT    struct  
#define STATIC    static  
#define CONST     const   

// Alias de Extensiones Propietarias (segun cada compilador)
#ifdef EXPORT
#undef EXPORT
#undef IMPORT
#endif // EXPORT
#ifdef _WIN32
#define EXPORT    __declspec(dllexport)  // MS VC/C++ 5.0
#define IMPORT    __declspec(dllimport)  // MS VC/C++ 5.0
#else
#define EXPORT    __export               // MS C/C++ 1.5
#define IMPORT
#endif

#ifndef EXPORT_ATTR
#define EXPORT_ATTR    __declspec(dllexport)  // MS VC/C++ 5.0
#define IMPORT_ATTR    __declspec(dllimport)  // MS VC/C++ 5.0
#endif // #define

#endif // _ALIAS_H_

