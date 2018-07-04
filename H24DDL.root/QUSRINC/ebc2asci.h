/*******************************************************************************************************/
/* IT24 SISTEMAS SL / COA SA / MARTIN D CERNADAS                                                      */
/* EBCDIC-ASCII y ASCII-EBCDIC segun tabla de EDS/INTERBANKING                                         */
/*                                                                                                     */
/* Tarea        Fecha           Autor   Observaciones                                                  */
/* (Release-4) 	2006.08.01		mdc		'E','e' y 'A','a' con o sin CR+LF.                             */
/*******************************************************************************************************/
#ifndef _EBCDIC_ASCII_LIB_
#define _EBCDIC_ASCII_LIB_

typedef enum {  ASCII_2_EBCDIC         = 'A',
                ASCII_2_EBCDIC_NO_CRLF = 'a',
                EBCDIC_2_ASCII         = 'E',
                EBCDIC_2_ASCII_NO_CRLF = 'e' }
    enumEBCDIC_ASCII_t ;


#ifdef __cplusplus
    extern "C" { 
#endif // __cpusplus


/*******************************************************************/
/* CONVERT ASCII EBCDIC FILE                                       */
/*******************************************************************/
/* char *szTrxFilename = NOMBRE DE ARCHIVO                         */
/* char chEbcdic2Ascii = 'E' = EBCDIC, 'e'=EBCDIC s/CR+LF          */
/*                       'A' = ASCII , 'a'=ASCII  s/CR+LF          */
/* long lRecordLen     = LONG. DE REGISTRO = 590/614               */
/* long *plCounter     = LONG. DE ARCHIVO DE SALIDA                */
/*******************************************************************/
int CONVERT_ASCII_EBCDIC_FILE( char               *szTrxFilename  , 
                               enumEBCDIC_ASCII_t eEBCDICASCII    , 
                               long lRecordLen                    , 
                               long *plCounter                    ,
                               char               *szExtOutputFilename );

#ifdef __cplusplus
    }
#endif // __cpusplus


#endif // _EBCDIC_ASCII_LIB_
