//                                                              
// IT24 Sistemas S.A.
// Standard I/O Library Extension (STDIOL)
//
// Tarea		Fecha			Autor	Observaciones
// (Inicial)	2005.05.08		mdc		http://www.codecomments.com/archive263-2004-11-326189.html
// (Inicial)	2005.05.10		mdc		PackedDecimalToString(), NumericToPackedDecimal(), DateTimeToDateYYYYMMDD()
// (Inicial)	2005.05.17		mdc		CHECK IF MUST BE MASKED FOR 'EBCDIC' POST-CONVERSION 
// (Release-3) 	2006.05.29		mdc		EXTERN unsigned char EXPORT_ATTR GetEBCDIC_ASCII( unsigned char uchBYTE ) ;
// (Release-3) 	2006.05.29		mdc		*** FIX *** NumericToPackedDecimal( ... ) de signo negativo, long. impar.
//


// Standar Headers C/C++
#include <limits.h>
#if CHAR_BIT != 8
#error "This code requires 8-bit bytes."
#endif
#include <string.h>
#include <stddef.h>
#include <memory.h>
#include <locale.h>

// Headers Estandar para Sistemas Internos COA-SYSASAP-IT24
#include <qusrinc/stdiol.h>

/* IMPORTABLE METHODS */
extern unsigned char Get_EBCDIC_ASCII_BYTE( unsigned char uchBYTE ) ;
extern unsigned char Get_ASCII_EBCDIC_BYTE( unsigned char uchBYTE ) ;


/*  Convert text-representation integers to packed-decimal representation.

    The text representation must be a sequence of decimal digits, with an
    optional leading sign; if no sign character is present, the packed-
    decimal representation is marked as unsigned. The packed-decimal
    representation is placed in a caller-supplied buffer.

    Returns 0 on errors, and the length of the packed representation on
    success. 

    http://www.codecomments.com/archive263-2004-11-326189.html

*/

/******************************************************/  
/* Basic Packed Decimal to STRING - no type checking  */
/******************************************************/  
size_t TextToPacked(unsigned char *Packed, const char *Text, size_t BufSize)

{
    char Sign     = 0x0F;
    size_t Digits = 0;
    unsigned char *PkdPtr = Packed;

    /* Validate parameters */
    if (! Packed || ! Text)
		return 0;

    /* Check for a sign character */
    switch (*Text)
    {
        case '+':
            Sign = 0x0C;
            Text++; /* skip past sign character */
            break;
        case '-':
            Sign = 0x0D;
            Text++; /* skip past sign character */
            break;
        default:
            break;
    };      

    /* Count the number of digits to see if we need to pad or if the
    destination is too small. */
    Digits = strlen(Text);

    /* Number of destination bytes needed: digits, plus sign, plus padding
    if required, all divided by two. */
    if ((Digits + 1 + !(Digits%2))/2 > BufSize)
        return 0;

    /* Make sure the text string is well-formed. */
    if (Digits < 1 || Digits != strspn(Text, "0123456789"))
        return 0;

    /* If there is an even number of digits, convert the first digit and
    place it in the first packed byte with four bits of padding. This
    reduces the even-digit case to the odd-digit case. */

    if (! (Digits % 2))
        *PkdPtr++ = *Text++ - '0';

    /* Convert the remaining digits two at a time, except for the last. */
    while (Text[1])
    {
        *PkdPtr = (Text[0] - '0') << 4 | (Text[1] - '0');
        PkdPtr += 1;
        Text += 2;
    };

    /* Convert the final remaining digit and the sign. */
    *PkdPtr++ = (*Text - '0' << 4) | Sign;

    return PkdPtr - Packed;
};


/**********************************************/  
/* Packed Decimal to STRING                   */
/**********************************************/  
size_t PackedDecimalToString (unsigned char *szPacked, size_t nPackSize, 
                              char          *szText  , size_t nBufSize ,
                              char          chPaddingChar ,
							  boolean_t	    bUnmaskFromEBCDIC)
{   
    register unsigned char Sign  = 0x0F;
    register int           iPack = nPackSize,
		                   iText = 0;

    /* Validate parameters */
    if (! szPacked || ! szText)
	    return 0;

    /* Check for a sign character */
    if( bUnmaskFromEBCDIC )
        Sign = Get_ASCII_EBCDIC_BYTE( szPacked[nPackSize-1] ) << 4 ;
    else
        Sign = szPacked[nPackSize-1] << 4 ;
    switch ( Sign )
    {
        case 0xc0:
            Sign = '+';
			szText[iText++] = Sign;            
            break;
        case 0xd0:
            Sign = '-';
			szText[iText++] = Sign;
            break;
        default:
            ;
            break;
    }      

    /* Convert the remaining digits two at a time, except for the last. */
    for ( iPack = 0; iPack <= (nPackSize-1); iPack++ )
    {		
		/* Unmask from EBCDIC ? */
		if( bUnmaskFromEBCDIC )
			Sign = Get_ASCII_EBCDIC_BYTE( szPacked[iPack] ) >> 4 ;        
		else
			Sign = szPacked[iPack] >> 4 ;        
		szText[iText++] = Sign  + '0' ;

        /* Skip the Sign */
        if( iPack != (nPackSize-1) )
        {
			/* Unmask from EBCDIC ? */
			if( bUnmaskFromEBCDIC )
				Sign = Get_ASCII_EBCDIC_BYTE( szPacked[iPack] ) << 4;
			else
				Sign = szPacked[iPack] << 4 ;
            Sign = Sign >> 4 ;
		    szText[iText++] = Sign  + '0' ; 		
        };
        
    };
    for ( ; iText < nBufSize; iText++ )
    {		
		szText[iText] = chPaddingChar ;
        
    };

    /* Return */
    return iText;

};

/**********************************************/  
/* STRING to Packed Decimal - PIC S9(15)V99 - */
/**********************************************/  
size_t NumericToPackedDecimal(unsigned char *szPacked, size_t nPackSize, 
                              char          *szText  , size_t nBufSize ,
                              char          chPaddingChar ,
							  boolean_t     bMaskForEBCDIC )
{   
   register size_t nLen        = 0;  /* INPUT STR LENGTH */
   /* Range : 0:0xffffffffffffffff (20 digits) */
   FIXEDINTEGER    fixAmmount  = 0;  /* 64-bit INTEGER - non-Standard (Microsoft format) */
   long            lDecimals   = 0;  /* 32-bit INTEGER */
   char            *ptrComma   = NULL; /* PTR to "comma" */
   const size_t    cnMaxSize   = 19 ;  /* MAX : 19 DECIMAL DIGITS : 9 BYTES (WITH SIGN) */
   char            szAuxText[32];      /* AUX BUFFER FOR ODD-EVEN LENGTH + SIGN */
   const char      cszFIXEDINTEGERMASK[] = "%I64i%02i";                   

   /* PRECONDITION : Local Settings USA : decimal_point "." - thousand_point "," */
   const char *szLocale = setlocale( LC_ALL, "LC_MONETARY=;LC_NUMERIC=;" ); 
   const struct lconv *pstConv = localeconv();

   if(pstConv && (strcmp(pstConv->decimal_point,".")!=0 || 
		strcmp(pstConv->thousands_sep,",")!=0))
   {		
		strrepchar(szText, (short)nBufSize, '.', ',');
   };

   /* PRECONDITION : 2 DECIMAL DIGITS AFTER COMMA */
   if( (ptrComma = strchr( szText , pstConv->decimal_point[0] )) != NULL )
       {
            /* CHECK FOR 1 COMMA AND 2 DEC. PLACES */
            if(( ptrComma + 1 + 2 ) != 0x00 ) 
            {
                *(ptrComma + 1 + 2) = 0x00;
                nBufSize = strlen(szText);
            };
       };
   
   /* PRECONDITION : VIRTUAL DECIMAL POINT : PIC S9(15)V99 */
   fixAmmount = antofix( szText  , nBufSize );   
   if( ptrComma + 1 != NULL )
        lDecimals  = antoi  ( ptrComma + 1 , nBufSize );
   sprintf( szAuxText, cszFIXEDINTEGERMASK, fixAmmount , lDecimals );
   nBufSize = strlen( szAuxText );

   /* PRECONDITION : MAX 19 DIGITS LENTH  */
   if( nBufSize > cnMaxSize )
       return 0;


   /* BASIC CONVERTION TO PACKED-DEC */
   nLen = TextToPacked( szPacked, szAuxText, nBufSize);   
   if( 0 == nLen )
       return 0;
   if( nPackSize - nLen < 0)
       return 0;

   /* RIGHT JUSTIFY WITH PADDING (FF - 00) */  
   memmove( szPacked + (nPackSize - nLen), szPacked,  nLen );
   memset(  szPacked, chPaddingChar, nPackSize - nLen );

   /* CHECK IF MUST BE MASKED FOR 'EBCDIC' POST-CONVERSION */   
   if( bMaskForEBCDIC )
   {
	    /* Convert from EBCDIC-2-ASCII , for further post-conversion ASCII-2-EBCDIC */
		for ( nLen = 0; nLen < nPackSize; nLen++ )
			szPacked[nLen] = Get_EBCDIC_ASCII_BYTE( szPacked[nLen] ); 
   };

   /* RETURN SIZE */
   return nPackSize;
};

// EBCDIC-ASCII byte
unsigned char GetEBCDIC_ASCII( unsigned char uchBYTE ) 
{
    return Get_EBCDIC_ASCII_BYTE( uchBYTE );
};

// ASCII-EBCDIC byte
unsigned char GetASCII_EBCDIC( unsigned char uchBYTE ) 
{
    return Get_ASCII_EBCDIC_BYTE( uchBYTE );
};

