//                                                              
// IT24 Sistemas S.A.
// Standard I/O Library Extension (STDIOL)
//
// Tarea		Fecha			Autor	Observaciones
// (Inicial)	1998.06.30		mdc		Base: numnzpad()
// (Inicial)	1998.11.19		mdc		Base: day_of_year()
// (Inicial)	1999.01.12		mdc		Base: antoi()
// (Inicial)	1999.03.22		mdc		Base: hexcharbinary() e inversa, desde Crypto Library.
// (Inicial)	1999.04.07		mdc		hexcharbinary() e inversa, corregidas para DES, Intel x86.
// (Inicial)	1999.04.09		mdc		_LITTLE_ENDIAN_BIT_ o _BIG_ENDIAN_BIT_
// (Inicial)	1999.04.21		mdc		long antol(char *pchAsciiNum, int iLen);
// (Inicial)	1999.06.30		mdc		open_file,close_file
// (Inicial)	1999.07.07		mdc		ftp_send_file(...)
// (Beta)   	2002.03.27      mdc     double EXPORT_ATTR antofix(char *pchAsciiNum, int iLen)
// (Beta)   	2003.01.21      mdc     short EXPORT_ATTR  strnumlen( char *pszValue , const short nMax )
// (Beta)   	2005.08.31      mdc     PCHAR EXPORT_ATTR strsubstr(char *pszBuf, int iFrom, int iSize)
// (Delta)   	2005.11.01      mdc     strcutchar(...)
// (Delta)   	2006.02.18      mdc     swapbyte(),swapbytel(),swapbytef() para intercambiar bits
// (Delta)   	2006.02.20      mdc     strcutchar(...) agrega precondiciones MAS fuertes.
// (Delta)   	2006.05.15      mdc     int CONVERT_ASCII_EBCDIC_FILE( char *szTrxFilename , char chEbcdic2Ascii , 
//                                      long lRecordLen     , long *plCounter )
// (Delta)   	2006.05.23      mdc     IS_VALID_POINTER
// (Delta)   	2006.05.31      mdc     *** FIX *** DateTimeToDateYYYYMMDD(...)
// (Delta)   	2006.10.19      mdc     void convert_extract_ammount( ... )
// (Delta)   	2007.09.23      mdc     int convert_extract_ammount( char *szExtAmmount , const size_t cEXTR_AMT_LEN)
// (Delta)   	2008.01.16		mdc		rtrim(const char* szPalabra);
//
// 
//

// Ms-Windows Header
#include <windows.h> 

// ANSI C/C++ Standard Library functions
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h> 
#include <errno.h>
#include <math.h>
#include <locale.h>
// Header propio
#include <qusrinc/stdiol.h>

#define IS_VALID_POINTER(x)	(NULL != x && \
							((void *)0xdddddddd != x) && \
							((void *)0xcdcdcdcd != x) && \
							((void *)0xffffffff != x) && \
                            ((void *)0x00000001 != x))



///////////////////////////////////////////////////////////////////////////////////
//
// Number-no-Zero-Padded:
// (Cadena Numerica no precedida de Ceros)
// Input: Puntero a Caracter y Longitud de Cadena de Caracteres Numericos.
// Output: Cantidad de caracteres '0' reemplazados por ' ' en la Cadena Numerica.
//
///////////////////////////////////////////////////////////////////////////////////
size_t EXPORT_ATTR numnzpad(char *lpszData, size_t nLimit) 
	{
	// Contador de ceros '0' hasta el fin del string o nCount
	size_t    nZeroes,
		      nCount; 
	boolean_t bSign;
	char      chSign;

	// Precondicion: Puntero y limites validos
	if((lpszData == NULL) && (nLimit > 0))
		return (-1);

	// Ciclo de reemplazo de ceros '0' x blancos, y verificacion de negativo
	for (nCount = 0, 
		 nZeroes = 0, 
		 bSign = is_false;

		 // Condicion mientras...
		 (nCount < nLimit) &&
		 (nZeroes < nLimit) &&
		 ((*lpszData) == '0' || (*lpszData) == ' ' ||
		  (*lpszData) == '-' || (*lpszData) == '+' || (*lpszData) == 0);

		 // Incremento de indexadores
		 lpszData++, 
		 nCount++)
	     if((*lpszData) == '0')
			{
			// Reemplazo del cero ('0') x blanco (' ')
			(*lpszData) = ' '; 
			// Contador de ceros reemplazados
			nZeroes++;
			}
		 else if ((*lpszData) == '-')
			{
			// Reemplazo del negativo ('-') x blanco (' ')
			(*lpszData) = ' ';
			// TRUE al booleano del signo reemplazado
			bSign  = is_true;
			chSign = '-';
			} 
		 else if ((*lpszData) == '+')
			{
			// Reemplazo del positivo ('+') x blanco (' ')
			(*lpszData) = ' ';
			// TRUE al booleano del signo reemplazado
			bSign  = is_true;
			chSign = '+';
			};//end-if-and-end-for

	// A la salida, (*lpszData) es un caracter distinto de '0',' ','-' y '+'...
	// y en el caracter anterior debe ir el signo, si es que fue reemplazado...
	if(bSign)
		 if(*(--lpszData) == ' ')
			 (*lpszData) = chSign;

	// Retorna los ceros ('0') reemplazados
	return (nZeroes);
	}; //end-numnzpad

///////////////////////////////////////////////////////////////////////////////////
//
// day_of_year 
// (dia del ano)
// Input  : Puntero a Caracter y Longitud de Cadena de Caracteres Numericos.
// Output : Cantidad de caracteres del dia juliano en el ano
// Return : Day-Of-Year
//
// (ANSI C-UNIX compliant version)
//
///////////////////////////////////////////////////////////////////////////////////
size_t EXPORT_ATTR day_of_year(	char *pchInput , size_t nInSize ,
							char *pchOutput, size_t nOutSize,
							int   bStrWithZero )
	{
	struct tm	tmDOY;        // Day-Of-Year
	time_t		tiTime;       // Time of day
	char        chBuffer[10]; // Temporary conversion buffer

	tmDOY.tm_sec   = 0; // Seconds after minute (0 - 59)
	tmDOY.tm_min   = 0; // Minutes after hour (0 - 59)
	tmDOY.tm_hour  = 0; // Hours since midnight (0 - 23)
	tmDOY.tm_mday  = 0; // Day of month (1 - 31)
	tmDOY.tm_mon   = 0; // Month (0 - 11; January = 0)
	tmDOY.tm_year  = 0; // Year (current year minus 1900)
	tmDOY.tm_wday  = 0; // Day of week (0 - 6; Sunday = 0)
	tmDOY.tm_yday  = 0; // Day of year (0 - 365; January 1 = 0)
	tmDOY.tm_isdst = 0; // Always 0 for gmtime

	// Convert from ASCII time string
	// Assume ISO YYYYMMDD format?
	if(nInSize == 8)
	{
		strncpy(chBuffer, pchInput, 4);
		chBuffer[4] = 0x00;
		tmDOY.tm_year = atoi(chBuffer) - 1900;
		strncpy(chBuffer, pchInput+4, 2);
		chBuffer[2] = 0x00;
		tmDOY.tm_mon = atoi(chBuffer) - 1;
		strncpy(chBuffer, pchInput+6, 2);
		chBuffer[2] = 0x00;
		tmDOY.tm_mday = atoi(chBuffer);
	}
	// Assume ISO YYMMDD format?
	else if(nInSize == 6)
	{
		strncpy(chBuffer, pchInput, 2);
		chBuffer[2] = 0x00;
		tmDOY.tm_year = atoi(chBuffer);
		// begin-Y2K-fix : assume 75 as the base year
		if( tmDOY.tm_year <= 75) 
			tmDOY.tm_year += 100;
		// end-Y2K-fix
		strncpy(chBuffer, pchInput+2, 2);
		chBuffer[2] = 0x00;
		tmDOY.tm_mon = atoi(chBuffer) - 1;
		strncpy(chBuffer, pchInput+4, 2);
		chBuffer[2] = 0x00;
		tmDOY.tm_mday = atoi(chBuffer);
	}
	else
		return (0);

	// Make time
	tiTime = mktime( &tmDOY );
	if(tiTime == (time_t)(-1))
		return (0);

	// Day of year (0 - 365; January 1 = 0)
	tmDOY.tm_yday++;

	// Convert to ASCII time string
	// checking parameters and string-ended-with-zero
	if(pchOutput && nOutSize > 0)
		if(bStrWithZero && pchOutput && nOutSize > 0)
			sprintf(pchOutput,"%0*i", min(sizeof(chBuffer), nOutSize), tmDOY.tm_yday);
		else
			{
			sprintf(chBuffer,"%i", tmDOY.tm_yday);
			strncpy(pchOutput, chBuffer, min(sizeof(chBuffer), nOutSize));
			}
	else
		// Return DOY
		return ( tmDOY.tm_yday );

	// Return DOY
	return ( tmDOY.tm_yday );

	}; //end-day_of_year

/////////////////////////////////////////////////////////////////////
//
// ascii-n-to-integer
// Input: Puntero a Caracter y Longitud de Cadena de Caracteres Numericos.
// Output : Numero binario convertido
// Return : Numero binario convertido
//
/////////////////////////////////////////////////////////////////////
int EXPORT_ATTR antoi(char *pchAsciiNum, int iLen)
{
	char szNumber[32];
	// Precondicion
	if(iLen >= (sizeof(szNumber)-1))
		return -1;
	// Precondicion
	if(!pchAsciiNum || !IS_VALID_POINTER(pchAsciiNum) )
		return -1;
	// Copia interna
	strncpy(szNumber, pchAsciiNum, iLen);
	szNumber[iLen] = 0x00;
	// Retorno estandard
	return atoi(szNumber);
};

/////////////////////////////////////////////////////////////////////
//
// ascii-n-to-long-integer
// Input: Puntero a Caracter y Longitud de Cadena de Caracteres Numericos.
// Output : Numero binario convertido
// Return : Numero binario convertido
//
/////////////////////////////////////////////////////////////////////
long EXPORT_ATTR antol(char *pchAsciiNum, int iLen)
{
	char szNumber[32];
	// Precondicion
	if(iLen >= (sizeof(szNumber)-1))
		return -1;
	// Copia interna
	strncpy(szNumber, pchAsciiNum, iLen);
	szNumber[iLen] = 0x00;
	// Retorno estandard
	return atol(szNumber);
};

/////////////////////////////////////////////////////////////////////
//
// ascii-n-to-fixed-integer
// Input: Puntero a Caracter y Longitud de Cadena de Caracteres Numericos.
// Output : Numero binario convertido
// Return : Numero binario convertido
//
/////////////////////////////////////////////////////////////////////
FIXEDINTEGER EXPORT_ATTR antofix(char *pchAsciiNum, int iLen)
{
	char szNumber[64];
	// Precondicion
	if(iLen >= (sizeof(szNumber)-1))
		return -1;
	// Copia interna
	strncpy(szNumber, pchAsciiNum, iLen);
	szNumber[iLen] = 0x00;
	// Retorno propietario
	return atofix(szNumber);
};

/////////////////////////////////////////////////////////////////////
//
// ascii-n-to-float / double
// Input: Puntero a Caracter y Longitud de Cadena de Caracteres Numericos.
// Output : Numero binario convertido (FLOAT / DOUBLE)
// Return : Numero binario convertido (FLOAT / DOUBLE)
//
/////////////////////////////////////////////////////////////////////
double EXPORT_ATTR antof(char *pchAsciiNum, int iLen, int iDecPlaces)
{
	char szNumber[64];
	// Precondicion
	if(iLen >= (sizeof(szNumber)-1) || iDecPlaces < 0 || iDecPlaces > iLen)
		return -1;
	// Punto decimal forzado ?
	if(iDecPlaces > 0 && iDecPlaces < iLen)
		sprintf(szNumber,"%*.*s.%*.*s", iLen-iDecPlaces,iLen-iDecPlaces,
										pchAsciiNum,
										iDecPlaces,iDecPlaces,
										pchAsciiNum + (iLen-iDecPlaces));
	else // O copia directa ?
	{		
		strncpy(szNumber, pchAsciiNum, iLen);
		szNumber[iLen] = 0x00;
	}
	// Retorno propietario
	return atof(szNumber);
};

/////////////////////////////////////////////////////////////////////
// de hexa a binario 
// short *: buffer binario 
// char * : buffer hexadecimal
// int    : longitud de buffer hexadecimal
/////////////////////////////////////////////////////////////////////
int EXPORT_ATTR hexcharbinary(short *dst_bin, char *src, int len)
{
	int		i, j, n;
	char	c;
	char    *dst = (char *)dst_bin;

	if(len < 0 || (len % 2) != 0 )
		return (0);

	for (i = 0; i < len/2; i++)
		{
		n = 0;
		for (j = 0; j < 2; j++)
			{
			n <<= 4;
			c = *(src + 2 * i + j);
			if (c >= '0' && c <= '9')
				n |= c - '0';
			////////////////////////////////////////////////////////////
			// Acepta tambien minusculas en hexadecimal, 2000.02.21,mdc
			else if (c >= 'A' && c <= 'F')
				n |= c - 'A' + 10;
			else if (c >= 'a' && c <= 'f')
				n |= c - 'a' + 10;
			////////////////////////////////////////////////////////////
			else
				return (0);
			};
		*(dst + i) = n;
		};	

	return (len);
};

////////////////////////////////////////////////////////////////////////////////
// Segun sea _LITTLE_ENDIAN_BIT_ o _BIG_ENDIAN_BIT_, deben compilarse
// distintos procedimientos de conversion Hexadecimal a Binario e inversa
// Little-endian para Intel x86 y similares
// Big-endian para Motorola, Alpha, MIPS y similares
#define	_LITTLE_ENDIAN_BIT_
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////
#if defined( _LITTLE_ENDIAN_BIT_ )
///////////////////////////////////

/////////////////////////////////////////////////////////////////////
// de binario a hexa 
// char * : buffer hexadecimal
// int    : longitud de buffer hexadecimal
// short *: buffer binario 
/////////////////////////////////////////////////////////////////////
int EXPORT_ATTR binaryhexchar(char *dst, int len, short *src_bin)
{
	int		i, j, n;
	char	c;
	char    *src = (char *)src_bin;

	if(len < 0 || (len % 2) != 0 )
		return (0);

	j = 0;
	for (i = 0; i < len/2; i++)
		{
		c = *(src + i);

		n = (c >> 4) & 0x0F;
		n += (n < 10) ? '0' : 'A' - 10;
		*(dst + j) = n;
		j++;

		n = c & 0x0F;
		n += (n < 10) ? '0' : 'A' - 10;
		*(dst + j) = n;
		j++;
		};

	return (len);
};

/////////////////////////////////////////////////////////////////////
// de binario a hexa decimalizado
// char * : buffer hexadecimal
// int    : longitud de buffer hexadecimal
// short *: buffer binario 
// short  : opcion de reemplazo de 0-9 (0), solo A-B (1) o ambos (3)
// short  : inicio de reemplazo en posicion indicada
/////////////////////////////////////////////////////////////////////
int EXPORT_ATTR binaryhexchardec(char *dst, int len, short *src_bin, short option, short start)
{
	int		i, j, n;
	char	c;
	char    *src = (char *)src_bin;

	if(len < 0 || (len % 2) != 0 || start < 0)
		return (0);

	j = start;
	for (i = 0; i < len/2; i++)
		{
		c = *(src + i);

		n = (c >> 4) & 0x0F;
		if((option == 0) && (n<10))
			{
			n += '0'; 
			*(dst + j) = n;
			j++;			
			}
		else if((option == 1) && (n>10))
			{
			n += '0' - 10; 
			*(dst + j) = n;
			j++;			
			}
		else if(option == 3)
			{
			n += (n < 10) ? '0' : '0' - 10;
			*(dst + j) = n;
			j++;			
			}

		n = c & 0x0F;
		if((option == 0) && (n<10))
			{
			n += '0'; 
			*(dst + j) = n;
			j++;			
			}
		else if((option == 1) && (n>10))
			{
			n += '0' - 10; 
			*(dst + j) = n;
			j++;			
			}
		else if(option == 3)
			{
			n += (n < 10) ? '0' : '0' - 10;
			*(dst + j) = n;
			j++;			
			}
		};

	return (j);
};


//////////////////////////////
#else // _LITTLE_ENDIAN_BIT_
//////////////////////////////


///////////////////////////////////////////////////////////////////////////////////	
// Estructuras de mapeo de campos para tomar de a 4 bits un entero
///////////////////////////////////////////////////////////////////////////////////

// ANSI-C Standard Bit-datatype Alignement (16 bits integer)
typedef unsigned short bit_basetype;

// Estructura de mapeo de a 4 bits, hasta un total de 16 bits.
typedef struct int16bitmap_tag
{
	bit_basetype first_low4bits  : 4;	// bits 12 a 15
	bit_basetype secnd_low4bits  : 4;	// bits  8 a 11
	bit_basetype first_high4bits : 4;	// bits  4 a  7
	bit_basetype secnd_high4bits : 4;	// bits  0 a  3
} int16bitmap_t;
typedef struct int16bitmap_tag *int16bitmap_ptr;
///////////////////////////////////////////////////////////////////////////////////

/* de hexa a binario (destiny,source) */
/* char * : buffer binario 
 * char * : buffer hexadecimal
 * int    : longitud de buffer hexadecimal
 */
int EXPORT_ATTR hexcharbinary(short *binbuffer, char *hexbuffer, int iHexLen)
{
	 int				j, max;
	 unsigned char		byte1, byte2;
	 int16bitmap_ptr	ptr_int = (int16bitmap_ptr)binbuffer;

	/* precondicion */
	if(iHexLen < 0 || (iHexLen % 2) != 0 )
		return (0);
	/* maximo de buffer de a 16 bits */
	max = iHexLen / 2;
	/* de hexa a binario byte a byte */
	for(j=0; j < max; j++)
		{
		/* asegurar la linealialidad del rango {0..9,A..F} */
		if(hexbuffer[j*2] >= 'A' && hexbuffer[j*2] <= 'F')
			byte1 = '9' + (hexbuffer[j*2] - 'A') + 1;
		else if(hexbuffer[j*2] >= '0' && hexbuffer[j*2] <= '9')
			byte1 = hexbuffer[j*2];
		else
			return (-1);
		/* asegurar la linealialidad del rango {0..9,A..F} */
		if(hexbuffer[(j*2)+1] >= 'A' && hexbuffer[(j*2)+1] <= 'F')
			byte2 = '9' + (hexbuffer[(j*2)+1] - 'A') + 1;
		else if(hexbuffer[(j*2)+1] >= '0' && hexbuffer[(j*2)+1] <= '9')
			byte2 = hexbuffer[(j*2)+1];
		else
			return (-1);
		/* 2 bytes hexa empaquetados en 1 byte binario */
		if((j % 2) == 0)
			{
		       ptr_int->secnd_high4bits = (byte1 - '0');
		       ptr_int->first_high4bits = (byte2 - '0');
			}
		else
			{  
			   ptr_int->secnd_low4bits = (byte1 - '0');
		       ptr_int->first_low4bits = (byte2 - '0');
			   /* cada 4 digitos hexa (16 bits) incrementar ptr entero */
			   ptr_int++;
			};	
		};
	/* ok */
	return max;
};

/* de binario a hexa (destiny,source) */
/* char * : buffer binario 
 * char * : buffer hexadecimal
 * int    : longitud de buffer hexadecimal
 */
int EXPORT_ATTR binaryhexchar(char *hexbuffer, int iHexLen, short *binbuffer)
{
	 int				j, max;
	 unsigned char		byte1, byte2;
	 int16bitmap_ptr	ptr_int = (int16bitmap_ptr)binbuffer;

	/* precondicion */
	if(iHexLen < 0 || (iHexLen % 2) != 0 )
		return (0);
	/* maximo de buffer de a 16 bits */
	max = iHexLen / 2;
	/* de binario a hexa byte a byte */
	for(j=0; j < max; j++)
		{
		/* 2 bytes hexa empaquetados en 1 byte binario */
		if((j % 2) == 0)
			{
		       byte1 = ptr_int->secnd_high4bits + '0';
		       byte2 = ptr_int->first_high4bits + '0';
			}
		else
			{
			   byte1 = ptr_int->secnd_low4bits + '0';
		       byte2 = ptr_int->first_low4bits + '0';
			   /* cada 4 digitos hexa (16 bits) incrementar ptr entero */
			   ptr_int++;
			};			
		/* asegurar la linealialidad del rango {0..9,A..F} */
		if(byte1 >= ':' && byte1 <= '?')
			byte1 = 'A' + (byte1 - ':');
		else if (!(byte1 >= '0' && byte1 <= '9'))
			return (-1);
		if(byte2 >= ':' && byte2 <= '?')
			byte2 = 'A' + (byte2 - ':');
		else if (!(byte2 >= '0' && byte2 <= '9'))
			return (-1);
		hexbuffer[j*2]     = byte1;
		hexbuffer[(j*2)+1] = byte2;
		};
	/* ok */
	return max;
};

//////////////////////////////
#endif // _LITTLE_ENDIAN_BIT_
//////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Funcion de apertura de archivo bajo un Sistema de Archivos Distribuido
// lpFileName : nombre de archivo segun sintaxis distribuida "\\?\?\?" (input) 
// iAccessMode: {0=write-read,1=write,2=read} (input)
// iActionType: {0=open-create,2=open,1=create,3=open-append} (input)
// retorno    : NULL por error sino handle valido distinto de NULL
////////////////////////////////////////////////////////////////////////////////
pvoid_t EXPORT_ATTR open_file(char *lpFileName, int iAccessMode, int iActionType)
{
#ifdef _WIN32
	// Variables auxiliares para soporte de seguridad y acceso generales
	SECURITY_ATTRIBUTES  saSecurityAttributes;         // Security Attributes	
	SECURITY_DESCRIPTOR  sdSecurityDescriptor;         // Security Descriptor
	DWORD				 dwDesiredAccess,		       // Desired Access
						 dwCreationDistribution,       // Distribution Desired
						 dwShareMode,			       // Shared Mode						
						 dwFlagsAndAttributes;		   // Attributes
	HANDLE				 hFile   = NULL;
	DWORD				 dwError = 0L;

	// Segun modo de acceso 
	// Lecto-escritura
	if(iAccessMode == 0)      
	{
		dwDesiredAccess = FILE_GENERIC_READ | FILE_GENERIC_WRITE ;
		dwShareMode     = FILE_SHARE_READ | FILE_SHARE_WRITE ;
	}
	// Escritura
	else if(iAccessMode == 1) 
	{	
		dwDesiredAccess = FILE_GENERIC_WRITE ;
	    dwShareMode     = FILE_SHARE_WRITE ;
	}
	// Lectura
	else if(iAccessMode == 2) 
	{
		dwDesiredAccess = FILE_GENERIC_READ ;
		dwShareMode     = FILE_SHARE_READ ;
	}
	else
		return (NULL);
	// Segun accion a realizar
	// Apertura o creacion
	if(iActionType == 0)
		dwCreationDistribution = OPEN_ALWAYS ; 
	// Solo creacion, truncando si existe
	else if(iActionType == 1)
		dwCreationDistribution = CREATE_ALWAYS ;
	// Solo apertura, no creacion	
	else if(iActionType == 2)
		dwCreationDistribution = OPEN_EXISTING;
	else if(iActionType == 3) {
		if(iAccessMode != 1 && iAccessMode != 0)
			return (NULL);
		dwCreationDistribution = OPEN_EXISTING;
		dwDesiredAccess |= FILE_APPEND_DATA;
	} else
		return (NULL);

	// Solo se permiten los objetos de tipo archivo
	dwFlagsAndAttributes =  FILE_ATTRIBUTE_ARCHIVE;

	
	/////////////////////////////////////////////////////////////////////////////////////////////
	// A la finalidad de entender primitivamente el concepto de Descriptor de Seguridad y
	// Atributos de Seguridad, ver el HelpOnLine del Compilador para la plataforma Win95/NT/2000
	//
	// Seguridad x/omision, inicializando la estructura sin Descriptor de Seguridad
    saSecurityAttributes.nLength              = sizeof(saSecurityAttributes); // Longitud
	saSecurityAttributes.lpSecurityDescriptor = NULL;                         // Sin Desc. Seg. asignado
	saSecurityAttributes.bInheritHandle       = TRUE;                         // Permite heredar el handle
	// Inicializar Descriptor de Seguridad segun propietario del proceso, como Revision (1)
	if( InitializeSecurityDescriptor( &sdSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION ) )
	{		
		// Asegurar -si se puede- permisos de acceso sobre el objeto, relativos a la Lista 
		// de Control de Accesos (ACL) del Sistema (SACL) y propietaria (DACL)
		if( !SetFileSecurity( lpFileName, SACL_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION , 
				&sdSecurityDescriptor  ) )
			// Retener error localmente para debugging
			// Errores posibles: { 1313=ERROR_NO_SUCH_PRIVILEGE, 1314=ERROR_PRIVILEGE_NOT_HELD, ...}
			dwError = GetLastError();
		// Redirigir puntero a descriptor de seguridad
		saSecurityAttributes.lpSecurityDescriptor = &sdSecurityDescriptor;
	};
	/////////////////////////////////////////////////////////////////////////////////////////////

	// Apertura en si misma
	hFile = CreateFile(	lpFileName,
						dwDesiredAccess,
						dwShareMode,
						&saSecurityAttributes, 
						dwCreationDistribution, 
						dwFlagsAndAttributes,
						NULL );
	// Retorno segun resultado de la apertura
	if (hFile == INVALID_HANDLE_VALUE || hFile == NULL) 
	{
		// Retener error localmente
		dwError = GetLastError();
		return( NULL );
	}
	else
		return( hFile );
#else
#error "Sin soporte de Sistema de Archivos Distribuido"
#endif
};

////////////////////////////////////////////////////////////////////////////////
// Funcion de apertura de archivo bajo un Sistema de Archivos Distribuido
// Si el archivo existe,crea uno nuevo con extension random
// lpFileName : nombre de archivo segun sintaxis distribuida "\\?\?\?" (input) 
// retorno    : NULL por error sino handle valido distinto de NULL
////////////////////////////////////////////////////////////////////////////////
pvoid_t EXPORT_ATTR open_file_ext (char *lpFileName){
#ifdef _WIN32
	HANDLE				 hFile   = NULL;
	DWORD				 dwError = 0L;
	char				 szFile[256] = {0};
	char				 szRandom[5] = {0};
	// Verificar existencia del archivo abriendolo en modo lectura
	 
	 strncpy(szFile,lpFileName,strlen(lpFileName));
	 hFile = open_file(lpFileName,2,2);
	 if(hFile == NULL)
	 {
		// Archivo inexistente.Lo creo en modo escritura

		hFile = open_file(lpFileName, 1, 1);
		if(hFile == NULL)
			{				
				dwError = GetLastError();
				return( NULL );
			};
	 }
	 else
	 {
		 // Archivo existente,generar con extension aleatoria
		close_file(hFile);

		/* use a random extension */
		srand((unsigned int)time(NULL));

		sprintf(szRandom,".%-u",rand() % 1000);
		strcat(szFile,szRandom);

		hFile = open_file(szFile, 1, 1);
		if(hFile == NULL)
			{				
				dwError = GetLastError();
				return( NULL );
			};
	 };	
	 strcpy(lpFileName,szFile);
	 return (hFile);
#else
#error "Sin soporte de Sistema de Archivos Distribuido"
#endif
};

////////////////////////////////////////////////////////////////////////////
// Funcion de renombrado (o movida) de un archivo hacia otro destino-nombre
// hFileObject: handle valido de archivo abierto previamente (input)
////////////////////////////////////////////////////////////////////////////
int EXPORT_ATTR move_file(char *lpFileName, char *lpNewFileName)
{
#ifdef _WIN32
	// Error en FileSystem (0=Sin error) , solo con motivos de debugging.
	DWORD dwError = 0;

	// Precondicion: ptrs. validos
	if(lpFileName && lpNewFileName)
	{
		// Renombrado-Movida de archivo-dispositivo-objeto hacia otro lugar-nombre
		// El nuevo destino NO debe existir...
		if( MoveFile(lpFileName, lpNewFileName) )
			// Retorno booleano en verdadero
			return 1;
		else
			{
			// Por motivos de debugging, capturar el error interno
			// Errores posibles: { 1313=ERROR_NO_SUCH_PRIVILEGE, 1314=ERROR_PRIVILEGE_NOT_HELD, ...}
			dwError = GetLastError();
			// Retorno booleano en falso
			return 0;
			};
	}
	else
		// Retorno booleano en falso
		return 0;
#else
#error "Sin soporte de Sistema de Archivos Distribuido"
#endif
};

///////////////////////////////////////////////////////////////////// 
// Funcion de cierre de objeto mediante un handle 
// hFileObject: handle valido de archivo abierto previamente (input)
/////////////////////////////////////////////////////////////////////
int EXPORT_ATTR close_file(pvoid_t hFileObject)
{
#ifdef _WIN32
	// Precondicion
	if(hFileObject)
		// Cierre de archivo-dispositivo-objeto
		return CloseHandle( hFileObject );
	else
		// Retorno booleano en falso
		return 0;
#else
#error "Sin soporte de Sistema de Archivos Distribuido"
#endif
};

///////////////////////////////////////////////////////////////////// 
// Funcion de escritura de archivo 
// hFile: handle valido de archivo abierto previamente (input)
/////////////////////////////////////////////////////////////////////
int EXPORT_ATTR write_file( pvoid_t hFile, pvoid_t lpBuffer, long nBytesToWrite, long *lpBytesWritten)
{
	// Precondicion
	return (hFile != NULL && lpBuffer != NULL && nBytesToWrite && lpBytesWritten != NULL)
		// Escritura en si misma
		? WriteFile( hFile, lpBuffer, nBytesToWrite, lpBytesWritten, NULL )
		// Error
		: 0;
};

///////////////////////////////////////////////////////////////////// 
// Funcion de lectura de archivo 
// hFile: handle valido de archivo abierto previamente (input)
/////////////////////////////////////////////////////////////////////
int EXPORT_ATTR read_file( pvoid_t hFile, pvoid_t lpBuffer, long nBytesToRead, long *lpBytesRead)
{
	// Precondicion
	return (hFile != NULL && lpBuffer != NULL && nBytesToRead && lpBytesRead != NULL)
		// Lectura en si misma
		? ReadFile( hFile, lpBuffer, nBytesToRead, lpBytesRead, NULL )
		// Error
		: 0;
};

///////////////////////////////////////////////////////////////////// 
// Funcion de lectura de cadenas de caracteres desde archivo 
// hFile: handle valido de archivo abierto previamente (input)
/////////////////////////////////////////////////////////////////////
int EXPORT_ATTR read_file_str( pvoid_t hFile, pvoid_t lpBuffer, long nBytesToRead, long *lpBytesRead)
{
	// Flag de Cr+Lf o Lf+Cr o Lf o Cr
	enum {fNone=0,fLF=1,fCR=2,fLFCR=4,fCRLF=8} eFlagCRLF = fNone;
	// Ptr a char
	char *ptrChar = (char *)lpBuffer;
	// Booleano de lectura x omision es TRUE al inicio
	boolean_t bRead = is_true;
	// Byte leido x omision es 1 al inicio
	long lByteRead = 1L;
	// Contador de bytes
	int  iCnt = 0;

	// Precondicion
	if (hFile == NULL || lpBuffer == NULL || nBytesToRead == 0 || lpBytesRead == NULL)
		return 0;

	// Inicializacion de la cadena
	*(char *)lpBuffer= (char) 0;
	// Lectura en si misma hasta Cr+Lf
	for(iCnt=0; iCnt < nBytesToRead && bRead == is_true && lByteRead > 0L; iCnt++)
	{
		// Ptr a char en buffer
		if(iCnt > 0)
			ptrChar = (char *)lpBuffer + iCnt;
		else
			ptrChar = (char *)lpBuffer;
		
		// De a 1 byte
		while((bRead = ReadFile( hFile, ptrChar, 1, &lByteRead, NULL )) && 
				lByteRead > 0 &&
				0 == *ptrChar &&
				0 == iCnt );

		// Chequeo de lectura valida
		if( lByteRead == 0L || bRead == is_false )
			// romper ciclo
			break;
		// Chequeo de NULL
		else if( 0 == *ptrChar && iCnt > 0)
			// romper ciclo
			break;
		// Chequeo de CarriageReturn
		else if( '\r' == *ptrChar )
			{
			// descartar caracter
			iCnt--;
			// Flag CR
			if(fNone == eFlagCRLF)
				eFlagCRLF = fCR;
			else
				eFlagCRLF = fLFCR;
			}
		// Chequeo de LineFeed
		else if( '\n' == *ptrChar )
			{
			iCnt--;
			// Flag LF
			if(fNone == eFlagCRLF)
				eFlagCRLF = fLF;
			else
				eFlagCRLF = fCRLF;
			}
		// No es Cr ni Lf ni NULL
		else
			{
			// Flag LF o CR previo?
			if(fNone != eFlagCRLF)
				// romper ciclo
				break;
			};//end-if

		// Cr+Lf o Lf+Cr? y no hay nada escrito
		if((fCRLF == eFlagCRLF || fLFCR == eFlagCRLF) && ptrChar == (char *)lpBuffer)
		{
			eFlagCRLF = fNone;
			// seguir ciclo
			continue;
		};

		// Cr+Lf o Lf+Cr?
		if(fCRLF == eFlagCRLF || fLFCR == eFlagCRLF) {
			iCnt++;
			// romper ciclo
			break;
		}

	};//end-for
	if(iCnt < 0)
		iCnt= 0;
	*((char *)lpBuffer + iCnt)= 0;
	// Bytes leidos
	*lpBytesRead= iCnt;
	// Ok
	return 1;
};


///////////////////////////////////////////////////////////////////// 
// Funcion de recuperacion del tamano del archivo
// hFile: handle valido de archivo abierto previamente (input)
/////////////////////////////////////////////////////////////////////
unsigned long EXPORT_ATTR get_file_size( char *lpFileName )
{
	// Variables auxiliares locales
	unsigned long	dwSizeLow  = 0L,
					dwSizeHigh = 0L,
					dwError    = 0L;	

	// Precondicion
	if(!lpFileName)
		return 0L;

	///////////////////////////////////////////////////////////////////////
	// Codigo extractado de un HelpOnLine del SDK de Win32
	///////////////////////////////////////////////////////////////////////
	// Try to obtain hFile's huge size. 
	dwSizeLow = GetCompressedFileSize (lpFileName, & dwSizeHigh) ;  
	if (dwSizeLow == 0xFFFFFFFF && (dwError = GetLastError()) != NO_ERROR )
		return 0;	
	// Return Size
	return dwSizeLow ;
	///////////////////////////////////////////////////////////////////////
    
};




// Used as a thread
#define PRINT_FILE_AS_THREAD
// Print buffer size
#define PRINTBUFFER_SIZE	4096

// Print File function
int PrintFile(const char *lpszFileName)
	{
#ifdef _WIN32
	ULONG		  ulLen                 = 0;        // I-O return value
	DWORD         dwJobId               = 0;        // JobId
	DWORD         dwError               = 0;        // Error Code
	HANDLE		  hPrinter              = NULL,	    // Printer handle
	      		  hFile                 = NULL;     // Input File handle
	PCHAR		  pszSeparator          = NULL;     // Comma separator
	BOOL		  bResult               = FALSE;    // Boolean result
	CHAR		  szDefaultPrinter[128];            // Default Printer,Driver,Spool
	CHAR		  szPrinterName[128];               // Printer Name
	BYTE		  bBuffer[PRINTBUFFER_SIZE] ;	    // Read-Write buffer
	MSG			  msg;							    // MS-Windows temporary message
	OSVERSIONINFO osvData;                          // O.S. version data
	int           iDocLevel             = 1;        // Document level (1=WinNT,2=Win95)	
	union 	{									    // Document info for Printing
			DOC_INFO_1	diData1;                    // Document info Level-1 WinNT
			DOC_INFO_2	diData2;				    // Document info Level-2 Win95
			}	  diDocInfo;	

	// Open Input File
	hFile = CreateFile( lpszFileName             ,  // File Name
	                    GENERIC_READ             ,  // Access mode
						FILE_SHARE_READ          ,  // Share mode
                        NULL                     ,  // Secutity Attributes
						OPEN_EXISTING            ,  // Creation mode
						FILE_FLAG_SEQUENTIAL_SCAN,  // File mode
                        NULL                     ); // Template
	if(hFile == NULL)
		return (-1);

	// Operating System Version data
	osvData.dwOSVersionInfoSize = sizeof osvData;
	if(!GetVersionEx( &osvData ))
		return(-1);

	// Default Printer Name, Driver and Spool
	GetProfileString(   "Windows",
						"Device",
						"PRN",
						szDefaultPrinter,
						sizeof szDefaultPrinter );
    // String is "Printer-Name,Device-Name,Spool-Name"
	if((pszSeparator = strchr( szDefaultPrinter, ',')) == NULL)
		return (-1);
	// Extract printer name, terminating with zero
	strncpy( szPrinterName, szDefaultPrinter, pszSeparator - szDefaultPrinter );
	szPrinterName[pszSeparator - szDefaultPrinter] = 0x00;

	// Open Printer
	bResult = OpenPrinter( szPrinterName, &hPrinter, NULL);		
	if(!bResult)
		{		
		dwError = GetLastError();		
		return (int)dwError;
		}
	
	// Segun Sistema Operativo corriendo...
	if( osvData.dwPlatformId == VER_PLATFORM_WIN32_NT ) // Windows NT
		{
		// Document properties
		iDocLevel = 1;
		diDocInfo.diData1.pDocName    = (LPTSTR)lpszFileName;
		diDocInfo.diData1.pOutputFile = NULL;
		diDocInfo.diData1.pDatatype   = NULL;
		}
	else if( osvData.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) // Windows 95
		{
		// Document properties
		iDocLevel = 2;
		diDocInfo.diData2.pDocName    = (LPTSTR)lpszFileName;
		diDocInfo.diData2.pOutputFile = NULL;
		diDocInfo.diData2.pDatatype   = NULL;
		diDocInfo.diData2.dwMode      = 0;
		diDocInfo.diData2.JobId		  = 0;
		}//end-doc-level

	// Start Document Printing
	dwJobId = StartDocPrinter( hPrinter, iDocLevel, (LPBYTE)&diDocInfo);
	// Error?
	if(dwJobId == 0) 
		{
		dwError = GetLastError();
		// Close Printer
		bResult = ClosePrinter( hPrinter );	
		return (int)dwError;
		}
	else 
		bResult = StartPagePrinter( hPrinter );
	
	// Write cycle
	while( ReadFile( hFile, (LPVOID)bBuffer, sizeof bBuffer, &ulLen, NULL ) 
	       && (ulLen > 0) 
		   && (bResult == TRUE) )
		{
		// Write buffer
		bResult = WritePrinter( hPrinter, bBuffer, ulLen, &ulLen);
		ulLen   = 0;
		// Yields control to other tasks after writing...
		while(PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
			{
		 	TranslateMessage( &msg );
		 	DispatchMessage( &msg );
		 	}//if
		}

	// Close Input File
	bResult = CloseHandle( hFile );
	// End Document Printing
	bResult = EndPagePrinter( hPrinter );
	bResult = EndDocPrinter( hPrinter );
	// Close Printer
	bResult = ClosePrinter( hPrinter );	
	
#ifdef PRINT_FILE_AS_THREAD
	// Return exit code
	ExitThread( (bResult == FALSE) ? (-1) : 0 );
	// Return result (to avoid compiler warnings)
	return ( (bResult == FALSE) ? (-1) : 0 );	
#else
	// Return result
	return ( (bResult == FALSE) ? (-1) : 0 );	
#endif // endif-post-printing

#else
	// Return error, not yet implemented
	return (-1);
#endif // endif-win32
	}

// Manejo de Impresion de Archivos
BOOL DoFilePrinting(LPSTR lpszFileName)
{	
#ifdef PRINT_FILE_AS_THREAD
	// Print file a thread...
	HANDLE  hThread    = NULL;   // Thread handle
	DWORD   dwThreadId = 0;	     // Thread Id
	DWORD   dwRetVal   = 0;      // Thread Return Value
	
	// Create the thread, but don´t start it...
	hThread = CreateThread(		   NULL ,  // Security Attributes
									  0 ,  // Stack size
       (LPTHREAD_START_ROUTINE)PrintFile,  // Routine
			(LPVOID)(LPSTR)lpszFileName ,  // Parameter
					   CREATE_SUSPENDED ,  // Creation Flags
							&dwThreadId ); // Thread Id
	if( hThread == NULL)
		return FALSE;
	// Start the thread
	if(ResumeThread( hThread ) == (-1L))
		return FALSE;
	// Wait a few miliseconds...
	Sleep( 100L );
	// Get Exit Code
	if(!GetExitCodeThread( hThread, &dwRetVal))
		return FALSE;
	// Return Value
	return ( dwRetVal == 0 || dwRetVal == STILL_ACTIVE) 
		? TRUE 
		: FALSE;
#else
	// Return Value
	return ( PrintFile(lpszFileName) == 0 )
			? TRUE
			: FALSE;
#endif //end-if-post-printing-as-thread
}//end-DoFilePrinting


///////////////////////////////////////////////////////////////////////////
// Funcion utilitaria que retorna la cantidad de bytes numericos en string
///////////////////////////////////////////////////////////////////////////
short EXPORT_ATTR  strnumlen( char *pszValue , const short nMax )
{
	short iLen = 0;
	// Mientras haya caracteres numericos, contar digitos, hasta un tope maximo
	for( iLen = 0; 
		pszValue != NULL && *pszValue >= '0' && *pszValue <= '9' && iLen < nMax ;
		iLen++ , pszValue++ ) 
			;
	// Retornar la longitud hallada
	return iLen;
}
///////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Funcion utilitaria que retorna la cantidad de bytes numericos en string
///////////////////////////////////////////////////////////////////////////
short EXPORT_ATTR  strnumlenpad( char *pszValue , const short nMax , char chPadding)
{
	short     iLen = 0;
	boolean_t bPadding = is_false;
	// Mientras haya caracteres numericos, contar digitos, hasta un tope maximo
	for( iLen = 0; 
		pszValue != NULL && 
			((*pszValue >= '0' && *pszValue <= '9') || *pszValue == chPadding) &&
			iLen < nMax ;
		iLen++ , pszValue++ ) 
		{
			// Se encontro el caracter de padding (gralmente. es un blanco) ?
			if(*pszValue == chPadding)
			{
				// Establecer inicio de padding, que no debe estar "roto" luego
				bPadding = is_true;
				continue;
			}
			// Numericos despues de un padding ? Error
			if(*pszValue >= '0' && *pszValue <= '9' && is_true == bPadding )
				// Terminar conteo
				break;
		}
	// Retornar la longitud hallada
	return iLen;
}
///////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//
// strsubstr
// Input: Puntero a String C (Cadena C).
// Output : Subcadena desde/hasta posicion (hasta 512 bytes)
// Return : Puntero a Subcadena C
//
/////////////////////////////////////////////////////////////////////
PCHAR EXPORT_ATTR strsubstr(char *pszBuf, int iFrom, int iSize)
{
	static CHAR szTempBuff[512+1] = {0x00};
	// Precondicion
	if( (NULL == pszBuf) || (iFrom < 0) || (iSize > 512) || (iSize <= 0))
		return NULL;
	// Copia global a buffer
	strncpy( szTempBuff, pszBuf + iFrom, iSize );
	szTempBuff[iSize] = 0x00;
	// Retorno subcadena
	return szTempBuff;
};

///////////////////////////////////////////////////////////////////////////
// Funcion utilitaria que saca espacios a IZQUIERDA
///////////////////////////////////////////////////////////////////////////
short EXPORT_ATTR  strltrim( char *pszValue , const short nMax)
{
	short iLen = 0 , jLen = 0;
	// Mientras haya ESPACIOS, descartarlos
	for( iLen = 0; 
		pszValue != NULL && *pszValue == ' ' && iLen < nMax ;
		iLen++ , pszValue++ ) 
		{
			strncpy( pszValue+iLen, pszValue+iLen+1,nMax-iLen);
			break;
		}
		
	// Retornar la longitud hallada
	return iLen;
}
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Funcion utilitaria que saca cualquier digito en cualquier posicion
///////////////////////////////////////////////////////////////////////////
short EXPORT_ATTR  strcutchar( char *pszValue , const short nMax, char chChar)
{
	short iLen = 0 , jLen = 0, lMax = nMax;
	// Precondicion
	if( NULL == pszValue )
		return 0;
	// Precondicion : Maximo local podria ser MENOR QUE Maximo general
	if(lMax > strlen(pszValue))
		lMax = strlen(pszValue);
	// Precondicion
	if(lMax <= 1)
		return 0;

	// Mientras haya ESPACIOS, descartarlos
	for( iLen = 0; 
		pszValue != NULL && iLen < lMax && iLen < nMax;
		iLen++ , pszValue++ ) 
		{
			/* char found, cut it */
			if( chChar == *pszValue)
			{
				strncpy( pszValue, pszValue+1,lMax-iLen);
				lMax -= 1;
			}
		}
	
	// Put end of string
	pszValue[iLen] = 0x00;
	// Retornar la longitud hallada
	return iLen;
}

///////////////////////////////////////////////////////////////////////////
// Funcion utilitaria que REEMPLAZA cualquier digito en cualquier posicion
///////////////////////////////////////////////////////////////////////////
short EXPORT_ATTR  strrepchar( char *pszValue , const short nMax, char chChar, char chNew)
{
	short iLen = 0 , jLen = 0, lMax = nMax;
	// Precondicion
	if(!pszValue)
		return 0;
	// Precondicion
	if(lMax <= 1)
		return 0;

	// Mientras haya ESPACIOS, descartarlos
	for( iLen = 0; 
		pszValue != NULL && iLen < lMax ;
		iLen++ , pszValue++ ) 
		{
			/* char found, cut it */
			if( chChar == *pszValue)
				(*pszValue) = chNew;
		}
	
	// Retornar la longitud hallada
	return iLen;
}

///////////////////////////////////////////////////////////////////////////////////
/*	alternar bytes en un short 16 bits, hacia LITTLE-ENDIAN desde BIG-ENDIAN     */
///////////////////////////////////////////////////////////////////////////////////
void	swapbyte(unsigned short *sh)
{
	register char	*cp = (char *)sh,
		            t   = 0;	
    /*****************************************/
	t         = *(cp);       /* save first   */
	*(cp)     = *(cp + 1);   /* first = last */
	*(cp + 1) = t;           /* last = saved */
	/*****************************************/
};

///////////////////////////////////////////////////////////////////////////////////////
/*	alternar bytes en un long 32 bits, hacia LITTLE-ENDIAN desde BIG-ENDIAN machines */
///////////////////////////////////////////////////////////////////////////////////////
void	swapbytel(unsigned long *lng)
{
	register char	*cp = (char *)lng,
		            t   = 0;	
	register short  i   = 0;
	for( i = 0; i <= 1; i++ )
	{
		t             = *(cp + i);      /* save first/second         */
		*(cp + i)     = *(cp + 3 - i);  /* first/second = last/thrid */
		*(cp + 3 - i) = t;              /* last/third   = saved      */
	};
	/********************************************/
	/* OUTPUT 'FOR' CICLES , CASE I=0 AND I=1 : */
	/********************************************/
	// t         = *(cp);       // save first   
	// *(cp)     = *(cp + 3);   // first = last 
	// *(cp + 3) = t;           // last = saved 
	//*******************************************
	// t         = *(cp + 1);   // save second   
	// *(cp + 1) = *(cp + 2);   // second = third
	// *(cp + 2) = t;           // third = saved 
	/********************************************/
};

//////////////////////////////////////////////////////////////////////////////////////////////
/*	alternar bytes en un fix-integer 64 bits, hacia LITTLE-ENDIAN desde BIG-ENDIAN machines */
//////////////////////////////////////////////////////////////////////////////////////////////
void	swapbytef(FIXEDINTEGER *fix)
{
	register char	*cp = (char *)fix,
		            t   = 0;	
	register short  i   = 0;
	for( i = 0; i <= 3; i++ )
	{
		t             = *(cp + i);      /* save first/...     */
		*(cp + i)     = *(cp + 7 - i);  /* first... = last... */
		*(cp + 7 - i) = t;              /* last...  = saved   */
	};
	/********************************************/
	/* OUTPUT 'FOR' CICLES , CASE I=0,1,2,3 :   */
	/********************************************/
	// t         = *(cp);       // save first   
	// *(cp)     = *(cp + 7);   // first = last 
	// *(cp + 7) = t;           // last = saved 
	//*******************************************
	// t         = *(cp + 1);   // ...
	// *(cp + 1) = *(cp + 6);   // 
	// *(cp + 6) = t;           // 
	/********************************************/
	// t         = *(cp + 2);   // ...
	// *(cp + 2) = *(cp + 5);   // 
	// *(cp + 5) = t;           // 
	/********************************************/
	// t         = *(cp + 3);   // ...
	// *(cp + 3) = *(cp + 4);   // 
	// *(cp + 4) = t;           // 
	/********************************************/
};

//////////////////////////////////////////////////////////////////////////////////////////////
/*	alternar bytes en un array de N bytes, hacia LITTLE-ENDIAN desde BIG-ENDIAN machines */
//////////////////////////////////////////////////////////////////////////////////////////////
void	swapbytearray(unsigned char **array, size_t array_size)
{
	register unsigned char	*cp = *array,
							t   = 0;	
	register short			i   = 0;
	register short			max = (array_size-1);
	for( i = 0; i <= max; i++ )
	{
		t               = *(cp + i);		/* save first/...     */
		*(cp + i)       = *(cp + max - i);  /* first... = last... */
		*(cp + max - i) = t;                /* last...  = saved   */
	};
};

/**********************************************/  
/* LOCAL-DATE-STRING to YYMMDD-STRING         */
/**********************************************/  
char *DateTimeToDateYYYYMMDD(const char *szDateAndTime, char *szYYYYMMDD   )
{   
   char nSep  = 0, /* flag for '/' , '.' or '-' */
        chDig = 0; /* char for '/' , '.' or '-' */

   /* precondition */
   if( !szDateAndTime || !szYYYYMMDD )
       return NULL;
   /* precondition : ¿ yyyymmdd ? */
   if( strlen(szDateAndTime) < 8)
       return NULL;

   /* check separators */
   if( strchr( szDateAndTime, '/') )
   {
       nSep = 1; chDig = '/';
   }
   else if( strchr( szDateAndTime, '-') )
   {
       nSep = 1; chDig = '-';
   }
   else if( strchr( szDateAndTime, '.') )
   {
       nSep = 1; chDig = '.';
   }
   else if( strchr( szDateAndTime, ':') )
   {
       nSep = 1; chDig = ':';
   };

   /* some basic format checking */
   if( (szDateAndTime[4] == '-' || szDateAndTime[4] == '/' || szDateAndTime[4] == '.') 
       &&
       (szDateAndTime[7] == '-' || szDateAndTime[7] == '/' || szDateAndTime[7] == '.')
     )
   {
      /* YYYY/MM/DD? */
      memcpy( szYYYYMMDD        , szDateAndTime                 , 4);  /* YEAR  */
      memcpy( szYYYYMMDD + 4    , szDateAndTime + 4 + nSep      , 2);  /* MONTH */
      memcpy( szYYYYMMDD + 4 + 2, szDateAndTime + 4 + 2 + 2*nSep, 2);  /* DAY   */
      szYYYYMMDD[4+2+2] = 0x00;
   }
   else if( (szDateAndTime[2] == '-' || szDateAndTime[2] == '/' || szDateAndTime[2] == '.')
            &&
            (szDateAndTime[5] == '-' || szDateAndTime[5] == '/' || szDateAndTime[5] == '.')
          )
   {
      /* DD-MM-YYYY? */
      if ( '.' == chDig || '-' == chDig )
      {
        memcpy( szYYYYMMDD        , szDateAndTime + 2 + 2 + 2*nSep, 4);  /* YEAR  */
        memcpy( szYYYYMMDD + 4    , szDateAndTime + 2 + nSep      , 2);  /* MONTH */
        memcpy( szYYYYMMDD + 4 + 2, szDateAndTime                 , 2);  /* DAY   */
        szYYYYMMDD[4+2+2] = 0x00;
      }
      /* MM/DD/YYYY? or DD/MM/YYYY? */
      else if ( '/' == chDig )
      {
        /* IT'S A KING OF MAGIC : guess if "mm/dd/yyyy" OR "dd/mm/yyyy"  */
        /* Local Settings USA : decimal_point "." - thousand_point "," ? */
	    const char *szLocale = setlocale( LC_ALL, "LC_MONETARY=;LC_NUMERIC=;" ); 
	    const struct lconv *pstConv = localeconv();
        /* ANGLOSAXON? LATIN? */
	    if(pstConv && (strcmp(pstConv->decimal_point,".")!=0 || 
		    strcmp(pstConv->thousands_sep,",")!=0))
        {
            /* DD/MM/YYYY */
            memcpy( szYYYYMMDD        , szDateAndTime + 2 + 2 + 2*nSep, 4);  /* YEAR  */
            memcpy( szYYYYMMDD + 4    , szDateAndTime + 2 + nSep      , 2);  /* MONTH */
            memcpy( szYYYYMMDD + 4 + 2, szDateAndTime                 , 2);  /* DAY   */
            szYYYYMMDD[4+2+2] = 0x00;
        }
        else
        {
            /* maybe MM/DD/YYYY - US.WEST-COAST/US.EAST-COAST? */
            memcpy( szYYYYMMDD        , szDateAndTime + 2 + 2 + 2*nSep, 4);  /* YEAR  */
            memcpy( szYYYYMMDD + 4    , szDateAndTime                 , 2);  /* MONTH */
            memcpy( szYYYYMMDD + 4 + 2, szDateAndTime + 2 + nSep      , 2);  /* DAY   */
            szYYYYMMDD[4+2+2] = 0x00;
        };
      };
   }
   else
   {     
      memcpy( szYYYYMMDD, szDateAndTime, 4+2+2);  /* YEAR+MONTH+DAY  */
      szYYYYMMDD[4+2+2] = 0x00;
   }
   return szYYYYMMDD;
};

/**********************************************/  
/* LOCAL-DATE-STRING to YYMMDD-STRING         */
/**********************************************/  
char *DateYYYYMMDDToDateTime(const char *szYYYYMMDD, char *szDateAndTime, char chDig   )
{           

   /* precondition */
   if( !szDateAndTime || !szYYYYMMDD || chDig <= ' ' )
       return NULL;
   /* precondition : ¿ yymmdd ? */
   if( strlen(szYYYYMMDD) < 6)
       return NULL;
   /* check size */
   if( strlen(szYYYYMMDD) == 6)
   {
      /* YY/MM/DD? ===> 20TH CENTURY? 21TH CENTURY? Y2K>=1975. */
      if( strncmp(szYYYYMMDD,"75",2) >= 0 )
         sprintf( szDateAndTime, "20%2.2s%c%2.2s%c%2.2s",szYYYYMMDD        , chDig, 
                                                         szYYYYMMDD + 2    , chDig,
                                                         szYYYYMMDD + 2 + 2);
      else
         sprintf( szDateAndTime, "20%2.2s%c%2.2s%c%2.2s",szYYYYMMDD        , chDig, 
                                                         szYYYYMMDD + 2    , chDig,
                                                         szYYYYMMDD + 2 + 2);
   }
   else if( strlen(szYYYYMMDD) == 8)
   {
        sprintf( szDateAndTime, "%4.4s%c%2.2s%c%2.2s"  , szYYYYMMDD        , chDig, 
                                                         szYYYYMMDD + 4    , chDig,
                                                         szYYYYMMDD + 4 + 2);
   }
   return szDateAndTime;
};

/* Conversion de monto RED LINK positivo y negativo */
/* Retorna el signo NEGATIVO (-1) o POSITIVO (+1)   */
int convert_extract_ammount( char *szExtAmmount , const size_t cEXTR_AMT_LEN)
{
	short nAuxLen    = min(strlen(szExtAmmount),cEXTR_AMT_LEN);
	char  chNegative = FALSE;

	// Segun ultimo caracter de cadena alfanumerica	
	switch(szExtAmmount[nAuxLen-1])
	 {
		// Positivos
		case '{' : szExtAmmount[nAuxLen-1] = '0'; break;
		case 'A' : szExtAmmount[nAuxLen-1] = '1'; break;
		case 'B' : szExtAmmount[nAuxLen-1] = '2'; break;
		case 'C' : szExtAmmount[nAuxLen-1] = '3'; break;
		case 'D' : szExtAmmount[nAuxLen-1] = '4'; break;
		case 'E' : szExtAmmount[nAuxLen-1] = '5'; break;
		case 'F' : szExtAmmount[nAuxLen-1] = '6'; break;
		case 'G' : szExtAmmount[nAuxLen-1] = '7'; break;
		case 'H' : szExtAmmount[nAuxLen-1] = '8'; break;
		case 'I' : szExtAmmount[nAuxLen-1] = '9'; break;
		// Negativos
		case '}' : szExtAmmount[nAuxLen-1] = '0'; chNegative = TRUE; break;
		case 'J' : szExtAmmount[nAuxLen-1] = '1'; chNegative = TRUE; break;
		case 'K' : szExtAmmount[nAuxLen-1] = '2'; chNegative = TRUE; break;
		case 'L' : szExtAmmount[nAuxLen-1] = '3'; chNegative = TRUE; break;
		case 'M' : szExtAmmount[nAuxLen-1] = '4'; chNegative = TRUE; break;
		case 'N' : szExtAmmount[nAuxLen-1] = '5'; chNegative = TRUE; break;
		case 'O' : szExtAmmount[nAuxLen-1] = '6'; chNegative = TRUE; break;
		case 'P' : szExtAmmount[nAuxLen-1] = '7'; chNegative = TRUE; break;
		case 'Q' : szExtAmmount[nAuxLen-1] = '8'; chNegative = TRUE; break;
		case 'R' : szExtAmmount[nAuxLen-1] = '9'; chNegative = TRUE; break;
		default  : break;
	 };

	// Adicionar signo negativo cuando corresponda
	if (TRUE == chNegative && '0'==szExtAmmount[0])
		szExtAmmount[0] = '-';
	// FIX
	return (TRUE == chNegative)
		? -1
		: +1;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// right-trim
// FIX : version con buffer local estatico
//////////////////////////////////////////////////////////////////////////////////////////////
char *rtrim(const char* szPalabra , const short nMaxLen )
{
	/* indice auxiliar y maximo auxiliar */
	int   i = 0;		
	/* buffer dinamico a liberar posteriormente  */
	char *szReturn = NULL;	

	/* FIX : precondicion */
	if( nMaxLen > 1024)
		return NULL;
	/* Alojamiento dinamico */
	szReturn = malloc( nMaxLen + 1 );
	/* FIX : precondicion */
	if( !szReturn )
		return NULL;

	/* FIX : agrego un limite maximo a 'pelar' de blancos, sino... agarrate catalina */
	for (i = 0; szPalabra[i] != '\0' && szPalabra[i] != ' ' && i < nMaxLen ; i++)
	{
		szReturn[i] = szPalabra[i];
	}
	/* NULL al final */
	szReturn[i] = '\0';
	/* Retorno puntero local con direccion externa */
	return ( szReturn );
}
