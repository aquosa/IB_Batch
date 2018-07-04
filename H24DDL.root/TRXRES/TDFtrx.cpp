////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// Process ATM Transaction Method
//
//   Metodo de Procesamiento de Transaccion ATM, con conexion y desconexion a base de
// datos para cada transaccion entrante, con manejo local de parsing del mensaje ISO
// y seteo del codigo de respuesta en el mensaje saliente.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2006.09.18      mdc     Base usando sockets y RPC para db/cr hacia el core - backoffice
// (Inicial)    2006.10.06      mdc     Mas detalles de LOGG y CR+LF x registro
// (Inicial)    2006.10.18      mdc     fix de inicializacion de libreria TCP4U antes de conectar 
// (Inicial)    2007.10.14      mdc     fix chequeo de respuesta "000000" en prefijo online
// (Inicial)    2007.11.20      mdc     FIX : los primeros bytes deben estar MODIFICADOS para que se asuma el codigo de respuesta interno.
// (Alfa)       2008.09.01      mdc     FIX : chExtractBuff[ CISOMESSAGESIZE * 8 ]; /* 64kb */
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
// Header EXTRACT BASE24 RED LINK
#include <qusrinc/extrctst.h>

#pragma comment(lib, "ws2_32.lib")

// Header conexion RPC usando sockets
#include <qusrinc/tcp4u.h>
#include <qusrinc/CToTcp4u.h>
#include <qusrinc/copymacr.h>



#ifdef _SYSTEM_ATM_DEFINED_
#undef _SYSTEM_ATM_
#endif // _SYSTEM_ATM_DEFINED_


/////////////////////////////////////////////////////////////////////////////////////////
// Definiciones y Macros de expansion MUY utiles para simplificar el codigo
/////////////////////////////////////////////////////////////////////////////////////////
// Macro de asignacion de String, con NULL al final
#define ASSIGNSTRING(dst,src) \
    { strncpy(dst,src,sizeof(src)); dst[sizeof(src)]=0x00; }
// Macro de asignacion de bytes a campo de estructura
#define ASSIGNFIELD(dst,src) \
    { memset(dst,' ',sizeof(dst)); memcpy(dst,src,min(sizeof(src),sizeof(dst)));  }
// Macro de asignacion de ceros a campo de estructura
#define SETFIELDZEROED(dst)    memset(dst ,'0',sizeof(dst))
#define SETSTRUCTBLANKED(dst)  memset(&dst,' ',sizeof(dst))
#define SETSTRUCTNULL(dst)     memset(&dst, 0x00,sizeof(dst))
// Mascara de inicializacion de time-stamp
#define _TIMESTAMP_MASK_YYYY_    "YYYYMMDDHHMMSS"
#define _TIMESTAMP_MASK_YY_      "YYMMDDHHMMSS"
#define _TIMESTAMP_MASK_         _TIMESTAMP_MASK_YY_
/////////////////////////////////////////////////////////////////////////////////////////
// Prototipo de funcion utilitaria que retorna la cantidad de bytes numericos en string
/////////////////////////////////////////////////////////////////////////////////////////
short strnumlen( char *pszValue , const short nMax );
/////////////////////////////////////////////////////////////////////////////////////////
// Longitud maxima de tarjeta en TACK-2
static const short _MAX_CARDNUM_LEN_ = 19;
///////////////////////////////////////////////////////////////////////////
// Nombre local al modulo para el archivo de logg del EXTRACT
static const char _LOGFILENAME_[] = "h24log.txt";
/////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
TrxResTDFUEGO::TrxResTDFUEGO()
{
    // Cierre preventivo
	glLogg.Close();
	// Apertura del archivo de logging
    glLogg.Open(_LOGFILENAME_);
    // Atributos de la instancia
    strRpcSrvAddress = ""; // Servidor RPC          
    ushRpcSrvPort    = 0;  // Port o Aplicacion RPC 

}

///////////////////////////////////////////////////////////////////////////////////
// Constructor opcional
///////////////////////////////////////////////////////////////////////////////////
TrxResTDFUEGO::TrxResTDFUEGO( string strIpAddress, unsigned short ushIpPort )
{
    // Cierre preventivo
	glLogg.Close();
	// Apertura del archivo de logging
    glLogg.Open(_LOGFILENAME_);
    // Atributos de la instancia
    strRpcSrvAddress = strIpAddress; // Servidor RPC          
    ushRpcSrvPort    = ushIpPort;    // Port o Aplicacion RPC 

}

///////////////////////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////////////////////
TrxResTDFUEGO::~TrxResTDFUEGO()
{
    // Cierre
    glLogg.Close();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Conexion con FUEGO DLL/LIB/PGM
// Retorna ERROR ISO por procesamiento aprobado o no,
////////////////////////////////////////////////////////////////////////////////////////////////
WORD TrxResTDFUEGO::ProcessGeneric(void)
    {
    /** Estructura EXTRACT, local ********************************************/    
    char            chExtractBuff[ (CISOMESSAGESIZE * 8 ) - 1 ]; /* 64kb */
    msgEXTRACT      *extractBuff    = (msgEXTRACT *)chExtractBuff;    
    char            *pszExtractBuff = chExtractBuff;
    char            chExtractExt[sizeof(msgEXTRACT)] = {0x00};    
    /*************************************************************************/
    // Saldo de respuesta en campo AMT_3, formato ISO de 12 digitos
    char            *psztTran_Amt   = (char *)extractBuff->cuerpo.AMT_3;
    // Variables de soporte
    int         iLenBuff    = 0,
                iRetval     = 0;
    WORD        wBuff       = 0,
                wRespCode   = CISO_EXTERNAL_DECLINE_I , /* respuesta default */
				wRvslReason = 0;                        /* motivo de reverso original */
    char        szMsgTypeID[4+1]  = {0},
                szRespCode[8]     = { CISO_EXTERNAL_DECLINE }; /* preasignado error="05" */
    // Constantes de soporte
    const boolean_t    cbCONNECT_BY_TRX = is_true; /* conectarse por cada trx ? */
	/*****************************************************************************/
	GenericLog glExtr;

	glExtr.Open("EXTRACT.TXT");
	/*****************************************************************************/


    /***********************************************************************/
    /* Precondicion = Descomponer mensaje ISO-8583 en campos estructurados */
    wRespCode    = ProcessInit();
    /***********************************************************************/
    /* default de procesamiento del autorizador back end */
    shAuthRespCde = TCP4U_SUCCESS;
    /***********************************************************************/

    /* Comienzo a procesar trx. para FUEGO */
    glLogg.Put("############# PARSEO DE TRX FUEGO-EXTRACT-ATM #############\n");

    /**************************************************/
    /* Completar campos de estructura , modo default  */
    /**************************************************/
    SETFIELDZEROED( extractBuff );
    wRespCode = Completar_MSG_EXTRACT( extractBuff);
    if(CISO_APPROVED_I != wRespCode)
    {
        /* Desconexion x / c/transa */
        glLogg.Put("### Desconexion x/cada transaccion ###\n");
        /* Transa con errores, retornar codigo de error para ISO8583 */
        return (wRespCode);
    };
    /**************************************************/


    /************************************************************************/
    /* Logger de estructura EXTRACT para mejor visualizacion en modo DEBUG */
    /************************************************************************/
    isoMsg.GetMsgTypeId((PBYTE)szMsgTypeID,sizeof(szMsgTypeID));
    glLogg.Put("### Requerimiento ###\n");
    glLogg.Put("Header y long. :<%i> MSG:<%120.120s...>\n", 
		strlen(pszExtractBuff), pszExtractBuff);

	glExtr.DraftPut("%s\r\n",	pszExtractBuff);

    glLogg.Put("Cod.Transaccion:<%6.6s> MsgTyp:<%4.4s>\n",
        extractBuff->cuerpo.stTRAN_CDE.T_CDE, szMsgTypeID);
    glLogg.Put("Entry-Time     :<%-19.19s> \n", extractBuff->cuerpo.ENTRY_TIM );
    glLogg.Put("Monto Online   :<%-12.12s> \n", isoFields.field4.chAmmount );
    glLogg.Put("Monto-1 Extr   :<%-19.19s> \n", extractBuff->cuerpo.AMT_1 );
    glLogg.Put("Monto-2 Extr   :<%-19.19s> \n", extractBuff->cuerpo.AMT_2 );
    glLogg.Put("Monto-3 Extr   :<%-19.19s> \n", extractBuff->cuerpo.AMT_3 );
    glLogg.Put("Moneda         :<%-3.3s> \n", extractBuff->cuerpo.ORIG_CRNCY_CDE );
    glLogg.Put("Cotiz.Comprador:<%-8.8s> \n", extractBuff->cuerpo.uDATOS.stDATOS2.TIP_EXCHA_COMP);
    glLogg.Put("Cotiz.Vendedor :<%-8.8s> \n", extractBuff->cuerpo.TIP_EXCHA_VEND);
    glLogg.Put("Arbitraje      :<%-8.8s> \n", extractBuff->cuerpo.uDATOS.stDATOS2.ARBITRAJE);
    glLogg.Put("Nro.Secuencia  :<%-12.12s> \n", extractBuff->cuerpo.SEQ_NUM );
    glLogg.Put("Fecha Ingreso  :<%-6.6s> \n", extractBuff->cuerpo.POST_DAT );
	glLogg.Put("Cod.Respuesta  :<%-3.3s> \n", extractBuff->cuerpo.RESP_CDE );
    glLogg.Put("Terminal       :<%-16.16s> \n", extractBuff->header.TERM_ID );
    glLogg.Put("Numero Tarjeta :<%-28.28s> \n" , extractBuff->header.CARD_PAN );
    glLogg.Put("Cuenta Desde   :<%-28.28s> \n" , extractBuff->cuerpo.FROM_ACCT );
    glLogg.Put("Cuenta Hasta   :<%-28.28s> \n" , extractBuff->cuerpo.TO_ACCT );
	/**************************************************************************/
    if( isoMsg.IsValidReversal() )    /* Es un Reverso valido ? */
	{
        glLogg.Put("Motivo EXTRACT de reverso:<%2.2s> \n", extractBuff->cuerpo.RVSL_RSN );
		if(!strncmp(extractBuff->cuerpo.RESP_CDE,"000",3))
		{
			strncpy(extractBuff->cuerpo.RESP_CDE+1,extractBuff->cuerpo.RVSL_RSN,2);
			glLogg.Put("Codigo de Respuesta actualizado:<%2.2s> \n", extractBuff->cuerpo.RVSL_RSN );
		}
	}
	/**************************************************************************/
    glLogg.Put("Longitud a enviar : <%i> bytes\n", sizeof(msgEXTRACT));
    /**********************************************************************************/

	if( !isoMsg.IsValidReversal() )    /* NO es un Reverso valido ? */
	{ 
		extractBuff->cuerpo.RESP_CDE[0] = '0'; 
		extractBuff->cuerpo.RESP_CDE[1] = '0';
		extractBuff->cuerpo.RESP_CDE[2] = '0';
	}
	else
		// Guardar motivo de reverso ANTERIOR al procesamiento (reversal reason)
		wRvslReason = antoi(	extractBuff->cuerpo.RESP_CDE , 3 );

#ifdef _SYSTEM_DATANET_TDFUEGO_
    // Algunos back-office no aceptan el ETX (ASCII-03) pero si CR+LF (0D+0A)
    extractBuff->chETX = 0x0D; 
    extractBuff->chEOF = 0x0A;
    extractBuff->chNUL = 0x00;
#endif

    /**********************************************************************************/
    /* enviar y recibir, segun plataforma (distribuida o mainframe)                   */
    /**********************************************************************************/    
    char        Status[7+1]    = {"0000000"},      /* Cod.Rta.   */
                AvailAmmt[12+1]= {"000000000000"}; /* Saldo      */
    char        *ptrBuffer     = chExtractBuff;    /* ptr Buffer */
    int         lLen           = 0;                /* aux longitud */
    CToTcp4u    rpc ; /* remote procedure call, via sockets */

    /****************************************************************************************/
    /* connect to rpc server via socket */
    /****************************************************************************************/
    glLogg.Put("Direccion IP y PORT : <%s>:<%i>\n", 
                strRpcSrvAddress.c_str() , ushRpcSrvPort);
    


	/****************************************************************************************/
    /* fix de inicializacion de libreria TCP4U antes de conectar */
    //pc.getInfoLibrary();
	//glLogg.Put("Inicializando conexion\n");
 //   rpc.setHost( (LPSTR)strRpcSrvAddress.c_str() );
 //   rpc.setUsPort( ushRpcSrvPort );

	//glLogg.Put("Desconectando por las dudas...\n");
	//rpc.disconnect();
	//glLogg.Put("Conectando...\n");
	//rpc.connect();

 //   if( rpc.getIrc() != TCP4U_SUCCESS )
 //   {        
 //       glLogg.Put("### No se pudo conectar al back-office, error <%i> ###\n", rpc.getIrc());        
 //       wRespCode = ( isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() )
 //                       ? CISO_SYSTEM_FAILURE_I
 //                       : CISO_DESTINY_NOT_AVAILABLE_I  ;
 //       return (wRespCode);
 //   }
	//else 
	//	glLogg.Put("CONECTADO\n");
 //   /****************************************************************************************/
 //   /* send trx request */
 //   /****************************************************************************************/
	//glLogg.Put("Enviando...\n");
 //   lLen = strlen( ptrBuffer );
 //   rpc.send( ptrBuffer, lLen);	
 //   if( rpc.getIrc() != TCP4U_SUCCESS )
 //   {        
 //       /* disconnect */
	//	rpc.disconnect();
 //       glLogg.Put("### No se pudo enviar la transferencia ###\n");        
 //       wRespCode = ( isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() )
 //                       ? CISO_SYSTEM_FAILURE_I
 //                       : CISO_DESTINY_NOT_AVAILABLE_I  ;
 //       return (wRespCode);
 //   }
	//else
	//	glLogg.Put("Enviado sin problemas\n");

 //   /****************************************************************************************/
 //   /* receive response */
 //   /****************************************************************************************/
	//glLogg.Put("Recibiendo...\n");
 //   lLen = sizeof( chExtractBuff )-1;
 //   rpc.recv( ptrBuffer, lLen );
	////2007-10-04	MRB	Comprobar que no venga en 000000 la respuesta y que
	////no que haya problemas de conexion
	//if( rpc.getIrc() != TCP4U_SUCCESS )
 //   {        
 //       /* disconnect */
	//	glLogg.Put("ESTADO DE SOCKET: <%i>\n",rpc.getIrc());
 //       rpc.disconnect();
	//	//2007-10-16 mdc
	//	if( !strncmp(ptrBuffer, "000000", 6) )
	//	{
	//		glLogg.Put("### Se recibio misma respuesta que requerimiento: se rechazara. ###\n");        
	//		glLogg.Put(ptrBuffer);
	//		wRespCode = CISO_DESTINY_NOT_AVAILABLE_I;
	//	}
	//	else
	//	{
	//		glLogg.Put("### No se pudo recibir respuesta, se asume impactada (forzada). ###\n");        
	//		//2007-10-04	MRB	
	//		wRespCode = ( isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() )
 //                       ? CISO_SYSTEM_ERROR_I
 //                       : CISO_DESTINY_NOT_AVAILABLE_I  ;
	//	}
 //       return (wRespCode);
 //   };
 //   /****************************************************************************************/
 //   /* disconnect */
 //   /****************************************************************************************/
	//glLogg.Put("Terminando conexion... cerrando\n");
 //   rpc.disconnect();

    /****************************************************************************************/



	/******************************************************************************************/
	// PLAN B
	std::string request;
    std::string response("");
    int resp_leng;
 
	char buffer[1024+1] = {0x00};
    struct sockaddr_in serveraddr;
    int sock;
 	char *ipaddress = new char[strRpcSrvAddress.length() + 1];
	strcpy(ipaddress, strRpcSrvAddress.c_str());

    WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
	    glLogg.Put("### No se pudo inicializar la conexion al back-office ###\n");        
        wRespCode = ( isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() )
                        ? CISO_SYSTEM_FAILURE_I
                        : CISO_DESTINY_NOT_AVAILABLE_I  ;
        return (wRespCode);
	}
	else
	{
		glLogg.Put("WSAStartup inicializado\n");
	}

	SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    struct hostent *host;
    host = gethostbyname(ipaddress);
    SOCKADDR_IN SockAddr;
    SockAddr.sin_port=htons(ushRpcSrvPort);
    SockAddr.sin_family=AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0)
	{
	    glLogg.Put("### No se pudo establecer la conexion al back-office ###\n");        
        wRespCode = ( isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() )
                        ? CISO_SYSTEM_FAILURE_I
                        : CISO_DESTINY_NOT_AVAILABLE_I  ;
        return (wRespCode);
	}
	else
		glLogg.Put("Conexion con server exitosa\n");
	
	request = ptrBuffer;
 
	int iEnviado = send(Socket,request.c_str(), strlen(request.c_str()) ,0);
	if(iEnviado > 0)
		glLogg.Put("Se envio correctamente el siguiente mensaje: <%50s>\n", request.c_str());
	else
		glLogg.Put("Error al enviar al server");

	glLogg.Put("Entrando a ciclo de recepcion\n");

	//char bufferAuxiliar[908];
	char bufferAuxiliar[910];
	if( recv(Socket, bufferAuxiliar, sizeof(bufferAuxiliar), 0) < 0 )
		glLogg.Put("Error en recepcion de datos\n");
	else
		glLogg.Put("Mensaje recibido: <%50s>\n", bufferAuxiliar);
	
	response = std::string(bufferAuxiliar);
	//int nDataLength = 1;
	//int iReintentos = 0;
	//while( nDataLength > 0 )
	//{
	//	if(iReintentos < 3)
	//	{
	//		nDataLength = recv(Socket,buffer,1024,0);
	//		iReintentos++;
	//	}
	//	else
	//		nDataLength = 0;
	//}

 //   while ((nDataLength = recv(Socket,buffer,1024,0)) > 0)
	//{        
 //   }
 //   response = buffer;

	glLogg.Put("Finalizo la recepcion de datos\n");

	char aux[500]={" "};
	int a=closesocket(Socket);
	int b=WSACleanup();
 	delete [] ipaddress;

	sprintf(aux,"devolucion closesocket [%d] WSACleanup [%d] \n", a,b);
	glLogg.Put(aux);

	glLogg.Put("Socket y conexion cerrada\n");

	if(response.length() > 0)
	{
		if( response.compare(0,6,"000000") == 0 )
		{
			glLogg.Put("### Se recibio misma respuesta que requerimiento: se rechazara. ###\n");        
			glLogg.Put(ptrBuffer);
			wRespCode = CISO_DESTINY_NOT_AVAILABLE_I;
			return (wRespCode);
		}
		strcpy(ptrBuffer, response.c_str());
	}
	else
	{
	    glLogg.Put("### Problemas en la transmision/recepcion de datos ###\n");        
        wRespCode = ( isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() )
                        ? CISO_SYSTEM_FAILURE_I
                        : CISO_DESTINY_NOT_AVAILABLE_I  ;
        return (wRespCode);
	}
	/******************************************************************************************/

	

    /*----------------------------------*\
    *  Retorno del programa autorizador  *
    \*----------------------------------*/
	iRetval = strlen( ptrBuffer );

    /***********************************************************************************/
    // Pasar formato FUEGO-EXTRACT a ISO8583-Red Link, la respuesta ISO
    /***********************************************************************************/
    if( iRetval <= 0 )
    {
        glLogg.Put("Error de invocacion\n" );
        //////////////////////////////////////////////////////////////////////////
        // Formatear campo ISO #44, RESP_DATA, saldo y disponible
        FormatField44_RESP_DATA( NULL, NULL , NULL);
        // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
        FormatField122_SECNDRY_RSRVD3_PRVT();
        // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
        FormatField123_SECNDRY_RSRVD4_PRVT();
        // Borrar campos no validos en la rta. : 43,48,52,63
        isoMsg.DeleteField ( 43 );
        isoMsg.DeleteField ( 48 );
        isoMsg.DeleteField ( 52 );
		isoMsg.DeleteField ( 62 );
        isoMsg.DeleteField ( 63 );
        // Borrar campo 120, OPCIONALMENTE PODRIAN NO VENIR EN UN REQUERIMIENTO
        isoMsg.DeleteField ( 120 );
        //////////////////////////////////////////////////////////////////////////
        // Destino no esta disponible, codigo de error para ISO8583, o si es SAF :
        // Declinacion del Sistema, no es respuesta, codigo de error para ISO8583
        wRespCode = ( isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() )
                        ? CISO_SYSTEM_FAILURE_I
                        : CISO_DESTINY_NOT_AVAILABLE_I  ;
        return (wRespCode);
        //////////////////////////////////////////////////////////////////////////
    };
    glLogg.Put("Longitud recibida : <%i> bytes\n", iRetval );

    // Convertir posibles signos NEGATIVOS en el monto del saldo (monto ISO de 12 posiciones)
    // En este caso es el saldo,que se espera en el MONTO 3 del diseno de registro EXTRACT
    psztTran_Amt = (char *)extractBuff->cuerpo.AMT_3;
    Convert_Extract_Ammount( psztTran_Amt, sizeof(extractBuff->cuerpo.AMT_3));

	////////////////////////////////////////////////////////////////////////////////////////	
	// Si habia un signo NEGATIVO, se copia al OFFSET de ajuste 
	psztTran_Amt[ sizeof(extractBuff->cuerpo.AMT_3)-sizeof(TRAN_AMT) ] = psztTran_Amt[0];
	psztTran_Amt += sizeof(extractBuff->cuerpo.AMT_3)-sizeof(TRAN_AMT);
	////////////////////////////////////////////////////////////////////////////////////////


    // Formatear campos 15 y 61 , OPCIONALMENTE PODRIAN NO VENIR EN UN REQUERIMIENTO
    FormatFields15_61();
    // Formatear campo ISO #44, RESP_DATA, saldo y disponible
    FormatField44_RESP_DATA( psztTran_Amt, psztTran_Amt, NULL );
    // Formatear campo ISO #54 de rta., stB24_0210_Def_Data
    // Balance Inquiry, Xfer, Withdrawal, Deposit, Payment
    FormatField54_B24_0210_Def_Data(
        "99", // chWITH_ADV_AVAIL, : contador extracciones o adelantos
        NULL, // chINT_OWE_AUSTRAL,12 : LIBRE DISPONIBILIDAD (CORRALITO) o intereses
        psztTran_Amt, // chCASH_AVAIL,12 : saldo disponible
        NULL, // chMIN_PAYMENT,12
        NULL, // chPAYMENT_DAT,6
        NULL, // chINTEREST_RATE,6
        NULL, // chOWE_DOLAR,10 : ACUMULADOR DE EXTRACCIONES (CORRALITO)
        NULL, // chMIN_PAYMENT_DOLAR,10
        NULL, // chPURCHASE_DOLAR,10
        NULL ); // chCASH_FEE,8 : arancel de usuario
    // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
    // Balance Inquiry, Xfer, Withdrawal, Deposit, Payment
    FormatField122_SECNDRY_RSRVD3_PRVT();
    // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
    // Balance Inquiry, Xfer, Withdrawal, Deposit, Payment
    FormatField123_SECNDRY_RSRVD4_PRVT();
    // Formatear campo ISO #125, SECNDRY_RSRVD6_PRVT, "ACCOUNT INDICATOR"
    // Balance Inquiry, Xfer, Withdrawal, & Deposit, NOT FOR PAYMENT
    if(!isoMsg.IsATMPayment())    // Pagos
        FormatField125_SECNDRY_RSRVD6_PRVT( '1' );
    // Formatear campo ISO #127, SECNDRY_RSRVD8_PRVT
    // Balance Inquiry, Xfer, Withdrawal, & Deposit, NOT FOR PAYMENT
    if(!isoMsg.IsATMPayment())    // Pagos
        FormatField127_SECNDRY_RSRVD8_PRVT();
    ////////////////////////////////////////////////////
    // Borrar campos no validos en la rta. : 43,48,52,63
    isoMsg.DeleteField ( 43 );
    isoMsg.DeleteField ( 48 );
    isoMsg.DeleteField ( 52 );
	isoMsg.DeleteField ( 62 );
    isoMsg.DeleteField ( 63 );
    // Borrar campo 120, OPCIONALMENTE PODRIAN NO VENIR EN UN REQUERIMIENTO
    isoMsg.DeleteField ( 120 );
    ////////////////////////////////////////////////////

    /**********************************************************************************/
    /* Mapear codigo de respuesta FUEGO hacia RED LINK                             */
    /**********************************************************************************/
    COPYSTRING(szRespCode, CISO_EXTERNAL_DECLINE); // default pre-asignado
    // El Cod.Rta. viene informada en el campo RESP_CDE del diseno de registro EXTRACT
    // El flag de procesado viene informada en el campo PREFIJO1 del diseno de registro EXTRACT
	// FIX : los primeros bytes deben estar MODIFICADOS para que se asuma el codigo de respuesta interno.

	// FIX: SE SACA EL CONTROL DE PREFIJO DEL DISEÑO EXTRACT PARA EL BCO INTERFINANZAS
//	if( antoi(extractBuff->chBLOCK_INI_TLF, sizeof(extractBuff->chBLOCK_INI_TLF)) != 0)
//    {
        sprintf( szRespCode, "%02i", antoi(extractBuff->cuerpo.RESP_CDE, 
									    sizeof(extractBuff->cuerpo.RESP_CDE))   );
//    };

    ////////////////////////////////////////////////////////////////
    // Pasar a binario el codigo de respuesta alfanumerico
    wRespCode = antoi(szRespCode,2);
    /////////////////////////////////////////////////////////////////
    // Actualizar mensaje ISO de respuesta
    if(isoMsg.IsValidField( 39 ))
	{
		// Verificar si el ACTUAL motivo de respuesta, es IGUAL al anterior cuando es un reverso
		// tal que si es asi, darlo por APROBADO (bug del backoffice)
		if( isoMsg.IsValidReversal()  
			&& 
			(wRvslReason == wRespCode 
			 ||
			 (CISO_SYSTEM_FAILURE_I == wRespCode && CEXTR_REV_HARDWARE_ERROR_I == wRvslReason)
			)
		  )
		{						
			COPYSTRING( szRespCode, CISO_APPROVED);
			isoMsg.SetField(39,&(wBuff=2),(PBYTE)CISO_APPROVED, 2);
			wRespCode = CISO_APPROVED_I ;
			glLogg.Put("Resultado interno sobre-escrito ante REVERSO HARDWARE ERROR:<%i>\n", wRespCode );
		}
		else
			isoMsg.SetField(39,&(wBuff=2),(PBYTE)szRespCode, 2);
	}
    else
	{
        isoMsg.InsertField(39,&(wBuff=2),(PBYTE)szRespCode, 2);
	}
    ////////////////////////////////////////////////////////////////


    /**********************************************************************************************/
    glLogg.Put("Saldo informado  :<%*.*s>\n", sizeof(TRAN_AMT),
											  sizeof(TRAN_AMT),
											  psztTran_Amt );
    glLogg.Put("Resultado interno:<%3.3s>, EXTRACT:<%3.3s>, ISO para LINK:<%2.2s>\n",
        extractBuff->cuerpo.RESP_CDE, extractBuff->cuerpo.RESP_CDE, szRespCode );
    /**********************************************************************************************/

    /*************************************************************************************/
    /* Verificar el numero de secuencia, si difiere del original                         */
    /* (solo los primeros 6 digitos ya que estan alineados diferentes (izq. y derecha)   */
    /*************************************************************************************/
    wBuff = sizeof(isoFields.field37.chNum);
    if( antofix(extractBuff->cuerpo.SEQ_NUM,wBuff) != antofix(isoFields.field37.chNum,wBuff)
        && CISO_APPROVED_I == wRespCode)
    {
        /* Si es diferente, actualizar la respuesta del mensaje ISO */
        wBuff = 2;
        isoMsg.SetField( 39, &wBuff,(PBYTE)CISO_EXTERNAL_DECLINE,2);
        /* Si es diferente, informar */
        glLogg.Put("Nro. de secuencia difiere del de requerimiento:<%12.12s>\n",
            extractBuff->cuerpo.SEQ_NUM);
    };

    /*************************************************************************************/
    /* Verificar que la cuenta de respuesta sea la misma que la de requerimiento         */
    /*************************************************************************************/
    wBuff = sizeof(isoFields.field102.chNum);
    if( strncmp(extractBuff->cuerpo.FROM_ACCT,isoFields.field102.chNum,wBuff)!=0
        && CISO_APPROVED_I == wRespCode )
    {
        /* Si es diferente, actualizar la respuesta del mensaje ISO */
        wBuff = sizeof(isoFields.field102.chNum);
        isoMsg.SetField( 102, &wBuff,(PBYTE)extractBuff->cuerpo.FROM_ACCT,wBuff);
        /* Si es diferente, informar */
        glLogg.Put("Cuenta de respuesta difiere de la del requerimiento, rta.actualizada\n");
    };
    /*************************************************************************************/


    // Ultima linea, codigo de rta segun FUEGO
    return (wRespCode = antoi(szRespCode,2)); 

};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
