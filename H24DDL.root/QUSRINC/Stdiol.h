//////////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 Sistemas S.A.
// Standard I/O Library Extension (STDIOL)
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.06.30      mdc     Base: numnzpad()
// (Inicial)    1998.11.19      mdc     Base: day_of_year()
// (Inicial)    1999.01.12      mdc     Base: antoi()
// (Inicial)    1999.03.22      mdc     Base: hexcharbinary() e inversa, desde Crypto
// (Inicial)    1999.04.07      mdc     binaryhexchardec();
// (Inicial)    1999.04.21      mdc     long antol(char *pchAsciiNum, int iLen);
// (Inicial)    1999.06.30      mdc     open_file,close_file,write_file,read_file
// (Inicial)    1999.07.07      mdc     ftp_send_file(...) y DoFileTransfering(...)
// (Inicial)    1999.07.13      mdc     DoFileTransfering() acepta hosts opcionales
// (Alfa)       1999.09.23      mdc     DoFileTransfering() acepta booleano de reemplazo de "."
//                                      #define _MLINK_HOST_ID_ (1)
// (Alfa)       1999.09.29      mdc     move_file() agregada.
// (Alfa)       1999.11.01      mdc     read_file_str() agregada
// (Alfa)       1999.11.16      mdc     get_file_size();
// (Alfa)       1999.11.26      mdc     DoFileCopy()
// (Beta)   	2000.01.19		mdc		ftp_api_send_file() como alternativa a ftp_send_file().
// (Beta)   	2000.02.03		mdc		ftp_read_db_params - DoFileTransfering_Ext
// (Release-1) 	2000.03.06		mdc		_FTP_MLINKRPT_HOST_ID_ Agregado.
// (Release-2) 	2002.11.20		mdc		FIXEDINTEGERMASK para SPRINTF()
// (Release-2) 	2003.01.21		mdc		EXTERN short EXPORT_ATTR  strnumlen( char *pszValue , const short nMax );
// (Release-2) 	2005.08.23		mdc		EXTERN char *EXPORT_ATTR  strsubstr(char *pszBuf, int iFrom, int iSize);
// (Release-2) 	2005.10.28		mdc		EXTERN short EXPORT_ATTR  strltrim(...);
// (Release-3) 	2006.02.17		mdc		SWAPBYTE Y SWAPBYTEL
// (Release-3) 	2006.05.08		mdc		TextToPacked(unsigned char *Packed, const char *Text, size_t BufSize);
// (Release-3) 	2006.05.15		mdc		swapbytearray(unsigned char *array, size_t array_size);
// (Release-3) 	2006.05.29		mdc		EXTERN unsigned char EXPORT_ATTR GetEBCDIC_ASCII( unsigned char uchBYTE ) ;
// (Delta)   	2008.01.16		mdc		EXTERN PCHAR  EXPORT_ATTR  rtrim(const char* szPalabra);
// 
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STDIOL_H
#define STDIOL_H

// Standard Library functions
#include <stdio.h> 

/*****************************************************************************************/
/* EXPORT ATTRIBUTE                                                                      */
/*****************************************************************************************/
#if (_MSC_VER >= 800) 
// Export segun plataforma y compilador
#ifdef  _WIN32 
#ifndef EXPORT_ATTR
#define EXPORT_ATTR __declspec( dllexport )
#endif // EXPORT_ATTR
#else
#define EXPORT_ATTR __export
#endif
#endif /* mscver */

#if defined( __cplusplus )
#define EXTERN extern "C" 
#else
#define EXTERN
#endif /* __cplusplus */

/*****************************************************************************************/
/* FIXEDINTEGER segun plataforma y compilador                                            */
/* Se tratara de ubicar segun compilador y plataforma, aquella implementacion de ENTEROS */
/* (en lo posible) que satisfaga el rango de 19 digitos representativos.                 */
/* Se determina en funcion de lo que digan los headers (.h) dependientes de plataforma.  */
/* 1999.09.02, mdc VISUAL C++ 32 bits                                                    */
/* 2006.02.17, mdc VISUAL C++ 64 bits                                                    */
/*****************************************************************************************/
#if (_MSC_VER >= 800) 
#if defined( _WIN32 ) || defined( _WIN64 )
/* Es Microsoft C/C++ 8.00 y es para MsWindows 32 bits */
typedef unsigned __int64 FIXEDINTEGER ;	/* range : 0:0xffffffffffffffff (20 digits, 64 bits) */
#define FIXEDINTEGERMASK "%I64i"        /* SPRINTF mask */
#define MAX_FIXEDINTEGER _UI64_MAX
#define atofix _atoi64
#else  /* else-win32 */
/* Es Microsoft C/C++ 8.00 y no es para MsWindows 32 bits */
#error "Verificar si el compilador mapea 'long double' hacia 'double' y comentar linea siguiente"
/* Comentar alguna de estas 2 lineas siguientes segun mapee el compilador hacia double o no */
typedef double FIXEDINTEGER ;			/* range : 1.7E +/- 308 (15 digits, 64 bits) */
typedef long double FIXEDINTEGER ;		/* range : 1.2E +/- 4932 (19 digits, 80 bits) */
#define FIXEDINTEGERMASK "%.0f"         /* SPRINTF mask */
#define MAX_FIXEDINTEGER HUGE_VAL
#define atofix atof
#endif /* win32 */
#else  /* else-mscver */	
/* No es Microsoft C/C++ 8.00 , cualquier otro compilador */
typedef long double FIXEDINTEGER ;		/* range : 1.2E +/- 4932 (19 digits, 80 bits) */
#define FIXEDINTEGERMASK "%.0f"         /* SPRINTF mask */
#define MAX_FIXEDINTEGER HUGE_VAL
#define atofix atold
#endif /* mscver */
/*****************************************************************************************/


/* Tipo ptr-a-void local al modulo */
typedef void *pvoid_t;
/* Tipo booleano local al modulo */
#ifndef _BOOLEAN_T_
#define _BOOLEAN_T_
typedef enum {is_false=0,is_true=1} boolean_t;
#endif /* _BOOLEAN_T_ */

/* Prototipos generales */
EXTERN size_t EXPORT_ATTR numnzpad(char *lpszData, size_t nLimit) ;
EXTERN size_t EXPORT_ATTR day_of_year(char *pchInput , size_t nInSize ,
									  char *pchOutput, size_t nOutSize,
									  int   bStrWithZero );
EXTERN int          EXPORT_ATTR antoi(char *pchAsciiNum, int iLen);
EXTERN long         EXPORT_ATTR antol(char *pchAsciiNum, int iLen);
EXTERN FIXEDINTEGER EXPORT_ATTR antofix(char *pchAsciiNum, int iLen);
EXTERN double       EXPORT_ATTR antof(char *pchAsciiNum, int iLen, int iDecPlaces);
EXTERN int     EXPORT_ATTR binaryhexchar(char *hexbuffer, int iHexLen, short *binbuffer);
EXTERN int     EXPORT_ATTR binaryhexchardec(char *hexbuffer, int iHexLen, short *binbuffer, short option,
											short start);
EXTERN int     EXPORT_ATTR hexcharbinary(short *binbuffer, char *hexbuffer, int iHexLen);

/* Prototipos de IO en archivos con nombres UNC */
EXTERN pvoid_t EXPORT_ATTR open_file(char *lpFileName, int iAccessMode, int iActionType);

EXTERN pvoid_t EXPORT_ATTR open_file_ext(char *lpFileName);

EXTERN int     EXPORT_ATTR close_file(pvoid_t hFileObject);
EXTERN int     EXPORT_ATTR write_file( pvoid_t hFile, pvoid_t lpBuffer,	long nBytesToWrite, 
									  long *lpBytesWritten);
EXTERN int     EXPORT_ATTR read_file(pvoid_t hFile, pvoid_t lpBuffer, long nBytesToRead, 
									 long *lpBytesRead);
EXTERN int     EXPORT_ATTR read_file_str(pvoid_t hFile, pvoid_t lpBuffer, long nBytesToRead, 
									 long *lpBytesRead);
EXTERN int     EXPORT_ATTR move_file(char *lpFileName, char *lpOldFileName);
EXTERN unsigned long EXPORT_ATTR get_file_size(char *lpFileName);

/*****************************************************************************************/
#ifdef _USE_FTP_API_
/* Prototipos de invocacion a servicio de FTP */
EXTERN int     EXPORT_ATTR ftp_send_file(char *szHost,char *szUser, char *szPassword, char *szMode,
										 char *szFileFrom, char *szFileTo, char *szLogger);
/* Prototipos de invocacion a servicio de FTP mediante APIs */
EXTERN int     EXPORT_ATTR ftp_api_send_file(char *pszHost, char *szUser, char *szPassword,
											 char *pszFileFrom, char *pszFileTo,
											 char *pszMode, char *pszErrorTxt, 
											 unsigned short nMaxTxt );
EXTERN int     EXPORT_ATTR ftp_read_db_params(
		char *szFTPHost, size_t nHost,			// Identificador de tipo de Host (input)
		char *szFIID, size_t nFIID,				// FIID de Institucion (input opcional)
		char *szXFerHost, size_t nIPHost,		// IP del host (output) 
		char *szXFerUser, size_t nUser,			// Username (output)
		char *szXFerPassw, size_t nPassw,		// Clave de acceso (output)
		char *szXFerMode, size_t nMode,			// Modo binario o ascii (output)
		char *szXFerRemotePath, size_t nPath);	// Path remoto (output)
EXTERN int     EXPORT_ATTR DoFileTransfering(char *szFIID, char *szXferFile, char *szMode,
											 char *szLogger, char *szINIFileName, int iHost,
											 boolean_t bExtAllowed);
EXTERN int     EXPORT_ATTR DoFileCopy(char *szFIID, char *szXferFile, char *szINIFile);
EXTERN int     EXPORT_ATTR DoFileTransfering_Ext(
	char *szFIID, char *szXferFile, char *szLoggerFile,
	char *szHostType, boolean_t bExtDisable, 
	char *szErrorMsg, size_t nErrorTxt );
#endif // _USE_FTP_API_
/*****************************************************************************************/

/*****************************************************************************************/
#ifndef PCHAR_DEFINED
#define PCHAR_DEFINED
typedef char * PCHAR ;
#endif 
/*****************************************************************************************/

EXTERN boolean_t EXPORT_ATTR DoFilePrinting(char *lpszFileName);
EXTERN short  EXPORT_ATTR  strnumlen( char *pszValue , const short nMax );
EXTERN short  EXPORT_ATTR  strnumlenpad( char *pszValue , const short nMax, char chPadding );
EXTERN PCHAR  EXPORT_ATTR  strsubstr(char *pszBuf, int iFrom, int iSize);
EXTERN short  EXPORT_ATTR  strltrim( char *pszValue , const short nMax);
EXTERN short  EXPORT_ATTR  strcutchar( char *pszValue , const short nMax, char chChar);
EXTERN short  EXPORT_ATTR  strrepchar( char *pszValue , const short nMax, char chChar, char chNew);
EXTERN void	  EXPORT_ATTR  swapbyte (unsigned short *sh);
EXTERN void	  EXPORT_ATTR  swapbytel(unsigned long *lng);
EXTERN void	  EXPORT_ATTR  swapbytef(FIXEDINTEGER *fix);
EXTERN void	  EXPORT_ATTR  swapbytearray(unsigned char **array, size_t array_size);
EXTERN PCHAR  EXPORT_ATTR  rtrim(const char* szPalabra, const short nMax );

/*****************************************************************************************/
EXTERN size_t EXPORT_ATTR TextToPacked(unsigned char *Packed, const char *Text, size_t BufSize);
EXTERN size_t EXPORT_ATTR PackedDecimalToString (unsigned char *szPacked, size_t nPackSize, 
                                     char          *szText  , size_t nBufSize ,
                                     char          chPaddingChar ,
									 boolean_t	   bUnmaskFromEBCDIC);
EXTERN size_t EXPORT_ATTR NumericToPackedDecimal(unsigned char *szPacked, size_t nPackSize, 
                                     char          *szText  , size_t nBufSize ,
                                     char          chPaddingChar ,
									 boolean_t     bMaskForEBCDIC);
EXTERN PCHAR  EXPORT_ATTR  DateTimeToDateYYYYMMDD(const char *szDateAndTime, 
                                    char       *szYYYYMMDD   );
EXTERN PCHAR  EXPORT_ATTR  DateYYYYMMDDToDateTime(const char *szYYYYMMDD     , 
                                                  char       *szDateAndTime  ,
                                                  char       chDig );
EXTERN unsigned char EXPORT_ATTR GetEBCDIC_ASCII( unsigned char uchBYTE ) ;
EXTERN unsigned char EXPORT_ATTR GetASCII_EBCDIC( unsigned char uchBYTE )  ;
/*****************************************************************************************/

EXTERN int EXPORT_ATTR convert_extract_ammount( char *szExtAmmount , const size_t cEXTR_AMT_LEN);

EXTERN unsigned long EXPORT_ATTR ProtectStringValue(const char *value, char *result, unsigned long *len);

EXTERN PCHAR EXPORT_ATTR UnprotectStringValue(char* value, unsigned long cryptLen);

//afp: declaro función para liberar memoria alocada dinamicamente en UnprotectStringValue desde fuera del dll, ya que no está asegurado que se comparta heap entre dll y exe
EXTERN void EXPORT_ATTR liberarMemoriaHeap(char *ptr);

#endif /* STDIOL_H */

