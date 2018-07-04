///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// COPY MACROS 
//
// Tarea      Fecha           Autor   Observaciones
// (1.0.0.1)  2005.12.31      mdc     BASE
// (1.0.0.1)  2006.05.17      mdc     PADCOPYFIXEDINT
// (1.0.0.2)  2007.12.31      mdc     ASSIGNSTRING, ASSIGNFIELD
// (1.0.0.2)  2011.05.02      mdc     PADCOPYINTEGER_INT
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _COPY_MACROS_

// Macros de copia 
#define COPYFIELD( to, from )      memcpy( to,from,min(sizeof(to),sizeof(from)) );
#define COPYSTRING( to, from )     strcpy( to,from );
#define COPYSTRING_len( to, from, len )     strncpy( to,from, len);
#define EXACTCOPYFIELD( to, from ) memcpy( to,from, sizeof(to) );
#define COPYSTRUCT( to, from )     memcpy( &to,&from, sizeof(to) );
#define RIGHTCOPYFIELD( to, from ) sprintf(to,"%-*.*s", sizeof(to), sizeof(to), from);
#define PADCOPYINTEGER( to, from ) sprintf(to,"%0*i" , sizeof(to), atoi(from) );
#define PADCOPYINTEGER_INT( to, from ) sprintf(to,"%0*i" , sizeof(to), from );
#define ZEROEFIELD( fld )     memset( fld,'0',sizeof(fld) );
#define BLANKFIELD( fld )     memset( fld,' ',sizeof(fld) );
#define ZEROESTRING( str )    memset( str,'0',sizeof(str)-1 );
#define BLANKSTRING( str )    memset( str,' ',sizeof(str)-1 );
#define BLANKSTRUCT( fld )    memset( &fld,' ',sizeof(fld) );
#define ASSIGNSTRING(dst,src) \
    { strncpy(dst,src,sizeof(src)); dst[sizeof(src)]=0x00; }
#define ASSIGNFIELD(dst,src) \
    { memset(dst,' ',sizeof(dst)); memcpy(dst,src,min(sizeof(src),sizeof(dst)));  }

#define _USE_FIXED_INTEGER_
#ifdef  _USE_FIXED_INTEGER_
#include <qusrinc/stdiol.h>
#define PADCOPYFIXEDINT_ASCII( to, from )    sprintf(to,"%0*I64i", sizeof(to), antofix(from,(int)strlen(from)) );
#define PADCOPYFIXEDINT( to, from )          sprintf(to,"%0*I64i", sizeof(to), from );
#define PADCOPYFIXEDINT_len( to, from, len ) sprintf(to,"%0*I64i", len, from );
#endif



#ifdef _SYSTEM_DATANET_

#ifndef _DATANET_H_
#include <qusrinc/datanet.h>
#endif // _DATANET_H_

// Macro de verificacion de codigo de banco emisor/receptor , si es banco propio
#define IS_ON_OUR_BANKCODE(x)   (BCRA_ID_DEFAULT_I == antoi(x,3) || BCRA_ALTERNATE_ID_I == antoi(x,3)) 

#endif //	_SYSTEM_DATANET_

#endif // _COPY_MACROS_



