///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// BTF-DATANET Resolution Class 
//
//   Clase de resolucion de transacciones DATANET, con controles internos de 
// debito y credito de transacciones. Solo para ON/2.
//
// Tarea        Fecha           Autor   Observaciones
// (Alfa)       2006.01.02		mdc		Interfase CONTINUUS ON/2 BANCO CIUDAD DE BS.AS.
// (Alfa)       2007.12.01		mdc		BTFUEGO, Base.
// (Alfa)       2008.07.02		mdc		FIX:Excepto B2B que tampoco verifica la MAC-1-2-3
// (Alfa)       2008.09.01		mdc		FIX:Excepcion ABONADOS con claves no cargadas
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Header de la clase
#include <qusrinc/trxdnet.h>
#include <qusrinc/copymacr.h>
#include <qusrinc/stdiol.h>

#include <stdlib.h>
#include <ctype.h>

#ifdef _SYSTEM_DATANET_TDFUEGO_

// TIMESTAMP-ASCII POR OMISION
#define _DEFAULT_DATE_TIME_STR_ "Jan 01 2007 00:00"
#define _DEFAULT_DATE_STR_      "Jan 01 2007"
#define _TIMESTAMPLENGTH_       (14) 
#define _DATE_STR_LENGTH_       (11) 
#define _TIME_STR_LENGTH_       (17) 

#define FULLCOPYFIELD( to, from )  memcpy( to,from,sizeof(from) );

///////////////////////////////////////////////////////////////////////
TrxResBTF::TrxResBTF(void)
{
    // Sin puntero a base de datos, pero NO des-instanciar aqui 
    // sino hacerlo en quien la haya creado
	ptrBackOffice = NULL;
    // Nulificar cuerpo de mensaje de notificacion
    szMSG_Mail[0]    = 0x00;
    shMSG_Counter    = 0;
    szMSG_Destiny[0] = 0x00;
    // Inicializar estructuras de mensajes
    memset( &refrMsg, 0x00, sizeof(refrMsg) );
    memset( &dpsMsg , 0x00, sizeof(dpsMsg) );
}
///////////////////////////////////////////////////////////////////////
TrxResBTF::TrxResBTF(void *ptr_rpc)
{
	// Asignar puntero de acceso a BDatos 
	// a propiedades internas y clases contenidas
	if(ptr_rpc != NULL)
		ptrBackOffice = (RPC_CONNECTOR_DNET *)ptr_rpc;
	else
		ptrBackOffice = NULL;	

	ptrBackOffice->ResetStatus();

	// Inicializar cuerpo de mensaje de notificacion
    szMSG_Mail[0]    = 0x00;
    shMSG_Counter    = 0;
    szMSG_Destiny[0] = 0x00;
    // Inicializar estructuras de mensajes
    memset( &refrMsg, 0x00, sizeof(refrMsg) );
    memset( &dpsMsg , 0x00, sizeof(dpsMsg) );
}

void TrxResBTF::ResetStatus()
{
	ptrBackOffice->ResetStatus();
}

///////////////////////////////////////////////////////////////////////
// Autorizar primer transaccion disponible
///////////////////////////////////////////////////////////////////////
boolean_t TrxResBTF::AuthorizeFirstTransaction( long *plExtRetStatus )
{
    WORD wLocRespCde = DPS_FUERZA_MAYOR;

    // Postcondicion : Solo por error "FUERZA MAYOR" se retorna FALSE
    // de manera de indicar que no habia mas registros para autorizar.
    wLocRespCde = AuthorizeTransaction( plExtRetStatus );
    if( DPS_FUERZA_MAYOR == wLocRespCde )
    {
        // Postcondicion : Si no hay mas registros, blanquear PK
        // para siguiente lectura secuencial
        memset( &dpsMsg   , ' ', sizeof dpsMsg );
        memset( &dpsMsgBkp, ' ', sizeof dpsMsgBkp );
        // Postcondicion : FALSE,no se ejecuto nada
        return is_false;
    }
    else
    {
        // Postcondicion : TRUE,se ejecuto una transferencia
        return is_true ;
    }
        
}

///////////////////////////////////////////////////////////////////////
// Recuperar primer transaccion disponible
///////////////////////////////////////////////////////////////////////
boolean_t TrxResBTF::RetrieveFirstTransaction(char *lpData, unsigned short *pnLen,                                                 
                                                char *szTStatus,
                                                char *lpDataBkp, unsigned short *pnLenBkp)
{
    // Precondicion : argumentos validos
    if(!szTStatus || !lpData || !pnLen || !pnLenBkp || !lpDataBkp)
        return is_false;

    // Si la recuperacion es exitosa, copiar el buffer a la salida.
    WORD wRespCde = RetrieveTransaction( szTStatus );
    if( DPS_APROBADA_SIN_RECHAZO == wRespCde )
    {
        // Copiar la estructura de salida (respuesta) YA FORMATEADA con MAC calculado.
        memcpy( lpData, &dpsMsg.output.data, min((*pnLen),sizeof(dpsMsg.output.data)));
        (*pnLen)    = sizeof(dpsMsg.output.data);
        // Copiar la estructura de entrada, YA FORMATEADA.
        memcpy( lpDataBkp, &dpsMsg.input.data, min((*pnLenBkp),sizeof(dpsMsg.input.data)));
        (*pnLenBkp) = sizeof(dpsMsg.input.data);
        // Postcondicion : asegurarse que la salida formateada ES VALIDA,
        // y retornar un indicativo si EXISTE o NO EL REGISTRO
        return ( strncmp(dpsMsg.output.data.header.TPNAME,"DPRE",4)==0 )
                ? is_true
                : is_false ;
    }
    else if( DPS_DEMORADA_NO_ENVIAR_RESPUESTA == wRespCde )
    {
        // Copiar la estructura de salida (respuesta) YA FORMATEADA con MAC calculado.
        memcpy( lpData, &dpsMsg.output.data, min((*pnLen),sizeof(dpsMsg.output.data)));
        (*pnLen)    = sizeof(dpsMsg.output.data);
        // Copiar la estructura de entrada, YA FORMATEADA.
        memcpy( lpDataBkp, &dpsMsg.input.data, min((*pnLenBkp),sizeof(dpsMsg.input.data)));
        (*pnLenBkp) = sizeof(dpsMsg.input.data);
        // Postcondicion : NO se debe enviar nada
        return is_false ;
    }
    else 
    {
        // Blanquear Primary-Keys para siguiente ciclo de lectura secuencial
        memset( &dpsMsg.output.data, ' ', sizeof(dpsMsg.output.data));
        memset( &dpsMsg.input.data , ' ', sizeof(dpsMsg.input.data) );
        memset( lpData   , ' ', *pnLen);
        memset( lpDataBkp, ' ', *pnLenBkp);
        (*pnLen)    = 0;
        (*pnLenBkp) = 0;
        // Postcondicion : NO existe registro, no hay nada que enviar
        return is_false ;
    };
}


///////////////////////////////////////////////////////////////////////////////////
// Validacion de trasacciones
// Validar MAC tipo 1, 2 o 3
///////////////////////////////////////////////////////////////////////////////////
WORD TrxResBTF::ValidateMAC(char *szMACTYPE)// Verificacion MAC1, 2 o 3
{
	CHAR  szCodAbonado[10] = {"0000000"},  // Abonado Default PRUEBA-EDS
		  szCodBanco[10]   = {"000"};      // Banco Default PRUEBA-EDS
	UCHAR uchAssocKEY[8+1] = {"00000000"}; // Clave Default PRUEBA-EDS
    UCHAR uchBankKEY[8+1]  = {"00000000"}; // Clave Default PRUEBA-EDS
    short shMacType        = 0;

		// Precondicion
	if( NULL == ptrBackOffice || NULL == szMACTYPE)
		return DPS_FUERZA_MAYOR;
	// MAC '1', '2' o '3' en ASCII?
    shMacType = szMACTYPE[0] - '0';
	if(shMacType != 1 && shMacType != 2 && shMacType != 3)
		return DPS_FUERZA_MAYOR;

    ///////////////////////////////////////////////////////////////////
    // Excepto que sea FACTURACION, porque no se verifica el MAC-1-2-3 
    if( dpsMsg.GetOperType(NULL) == DPS_FACTURACION)
        return DPS_APROBADA_SIN_RECHAZO;
	// FIX:Excepto B2B que tampoco verifica la MAC-1-2-3
    if( dpsMsg.GetOperType(NULL) == DPS_INTERPAGOS)
        return DPS_APROBADA_SIN_RECHAZO;
    ///////////////////////////////////////////////////////////////////
    
    //////////////////////////////////////////////////////////////////////////
    // Obtener la clave de encripcion-desencripcion
	// Validar el abonado de la cuenta
    COPYFIELD(szCodAbonado, dpsMsg.input.data.NUMABO); //  Abonado	, CHAR(7)
    //////////////////////////////////////////////////////////////////////////
    // Validar existencia del banco DEBITO si es que no somos nosotros mismos
    if( ! IS_ON_OUR_BANKCODE(dpsMsg.input.data.BANDEB) )
    {
        COPYFIELD(szCodBanco  , dpsMsg.input.data.BANDEB) //  Banco debito, CHAR(3)
	    ptrBackOffice->GetEncriptionKeys( szCodAbonado, szCodBanco, (char *)uchAssocKEY, (char *)uchBankKEY );
	    if(ptrBackOffice->GetStatus() != 0)
		    return DPS_FALTAN_DATOS_EN_DETALLE_OPERACION;
    }
    //////////////////////////////////////////////////////////////////////////
    // Validar existencia del banco CREDITO si es que no somos nosotros mismos
    if( ! IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) )
    {
        COPYFIELD(szCodBanco  , dpsMsg.input.data.DATCRE.BANCRE) //  Banco credito CHAR(3)
	    ptrBackOffice->GetEncriptionKeys( szCodAbonado, szCodBanco, (char *)uchAssocKEY, (char *)uchBankKEY );
	    if(ptrBackOffice->GetStatus() != 0)
		    return DPS_FALTAN_DATOS_EN_DETALLE_OPERACION;
    }
    //////////////////////////////////////////////////////////////////////////
    // Validar existencia del banco correspondiente al MAC solicitado (1,2,o 3)
    if( 1 == shMacType )      COPYFIELD(szCodBanco  , dpsMsg.input.data.BANDEB) //  Banco debito, CHAR(3)
    else if( 2 == shMacType ) COPYFIELD(szCodBanco  , dpsMsg.input.data.BANDEB) //  Banco credito, CHAR(3)
    else if( 3 == shMacType ) COPYFIELD(szCodBanco  , dpsMsg.input.data.DATCRE.BANCRE) //  Banco credito, CHAR(3)
    else return DPS_FALTAN_DATOS_EN_DETALLE_OPERACION;
    // Acceder a base de datos
	ptrBackOffice->GetEncriptionKeys( szCodAbonado, szCodBanco, (char *)uchAssocKEY, (char *)uchBankKEY );
	if(ptrBackOffice->GetStatus() != 0)
		return DPS_FALTAN_DATOS_EN_DETALLE_OPERACION;
  
#if ( TRUE )
	// FIX:Excepcion ABONADOS con claves no cargadas
    if( 1 == shMacType && strcmp((char*)uchAssocKEY,"********")==0)
        return DPS_APROBADA_SIN_RECHAZO;	 
#endif

	// Verificar MAC con algoritmo base
    // Postcondicion : retorno de funcion heredada
    return TrxResDATANET::ValidateMAC(szMACTYPE,uchAssocKEY,uchBankKEY);

}


//////////////////////////////////////////////////////////////////////
// Calcular MAC tipo 2 o 3
///////////////////////////////////////////////////////////////////////////////////
WORD TrxResBTF::CalculateMAC(char *szMACTYPE, char*szExtMAC)// Verificacion MAC 2 o 3
{
	CHAR  szCodAbonado[10]     = {"0000000"},// Abonado Default PRUEBA-EDS
		  szCodBanco[10]       = {"000"};    // Banco Default PRUEBA-EDS		      
	UCHAR uchAssocKEY[8+1] = {"00000000"};        // Clave Default PRUEBA-EDS
    UCHAR uchBankKEY[8+1]  = {"00000000"};        // Clave Default PRUEBA-EDS
    CHAR  szLocMACTYPE[2] = {"0"}; // Tipo de MAC ASCII y local
    short shMacType = 0;         // Tipo de MAC binario y local

		// Precondicion
	if( NULL == ptrBackOffice || NULL == szMACTYPE || NULL == szExtMAC)
		return DPS_FUERZA_MAYOR;
	// MAC '2' o '3' en ASCII?
    szLocMACTYPE[0] = szMACTYPE[0];
    shMacType = szMACTYPE[0] - '0';
	if(shMacType != 2 && shMacType != 3)
		return DPS_FUERZA_MAYOR;

    // Obtener la clave de encripcion-desencripcion
	// Validar el abonado de la cuenta
    COPYFIELD(szCodAbonado, dpsMsg.input.data.NUMABO); //  Abonado, CHAR(7)
    if( 2 == shMacType )      COPYFIELD(szCodBanco, dpsMsg.input.data.DATCRE.BANCRE) //  Banco credito, CHAR(3)
    else if( 3 == shMacType ) COPYFIELD(szCodBanco, dpsMsg.input.data.BANDEB)        //  Banco debito , CHAR(3)
    else return DPS_FALTAN_DATOS_EN_DETALLE_OPERACION;

	ptrBackOffice->GetEncriptionKeys( szCodAbonado, szCodBanco, (char *)uchAssocKEY, (char *)uchBankKEY );
	if(ptrBackOffice->GetStatus() != 0)
		return DPS_FALTAN_DATOS_EN_DETALLE_OPERACION;

	// Calcular MAC con algoritmo base
    return TrxResDATANET::CalculateMAC(szLocMACTYPE,szExtMAC,uchAssocKEY,uchBankKEY);
}

///////////////////////////////////////////////////////////////////////
WORD  TrxResBTF::ValidateExistingAccount()   // Verificar cuenta existente
{
	CHAR  szNroCuenta[sizeof(dpsMsg.input.data.DATDEB.NUMCTA)+1] = 
            {"                 "};// Cuenta Nro. DEFAULT

	// Precondicion
	if( NULL == ptrBackOffice)
		return ( DPS_FUERZA_MAYOR );
	// Pre-Validar datos de la transaccion, 0 por OK
	if( !dpsMsg.IsDPSTransaction() ) 
		return ( DPS_OPERACION_INEXISTENTE );	
    if( IS_ON_OUR_BANKCODE(dpsMsg.input.data.BANDEB) )
    {
	    // Verificacion de campos
	    if(strncmp( dpsMsg.input.data.DATDEB.TIPCUE,"                    ",
		    sizeof(dpsMsg.input.data.DATDEB.TIPCUE))==0)
		    return ( DPS_FALTAN_DATOS_EN_DETALLE_OPERACION );
	    // Copia local de variables
	    strncpy( szNroCuenta , dpsMsg.input.data.DATDEB.NUMCTA, 
            sizeof(dpsMsg.input.data.DATDEB.NUMCTA));
	    // Validar el abonado de la cuenta
	    ptrBackOffice->ValidateExistingAccount( szNroCuenta ,
            antoi(dpsMsg.input.data.DATDEB.TIPCUE,2) );
	    if(ptrBackOffice->GetStatus() != 0)
		    return ( DPS_CUENTA_INVALIDA );
    };
	// Verificar CUENTA A ACREDITAR si pertenece a este banco    
    if( IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) )
    {
	    // Verificacion de campos
	    if(strncmp( dpsMsg.input.data.DATCRE.TIPCRE,"                    ",
		    sizeof(dpsMsg.input.data.DATCRE.TIPCRE))==0)
		    return ( DPS_FALTAN_DATOS_EN_DETALLE_OPERACION );
	    // Copia local de variables
	    strncpy( szNroCuenta , dpsMsg.input.data.DATCRE.CTACRE, 
            sizeof(dpsMsg.input.data.DATCRE.CTACRE));
	    // Validar el abonado de la cuenta
	    ptrBackOffice->ValidateExistingAccount( szNroCuenta ,
            antoi(dpsMsg.input.data.DATCRE.TIPCRE,2) );
	    if(ptrBackOffice->GetStatus() != 0)
		    return ( DPS_CUENTA_INVALIDA );
    };

	// OK
	return ( DPS_APROBADA_SIN_RECHAZO );
}
///////////////////////////////////////////////////////////////////////
WORD  TrxResBTF::ValidateAccountStatus(char *pszEstado)     // Verificar estado de la cuenta
{
	CHAR  szNroCuenta[sizeof(dpsMsg.input.data.DATDEB.NUMCTA)+1] = 
            {"                 "};   // Cuenta Nro. DEFAULT
	CHAR  szEstadoCuenta[5] = {"A"}; // Estado Cta. DEFAULT NO EXISTE, 'X'
    WORD  wRetVal = DPS_APROBADA_SIN_RECHAZO;
	
    // Precondicion
	if( NULL == ptrBackOffice)
		return ( DPS_FUERZA_MAYOR );
	// Pre-Validar datos de la transaccion, 0 por OK
	if( !dpsMsg.IsDPSTransaction() ) 
		return ( DPS_OPERACION_INEXISTENTE );	
	// Verificacion de campos                    ....:....1....:....2
	if(strncmp( dpsMsg.input.data.DATDEB.NUMCTA,"                    ",
		sizeof(dpsMsg.input.data.DATDEB.NUMCTA))==0)
		return ( DPS_FALTAN_DATOS_EN_DETALLE_OPERACION );
	// Verificacion de campos                    ....:....1....:....2
	if(strncmp( dpsMsg.input.data.DATCRE.CTACRE,"                    ",
		sizeof(dpsMsg.input.data.DATCRE.CTACRE))==0)
		return DPS_FALTAN_DATOS_EN_DETALLE_OPERACION;

    ////////////////////////////////////////////////////////////////
    // Excepto que sea FACTURACION, porque no se verifica el estado 
    if( dpsMsg.GetOperType(NULL) == DPS_FACTURACION)
        return DPS_APROBADA_SIN_RECHAZO;
    ////////////////////////////////////////////////////////////////

	// Verificar CUENTA A DEBITAR si pertenece a este banco
    if( IS_ON_OUR_BANKCODE(dpsMsg.input.data.BANDEB) )
    {
	    strncpy( szNroCuenta , dpsMsg.input.data.DATDEB.NUMCTA, 
                 sizeof(dpsMsg.input.data.DATDEB.NUMCTA) );	
	    // Validar el abonado de la cuenta
	    ptrBackOffice->ValidateAccountStatus( szNroCuenta, szEstadoCuenta,
            antoi(dpsMsg.input.data.DATDEB.TIPCUE,2) );
	    if(ptrBackOffice->GetStatus() != 0)
		    return ( DPS_CUENTA_INVALIDA );
        if('A' == szEstadoCuenta[0])       wRetVal = DPS_APROBADA_SIN_RECHAZO;
        else if('C' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_CERRADA;
        else if('E' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_EMBARGADA;
        else if('B' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_BLOQUEADA;
        else if('I' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_EN_VIA_DE_CIERRE;
        // No es ERROR un bloqueo contra creditos en cuenta debito
        else if('1' == szEstadoCuenta[0])  wRetVal = DPS_APROBADA_SIN_RECHAZO; // credito
        // Si es ERROR un bloqueo contra debitos en cuenta debito
        else if('2' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_BLOQUEADA; // debito
        else if('3' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_BLOQUEADA; // ambos cr-db
        else if(' ' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_INVALIDA;
        else if(0x00== szEstadoCuenta[0])  wRetVal = DPS_CUENTA_INVALIDA;
        else                               wRetVal = DPS_CUENTA_INVALIDA;
        // Ante un error, informar
        if(DPS_APROBADA_SIN_RECHAZO != wRetVal)
        {
            // Copiar estado aunque sea el INEXISTENTE y si se paso el argumento
    	    if(pszEstado) strcpy(pszEstado, szEstadoCuenta);
            // ERROR
	        return ( wRetVal );
        };
    }; /*end-if-CTA-DEBITO*/

	// Verificar CUENTA A ACREDITAR si pertenece a este banco    
    if( IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) )
    {
	    strncpy( szNroCuenta , dpsMsg.input.data.DATCRE.CTACRE, 
                 sizeof(dpsMsg.input.data.DATCRE.CTACRE) );	
	    // Validar el abonado de la cuenta
	    ptrBackOffice->ValidateAccountStatus( szNroCuenta, szEstadoCuenta,
            antoi(dpsMsg.input.data.DATCRE.TIPCRE,2) );
	    if(ptrBackOffice->GetStatus() != 0)
		    return ( DPS_CUENTA_INVALIDA );
        if('A' == szEstadoCuenta[0])       wRetVal = DPS_APROBADA_SIN_RECHAZO;
        else if('C' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_CERRADA;
        else if('E' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_EMBARGADA;
        else if('B' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_BLOQUEADA;
        else if('I' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_EN_VIA_DE_CIERRE;
        // Si es ERROR un bloqueo contra creditos en cuenta credito
        else if('1' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_BLOQUEADA; // credito
        // No es ERROR un bloqueo contra debitos en cuenta credito
        else if('2' == szEstadoCuenta[0])  wRetVal = DPS_APROBADA_SIN_RECHAZO; // debito
        else if('3' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_BLOQUEADA; // ambos cr-db
        else if(' ' == szEstadoCuenta[0])  wRetVal = DPS_CUENTA_INVALIDA;
        else if(0x00== szEstadoCuenta[0])  wRetVal = DPS_CUENTA_INVALIDA;
        else                               wRetVal = DPS_CUENTA_INVALIDA;
        // Ante un error, informar
        if(DPS_APROBADA_SIN_RECHAZO != wRetVal)
        {
            // Copiar estado aunque sea el INEXISTENTE y si se paso el argumento
    	    if(pszEstado) strcpy(pszEstado, szEstadoCuenta);
            // ERROR
	        return ( wRetVal );
        };
    }; /*end-if-CTA-CREDITO*/

	// OK ! Si llego hasta este punto, esta todo OK.
	return ( wRetVal );
}
///////////////////////////////////////////////////////////////////////
WORD  TrxResBTF::ValidateAccountAssociate()  // Verificar abonado de la cuenta
{
    ////////////////////////////////////////////
	CHAR  szCodAbonado[10]     = {0}, // Abonado
		  szNroCuenta[20]      = {0}; // Cuenta
    ////////////////////////////////////////////


    // Precondicion
	if( NULL == ptrBackOffice)
		return DPS_FUERZA_MAYOR;
	// Pre-Validar datos de la transaccion, 0 por OK
	if( !dpsMsg.IsDPSTransaction() ) 
		return DPS_OPERACION_INEXISTENTE;	
	// Verificacion de campos
	if(strncmp( dpsMsg.input.data.NUMABO,"       ",sizeof(dpsMsg.input.data.NUMABO))==0)
		return DPS_FALTAN_DATOS_EN_DETALLE_OPERACION;
	if(strncmp( dpsMsg.input.data.BANDEB,"       ",sizeof(dpsMsg.input.data.BANDEB))==0)
		return DPS_FALTAN_DATOS_EN_DETALLE_OPERACION;

    ////////////////////////////////////////////////////////////////
    // Excepto que sea FACTURACION, porque no se verifica el ABONADO
    if( dpsMsg.GetOperType(NULL) == DPS_FACTURACION)
        return DPS_APROBADA_SIN_RECHAZO;
    ////////////////////////////////////////////////////////////////


    // Copia local de variables
	strncpy( szCodAbonado, dpsMsg.input.data.NUMABO, sizeof(dpsMsg.input.data.NUMABO));
	strncpy( szNroCuenta , dpsMsg.input.data.DATDEB.NUMCTA, sizeof(dpsMsg.input.data.DATDEB.NUMCTA));	
	// Validar el abonado de la cuenta
	ptrBackOffice->ValidateAccountAssociate( szCodAbonado, szNroCuenta );
	if(ptrBackOffice->GetStatus() != 0)
    {
        // Error por omision
        WORD wRetVal = DPS_CUENTA_INVALIDA;
        // Cuenta invalida, no existe afiliado
		wRetVal = DPS_CUENTA_INVALIDA;
        // Se modifica por otro error, 'MAC Erroneo'
        wRetVal = DPS_MAC_ERRONEO;
        return ( wRetVal );
    };

	// OK
	return DPS_APROBADA_SIN_RECHAZO;
}

///////////////////////////////////////////////////////////////////////
// Controles de trasacciones en general
WORD  TrxResBTF::ProcessInit(boolean_t bIsAuth)	// Proceso base, validacion		
{	
	// Precondicion
	if(!ptrBackOffice)
		return DPS_FUERZA_MAYOR;
	// Retorno de metodo sobrecargado de clase heredada
	return TrxResDATANET::ProcessInit( bIsAuth );
};

////////////////////////////////////////////////////////////////////
// Mapear errores BTF si no estuvieran ya mepeados
WORD  TrxResBTF::MapBTFErrors( long lRetStatus )
{
// No usar este mapeo xque YA viene mapeado....
#if FALSE
    WORD wRespCde = 0;
    switch(lRetStatus)
    {
        case     0: wRespCde = DPS_APROBADA_SIN_RECHAZO ; break;     
        case     1: wRespCde = DPS_APROBADA_SIN_RECHAZO; break;     
		case     2: wRespCde = DPS_OPERACION_NO_HABILITADA_EN_BANCO; break;     
		case     3: wRespCde = DPS_SALDO_INSUFICIENTE    ; break;     
		case     4: wRespCde = DPS_CUENTA_INVALIDA       ; break;     
		case     5: wRespCde = DPS_CUENTA_BLOQUEADA;       break;
		/* faltan mas */
        default   : wRespCde = DPS_OPERACION_INEXISTENTE; break;  
    }
    return wRespCde;
#else
	// Si esta en el rango DATANET de 1 a 40, devolver eso. Sino defualt = 21.
	return ( lRetStatus >=1 && lRetStatus<= 40)
		? (WORD) lRetStatus
		: (WORD) DPS_OPERACION_NO_HABILITADA_EN_BANCO;
#endif
}
//////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////
// Actualizar transaccion con el sistema autorizador del banco
///////////////////////////////////////////////////////////////////////////////////
boolean_t TrxResBTF::UpdateTransaction( char *pszTxt , size_t nLen )
{
	// Campos locales que mapean campos de estructruras DPS
	char szBANDEB [3+1] = {"000"}; //  Código Banco según BCRA	ej."067" Bansud, CHAR(3)
	char szFECSOL [8+3+1+4+1+1] = {_DEFAULT_DATE_TIME_STR_}; //  Fecha de compensación, de día o prox. Hábil -aaaammdd-, CHAR(8)
	char szNUMTRA [7+1] = {"0000000"}; //  Número de transferencia,CHAR(7) 
	char szNUMABO [7+1] = {"0000000"}; //  Abonado	, CHAR(7)
	char szNUMCTADEB [17+1] = {"00000000000000000"}; //  Número de cuenta, formato Banco, CHAR(17)
	char szBANCRE [3+1] = {"000"}; //  Código de Banco según BCRA, p/credito. CHAR(3)
	char szCTACRE[17+1] = {"00000000000000000"}; //  Número de cuenta -Formato Banco-, CHAR(17)
	char szESTADO [2+1] = {"00"}; //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	char szNUMREF [7+1] = {"0000000"}; //  Número de transferencia PC	 [+1] = {" "}; //  nro.lote (N,3)+nro. operación (N,4) , CHAR(7) 
	char szNUMENV [3+1] = {"000"}; //  Número de envío del abonadochar(3)
    char szPENDENVIO[2] = { _DPS_PEND_ENV_NO_ }; //  Pendiente de ENVIO pasa a ENVIADO, "N"
    char szRECHDB [4+1] = {"0000"}; //  Motivo del rechazo del Banco de débito -Ver tabla III - CHAR(4)
    char szRECHCR [4+1] = {"0000"}; //  Motivo del rechazo del Banco de credito -Ver tabla III - CHAR(4)
    char szESTADOTRAT[2]= { _DPS_TRAT_PROCESADA_ }; //  Estado tratamiento por omision en PROCESADA, CHAR(1)
    WORD       wRespCde = 0;
    size_t     tSize    = sizeof(dpsMsg.input.data);
    ////////////////////////////////////////////////////////////////////
	const char bFORMAT_DATE_AND_TIME = FALSE;
    // Handle de archivo de impresion de parametros hacia B. Datos
	FILE        *hFile                = NULL;
    const char  bDEBUG_PRINT_PARAMS   = TRUE;        
    ////////////////////////////////////////////////////////////////////

	// Precondicion : punteros validos y longitudes validas
	if( NULL == ptrBackOffice || NULL == pszTxt || nLen <= 0)
		return is_false; 

    ////////////////////////////////////////////////////////////////////////
    // Copiar mensaje original a la estructura, unicamente el HEADER
    tSize = sizeof(dpsMsg.input.data) ;
    if(nLen < tSize)
        tSize = nLen ;
    memcpy( &dpsMsg.input.data, pszTxt, tSize );
    ////////////////////////////////////////////////////////////////////////

	// Pre-Validar datos de la transaccion : 
	// QUE SI ES DEL REFRESH ONLINE... o de COBRANZAS....
	if( dpsMsg.IsRefreshMovement() ) 
	{
        // ...ENTONCES QUE RETORNE OK, porque no se actualiza nada de nada
		return is_true;	
	}

    ////////////////////////////////////////////////////////////////////////
	// Pre-Validar datos de la transaccion, 0 por OK
	if( !dpsMsg.IsDPSTransaction() ) 
        // Error, porque no es un registro procesable
		return is_false;	


	// Adecuacion de campos de estructuras a variables string
	COPYFIELD(szBANDEB , dpsMsg.input.data.BANDEB); //  Código Banco según BCRA	ej."067" Bansud, CHAR(3)
	COPYFIELD(szFECSOL , dpsMsg.input.data.FECSOL ); //  Fecha de compensación, de día o prox. Hábil -aaaammdd-, CHAR(8)
    // Pre-formatear campo
	if( bFORMAT_DATE_AND_TIME )
		FormatTrxDateTime_WMask(szFECSOL, NULL, dpsMsg.input.data.FECSOL, NULL, 
                            is_true, is_true, is_false, is_true, 0, 0 );
	COPYFIELD(szNUMTRA , dpsMsg.input.data.NUMTRA); //  Número de transferencia,CHAR(7) 
	COPYFIELD(szNUMABO , dpsMsg.input.data.NUMABO); //  Abonado	, CHAR(7)
	COPYFIELD(szNUMCTADEB , dpsMsg.input.data.DATDEB.NUMCTA); //  Número de cuenta, formato Banco, CHAR(17)
	COPYFIELD(szBANCRE , dpsMsg.input.data.DATCRE.BANCRE ); //  Código de Banco según BCRA, p/credito. CHAR(3)
	COPYFIELD(szCTACRE, dpsMsg.input.data.DATCRE.CTACRE ); //  Número de cuenta -Formato Banco-, CHAR(17)
	COPYFIELD(szESTADO, dpsMsg.input.data.ESTADO );//  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	COPYFIELD(szNUMREF, dpsMsg.input.data.NUMREF );//  Número de transferencia PC	 , //  nro.lote (N,3)+nro. operación (N,4) , CHAR(7) 
	COPYFIELD(szNUMENV, dpsMsg.input.data.NUMENV );//  Número de envío del abonadochar(3)
    // Motivos de rechazo CR/DB
    COPYFIELD(szRECHDB, dpsMsg.input.data.DATDEB.RECHDB );//  Motivo de rechazo
    COPYFIELD(szRECHCR, dpsMsg.input.data.DATCRE.RECHCR );//  Motivo de rechazo
    ////////////////////////////////////////////////////////////////////////////////////
    // Si el estado tratamiento variara en funcion del ESTADO original
    // tal que si era una demorada '20' en estado tratamiento procesada '9',
    // ahora pasara nuevamente a pendiente '0' para su re-ejecucion.
    // PROGRAMA    : ESTADO  :  TRATAMIENTO  : OBSERVACIONES
    // -----------------------------------------------------------------------------------
    // AUTORIZADOR : "00"    :      "0"      : Llega desde la red como enviada por abonado
    // AUTORIZADOR : "20"    :      "9"      : 1er DEMORADO es definitivo
    // EMISOR      : "20"    :      "0"      : El EMISOR envia rta. y la vuelve a PENDIENTE
    // AUTORIZADOR : "20"    :      "0"      : Sucesivamente se reintenta 
    if(antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO
        && 
       _DPS_TRAT_PROCESADA_C_ == szESTADOTRAT[0] ) 
        szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_ ; //  Estado tratamiento es PENDIENTE
    else
        szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ; //  Estado tratamiento es PROCESADA
    ////////////////////////////////////////////////////////////////////////////////////


    // Actualizar la transaccion en la tabla TEF ONLINE
    wRespCde = ptrBackOffice->UpdateTransaction(
	    szBANDEB , //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
	    szFECSOL , //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	    szNUMTRA , //  Numero de transferencia,CHAR(7) 
	    szNUMABO , //  Abonado	, CHAR(7)
	    szNUMCTADEB, //  Numero de cuenta, formato Banco, CHAR(17)
	    szBANCRE , //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
	    szCTACRE , //  Numero de cuenta -Formato Banco-, CHAR(17)
	    szESTADO , //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	    szNUMREF , //  Numero de transferencia PC	 []
	    szNUMENV , //  Numero de envio del abonadochar(3)
        szPENDENVIO, // Pendiente de ENVIO se modifica y pasa a ENVIADO, "N"
        szRECHDB,    // Motivos de rechazo DB
        szRECHCR,  // Motivos de rechazo CR
        szESTADOTRAT); // Estado tratamiento
	if(ptrBackOffice->GetStatus() != 0)
	{
		/////////////////////////////////////////////////////////
		if(bDEBUG_PRINT_PARAMS) hFile = fopen("debug.out", "a");
		if (hFile && bDEBUG_PRINT_PARAMS)
		if (hFile && bDEBUG_PRINT_PARAMS)
		{
			fprintf(hFile,"\r\n%s %s\r\n", 
				ptrBackOffice->GetErrorMessage(NULL) , ptrBackOffice->GetLastSQLCommand(NULL) );			
	        fprintf(hFile,"\r\n----------------------");
		    fclose(hFile);        
		}
		/////////////////////////////////////////////////////////
		return is_false; // DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED;
	}

	// OK
	return is_true ; // DPS_APROBADA_SIN_RECHAZO;
}
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// Loggear la transaccion con el sistema autorizador del banco
///////////////////////////////////////////////////////////////////////////////////
WORD TrxResBTF::LoggTransaction(char *szESTADOTRAT , //  Estado tratamiento externo
                                  char *szEXTPEDENVIO) //  Pendiente de envio?
{
	// Campos locales que mapean campos de estructruras DPS
	char szFRNAME [4+1] = {0x00};// Nombre nodo emisor, ej. "DNET", char(4)
	char szTONAME [4+1] = {0x00}; //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
	char szRECTYPE [3+1] = {0x00};// Tipo de registro, TDE = débito TCE = crédito, CHAR(3)
    char chACTIVITY[2] = {"N"};  //  Actividad, CHAR(1)="N"
    char szFILLER1[6+1] = {"150   "}; // RESERVADO
	char szBANDEB [3+1] = {0x00}; //  Código Banco según BCRA	ej."067" Bansud, CHAR(3)
	char szFECSOL [8+2+1] = {0x00}; //  Fecha de compensación, de día o prox. Hábil -aaaammdd-, CHAR(8)
	char szNUMTRA [7+1] = {0x00}; //  Número de transferencia,CHAR(7) 
	char szNUMABO [7+1] = {0x00}; //  Abonado	, CHAR(7)
	char szTIPOPE [2+1] = {0x00}; //  Tipo de operación -	Ver tabla I - , CHAR(2)
	/* VISUAL .NET 2003 STACK CORRUPTED */
	char szIMPORTE[17+4+1] = {0x00};//  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	/* VISUAL .NET 2003 STACK CORRUPTED */
	char szSUCDEB [4+1] = {0x00};  //  Sucursal del Banco que tratará el débito, CHAR(4)
	char szNOMSOL [30+1] = {0x00};  //  Nombre de la cuenta, 
	char szTIPCUEDEB[2+1] = {0x00};//   Tipo de cuenta	- Ver tabla II  - CHAR(2)
	char szNCUECMDEB [2+1] = {0x00}; //  Número de cuenta corto -Interno Datanet- CHAR(2)
	char szNUMCTADEB [17+1] = {0x00}; //  Número de cuenta, formato Banco, CHAR(17)
	char szFSENDB [8+3+1+6+1+1+32] = {0x00}; //  Fecha de envío de la transf.  al Banco -Aammdd-, CHAR(6)
	char szHSENDB [8+3+1+6+1+1+32] = {0x00}; // Hora de envío, Hhmm, CHAR(4)
	char szOPEDB1 [2+1] = {0x00}; //  Identificación del operador #1. Primer autorizante del Banco de db. CHAR(2)
	char szOPEDB2 [2+1] = {0x00}; //  Identificación del operador #2. Segundo autorizante, CHAr(2).
	char szRECHDB [4+1] = {0x00}; //  Motivo del rechazo del Banco de débito -Ver tabla III - CHAR(4)
	char szBANCRE [3+1] = {0x00}; //  Código de Banco según BCRA, p/credito. CHAR(3)
	char szSUCCRE [4+1] = {0x00}; //  Sucursal del Banco que tratará el crédito. CHAR(4)				
	char szNOMBEN[30+1] = {0x00};  //  Nombre de la cuenta a acreditar, VARCHAR(29)
	char szTIPCRE [2+1] = {0x00}; //  Tipo de cuenta -Ver tabla II-, CHAR(2)
	char szCTACRE[17+1] = {0x00}; //  Número de cuenta -Formato Banco-, CHAR(17)
	char szFSENCR[6+2+1] = {0x00}; //  Fecha de envío de la transf.  al Banco	 -Aammdd-, CHAR(6)
	char szHSENCR[8+3+1+6+1+1+32] = {0x00}; //  Hora de envío -Hhmm-, CHAR(4)
	char szOPECR1[2+1] = {0x00}; //  Identificación del operador #1, primer autorizante del Banco de cr. CHAR(2)
	char szOPECR2[2+1] = {0x00}; //  Identificación del operador #2, segundo autorizante, CHAR(2)
	char szRECHCR[4+1] = {0x00}; //  Motivo del rechazo del Banco de crédito -ver tabla III-, CHAR(4)
	char szOPECON [2+1] = {0x00}; //  Operador que confeccionó la transferencia	, CHAR(2)
	char szOPEAU1 [2+1] = {0x00}; //  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	char szOPEAU2 [2+1] = {0x00}; //  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	char szOPEAU3 [2+1] = {0x00}; //  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	char szFECAUT [6+2+1] = {0x00}; //  Fecha de autorización (envío del pago a la red) aammdd,  CHAR(6)
	char szHORAUT [8+3+1+6+1+1+32] = {0x00}; //  Hora de autorización (envío del pago a la red) hhmm, CHAR(4)
	char szESTADO [2+1] = {0x00}; //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	char szFECEST [6+2+1] = {0x00}; //  Fecha última modificación de ESTADO	, CHAR(6)
	char szOBSER1 [60+1] = {0x00}; //  Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
	char szOBSER2 [100+1] = {0x00}; //  Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
	char szCODMAC [DNET_MAC_LENGTH+1] = {0x00}; //  Message Authentication Code (MAC), CHAR(12)
	char szNUMREF [7+1] = {0x00}; //  Número de transferencia PC	 [+1] = {0x00}; //  nro.lote (N,3)+nro. operación (N,4) , CHAR(7) 
	char szNUMENV [3+1] = {0x00}; //  Número de envío del abonadochar(3)
    char chCONSOL[2] = {"N"}; //  Marca de consolidación de movimientos ("S" ó "N")	"S" se genera un 
		// único movimiento en operaciones de débito con igual fecha de solicitud y  nro. de 
		// lote, CHAR(1)
    char chMARTIT[2] = {"N"}; //  Indica que la cuenta de débito y la cuenta de crédito son de la misma
		// empresa, CHAR(1)
    char chPRIVEZ[2] = {"N"}; //  "S" indica que al momento en que se envía el registro al Banco de 
		//débito no existe un pago previo aceptado por los dos Bancos que involucre a las 
		//cuentas intervinientes."N" indica que los Bancos aceptaron previamente un pago en el
		//que intervenían las mismas cuentas	, CHAR(1)
    char chDIFCRE[2] = {"N"}; //  "S" habilita al Banco de débito que así lo desee a tratar la 
		//transferencia sin importar que sea diferida (prox. hábil)."N" indica que el Banco de
		//débito no puede tratar la transferencia diferida porque el Banco de crédito no las 
		//puede manejar. char chAR(1)
    char chRIEABO[2] = {"N"}; //  "S" indica que el pago supera el riesgo fijado"N" el pago no supera 
		//el riesgochar chAR(1)
    char chRIEBCO[2] = {"N"}; //  "S" indica que al ser aceptado el débito la transferencia superó el 
		//riesgo establecido"N" la transferencia no supera el riesgo pero tiene tratamiento 
		//del crédito obligatorio" " indica que la transferencia quedó ejecutada por el 
		//débito (no superó el riesgo ni fue tratado el crédito),CHAR(1)							        
    ////////////////////////////////////////////////////////////////////
    // Error local , ante eventualidades
    WORD wLRespCde = DPS_APROBADA_SIN_RECHAZO;
    // Indice de SCAN de contenido
    short   nIndex = 0;
    ////////////////////////////////////////////////////////////////////
    // Estados anteriores
	char szESTADOSANT[140+1] = {" "}; //  Estados anteriores (140)
    //////////////////////////////////////////////////////////////
	char szCUITDEB[11+1] = {" "}; // CUIT DEBITO
	char szCUITCRE[11+1] = {" "}; // CUIT CREDITO
    char szFILLER2[25+1] = {" "}; // FILLER FINAL
    ////////////////////////////////////////////////////////////////////
    // Handle de archivo de impresion de parametros hacia B. Datos
	FILE        *hFile                = NULL;
    const char  bDEBUG_PRINT_PARAMS   = TRUE;        
	const char  bFORMAT_DATE_AND_TIME = FALSE;
    //////////////////////////////////////////////////////////////

	// Precondicion
	if( NULL == ptrBackOffice )
		return DPS_FUERZA_MAYOR;
	// Precondicion : validez de datos de la transaccion, 0 por OK
	if( !dpsMsg.IsDPSTransaction() ) 
		return DPS_OPERACION_INEXISTENTE;	
    // Precondicion : Estado tratamiento informado en forma parametrica
	if( NULL == szESTADOTRAT)
        szESTADOTRAT = _DPS_TRAT_PENDIENTE_ ; // Sino, asumir valor por omision
	// Precondicion : valores validos del estado tratamiento (NUMERICOS)
	else if( !isdigit( szESTADOTRAT[0] ) )
        return DPS_FUERZA_MAYOR;

	// Adecuacion de campos de estructuras a variables string
	COPYFIELD( szFRNAME , dpsMsg.input.data.FRNAME ); // Nombre nodo emisor, ej. "DNET", char(4)
	COPYFIELD(szTONAME , dpsMsg.input.data.TONAME); //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
	COPYFIELD(szRECTYPE , dpsMsg.input.data.RECTYPE); // Tipo de registro, TDE = débito TCE = crédito, CHAR(3)
	chACTIVITY[0] = dpsMsg.input.data.ACTIVITY[0];  //  Actividad, CHAR(1)="N"
    COPYFIELD(szFILLER1, dpsMsg.input.data.FILLER1); //  RESERVADO
	//////////////////////////////////////////////////////////////
	// Verificar NULL en caracteres 
    for(nIndex=0;nIndex<sizeof(dpsMsg.input.data.FILLER1);nIndex++)
	{
        if(0x00 == szFILLER1[nIndex]) szFILLER1[nIndex] = 0xff;   /* igual a null               */ 
		else if(0x27 == szFILLER1[nIndex]) szFILLER1[nIndex] = 0xff;   /* igual a comilla simple     */ 
		else if(0x7f <= szFILLER1[nIndex]) szFILLER1[nIndex] = 0xff;   /* mayor a extendido (ascii-7)*/ 
	}
	//////////////////////////////////////////////////////////////
	COPYFIELD(szBANDEB , dpsMsg.input.data.BANDEB); //  Código Banco según BCRA	ej."067" Bansud, CHAR(3)
	COPYFIELD(szFECSOL , dpsMsg.input.data.FECSOL ); //  Fecha de compensación, de día o prox. Hábil -aaaammdd-, CHAR(8)
    // Pre-formatear campo
	if( bFORMAT_DATE_AND_TIME )
		FormatTrxDateTime_WMask(szFECSOL, NULL, dpsMsg.input.data.FECSOL, NULL, 
                            is_true, is_true, is_false, is_true, 0, 0, 4 ); /* YEAR DIGITS IS 4 , 20051110-MDC */
	COPYFIELD(szNUMTRA , dpsMsg.input.data.NUMTRA); //  Número de transferencia,CHAR(7) 
	COPYFIELD(szNUMABO , dpsMsg.input.data.NUMABO); //  Abonado	, CHAR(7)
	COPYFIELD(szTIPOPE , dpsMsg.input.data.TIPOPE); //  Tipo de operación -	Ver tabla I - , CHAR(2)
	COPYFIELD(szIMPORTE, dpsMsg.input.data.IMPORTE); //  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
    Format_ISO_Ammount( dpsMsg.input.data.IMPORTE, 2, szIMPORTE, sizeof(szIMPORTE), 
        is_false, NULL ); 
	COPYFIELD(szSUCDEB , dpsMsg.input.data.DATDEB.SUCDEB); //  Sucursal del Banco que tratará el débito, CHAR(4)
	COPYFIELD(szNOMSOL , dpsMsg.input.data.DATDEB.NOMSOL); //  Nombre de la cuenta, 
	COPYFIELD(szTIPCUEDEB, dpsMsg.input.data.DATDEB.TIPCUE); //   Tipo de cuenta	- Ver tabla II  - CHAR(2)
	COPYFIELD(szNCUECMDEB , dpsMsg.input.data.DATDEB.NCUECM); //  Número de cuenta corto -Interno Datanet- CHAR(2)
	COPYFIELD(szNUMCTADEB , dpsMsg.input.data.DATDEB.NUMCTA); //  Número de cuenta, formato Banco, CHAR(17)    
	COPYFIELD(szFSENDB , dpsMsg.input.data.DATDEB.FSENDB ); //  Fecha de envío de la transf.  al Banco -Aammdd-, CHAR(6)
	COPYFIELD(szHSENDB , dpsMsg.input.data.DATDEB.HSENDB ); // Hora de envío, Hhmm, CHAR(4)
    // Pre-formatear campo
	if( bFORMAT_DATE_AND_TIME )
		FormatTrxDateTime_WMask(szFSENDB, szHSENDB, 
                            dpsMsg.input.data.DATDEB.FSENDB, dpsMsg.input.data.DATDEB.HSENDB, 
                            is_true, is_false, is_true, is_true, 0, 0, 2 );
	COPYFIELD(szOPEDB1 , dpsMsg.input.data.DATDEB.OPEDB1 ); //  Identificación del operador #1. Primer autorizante del Banco de db. CHAR(2)
	COPYFIELD(szOPEDB2 , dpsMsg.input.data.DATDEB.OPEDB2 ); //  Identificación del operador #2. Segundo autorizante, CHAr(2).
	COPYFIELD(szRECHDB , dpsMsg.input.data.DATDEB.RECHDB ); //  Motivo del rechazo del Banco de débito -Ver tabla III - CHAR(4)
	COPYFIELD(szBANCRE , dpsMsg.input.data.DATCRE.BANCRE ); //  Código de Banco según BCRA, p/credito. CHAR(3)
	COPYFIELD(szSUCCRE , dpsMsg.input.data.DATCRE.SUCCRE ); //  Sucursal del Banco que tratará el crédito. CHAR(4)				
	COPYFIELD(szNOMBEN,  dpsMsg.input.data.DATCRE.NOMBEN ); //  Nombre de la cuenta a acreditar, VARCHAR(29)
	COPYFIELD(szTIPCRE , dpsMsg.input.data.DATCRE.TIPCRE ); //  Tipo de cuenta -Ver tabla II-, CHAR(2)
	COPYFIELD(szCTACRE, dpsMsg.input.data.DATCRE.CTACRE ); //  Número de cuenta -Formato Banco-, CHAR(17)
	COPYFIELD(szFSENCR, dpsMsg.input.data.DATCRE.FSENCR ); //  Fecha de envío de la transf.  al Banco	 -Aammdd-, CHAR(6)
	COPYFIELD(szHSENCR, dpsMsg.input.data.DATCRE.HSENCR ); //  Hora de envío -Hhmm-, CHAR(4)
    // Pre-formatear campo
	if( bFORMAT_DATE_AND_TIME )
		FormatTrxDateTime_WMask(szFSENCR, szHSENCR, 
                            dpsMsg.input.data.DATCRE.FSENCR, dpsMsg.input.data.DATCRE.HSENCR, 
                            is_true, is_false, is_true, is_true, 0, 0, 2 );
	COPYFIELD(szOPECR1, dpsMsg.input.data.DATCRE.OPECR1 ); //  Identificación del operador #1, primer autorizante del Banco de cr. CHAR(2)
	COPYFIELD(szOPECR2, dpsMsg.input.data.DATCRE.OPECR2 ); //  Identificación del operador #2, segundo autorizante, CHAR(2)
	COPYFIELD(szRECHCR, dpsMsg.input.data.DATCRE.RECHCR ); //  Motivo del rechazo del Banco de crédito -ver tabla III-, CHAR(4)
	COPYFIELD(szOPECON, dpsMsg.input.data.OPECON ); //  Operador que confeccionó la transferencia	, CHAR(2)
	COPYFIELD(szOPEAU1, dpsMsg.input.data.OPEAU1 );//  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	COPYFIELD(szOPEAU2, dpsMsg.input.data.OPEAU2 );//  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	COPYFIELD(szOPEAU3, dpsMsg.input.data.OPEAU3 );//  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	COPYFIELD(szFECAUT, dpsMsg.input.data.FECAUT );//  Fecha de autorización (envío del pago a la red) aammdd,  CHAR(6)
	COPYFIELD(szHORAUT, dpsMsg.input.data.HORAUT );//  Hora de autorización (envío del pago a la red) hhmm, CHAR(4)
    // Pre-formatear campo
	if( bFORMAT_DATE_AND_TIME )
		FormatTrxDateTime_WMask(szFECAUT, szHORAUT, 
                            dpsMsg.input.data.FECAUT, dpsMsg.input.data.HORAUT, 
                            is_true, is_false, is_true, is_true, 0, 0, 2 );
	COPYFIELD(szESTADO, dpsMsg.input.data.ESTADO );//  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	COPYFIELD(szFECEST, dpsMsg.input.data.FECEST );//  Fecha última modificación de ESTADO	, CHAR(6)
    // Pre-formatear campo
	if( bFORMAT_DATE_AND_TIME )
		FormatTrxDateTime_WMask(szFECEST, NULL, dpsMsg.input.data.FECEST, NULL, 
                            is_true, is_false, is_true, is_true, 0 , 0, 2 );
	COPYFIELD(szOBSER1, dpsMsg.input.data.OBSER1 );//  Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
	//////////////////////////////////////////////////////////////
	// Verificar NULL en caracteres 
    for(nIndex=0;nIndex<sizeof(dpsMsg.input.data.OBSER1);nIndex++)
	{
        if(0x00 == szOBSER1[nIndex]) szOBSER1[nIndex] = 0xff;    
		else if(0x27 == szOBSER1[nIndex]) szOBSER1[nIndex] = 0xff;    
	}
	//////////////////////////////////////////////////////////////
	COPYFIELD(szOBSER2, dpsMsg.input.data.OBSER2 );//  Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
	//////////////////////////////////////////////////////////////
	// Verificar NULL en caracteres 
    for(nIndex=0;nIndex<sizeof(dpsMsg.input.data.OBSER2);nIndex++)
        if(0x00 == szOBSER2[nIndex]) szOBSER2[nIndex] = 0xff;    
	//////////////////////////////////////////////////////////////
	COPYFIELD(szCODMAC, dpsMsg.input.data.CODMAC );//  Message Authentication Code (MAC), CHAR(12)
	//////////////////////////////////////////////////////////////
	// Verificar NULL en caracteres , y SINGLE-QUOTES tambien (codigo ASCII-39 , hexa : 27 )
    for(nIndex=0;nIndex<sizeof(dpsMsg.input.data.CODMAC);nIndex++)
	{
        if(0x00 == szCODMAC[nIndex]) szCODMAC[nIndex] = 0xff;
		else if(0x27 == szCODMAC[nIndex]) szCODMAC[nIndex] = 0xff;
	}
	COPYFIELD(szNUMREF, dpsMsg.input.data.NUMREF );//  Número de transferencia PC	 , //  nro.lote (N,3)+nro. operación (N,4) , CHAR(7) 
	COPYFIELD(szNUMENV, dpsMsg.input.data.NUMENV );//  Número de envío del abonadochar(3)
	chCONSOL[0] = dpsMsg.input.data.CONSOL[0];  //  Marca de consolidación de movimientos ("S" ó "N")
	chMARTIT[0] = dpsMsg.input.data.MARTIT[0];  //  "S"/"N"
	chPRIVEZ[0] = dpsMsg.input.data.PRIVEZ[0];  //  "S"/"N"
	chDIFCRE[0] = dpsMsg.input.data.DIFCRE[0];  //  "S"/"N"
	chRIEABO[0] = dpsMsg.input.data.RIEABO[0];  //  "S"/"N"
	chRIEBCO[0] = dpsMsg.input.data.RIEBCO[0];  //  "S"/"N"
    /////////////////////////////////////////////////////////////////////////////////////////
    COPYFIELD(szESTADOSANT, dpsMsg.input.data.TABEST );//  ESTADOS ANTERIORES
    /////////////////////////////////////////////////////////////////////////////////////////    
	COPYFIELD(szCUITDEB   , dpsMsg.input.data.CUITDEB ); // CUIT DEBITO
	COPYFIELD(szCUITCRE   , dpsMsg.input.data.CUITCRE ); // CUIT CREDITO
    COPYFIELD(szFILLER2   , dpsMsg.input.data.FILLER2 ); // FILLER FINAL
	/////////////////////////////////////////////////////////////////////////////////////////
	


    //////////////////////////////////////////////////////////////
	// Open appending for DEBUGGING
    if(bDEBUG_PRINT_PARAMS) hFile = fopen("debug.out", "a");
	if (hFile && bDEBUG_PRINT_PARAMS)
    {
        DPS_i   *ptr = &dpsMsg.input;
        fprintf(hFile,"\r\n------------------------------");
        fprintf(hFile,"\r\nPARAMETROS DPRE HACIA API-DB  ");
        fprintf(hFile,"\r\n------------------------------");
        fprintf(hFile,"\r\nFRNAME [%*.*s]",4,4,szFRNAME	),	//Nombre nodo emisor	ej."DNET" : Datanet
        fprintf(hFile,"\r\nTONAME [%*.*s]",4,4,szTONAME	),	//Nombre nodo receptor	ej."BSUD" : Bansud
	    fprintf(hFile,"\r\nREC [%*.*s]",3,3,szRECTYPE	),	//Tipo de registro	TDE = debito TCE = credito
	    fprintf(hFile,"\r\nACTIVITY [%c]",chACTIVITY[0] );	//Actividad	"N"	    
	    fprintf(hFile,"\r\nFILLER1 [%*.*s]",6,6,szFILLER1	),	//Reservado	Uso Datanet
	    fprintf(hFile,"\r\nBANDEB [%*.*s]",3,3,szBANDEB	),	//Codigo Banco segun BCRA	ej."067":BANSUD
	    fprintf(hFile,"\r\nFECSOL [%*.*s]",10,10,szFECSOL	),	//Fecha de compensacion	Del dia o prox. Habil -aaaammdd-
	    fprintf(hFile,"\r\nNUMTRA [%*.*s]",7,7,szNUMTRA	),	//Numero de transferencia	
	    fprintf(hFile,"\r\nNUMABO [%*.*s]",7,7,szNUMABO	),	//Abonado	
	    fprintf(hFile,"\r\nTIPOPE [%*.*s]",2,2,szTIPOPE	),	//Tipo de operacion	Ver tabla I
	    fprintf(hFile,"\r\nIMPORTE [%*.*s]",17,17,szIMPORTE );//Importe de la transferencia	15 enteros y 2 decimales
        fprintf(hFile,"\r\nSUCDEB [%*.*s]",4,4,szSUCDEB	),	//Sucursal del Banco que tratara el debito	No es necesariamente la de la cuenta
	    fprintf(hFile,"\r\nNOMSOL [%*.*s]",29,29,szNOMSOL	),	//Nombre de la cuenta	
	    fprintf(hFile,"\r\nTIPCUE [%*.*s]",2,2,szTIPCUEDEB	),	//Tipo de cuenta	Ver tabla II
	    fprintf(hFile,"\r\nNCUECM [%*.*s]",2,2,szNCUECMDEB	),	//Numero de cuenta corto	Interno Datanet
	    fprintf(hFile,"\r\nNUMCTA [%*.*s]",17,17,szNUMCTADEB	),	//Numero de cuenta	Formato Banco
	    fprintf(hFile,"\r\nFSENDB [%*.*s]",8,8,szFSENDB	),	//Fecha de envio de la transf.  al Banco	Aammdd
	    fprintf(hFile,"\r\nHSENDB [%*.*s]",17,17,szHSENDB	),	//Hora de envio	Hhmm
	    fprintf(hFile,"\r\nOPEDB1 [%*.*s]",2,2,szOPEDB1	),	//Identificacion del operador #1	Primer autorizante del Banco de db.
	    fprintf(hFile,"\r\nOPEDB2 [%*.*s]",2,2,szOPEDB2	),	//Identificacion del operador #2 	Segundo autorizante
	    fprintf(hFile,"\r\nRECHDB [%*.*s]",4,4,szRECHDB	);	//Motivo del rechazo del Banco de debito	Ver tabla III
        fprintf(hFile,"\r\nBANCRE [%*.*s]",3,3,szBANCRE	),	//Codigo de Banco segun BCRA	
	    fprintf(hFile,"\r\nSUCCRE [%*.*s]",4,4,szSUCCRE	),	//Sucursal del Banco que tratara el credito	No es necesariamente la de la cuenta
	    fprintf(hFile,"\r\nNOMBEN [%*.*s]",29,29,szNOMBEN	),	//Nombre de la cuenta a acreditar	
	    fprintf(hFile,"\r\nTIPCRE [%*.*s]",2,2,szTIPCRE	),	//Tipo de cuenta	Ver tabla II
	    fprintf(hFile,"\r\nCTACRE [%*.*s]",17,17,szCTACRE	),	//Numero de cuenta	Formato Banco
	    fprintf(hFile,"\r\nFSENCR [%*.*s]",8,8,szFSENCR	),	//Fecha de envio de la transf.  al Banco	Aammdd
	    fprintf(hFile,"\r\nHSENCR [%*.*s]",17,17,szHSENCR	),	//Hora de envio	Hhmm
	    fprintf(hFile,"\r\nOPECR1 [%*.*s]",2,2,szOPECR1	),	//Identificacion del operador #1	Primer autorizante del Banco de cr.
	    fprintf(hFile,"\r\nOPECR2 [%*.*s]",2,2,szOPECR2	),	//Identificacion del operador #2 	Segundo autorizante
	    fprintf(hFile,"\r\nRECHCR [%*.*s]",4,4,szRECHCR	);	//Motivo del rechazo del Banco de credito	Ver tabla III
        fprintf(hFile,"\r\nOPECON [%*.*s]",2,2,szOPECON	),	//Operador que confecciono la transferencia	
	    fprintf(hFile,"\r\nOPEAU1 [%*.*s]",2,2,szOPEAU1	),	//Id. Firmante #1 (primer autorizante de la Empresa	
	    fprintf(hFile,"\r\nOPEAU2 [%*.*s]",2,2,szOPEAU2	),	//Id. Firmante #2 (segundo autorizante de la Empresa	
	    fprintf(hFile,"\r\nOPEAU3 [%*.*s]",2,2,szOPEAU3	),	//Id. Firmante #3 (tercer autorizante de la Empresa	
	    fprintf(hFile,"\r\nFECAUT [%*.*s]",8,8,szFECAUT	),	//Fecha de autorizacion (envio del pago a la red)	Aammdd
	    fprintf(hFile,"\r\nHORAUT [%*.*s]",17,17,szHORAUT	),	//Hora de autorizacion (envio del pago a la red)	Hhmm
	    fprintf(hFile,"\r\nESTADO [%*.*s]",2,2,szESTADO	),	//Estado de la transferencia	Ver tabla IV
	    fprintf(hFile,"\r\nFECEST [%*.*s]",8,8,szFECEST	),	//Fecha ultima modificacion de ESTADO	
	    fprintf(hFile,"\r\nOBSER1 [%*.*s]",60,60,szOBSER1	),	//Texto libre ingresado por el abonado	Opcional
	    fprintf(hFile,"\r\nOBSER2 [%*.*s]",100,100,szOBSER2	),	//Datos adicionales ingresados por el abonado	Opcional, varia segun TIPOPE. Ver Tabla V
	    fprintf(hFile,"\r\nCODMAC [%*.*s]",12,12,szCODMAC	),	//Message Authentication Code (MAC)	Ver Anexo A
	    fprintf(hFile,"\r\nNUMREF [%*.*s]",7,7,szNUMREF	),	//Numero de transferencia PC	Nro. De lote (N,3) + nro. de operacion dentro del lote (N,4) 
	    fprintf(hFile,"\r\nNUMENV [%*.*s]",3,3,szNUMENV	),	//Numero de envio del abonado	
	    fprintf(hFile,"\r\nCONSOL [%c]",chCONSOL[0]),	//Marca de consolidacion de movimientos ("s]" o "N")	"s]" se genera un unico movimiento en operaciones de debito con igual fecha de solicitud y  nro. de lote
	    fprintf(hFile,"\r\nMARTIT [%c]",chMARTIT[0]),	//Indica que la cuenta de debito y la cuenta de credito son de la misma empresa	
	    fprintf(hFile,"\r\nPRIVEZ [%c]",chPRIVEZ[0]),	//"s]" indica que al momento en que se envia el registro al Banco de debito no existe un pago previo aceptado por los dos Bancos que involucre a las cuentas intervinientes."N" indica que los Bancos aceptaron previamente un pago en el que intervenian las mismas cuentas	
	    fprintf(hFile,"\r\nDIFCRE [%c]",chDIFCRE[0]),	//"s]" habilita al Banco de debito que asi lo desee a tratar la transferencia sin importar que sea diferida (prox. habil)."N" indica que el Banco de debito no puede tratar la transferencia diferida porque el Banco de credito no las puede manejar. 	
	    fprintf(hFile,"\r\nRIEABO [%c]",chRIEABO[0]),	//"s]" indica que el pago supera el riesgo fijado"N" el pago no supera el riesgo	
	    fprintf(hFile,"\r\nRIEBCO [%c]",chRIEBCO[0]);	//"s]" indica que al ser aceptado el debito la transferencia supero el riesgo establecido"N" la transferencia no supera el riesgo pero tiene tratamiento del credito obligatorio" " indica que la transferencia quedo ejecutada por el debito (no supero el riesgo ni fue tratado el credito)	
        fprintf(hFile,"\r\n----------------------");
    };
    //////////////////////////////////////////////////////////////
	if (hFile && bDEBUG_PRINT_PARAMS)
    {
        DPS_i   *ptr = &dpsMsg.input;
        fprintf(hFile,"\r\n----------------------");
        fprintf(hFile,"\r\nMENSAJE DPRE - DATANET");
        fprintf(hFile,"\r\n----------------------");
        fprintf(hFile,"\r\nTPNAME [%*.*s]",4,4,ptr->data.header.TPNAME	),	//Nombre nodo emisor	ej."DPRE" : Datanet
	    fprintf(hFile,"\r\nNDNAME [%*.*s]",4,4,ptr->data.header.NDNAME	);	//Nombre nodo receptor	ej."DNET" : 
        fprintf(hFile,"\r\nHORA [%*.*s]",4,4,ptr->data.header.HORA     ); // Hora del sistema COMP(7) SIGN
        fprintf(hFile,"\r\nSECUENCIA [%*.*s]",2,2,ptr->data.header.SECUENCIA); // Nro. Secuencia COMP(3) SIGN
        fprintf(hFile,"\r\nFRNAME [%*.*s]",4,4,ptr->data.FRNAME	),	//Nombre nodo emisor	ej."DNET" : Datanet
        fprintf(hFile,"\r\nTONAME [%*.*s]",4,4,ptr->data.TONAME	),	//Nombre nodo receptor	ej."BSUD" : Bansud
	    fprintf(hFile,"\r\nREC [%*.*s]",3,3,ptr->data.RECTYPE	),	//Tipo de registro	TDE = debito TCE = credito
	    fprintf(hFile,"\r\nACTIVITY [%c]",ptr->data.ACTIVITY[0] );	//Actividad	"N"
	    fprintf(hFile,"\r\nCOUNTER [%i]",ptr->data.COUNTER);		//Longitud del registro	580 (HEXA'0244'), en BINARIO, 2 bytes
	    fprintf(hFile,"\r\nFILLER1 [%*.*s]",6,6,ptr->data.FILLER1	),	//Reservado	Uso Datanet
	    fprintf(hFile,"\r\nBANDEB [%*.*s]",3,3,ptr->data.BANDEB	),	//Codigo Banco segun BCRA	ej."067":BANSUD
	    fprintf(hFile,"\r\nFECSOL [%*.*s]",8,8,ptr->data.FECSOL	),	//Fecha de compensacion	Del dia o prox. Habil -aaaammdd-
	    fprintf(hFile,"\r\nNUMTRA [%*.*s]",7,7,ptr->data.NUMTRA	),	//Numero de transferencia	
	    fprintf(hFile,"\r\nNUMABO [%*.*s]",7,7,ptr->data.NUMABO	),	//Abonado	
	    fprintf(hFile,"\r\nTIPOPE [%*.*s]",2,2,ptr->data.TIPOPE	),	//Tipo de operacion	Ver tabla I
	    fprintf(hFile,"\r\nIMPORTE [%*.*s]",17,17,ptr->data.IMPORTE );//Importe de la transferencia	15 enteros y 2 decimales
        fprintf(hFile,"\r\nSUCDEB [%*.*s]",4,4,ptr->data.DATDEB.SUCDEB	),	//Sucursal del Banco que tratara el debito	No es necesariamente la de la cuenta
	    fprintf(hFile,"\r\nNOMSOL [%*.*s]",29,29,ptr->data.DATDEB.NOMSOL	),	//Nombre de la cuenta	
	    fprintf(hFile,"\r\nTIPCUE [%*.*s]",2,2,ptr->data.DATDEB.TIPCUE	),	//Tipo de cuenta	Ver tabla II
	    fprintf(hFile,"\r\nNCUECM [%*.*s]",2,2,ptr->data.DATDEB.NCUECM	),	//Numero de cuenta corto	Interno Datanet
	    fprintf(hFile,"\r\nNUMCTA [%*.*s]",17,17,ptr->data.DATDEB.NUMCTA	),	//Numero de cuenta	Formato Banco
	    fprintf(hFile,"\r\nFSENDB [%*.*s]",6,6,ptr->data.DATDEB.FSENDB	),	//Fecha de envio de la transf.  al Banco	Aammdd
	    fprintf(hFile,"\r\nHSENDB [%*.*s]",4,4,ptr->data.DATDEB.HSENDB	),	//Hora de envio	Hhmm
	    fprintf(hFile,"\r\nOPEDB1 [%*.*s]",2,2,ptr->data.DATDEB.OPEDB1	),	//Identificacion del operador #1	Primer autorizante del Banco de db.
	    fprintf(hFile,"\r\nOPEDB2 [%*.*s]",2,2,ptr->data.DATDEB.OPEDB2	),	//Identificacion del operador #2 	Segundo autorizante
	    fprintf(hFile,"\r\nRECHDB [%*.*s]",4,4,ptr->data.DATDEB.RECHDB	);	//Motivo del rechazo del Banco de debito	Ver tabla III
        fprintf(hFile,"\r\nBANCRE [%*.*s]",3,3,ptr->data.DATCRE.BANCRE	),	//Codigo de Banco segun BCRA	
	    fprintf(hFile,"\r\nSUCCRE [%*.*s]",4,4,ptr->data.DATCRE.SUCCRE	),	//Sucursal del Banco que tratara el credito	No es necesariamente la de la cuenta
	    fprintf(hFile,"\r\nNOMBEN [%*.*s]",29,29,ptr->data.DATCRE.NOMBEN	),	//Nombre de la cuenta a acreditar	
	    fprintf(hFile,"\r\nTIPCRE [%*.*s]",2,2,ptr->data.DATCRE.TIPCRE	),	//Tipo de cuenta	Ver tabla II
	    fprintf(hFile,"\r\nCTACRE [%*.*s]",17,17,ptr->data.DATCRE.CTACRE	),	//Numero de cuenta	Formato Banco
	    fprintf(hFile,"\r\nFSENCR [%*.*s]",6,6,ptr->data.DATCRE.FSENCR	),	//Fecha de envio de la transf.  al Banco	Aammdd
	    fprintf(hFile,"\r\nHSENCR [%*.*s]",4,4,ptr->data.DATCRE.HSENCR	),	//Hora de envio	Hhmm
	    fprintf(hFile,"\r\nOPECR1 [%*.*s]",2,2,ptr->data.DATCRE.OPECR1	),	//Identificacion del operador #1	Primer autorizante del Banco de cr.
	    fprintf(hFile,"\r\nOPECR2 [%*.*s]",2,2,ptr->data.DATCRE.OPECR2	),	//Identificacion del operador #2 	Segundo autorizante
	    fprintf(hFile,"\r\nRECHCR [%*.*s]",4,4,ptr->data.DATCRE.RECHCR	);	//Motivo del rechazo del Banco de credito	Ver tabla III
        fprintf(hFile,"\r\nOPECON [%*.*s]",2,2,ptr->data.OPECON	),	//Operador que confecciono la transferencia	
	    fprintf(hFile,"\r\nOPEAU1 [%*.*s]",2,2,ptr->data.OPEAU1	),	//Id. Firmante #1 (primer autorizante de la Empresa	
	    fprintf(hFile,"\r\nOPEAU2 [%*.*s]",2,2,ptr->data.OPEAU2	),	//Id. Firmante #2 (segundo autorizante de la Empresa	
	    fprintf(hFile,"\r\nOPEAU3 [%*.*s]",2,2,ptr->data.OPEAU3	),	//Id. Firmante #3 (tercer autorizante de la Empresa	
	    fprintf(hFile,"\r\nFECAUT [%*.*s]",6,6,ptr->data.FECAUT	),	//Fecha de autorizacion (envio del pago a la red)	Aammdd
	    fprintf(hFile,"\r\nHORAUT [%*.*s]",4,4,ptr->data.HORAUT	),	//Hora de autorizacion (envio del pago a la red)	Hhmm
	    fprintf(hFile,"\r\nESTADO [%*.*s]",2,2,ptr->data.ESTADO	),	//Estado de la transferencia	Ver tabla IV
	    fprintf(hFile,"\r\nFECEST [%*.*s]",6,6,ptr->data.FECEST	),	//Fecha ultima modificacion de ESTADO	
	    fprintf(hFile,"\r\nOBSER1 [%*.*s]",60,60,ptr->data.OBSER1	),	//Texto libre ingresado por el abonado	Opcional
	    fprintf(hFile,"\r\nOBSER2 [%*.*s]",100,100,ptr->data.OBSER2	),	//Datos adicionales ingresados por el abonado	Opcional, varia segun TIPOPE. Ver Tabla V
	    fprintf(hFile,"\r\nCODMAC [%*.*s]",12,12,ptr->data.CODMAC	),	//Message Authentication Code (MAC)	Ver Anexo A
	    fprintf(hFile,"\r\nNUMREF [%*.*s]",7,7,ptr->data.NUMREF	),	//Numero de transferencia PC	Nro. De lote (N,3) + nro. de operacion dentro del lote (N,4) 
	    fprintf(hFile,"\r\nNUMENV [%*.*s]",3,3,ptr->data.NUMENV	),	//Numero de envio del abonado	
	    fprintf(hFile,"\r\nCONSOL [%c]",ptr->data.CONSOL[0]),	//Marca de consolidacion de movimientos ("s]" o "N")	"s]" se genera un unico movimiento en operaciones de debito con igual fecha de solicitud y  nro. de lote
	    fprintf(hFile,"\r\nMARTIT [%c]",ptr->data.MARTIT[0]),	//Indica que la cuenta de debito y la cuenta de credito son de la misma empresa	
	    fprintf(hFile,"\r\nPRIVEZ [%c]",ptr->data.PRIVEZ[0]),	//"s]" indica que al momento en que se envia el registro al Banco de debito no existe un pago previo aceptado por los dos Bancos que involucre a las cuentas intervinientes."N" indica que los Bancos aceptaron previamente un pago en el que intervenian las mismas cuentas	
	    fprintf(hFile,"\r\nDIFCRE [%c]",ptr->data.DIFCRE[0]),	//"s]" habilita al Banco de debito que asi lo desee a tratar la transferencia sin importar que sea diferida (prox. habil)."N" indica que el Banco de debito no puede tratar la transferencia diferida porque el Banco de credito no las puede manejar. 	
	    fprintf(hFile,"\r\nRIEABO [%c]",ptr->data.RIEABO[0]),	//"s]" indica que el pago supera el riesgo fijado"N" el pago no supera el riesgo	
	    fprintf(hFile,"\r\nRIEBCO [%c]",ptr->data.RIEBCO[0]);	//"s]" indica que al ser aceptado el debito la transferencia supero el riesgo establecido"N" la transferencia no supera el riesgo pero tiene tratamiento del credito obligatorio" " indica que la transferencia quedo ejecutada por el debito (no supero el riesgo ni fue tratado el credito)	
        fprintf(hFile,"\r\nCODEST-1-2-3 [%2.2s,%2.2s,%2.2s]",
            ptr->data.TABEST[0].CODEST, ptr->data.TABEST[1].CODEST, ptr->data.TABEST[2].CODEST );
	    fprintf(hFile,"\r\nFECEST-1-2-3 [%6.6s,%6.6s,%6.6s]",
            ptr->data.TABEST[0].FECEST,ptr->data.TABEST[1].FECEST,ptr->data.TABEST[2].FECEST	), //Fecha de estado	
	    fprintf(hFile,"\r\nHOREST-1-2-3 [%6.6s,%6.6s,%6.6s]",
        ptr->data.TABEST[0].HOREST, ptr->data.TABEST[1].HOREST, ptr->data.TABEST[2].HOREST	);	//Hora del estado	
        fprintf(hFile,"\r\n----------------------");
        fflush(hFile);        
    };
    //////////////////////////////////////////////////////////////

	// Loggear la transaccion en TEF ONLINE
	ptrBackOffice->LoggTransaction(  
		szFRNAME ,// Nombre nodo emisor, ej. "DNET", char(4)
		szTONAME , //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
		szRECTYPE ,// Tipo de registro, TDE = débito TCE = crédito, CHAR(3)
		chACTIVITY,  //  Actividad, CHAR(1)="N"
        szFILLER1 , // RESERVADO="150"
		szBANDEB , //  Código Banco según BCRA	ej."067" Bansud, CHAR(3)
		szFECSOL , //  Fecha de compensación, de día o prox. Hábil -aaaammdd-, CHAR(8)
		szNUMTRA , //  Número de transferencia,CHAR(7) 
		szNUMABO , //  Abonado	, CHAR(7)
		szTIPOPE , //  Tipo de operación -	Ver tabla I - , CHAR(2)
		szIMPORTE  ,//  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
		szSUCDEB ,  //  Sucursal del Banco que tratará el débito, CHAR(4)
		szNOMSOL ,  //  Nombre de la cuenta, 
		szTIPCUEDEB,//   Tipo de cuenta	- Ver tabla II  - CHAR(2)
		szNCUECMDEB , //  Número de cuenta corto -Interno Datanet- CHAR(2)
		szNUMCTADEB , //  Número de cuenta, formato Banco, CHAR(17)
		szFSENDB , //  Fecha de envío de la transf.  al Banco -Aammdd-, CHAR(6)
		szHSENDB , // Hora de envío, Hhmm, CHAR(4)
		szOPEDB1 , //  Identificación del operador #1. Primer autorizante del Banco de db. CHAR(2)
		szOPEDB2 , //  Identificación del operador #2. Segundo autorizante, CHAr(2).
		szRECHDB , //  Motivo del rechazo del Banco de débito -Ver tabla III - CHAR(4)
		szBANCRE , //  Código de Banco según BCRA, p/credito. CHAR(3)
		szSUCCRE , //  Sucursal del Banco que tratará el crédito. CHAR(4)				
		szNOMBEN,  //  Nombre de la cuenta a acreditar, VARCHAR(29)
		szTIPCRE , //  Tipo de cuenta -Ver tabla II-, CHAR(2)
		szCTACRE, //  Número de cuenta -Formato Banco-, CHAR(17)
		szFSENCR, //  Fecha de envío de la transf.  al Banco	 -Aammdd-, CHAR(6)
		szHSENCR, //  Hora de envío -Hhmm-, CHAR(4)
		szOPECR1, //  Identificación del operador #1, primer autorizante del Banco de cr. CHAR(2)
		szOPECR2, //  Identificación del operador #2, segundo autorizante, CHAR(2)
		szRECHCR, //  Motivo del rechazo del Banco de crédito -ver tabla III-, CHAR(4)
		szOPECON , //  Operador que confeccionó la transferencia	, CHAR(2)
		szOPEAU1 , //  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
		szOPEAU2 , //  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
		szOPEAU3 , //  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
		szFECAUT , //  Fecha de autorización (envío del pago a la red) aammdd,  CHAR(6)
		szHORAUT , //  Hora de autorización (envío del pago a la red) hhmm, CHAR(4)
		szESTADO , //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
		szFECEST , //  Fecha última modificación de ESTADO	, CHAR(6)
		szOBSER1 , //  Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
		szOBSER2 , //  Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
		szCODMAC , //  Message Authentication Code (MAC), CHAR(12)
		szNUMREF , //  Número de transferencia PC	 , //  nro.lote (N,3)+nro. operación (N,4) , CHAR(7) 
		szNUMENV , //  Número de envío del abonadochar(3)
		chCONSOL,  //  Marca de consolidación de movimientos ("S" ó "N")
		chMARTIT,  //  "S"/"N"
		chPRIVEZ,  //  "S"/"N"
		chDIFCRE,  //  "S"/"N"
		chRIEABO,  //  "S"/"N"
		chRIEBCO,  //  "S"/"N"
		szESTADOTRAT, // ESTADO TRATAMIENTO
        szESTADOSANT, //  ESTADOS ANTERIORES (arreglo de 10 posiciones)
		/***************************************************************/
		szCUITDEB,	// CUIT debito
		szCUITCRE,	// CUIT credito
		szFILLER2	// FILLER final
		/***************************************************************/
		);
	if(ptrBackOffice->GetStatus() != 0)
    {
        // Actualizar resultado interno del autorizador
        shAuthRespCde = ptrBackOffice->GetStatus();
		wLRespCde = DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED;
		/////////////////////////////////////////////////////////////
		if (hFile && bDEBUG_PRINT_PARAMS)
		{
			fprintf(hFile,"\r\n%s %s\r\n", ptrBackOffice->GetErrorMessage(NULL) , ptrBackOffice->GetLastSQLCommand(NULL) );			
	        fprintf(hFile,"\r\n----------------------");
		    fflush(hFile);        
		}
		/////////////////////////////////////////////////////////////
    }
	
	///////////////////////////////////
	if (hFile && bDEBUG_PRINT_PARAMS)
	{
		fclose(hFile);        
		hFile = NULL;
	}
	///////////////////////////////////

    // Si se informo un estado pendiente de envio ...
    // y/o un estado tratamiento...
    if(NULL != szEXTPEDENVIO)
    {
        // Actualizar la transaccion en la tabla TEF ONLINE
        WORD wRespCde = ptrBackOffice->UpdateTransaction(
	        szBANDEB , //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
	        szFECSOL , //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	        szNUMTRA , //  Numero de transferencia,CHAR(7) 
	        szNUMABO , //  Abonado	, CHAR(7)
	        szNUMCTADEB, //  Numero de cuenta, formato Banco, CHAR(17)
	        szBANCRE , //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
	        szCTACRE , //  Numero de cuenta -Formato Banco-, CHAR(17)
	        szESTADO , //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	        szNUMREF , //  Numero de transferencia PC	 []
	        szNUMENV , //  Numero de envio del abonadochar(3)
            szEXTPEDENVIO,// Pendiente de ENVIO se modifica y pasa a ENVIADO, "N"
            szRECHDB,     // Motivos de rechazo DB
            szRECHCR,     // Motivos de rechazo CR
            szESTADOTRAT );
	    if(ptrBackOffice->GetStatus() != 0)
        {
            // Actualizar resultado interno del autorizador
            shAuthRespCde = ptrBackOffice->GetStatus();
		    wLRespCde = DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED; 
        };
    }
	
	// OK
	return DPS_APROBADA_SIN_RECHAZO;
}

///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// Formatear mensaje de entrada desde los parametros
///////////////////////////////////////////////////////////////////////////////////
void TrxResBTF::FormatInputDataFromParams( 
	            char * szFRNAME ,	char * szTONAME ,
	            char * szRECTYPE,   char * chACTIVITY,
                char * szFILLER1,	char * szBANDEB ,
	            char * szFECSOL ,   char * szNUMTRA ,
	            char * szNUMABO ,   char * szTIPOPE ,
	            char * szIMPORTE,   char * szSUCDEB ,
	            char * szNOMSOL ,   char * szTIPCUEDEB,
	            char * szNCUECMDEB ,char * szNUMCTADEB ,
	            char * szFSENDB ,   char * szHSENDB ,
	            char * szOPEDB1 ,   char * szOPEDB2 ,
	            char * szRECHDB ,   char * szBANCRE ,
	            char * szSUCCRE ,   char * szNOMBEN,
	            char * szTIPCRE ,   char * szCTACRE,
	            char * szFSENCR,    char * szHSENCR,
	            char * szOPECR1,    char * szOPECR2,
	            char * szRECHCR,    char * szOPECON,
	            char * szOPEAU1,    char * szOPEAU2,
	            char * szOPEAU3,    char * szFECAUT,
	            char * szHORAUT,    char * szESTADO,
	            char * szFECEST,    char * szOBSER1,
	            char * szOBSER2,    char * szCODMAC,
	            char * szNUMREF,    char * szNUMENV,
                char * chCONSOL,    char * chMARTIT,
                char * chPRIVEZ,    char * chDIFCRE,
                char * chRIEABO,    char * chRIEBCO )
{
    short       nIndex = 0; // Indice de SCAN de contenido

    // Formateo de estructura de entrada desde campos de base de datos
	EXACTCOPYFIELD( dpsMsg.input.data.FRNAME , szFRNAME ); // Nombre nodo emisor, ej. "DNET", char(4)
	EXACTCOPYFIELD( dpsMsg.input.data.TONAME , szTONAME ); //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
	EXACTCOPYFIELD( dpsMsg.input.data.RECTYPE, szRECTYPE ); // Tipo de registro, TDE = débito TCE = crédito, CHAR(3)
	dpsMsg.input.data.ACTIVITY[0] = chACTIVITY[0] ;  //  Actividad, CHAR(1)="N"
    EXACTCOPYFIELD(dpsMsg.input.data.FILLER1 , szFILLER1 ); //  RESERVADO, CHAR(6)
	PADCOPYINTEGER(dpsMsg.input.data.BANDEB , szBANDEB ); //  Código Banco según BCRA	ej."067" Bansud, CHAR(3)
	EXACTCOPYFIELD(dpsMsg.input.data.FECSOL , szFECSOL ); //  Fecha de compensación, de día o prox. Hábil -aaaammdd-, CHAR(8)
    // Pre-formatear campo
    FormatTrxDateTime_WMask(dpsMsg.input.data.FECSOL, NULL, szFECSOL, NULL, 
                            is_false, is_true, is_false, is_true, 0, 0 );
	PADCOPYINTEGER(dpsMsg.input.data.NUMTRA , szNUMTRA ); //  Número de transferencia,CHAR(7) 
	EXACTCOPYFIELD(dpsMsg.input.data.NUMABO , szNUMABO ); //  Abonado	, CHAR(7)
	PADCOPYINTEGER(dpsMsg.input.data.TIPOPE , szTIPOPE ); //  Tipo de operación -	Ver tabla I - , CHAR(2)
	EXACTCOPYFIELD(dpsMsg.input.data.IMPORTE, szIMPORTE); //  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
    Format_ISO_Ammount( szIMPORTE, 0, dpsMsg.input.data.IMPORTE, sizeof(dpsMsg.input.data.IMPORTE), 
        is_false, NULL ); 
	PADCOPYINTEGER(dpsMsg.input.data.DATDEB.SUCDEB, szSUCDEB ); //  Sucursal del Banco que tratará el débito, CHAR(4)
	EXACTCOPYFIELD(dpsMsg.input.data.DATDEB.NOMSOL, szNOMSOL ); //  Nombre de la cuenta, 
	PADCOPYINTEGER(dpsMsg.input.data.DATDEB.TIPCUE, szTIPCUEDEB); //   Tipo de cuenta	- Ver tabla II  - CHAR(2)
	PADCOPYINTEGER(dpsMsg.input.data.DATDEB.NCUECM, szNCUECMDEB ); //  Número de cuenta corto -Interno Datanet- CHAR(2)
	EXACTCOPYFIELD(dpsMsg.input.data.DATDEB.NUMCTA, szNUMCTADEB); //  Número de cuenta, formato Banco, CHAR(17)
	EXACTCOPYFIELD(dpsMsg.input.data.DATDEB.FSENDB, szFSENDB ); //  Fecha de envío de la transf.  al Banco -Aammdd-, CHAR(6)
	EXACTCOPYFIELD(dpsMsg.input.data.DATDEB.HSENDB, szHSENDB ); // Hora de envío, Hhmm, CHAR(4)
    // Pre-formatear campo
    FormatTrxDateTime_WMask(dpsMsg.input.data.DATDEB.FSENDB, 
                            dpsMsg.input.data.DATDEB.HSENDB,
                            szFSENDB, szHSENDB, 
                            is_false, is_false, is_false, is_true, 0, 0 );
	PADCOPYINTEGER(dpsMsg.input.data.DATDEB.OPEDB1, szOPEDB1  ); //  Identificación del operador #1. Primer autorizante del Banco de db. CHAR(2)
	PADCOPYINTEGER(dpsMsg.input.data.DATDEB.OPEDB2, szOPEDB2 ); //  Identificación del operador #2. Segundo autorizante, CHAr(2).
	PADCOPYINTEGER(dpsMsg.input.data.DATDEB.RECHDB, szRECHDB ); //  Motivo del rechazo del Banco de débito -Ver tabla III - CHAR(4)
	PADCOPYINTEGER(dpsMsg.input.data.DATCRE.BANCRE, szBANCRE ); //  Código de Banco según BCRA, p/credito. CHAR(3)
	PADCOPYINTEGER(dpsMsg.input.data.DATCRE.SUCCRE, szSUCCRE ); //  Sucursal del Banco que tratará el crédito. CHAR(4)				
	EXACTCOPYFIELD(dpsMsg.input.data.DATCRE.NOMBEN, szNOMBEN ); //  Nombre de la cuenta a acreditar, VARCHAR(29)
	PADCOPYINTEGER(dpsMsg.input.data.DATCRE.TIPCRE, szTIPCRE ); //  Tipo de cuenta -Ver tabla II-, CHAR(2)
	EXACTCOPYFIELD(dpsMsg.input.data.DATCRE.CTACRE, szCTACRE ); //  Número de cuenta -Formato Banco-, CHAR(17)
	EXACTCOPYFIELD(dpsMsg.input.data.DATCRE.FSENCR, szFSENCR ); //  Fecha de envío de la transf.  al Banco	 -Aammdd-, CHAR(6)
	EXACTCOPYFIELD(dpsMsg.input.data.DATCRE.HSENCR, szHSENCR ); //  Hora de envío -Hhmm-, CHAR(4)
    // Pre-formatear campo
    FormatTrxDateTime_WMask(dpsMsg.input.data.DATCRE.FSENCR, 
                            dpsMsg.input.data.DATCRE.HSENCR, 
                            szFSENCR, szHSENCR, 
                            is_false, is_false, is_false, is_true, 0, 0 );
	PADCOPYINTEGER(dpsMsg.input.data.DATCRE.OPECR1, szOPECR1 ); //  Identificación del operador #1, primer autorizante del Banco de cr. CHAR(2)
	PADCOPYINTEGER(dpsMsg.input.data.DATCRE.OPECR2, szOPECR2 ); //  Identificación del operador #2, segundo autorizante, CHAR(2)
	PADCOPYINTEGER(dpsMsg.input.data.DATCRE.RECHCR, szRECHCR ); //  Motivo del rechazo del Banco de crédito -ver tabla III-, CHAR(4)
	PADCOPYINTEGER(dpsMsg.input.data.OPECON, szOPECON ); //  Operador que confeccionó la transferencia	, CHAR(2)
	PADCOPYINTEGER(dpsMsg.input.data.OPEAU1, szOPEAU1 );//  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	PADCOPYINTEGER(dpsMsg.input.data.OPEAU2, szOPEAU2 );//  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	PADCOPYINTEGER(dpsMsg.input.data.OPEAU3, szOPEAU3 );//  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	EXACTCOPYFIELD(dpsMsg.input.data.FECAUT, szFECAUT );//  Fecha de autorización (envío del pago a la red) aammdd,  CHAR(6)
	EXACTCOPYFIELD(dpsMsg.input.data.HORAUT, szHORAUT );//  Hora de autorización (envío del pago a la red) hhmm, CHAR(4)
    // Pre-formatear campo
    FormatTrxDateTime_WMask(dpsMsg.input.data.FECAUT, 
                            dpsMsg.input.data.HORAUT, szFECAUT, szHORAUT,                             
                            is_false, is_false, is_false, is_true, 0, 0 );
	EXACTCOPYFIELD(dpsMsg.input.data.ESTADO, szESTADO );//  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	EXACTCOPYFIELD(dpsMsg.input.data.FECEST, szFECEST );//  Fecha última modificación de ESTADO	, CHAR(6)
    // Pre-formatear campo
    FormatTrxDateTime_WMask(dpsMsg.input.data.FECEST, NULL, szFECEST, NULL, 
                            is_false, is_false, is_false, is_true, 0, 0 );

	EXACTCOPYFIELD(dpsMsg.input.data.OBSER1, szOBSER1 );//  Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
	strrepchar(dpsMsg.input.data.OBSER1, sizeof(dpsMsg.input.data.OBSER1), 0xfe, 0x00);
	strrepchar(dpsMsg.input.data.OBSER1, sizeof(dpsMsg.input.data.OBSER1), 0xff, 0x27);

	//////////////////////////////////////////////////////////////
	EXACTCOPYFIELD(dpsMsg.input.data.OBSER2, szOBSER2 );//  Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
	strrepchar(dpsMsg.input.data.OBSER2, sizeof(dpsMsg.input.data.OBSER2), 0xfe, 0x00);
	strrepchar(dpsMsg.input.data.OBSER2, sizeof(dpsMsg.input.data.OBSER2), 0xff, 0x27);

	//////////////////////////////////////////////////////////////
	EXACTCOPYFIELD(dpsMsg.input.data.CODMAC, szCODMAC );//  Message Authentication Code (MAC), CHAR(12)
    strrepchar(dpsMsg.input.data.CODMAC, sizeof(dpsMsg.input.data.CODMAC), 0xfe, 0x00);    
	strrepchar(dpsMsg.input.data.CODMAC, sizeof(dpsMsg.input.data.CODMAC), 0xff, 0x27);    
	
	PADCOPYINTEGER(dpsMsg.input.data.NUMREF, szNUMREF );//  Número de transferencia PC	 , //  nro.lote (N,3)+nro. operación (N,4) , CHAR(7) 
	PADCOPYINTEGER(dpsMsg.input.data.NUMENV, szNUMENV );//  Número de envío del abonadochar(3)
	dpsMsg.input.data.CONSOL[0] = chCONSOL[0];  //  Marca de consolidación de movimientos ("S" ó "N")
	dpsMsg.input.data.MARTIT[0] = chMARTIT[0];  //  "S"/"N"
	dpsMsg.input.data.PRIVEZ[0] = chPRIVEZ[0];  //  "S"/"N"
	dpsMsg.input.data.DIFCRE[0] = chDIFCRE[0];  //  "S"/"N"
	dpsMsg.input.data.RIEABO[0] = chRIEABO[0];  //  "S"/"N"
	dpsMsg.input.data.RIEBCO[0] = chRIEBCO[0];  //  "S"/"N"
};

///////////////////////////////////////////////////////////////////////////////////
// Recuperar la primer transaccion disponible en el sistema autorizador del banco
///////////////////////////////////////////////////////////////////////////////////
WORD TrxResBTF::RetrieveTransaction( char *szTStatus )
{
	// Campos locales que mapean campos de estructruras DPS
	char szFRNAME [4+1] = {"DNET"};// Nombre nodo emisor, ej. "DNET", char(4)
	char szTONAME [4+1] = {"BSUQ"}; //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
	char szRECTYPE [3+1] = {"TDE"};// Tipo de registro, TDE = débito TCE = crédito, CHAR(3)
    char chACTIVITY[2] = {"N"};  //  Actividad, CHAR(1)="N"
    char szFILLER1[6+1] = {"000000"}; // RESERVADO
	char szBANDEB [3+1] = {"000"}; //  Código Banco según BCRA	ej."067" Bansud, CHAR(3)
	char szFECSOL [8+3+1] = {_DEFAULT_DATE_STR_}; //  Fecha de compensación, de día o prox. Hábil -aaaammdd-, CHAR(8)
	char szNUMTRA [7+1] = {"0000000"}; //  Número de transferencia,CHAR(7) 
	char szNUMABO [7+1] = {"0000000"}; //  Abonado	, CHAR(7)
	char szTIPOPE [2+1] = {"00"}; //  Tipo de operación -	Ver tabla I - , CHAR(2)
	/* VISUAL .NET 2003 STACK CORRUPTED */
	char szIMPORTE[17+4+1] = {"00000000000000.00"};//  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	/* VISUAL .NET 2003 STACK CORRUPTED */
	char szSUCDEB [4+1] = {"0000"};  //  Sucursal del Banco que tratará el débito, CHAR(4)
	char szNOMSOL [30+1] = {"                             "};  //  Nombre de la cuenta, 
	char szTIPCUEDEB[2+1] = {"00"};//   Tipo de cuenta	- Ver tabla II  - CHAR(2)
	char szNCUECMDEB [2+1] = {"00"}; //  Número de cuenta corto -Interno Datanet- CHAR(2)
	char szNUMCTADEB [17+1] = {"00000000000000000"}; //  Número de cuenta, formato Banco, CHAR(17)
	char szFSENDB [8+3+1+6+1+1+32] = {_DEFAULT_DATE_TIME_STR_}; //  Fecha de envío de la transf.  al Banco -Aammdd-, CHAR(6)
	char szHSENDB [8+3+1+6+1+1+32] = {_DEFAULT_DATE_TIME_STR_}; // Hora de envío, Hhmm, CHAR(4)
	char szOPEDB1 [2+1] = {"00"}; //  Identificación del operador #1. Primer autorizante del Banco de db. CHAR(2)
	char szOPEDB2 [2+1] = {"00"}; //  Identificación del operador #2. Segundo autorizante, CHAr(2).
	char szRECHDB [4+1] = {"0000"}; //  Motivo del rechazo del Banco de débito -Ver tabla III - CHAR(4)
	char szBANCRE [3+1] = {"000"}; //  Código de Banco según BCRA, p/credito. CHAR(3)
	char szSUCCRE [4+1] = {"0000"}; //  Sucursal del Banco que tratará el crédito. CHAR(4)				
	char szNOMBEN[30+1] = {"                              "};  //  Nombre de la cuenta a acreditar, VARCHAR(29)
	char szTIPCRE [2+1] = {"00"}; //  Tipo de cuenta -Ver tabla II-, CHAR(2)
	char szCTACRE[17+1] = {"00000000000000000"}; //  Número de cuenta -Formato Banco-, CHAR(17)
	char szFSENCR[8+3+1+6+1+1+32] = {_DEFAULT_DATE_TIME_STR_}; //  Fecha de envío de la transf.  al Banco	 -Aammdd-, CHAR(6)
	char szHSENCR[8+3+1+6+1+1+32] = {_DEFAULT_DATE_TIME_STR_}; //  Hora de envío -Hhmm-, CHAR(4)
	char szOPECR1[2+1] = {"00"}; //  Identificación del operador #1, primer autorizante del Banco de cr. CHAR(2)
	char szOPECR2[2+1] = {"00"}; //  Identificación del operador #2, segundo autorizante, CHAR(2)
	char szRECHCR[4+1] = {"0000"}; //  Motivo del rechazo del Banco de crédito -ver tabla III-, CHAR(4)
	char szOPECON [2+1] = {"00"}; //  Operador que confeccionó la transferencia	, CHAR(2)
	char szOPEAU1 [2+1] = {"00"}; //  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	char szOPEAU2 [2+1] = {"00"}; //  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	char szOPEAU3 [2+1] = {"00"}; //  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	char szFECAUT [8+3+1+6+1+1+32] = {_DEFAULT_DATE_TIME_STR_}; //  Fecha de autorización (envío del pago a la red) aammdd,  CHAR(6)
	char szHORAUT [8+3+1+6+1+1+32] = {_DEFAULT_DATE_TIME_STR_}; //  Hora de autorización (envío del pago a la red) hhmm, CHAR(4)
	char szESTADO [2+1] = {"00"}; //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	char szFECEST [8+3+1] = {_DEFAULT_DATE_STR_}; //  Fecha última modificación de ESTADO	, CHAR(6)
    /* RULER                  ....:....1....:....2....:....3....:....4 */
	char szOBSER1 [60+1]  = {"                                        " //  Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
                             "                    " };
    /* RULER                  ....:....1....:....2....:....3....:....4 */
	char szOBSER2 [100+1] = {"                                        " //  Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
                             "                                        "
                             "                    " };
	char szCODMAC [DNET_MAC_LENGTH+1] = {"            "}; //  Message Authentication Code (MAC), CHAR(12)
	char szNUMREF [7+1] = {"0000000"}; //  Número de transferencia PC	 [+1] = {" "}; //  nro.lote (N,3)+nro. operación (N,4) , CHAR(7) 
	char szNUMENV [3+1] = {"000"}; //  Número de envío del abonadochar(3)
    char chCONSOL[2] = {"N"}; //  Marca de consolidación de movimientos ("S" ó "N")	"S" se genera un 
		// único movimiento en operaciones de débito con igual fecha de solicitud y  nro. de 
		// lote, CHAR(1)
    char chMARTIT[2] = {"N"}; //  Indica que la cuenta de débito y la cuenta de crédito son de la misma
		// empresa, CHAR(1)
    char chPRIVEZ[2] = {"N"}; //  "S" indica que al momento en que se envía el registro al Banco de 
		//débito no existe un pago previo aceptado por los dos Bancos que involucre a las 
		//cuentas intervinientes."N" indica que los Bancos aceptaron previamente un pago en el
		//que intervenían las mismas cuentas	, CHAR(1)
    char chDIFCRE[2] = {"N"}; //  "S" habilita al Banco de débito que así lo desee a tratar la 
		//transferencia sin importar que sea diferida (prox. hábil)."N" indica que el Banco de
		//débito no puede tratar la transferencia diferida porque el Banco de crédito no las 
		//puede manejar. char chAR(1)
    char chRIEABO[2] = {"N"}; //  "S" indica que el pago supera el riesgo fijado"N" el pago no supera 
		//el riesgochar chAR(1)
    char chRIEBCO[2] = {"N"}; //  "S" indica que al ser aceptado el débito la transferencia superó el 
		//riesgo establecido"N" la transferencia no supera el riesgo pero tiene tratamiento 
		//del crédito obligatorio" " indica que la transferencia quedó ejecutada por el 
		//débito (no superó el riesgo ni fue tratado el crédito),CHAR(1)							
    // Codigo de respuesta retornado
    WORD wRespCde = DPS_APROBADA_SIN_RECHAZO,
         wRetVal  = DPS_APROBADA_SIN_RECHAZO;
    // Estado Tratamiento por OMISION es PENDIENTE
    char szEstadoTratamiento[] = {_DPS_TRAT_PENDIENTE_};
    char szMAC2 [DNET_MAC_LENGTH+1] = {"            "}, //  Message Authentication Code (MAC), CHAR(12)
         szMAC3 [DNET_MAC_LENGTH+1] = {"            "}; //  Message Authentication Code (MAC), CHAR(12)
    // Codigo de retorno para comparar fechas
    e_settlement_retcode esRcode       = ESTL_TRANSFERENCIA_OK, // FEC-SOLICITUD vs hoy
                         esRcodeStatus = ESTL_TRANSFERENCIA_OK; // FEC-SOLICITUD vs FEC-ESTADO
    /******************************************/
    char szACCION[10]    = {"0"}; // Accion
    /******************************************/


	// Precondicion : Existe conexion a base de datos
	if( NULL == ptrBackOffice )
		return DPS_FUERZA_MAYOR;
    // Precondicion : Estado tratamiento , si no es informado, se asume el DEFAULT
    if(szTStatus)
       szEstadoTratamiento[0] = ( *szTStatus ) ;

    /////////////////////////////////////////////////////////////////////////
    // Para la recuperacion de la tabla de TEF ONLINE, se lee secuencialmente 
    // ordenado por la Primary Key y segun los valores del ultimo registro 
    // tal que en la primer lectura esos valores estan en blanco y ceros,
    // y en las subsiguientes lecturas contienen los valores del anterior.
    /////////////////////////////////////////////////////////////////////////

    // Formatear parametros de lectura segun ultima PRIMARY KEY obtenida
    FormatParamsFromInputData( 
	     szFRNAME ,	   szTONAME ,
	     szRECTYPE,    chACTIVITY,
         szFILLER1,	   szBANDEB ,
	     szFECSOL ,    szNUMTRA ,
	     szNUMABO ,    szTIPOPE ,
	     szIMPORTE,    szSUCDEB ,
	     szNOMSOL ,    szTIPCUEDEB,
	     szNCUECMDEB , szNUMCTADEB ,
	     szFSENDB ,    szHSENDB ,
	     szOPEDB1 ,    szOPEDB2 ,
	     szRECHDB ,    szBANCRE ,
	     szSUCCRE ,    szNOMBEN,
	     szTIPCRE ,    szCTACRE,
	     szFSENCR,     szHSENCR,
	     szOPECR1,     szOPECR2,
	     szRECHCR,     szOPECON,
	     szOPEAU1,     szOPEAU2,
	     szOPEAU3,     szFECAUT,
	     szHORAUT,     szESTADO,
	     szFECEST,     szOBSER1,
	     szOBSER2,     szCODMAC,
	     szNUMREF,     szNUMENV,
         chCONSOL,     chMARTIT,
         chPRIVEZ,     chDIFCRE,
         chRIEABO,     chRIEBCO );

    ///////////////////////////////////////////////////////////////////
    // Defaults a FECHAS que no intervienen en la PK, omision es FECSOL
    COPYFIELD(szFECEST , szFECSOL ); 
    COPYFIELD(szFSENDB , szFECSOL ); 
    COPYFIELD(szHSENDB , szFECSOL );     
    COPYFIELD(szFSENCR , szFECSOL ); 
    COPYFIELD(szHSENCR , szFECSOL ); 
    COPYFIELD(szFECAUT , szFECSOL ); 
    COPYFIELD(szHORAUT , szFECSOL ); 
    ///////////////////////////////////////////////////////////////////

	// Recuperar la transaccion desde TEF ONLINE
	ptrBackOffice->RetrieveTransaction(  
		szFRNAME ,// Nombre nodo emisor, ej. "DNET", char(4)
		szTONAME , //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
		szRECTYPE ,// Tipo de registro, TDE = débito TCE = crédito, CHAR(3)
        chACTIVITY, // Actividad
        szFILLER1, // RESERVADO
		szBANDEB , //  Código Banco según BCRA	ej."067" Bansud, CHAR(3)
		szFECSOL , //  Fecha de compensación, de día o prox. Hábil -aaaammdd-, CHAR(8)
		szNUMTRA , //  Número de transferencia,CHAR(7) 
		szNUMABO , //  Abonado	, CHAR(7)
		szTIPOPE , //  Tipo de operación -	Ver tabla I - , CHAR(2)
		szIMPORTE  ,//  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
		szSUCDEB ,  //  Sucursal del Banco que tratará el débito, CHAR(4)
		szNOMSOL ,  //  Nombre de la cuenta, 
		szTIPCUEDEB,//   Tipo de cuenta	- Ver tabla II  - CHAR(2)
		szNCUECMDEB , //  Número de cuenta corto -Interno Datanet- CHAR(2)
		szNUMCTADEB , //  Número de cuenta, formato Banco, CHAR(17)
		szFSENDB , //  Fecha de envío de la transf.  al Banco -Aammdd-, CHAR(6)
		szHSENDB , // Hora de envío, Hhmm, CHAR(4)
		szOPEDB1 , //  Identificación del operador #1. Primer autorizante del Banco de db. CHAR(2)
		szOPEDB2 , //  Identificación del operador #2. Segundo autorizante, CHAr(2).
		szRECHDB , //  Motivo del rechazo del Banco de débito -Ver tabla III - CHAR(4)
		szBANCRE , //  Código de Banco según BCRA, p/credito. CHAR(3)
		szSUCCRE , //  Sucursal del Banco que tratará el crédito. CHAR(4)				
		szNOMBEN,  //  Nombre de la cuenta a acreditar, VARCHAR(29)
		szTIPCRE , //  Tipo de cuenta -Ver tabla II-, CHAR(2)
		szCTACRE, //  Número de cuenta -Formato Banco-, CHAR(17)
		szFSENCR, //  Fecha de envío de la transf.  al Banco	 -Aammdd-, CHAR(6)
		szHSENCR, //  Hora de envío -Hhmm-, CHAR(4)
		szOPECR1, //  Identificación del operador #1, primer autorizante del Banco de cr. CHAR(2)
		szOPECR2, //  Identificación del operador #2, segundo autorizante, CHAR(2)
		szRECHCR, //  Motivo del rechazo del Banco de crédito -ver tabla III-, CHAR(4)
		szOPECON , //  Operador que confeccionó la transferencia	, CHAR(2)
		szOPEAU1 , //  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
		szOPEAU2 , //  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
		szOPEAU3 , //  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
		szFECAUT , //  Fecha de autorización (envío del pago a la red) aammdd,  CHAR(6)
		szHORAUT , //  Hora de autorización (envío del pago a la red) hhmm, CHAR(4)
		szESTADO , //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
		szFECEST , //  Fecha última modificación de ESTADO	, CHAR(6)
		szOBSER1 , //  Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
		szOBSER2 , //  Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
		szCODMAC , //  Message Authentication Code (MAC), CHAR(12)
		szNUMREF , //  Número de transferencia PC	 , //  nro.lote (N,3)+nro. operación (N,4) , CHAR(7) 
		szNUMENV , //  Número de envío del abonadochar(3)
		chCONSOL,  //  Marca de consolidación de movimientos ("S" ó "N")
		chMARTIT,  //  "S"/"N"
		chPRIVEZ,  //  "S"/"N"
		chDIFCRE,  //  "S"/"N"
		chRIEABO,  //  "S"/"N"
		chRIEBCO,  //  "S"/"N"
        szEstadoTratamiento, // Estado tratamiento parametrizado ["0".."9"]
		szACCION );
	if(ptrBackOffice->GetStatus() != 0)
		return ( DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED );
    

	// Adecuacion de estructura de entrada desde campos de base de datos
    FormatInputDataFromParams( 
         szFRNAME ,	   szTONAME ,
	     szRECTYPE,    chACTIVITY,
         szFILLER1,	   szBANDEB ,
	     szFECSOL ,    szNUMTRA ,
	     szNUMABO ,    szTIPOPE ,
	     szIMPORTE,    szSUCDEB ,
	     szNOMSOL ,    szTIPCUEDEB,
	     szNCUECMDEB , szNUMCTADEB ,
	     szFSENDB ,    szHSENDB ,
	     szOPEDB1 ,    szOPEDB2 ,
	     szRECHDB ,    szBANCRE ,
	     szSUCCRE ,    szNOMBEN,
	     szTIPCRE ,    szCTACRE,
	     szFSENCR,     szHSENCR,
	     szOPECR1,     szOPECR2,
	     szRECHCR,     szOPECON,
	     szOPEAU1,     szOPEAU2,
	     szOPEAU3,     szFECAUT,
	     szHORAUT,     szESTADO,
	     szFECEST,     szOBSER1,
	     szOBSER2,     szCODMAC,
	     szNUMREF,     szNUMENV,
         chCONSOL,     chMARTIT,
         chPRIVEZ,     chDIFCRE,
         chRIEABO,     chRIEBCO );

    //////////////////////////////////////////////////////////////////////////
    // Establecer el mensaje como RESPUESTA, y calcular la MAC respectiva
    //////////////////////////////////////////////////////////////////////////
	if( strncmp(dpsMsg.input.data.RECTYPE,DPS_RECTYPE_DEBITO_EJECUTADO ,3)==0 
        ||
        strncmp(dpsMsg.input.data.RECTYPE,DPS_RECTYPE_DEBITO_RESPUESTA ,3)==0 )
    {
        dpsMsg.SetRecType( DPS_RECTYPE_DEBITO_RESPUESTA );
        // Calcular MACs de respuesta
        if(wRespCde = CalculateMAC("2", szMAC2 ) != DPS_APROBADA_SIN_RECHAZO)
        {
            dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer() );
            wRespCde = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO ;
            dpsMsg.SetStatusCode( wRespCde );
            wRespCde = DPS_DEMORADA_NO_ENVIAR_RESPUESTA ;
        }
    }
	else if( strncmp(dpsMsg.input.data.RECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO ,3)==0 
             ||
             strncmp(dpsMsg.input.data.RECTYPE,DPS_RECTYPE_CREDITO_RESPUESTA ,3)==0 )
    {
        dpsMsg.SetRecType( DPS_RECTYPE_CREDITO_RESPUESTA );
        // Calcular MACs de respuesta
        if(wRespCde = CalculateMAC("3", szMAC3 ) != DPS_APROBADA_SIN_RECHAZO)
        {
            dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer() );
            wRespCde = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO ;
            dpsMsg.SetStatusCode( wRespCde );
            wRespCde = DPS_DEMORADA_NO_ENVIAR_RESPUESTA ;
        }
    }
    else
        // Error del sistema
        wRespCde = DPS_SISTEMA_NO_DISPONIBLE;

    //////////////////////////////////////////////////////////////////////////////////
    // POST-CONTROL : Verificar fecha de solicitud ante reprocesamientos, tal que 
    // no se reenvien respuestas ya enviadas : Para CREDITOS y  DEBITOS.
    ValidatePostingDate( &esRcode , &esRcodeStatus , is_true );
    // Es una fecha de solicitud diferida... significa que ya se envio
    if( ( ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esRcodeStatus
          &&
          ESTL_TRANSFERENCIA_OK == esRcode )
        ||
        // O es una fecha menor a la del dia
        ESTL_FECHA_DE_SOLICITUD_MENOR_A_FECHA_DEL_DIA == esRcode )
    {       
        // Si era ANTERIOR y ya estamos en fecha de procesamiento siguiente, no enviar.
        // Esto es un error y significa que NO SE EJECUTO EL CIERRE del dia anterior.
        if ( ESTL_FECHA_DE_SOLICITUD_MENOR_A_FECHA_DEL_DIA == esRcode )
        {
            // Respuestas de dias anteriores NO SE ENVIAN
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
        }
        // Si era DIFERIDA y ya estamos en fecha de procesamiento valida, enviarla
        else if ( ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esRcodeStatus
                  &&
                  ESTL_TRANSFERENCIA_OK == esRcode )
        {
            // Marca de diferida en general ahora en pendiente
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
        };
        // Ademas :        
        // ?Trata CREDITO diferido ? "SI", entonces no diferir la respuesta
        if ( 'S' == chDIFCRE[0] 
             && 
             !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) 
             &&
             ESTL_FECHA_DE_SOLICITUD_MENOR_A_FECHA_DEL_DIA != esRcode )
        {
            // Marca de envio ahora en pendiente
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_ ;
        }
        // ?Trata CREDITO diferido ? "NO", entonces no enviar la respuesta
        else if ( 'S' != chDIFCRE[0] 
             && 
             !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) 
             &&
             ESTL_FECHA_DE_SOLICITUD_MENOR_A_FECHA_DEL_DIA != esRcode )
        {
            // Marca de envio diferido en gral. 
            szPENDENVIO[0] = _DPS_PEND_ENV_DIFERIDA_C_;
        };
    };// end-if-FECHA_SOLICITUD

    //////////////////////////////////////////////////////////////////////////////////
    // POST-CONTROL : Actualizar la transaccion en la tabla TEF ONLINE si la misma 
    // esta demorada o corresponde no enviar la respuesta
    if( DPS_DEMORADA_NO_ENVIAR_RESPUESTA == wRespCde 
        ||
       _DPS_PEND_ENV_NO_C_ == szPENDENVIO[0] )
    {
        // No esta pendiente de envio, sino fallida por dato faltante (BANCO/ABONADO)
        // y corresponde la lectura al AUTORIZADOR (tratamiento distinto de '9' y '0')
        // entonces el tratamiento es ahora pendiente (aunque sea diferida '8')
        if( DPS_DEMORADA_NO_ENVIAR_RESPUESTA == wRespCde
            &&
            _DPS_TRAT_PROCESADA_C_ != szEstadoTratamiento[0] 
            &&
            _DPS_TRAT_PENDIENTE_C_ != szEstadoTratamiento[0] )
        {
            szPENDENVIO[0]         = _DPS_PEND_ENV_FALLIDA_C_;
            szEstadoTratamiento[0] = _DPS_TRAT_PENDIENTE_C_ ;
        }
        // Actualizar tabla
        wRetVal = ptrBackOffice->UpdateTransaction(
	        szBANDEB , //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
	        szFECSOL , //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	        szNUMTRA , //  Numero de transferencia,CHAR(7) 
	        szNUMABO , //  Abonado	, CHAR(7)
	        szNUMCTADEB, //  Numero de cuenta, formato Banco, CHAR(17)
	        szBANCRE , //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
	        szCTACRE , //  Numero de cuenta -Formato Banco-, CHAR(17)
	        szESTADO , //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	        szNUMREF , //  Numero de transferencia PC	 []
	        szNUMENV , //  Numero de envio del abonadochar(3)
            szPENDENVIO, // Pendiente de ENVIO se modifica y pasa a ENVIADO, "N"
            szRECHDB,    // Motivos de rechazo DB
            szRECHCR,   // Motivos de rechazo CR
            szEstadoTratamiento );
    	if(ptrBackOffice->GetStatus() != 0)
	    	wRetVal = DPS_SISTEMA_NO_DISPONIBLE; 
    };

    // Actualizar ESTADO
    dpsMsg.SetStatusCode( szESTADO );
    // Post-Formatear mensaje de respuesta desde mensaje de entrada recuperado
    dpsMsg.FormatResponseMessage( szMAC2 , szMAC3 );

	// Postcondicion segun se haya detectado un error o no:
    // DPS_APROBADA_SIN_RECHAZO es el valor siempre que haya algo autorizado
    // DPS_DEMORADA_NO_ENVIAR_RESPUESTA cuando se recupera algo pendiente prox dia
    // DPS_SISTEMA_NO_DISPONIBLE cuando no es respuesta de debito/credito
	return ( wRespCde );
}

///////////////////////////////////////////////////////////////////////////////////
// Formatear parametros desde el mensaje de entrada
///////////////////////////////////////////////////////////////////////////////////
void TrxResBTF::FormatParamsFromInputData( 
	            char * szFRNAME ,	char * szTONAME ,
	            char * szRECTYPE,   char * chACTIVITY,
                char * szFILLER1,	char * szBANDEB ,
	            char * szFECSOL ,   char * szNUMTRA ,
	            char * szNUMABO ,   char * szTIPOPE ,
	            char * szIMPORTE,   char * szSUCDEB ,
	            char * szNOMSOL ,   char * szTIPCUEDEB,
	            char * szNCUECMDEB ,char * szNUMCTADEB ,
	            char * szFSENDB ,   char * szHSENDB ,
	            char * szOPEDB1 ,   char * szOPEDB2 ,
	            char * szRECHDB ,   char * szBANCRE ,
	            char * szSUCCRE ,   char * szNOMBEN,
	            char * szTIPCRE ,   char * szCTACRE,
	            char * szFSENCR,    char * szHSENCR,
	            char * szOPECR1,    char * szOPECR2,
	            char * szRECHCR,    char * szOPECON,
	            char * szOPEAU1,    char * szOPEAU2,
	            char * szOPEAU3,    char * szFECAUT,
	            char * szHORAUT,    char * szESTADO,
	            char * szFECEST,    char * szOBSER1,
	            char * szOBSER2,    char * szCODMAC,
	            char * szNUMREF,    char * szNUMENV,
                char * chCONSOL,    char * chMARTIT,
                char * chPRIVEZ,    char * chDIFCRE,
                char * chRIEABO,    char * chRIEBCO )
{
	// Adecuacion de campos de estructuras a variables string
	FULLCOPYFIELD(szFRNAME , dpsMsg.input.data.FRNAME ); // Nombre nodo emisor, ej. "DNET", char(4)
	FULLCOPYFIELD(szTONAME , dpsMsg.input.data.TONAME); //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
	FULLCOPYFIELD(szRECTYPE , dpsMsg.input.data.RECTYPE); // Tipo de registro, TDE = débito TCE = crédito, CHAR(3)
	chACTIVITY[0] = dpsMsg.input.data.ACTIVITY[0];  //  Actividad, CHAR(1)="N"
    FULLCOPYFIELD(szFILLER1, dpsMsg.input.data.FILLER1); //  RESERVADO
	FULLCOPYFIELD(szBANDEB , dpsMsg.input.data.BANDEB); //  Código Banco según BCRA	ej."067" Bansud, CHAR(3)
	FULLCOPYFIELD(szFECSOL , dpsMsg.input.data.FECSOL ); //  Fecha de compensación, de día o prox. Hábil -aaaammdd-, CHAR(8)
    // Pre-formatear campo
    FormatTrxDateTime_WMask(szFECSOL, NULL, dpsMsg.input.data.FECSOL, NULL, 
                            is_true, is_false, is_true, is_true, 
                            _DATE_STR_LENGTH_, 0 );
                            // Anteriormente se utilizaban estos argumentos :
                            // bUseSeparators=is_true, 
                            // bUse4DigitsYear=is_true, 
                            // bFormatIsMMDDYY=is_false, 
                            // bAlwaysTimeIsDateAndTimeis_true, 
                            // nTimestampLength=0, 
                            // nHourLength=0 
	FULLCOPYFIELD(szNUMTRA , dpsMsg.input.data.NUMTRA); //  Número de transferencia,CHAR(7) 
	FULLCOPYFIELD(szNUMABO , dpsMsg.input.data.NUMABO); //  Abonado	, CHAR(7)
	FULLCOPYFIELD(szTIPOPE , dpsMsg.input.data.TIPOPE); //  Tipo de operación -	Ver tabla I - , CHAR(2)
	FULLCOPYFIELD(szIMPORTE, dpsMsg.input.data.IMPORTE); //  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
    Format_ISO_Ammount( dpsMsg.input.data.IMPORTE, 2, szIMPORTE, sizeof(szIMPORTE), 
        is_false, NULL ); 
	FULLCOPYFIELD(szSUCDEB , dpsMsg.input.data.DATDEB.SUCDEB); //  Sucursal del Banco que tratará el débito, CHAR(4)
	FULLCOPYFIELD(szNOMSOL , dpsMsg.input.data.DATDEB.NOMSOL); //  Nombre de la cuenta, 
	FULLCOPYFIELD(szTIPCUEDEB, dpsMsg.input.data.DATDEB.TIPCUE); //   Tipo de cuenta	- Ver tabla II  - CHAR(2)
	FULLCOPYFIELD(szNCUECMDEB , dpsMsg.input.data.DATDEB.NCUECM); //  Número de cuenta corto -Interno Datanet- CHAR(2)
	FULLCOPYFIELD(szNUMCTADEB , dpsMsg.input.data.DATDEB.NUMCTA); //  Número de cuenta, formato Banco, CHAR(17)
	FULLCOPYFIELD(szFSENDB , dpsMsg.input.data.DATDEB.FSENDB ); //  Fecha de envío de la transf.  al Banco -Aammdd-, CHAR(6)
	FULLCOPYFIELD(szHSENDB , dpsMsg.input.data.DATDEB.HSENDB ); // Hora de envío, Hhmm, CHAR(4)
    // Pre-formatear campo
    FormatTrxDateTime_WMask(szFSENDB, szHSENDB, 
                            dpsMsg.input.data.DATDEB.FSENDB, dpsMsg.input.data.DATDEB.HSENDB, 
                            is_true, is_false, is_true, is_true, _DATE_STR_LENGTH_, _TIME_STR_LENGTH_ );
	FULLCOPYFIELD(szOPEDB1 , dpsMsg.input.data.DATDEB.OPEDB1 ); //  Identificación del operador #1. Primer autorizante del Banco de db. CHAR(2)
	FULLCOPYFIELD(szOPEDB2 , dpsMsg.input.data.DATDEB.OPEDB2 ); //  Identificación del operador #2. Segundo autorizante, CHAr(2).
	FULLCOPYFIELD(szRECHDB , dpsMsg.input.data.DATDEB.RECHDB ); //  Motivo del rechazo del Banco de débito -Ver tabla III - CHAR(4)
	FULLCOPYFIELD(szBANCRE , dpsMsg.input.data.DATCRE.BANCRE ); //  Código de Banco según BCRA, p/credito. CHAR(3)
	FULLCOPYFIELD(szSUCCRE , dpsMsg.input.data.DATCRE.SUCCRE ); //  Sucursal del Banco que tratará el crédito. CHAR(4)				
	FULLCOPYFIELD(szNOMBEN,  dpsMsg.input.data.DATCRE.NOMBEN ); //  Nombre de la cuenta a acreditar, VARCHAR(29)
	FULLCOPYFIELD(szTIPCRE , dpsMsg.input.data.DATCRE.TIPCRE ); //  Tipo de cuenta -Ver tabla II-, CHAR(2)
	FULLCOPYFIELD(szCTACRE, dpsMsg.input.data.DATCRE.CTACRE ); //  Número de cuenta -Formato Banco-, CHAR(17)
	FULLCOPYFIELD(szFSENCR, dpsMsg.input.data.DATCRE.FSENCR ); //  Fecha de envío de la transf.  al Banco	 -Aammdd-, CHAR(6)
	FULLCOPYFIELD(szHSENCR, dpsMsg.input.data.DATCRE.HSENCR ); //  Hora de envío -Hhmm-, CHAR(4)
    // Pre-formatear campo
    FormatTrxDateTime_WMask(szFSENCR, szHSENCR, 
                            dpsMsg.input.data.DATCRE.FSENCR, dpsMsg.input.data.DATCRE.HSENCR, 
                            is_true, is_false, is_true, is_true, _DATE_STR_LENGTH_, _TIME_STR_LENGTH_ );
	FULLCOPYFIELD(szOPECR1, dpsMsg.input.data.DATCRE.OPECR1 ); //  Identificación del operador #1, primer autorizante del Banco de cr. CHAR(2)
	FULLCOPYFIELD(szOPECR2, dpsMsg.input.data.DATCRE.OPECR2 ); //  Identificación del operador #2, segundo autorizante, CHAR(2)
	FULLCOPYFIELD(szRECHCR, dpsMsg.input.data.DATCRE.RECHCR ); //  Motivo del rechazo del Banco de crédito -ver tabla III-, CHAR(4)
	FULLCOPYFIELD(szOPECON, dpsMsg.input.data.OPECON ); //  Operador que confeccionó la transferencia	, CHAR(2)
	FULLCOPYFIELD(szOPEAU1, dpsMsg.input.data.OPEAU1 );//  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	FULLCOPYFIELD(szOPEAU2, dpsMsg.input.data.OPEAU2 );//  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	FULLCOPYFIELD(szOPEAU3, dpsMsg.input.data.OPEAU3 );//  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	FULLCOPYFIELD(szFECAUT, dpsMsg.input.data.FECAUT );//  Fecha de autorización (envío del pago a la red) aammdd,  CHAR(6)
	FULLCOPYFIELD(szHORAUT, dpsMsg.input.data.HORAUT );//  Hora de autorización (envío del pago a la red) hhmm, CHAR(4)
    // Pre-formatear campo
    FormatTrxDateTime_WMask(szFECAUT, szHORAUT, 
                            dpsMsg.input.data.FECAUT, dpsMsg.input.data.HORAUT, 
                            is_true, is_false, is_true, is_true, _DATE_STR_LENGTH_, _TIME_STR_LENGTH_ );
	FULLCOPYFIELD(szESTADO, dpsMsg.input.data.ESTADO );//  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	FULLCOPYFIELD(szFECEST, dpsMsg.input.data.FECEST );//  Fecha última modificación de ESTADO	, CHAR(6)
    // Pre-formatear campo
    FormatTrxDateTime_WMask(szFECEST, NULL, dpsMsg.input.data.FECEST, NULL, 
                            is_true, is_false, is_true, is_true, _DATE_STR_LENGTH_, 0 );
	FULLCOPYFIELD(szOBSER1, dpsMsg.input.data.OBSER1 );//  Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
	FULLCOPYFIELD(szOBSER2, dpsMsg.input.data.OBSER2 );//  Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
	FULLCOPYFIELD(szCODMAC, dpsMsg.input.data.CODMAC );//  Message Authentication Code (MAC), CHAR(12)
	FULLCOPYFIELD(szNUMREF, dpsMsg.input.data.NUMREF );//  Número de transferencia PC	 , //  nro.lote (N,3)+nro. operación (N,4) , CHAR(7) 
	FULLCOPYFIELD(szNUMENV, dpsMsg.input.data.NUMENV );//  Número de envío del abonadochar(3)
	chCONSOL[0] = dpsMsg.input.data.CONSOL[0];  //  Marca de consolidación de movimientos ("S" ó "N")
	chMARTIT[0] = dpsMsg.input.data.MARTIT[0];  //  "S"/"N"
	chPRIVEZ[0] = dpsMsg.input.data.PRIVEZ[0];  //  "S"/"N"
	chDIFCRE[0] = dpsMsg.input.data.DIFCRE[0];  //  "S"/"N"
	chRIEABO[0] = dpsMsg.input.data.RIEABO[0];  //  "S"/"N"
	chRIEBCO[0] = dpsMsg.input.data.RIEBCO[0];  //  "S"/"N"

};

///////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////
// Recuperar el cuerpo del mensaje de notificacion, ya formateado
///////////////////////////////////////////////////////////////////////////////////
LPCSTR TrxResBTF::GetMSG_Mail(char *pszMsg , short *pshQSends, char *pszDestiny)
{
    // Longitud del buffer de notificacion, si hubiera
    short wMailLen = 0;
    // RPrecondicion : ecuperar long. del texto y verificar la misma
    wMailLen = strlen(szMSG_Mail);
    // Precondicion de copia : buffer externo informado
    if(pszMsg && wMailLen > 0)
    {
        // Copiar al buffer , el mensaje interno formateado
        strcpy(pszMsg,szMSG_Mail);
    }    
    // Precondicion de copia : parametro externo informado
    wMailLen = strlen(szMSG_Destiny);
    if(pszDestiny && wMailLen > 0 )
    {
        // Copiar al parametro el destinatario del alerta
        if( szMSG_Destiny[0]               != 0x00 &&
            strcmp(szMSG_Destiny,"DEFAULT")!=0     &&
            strcmp(szMSG_Destiny,"ALERTA") !=0     )
            // Copia en si misma
            strcpy(pszDestiny,szMSG_Destiny);
        else
            // Sin destinatario
            (*pszDestiny) = 0x00;        
    }
    // Precondicion de copia : argumento valido
    if( pshQSends )
    {
        // Copiar la cantidad de envios efectuados
        (*pshQSends) = shMSG_Counter;
    }
    // Postcondicion : Retorno del buffer formateado, si hubiera
    wMailLen = strlen(szMSG_Mail);
    return (wMailLen > 0)
        ? szMSG_Mail 
        : NULL ;
}



///////////////////////////////////////////////////////////////////////
// Recuperar primer movimiento de refresh online disponible
///////////////////////////////////////////////////////////////////////
boolean_t TrxResBTF::RetrieveFirstRefreshMovement(char *lpData, unsigned short *pnLen,                                                 
                                                    char *lpDataBkp, unsigned short *pnLenBkp,
                                                    const char cszRECTYPE[] ,
                                                    char *szSTAMP  )   // Stamp Pk
{

    // Precondicion : argumentos validos
    if( NULL == lpData     ||    // Buffer de datos principal
        NULL == pnLen      ||    // Longitud de buffer (INPUT-OUTPUT)
        NULL == pnLenBkp   ||    // Buffer de datos backup
        NULL == lpDataBkp  ||    // Longitud de buffer (INPUT-OUTPUT)
        NULL == cszRECTYPE ||    // Tipo de registro Transferencia/refresh
        NULL == szSTAMP   )      // Stamp Pk
        // Error
        return is_false;


    // Si la recuperacion es exitosa, copiar el buffer a la salida.
    WORD wRespCde = RetrieveRefreshMovement( 
                         szSTAMP    ,   // Stamp PK 
                         cszRECTYPE );  // Tipo de registro de interfase

    if( DPS_APROBADA_SIN_RECHAZO == wRespCde )
    {
        // Copiar la estructura de salida YA FORMATEADA
        if(strcmp(cszRECTYPE,"CTA")==0)
        {
            (*pnLen) = sizeof(refrMsg.data.BUFFER.CTA);
        }
        else if(strcmp(cszRECTYPE,"COB")==0)
        {
            (*pnLen) = sizeof(refrMsg.data.BUFFER.COB);
        }
        else if(strcmp(cszRECTYPE,"PGO")==0)
        {
            (*pnLen) = sizeof(refrMsg.data.BUFFER.PGO);
        }
        else if(strcmp(cszRECTYPE,"CCB")==0)
        {
            (*pnLen) = sizeof(refrMsg.data.BUFFER.CCB);
        }
        else if(strcmp(cszRECTYPE,"CCP")==0)
        {
            (*pnLen) = sizeof(refrMsg.data.BUFFER.CCP);
        }
        // Sumar el tamaño del Header
        (*pnLen) += sizeof(refrMsg.data.header) ;
        // Copiar la estructura de envio-output
        memcpy( lpData, &refrMsg, (*pnLen) );
        // Copiar la estructura backup
        memcpy( lpDataBkp, lpData, (*pnLen) );
        (*pnLenBkp) = (*pnLen);
        // Postcondicion : asegurarse que la salida formateada ES VALIDA,
        // y retornar un indicativo si EXISTE o NO EL REGISTRO
        return ( strncmp(refrMsg.data.header.TPNAME,"DPRE",4)==0 )
                ? is_true
                : is_false ;
    }
    else if( DPS_DEMORADA_NO_ENVIAR_RESPUESTA == wRespCde )
    {
        // Copiar la estructura de salida YA FORMATEADA
        if(strcmp(cszRECTYPE,"CTA")==0)
        {
            (*pnLen) = sizeof(refrMsg.data.BUFFER.CTA);
        }
        else if(strcmp(cszRECTYPE,"COB")==0)
        {
            (*pnLen) = sizeof(refrMsg.data.BUFFER.COB);
        }
        else if(strcmp(cszRECTYPE,"PGO")==0)
        {
            (*pnLen) = sizeof(refrMsg.data.BUFFER.PGO);
        }
        else if(strcmp(cszRECTYPE,"CCB")==0)
        {
            (*pnLen) = sizeof(refrMsg.data.BUFFER.CCB);
        }
        else if(strcmp(cszRECTYPE,"CCP")==0)
        {
            (*pnLen) = sizeof(refrMsg.data.BUFFER.CCP);
        }
        // Sumar el tamaño del Header
        (*pnLen) += sizeof(refrMsg.data.header);
        // Copiar la estructura de envio-output
        memcpy( lpData, &refrMsg, (*pnLen) );
        // Copiar la estructura backup
        memcpy( lpDataBkp, lpData, (*pnLen) );
        (*pnLenBkp) = (*pnLen);
        // Postcondicion : NO se debe enviar nada
        return is_false ;
    }
    else 
    {
        // Blanquear Primary-Keys para siguiente ciclo de lectura secuencial
        memset( &refrMsg.data.BUFFER, ' ', sizeof(refrMsg.data.BUFFER) );
        memset( lpData   , ' ', (*pnLen));
        memset( lpDataBkp, ' ', (*pnLenBkp));
        (*pnLen)    = 0;
        (*pnLenBkp) = 0;
        // Postcondicion : NO existe registro, no hay nada que enviar
        return is_false ;
    };
}

///////////////////////////////////////////////////////////////////////////////////
// Recuperar la primer transaccion disponible en el sistema autorizador del banco
///////////////////////////////////////////////////////////////////////////////////
WORD TrxResBTF::RetrieveRefreshMovement( 
         char *szSTAMP,
         const char cszRECTYPE[] ) // Tipo de Registro a recuperar
{
	// Campos locales que mapean campos de estructruras DPS
    // Codigo de respuesta retornado
    WORD wRespCde = DPS_APROBADA_SIN_RECHAZO,
         wRetVal  = DPS_APROBADA_SIN_RECHAZO;
    char szBANCO[4+1]      = {"000"},   //Codigo Banco segun BCRA	ej."067":Bansud, CHAR(3)
	     szNUMCTA[17+1]    = {"00000000000000000"},  //Numero de cuenta, formato Banco, CHAR(17)
	     szFECVAL[8+8+8+1] = {"2001/01/01 00:00:00"},  //Fecha Valor -aaaammdd-, CHAR(8)
	     szSECUENCIAL[12+1]= {"0000000000"},//Numero correlativo, CHAR(10)
	     chDEBCRE[1+1]     = {"D"},   //Debito-Credito('D'/'C')	, CHAR(1)
	     szIMPORTE[18+1]   = {"000000000000000000"},  //Importe de la transferencia, Signo, 15 enteros y 2 decimales, tipo de dato MONEY.
	     szNUMCOMP[12+1]   = {"0000000000"},  //Numero de comprobante, CHAR(10)
	     chDIFMOV[1+1]     = {"M"},   //Diferido-Movimiento('D'/'M')	, CHAR(1)
         szCODOPE[3+1]     = {"000"},   //Tipo de operacion -	CHAR(3)
	     szHORAMOV[8+8+8+1]= {"2001/01/01 00:00:00"},  //Hora de movimiento, Hhmm, CHAR(4)
	     szSUCURSAL[3+1]   = {"000"},//Sucursal, CHAR(3)
	     szFECPRO[8+8+8+1] = {"2001/01/01 00:00:00"},   //Fecha de proceso en el Banco -Aaaammdd-, CHAR(8)
	     chCONTRA[1+1]     = {"0"},   //Contraaseinto('1'/'0')	, CHAR(1)
	     szTIPCUE[2+1]     = {"00"},   //tipo de cuenta	- CHAR(2)
	     szDEPOSITO[8+1]   = {"....:..."},  //DEPOSITO, VARCHAR(8)
	     szDESCRIP[25+1]   = {"....:....1....:....2....:"},  //Texto libre , VARCHAR(25)
	     chCODPRO[1+1]     = {"0"};   //Codigo de Proceso	, CHAR(1)	
    ///////////////////////////////////////////////////////////////////////////////
	char szNUMCTAEDIT[23+1] = {"00000000000000000000000"},  //Numero de cuenta editada, formato Banco, CHAR(23)
	     szMONEDA[3+1]      = {"000"},  //Moneda, CHAR(3)
	     szTIPOMOV[1+1]     = {"0"}, //Tipo Mov. CHAR(1)
	     szCLEARING[1+1]    = {"0"},//Clearing , CHAR(1)
	     szNROCHEQUE[4+1]   = {"0000"},//Nro. Cheque, CHAR(4)
	     szCODCLI[10+1]     = {"0000000000"},  //Codigo Cliente, CHAR(10)
	     szNOMCLI[24+1]     = {"N/A                     "},  //Codigo Cliente, CHAR(24)
	     szCOMPROBANTE[16+1]= {"0000000000000000"}, //Comprobante-Factura-Recibo, CHAR(16)
	     szBOLETA[9+1]      = {"000000000"},  //BOLETA, VARCHAR(9)
	     szCOBRANZA[9+1]    = {"000000000"}; //Cobranza , VARCHAR(9)
    // Fecha y Hora actuales y locales del Sistema
    time_t tSystemTime   = time(NULL);               // System Time
    tm     *ptmLocalTime = localtime(&tSystemTime);  // Local System Time
    ///////////////////////////////////////////////////////////////////////////////

	// Precondicion : Existe conexion a base de datos
	if( NULL == ptrBackOffice   ||    // Conexion a Base de datos        
        NULL == cszRECTYPE )   // Tipo de registro
		return (wRespCde = DPS_FUERZA_MAYOR);

    //////////////////////////////////////////////////////////////////////////////////
    // Para la recuperacion de la vista de MOVIMIENTOS REFRESH, se lee secuencialmente 
    // ordenado por elindice y segun los valores del ultimo registro 
    // tal que en la primer lectura esos valores estan en blanco y ceros,
    // y en las subsiguientes lecturas contienen los valores del anterior.
    //////////////////////////////////////////////////////////////////////////////////


	// Recuperar la transaccion desde la vista de MOVIEMIENTOS REFRESH A ENVIAR
    if( strcmp(cszRECTYPE,"CTA")== 0)
    {
	    ptrBackOffice->RetrieveRefreshMovement(  
	        szBANCO ,   //Codigo Banco segun BCRA	ej."067":Bansud, CHAR(3)
	        szNUMCTA ,  //Numero de cuenta, formato Banco, CHAR(17)
	        szFECVAL,   //Fecha Valor -aaaammdd-, CHAR(8)
	        szSECUENCIAL,//Numero correlativo, CHAR(10)
	        chDEBCRE,   //Debito-Credito('D'/'C')	, CHAR(1)
	        szIMPORTE,  //Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	        szNUMCOMP,  //Numero de comprobante, CHAR(10)
	        chDIFMOV,   //Diferido-Movimiento('D'/'M')	, CHAR(1)
            szCODOPE,   //Tipo de operacion -	CHAR(3)
	        szHORAMOV,  //Hora de movimiento, Hhmm, CHAR(4)
	        szSUCURSAL, //Hora de movimiento, Hhmm, CHAR(4)
	        szFECPRO,   //Fecha de proceso en el Banco -Aaaammdd-, CHAR(8)
	        chCONTRA,   //Contraaseinto('1'/'0')	, CHAR(1)
	        szTIPCUE,   //tipo de cuenta	- CHAR(2)
	        szDEPOSITO, //DEPOSITO, VARCHAR(8)
	        szDESCRIP,  //Texto libre , VARCHAR(25)
	        chCODPRO ,  //Codigo de Proceso	, CHAR(1)
			szSTAMP );  // Last Value of PK;
    }
    else
        // Error por omision
        return ( DPS_FUERZA_MAYOR );

    // Hay registros disponibles?
	if(ptrBackOffice->GetStatus() != 0)
		return ( DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED );
    

    // Inicializar mensaje de salida
    refrMsg.FormatResponseMessage( cszRECTYPE );
    // Post-Formatear mensaje de respuesta desde mensaje de entrada recuperado
    FormatOutputDataFromParams(
	    szBANCO ,   //Codigo Banco segun BCRA	ej."067":Bansud, CHAR(3)
	    szNUMCTA ,  //Numero de cuenta, formato Banco, CHAR(17)
	    szFECVAL,  //Fecha Valor -aaaammdd-, CHAR(8)
	    szSECUENCIAL,//Numero correlativo, CHAR(10)
	    chDEBCRE,   //Debito-Credito('D'/'C')	, CHAR(1)
	    szIMPORTE,  //Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	    szNUMCOMP,  //Numero de comprobante, CHAR(10)
	    chDIFMOV,   //Diferido-Movimiento('D'/'M')	, CHAR(1)
        szCODOPE,   //Tipo de operacion -	CHAR(3)
	    szHORAMOV,  //Hora de movimiento, Hhmm, CHAR(4)
	    szSUCURSAL, //Surcursal, CHAR(3)
	    szFECPRO,   //Fecha de proceso en el Banco -Aaaammdd-, CHAR(8)
	    chCONTRA,   //Contraaseinto('1'/'0')	, CHAR(1)
	    szTIPCUE,   //tipo de cuenta	- CHAR(2)
	    szDEPOSITO,  //DEPOSITO, VARCHAR(8)
	    szDESCRIP,  //Texto libre , VARCHAR(25)
	    chCODPRO ,  //Codigo de Proceso	, CHAR(1)
        cszRECTYPE ,// Tipo de Registro a Formatear
        /*************************************************************************/
        szNUMCTAEDIT, //Nro. Cta. editado , CHAR(23)
        szMONEDA    , //Codigo de Moneda, CHAR(3)
        szTIPOMOV   , //Tipo de Movimiento , CHAR(1)
        szCLEARING  , //Clearing, CHAR(1)
	    szNROCHEQUE , //Nro. Cheque, CHAR(4)
	    szCODCLI    , //Codigo Cliente, CHAR(10)
	    szNOMCLI    , //Codigo Cliente, CHAR(24)
	    szCOMPROBANTE,//Comprobante-Factura-Recibo, CHAR(16)
	    szBOLETA     ,//BOLETA, VARCHAR(9)
        szCLEARING   ,//Valor al Cobro, CHAR(1)
	    szCOBRANZA    //Cobranza , VARCHAR(9)
        );
        /*************************************************************************/



	// Postcondicion segun se haya detectado un error o no:
    // DPS_APROBADA_SIN_RECHAZO es el valor siempre que haya algo autorizado
    // DPS_DEMORADA_NO_ENVIAR_RESPUESTA cuando se recupera algo pendiente prox dia
    // DPS_SISTEMA_NO_DISPONIBLE cuando no es respuesta de debito/credito
	return ( wRespCde );
}

///////////////////////////////////////////////////////////////////////////////////
// Formatear mensaje de salida desde los parametros (REFRESH ONLINE)
void TrxResBTF::FormatOutputDataFromParams( 
	    char szBANCO[] ,   //Codigo Banco segun BCRA	ej."067":Bansud, CHAR(3)
	    char szNUMCTA[] ,  //Numero de cuenta, formato Banco, CHAR(17)
	    char szFECVAL[],  //Fecha Valor -aaaammdd-, CHAR(8)
	    char szSECUENCIAL[],//Numero correlativo, CHAR(10)
	    char chDEBCRE[],   //Debito-Credito('D'/'C')	, CHAR(1)
	    char szIMPORTE[],  //Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	    char szNUMCOMP[],  //Numero de comprobante, CHAR(10)
	    char chDIFMOV[],   //Diferido-Movimiento('D'/'M')	, CHAR(1)
        char szCODOPE[],   //Tipo de operacion -	CHAR(3)
	    char szHORAMOV[],  //Hora de movimiento, Hhmm, CHAR(4)
	    char szSUCURSAL[],//Sucursal, CHAR(3)
	    char szFECPRO[],   //Fecha de proceso en el Banco -Aaaammdd-, CHAR(8)
	    char chCONTRA[],   //Contraaseinto('1'/'0')	, CHAR(1)
	    char szTIPCUE[],   //tipo de cuenta	- CHAR(2)
	    char szDEPOSITO[],  //DEPOSITO, VARCHAR(8)
	    char szDESCRIP[],  //Texto libre , VARCHAR(25)
	    char chCODPRO[] , //Codigo de Proceso	, CHAR(1)
        const char cszRECTYPE[],
        char szNUMCTAEDIT[], //Nro. Cta. editado , CHAR(23)
        char szMONEDA[]    , //Codigo de Moneda, CHAR(3)
        char szTIPOMOV[]   , //Tipo de Movimiento , CHAR(1)
        char szCLEARING[]  , //Clearing, CHAR(1)
	    char szNROCHEQUE[] , //Nro. Cheque, CHAR(4)
	    char szCODCLI[]    , //Codigo Cliente, CHAR(10)
	    char szNOMCLI[]    , //Codigo Cliente, CHAR(24)
	    char szCOMPROBANTE[],//Comprobante-Factura-Recibo, CHAR(16)
	    char szBOLETA[]     ,//BOLETA, VARCHAR(9)
        char szVALCOBRO[]   ,//Valor al Cobro, CHAR(1)
	    char szCOBRANZA[]    //Cobranza , VARCHAR(9)
)
{
    // FILLER                      ....:....1....:....2
    const char cszFILLER[20+1] = {"                    "};

    // Tipo de registro "CTA"-cuentas	
    if(strcmp(cszRECTYPE,"CTA")==0)
    {
        // Formateo de estructura de entrada desde campos de base de datos	
	    PADCOPYINTEGER( refrMsg.data.BUFFER.CTA.CODBAN, szBANCO ); 
	    EXACTCOPYFIELD( refrMsg.data.BUFFER.CTA.NUMCTA, szNUMCTA );     
		/*******************************************************************************/
        // Pre-formatear campo : año de 2 digitos en ON/2 CONTINUUS
        FormatTrxDateTime_WMask(refrMsg.data.BUFFER.CTA.FEIMP, NULL, szFECVAL, NULL, 
                                is_false, is_false, is_false, is_false, 0, 0 , 2 );
		/*******************************************************************************/
	    PADCOPYINTEGER( refrMsg.data.BUFFER.CTA.NROCOR, szSECUENCIAL ); 
        refrMsg.data.BUFFER.CTA.DEBCRE = chDEBCRE[0];
        // Monto formateado
        Format_ISO_Ammount( szIMPORTE, 0, refrMsg.data.BUFFER.CTA.IMPORTE, sizeof(refrMsg.data.BUFFER.CTA.IMPORTE), 
            is_false, NULL ); 
        PADCOPYINTEGER( refrMsg.data.BUFFER.CTA.NROCOM, szNUMCOMP ); 
        refrMsg.data.BUFFER.CTA.DIFMOV = chDIFMOV[0];
	    PADCOPYINTEGER( refrMsg.data.BUFFER.CTA.CODOPE, szCODOPE );     
        EXACTCOPYFIELD(refrMsg.data.BUFFER.CTA.HORMOV , szHORAMOV );
	    PADCOPYINTEGER( refrMsg.data.BUFFER.CTA.SUCORI, szSUCURSAL ); 
		/*******************************************************************************/
        // Pre-formatear campo : año de 2 digitos en ON/2 CONTINUUS
        FormatTrxDateTime_WMask(refrMsg.data.BUFFER.CTA.FECPRO, NULL, szFECPRO, NULL, 
                                is_false, is_false, is_false, is_false, 0, 0 , 2 );
		/*******************************************************************************/
        refrMsg.data.BUFFER.CTA.CONTRA = chCONTRA[0];
	    PADCOPYINTEGER( refrMsg.data.BUFFER.CTA.TIPCUE, szTIPCUE ); 
	    EXACTCOPYFIELD( refrMsg.data.BUFFER.CTA.DEPOSI, szDEPOSITO ); 
	    EXACTCOPYFIELD( refrMsg.data.BUFFER.CTA.DESCRI, szDESCRIP ); 
	    EXACTCOPYFIELD( refrMsg.data.BUFFER.CTA.FILLER, cszFILLER ); 
    }
    else if(strcmp(cszRECTYPE,"COB")==0)
    {		
        // Formateo de estructura de entrada desde campos de base de datos	
	    PADCOPYINTEGER( refrMsg.data.BUFFER.COB.BANK   , szBANCO ); 
	    EXACTCOPYFIELD( refrMsg.data.BUFFER.COB.ACCOUNT, szNUMCTA );     
        EXACTCOPYFIELD( refrMsg.data.BUFFER.COB.NUMEDI , szNUMCTAEDIT );             
        EXACTCOPYFIELD( refrMsg.data.BUFFER.COB.FEPRO  , szFECPRO );             
        EXACTCOPYFIELD( refrMsg.data.BUFFER.COB.MONEDA , szMONEDA );
       	//	DPIF_FACTURA		'F';
		//	DPIF_NOTA_CREDITO	'C';
		//	DPIF_BONIFICACION	'B';
		//	DPIF_REVERSO		'R';
        refrMsg.data.BUFFER.COB.TIPO = szTIPOMOV[0];
        EXACTCOPYFIELD( refrMsg.data.BUFFER.COB.FEIMP  , szFECVAL );
		//	DPIF_EFECTIVO	  '0';
		//	DPIF_HORAS_24     '1';
		//	DPIF_HORAS_48	  '2';
		//	DPIF_HORAS_72	  '3';
		//	DPIF_HORAS_96	  '4';
		//	DPIF_HORAS_120	  '5';
		//  DPIF_VALOR_COBRO  '9';
        refrMsg.data.BUFFER.COB.CLEARING = szCLEARING[0];
	    PADCOPYINTEGER( refrMsg.data.BUFFER.COB.NRO_CHEQUE , szNROCHEQUE   ); 
        EXACTCOPYFIELD( refrMsg.data.BUFFER.COB.COD_CLIENTE, szCODCLI ); 
        EXACTCOPYFIELD( refrMsg.data.BUFFER.COB.NOM_CLIENTE, szNOMCLI );
        EXACTCOPYFIELD( refrMsg.data.BUFFER.COB.FACTURA    , szCOMPROBANTE );
        // Es DPIF_VALOR_COBRO ?
        if( '9' == szCLEARING[0] )
            refrMsg.data.BUFFER.COB.VL_COBRO = '1';
        else
            refrMsg.data.BUFFER.COB.VL_COBRO = ' ';
        // Monto formateado
        Format_ISO_Ammount( szIMPORTE, 0, refrMsg.data.BUFFER.COB.IMPORTE, 
                            sizeof(refrMsg.data.BUFFER.COB.IMPORTE), is_false, NULL ); 
        PADCOPYINTEGER( refrMsg.data.BUFFER.COB.SUCURSAL, szSUCURSAL ); 
        BLANKFIELD( refrMsg.data.BUFFER.COB.DESCRI );
		BLANKFIELD( refrMsg.data.BUFFER.COB.FILLER );
    }
    else if(strcmp(cszRECTYPE,"PGO")==0)
    {
    }
    else if(strcmp(cszRECTYPE,"CCB")==0)
    {
    }
    else if(strcmp(cszRECTYPE,"CCP")==0)
    {
    };
};
///////////////////////////////////////////////////////////////////////////////////

#endif //  _SYSTEM_DATANET_ON2_


