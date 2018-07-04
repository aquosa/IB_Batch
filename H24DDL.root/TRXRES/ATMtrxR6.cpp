////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// Process ATM and POS Transaction Method, Base24 R6
//
//   Metodo de Procesamiento de Transaccion POS para NEAT, usando la libreria NEATCOM y la
// base de datos local.
//
// Tarea        Fecha           Autor   Observaciones
// (BasedUpon)  2003.07.17	    Clovis  Clovis Von Zuben, Tecnomedia Systems, Brazil
// (Inicial)    2004.11.25      Martin  Neat Argentina - Prepago Argentina.
// (Alfa)		2005.01.09		Martin  TrxResATM_POS_B24_R6::FormatField38_RETRVL_REF_NUM( char *szREFNUM );
// (Alfa)		2005.01.17		Martin  Eliminar campos de rta. en reverso :  35,38,48,60
// (Beta)       2005.08.23      mdc     chCREDIT_AMMOUNT en cuentas con acuerdo / adelanto.
// (Beta)       2005.08.29      mdc     POS: agrega campo 48.
// (Beta)       2005.09.21      mdc     POS: agrega campo 63 para cotizaciones.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Headers ANSI C/C++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _SYSTEM_ATM_
#define _SYSTEM_ATM_DEFINED_
#define _SYSTEM_ATM_
#endif // _SYSTEM_ATM_DEFINED_

// Header Transaction Resolution
#include <qusrinc/trxres.h>
// Libreria de utilidades estandards
#include <qusrinc/stdiol.h>
// Header ISO Msg Redefinition
#include <qusrinc/isoredef.h>
// Header ISO 8583
#include <qusrinc/iso8583.h>
// Header Own STD IO Lib 
#include <qusrinc/stdiol.h>

#ifdef _SYSTEM_ATM_DEFINED_
#undef _SYSTEM_ATM_
#endif // _SYSTEM_ATM_DEFINED_


///////////////////////////////////////////////////////////////////////////
// Macro de invocacion de copia de valores campo a campo
#define COPYVALUE(destiny,source)            \
if(source && destiny)                        \
    {                                        \
    memset(destiny,' ',sizeof(destiny));    \
    memcpy(destiny,source,min(strlen(source),sizeof(destiny)));    \
    }
///////////////////////////////////////////////////////////////////////////
// Macro de actualizacion de valor en cero
#define SETVALUEZEROE(destiny)                \
if(destiny)                                    \
    {                                        \
    memset(destiny,'0',sizeof(destiny));    \
    }                                            
// Macro de asignacion de String, con NULL al final
#define COPYSTRING(dst,src) strcpy(dst,src) 
// Macro de asignacion de String, con NULL al final
#define ASSIGNSTRING(dst,src) \
    { strncpy(dst,src,sizeof(src)); dst[sizeof(src)]=0x00; }; 
// Macro de asignacion de bytes
#define ASSIGNFIELD(dst,src) \
    { memset(dst,' ',sizeof(dst)); memcpy(dst,src,min(sizeof(src),sizeof(dst)));  }; 


///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
TrxResATM_POS_B24_R6::TrxResATM_POS_B24_R6()
{
}

///////////////////////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////////////////////
TrxResATM_POS_B24_R6::~TrxResATM_POS_B24_R6()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #63 de rta., PRIMARY_RSRVD4_ISO para ATM/POS BANELCO
void TrxResATM_POS_B24_R6::FormatField63_PRIMARY_RSRVD4_ISO(char *chWITH_ADV_AVAIL , 
														   char *chCASH_AVAIL     ,
														   char *chCREDIT_AMMOUNT )
{
	TrxResATM_POS_B24_R6::FormatField_TOKENS( chWITH_ADV_AVAIL , 
											  chCASH_AVAIL     ,
											  chCREDIT_AMMOUNT ,											   
											  63     );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #63 de rta., PRIMARY_RSRVD4_ISO
void TrxResATM_POS_B24_R6::FormatField63_PRIMARY_RSRVD4_ISO(char *szUSER_DATA1,
														    char *szUSER_DATA2)
{   
	// Copia local del campo #63
	FIELD63	field63;
	// Armado local de los tokens del campo #63, con valores por omision
	struct stToken_04_tag stToken_04 = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ',' ','0',
										'B','S','A','S',' ','N','0'};
	struct stToken_C0_tag stToken_C0 = {'0','0','0','0',' ',' ',' ',' ','0','0',
										'0','0','0','0','0','0','0','0','0','0',
										' ','0',' ','0','0',' '};
	struct stToken_C4_tag stToken_C4 = {'0','0','0','0','0','0','0','0','0','0','2','0'};
	struct stToken_R2_tag stToken_R2 = {'0','0','0','0','0','0','0','0','0',
										'0','0','0','0','0','0','0','0','0','0','0','0',
										'1','0','0','0','0','0','0','0','0','0','0','0',
										'0','0','0',
										'0','0','0','0','0','0','0','0','0','0'};
	struct stToken_P3_tag stToken_P3    = {'0','0','0',' '};
	char szBuffer[sizeof(stToken_P3)+1] = {0x00};
    WORD	    wRspCdeLen  = 0,
			    wTokenLen   = 0,
				wUserP3Len	= 0;	
	char		szPRODUCT[32] = {0x00};

	/////////////////////////////////////////////////////////////////////////////////
    // Formatear campo ISO #63 de rta. con tokens de transaccion segun MANUAL POSNET
	// Ejemplo : Cada token se separa con "&" - primario, y "!" los demas :
	// "& 0000500156! 0400020  00000000032 AL   Y ! C000026 595     1106      00   00 "...
	// "! C400012 000000000022! R200046 01    0                          X            "
	/////////////////////////////////////////////////////////////////////////////////

	// PIN en posicion especifica del TOKEN "P3" hasta 40 bytes por TICKET-POS
	// o hasta 200 bytes en multiples renglones. Caracteres "|" concatenan y "^" finaliza.
	wRspCdeLen = strlen(szUSER_DATA1);
	// Etiqueta del producto que se ha comprado
	GetField63_PRODUCT_LABEL( szPRODUCT );
	sprintf( szBuffer,",CARD [%s]|PIN [%-*.*s] SERIAL [%s]^", 
					  szPRODUCT, wRspCdeLen, wRspCdeLen, szUSER_DATA1, szUSER_DATA2 );	
	// Unicamente usar una longitud interna-implicita de los bytes imprimibles
	wUserP3Len = ( strchr( szBuffer, '^' ) - szBuffer ) + 1;	
	sprintf( stToken_P3.LEN, "%03i%-*.*s" ,
							wUserP3Len , 
							sizeof(stToken_P3.DATOS), 
							sizeof(stToken_P3.DATOS), 
							szBuffer );	
	stToken_P3.NUL = 0x00;	

	//////////////////////////////////////////////////////////////
	wTokenLen	=	 1 + 1 + 5 + 5 + 
					 1 + 1 + 2 + 5 + 1 + sizeof(stToken_04_tag) +
					 1 + 1 + 2 + 5 + 1 + sizeof(stToken_C0_tag) +
					 1 + 1 + 2 + 5 + 1 + sizeof(stToken_C4_tag) +
					 1 + 1 + 2 + 5 + 1 + sizeof(stToken_R2_tag) +
					 1 + 1 + 2 + 5 + 1 + sizeof(stToken_P3_tag)-1  ;
	//////////////////////////////////////////////////////////////

	// Recuperar el contenido de los tokens originales para formatearlos
	GetField63_TOKEN( "! 04", &stToken_04, sizeof stToken_04 );
	GetField63_TOKEN( "! C0", &stToken_C0, sizeof stToken_C0 );
	GetField63_TOKEN( "! C4", &stToken_C4, sizeof stToken_C4 );
	GetField63_TOKEN( "! R2", &stToken_R2, sizeof stToken_R2 );

	// Copia local del campo antes de actualizarlo con el token P3
	field63    = isoFields.field63;
	// Longitud variable del TOKEN P3
	wRspCdeLen = strlen( (char *)&stToken_P3 );	
	sprintf( field63.chData,
			 "& %05i%05i"		 // token 1 (primario), X longitud
			 "! 04%05i %-*.*s"	 // token 2-04, 20  bytes de longitud
			 "! C0%05i %-*.*s"	 // token 3-C0, 26  bytes de longitud
			 "! C4%05i %-*.*s"	 // token 4-C4, 14  bytes de longitud
			 "! R2%05i %-*.*s"	 // token 5-R2, 46  bytes de lognitud
			 "! P3%05i %-*.*s" , // token 6-P3, 202 bytes de longitud
			 6                 , // 6 tokens en total, primario + 5
			 wTokenLen         , // largo de este campo, incluyendo el token primario
			 sizeof(stToken_04)  ,sizeof(stToken_04),sizeof(stToken_04),
				(char *)&stToken_04,
			 sizeof(stToken_C0)  ,sizeof(stToken_C0),sizeof(stToken_C0),
				(char *)&stToken_C0,
			 sizeof(stToken_C4)  ,sizeof(stToken_C4),sizeof(stToken_C4),
				(char *)&stToken_C4,
			 sizeof(stToken_R2)  ,sizeof(stToken_R2),sizeof(stToken_R2),
				(char *)&stToken_R2,
			 sizeof(stToken_P3)-1, wRspCdeLen, wRspCdeLen, 
				(char *)&stToken_P3 );
	
	wRspCdeLen = strlen( field63.chData );
	// Restauracion 
	isoFields.field63 = field63;
	// Verificar longitudes
	wRspCdeLen=min(sizeof(isoFields.field63),strlen(isoFields.field63.chData));

    // Respuesta parametrica en campo #63
    if( isoMsg.IsValidField( 63 ) )
		isoMsg.DeleteField( 63 );

	isoMsg.InsertField(63, &wRspCdeLen, (PBYTE)&isoFields.field63, wRspCdeLen );

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #48 de rta., ADD_DATA_PRIVATE
void TrxResATM_POS_B24_R6::FormatField48_ADD_DATA_PRIVATE( char *szUSER_DATA , BOOL	bUsePOS)
{   
	// Copia local del campo #48
	FIELD48	field48;
    WORD    wRspCdeLen = 2;

    // Precondicion : // Ya existe el campo 48, entonces dejarlo como está
    if( isoMsg.IsValidField( 48 ) )
		return;
	
	// Segun configuracion local de cada pais, en este caso para Argentina (032)
	memset( &field48, ' ', sizeof field48);
	if( !bUsePOS )
	{
		strncpy(field48.ATM.chSHRG_GRP    , "A"  , 1);
		field48.ATM.chTERM_TRAN_ALLOWED   = 'Y';	   // TERMINAL ALLOWED
		strncpy(field48.ATM.chTERM_ST     , "BU" , 2);  // BUENOS AIRES
		strncpy(field48.ATM.chTERM_CNTY   , "BUE", 3); // BUENOS AIRES
		strncpy(field48.ATM.chTERM_CNTRY  ,"032" , 3);  // ARGENTINA
		strncpy(field48.ATM.chTERM_RTE_GRP, "A"  , 1);   // PRIMARY GROUP 'A'
		field48.ATM.chRESERVED[sizeof(field48.ATM.chRESERVED)-1] = 0x00;
	}
	else
	{
		/******************************** ....:....1....:....2 *********/
		strncpy(field48.POS.chRETL_ID  , "HOST24 RETAILER ID " , 19);
		strncpy(field48.POS.chRETL_GRP , "A   "                , 4);
		strncpy(field48.POS.chRETL_REGN, "A   "                , 4);
		field48.POS.chFILLER = 0x00;
	};

	// Restauracion 
	isoFields.field48 = field48;
	// Verificar longitudes
	wRspCdeLen=min(sizeof(isoFields.field48),strlen(isoFields.field48.ATM.chSHRG_GRP));

    // Respuesta parametrica en campo #48
    if( isoMsg.IsValidField( 48 ) )
		// Ya existe el campo 48, entonces dejarlo como está
		return;
	if( !bUsePOS )
		// Si es ATM, no agregarlo 
		return;
	else
		// Solo si es POS, agregarlo 
		isoMsg.InsertField(48, &wRspCdeLen, (PBYTE)&isoFields.field48, wRspCdeLen );

}
///////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #38 de rta., RETRVL_REF_NUM
void TrxResATM_POS_B24_R6::FormatField38_RETRVL_REF_NUM( char *szREFNUM )
{   
	// Copia local del campo #38
	FIELD38	field38 = {' ',' ',' ',' ',' ',' '};
    WORD    wRspCdeLen = sizeof(isoFields.field38);
	WORD    wREFNUMLen = 0;

	
	// Armado local 	
	if( !szREFNUM)
		return;
	// Ultimos 6 digitos del SERIAL para identificar al ID AUTORIZACION
	wREFNUMLen = strlen(szREFNUM);
	if( wREFNUMLen > sizeof(szREFNUM) )
	    strncpy(field38.chNum, szREFNUM+(wREFNUMLen-sizeof(field38)), sizeof(field38.chNum));
	else
		strncpy(field38.chNum, szREFNUM, sizeof field38.chNum );

	// Restauracion 
	isoFields.field38 = field38;
	// Verificar longitudes
	wRspCdeLen=sizeof(isoFields.field38);

    // Respuesta parametrica en campo #38
    if( isoMsg.IsValidField( 38 ) )
		isoMsg.DeleteField( 38 );

	isoMsg.InsertField(38, &wRspCdeLen, (PBYTE)&isoFields.field38, wRspCdeLen );

}
///////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #90 de rta., ORIG INFO
void TrxResATM_POS_B24_R6::FormatField90_ORIG_INFO( void )
{    
    WORD wRspCdeLen=0;    

    // Actualizar o insertar segun requiera POSNET
    if(isoMsg.IsValidField( 90 ))
    {
        FIELD90 resp90;
        // Copia local de trabajo
        resp90 = isoFields.field90;
        // Copiar las cotizaciones del requerimiento hacia las cotizaciones de respuesta
        memset( resp90.stB24Orig.chReserved , ' ', sizeof resp90.stB24Orig.chReserved );
        // Sobreescitura del campo existente
        isoFields.field90 = resp90;
        // Actualizacion en mensaje ISO
        wRspCdeLen=sizeof(isoFields.field90.chData); // Longitud fija
        isoMsg.SetField(90, &wRspCdeLen, (PBYTE)&isoFields.field90, wRspCdeLen );
    }
}
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
// Recuperar un TOKEN del campo #63 original del requerimiento
short TrxResATM_POS_B24_R6::GetField63_TOKEN( char *szTokenID, void *pstTokenTag, size_t nTokenSize )
{    
	char *pStr = NULL; /* Aux String Pointer */

    // PRECONDITION : Actualizar o insertar segun requiera la red financiera ( bajo ACI BASE24)
    if(!isoMsg.IsValidField( 63 ))
		return 0;
    // PRECONDITION
    if( !szTokenID || !pstTokenTag || 0 == nTokenSize )
        return 0;

	//////////////////////////////////////////////////////////////
	// Verificar si acaso ya viene un TOKEN "XX" y copiar contenido
	if( pStr = strstr(isoFields.field63.chData, szTokenID) ) 
	{
		/* EJEMPLO : "! 0400020  00000000032 AL   Y " */
		/* Sumar 4 de HEADER TOKEN */
		pStr += 4;
		/* Sumar 5 de longitud (LENGTH) del HEADER TOKEN */
		pStr += 5;
		/* Sumar 1 de espacio en blanco */
		pStr++;
		/* Copiar Token en si mismo */
		strncpy( (char *)pstTokenTag, pStr, nTokenSize );
        return nTokenSize;
	}
    else
        return 0;
	//////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////
// Recuperar un TOKEN del campo #126 original del requerimiento
short TrxResATM_POS_B24_R6::GetField126_TOKEN( char *szTokenID, void *pstTokenTag, size_t nTokenSize )
{    
	char *pStr = NULL; /* Aux String Pointer */

    // PRECONDITION : Actualizar o insertar segun requiera la red financiera ( bajo ACI BASE24)
    if(!isoMsg.IsValidField( 126 ))
		return 0;
    // PRECONDITION
    if( !szTokenID || !pstTokenTag || 0 == nTokenSize )
        return 0;

	//////////////////////////////////////////////////////////////
	// Verificar si acaso ya viene un TOKEN "XX" y copiar contenido
	if( pStr = strstr(isoFields.field126.chData, szTokenID) ) 
	{
		/* EJEMPLO : "! QB00140  00000000032..." */
		/* Sumar 4 de HEADER TOKEN */
		pStr += 4;
		/* Sumar 5 de longitud (LENGTH) del HEADER TOKEN */
		pStr += 5;
		/* Sumar 1 de espacio en blanco */
		pStr++;
		/* Copiar Token en si mismo */
		strncpy( (char *)pstTokenTag, pStr, nTokenSize );
        return nTokenSize;
	}
    else
        return 0;
	//////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
// Recuperar la etiqueta de un producto en base a un PLAN elegido en el equipo POS
char *TrxResATM_POS_B24_R6::GetField63_PRODUCT_LABEL( char *pszPRODUCT )
{   	
	struct stToken_R2_tag stToken_R2 = {'0','0','0','0','0','0','0','0','0',
										'0','0','0','0','0','0','0','0','0','0','0','0',
										'1','0','0','0','0','0','0','0','0','0','0','0',
										'0','0','0',
										'0','0','0','0','0','0','0','0','0','0'};
    const char cszProduct_Mask[] = "PRODUCT %i";
	int   iAmmount = antoi(isoFields.field4.chAmmount,12) / 100 ; 		
    short nLen     = 0;

	/* Precondicion */
	if(!pszPRODUCT)
		return NULL;
	/* Recuperar contenido del TOKEN R2 que informa el producto elegido */
    nLen = GetField63_TOKEN( "! R2", &stToken_R2, sizeof stToken_R2 );
    if( nLen )
    {
	    sprintf(pszPRODUCT,cszProduct_Mask  ,iAmmount);
        return pszPRODUCT;
    }
    else
        return NULL;
}

// Formatear campo ISO #126 de rta., con tokens 
void TrxResATM_POS_B24_R6::FormatField126_SECNDRY_RSRVD7_PRVT( char *chWITH_ADV_AVAIL, 
															   char *chCASH_AVAIL,
															   char *chCREDIT_AMMOUNT )
{
	TrxResATM_POS_B24_R6::FormatField_TOKENS( chWITH_ADV_AVAIL , 
											  chCASH_AVAIL     ,
											  chCREDIT_AMMOUNT ,											   
											  126     );
}

// Formatear campo ISO #54 de rta., stB24_0210_Def_Data
void TrxResATM_POS_B24_R6::FormatField54_B24_0210_Def_Data( 
    char *chWITH_ADV_AVAIL,char *chINT_OWE_AUSTRAL ,
    char *chCASH_AVAIL    ,char *chMIN_PAYMENT,
    char *chPAYMENT_DAT   ,char *chINTEREST_RATE,
    char *chOWE_DOLAR     ,char *chMIN_PAYMENT_DOLAR,
    char *chPURCHASE_DOLAR,char *chCASH_FEE )
{
    // No hacer nada, es otra configuracion EMF en este Release. DO NOTHING.
}

// Formatear campo ISO #55, PRIRSRVD1_ISO
void TrxResATM_POS_B24_R6::FormatField55_PRIRSRVD1_ISO( 
    char *chTrack2,char *chCA,
    char *chFiid_1,char *chTyp_1,
    char *chAcct_num_1,char *chFiid_2,
    char *chTyp_2,char *chAcct_num_2    )
{
    // Invocar a las funciones de la clase Base24R4 para estos campos
    TrxResATM_Base24_R4::FormatField55_PRIRSRVD1_ISO(
        chTrack2, chCA, chFiid_1, chTyp_1, chAcct_num_1, 
                        chFiid_2, chTyp_2, chAcct_num_2 );

}
// Formatear campo ISO #44, RESP_DATA
void TrxResATM_POS_B24_R6::FormatField44_RESP_DATA( char *chWITH_ADV_AVAIL, 
												    char *chCASH_AVAIL ,
													char *chCREDIT_AMMOUNT )
{
    // Invocar a las funciones de la clase Base24R4 para estos campos
    TrxResATM_Base24_R4::FormatField44_RESP_DATA( chWITH_ADV_AVAIL, 
												  NULL ,
												  NULL );
}

// Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
void TrxResATM_POS_B24_R6::FormatField123_SECNDRY_RSRVD4_PRVT( void )
{
    // No hacer nada, es otra configuracion EMF en este Release. DO NOTHING.
}

// Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
void TrxResATM_POS_B24_R6::FormatField122_SECNDRY_RSRVD3_PRVT( void )
{
    // No hacer nada, es otra configuracion EMF en este Release. DO NOTHING.
}

// Formatear campo ISO #127, SECNDRY_RSRVD8_PRVT
void TrxResATM_POS_B24_R6::FormatField127_SECNDRY_RSRVD8_PRVT( void )
{
    // Como es otra configuracion EMF en este Release,verificar que no vaya este campo.
	// Y si esta presente, eliminarlo.
	isoMsg.DeleteField ( 127 );
}

// Formatear campo ISO #125, SECNDRY_RSRVD6_PRVT
void TrxResATM_POS_B24_R6::FormatField125_SECNDRY_RSRVD6_PRVT( char chProc_Acct_Ind )
{
    // No hacer nada, es otra configuracion EMF en este Release. DO NOTHING.
}

// Formatear campo ISO #15 y #61 que A POR ERRORES DEL PREHOMO DE LINK, a veces no vienen en el 0200
void TrxResATM_POS_B24_R6::FormatFields15_61(void)
{
	// No hacer nada, es otra configuracion EMF en este Release. DO NOTHING.
}

// Formatear campo ISO #60 
void TrxResATM_POS_B24_R6::FormatField60( void )
{                  
    // Invocar a las funciones de la clase Base24R4 para estos campos
    TrxResATM_Base24_R4::FormatField60();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #124 de rta., SECNDRY_RSRVD5_PRVT
void TrxResATM_POS_B24_R6::FormatField124_SECNDRY_RSRVD5_PRVT( void )
{
    // Invocar a las funciones de la clase Base24R4 para estos campos
    TrxResATM_Base24_R4::FormatField124_SECNDRY_RSRVD5_PRVT();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
SHORT TrxResATM_POS_B24_R6::Format_ISO_Ammount( const char *szAmmount, 
   short shDecPos, char *szNew, short shAlignWidth, 
   boolean_t bApplyChangeRate, char *szExchangeRate )
{
	// Funcion de formateo base
	return TrxBaseResolution::Format_ISO_Ammount(szAmmount, shDecPos, 
                                      szNew, shAlignWidth, bApplyChangeRate, 
									  szExchangeRate);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inicio de Procesamiento de transacciones en general
WORD TrxResATM_POS_B24_R6::ProcessInit(void)
{
	/*****************************************************************/
	struct stToken_P6_tag stToken_P6 ;
	WORD   wTknP6Len        = 0,
		   wFieldLength     = 0;
	CHAR   szRateAmmnt[8+1] = {"00000000"};
	WORD   wResult          = 0;
	/*****************************************************************/
	struct stToken_PD_tag stToken_PD ;
	WORD   wTknPDLen        = 0;
	/*****************************************************************/

	// CALL TO BASE CLASS METHOD
	wResult = TrxBaseResolution::ProcessInit();

	/*****************************************************************************/
	/* TRAN.AMMOUNT AND CONVERSION RATE                                          */
	/*****************************************************************************/
	if( ( isoMsg.IsValidField( 126 ) || isoMsg.IsValidField( 63 ) )
		&& 
		!isoMsg.IsValidField( 127 ) )
	{
		// Recuperar el contenido de los tokens originales para formatearlos
		// (especificamente P6 para Base24-ATM, o PD para compras POS )
		wTknP6Len = GetField126_TOKEN( "! P6", &stToken_P6, sizeof(stToken_P6) );
		wTknPDLen = GetField63_TOKEN( "! PD", &stToken_PD, sizeof(stToken_PD) );
		
		// Puede venir en cualquiera de los 2, sea P6 o PD :
		if(wTknP6Len)
		{
			/* ACI Base24 R4 : mover a campo 127 */
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chRegnId, 
				stToken_P6.ZONA);
			COPYSTRING(szRateAmmnt, strsubstr(stToken_P6.COTIZACION_COMPRA,2,8));
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C,
				 szRateAmmnt);
			COPYSTRING(szRateAmmnt, strsubstr(stToken_P6.COTIZACION_VENTA,2,8));
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V,
				szRateAmmnt );
			COPYSTRING(szRateAmmnt, strsubstr(stToken_P6.IMPORTE_CONVERTIDO,4,8));
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1,
				szRateAmmnt );
			COPYSTRING(szRateAmmnt, strsubstr(stToken_P6.IMPORTE_CONVERTIDO_DISPENSADO,4,8));
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2,
				szRateAmmnt);
			
			// Y verificar si lo informado en el campo #4 AMMT1 es igual a lo que TKN-P6			
			if(strncmp( isoFields.field4.chAmmount,	stToken_P6.IMPORTE_CONVERTIDO,
						sizeof(isoFields.field4)) != 0 )
			{
				////////////////////////////////////////////////////////
				// Asi estaba antes, comentado por Maximiliano Broinsky
				// 2005-09-28
				////////////////////////////////////////////////////////
				/*// El importe convertido se expresa en la moneda inversa (SI NO ES MONEDA LOCAL)
				// Excepto para TIPOS DE CUENTA en mismo TIPO DE MONEDA

				// Comentario Maximiliano Broinsky (2005-09-28)
				// Tipo operación: Transferencia cruzada
				// Moneda Cuenta origen: Pesos
				// Moneda Cuenta destino: Dolares
				// Moneda de operación: Dolares
				if(antoi(isoFields.field49.chData,
						sizeof(isoFields.field49.chData)) 
						!= _B24_CURRENCY_ARGENTINA_
					&&
					strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
						   isoFields.field3.uProcCde.stTranCde.chTo_Acct_Typ  ,
						   sizeof(isoFields.field3.uProcCde.stTranCde.chTo_Acct_Typ) )
			
					///////////////////////////////////////////////////////////////////////////////////
					// Condición agregada para no pisar las transferencias 
					// de dolares a cuentas de pesos.
					//
					// 2005-09-23 Maximiliano Broinsky
					//
					&&
					strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
						   B24_SAVINGS_ACCOUNT_USD,
						   sizeof(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ) )  
					&&
					strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
						   B24_CHECKINGS_ACCOUNT_USD,
						   sizeof(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ) )
					//////////////////////////////////////////////////////////////////////////////////
				    )
				{
					///////////////////////////////////////////////////////////////////////////
					// COPY - PASTE del caso del else if de abajo, pero con el 
					// cambio que en este caso va el valor convertido, debido a que ES 
					// el valor destino.
					ASSIGNFIELD(isoFields.field123.chDepositCreditAmmount,stToken_P6.IMPORTE_CONVERTIDO);
					// Actualizar el campo 123, CREDIT AMMT
					wFieldLength = sizeof(isoFields.field123);
					isoMsg.InsertField( 123, &wFieldLength, (PBYTE)&isoFields.field123, wFieldLength);
					///////////////////////////////////////////////////////////////////////////

					// AMMT1 field 4 = TRAN.AMMT IN TOKEN P6 (MULTIPLIED BY CONVERSION RATE)
					ASSIGNFIELD(isoFields.field4.chAmmount,stToken_P6.IMPORTE_CONVERTIDO);
					// Actualizar el campo 4, necesario para cotizaciones US$ y otras
					wFieldLength = sizeof(isoFields.field4);
					isoMsg.SetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength);
					// Actualizar buffer
					isoMsg.GetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength );
				}

				///////////////////////////////////////////////////////////////////////////////////
				// Condición agregada para pisar las transferencias 
				// en moneda pesos de cuentas en dolares a cuentas de pesos.
				//
				// 2005-09-27 Maximiliano Broinsky
				//
				// Comentario Maximiliano Broinsky (2005-09-28)
				// Tipo operación: Transferencia cruzada
				// Moneda Cuenta origen: Dolares
				// Moneda Cuenta destino: Pesos
				// Moneda de operación: Pesos

				else if(antoi(isoFields.field49.chData,
						sizeof(isoFields.field49.chData)) 
						!= _B24_CURRENCY_USA_ //Moneda distinta de dolar
					&&
					//Tipos de cuentas distintas
					strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
						   isoFields.field3.uProcCde.stTranCde.chTo_Acct_Typ  ,
						   sizeof(isoFields.field3.uProcCde.stTranCde.chTo_Acct_Typ))
					//Cuenta en dolares
					&&
					!(strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
						   B24_SAVINGS_ACCOUNT_USD,
						   sizeof(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ) )  
					&&
					strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
						   B24_CHECKINGS_ACCOUNT_USD,
						   sizeof(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ)))
					//////////////////////////////////////////////////////////////////////////////////
				    )
				{
					
					///////////////////////////////////////////////////////////////////////////
					// Backup antes de pisar el monto 1 : copiarlo en el monto 123, CREDIT AMMT
					ASSIGNFIELD(isoFields.field123.chDepositCreditAmmount,isoFields.field4.chAmmount);
					// Actualizar el campo 123, CREDIT AMMT
					wFieldLength = sizeof(isoFields.field123);
					isoMsg.InsertField( 123, &wFieldLength, (PBYTE)&isoFields.field123, wFieldLength);
					///////////////////////////////////////////////////////////////////////////
					
					// AMMT1 field 4 = TRAN.AMMT IN TOKEN P6 (MULTIPLIED BY CONVERSION RATE)
					ASSIGNFIELD(isoFields.field4.chAmmount,stToken_P6.IMPORTE_CONVERTIDO);
					// Actualizar el campo 4, necesario para cotizaciones US$ y otras
					wFieldLength = sizeof(isoFields.field4);
					isoMsg.SetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength);
					// Actualizar buffer
					isoMsg.GetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength );
					///////////////////////////////////////////////////////////////////////////
				};*/
				// Hasta aca como era antes del 2005-09-28
				/////////////////////////////////////////////////////////////////////////////////////

				/////////////////////////////////////////////////////////////////////////////////////
				// Cambios para agrupar los if y en todos los casos de
				// transferencias cruzadas, laburar los valores que se
				// mandan al SP (Maximiliano Broinsky 2005-09-28)				
				
				// Lo primero que pregunto es si las cuentas son de
				// distintas monedas.

				if (strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
						   isoFields.field3.uProcCde.stTranCde.chTo_Acct_Typ  ,
						   sizeof(isoFields.field3.uProcCde.stTranCde.chTo_Acct_Typ)))
				{

					// Tipo de moneda distinto de pesos?
					if(antoi(isoFields.field49.chData, sizeof(isoFields.field49.chData)) 
						!= _B24_CURRENCY_ARGENTINA_)
					{
						// Moneda Cuenta origen: Pesos
						// Moneda Cuenta destino: Dolares
						// Moneda de operación: Dolares
						if (strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
								B24_SAVINGS_ACCOUNT_USD,
								sizeof(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ) )  
							&&
							strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
								B24_CHECKINGS_ACCOUNT_USD,
								sizeof(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ) )
							)
						{
							///////////////////////////////////////////////////////////////////////////
							// Backup antes de pisar el monto 1 : copiarlo en el monto 123, CREDIT AMMT
							ASSIGNFIELD(isoFields.field123.chDepositCreditAmmount,isoFields.field4.chAmmount);
							// Actualizar el campo 123, CREDIT AMMT
							wFieldLength = sizeof(isoFields.field123);
							isoMsg.InsertField( 123, &wFieldLength, (PBYTE)&isoFields.field123, wFieldLength);
							///////////////////////////////////////////////////////////////////////////

							// AMMT1 field 4 = TRAN.AMMT IN TOKEN P6 (MULTIPLIED BY CONVERSION RATE)
							ASSIGNFIELD(isoFields.field4.chAmmount,stToken_P6.IMPORTE_CONVERTIDO);
							// Actualizar el campo 4, necesario para cotizaciones US$ y otras
							wFieldLength = sizeof(isoFields.field4);
							isoMsg.SetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength);
							// Actualizar buffer
							isoMsg.GetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength );
						}
						// Moneda Cuenta origen: Dolares
						// Moneda Cuenta destino: Pesos
						// Moneda de operación: Dolares
						else
						{
							///////////////////////////////////////////////////////////////////////////
							// Backup antes de pisar el monto 1 : copiarlo en el monto 123, CREDIT AMMT
							ASSIGNFIELD(isoFields.field123.chDepositCreditAmmount,stToken_P6.IMPORTE_CONVERTIDO);
							// Actualizar el campo 123, CREDIT AMMT
							wFieldLength = sizeof(isoFields.field123);
							isoMsg.InsertField( 123, &wFieldLength, (PBYTE)&isoFields.field123, wFieldLength);
							///////////////////////////////////////////////////////////////////////////
						};
					}
					//Tipo de moneda distinto de dolares?
					else if(antoi(isoFields.field49.chData,
							sizeof(isoFields.field49.chData)) 
							!= _B24_CURRENCY_USA_)
					{
						// Moneda Cuenta origen: Dolares
						// Moneda Cuenta destino: Pesos
						// Moneda de operación: Pesos
						if (!(strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
								B24_SAVINGS_ACCOUNT_USD,
								sizeof(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ) )  
							&&
							strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
								B24_CHECKINGS_ACCOUNT_USD,
								sizeof(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ))
							))
						{
					
							///////////////////////////////////////////////////////////////////////////
							// Backup antes de pisar el monto 1 : copiarlo en el monto 123, CREDIT AMMT
							ASSIGNFIELD(isoFields.field123.chDepositCreditAmmount,isoFields.field4.chAmmount);
							// Actualizar el campo 123, CREDIT AMMT
							wFieldLength = sizeof(isoFields.field123);
							isoMsg.InsertField( 123, &wFieldLength, (PBYTE)&isoFields.field123, wFieldLength);
							///////////////////////////////////////////////////////////////////////////
						
							// AMMT1 field 4 = TRAN.AMMT IN TOKEN P6 (MULTIPLIED BY CONVERSION RATE)
							ASSIGNFIELD(isoFields.field4.chAmmount,stToken_P6.IMPORTE_CONVERTIDO);
							// Actualizar el campo 4, necesario para cotizaciones US$ y otras
							wFieldLength = sizeof(isoFields.field4);
							isoMsg.SetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength);
							// Actualizar buffer
							isoMsg.GetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength );
							///////////////////////////////////////////////////////////////////////////
						}
						// Moneda Cuenta origen: Pesos
						// Moneda Cuenta destino: Dolares
						// Moneda de operación: Pesos
						else
						{
							///////////////////////////////////////////////////////////////////////////
							// Backup antes de pisar el monto 1 : copiarlo en el monto 123, CREDIT AMMT
							ASSIGNFIELD(isoFields.field123.chDepositCreditAmmount,stToken_P6.IMPORTE_CONVERTIDO);
							// Actualizar el campo 123, CREDIT AMMT
							wFieldLength = sizeof(isoFields.field123);
							isoMsg.InsertField( 123, &wFieldLength, (PBYTE)&isoFields.field123, wFieldLength);
							///////////////////////////////////////////////////////////////////////////
						};

					};
				};
				// FIN CAMBIOS MAXI 2005-09-29
				/////////////////////////////////////////////////////////////////////////
			};			
		}
		/*************************************************************************/
		/* Si es compras, la cotizacion viene en el TOKEN PD campo 63            */
		else if(wTknPDLen)
		{
			/* ACI Base24 R4 : mover a campo 127 */
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chRegnId, "0000");
			COPYSTRING(szRateAmmnt, strsubstr(stToken_PD.COTIZACION_COMPRA,2,8));
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C,
				 szRateAmmnt);
			COPYSTRING(szRateAmmnt, strsubstr(stToken_PD.COTIZACION_VENTA,2,8));
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V,
				szRateAmmnt );
			COPYSTRING(szRateAmmnt, strsubstr(stToken_PD.IMPORTE_CONVERTIDO,4,8));
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1,
				szRateAmmnt );
			COPYSTRING(szRateAmmnt, strsubstr(stToken_PD.IMPORTE_CONVERTIDO_AJUSTADO,4,8));
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2,
				szRateAmmnt);
			// Y verificar si lo informado en el campo #4 AMMT1 es igual a lo que TKN-PD
			if(strncmp( isoFields.field4.chAmmount,	stToken_PD.IMPORTE_CONVERTIDO,
						sizeof(isoFields.field4)) != 0 )
			{
				/* COMPRAS EN MONEDA LOCAL DE CUENTA EN OTRA MONEDA */
				if(antoi(isoFields.field49.chData,sizeof(isoFields.field49.chData)) 
						== _B24_CURRENCY_ARGENTINA_
					&&
					( strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
						   B24_SAVINGS_ACCOUNT_USD,
						   sizeof(isoFields.field3.uProcCde.stTranCde.chTo_Acct_Typ) )  
						||
					  strncmp(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,
						   B24_CHECKINGS_ACCOUNT_USD,
						   sizeof(isoFields.field3.uProcCde.stTranCde.chTo_Acct_Typ) )
					)
				  )
				{
					// AMMT1 field 4 = TRAN.AMMT IN TOKEN PD (MULTIPLIED BY CONVERSION RATE)
					ASSIGNFIELD(isoFields.field4.chAmmount,stToken_PD.IMPORTE_CONVERTIDO);
					// Actualizar el campo 4, necesario para cotizaciones US$ y otras
					wFieldLength = sizeof(isoFields.field4);
					isoMsg.SetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength);
					// Actualizar buffer
					isoMsg.GetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength );
				}
				/* COMPRAS DEL EXTERIOR EN OTRA MONEDA */
				else if( antoi(isoFields.field49.chData,sizeof(isoFields.field49.chData)) 
						 != _B24_CURRENCY_ARGENTINA_ )  
				{
					// AMMT1 field 4 = TRAN.AMMT IN TOKEN PD (MULTIPLIED BY CONVERSION RATE)
					ASSIGNFIELD(isoFields.field4.chAmmount,stToken_PD.IMPORTE_CONVERTIDO);
					// Actualizar el campo 4, necesario para cotizaciones US$ y otras
					wFieldLength = sizeof(isoFields.field4);
					isoMsg.SetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength);
					// Actualizar buffer
					isoMsg.GetField( 4, &wFieldLength, (PBYTE)&isoFields.field4, wFieldLength );
				};
			};
		}		
		/*************************************************************************/
		else
		{
			/* ACI Base24 R4 */
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chRegnId , "0000");
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C,"000000000000");
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V,"000000000000");
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1,"000000000000");
			ASSIGNFIELD(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2,"000000000000");
		};
		// Agregar el campo 127, necesario para cotizaciones US$ y otras
		wFieldLength = sizeof(isoFields.field127);
		isoMsg.InsertField( 127, &wFieldLength, (PBYTE)&isoFields.field127, wFieldLength);
		// Actualizar buffer
        isoMsg.GetField( 127, &wFieldLength, (PBYTE)&isoFields.field127, wFieldLength );
	};
	/*****************************************************************************/
	/* PARTIAL REVERSAL AMMOUNT ?                                                */
	/*****************************************************************************/
	if( isoMsg.IsValidField( 126 ) && isoMsg.IsValidField( 95 ) )
	{
		if(wTknP6Len)
		{			
			// Y verificar si lo informado en el campo #95 REV_AMMT es igual a que TKN-P6
			if(strncmp( isoFields.field95.chReversalAmmount,
						stToken_P6.IMPORTE_CONVERTIDO_DISPENSADO,
						sizeof(isoFields.field95.chReversalAmmount)) != 0 )
			{
				// REV_AMMT field 95 = REV_AMMT IN TOKEN P6
				ASSIGNFIELD(isoFields.field95.chReversalAmmount,
							stToken_P6.IMPORTE_CONVERTIDO_DISPENSADO);
				// Actualizar el campo 95, necesario para cotizaciones US$ y otras
				wFieldLength = sizeof(isoFields.field95);
				isoMsg.SetField( 95, &wFieldLength, (PBYTE)&isoFields.field95, wFieldLength);
				// Actualizar buffer
				isoMsg.GetField( 95, &wFieldLength, (PBYTE)&isoFields.field95, wFieldLength );
			};		
		};
	}
	/*****************************************************************************/

    // RETURN RESULT
    return wResult;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Formatear campo ISO #44, RESP_DATA
void TrxResATM_POS_B24_R6::FormatField5_SETL_AMMT( char *chWITH_ADV_AVAIL )
{
    WORD            wRspCdeLen           = 0;

    // Formatear campo ISO #5 de rta.,
    memset(&isoFields.field5,' ',sizeof(isoFields.field5));

    // Segun la cantidad de montos informados...
    if( chWITH_ADV_AVAIL )
    {
        COPYVALUE(isoFields.field5.chData,chWITH_ADV_AVAIL);  /* 9(10).99 : saldo */
    }
    else 
    {        
        SETVALUEZEROE(isoFields.field5.chData );  /* 9(10).99 : saldo  */
    };


    // Respuesta parametrica en campo #44        
    // Insertar el campo #44 solo si no existe
    if(isoMsg.IsValidField( 5 ))
        isoMsg.DeleteField( 5 );
    wRspCdeLen=min(sizeof(isoFields.field5),strlen(isoFields.field5.chData));

    isoMsg.InsertField(5, &wRspCdeLen, (PBYTE)&isoFields.field5,    wRspCdeLen );

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO XX de rta. con TOKENS
void TrxResATM_POS_B24_R6::FormatField_TOKENS( char *chWITH_ADV_AVAIL , 
											   char *chCASH_AVAIL     ,
											   char *chCREDIT_AMMOUNT ,											   
											   const short nField     )
{
	// Armado local de los tokens del campo #63
	struct stToken_QB_tag stToken_QB ;
    struct stToken_PC_tag stToken_PC ;
    struct stToken_P6_tag stToken_P6 ;
    struct stToken_R6_tag stToken_R6 ;
    struct stToken_PD_tag stToken_PD ;

	/////////////////////////////////////////////
    // Agrego P2 para devolverlo cuando viene
	// 2005-09-16 Maximiliano Broinsky
	struct stToken_P2_tag stToken_P2 ;
	/////////////////////////////////////////////

	char szBuffer[max(sizeof(FIELD126),sizeof(FIELD63))] = {0x00};
    
	WORD	    wRspCdeLen  = 0,
			    wTokenLen   = 0,
				wTknQBLen	= 0,
                wTknPCLen	= 0,
                wTknP6Len	= 0,
                wTknR6Len	= 0,
                wTknPDLen	= 0,
				wTknP2Len	= 0,
                wTknCounter = 1;

    char *szDefaultToken = "QB";
	////////////////////////////////////////////////
	char  fieldTKN[max(sizeof(FIELD126),sizeof(FIELD63))] = {0x00};
	char   *isoFieldsTKN   = (char *)&isoFields.field126 ;       // x omision
	size_t nSize           = sizeof(isoFields.field126); // x omision
	////////////////////////////////////////////////

	// Precondicion
	if( !chWITH_ADV_AVAIL) chWITH_ADV_AVAIL = "0";
	if( !chCASH_AVAIL)     chCASH_AVAIL     = "0";
	if( !chCREDIT_AMMOUNT) chCREDIT_AMMOUNT = "0";

	/////////////////////////////////////////////////////////////////////////////////
	// Ejemplo : Cada token se separa con "&" - primario, y "!" los demas :
	// "& 0000500156! 0400020  00000000032 AL   Y ! C000026 595     1106      00   00 "...
	// "! C400012 000000000022! R200046 01    0                          X            "
	/////////////////////////////////////////////////////////////////////////////////


	// Recuperar el contenido de los tokens originales para formatearlos
	if( 126 == nField )
	{
		wTknQBLen = GetField126_TOKEN( "! QB", &stToken_QB, sizeof(stToken_QB) );	    
		wTknPCLen = GetField126_TOKEN( "! PC", &stToken_PC, sizeof(stToken_PC) );
		wTknP6Len = GetField126_TOKEN( "! P6", &stToken_P6, sizeof(stToken_P6) );
		wTknR6Len = GetField126_TOKEN( "! R6", &stToken_R6, sizeof(stToken_R6) );
		wTknPDLen = GetField126_TOKEN( "! PD", &stToken_PD, sizeof(stToken_PD) );
		//////////////////////////////////////////////////////////////////////////
		wTknP2Len = GetField126_TOKEN( "! P2", &stToken_P2, sizeof(stToken_P2) );
		//////////////////////////////////////////////////////////////////////////
		nSize        = sizeof(isoFields.field126 );
		isoFieldsTKN = (char *)&isoFields.field126 ;
		//////////////////////////////////////////////////////////////////////////
	}
	else if( 63 == nField )
	{
		wTknQBLen = GetField63_TOKEN( "! QB", &stToken_QB, sizeof(stToken_QB) );		
		wTknPCLen = GetField63_TOKEN( "! PC", &stToken_PC, sizeof(stToken_PC) );
		wTknP6Len = GetField63_TOKEN( "! P6", &stToken_P6, sizeof(stToken_P6) );
		wTknR6Len = GetField63_TOKEN( "! R6", &stToken_R6, sizeof(stToken_R6) );
		wTknPDLen = GetField63_TOKEN( "! PD", &stToken_PD, sizeof(stToken_PD) );
		//////////////////////////////////////////////////////////////////////////
		wTknP2Len = GetField63_TOKEN( "! P2", &stToken_P2, sizeof(stToken_P2) );
		//////////////////////////////////////////////////////////////////////////
		nSize        = sizeof(isoFields.field63 );
		isoFieldsTKN = (char *)&isoFields.field63 ;
		//////////////////////////////////////////////////////////////////////////
	}
	else
		return; // ERROR
	
	// Finalizar el QB con un NULL
	stToken_QB.NUL = 0x00;
	if( wTknQBLen > (sizeof(stToken_QB)-1))
		wTknQBLen=sizeof(stToken_QB)-1;



    /////////////////////////////////////////////////////////////////
    // HEADER LENGTH 
    wTknCounter = 1;
    wTokenLen	=	 1 + 1 + 5 + 5 ;
    // QB TOKEN ***ALWAYS***
    if( wTknQBLen || TRUE )
    {
        wTknCounter++;
	    wTokenLen   +=	 1 + 1 + 2 + 5 + 1 + wTknQBLen ;
    }
    // OTHERS TOKENS LENGTHS
    if( wTknPCLen )
    {
        wTknCounter++;
        wTokenLen	+=	 1 + 1 + 2 + 5 + 1 + wTknPCLen ;
    }
    if( wTknP6Len )
    {
        wTknCounter++;
        wTokenLen	+=	 1 + 1 + 2 + 5 + 1 + wTknP6Len ;
    }
    if( wTknR6Len )
    {
        wTknCounter++;
        wTokenLen	+=	 1 + 1 + 2 + 5 + 1 + wTknR6Len ;
    }
    if( wTknPDLen )
    {
        wTknCounter++;
        wTokenLen	+=	 1 + 1 + 2 + 5 + 1 + wTknPDLen ;
    }
	/////////////////////////////////////////////////////
	// 2005-09-16 Maximiliano Broinsky
	if ( wTknP2Len )
	{
		wTknCounter++;
		wTokenLen	+=	 1 + 1 + 2 + 5 + 1 + wTknP2Len; 
	}
	
	/////////////////////////////////////////////////////////////////

	// Copia local del campo antes de actualizarlo con el token 
	memcpy( fieldTKN, isoFieldsTKN, nSize );
    // Buffer temporal a NULL
    szBuffer[0] = 0x00;

    // Formateo del HEADER contenedor de todos los tokens
    sprintf( szBuffer,
			 "& %05i%05i", // token 1 (primario), X longitud			 
		     wTknCounter , // N tokens en total, o sea el primario + (N-1)
		     wTokenLen   );// largo de este campo, incluyendo el token primario
	// Copia hacia el campo respectivo del mensaje de rta.
	strcpy( fieldTKN, szBuffer );
    
    // Verificar que tokens estaba y deben seguir estando (QB lo ponemos siempre)
    if( wTknQBLen || TRUE )
    {
	    // Longitud variable del TOKEN "QB"
	    wRspCdeLen = sizeof(stToken_QB)-1;			
		
		// Formateo del token propiamente dicho (en CEROS para inicializar)
		// 2005-09-19 Maximiliano Broinsky
		// Cambio para setear bien el QB
		//
		if( 0 == wTknQBLen )
		{
			//memset( &stToken_QB, '0', sizeof (stToken_QB));			
			//Lo Formateo Campo por campo por claridad (MRB)
			memset(&stToken_QB.NUMERO_CBU_1, ' ', sizeof(stToken_QB.NUMERO_CBU_1));
			memset(&stToken_QB.NUMERO_CBU_2, ' ', sizeof(stToken_QB.NUMERO_CBU_2));
			memset(&stToken_QB.CUIT_CUIL_CDI, ' ', sizeof(stToken_QB.CUIT_CUIL_CDI));
			memset(&stToken_QB.CONCEPTO, ' ', sizeof(stToken_QB.CONCEPTO));
			memset(&stToken_QB.TIPO_DOCUMENTO, ' ', sizeof(stToken_QB.TIPO_DOCUMENTO));
			memset(&stToken_QB.TIPO_CUENTA,	' ', sizeof(stToken_QB.TIPO_CUENTA));
			memset(&stToken_QB.IMPORTE_ACUERDO_SOBREGIRO, '0', sizeof(stToken_QB.IMPORTE_ACUERDO_SOBREGIRO));
			memset(&stToken_QB.INDICACION_SOBREGIRO, ' ', sizeof(stToken_QB.INDICACION_SOBREGIRO));
			memset(&stToken_QB.INDICACION_INTERESES, ' ', sizeof(stToken_QB.INDICACION_INTERESES));
			memset(&stToken_QB.IMPORTE_INTERESES, '0', sizeof(stToken_QB.IMPORTE_INTERESES));
			memset(&stToken_QB.IMPORTE_COMISION, '0', sizeof(stToken_QB.IMPORTE_COMISION));
			memset(&stToken_QB.INSTITUCION_TRANSFERIDA, ' ', sizeof(stToken_QB.INSTITUCION_TRANSFERIDA));
			memset(&stToken_QB.uREF.REFERENCIA, ' ', sizeof(stToken_QB.uREF.REFERENCIA));
		}
		
		// Detallar intereses si los hubiera
		sprintf(stToken_QB.IMPORTE_ACUERDO_SOBREGIRO, "%012.0f", atof(chCREDIT_AMMOUNT) );
		if( atof(chCREDIT_AMMOUNT) == 0.0f )
			stToken_QB.INDICACION_SOBREGIRO = 'N' ;	
		else
			stToken_QB.INDICACION_SOBREGIRO = 'S' ;	
		
		///////////////////////////////////////////////////////////////////////////////////
		// Aca estan los campos que sobran
		// Los comento para cumplir con lo pedido por Arcuri
		// 2005-09-15 Maximiliano Broinsky

		//stToken_QB.INDICACION_INTERESES = 'N' ;
		//sprintf(stToken_QB.uREF.stCREDIT.DISPONIBLE, "%08.0f", atof(chWITH_ADV_AVAIL) );
		//if( atof(chWITH_ADV_AVAIL) == 0.0f )
		//	stToken_QB.uREF.stCREDIT.INDICACION_DISPONIBLE = 'N';			
		//else
		//	stToken_QB.uREF.stCREDIT.INDICACION_DISPONIBLE = 'S';	
		///////////////////////////////////////////////////////////////////////////////////

		stToken_QB.NUL = 0x00;
		
		// Long. de texto variable
		wRspCdeLen = strlen((char *)&stToken_QB);	
		////////////////////////////////////////////////////////////////////////////
        sprintf( szBuffer, "! QB%05i %-*.*s" , // token QB
			     sizeof(stToken_QB)-1, wRspCdeLen, wRspCdeLen, (char *)&stToken_QB );
        // Copia inicial al campo 63
        strcat( fieldTKN, szBuffer );
    };
    if( wTknPCLen )
    {
	    // Longitud variable del TOKEN "PC"
	    wRspCdeLen = sizeof(stToken_PC);	
        sprintf( szBuffer, "! PC%05i %-*.*s" , // token PC
			     sizeof(stToken_PC), wRspCdeLen, wRspCdeLen, (char *)&stToken_PC );
        // Copia inicial al campo 63
        strcat( fieldTKN, szBuffer );
    };
    if( wTknP6Len )
    {
	    // Longitud variable del TOKEN "P6"
	    wRspCdeLen = sizeof(stToken_P6);	
        sprintf( szBuffer, "! P6%05i %-*.*s" , // token P6
			     sizeof(stToken_P6), wRspCdeLen, wRspCdeLen, (char *)&stToken_P6 );
        // Copia inicial al campo 63
        strcat( fieldTKN, szBuffer );
    };
    if( wTknR6Len )
    {
	    // Longitud variable del TOKEN "R6"
	    wRspCdeLen = sizeof(stToken_R6);	
        sprintf( szBuffer, "! R6%05i %-*.*s" , // token R6
			     sizeof(stToken_R6), wRspCdeLen, wRspCdeLen, (char *)&stToken_R6 );
        // Copia inicial al campo 63
        strcat( fieldTKN, szBuffer );
    };
    if( wTknPDLen )
    {
	    // Longitud variable del TOKEN "PD"
	    wRspCdeLen = sizeof(stToken_PD);	
        sprintf( szBuffer, "! PD%05i %-*.*s" , // token PD
			     sizeof(stToken_PD), wRspCdeLen, wRspCdeLen, (char *)&stToken_PD );
        // Copia inicial al campo 63
        strcat( fieldTKN, szBuffer );
    };

	///////////////////////////////////////////////////////
	// 2005-09-16 Maximiliano Broinsky
	if( wTknP2Len )
    {
	    // Longitud variable del TOKEN "P2"
	    wRspCdeLen = sizeof(stToken_P2);	
        sprintf( szBuffer, "! P2%05i %-*.*s" , // token P2
			     sizeof(stToken_P2), wRspCdeLen, wRspCdeLen, (char *)&stToken_P2 );
        // Copia inicial al campo 63
        strcat( fieldTKN, szBuffer );
    };

    wRspCdeLen = strlen( fieldTKN );
	// Copiar a estructura de campos expandidos
	memcpy(isoFieldsTKN, fieldTKN, nSize );
	// Verificar longitudes
	wRspCdeLen=min(nSize , strlen(isoFieldsTKN));
	

    // Respuesta parametrica en campo #63 o #126
    if( isoMsg.IsValidField( nField ) )
		isoMsg.DeleteField( nField );
    // Insertar campo
	isoMsg.InsertField(nField, &wRspCdeLen, (PBYTE)isoFieldsTKN, wRspCdeLen );

}

////////////////////////////////////////////////////////////////////////////////////////////

