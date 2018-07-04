/**********************************************************************/
/* COA S.A.                                                           */
/* EBCDIC-ASCII y ASCII-EBCDIC segun tabla de EDS/INTERBANKING        */
/*                                                                    */
/* Tarea        Fecha           Autor   Observaciones                 */
/* (Beta)       2004.01.07      mdc     Inicial                       */
/* (Beta)       2006.03.27      mdc     CR+LF ?                       */
/* (Beta)       2006.05.15      mdc     VERSION CON METODO EXPORTABLE */
/* (Beta)       2006.08.14      mdc     Bug del 3er. parametro        */
/**********************************************************************/


/* ANSI headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <qusrinc/ebc2asci.h>


/**************************************************/
/* Mensajes */
/**************************************************/

const char szCopyright[]  = "EBC2ASCII - COA S.A. \r\n"
                            "Conversor de EBCDIC a ASCII segun tabla de codigos DATANET\r\n"
                            "@ BANCO MACRO BANSUD S.A., BANCO CIUDAD DE BS.AS. , BANCO FINANSUR S.A.\r\n"
                            "@ BANCO HIPOTECARIO S.A. , BANCO DE TIERRA DEL FUEGO S.A.\r\n";
const char szIO_Error[]   = "Error al intentar abrir el archivo <%s>:<%i>\r\n";
const char szSEEK_Error[] = "Error al posicionarse en el archivo <%s>:<%i>\r\n";
const char szARGC_Error[] = "Argumentos invalidos,especifique <archivo> <longitud>.\r\n"
                            "  Ej. EBC2ASCI.exe TRANSFER.BIN\r\n"
                            "  ( por omision convierte desde EBCDIC hacia ASCII, en registros de 590 bytes )\r\n"
                            "  Opcionalmente 2do. parametro : 590 (quinientos noventa bytes)\r\n" 
                            "  Y 3er. parametro             : 'E'=Hacia EBCDIC , 'A'=Hacia ASCII\r\n\r\n";
const char szMEM_Error[]  = "Error de memoria, no se pueden alocar todo el archivo.\r\n";
const char szEND_Message[]= "Registros convertidos <%i>. Bytes <%i>.\r\n";

#ifndef FALSE
#define FALSE (0)
#define TRUE  (!FALSE)
#endif



/* Main function */
int main(int argc, char *argv[], char *envp[])
	{
	char    szTrxFilename[256]    = {0x00};                
    char    szOutputFilename[256] = {0x00};                
	int		iRetval = 0;
	long	nLen       = 0,
            lCounter   = 0,           
            lRecordLen = 590;
    char    chEbcdic2Ascii = 'E'; // E=EBCDIC,A=ASCII

    printf(szCopyright);

    /* control de argumentos */
    if(argc < 2 )
    {
        printf(szARGC_Error);
        return -1;
    }

    if(argc >= 2)
    {
        strcpy(szTrxFilename, argv[1]);
    }
    if(argc >= 3)
    {
        lRecordLen = atol( argv[2] ) ;
        if( 0 == lRecordLen )
        {
	    	lRecordLen = 590;        
            /********************************/
            chEbcdic2Ascii = argv[2][0] ;
            if(!('A' == chEbcdic2Ascii 
                ||
                'a' == chEbcdic2Ascii 
                ||
                'E' == chEbcdic2Ascii 
                ||
                'e' == chEbcdic2Ascii 
                ))
            {
                printf(szARGC_Error);
		        return -1;
            };
            /********************************/
        };        
    };
    if(argc >= 4)
    {
        /********************************/
        chEbcdic2Ascii = argv[3][0] ;
        if(!('A' == chEbcdic2Ascii 
            ||
            'a' == chEbcdic2Ascii 
            ||
            'E' == chEbcdic2Ascii 
            ||
            'e' == chEbcdic2Ascii 
            ))
        {
            printf(szARGC_Error);
		    return -1;
        };
        /********************************/
    };


    /***************************************************************************/
    if( lRecordLen <= 0  )
	    lRecordLen = 590;
    /***************************************************************************/
    strcpy(szOutputFilename, szTrxFilename);
    strcat(szOutputFilename, ".out");
    /***************************************************************************/
    iRetval = CONVERT_ASCII_EBCDIC_FILE( szTrxFilename   , 
                                        chEbcdic2Ascii   , 
                                        lRecordLen       ,
                                        &lCounter        , 
                                        szOutputFilename );
    /***************************************************************************/

    switch (iRetval)
    {
        case -1 :      printf(szARGC_Error );                         exit(iRetval);break;
        case -2 :      printf(szIO_Error, szTrxFilename, errno);      exit(iRetval);break;
        case -3 :      printf(szIO_Error, szOutputFilename, errno);   exit(iRetval);break;
        case -4 :      printf(szSEEK_Error, szOutputFilename, errno); exit(iRetval);break;
        case -5:       printf(szMEM_Error );                          exit(iRetval);break;
        case -6:       printf(szARGC_Error );                         exit(iRetval);break;
        default:       break;
    };

    printf(szEND_Message, lCounter , lCounter * lRecordLen);

    /* OK */
    return 0;
} ;

