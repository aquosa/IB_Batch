/*******************************************************************************************************/
/* COA S.A.                                                                                            */
/* EBCDIC-ASCII y ASCII-EBCDIC segun tabla de EDS/INTERBANKING                                         */
/*                                                                                                     */
/* Tarea        Fecha           Autor   Observaciones                                                  */
/* (Beta)       2004.01.07      mdc     Inicial                                                        */
/* (Beta)       2006.03.27      mdc     CR+LF ?                                                        */
/* (Beta)       2006.05.15      mdc     VERSION CON METODO EXPORTABLE                                  */
/* (Beta)       2006.05.17      mdc     Get_EBCDIC_ASCII_TABLE( ... )                                  */
/* (Release-3) 	2006.05.29		mdc		STDIOL.h con Get_EBCDIC_ASCII_BYTE( unsigned char uchBYTE ) ;  */
/* (Release-4) 	2006.08.01		mdc		'E','e' y 'A','a' con o sin CR+LF.                             */
/*******************************************************************************************************/


/* ANSI headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <QUSRINC/EBC2ASCI.H>

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

#ifndef FALSE
#define FALSE (0)
#define TRUE  (!FALSE)
#endif
/*****************************************************************************************/

/****************************************************/
/*      TABLA DE CONVERSION DE EBCDIC A ASCII       */
/*            DATANET (TABASC)                      */
/****************************************************
    -0 -1 -2 -3 -4 -5 -6 -7   -8 -9 -A -B -C -D -E -F
0-   00 01 02 03 80 09 81 7F - 82 83 0A 0B 0C 0D 0E 0F
1-   10 11 12 13 85 86 08 87 - 18 19 88 89 1C 1D 1E 1F
2-   8A 8B 8C 8D 8E 84 17 1B - 8F 90 91 92 93 05 06 07
3-   94 95 16 96 97 98 99 04 - 9A 9B 9C 9D 14 15 9E 1A
4-   20 9F A0 A1 A2 A3 A4 A5 - A6 A7 5B 2E 3C 28 2B 5D
5-   26 A8 A9 AA AB AC AD AE - AF B0 21 24 2A 29 3B 5E
6-   2D 2F B1 B2 B3 B4 B5 B6 - B7 B8 7C 2C 25 5F 3E 3F
7-   B9 BA BB BC BD BE BF C0 - C1 60 3A 23 40 27 3D 22
8-   C2 61 62 63 64 65 66 67 - 68 69 C3 C4 C5 C6 C7 C8
9-   C9 6A 6B 6C 6D 6E 6F 70 - 71 72 CA CB CC CD CE CF
A-   D0 7E 73 74 75 76 77 78 - 79 7A D1 D2 D3 D4 D5 D6
B-   D7 D8 D9 DA DB DC DD DE - DF E0 E1 E2 E3 E4 E5 E6
C-   7B 41 42 43 44 45 46 47 - 48 49 E7 E8 E9 EA EB EC
D-   7D 4A 4B 4C 4D 4E 4F 50 - 51 52 ED EE EF F0 F1 F2
E-   5C F3 53 54 55 56 57 58 - 59 5A F4 F5 F6 F7 F8 F9
F-   30 31 32 33 34 35 36 37 - 38 39 FA FB FC FD FE FF
 *****************************************************/


const unsigned char  EXPORT_ATTR EBCDIC_ASCII_TABLE[] = {  
  /* -0   -1   -2   -3   -4   -5   -6   -7   -8   -9   -A   -B   -C   -D   -E   -F  */
    0x00,0x01,0x02,0x03,0x80,0x09,0x81,0x7F,0x82,0x83,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F, /*0-*/
    0x10,0x11,0x12,0x13,0x85,0x86,0x08,0x87,0x18,0x19,0x88,0x89,0x1C,0x1D,0x1E,0x1F, /*1-*/
    0x8A,0x8B,0x8C,0x8D,0x8E,0x84,0x17,0x1B,0x8F,0x90,0x91,0x92,0x93,0x05,0x06,0x07, /*2-*/
    0x94,0x95,0x16,0x96,0x97,0x98,0x99,0x04,0x9A,0x9B,0x9C,0x9D,0x14,0x15,0x9E,0x1A, /*3-*/
    0x20,0x9F,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0x5B,0x2E,0x3C,0x28,0x2B,0x5D, /*4-*/
    0x26,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0x21,0x24,0x2A,0x29,0x3B,0x5E, /*5-*/
    0x2D,0x2F,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0x7C,0x2C,0x25,0x5F,0x3E,0x3F, /*6-*/
    0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0x60,0x3A,0x23,0x40,0x27,0x3D,0x22, /*7-*/
    0xC2,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8, /*8-*/
    0xC9,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, /*9-*/
    0xD0,0x7E,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6, /*A-*/
    0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6, /*B-*/
    0x7B,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC, /*C-*/
    0x7D,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0xED,0xEE,0xEF,0xF0,0xF1,0xF2, /*D-*/
    0x5C,0xF3,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9, /*E-*/
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF  /*F-*/
    } ;

const unsigned char  EXPORT_ATTR ASCII_EBCDIC_TABLE[] = {   
  /* -0   -1   -2   -3   -4   -5   -6   -7   -8   -9   -A   -B   -C   -D   -E   -F  */
    0x00,0x01,0x02,0x03,0x37,0x2D,0x2E,0x2F,0x16,0x05,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F, /*0-*/
    0x10,0x11,0x12,0x13,0x3C,0x3D,0x32,0x26,0x18,0x19,0x3F,0x27,0x1C,0x1D,0x1E,0x1F, /*1-*/
    0x40,0x5A,0x7F,0x7B,0x5B,0x6C,0x50,0x7D,0x4D,0x5D,0x5C,0x4E,0x6B,0x60,0x4B,0x61, /*2-*/
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0x7A,0x5E,0x4C,0x7E,0x6E,0x6F, /*3-*/
    0x7C,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6, /*4-*/
    0xD7,0xD8,0xD9,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0x4A,0xE0,0x4F,0x5F,0x6D, /*5-*/
    0x79,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x91,0x92,0x93,0x94,0x95,0x96, /*6-*/
    0x97,0x98,0x99,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xC0,0x6A,0xD0,0xA1,0x07, /*7-*/
    0x04,0x06,0x08,0x09,0x25,0x14,0x15,0x17,0x1A,0x1B,0x20,0x21,0x22,0x23,0x24,0x28, /*8-*/
    0x29,0x2A,0x2B,0x2C,0x30,0x31,0x33,0x34,0x35,0x36,0x38,0x39,0x3A,0x3B,0x3E,0x41, /*9-*/
    0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58, /*A-*/
    0x59,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76, /*B-*/
    0x77,0x78,0x80,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, /*C-*/
    0xA0,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8, /*D-*/
    0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xDA,0xDB,0xDC, /*E-*/
    0xDD,0xDE,0xDF,0xE1,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF  /*F-*/
    } ;

/*******************************************************************/                             
/* CONVERT THE STRING, BYTE PER BYTE, FROM TABLE                   */
/*******************************************************************/                             
unsigned long Convert_EBCDIC_ASCII (char *szTEXT , unsigned long nLen)
{
    unsigned char *pszTEXT= (unsigned char *)szTEXT;
    unsigned long inx = 0;
    for(inx = 0; inx < nLen ; pszTEXT++ , inx++)
        (*pszTEXT) = EBCDIC_ASCII_TABLE[ (*pszTEXT) ];
    return inx;
}

/*******************************************************************/                             
/* CONVERT THE STRING, BYTE PER BYTE, FROM TABLE                   */
/*******************************************************************/                             
unsigned long Convert_ASCII_EBCDIC (char *szTEXT , unsigned long nLen)
{
    unsigned char *pszTEXT = (unsigned char *)szTEXT;
    unsigned long inx = 0;
    for(inx = 0; inx < nLen ; pszTEXT++ , inx++)
        (*pszTEXT) = ASCII_EBCDIC_TABLE[ (*pszTEXT) ];
    return inx;
}

/*******************************************************************/                             
/* CONVERT 1 BYTE FROM TABLE                                       */
/*******************************************************************/                             
unsigned char Get_EBCDIC_ASCII_BYTE( unsigned char uchBYTE ) 
{
	return EBCDIC_ASCII_TABLE[ uchBYTE ];
}

/*******************************************************************/                             
/* CONVERT 1 BYTE FROM TABLE                                       */
/*******************************************************************/                             
unsigned char Get_ASCII_EBCDIC_BYTE( unsigned char uchBYTE ) 
{
	return ASCII_EBCDIC_TABLE[ uchBYTE ];
}


/*******************************************************************/
/* CONVERT ASCII EBCDIC FILE                                       */
/*******************************************************************/
/* char *szTrxFilename = NOMBRE DE ARCHIVO                         */
/* char eEBCDICASCII = 'E' = EBCDIC, 'e'=EBCDIC s/CR+LF          */
/*                       'A' = ASCII , 'a'=ASCII  s/CR+LF          */
/* long lRecordLen     = LONG. DE REGISTRO = 590/614               */
/* long *plCounter     = CANT. DE REGISTROS DE SALIDA              */
/*******************************************************************/
int CONVERT_ASCII_EBCDIC_FILE( char               *szTrxFilename , 
                               enumEBCDIC_ASCII_t eEBCDICASCII , 
                               long lRecordLen    , 
                               long *plCounter    ,
                               char               *szExtOutputFilename )
    {
    FILE *hFile    = NULL,
         *hFileOut = NULL;
    char szOutputFile[256];
    size_t nFileLen    = 0,
           nFileLenOut = 0,
           nLen        = 0;
    long   lCounter    = 0,
           lOffset     = 0;
    char    *pszBuffer = NULL;
    unsigned char bUseCRLF = FALSE;
    const    char szCRLF[] = {"\r\n"};  // Cr+Lf = 0x0D + 0x0A
    const    char szEOF[]  = { 0x88, 0x00 } ; // OS Digital=88, OS Ms-DOS=1A (ctrl-z)

    /********************************************/
    /* precondicion                             */
    /********************************************/
    if(!(EBCDIC_2_ASCII         == eEBCDICASCII 
         ||
         EBCDIC_2_ASCII_NO_CRLF == eEBCDICASCII
         ||
         ASCII_2_EBCDIC         == eEBCDICASCII 
         ||
         ASCII_2_EBCDIC_NO_CRLF == eEBCDICASCII)
       )
       return (-1);
    /********************************************/
    /* precondicion                             */
    /********************************************/
    if( !szTrxFilename  
        ||
        lRecordLen <= 0
        ||
        !plCounter )
       return (-1);

    /* apertura de archivos */
	hFile = fopen(szTrxFilename, "rb");
	if(!hFile)
    {        
		return (-2);
    }
    else
    {
        /***********************************************/
        if( szExtOutputFilename )
            strcpy( szOutputFile, szExtOutputFilename );        
        else
        /***********************************************/
        {
            strcpy(szOutputFile, szTrxFilename);
            strcat(szOutputFile, ".out");
        };
	    hFileOut = fopen(szOutputFile, "wb");
	    if(!hFileOut)
        {            
		    return (-3);
        };
    };

    /* verificar si se puede leer todo el archivo de una sola vez */
    fseek(hFile, 0L, SEEK_END);    
    nFileLen = ftell(hFile);
    fseek(hFile, 0L, SEEK_SET);    
    if(0 == nFileLen)
    {        
		return (-4);
    };
    pszBuffer = (char *)malloc( nFileLen );
    if(!pszBuffer)
    {        
		return (-5);
    };


    /************************************************/
    /* EBCDIC-ASCII  ?                              */
    /************************************************/
    nLen = fread( pszBuffer, nFileLen, 1, hFile ) ;
    if( EBCDIC_2_ASCII == eEBCDICASCII 
        ||
        EBCDIC_2_ASCII_NO_CRLF == eEBCDICASCII)
    {
        /* A-2-E */
        Convert_EBCDIC_ASCII( pszBuffer, nFileLen );
        /* CR+LF ? */
        if( EBCDIC_2_ASCII_NO_CRLF == eEBCDICASCII )
            bUseCRLF = FALSE;
        /* CR+LF ? */
        else if( EBCDIC_2_ASCII == eEBCDICASCII )
            bUseCRLF = TRUE;
    }
    else if( ASCII_2_EBCDIC == eEBCDICASCII 
             ||
             ASCII_2_EBCDIC_NO_CRLF == eEBCDICASCII)
    {
        /* A-2-E */
        Convert_ASCII_EBCDIC( pszBuffer, nFileLen );
        /* CR+LF  = Jamas en los envios hacia DNET */
        if( ASCII_2_EBCDIC_NO_CRLF == eEBCDICASCII 
            ||
            ASCII_2_EBCDIC         == eEBCDICASCII )
            /* FALSO */
            bUseCRLF = FALSE;
        else
            /* FALSO */
            bUseCRLF = FALSE;
    }
    else
        return (-6);
    /************************************************/

    /*************************************************/
    /* lectura, conversion EBCDIC-ASCII, y grabacion */
    /*************************************************/
    for(lCounter = 0, nFileLenOut = 0, lOffset = 0; 
        lOffset <= (nFileLen-lRecordLen) ; 
        lCounter++ , lOffset += lRecordLen, nFileLenOut += (lRecordLen + 2) )
    {
        nLen = fwrite( pszBuffer+lOffset, lRecordLen, 1, hFileOut ) ;
		if( bUseCRLF )
			nLen = fwrite( szCRLF, 2, 1, hFileOut ) ;
		if( 0==nLen )
			break;
    } ;
    /*************************************************/
    
    /***************/
    /* End-of-file */
    /***************/
	if( bUseCRLF )
	{
		nLen = fwrite( szEOF, 1, 1, hFileOut ) ;  
		nFileLenOut++;
	};
	
    /* desalojo de memoria */
    free( pszBuffer ) ;
    pszBuffer = NULL;

    /* cierre de archivos */
    fclose( hFileOut );
	hFileOut = NULL;
    fclose( hFile );
	hFile = NULL;

    /* cantidad de registros */
    if(plCounter)
        (*plCounter) = lCounter ;

    /********/
    /* OK ! */
    /********/
    return 0;
    };



