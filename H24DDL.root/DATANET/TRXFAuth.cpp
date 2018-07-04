///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// RPC-EXTRACT-DATANET Resolution Class - AuthorizeTransaction Method
//
//   Clase de resolucion de transacciones DATANET. Metodo Autorizador.
//
// Tarea        Fecha           Autor   Observaciones
// (1.0.1.0)    2007.08.15      mdc     INICIAL tomando como ejemplo al BISEL
// (1.0.1.1)    2007.09.25      mdc     PBYTE a dpsMsg
// (1.0.1.2)    2008.07.03      mdc     FIX:REVERSO MONOBANCO que andaba mal.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Libreria std. string
#include <string.h>
// Libreria std. lib.
#include <stdlib.h>
// Header Type Definitions
#include <qusrinc/typedefs.h>    
// Header Transaction Resolution
#include <qusrinc/trxdnet.h>
// DPS Msg Redefinition
#include <qusrinc/datanet.h>
#include <qusrinc/copymacr.h>
#include <qusrinc/databatc.h>
// DPS BATCH 
#ifndef DPS_B_RECHAZO_CUENTA_CREDITO
#define DPS_B_RECHAZO_CUENTA_CREDITO		90
#endif

// DPS SYSTEM
#include <qusrinc/dpcsys.h>
// Libreria de utilidades estandards
#include <qusrinc/stdiol.h>

#ifdef _SYSTEM_DATANET_TDFUEGO_

// TIMESTAMP-ASCII POR OMISION
#define _DEFAULT_DATE_TIME_STR_ "Jan 01 2007 00:00"
#define _DEFAULT_DATE_STR_      "Jan 01 2007"
// CANTIDAD MAXIMA DE RECLAMOS EXIGIBLES VIA EMAIL/BUZON
#define _DEFAULT_MAX_MSG_NOTIFICATION_     (3)

#define COPYMEM(to, from, cant) memcpy(to, from, cant);

///////////////////////////////////////////////////////////////////////////////////
// Autorizar transaccion con el sistema autorizador del banco
///////////////////////////////////////////////////////////////////////////////////
WORD TrxResBTF::AuthorizeTransaction( long *plExtRetStatus )
{
    // TIMESTAMP de este procesamiento
	time_t tSystemTime   = time(NULL);               // System Time, in seconds since 1-Jan-70
	tm     *ptmLocalTime = localtime(&tSystemTime);  // Local System Time (GMT - h)
	// Campos locales que mapean campos de estructruras DPS
	char szFRNAME [4+1] = {"DNET"};// Nombre nodo emisor, ej. "DNET", char(4)
	char szTONAME [4+1] = {"BSUQ"}; //  Nombre nodo receptor, ej."BSUQ"  CHAR(4)
	char szRECTYPE [3+1] = {"TDE"};// Tipo de registro, TDE = débito TCE = crédito, CHAR(3)
    char chACTIVITY[2] = {"N"};  //  Actividad, CHAR(1)="N"
    char szFILLER1[6+1] = {"000000"}; // RESERVADO
	char szBANDEB [3+1] = {"000"}; //  Código Banco según BCRA	ej."067" Bansud, CHAR(3)
	char szFECSOL [8+3+1] = {_DEFAULT_DATE_STR_}; //  Fecha de compensación, de día o prox. Hábil -aaaammdd-, CHAR(8)
	char szNUMTRA [7+1] = {"0000000"}; //  Número de transferencia,CHAR(7) 
	char szNUMABO [7+1] = {"0000000"}; //  Abonado	, CHAR(7)
	char szTIPOPE [2+1] = {"00"}; //  Tipo de operación -	Ver tabla I - , CHAR(2)
	/* FIX STACK CORRUPTED, VISUAL .NET */
	char szIMPORTE[17+4+1] = {"00000000000000.00"};//  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	/* FIX STACK CORRUPTED, VISUAL .NET */
	char szSUCDEB [4+1] = {"0000"};  //  Sucursal del Banco que tratará el débito, CHAR(4)
	char szNOMSOL [30+1] = {"                             "};  //  Nombre de la cuenta, 
	char szTIPCUEDEB[2+1] = {"00"};//   Tipo de cuenta	- Ver tabla II  - CHAR(2)
	char szNCUECMDEB [2+1] = {"00"}; //  Número de cuenta corto -Interno Datanet- CHAR(2)
	char szNUMCTADEB [17+1] = {"00000000000000000"}; //  Número de cuenta, formato Banco, CHAR(17)
	char szFSENDB [8+3+1+6+1+1+32] = {_DEFAULT_DATE_STR_}; //  Fecha de envío de la transf.  al Banco -Aammdd-, CHAR(6)
	char szHSENDB [8+3+1+6+1+1+32] = {_DEFAULT_DATE_TIME_STR_}; // Hora de envío, Hhmm, CHAR(4)
	char szOPEDB1 [2+1] = {"00"}; //  Identificación del operador #1. Primer autorizante del Banco de db. CHAR(2)
	char szOPEDB2 [2+1] = {"00"}; //  Identificación del operador #2. Segundo autorizante, CHAr(2).
	char szRECHDB [4+1] = {"0000"}; //  Motivo del rechazo del Banco de débito -Ver tabla III - CHAR(4)
	char szBANCRE [3+1] = {"000"}; //  Código de Banco según BCRA, p/credito. CHAR(3)
	char szSUCCRE [4+1] = {"0000"}; //  Sucursal del Banco que tratará el crédito. CHAR(4)				
	char szNOMBEN[30+1] = {"                              "};  //  Nombre de la cuenta a acreditar, VARCHAR(29)
	char szTIPCRE [2+1] = {"00"}; //  Tipo de cuenta -Ver tabla II-, CHAR(2)
	char szCTACRE[17+1] = {"00000000000000000"}; //  Número de cuenta -Formato Banco-, CHAR(17)
	char szFSENCR[8+3+1] = {_DEFAULT_DATE_STR_}; //  Fecha de envío de la transf.  al Banco	 -Aammdd-, CHAR(6)
	char szHSENCR[8+3+1+6+1+1+32] = {_DEFAULT_DATE_TIME_STR_}; //  Hora de envío -Hhmm-, CHAR(4)
	char szOPECR1[2+1] = {"00"}; //  Identificación del operador #1, primer autorizante del Banco de cr. CHAR(2)
	char szOPECR2[2+1] = {"00"}; //  Identificación del operador #2, segundo autorizante, CHAR(2)
	char szRECHCR[4+1] = {"0000"}; //  Motivo del rechazo del Banco de crédito -ver tabla III-, CHAR(4)
	char szOPECON [2+1] = {"00"}; //  Operador que confeccionó la transferencia	, CHAR(2)
	char szOPEAU1 [2+1] = {"00"}; //  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	char szOPEAU2 [2+1] = {"00"}; //  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	char szOPEAU3 [2+1] = {"00"}; //  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	char szFECAUT [8+3+1] = {_DEFAULT_DATE_STR_}; //  Fecha de autorización (envío del pago a la red) aammdd,  CHAR(6)
	char szHORAUT [8+3+1+6+1+1+32] = {_DEFAULT_DATE_TIME_STR_}; //  Hora de autorización (envío del pago a la red) hhmm, CHAR(4)
	char szESTADO [2+1] = {"00"}; //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
    char szESTADO_PREV [2+1] = {"00"}; //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
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
    // Siempre esta PENDIENTE DE ENVIO a menos que el banco credito difiera los creditos    
    WORD  wRespCde      = 0, // Codigo de respuesta hacia nivel de invocacion superior
          wRespCdeBkp   = 0; // Codigo de respuesta anterior 
    WORD  wRetVal       = 0; // Codigo de respuesta desde nivel de invocacion inferior
    long  lRetStatus    = +1L,// Codigo de respuesta BACKEND : End-Of-Table
          lRetStatusBkp = +1L;// Codigo de respuesta BACKEND para reversos : End-Of-Table	
    char  szEstadoCta[5] = {"X"};//Estado inexistente, por omision
    // Codigo de retorno para comparar fechas
    e_settlement_retcode esReqDate  = ESTL_TRANSFERENCIA_OK, // FEC-SOLICITUD vs hoy
                         esSendDate = ESTL_TRANSFERENCIA_OK; // FEC-SOLICITUD vs FEC-ESTADO
    char  szMAC [DNET_MAC_LENGTH+1] = {"            "}; //  Message Authentication Code (MAC), CHAR(12)
    char  szESTADOTRAT[2]           = { _DPS_TRAT_PEND_PROX_DIA_ };
    BOOL  bValidateMAC				= _DPS_CONTROL_MAC_123_; // Validar MAC en reprocesamiento?
    //////////////////////////////////////////////////////////////////////
    // Para mensaje de alta de acuerdo por exceso/caducidad del mismo
	char  szMSG_Acuerdo[20+32+1]    = {"00000000000000.00"};//  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
    char  szMSG_NomOficial[20+32+1] = {"OFICIAL             "}; // Oficial, CHAR(20)
	char  szMSG_Abonado[7+32+1]     = {"0000000"}; //  Abonado	, CHAR(7)
	// FIX VISUAL .NET "STACK CORRUPTED"
	char  szMSG_CtaDebito[24+512+1]         = {"000000000000000         "}; //  Número de cuenta, formato Banco, CHAR(24)
    char  szMSG_NomCompletoOficial[30+64+1] = {"NOMBRE DEL OFICIAL            "}; // Nombre Completo, CHAR(30)
    char  szMSG_Importe[20+64+1]            = {"00000000000000.00"}; // Importe TEF
	// FIX VISUAL .NET "STACK CORRUPTED"
    char  szMSG_NomSolicitante[30+512+1]    = {"NOMBRE DEL SOLICITANTE        "}; // Nombre del Solicitante, CHAR(30)
	// FIX VISUAL .NET stack corrupted
    char  szMSG_QReclamos[2+256+1]= {"00"}; // Cantidad de reclamos solicitados
	// FIX VISUAL .NET stack corrupted
    char  szMSG_MonedaCta[13+256+1]  = {"DOLARES/PESOS"}; // Moneda de la Cuenta
    char  szMSG_TIMESTAMP[20+10+1]  = {"01/01/2001 00:00:00"}; // Timestamp "dd/mm/yyyy hh:mm:ss"
    // Indicativo de que era BANCO INEXISTENTE y desde ahora esta OK
    boolean_t bAPROBADA_FALTAN_DATOS_EN_OPERACION = is_false;
    // Indicativo de que era VALOR AL COBRO antes de estar APROBADA-RECHAZADA
    boolean_t bESTADO_VALOR_AL_COBRO = is_false;
    // Mascara de formateo de autorizacion
    const char cszAUTH_MSG_FORMAT_MASK [] = {
                    "Subject: DATANET [%s] CUENTA [%s]\r\n"
                    "Date: %s \r\n\r\n"
                    "DATOS:\r\n"
                    "TRANSFERENCIA         [%s]\r\n"
                    "CUENTA                [%s]\r\n"
                    "NOMBRE CUENTA         [%s]\r\n\r\n"
                    "NUMERO DE LOTE        [%i]\r\n"
                    "IMPORTE TRANSFERENCIA [%s]\r\n"
                    "SOBREGIRO             [%s]\r\n"
                    "MONEDA                [%s]\r\n"
                    "OFICIAL               [%s] [%s]\r\n"
                    "CONTADOR DE ENVIO     [%i]\r\n\r\n"
                    "DEBITO PENDIENTE DE AUTORIZACION.\r\n"
                    "FAVOR DE GESTIONAR ACUERDO EN CUENTA.\r\n"
                    "FECHA Y HORA DE ESTE MENSAJE [%s]\r\n"
                    };
    const char cszAUTH_MSG_NO_OFFICER[]      = {"NO SE DETECTO OFICIAL"};
    const char cszAUTH_MSG_OFFICER_ALERT[]   = {"ATENCION"};
    const char cszAUTH_MSG_DEFAULT_OFFICER[] = {"DEFAULT"};
    char  szMSG_EMail[256+1]  = {"administradordelcorreo@dominioinstitucional.com.ar"}; 
    ///////////////////////////////////////////////////////////////////////////////////
    BOOL  bCheckCutoverInCredit = FALSE; // Chequear hora de corte en CREDITOS?
    BOOL  bCheckCutoverInDebit  = FALSE;  // Chequear hora de corte en DEBITOS?
    ///////////////////////////////////////////////////////////////////////////////////
    // Handle de archivo de impresion de parametros hacia B. Datos
	FILE        *hFile                = NULL;
    const char  bDEBUG_PRINT_PARAMS   = TRUE;        
    ////////////////////////////////////////////////////////////////////
    char  szCODRESP[20]   = {"0"}; // Codigo Respuesta 
	char  szCODUSUOFI[20] = {"0"}; // Codigo Usuario Oficial 
    char  szACCION[10]    = {"0"}; // Accion
	////////////////////////////////////////////////////////////////////
	short iNx             = _DPS_TRAT_PENDIENTE_C_;
    ////////////////////////////////////////////////////////////////////
    
    // Precondicion : conexion a base de datos BACKEND(CONTINUUS ON/2)
	if( NULL == ptrBackOffice )
    {
        // Copia de resultado interno de error BACKEND(CONTINUUS ON/2) si hubiera
        if(plExtRetStatus) (*plExtRetStatus) = lRetStatus;
		return (wRespCde = DPS_FUERZA_MAYOR);
    }

    // Adecuacion de parametros segun ultimo registro leido
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

    // Navegar todos los ESTADO_TRATAMIENTO factibles de ser autorizados
    // o sea , todos aquellos menor a PROCESADO='9' y menor a DIFERIDO='8'
	// Recuperar la 1er. transaccion desde TEF ONLINE segun sea menor a ESTADO_TRATAMIENTO

 wRespCde        = DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED;
 iNx             = _DPS_TRAT_PENDIENTE_C_ ; 
 szESTADOTRAT[0] = iNx ;
 ptrBackOffice->RetrieveTransaction(  
		    szFRNAME ,// Nombre nodo emisor, ej. "DNET", char(4)
		    szTONAME , //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
		    szRECTYPE ,// Tipo de registro, TDE = débito TCE = crédito, CHAR(3)
            chACTIVITY, // Actividad
            szFILLER1, // Reservado DATANET
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
		    szESTADOTRAT ,      // Estado tratamiento en "0","1","2" u "8"
            szACCION ); // Accion del Sistema
	if(ptrBackOffice->GetStatus()!=0)
		// NO EXISTE LA CONDICION BUSCADA :
		// El error "FUERZA MAYOR" indica que no hay mas registros para autorizar.
		// Postcondicion : NO existe registros, FUERZA MAYOR.
		wRespCde = DPS_FUERZA_MAYOR;
	else
		// EXISTE LA CONDICION BUSCADA
		wRespCde = DPS_APROBADA_SIN_RECHAZO;

    // EXIT CONDITION
    if( wRespCde != DPS_APROBADA_SIN_RECHAZO)
		return (wRespCde = DPS_FUERZA_MAYOR);
	    

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
    // Guardar una copia segura del estado actual para comparaciones ante cambios del mismo
    strcpy(szESTADO_PREV,szESTADO);

    //////////////////////////////////////////////////////////////////////////////////
    // FECHA DE SOLICITUD Y ENVIO , como indicadores IGUAL-MAYOR-MENOR a hoy
    //////////////////////////////////////////////////////////////////////////////////
    esReqDate = esSendDate = ESTL_TRANSFERENCIA_OK;
    ValidatePostingDate( &esReqDate , &esSendDate , is_true );

    ////////////////////////////////////////////////////////////////////////////////////
	// Cada mensaje loggeado, tanto débito como crédito, tiene como control, la 
	// re-verificación de la clave MAC. Esta deberá ser recalculada con el algoritmo 
	// MAC1, en el caso de un crédito con ESTADO = "00", o MAC2 para el resto.
	// Si la clave calculada no se corresponde con la recibida, se procederá a rechazar
	// la operación cambiando el ESTADO a "70" u "80" e integrando el campo RECHDB o 
	// RECHCR con el valor "0033" según corresponda. 
	// Si no se puede calcular la clave, se demora con estado "20" .	
    // CONTROL 2-bis (MAC): Validar MAC de entrada, MAC 1 o 3 segun sea CREDITO o no
    //////////////////////////////////////////////////////////////////////////////////////
    // Backup de Estado ACTUAL de la transferencia
    COPYFIELD(szESTADO_PREV,  dpsMsg.input.data.ESTADO );	    
    // Solo para DEMORADAS, aplicar el estado ORIGINAL para validar MAC
    if( antoi(dpsMsg.input.data.ESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO )
    {
        // Si es DEBITO EJECUTADO y DEMORADA, el estado original era ENVIADA POR ABONADO
        if( dpsMsg.GetRecType() == DPS_DEBITO_EJECUTADO )
        {
            COPYFIELD(dpsMsg.input.data.ESTADO , _DPS_ENVIADO_POR_EL_ABONADO_ );
        }
        // Sino si es CREDITO EJECUTADO y DEMORADA, el estado original era VALOR AL COBRO
        else if( dpsMsg.GetRecType() == DPS_CREDITO_EJECUTADO )
        {
            COPYFIELD(dpsMsg.input.data.ESTADO , _DPS_VALOR_AL_COBRO_ );
            bESTADO_VALOR_AL_COBRO = is_true;
        };
    }
    else if(antoi(szESTADO,2)==DPS_VALOR_AL_COBRO)
    {
        bESTADO_VALOR_AL_COBRO = is_true;
    };

    /////////////////////////////////////////////////////////////////////////////
    if( dpsMsg.IsDPSDebitTransfer() && antoi(szESTADO,2)==DPS_RECHAZO_DEL_BANCO_DE_CREDITO) 
        wRespCde = ValidateMAC3();
    else if( dpsMsg.IsDPSCreditTransfer() ) 
        wRespCde = ValidateMAC2();
    else                                    
        wRespCde = ValidateMAC1();        
    // Restore de Estado ACTUAL de la transferencia
    COPYFIELD(dpsMsg.input.data.ESTADO , szESTADO_PREV);	
    //////////////////////////////////////////////////////////////////////////////////////
    if( DPS_APROBADA_SIN_RECHAZO != wRespCde && bValidateMAC ) 
    {
        // Excepto reverso por RIESGO RED EXCEDIDO que no viene el CODIGO MAC
        if( antoi(dpsMsg.input.data.ESTADO,2) == DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED  )
        {
            wRespCde = DPS_APROBADA_SIN_RECHAZO;
            ZEROESTRING(szRECHDB );//  Motivo de rechazo DB en cero
            ZEROESTRING(szRECHCR );//  Motivo de rechazo CR en cero
        }
        // Excepto que sea FACTURACION, porque no se calcula el MAC-1 por definicion 
        else if( dpsMsg.GetOperType(NULL) == DPS_FACTURACION )
        {
            wRespCde = DPS_APROBADA_SIN_RECHAZO;
            ZEROESTRING(szRECHDB );//  Motivo de rechazo DB en cero
            ZEROESTRING(szRECHCR );//  Motivo de rechazo CR en cero
        }
        // Excepto EJECUTADA que ya viene en un estado definitivo
        else if( antoi(dpsMsg.input.data.ESTADO,2) == DPS_EJECUTADA 
                &&
                DPS_FALTAN_DATOS_EN_DETALLE_OPERACION != wRespCde)
        {
            //  Motivo de rechazo debito / credito
            dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer() );
            //  Recuperar motivos en formato ASCII
            dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
            wRespCde = DPS_APROBADA_SIN_RECHAZO;
        }
        // Excepto RECHAZO BANCO CREDITO
        else if( antoi(dpsMsg.input.data.ESTADO,2) == DPS_RECHAZO_DEL_BANCO_DE_CREDITO )
        {
            //  Motivo de rechazo debito / credito
            if( antoi(szRECHDB,2)==0 && antoi(szRECHCR,2)==0) //  Motivo de rechazo CR-DB
            {
                wRespCde = DPS_APROBADA_SIN_RECHAZO;
                ZEROESTRING(szRECHDB );//  Motivo de rechazo DB en cero
                ZEROESTRING(szRECHCR );//  Motivo de rechazo CR en cero
            }
            //////////////////////////////////////////////////////////////////////
            //  Motivo de rechazo debito / credito
            else if( antoi(szRECHDB,2)==0 && antoi(szRECHCR,2)!=0) //  Motivo de rechazo CR-DB
            {
                wRespCde = DPS_APROBADA_SIN_RECHAZO;
                ZEROESTRING(szRECHDB );//  Motivo de rechazo DB en cero
                ZEROESTRING(szRECHCR );//  Motivo de rechazo CR en cero
            };
            //////////////////////////////////////////////////////////////////////
        }
        else if( antoi(dpsMsg.input.data.ESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO
                &&
                DPS_FALTAN_DATOS_EN_DETALLE_OPERACION == wRespCde )
        {
            //  Motivo de rechazo debito / credito
            dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer() );
            //  Recuperar motivos en formato ASCII
            dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
            if( dpsMsg.IsResponseCodeInFinalState() && dpsMsg.IsDPSCreditTransfer() )
            {
                wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED ;
            }
            if( _DPS_TRAT_PENDIENTE_C_ != szESTADOTRAT[0]  )
            {
                szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_ ;
            }
            /////////////////////////////////////////////////////////////////////////////////
            // Si es DEBITO EJECUTADO y DEMORADA, el estado original era ENVIADA POR ABONADO
            if( dpsMsg.GetRecType() == DPS_DEBITO_EJECUTADO )
            {
                dpsMsg.SetStatusCode( DPS_ENVIADO_POR_EL_ABONADO_ );
            }
            // Sino si es CREDITO EJECUTADO y DEMORADA, el estado original era VALOR AL COBRO
            else if( dpsMsg.GetRecType() == DPS_CREDITO_EJECUTADO )
            {
                dpsMsg.SetStatusCode( DPS_VALOR_AL_COBRO );
            };
            /////////////////////////////////////////////////////////////////////////////////
            // Envio pendiente? No, no enviar por ausencia de banco
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
        }
        else if( antoi(dpsMsg.input.data.ESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO
                &&
                DPS_FALTAN_DATOS_EN_DETALLE_OPERACION != wRespCde )
        {
            //  Motivo de rechazo debito / credito
            dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer() );
            //  Recuperar motivos en formato ASCII
            dpsMsg.GetCauseCode(szRECHCR, szRECHDB );            
            // Pasa a RECHAZADA
            wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					    ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					    : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
            // Envio de la respuesta, esta pendiente? Si
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
            // Procesada, MAC erroneo
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
            /////////////////////////////////////////////////////////////////////
            // FEC-SOLICITUD vs FEC-ENVIO : difieren a hoy y respuesta pendiente?
            VerifySendFlag( esReqDate,esSendDate,bESTADO_VALOR_AL_COBRO);
            /////////////////////////////////////////////////////////////////////
        }
        else        
        {
            //  Motivo de rechazo debito / credito
            dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer() );
            //  Recuperar motivos en formato ASCII
            dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
            // Los estados definitivos, no se deben responder
            if( dpsMsg.IsResponseCodeInFinalState() && dpsMsg.IsDPSCreditTransfer() )
            {
                wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED ;
            }
            // Por ausencia del ID del BANCO en la base de datos, demorarla
            else if( DPS_FALTAN_DATOS_EN_DETALLE_OPERACION == wRespCde )
            {
                // Pasa a DEMORADA
                wRespCde = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO;
                // Envio de la respuesta, esta pendiente? No, no enviar x/ausencia de ID.
                szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
            }
            // Sino, rechazarla
            else
            {
                // Pasa a RECHAZADA
                wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					        ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					        : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
                // Envio de la respuesta, esta pendiente? Si
                szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
            };
            // Procesada, MAC erroneo
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
        };/* end-if ESTADO */
    }
    else
    {
       // Ok, sin errores de MAC
       wRespCde = DPS_APROBADA_SIN_RECHAZO;
       // Si es BANCO INEXISTENTE, y ahora ya no lo es mas, indicarlo.
       wRetVal = antoi(szRECHDB,4);
       if(DPS_FALTAN_DATOS_EN_DETALLE_OPERACION != wRetVal)
           wRetVal = antoi(szRECHCR,4);
       // Si era BANCO INEXISTENTE, y ahora esta aprobada, volver al ESTADO ORIGINAL
       // que puede ser '00' para debitos y '40','60','70' para creditos:
       if(DPS_FALTAN_DATOS_EN_DETALLE_OPERACION == wRetVal)
       {
           bAPROBADA_FALTAN_DATOS_EN_OPERACION = is_true;
           // Si estaba DEMORADA y era BANCO INEXISTENTE, recomponer el estado original
           if( antoi(dpsMsg.input.data.ESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO )
           {
               // Si es DEBITO EJECUTADO, el estado original era ENVIADA POR EL ABONADO
               if( dpsMsg.GetRecType() == DPS_DEBITO_EJECUTADO )
               {
                  COPYFIELD(dpsMsg.input.data.ESTADO , _DPS_ENVIADO_POR_EL_ABONADO_ );
               }
               // Sino si es CREDITO EJECUTADO y DEMORADA, el estado original era VALOR AL COBRO
               else if( dpsMsg.GetRecType() == DPS_CREDITO_EJECUTADO )
               {
                  COPYFIELD(dpsMsg.input.data.ESTADO , _DPS_VALOR_AL_COBRO_ );             
                  bESTADO_VALOR_AL_COBRO = is_true;
               };
               // Todo otro estado , no se modifico originalmente, y se mantiene
           };/*end-if-ESTADO=DEMORADA*/
           COPYFIELD(szESTADO,  dpsMsg.input.data.ESTADO );
       }
       else
           // No era Banco Inexistente
           bAPROBADA_FALTAN_DATOS_EN_OPERACION = is_false;
    };

	/////////////////////////////////////////////////////////////////////////////////////
    // Prevalidar con controles por tipo de transaccion
    if(DPS_APROBADA_SIN_RECHAZO == wRespCde)
    {
	    if(dpsMsg.IsValidReversal())	     // Es reversa generica para cualquier transaccion?
		    wRespCde = ProcessReversal(is_true,is_true); 
	    else if(dpsMsg.IsDPSDebitTransfer()) // Transferencia cuentas relacionadas
		    wRespCde = ProcessDebitNote(is_true,is_true);
	    else if(dpsMsg.IsDPSCreditTransfer())	        // Transferencia cuentas relacionadas
		    wRespCde = ProcessCreditNote(is_true,is_true);
	    else if(dpsMsg.IsDPSCreditAndDebitTransfer())	// Transferencia cuentas relacionadas
		    wRespCde = ProcessCreditDebitNote(is_true,is_true);
	    else if(dpsMsg.IsDPSAccountVerification())	    // Verificacion cuenta destino
		    wRespCde = ProcessAccountVerification(is_true,is_true);
	    else if(dpsMsg.IsDPSOtherMessages())            // Otros mensajes al banco
		    wRespCde = ProcessOtherMessages(is_true,is_true);
	    else
		    // Todo lo demas NO SE RECONOCE, SON "TRANSACCIONES INVALIDAS"
		    wRespCde = DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
        // Actualizar motivos en formato ASCII, si hubieren
        dpsMsg.GetCauseCode(szRECHCR, szRECHDB );

    };
    /////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////
	// Si la cuenta debito o crédito estuviese cerrada o en vías de cierre o embargada
	// o en algún estado no operativo, el banco rechazara el movimiento cambiando a 
	// ESTADO a "70" u "80" e integrando el campo RECHDB o RECHCR con el valor "0011",
	// o "0003", o desde "0005" al "0007", según corresponda.
	// CONTROL 4-bis: Validar cuentas en estado valido
	if ( DPS_APROBADA_SIN_RECHAZO == wRespCde )
    {
        wRespCde = ValidateAccountStatus(szEstadoCta);
        if (wRespCde != DPS_APROBADA_SIN_RECHAZO)
	    {
            bESTADO_VALOR_AL_COBRO = (antoi(szESTADO,2)==DPS_VALOR_AL_COBRO)
                                            ? is_true
                                            : is_false;
            //////////////////////////////////////////////////////////////////////////////
            // Verificar caso de EXCEPCION para creditos con cuentas bloqueadas, estado 40
            if( is_true == bESTADO_VALOR_AL_COBRO &&
                DPS_CUENTA_BLOQUEADA == wRespCde   )
            {            
		        // Formar respuesta con diferentes motivos de rechazo
		        dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer() ); 
                wRespCde = DPS_APROBADA_SIN_RECHAZO;
            }
            //////////////////////////////////////////////////////////////////////////////
            else
            {
		        // Formar respuesta con diferentes motivos de rechazo
		        dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer() );
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
		        wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					            ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					            : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;        
                //////////////////////////////////////////////////////////////////////////////
                // FEC-SOLICITUD vs FEC-ENVIO : difieren a hoy y respuesta pendiente?
                bESTADO_VALOR_AL_COBRO = (antoi(szESTADO,2)==DPS_VALOR_AL_COBRO)
                                                ? is_true
                                                : is_false;
                if(VerifySendFlag( esReqDate,esSendDate,bESTADO_VALOR_AL_COBRO))
                    szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ; //  Procesada
                //////////////////////////////////////////////////////////////////////////////

                //////////////////////////////////////////////////////////////////////////
                // Verificar que para DOLARES jamas se procesen diferidas por norma BCRA
                // aunque la cuenta este en estado invalido (CONTROL REITERADO)
                if( ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate 
                    &&
                   (antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CUENTA_CORRIENTE_USD ||
                    antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CAJA_DE_AHORROS_USD)
                    &&
                   (antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CUENTA_CORRIENTE_USD ||
                    antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CAJA_DE_AHORROS_USD))
                {
                    // Diferimiento, no se tratan DOLARES diferidos 
                    wRespCde = DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED;
                    szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
                    if( _DPS_TRAT_PENDIENTE_C_ == szESTADOTRAT[0]  )
                    {
                        szESTADOTRAT[0] = _DPS_TRAT_PEND_PROX_DIA_C_ ;
                    }
                };
                //////////////////////////////////////////////////////////////////////////
            };//end-if-excepcion-valores-al-cobro
	    }
        else
            wRespCde = DPS_APROBADA_SIN_RECHAZO;
    };/* end-if-APROBADA_SIN_RECHAZO */

    /////////////////////////////////////////////////////////////////////////
    // CONTROL 5 : No se procesan operaciones bimonetarias. 
    // Solo se aceptan debito y credito en DOLARES monobanco,
    // o creditos DOLARES de terceros si ambas cuentas tambien son DOLARES.
    /////////////////////////////////////////////////////////////////////////
    if( DPS_APROBADA_SIN_RECHAZO == wRespCde 
        &&
       ((antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CUENTA_CORRIENTE_ARG ||
         antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CAJA_DE_AHORROS_ARG)
          &&
        (antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CUENTA_CORRIENTE_USD ||
         antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CAJA_DE_AHORROS_USD))
        ||
       ((antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CUENTA_CORRIENTE_USD ||
         antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CAJA_DE_AHORROS_USD)
          &&
        (antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CUENTA_CORRIENTE_ARG ||
         antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CAJA_DE_AHORROS_ARG))
       )
    {
       // Operacion no habilitada 
       wRespCde = DPS_OPERACION_NO_HABILITADA_EN_BANCO;        
    };
    
    //////////////////////////////////////////////////////////////////////////////////
    // CONTROL 6 : No se procesan operaciones diferidas DOLARES por norma BCRA.
    // Actualizar el estado pendiente de ENVIO segun corresponda a diferidas o no
    //////////////////////////////////////////////////////////////////////////////////
    if( ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate )
    {       
        // Verificar que para DOLARES jamas se procesen diferidas por norma BCRA
        if((antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CUENTA_CORRIENTE_USD ||
            antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CAJA_DE_AHORROS_USD)
            &&
           (antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CUENTA_CORRIENTE_USD ||
            antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CAJA_DE_AHORROS_USD))
        {
            // Diferimiento, no se tratan DOLARES diferidos 
            wRespCde = DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED;
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
            if( _DPS_TRAT_PENDIENTE_C_ == szESTADOTRAT[0]  )
            {
                szESTADOTRAT[0] = _DPS_TRAT_PEND_PROX_DIA_C_ ;
            }
        }
        // ?Trata CREDITO diferido ? "SI", entonces enviar la respuesta
        else if ( 'S' == chDIFCRE[0] 
                  && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) )
        {
            // Marca de pendiente de envio en general 
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
        }
        // ?Trata CREDITO diferido ? "NO", entonces no enviar la respuesta
        else if ( 'S' != chDIFCRE[0] 
                  && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) )
        {
            // Marca de no enviar en general 
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
        }
        ////////////////////////////////////////////////////////////
        // Creditos Ejecutados se pasan a mañana, si son diferidos
        ////////////////////////////////////////////////////////////
        else if( strcmp(szRECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO) == 0 
                 &&
                 antoi(szESTADO,2) == DPS_EJECUTADA )    
        {
            // No se responden creditos ejecutados
            wRespCde = DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED;
            // Marca de NO envio 
            szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;
            // Tratamiento diferido ahora es procesado, compensa el bacth de diferidas.
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
            ////////////////////////////////////////////////////
            // Pero si la hora de corta ya vencio....
            if( IsCutoverTimeElapsed() && bCheckCutoverInCredit ) 
                // Tratamiento diferido
                szESTADOTRAT[0] = _DPS_TRAT_PEND_PROX_DIA_C_ ;
            ////////////////////////////////////////////////////

        }
        ////////////////////////////////////////////////////////////
        // Valores al Cobro se pasan a mañana,si son diferidos
        ////////////////////////////////////////////////////////////
        else if( strcmp(szRECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO) == 0 
                 &&
                 antoi(szESTADO,2) == DPS_VALOR_AL_COBRO )    
        {
            // Se debe responder como creditos ejecutado aunque no se aplique,
            // ya que es el batch de diferidas quien monetiza el credito
            wRespCde = DPS_APROBADA_SIN_RECHAZO;
            // Marca de envio pendiente
            szPENDENVIO[0]  = _DPS_PEND_ENV_DEFAULT_C_;
            // Tratamiento diferido ahora es procesado, compensa el bacth de diferidas.
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;  
            ////////////////////////////////////////////////////
            // Pero si la hora de corta ya vencio....
            if( IsCutoverTimeElapsed() && bCheckCutoverInCredit ) 
                // Tratamiento diferido
                szESTADOTRAT[0] = _DPS_TRAT_PEND_PROX_DIA_C_ ;
            ////////////////////////////////////////////////////
        }
        ////////////////////////////////////////////////////////////////////////
        // ?Trata DEBITOS-CREDITOS diferidos, despues de la hora de corte de red? 
        else if ( IS_ON_OUR_BANKCODE(dpsMsg.input.data.BANDEB)
                  &&
                  IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) 
                ) 
        {
            ////////////////////////////////////////////////////
            // Solo si la hora de corta ya vencio....
            if( IsCutoverTimeElapsed() && bCheckCutoverInDebit ) 
                // Tratamiento diferido
                szESTADOTRAT[0] = _DPS_TRAT_PEND_PROX_DIA_C_ ;
            ////////////////////////////////////////////////////
        }
        ////////////////////////////////////////////////////////////////////////
        // Por omision
        else
        {
            // Marca de pendiente por omision
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
        };
    }
    // Fecha anterior a hoy
    else if( ESTL_FECHA_DE_SOLICITUD_MENOR_A_FECHA_DEL_DIA == esReqDate )
    {       
        // No se envian respuestas para dias anteriores 
        wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					        ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					        : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
        //  Motivo de rechazo debito - credito
        dpsMsg.SetCauseCode( DPS_SISTEMA_NO_DISPONIBLE,dpsMsg.IsDPSCreditTransfer());
        //  Recuperar motivos en formato ASCII
        dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
        // Marca de NO enviar en general 
        szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;  
    };// end-if-FECHA_SOLICITUD

    
    //////////////////////////////////////////////////////////////////////////////////
    // Autorizar en si mismo, como CREDITO o DEBITO o REVERSO    
    //////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////
    // Hubo error? No procesar, actualizar si corresponde, el estado envio y 
    // tratamiento:
    //////////////////////////////////////////////////////////////////////////////////
    if( DPS_APROBADA_SIN_RECHAZO != wRespCde )
    {
       /////////////////////////////////////////////////////////////
       // Falló el MAC-1 de autorizacion, no ejecutar bloqueos, 
       // creditos ni reversos. O No existen respuestas hacia la red.
       // O es Banco Inexistente. O son Otros Fallos.
       /////////////////////////////////////////////////////////////
       wRetVal = antoi(szRECHDB,4);
       if(DPS_APROBADA_SIN_RECHAZO == wRetVal)
           wRetVal = antoi(szRECHCR,4);
       // Es BANCO INEXISTENTE y esta en ESTADO FINAL-DEMORADO-NO_RESPONDER, 
       // y esta pendiente de tratamiento, y no se envio, 
       // pues reprocesar.
       if ((DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO      == wRespCde ||
            DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED   == wRespCde )
           && 
           // Pendiente
           szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ 
           &&
           // Indicativo de Banco Inexistente-Abonado Inexistente
           is_true == bAPROBADA_FALTAN_DATOS_EN_OPERACION 
           &&
           // Banco Inexistente-Abonado Inexistente
           DPS_FALTAN_DATOS_EN_DETALLE_OPERACION == wRetVal)
       {
           // Marcar para procesar como por 1ra vez
           wRespCde       = DPS_APROBADA_SIN_RECHAZO;
           szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_ ;
           /////////////////////////////////////////////////////////////////////
           // FEC-SOLICITUD vs FEC-ENVIO : difieren a hoy y respuesta pendiente?
           VerifySendFlag( esReqDate,esSendDate,bESTADO_VALOR_AL_COBRO);
           /////////////////////////////////////////////////////////////////////
       }/* end-if-FALTAN_DATOS_DETALLE */
       else if ((DPS_RECHAZO_DEL_BANCO_DE_DEBITO  == wRespCde ||
                 DPS_RECHAZO_DEL_BANCO_DE_CREDITO == wRespCde )
                && 
                // Pendiente
                szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ 
                &&
                // Indicativo de Banco Inexistente-Abonado Inexistente
                is_true == bAPROBADA_FALTAN_DATOS_EN_OPERACION  )
       {             
           // Enviar rta.
           if(szPENDENVIO[0] != _DPS_PEND_ENV_DIFERIDA_C_)
                szPENDENVIO[0]  = _DPS_PEND_ENV_DEFAULT_C_ ;
           // Procesada-Ejecutada                       
           szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_;
           /////////////////////////////////////////////////////////////////////
           // FEC-SOLICITUD vs FEC-ENVIO : difieren a hoy y respuesta pendiente?
           VerifySendFlag( esReqDate,esSendDate,bESTADO_VALOR_AL_COBRO);
           /////////////////////////////////////////////////////////////////////
       } /* end-if-FALTAN_DATOS_DETALLE */
       /////////////////////////////////////////////////////////
       else if ((DPS_RECHAZO_DEL_BANCO_DE_DEBITO  == wRespCde ||
                 DPS_RECHAZO_DEL_BANCO_DE_CREDITO == wRespCde )
                && 
                // Pendiente
                szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_  )
       {
           // Enviar rta.                                      
           if(szPENDENVIO[0] != _DPS_PEND_ENV_DIFERIDA_C_)
                szPENDENVIO[0]  = _DPS_PEND_ENV_DEFAULT_C_ ;
           // Procesada-Ejecutada
           szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_;
           // FEC-SOLICITUD vs FEC-ENVIO : difieren a hoy y respuesta pendiente?
           VerifySendFlag( esReqDate,esSendDate,bESTADO_VALOR_AL_COBRO);
       };/* end-if */
       /////////////////////////////////////////////////////////
    }; /* end-if-NO_APROBADA */

	// FIX BATCH RECORD ( original y backup , 2 copias )
	DPS_i_batch::data_tag recBATCH[2];
	recBATCH[0].TIPOREG = DPS_B_TIPO_REGISTRO_BODY;
	COPYMEM( recBATCH[0].BANDEB , 
			 this->dpsMsg.input.data.BANDEB, 
			 min( sizeof(DPS_i_batch::data_tag) , sizeof(dpsMsg.input.data)) );
	// FIX BATCH RECORD

    //////////////////////////////////////////////////////////////////////////////////
    // ERROR DE PROCESAMIENTO, no autorizar.
    //////////////////////////////////////////////////////////////////////////////////
    if( DPS_APROBADA_SIN_RECHAZO != wRespCde )
    {
       ///////////////////////////////////////////////////////////////////////////////
       // Falló el MAC-1 de autorizacion, no ejecutar bloqueos ni creditos ni reversos.
       // No existen respuestas hacia la red.
       // Otros fallos.
       ///////////////////////////////////////////////////////////////////////////////
    }
    // DEBITO EJECUTADO, o 
    // DEBITO REVERSADO por RIESGO DE RED, y tratamiento AUTORIZADO (reintentar), o
    // DEBITO DEMORADO  por falta de fondos,y tratamiento AUTORIZADO (reintentar)
    else if ((strcmp(szRECTYPE,DPS_RECTYPE_DEBITO_EJECUTADO) == 0
              &&
              antoi(szESTADO,2) == DPS_ENVIADO_POR_EL_ABONADO_ )
              ||
              // DEBITO REVERSADO por RIESGO DE RED, y tratamiento AUTORIZADO-RECHAZADO
             (strcmp(szRECTYPE,DPS_RECTYPE_DEBITO_EJECUTADO) == 0 
               &&    
              antoi(szESTADO,2) == DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED  
              &&
              (szESTADOTRAT[0] == _DPS_TRAT_SOBREG_APROB_C_ 
               ||
               szESTADOTRAT[0] == _DPS_TRAT_SOBREG_RECHA_C_) ) 
              ||
              // DEMORADA y tratamiento PENDIENTE,AUTORIZADO o RECHAZADO
             (antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO 
              &&
              (szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ 
               ||
               szESTADOTRAT[0] == _DPS_TRAT_SOBREG_APROB_C_ 
               ||
               szESTADOTRAT[0] == _DPS_TRAT_SOBREG_RECHA_C_) )
            )
    {
        // Si originalmente fue reversado por EXCEDE RIESGO RED, o fue DEMORADO,
        // y esta indicado como APROBADO-FORZADO o RECHAZADO, pues hacer o no el debito.
        if( (antoi(szESTADO,2) == DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED  
            ||
            antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO)
            &&
            _DPS_TRAT_SOBREG_RECHA_C_ == szESTADOTRAT[0] )
        {
            // Se rechazo el DEMORADO-REVERSO manualmente, por Banca Electronica
            wRespCde = DPS_CTA_NO_DEBITABLE_EN_TRANSFERENCIA ;
        }
        else if( (antoi(szESTADO,2) == DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED  
                ||
                antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO)  
                &&
                _DPS_TRAT_SOBREG_APROB_C_ == szESTADOTRAT[0]  )
        {
            // Se aprobo el REVERSO, se forzo el DEMORADO para que se apruebe
            wRespCde = DPS_APROBADA_SIN_RECHAZO ;
        };

        //////////////////////////////////////////////////////////////////////////////
        // Si es ENVIADO POR EL ABONADO y esta ok,               
        // Si es una DEMORADA rechazada manualmente, actaulizar el LOTE
        // Si es una REVERSADA RIESGO RED y rechazada anualmente, actaulizar el LOTE
        if( DPS_APROBADA_SIN_RECHAZO              == wRespCde
            ||             
            DPS_CTA_NO_DEBITABLE_EN_TRANSFERENCIA == wRespCde )
        //////////////////////////////////////////////////////////////////////////////
        {
            // Resguardar respuesta actual
            wRespCdeBkp = wRespCde;

            // Si ESTADO esta en {"00","20","30"}, se enviará el débito al autorizador
            // el cual debitara y actualizara lote, si esta APROBADA (tratamiento=1)
            // o UNICAMENTE actualizara lote....si esta RECHAZADA (tratamiento=2)
            wRespCde = ptrBackOffice->AuthorizeTransaction(
	            szRECTYPE, // Tipo de registro, TDE = debito TCE = credito, CHAR(3)
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
	            szCODRESP , // Codigo Respuesta (16)
	            szCODUSUOFI,// Codigo Usuario Oficial (16)
				/***************************/
				szTIPCUEDEB, // Tipo Cta. Deb. (2)
				szTIPCRE   , // Tipo Cta. Cre. (2)
				szSUCDEB   , // Sucursal DB (4)
				szSUCCRE   , // Sucursal CR (4)
				szIMPORTE  , // Importe (17)
				chMARTIT[0],
				szTIPOPE   ,
				/***************************************/
				(PBYTE) recBATCH
				/***************************************/
                );
            lRetStatus = ptrBackOffice->GetStatus();// ERROR : Numero entero de amplio rango
            // Aprobado y tratamiento anterior era APROBAR? OK entonces .
	        if(0L == lRetStatus && DPS_APROBADA_SIN_RECHAZO == wRespCdeBkp)
            {
                // Si este lo aplica se generará un respuesta RECTYPE="TDR", ESTADO="60" 
	            wRespCde = DPS_EJECUTADA;
                ZEROESTRING(szRECHDB );//  Motivo de rechazo DB en cero
                ZEROESTRING(szRECHCR );//  Motivo de rechazo CR en cero
            }
            /////////////////////////////////////////////////////////////////////////////////
            // UNICAMENTE actualizacion de LOTE por rechazo manual-banca electronica....
	        else if(0L == lRetStatus && DPS_CTA_NO_DEBITABLE_EN_TRANSFERENCIA == wRespCdeBkp)
            {
                //  Motivo de rechazo debito, CTA. NO DEBITABLE
                dpsMsg.SetCauseCode( wRespCdeBkp, dpsMsg.IsDPSCreditTransfer());
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
                wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					        ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					        : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
            }
            /////////////////////////////////////////////////////////////////////////////////
            // Es Error de Sybase ?
            else if (lRetStatus < 0)
            {
                // FIX : demorada porque no se puede conectar.
				// Motivo de rechazo debito, CTA. NO DEBITABLE
                dpsMsg.SetCauseCode( DPS_SISTEMA_NO_DISPONIBLE, dpsMsg.IsDPSCreditTransfer());
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
                // Si este no lo aplica se generará un respuesta RECTYPE="TDR", ESTADO="20" 
                wRespCde = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO;
                // Estado tratamiento PROCESADO,para que el emisor emita respuesta
                if(antoi(szESTADO,2) == DPS_ENVIADO_POR_EL_ABONADO_ )
                {
                    szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
                }
                // Sino, sigue pendiente y se reintenta en prox ciclo
                else if(antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO )
                {
                    szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_ ;
                }
                // Envio pendiente
                szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
            }
			//*****************************************************************************//
            // FIX : Es Error de interno de TCP o algo mas grave? SE DEMORARAR CON ESE CAUSAL
			// en lugar de RECHAZARLAS directamente, y que queden para verificacion manual.
			//*****************************************************************************//
            else if (DPS_OPERACION_INEXISTENTE            == lRetStatus
					 ||
					 DPS_OPERACION_NO_HABILITADA_EN_BANCO == lRetStatus
					 ||
					 DPS_FUERZA_MAYOR                     == lRetStatus
					 ||
					 DPS_APLICACION_NO_DISPONIBLE		  == lRetStatus
					 ||
					 DPS_SISTEMA_NO_DISPONIBLE		      == lRetStatus)
					
            {
                //  Motivo de rechazo debito y estado DEMORADA
				dpsMsg.SetCauseCode( MapBTFErrors( lRetStatus ) , dpsMsg.IsDPSCreditTransfer() );
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
                // Si este no lo aplica se generará un respuesta RECTYPE="TDR", ESTADO="20" 
                wRespCde = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO;
                // Estado tratamiento PROCESADO,para que el emisor emita respuesta
                if(antoi(szESTADO,2) == DPS_ENVIADO_POR_EL_ABONADO_ )
                {
                    szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
                }
                // Sino, sigue pendiente y se reintenta en prox ciclo
                else if(antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO )
                {
                    szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_ ;
                }
                // Envio pendiente
                szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
            }
            
            /////////////////////////////////////////////////////////////////
            // Error de sobregiro en cuenta corriente
            else if (_DPS_ERROR_BLOQUEO_EXCEDE_SALDO_PARA_GIRAR_ == lRetStatus )
            {
                //  Motivo de rechazo debito y estado DEMORADA
				dpsMsg.SetCauseCode( MapBTFErrors( lRetStatus ) , dpsMsg.IsDPSCreditTransfer() );
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
                // Estado demorada
                wRespCde = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO;
                // Envio pendiente? Si, a menos que YA ESTUVIESE enviada
                if(szPENDENVIO[0] != _DPS_PEND_ENV_NO_C_);
                {
                    szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
                }
                // Estado tratamiento PROCESADO,para que el emisor emita respuesta
                if(antoi(szESTADO,2) == DPS_ENVIADO_POR_EL_ABONADO_ )
                {
                    szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
                }
                else if(antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO )
                {
                    szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_ ;
                }
                // Pero si es DEMORADA RIESGO RED debe seguir en ese ESTADO 30
                else if(antoi(szESTADO,2) == DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED )
                {                    
                    //////////////////////////////////////////////////////////////////////////
                    dpsMsg.SetStatusCode( wRespCde = DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED );
                    // Guardar Estado NUEVO de la transferencia
	                COPYFIELD(szESTADO, dpsMsg.input.data.ESTADO );	
                    //////////////////////////////////////////////////////////////////////////
                    szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_ ;
                }
                // Mensaje de Autorizacion para acuerdo temporal
                /*lRetStatusBkp = ptrBackOffice->SendAuthMessage(
	                    szFECSOL ,           // Fecha de Solicitud
	                    szNUMABO ,           // Codigo de Abonado
	                    szNUMCTADEB,         // Cuenta de Debito
                        szNUMREF,            // Lote, inferido de Referencia
                        szNUMTRA,            // Nro. Transferencia
                        szMSG_Acuerdo,       // Monto del Acuerdo
                        szMSG_NomOficial,    // Nombre del Oficial
	                    szMSG_Abonado,       // Cod. Abonado
	                    szMSG_CtaDebito,     // Cta. de Debito
                        szMSG_NomCompletoOficial, // Nombre Completo del Oficial
                        szMSG_Importe,          //Importe
                        szMSG_NomSolicitante,   // Nombre del Solicitante
                        szMSG_QReclamos,        // Cantidad de reclamos solicitados
                        szMSG_MonedaCta,       // Moneda de la Cta.
                        szMSG_EMail);          // E-Mail
				*/
                // Formatear mensaje a enviar
                //shMSG_Counter = atoi(szMSG_QReclamos);
                // Verificar contador menor a un maximo de reclamos efectuados
                if(shMSG_Counter >= 0 && shMSG_Counter < _DEFAULT_MAX_MSG_NOTIFICATION_)
                {                                        
	                // Formateo YYYYMMDDHHMMSS para fecha reclamo
	                strftime(szMSG_TIMESTAMP, sizeof(szMSG_TIMESTAMP), 
                        "%d/%m/%Y %H:%M:%S", ptmLocalTime );
                    // Formato por si no se encontro el oficial, y hay un DEFAULT
                    if(strcmp(szMSG_NomOficial,cszAUTH_MSG_DEFAULT_OFFICER)==0)
                    {
                        // Para instancia superior, se guarda el email enviado
                        // Mensaje apropiado, no se hallo el oficinal
                        strcpy(szMSG_NomOficial,cszAUTH_MSG_NO_OFFICER);
                        // Atencion !
                        strcpy(szMSG_NomCompletoOficial,cszAUTH_MSG_OFFICER_ALERT);
                        // Sin destinatario, ya que es el DEFAULT
                        szMSG_Destiny[0] = 0x00;
                    }
                    else
                    {
                        // Para instancia superior, se guarda el email enviado
                        strcpy(szMSG_Destiny, szMSG_EMail );
                    };
                    // Formateo del cuerpo del mensaje de reclamo autorizacion
                    sprintf(szMSG_Mail, cszAUTH_MSG_FORMAT_MASK ,
                        // Parametros del TITULO
                        szNUMTRA,            // Nro. Transferencia
                        szNUMCTADEB,         // Cuenta de Debito
                        // Parametro de FECHA, HORA y GMT +/- 000
                        asctime( ptmLocalTime ),   // Fecha-Hora de envio
                        // Parametros del CUERPO
                        szNUMTRA,                  // Nro. Transferencia
                        szNUMCTADEB,               // Cuenta de Debito
                        szMSG_NomSolicitante,      // Nombre de la Cuenta                    
                        antoi(szNUMREF,3),         // Lote, inferido de Referencia
                        szMSG_Importe,             // Importe TEF
                        szMSG_Acuerdo,             // Acuerdo necesario
                        szMSG_MonedaCta,           // Moneda de la cta
                        szMSG_NomOficial,          // Nombre del Oficial                        
                        szMSG_NomCompletoOficial,  // Nombre Completo del Oficial
                        shMSG_Counter,             // Contador de Envios
                        szMSG_TIMESTAMP );         // Fecha-Hora de envio                    
                } ; /* end-if-MSG-COUNTER */
            }
            ///////////////////////////////////////////////////////////////////////////
            // Es FACTURACION y estaba la cuenta BLOQUEADA y fue aprobada FORZADAMENTE?
            // Pues informar como DEBITADA-EJECUTADA
	        else if(0L                        != lRetStatus      &&                     
                    dpsMsg.GetOperType(NULL)  == DPS_FACTURACION &&
                    (_DPS_TRAT_SOBREG_APROB_C_ == szESTADOTRAT[0] ||
                     _DPS_TRAT_SOBREG_RECHA_C_ == szESTADOTRAT[0]) )
            {
                // Si este lo aplica se generará un respuesta RECTYPE="TDR", ESTADO="60" 
                ZEROESTRING(szRECHDB );//  Motivo de rechazo DB en cero
                ZEROESTRING(szRECHCR );//  Motivo de rechazo CR en cero
                if(_DPS_TRAT_SOBREG_APROB_C_ == szESTADOTRAT[0])
	                wRespCde = DPS_EJECUTADA;
                else
                {
                    wRespCde = DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
                    //  Motivo de rechazo debito y estado DEMORADA
                    dpsMsg.SetCauseCode( MapBTFErrors( lRetStatus ), dpsMsg.IsDPSCreditTransfer() );
                };                                    
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
            }
            ///////////////////////////////////////////////////////////////////////////
            // Es FACTURACION y estaba PENDIENTE y la cuenta BLOQUEADA?
            // Pues informar como DEMORADA
	        else if(0L                       != lRetStatus      &&                     
                    dpsMsg.GetOperType(NULL) == DPS_FACTURACION &&
                    _DPS_TRAT_PENDIENTE_C_   == szESTADOTRAT[0])
            {
                //  Motivo de rechazo debito y estado DEMORADA
                dpsMsg.SetCauseCode( DPS_APLICACION_NO_DISPONIBLE,dpsMsg.IsDPSCreditTransfer());
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
                // Estado demorada
                wRespCde = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO; 
                // Estado tratamiento PROCESADO,para que el emisor emita respuesta
                if(antoi(szESTADO,2) == DPS_ENVIADO_POR_EL_ABONADO_ )
                {
                    szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
                }
                else if(antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO )
                {
                    szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_ ;
                };
            }
            /* Otros errores */
            else
            {
                //  Motivo de rechazo debito
                dpsMsg.SetCauseCode( MapBTFErrors( lRetStatus ), dpsMsg.IsDPSCreditTransfer() );
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
                wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					        ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					        : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;

            };        
        } /* end-if-APROBADA_SIN_RECHAZO */
        else if (DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED == wRespCde)
        {
            /* Continuar sin hacer nada... este caso es MONOBANCO DIFERIDO */
        }
        else
        {
            //  Motivo de rechazo debito, CTA. NO DEBITABLE
            dpsMsg.SetCauseCode( wRespCde,dpsMsg.IsDPSCreditTransfer());
            //  Recuperar motivos en formato ASCII
            dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
            wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					    ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					    : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
        };        

        /////////////////////////////////////////////////////////////////////////
        // CONTROL DE MONOBANCO DIFERIDA, DEBITO Y CREDITO.
        // Si es CREDITO EJECUTADO MONOBANCO Y DIFERIDO? Queda PENDIENTE PROX DIA
        if( strcmp(szBANDEB,szBANCRE)   == 0        &&
            DPS_EJECUTADA               == wRespCde &&
            ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate )
        {
            /* Continuar sin acreditar, porque lo acredita el BATCH de COMPENSACION */            
            /////////////////////////////////////////////////////////////////////////
            // Solo si la hora de corta ya vencio....
            if( IsCutoverTimeElapsed() && bCheckCutoverInDebit ) 
                // Tratamiento diferido
                szESTADOTRAT[0] = _DPS_TRAT_PEND_PROX_DIA_C_ ;
            /////////////////////////////////////////////////////////////////////////
        }
        else
        // CREDITO EJECUTADO MONOBANCO
        // Autorizar en si mismo, como CREDITO si lo anterior fue un DEBITO 
        // y es una tansferencia MONOBANCO y resulto EJECUTADA en el paso anterior.
        // No se procesan bimonetarias.
        if( strcmp(szBANDEB,szBANCRE) == 0        && 
            DPS_EJECUTADA             == wRespCde   )
        {
			// Anular BANCO DEBITO al momento del CREDITO
            COPYFIELD( recBATCH[0].BANDEB, "***");
			// Impactar CREDITO
            wRespCde = ptrBackOffice->AuthorizeTransaction(
	            DPS_RECTYPE_CREDITO_EJECUTADO, // Tipo de registro, TDE = debito TCE = credito, CHAR(3)
	            "***"    , //  Codigo Banco segun BCRA	ej."***" INVALIDO       
	            szFECSOL , //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	            szNUMTRA , //  Numero de transferencia,CHAR(7) 
	            szNUMABO , //  Abonado	, CHAR(7)
	            szNUMCTADEB, //  Numero de cuenta, formato Banco, CHAR(17)
	            szBANCRE ,   //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
	            szCTACRE ,   //  Numero de cuenta -Formato Banco-, CHAR(17)
	            szESTADO ,   //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	            szNUMREF ,   //  Numero de transferencia PC	 []
	            szNUMENV ,   //  Numero de envio del abonadochar(3)
	            szCODRESP,   // Codigo Respuesta 
	            szCODUSUOFI, // Codigo Usuario Oficial 
				/***************************/
				szTIPCUEDEB, // Tipo Cta. Deb. (2)
				szTIPCRE   , // Tipo Cta. Cre. (2)
				szSUCDEB   , // Sucursal DB (4)
				szSUCCRE   , // Sucursal CR (4)
				szIMPORTE  , // Importe (17)
				chMARTIT[0],
				szTIPOPE   ,
				/*************************************/
				(PBYTE) recBATCH
				/*************************************/
                );
            lRetStatus = ptrBackOffice->GetStatus();
	        if(0L == lRetStatus)
		        wRespCde = DPS_EJECUTADA;
            else
            {
                // Guardar codigo de error SQL del CREDITO
                lRetStatusBkp = lRetStatus;

				// Fix:BANCO DEBITO = BANCO CREDITO, ASI REVERSA EL DEBITO
				COPYFIELD( recBATCH[0].BANDEB, recBATCH[0].DATCRE.BANCRE);

                // Reversar el debito original,
                // invirtiendo las cuentas DEBITO-CREDITO ya que se graba asi en el LOGG.
                wRespCde = ptrBackOffice->ReverseTransaction(
									DPS_RECTYPE_DEBITO_EJECUTADO, // Tipo de registro, TDE = debito TCE = credito, CHAR(3)
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
									szCODRESP , // Codigo Respuesta (16)
									szCODUSUOFI,// Codigo Usuario Oficial (16)
									/***************************/
									szTIPCUEDEB, // Tipo Cta. Deb. (2)
									szTIPCRE   , // Tipo Cta. Cre. (2)
									szSUCDEB   , // Sucursal DB (4)
									szSUCCRE   , // Sucursal CR (4)
									szIMPORTE  , // Importe (17)
									chMARTIT[0],
									szTIPOPE   ,
									/*************************************/
									(PBYTE) recBATCH
									/*************************************/									
                            );
                lRetStatus = ptrBackOffice->GetStatus();
				// FIX : motivo de rechazo, CUENTA CREDITO NO HABILITADA				
				lRetStatusBkp = DPS_ERROR_EN_CTA_CREDITO;
                //  Motivo de rechazo del credito, informado en debito,con error original SQL
                dpsMsg.SetCauseCode( lRetStatusBkp ,dpsMsg.IsDPSCreditTransfer()); 
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
                wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					        ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					        : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
            }; // end-if-getstatus
        }; //end-if-credito-debito-monobanco

        // Actualizar el Estado de Tratamiento a PROCESADA si corresponde
        if( szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ 
            &&
            DPS_EJECUTADA == wRespCde )
        {
            /* Queda procesada */
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
            /* Pendiente de Envio */
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
        }
        else if( szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ 
                &&
                DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO == wRespCde )
        {
            /* Queda pendiente nuevamente */
            szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_;
            /* Pendiente de Envio */
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
        }
        /////////////////////////////////////////////////////////////
        else if( szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ 
                &&
                DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED == wRespCde )
        {
            /* Queda pendiente nuevamente */
            szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_;
            /* Pendiente de Envio */
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
        }
        /////////////////////////////////////////////////////////////
        else if( _DPS_TRAT_PEND_PROX_DIA_C_ == szESTADOTRAT[0] 
                &&
                DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED == wRespCde )
        {
            /* Continuar sin hacer nada... este caso es MONOBANCO DIFERIDO */
        }
        /////////////////////////////////////////////////////////////
        else
        {
            /* Queda procesada */
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
            /* Pendiente de Envio */
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
            /* Continuar sin acreditar, porque lo acredita el BATCH de COMPENSACION */            
            /////////////////////////////////////////////////////////////////////////
            // Solo si la hora de corta ya vencio....
            if( IsCutoverTimeElapsed() && bCheckCutoverInDebit ) 
            {
                // Tratamiento diferido
                szESTADOTRAT[0] = _DPS_TRAT_PEND_PROX_DIA_C_ ;
                // Pendiente de Envio , no !
                szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
            }
            /////////////////////////////////////////////////////////////////////////
        };
    }
    // CREDITO EJECUTADO enviado desde la red (estado definitivo = 60)
    else if( strcmp(szRECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO) == 0 
             &&
             antoi(szESTADO,2) == DPS_EJECUTADA )    
    {
        // Si ESTADO="60", corresponde aplicar el crédito. No se genera respuesta a la Red.
        if( strcmp(szBANCRE,BCRA_ID_DEFAULT) == 0 )
        {            
            wRespCde = ptrBackOffice->AuthorizeTransaction(
	            szRECTYPE, // Tipo de registro, TDE = debito TCE = credito, CHAR(3)
	            szBANDEB , //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
	            szFECSOL , //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	            szNUMTRA , //  Numero de transferencia,CHAR(7) 
	            szNUMABO , //  Abonado	, CHAR(7)
	            szNUMCTADEB, //  Numero de cuenta, formato Banco, CHAR(17)
	            szBANCRE ,   //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
	            szCTACRE ,   //  Numero de cuenta -Formato Banco-, CHAR(17)
	            szESTADO ,   //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	            szNUMREF ,   //  Numero de transferencia PC	 []
	            szNUMENV ,   //  Numero de envio del abonadochar(3)
	            szCODRESP,   // Codigo Respuesta 
	            szCODUSUOFI, // Codigo Usuario Oficial 
				/***************************/
				szTIPCUEDEB, // Tipo Cta. Deb. (2)
				szTIPCRE   , // Tipo Cta. Cre. (2)
				szSUCDEB   , // Sucursal DB (4)
				szSUCCRE   , // Sucursal CR (4)
				szIMPORTE  , // Importe (17)
				chMARTIT[0],
				szTIPOPE   ,
				/*************************************/				
				(PBYTE) recBATCH
				/*************************************/
                );
            lRetStatus = ptrBackOffice->GetStatus();
	        if(0L == lRetStatus)
            {
                // No se genera respuesta a la Red.
		        wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
                ZEROESTRING(szRECHDB );//  Motivo de rechazo DB en cero
                ZEROESTRING(szRECHCR );//  Motivo de rechazo CR en cero
            }
            else
            {
                //  Motivo de rechazo del credito, informado con error original SQL
                dpsMsg.SetCauseCode( MapBTFErrors( lRetStatus ) , is_true );
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
                // Solo si es otro banco. no nosotros mismos, o RIESGO BANCO
                if( ! IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) 
                    ||
                    'S' == chRIEBCO[0] 
                    &&
                    // Si no es una diferida, no enviar respuestas hacia la red
                    ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA != esSendDate
                  )
                {
                    wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					        ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					        : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
                }
                else
                {
                    ////////////////////////////////////////////////////////
                    // No se marca como rechazada porque ya se encuentra
                    // en un estado final (CREDITO EJECUTADO = 60)
                    ////////////////////////////////////////////////////////
                    // No se genera respuesta a la Red.
	    	        wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
                };

            }; // end-if-getstatus
        }; //end-if-credito-desde-otros-bancos

        // Actualizar el Estado de Tratamiento a PROCESADA si corresponde
        if( _DPS_TRAT_SOBREG_APROB_C_ == szESTADOTRAT[0] 
            ||
            _DPS_TRAT_SOBREG_RECHA_C_ == szESTADOTRAT[0] )
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
        // Actualizar el Estado de Tratamiento a PROCESADA si corresponde
		// 2008-12-22	MRB	Demorada por respuestas de error interno
        else if( _DPS_TRAT_PENDIENTE_C_ == szESTADOTRAT[0]  &&
				!(
				atoi(szRECHCR) == DPS_SISTEMA_NO_DISPONIBLE ||
				atoi(szRECHCR) == DPS_APLICACION_NO_DISPONIBLE ||
				atoi(szRECHCR) == DPS_OPERACION_NO_HABILITADA_EN_BANCO)
			)
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
        // Actualizar el Estado de Tratamiento a PROCESADA si era
        // diferida y es fecha de hoy (es un caso de error ya que
        // el batch de cierre es quien la pone PENDIENTE para hoy)
        else if( _DPS_TRAT_PEND_PROX_DIA_C_ == szESTADOTRAT[0]  
                &&
                ESTL_TRANSFERENCIA_OK == esReqDate  )
        {
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
        }
        // Pendiente de Envio siempre en "NO",para creditos en estado DEFINITIVO
        szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_; 

    }
    // DEBITO REVERSADO por RIESGO DE RED, o 
	// RECHAZO BANCO CREDITO (TDE) y ESTADO TRATAMIENTO en PENDIENTE, O
	// REVERSA CUENTA CREDITO X RED (TCE)
    else if( strcmp(szRECTYPE,DPS_RECTYPE_DEBITO_EJECUTADO) == 0 
             &&
             ((antoi(szESTADO,2) == DPS_RECHAZO_DEL_BANCO_DE_CREDITO               
              &&
              (szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_    ||
               szESTADOTRAT[0] == _DPS_TRAT_SOBREG_APROB_C_ ||
               szESTADOTRAT[0] == _DPS_TRAT_REVERSOS_EXCE_C_))
              ||
             (antoi(szESTADO,2) == DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED  
               &&
               szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ ))
             )
    {        
        // Si ESTADO="30", se actualiza el registro correspondiente y el lote.
        // La operación queda disponible para su reenvío. 
        // Con este estado la Red le informa que la ejecución que recibió provocó que se 
        // supere el Riesgo Red, por lo que deja pendiente la aprobación del débito a la 
        // espera de que cambie la situación del Banco.
        wRespCde = ptrBackOffice->ReverseTransaction(
							szRECTYPE, // Tipo de registro, TDE = debito TCE = credito, CHAR(3)
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
							szCODRESP , // Codigo Respuesta (16)
							szCODUSUOFI,// Codigo Usuario Oficial (16)
							/***************************/
							szTIPCUEDEB, // Tipo Cta. Deb. (2)
							szTIPCRE   , // Tipo Cta. Cre. (2)
							szSUCDEB   , // Sucursal DB (4)
							szSUCCRE   , // Sucursal CR (4)
							szIMPORTE  , // Importe (17)
							chMARTIT[0],
							szTIPOPE   ,
							/*************************************/
							(PBYTE) recBATCH
							/*************************************/							
                    );
        lRetStatus = ptrBackOffice->GetStatus();
	    if(0L == lRetStatus)            
        {
            // No se genera respuesta a la Red.
            wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
            szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;
            // Tratamiento es exceso de riesgo red , para oficial de cta.
            if(antoi(szESTADO,2) == DPS_RECHAZO_DEL_BANCO_DE_CREDITO)
                szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_;
            else
                szESTADOTRAT[0] = _DPS_TRAT_REVERSOS_EXCE_C_;
        }
        else
        {
            //  Motivo de rechazo del credito, informado con error original SQL
            dpsMsg.SetCauseCode( MapBTFErrors( lRetStatus ), dpsMsg.IsDPSCreditTransfer() );                        
            //  Recuperar motivos en formato ASCII
            dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
            wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					        ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					        : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
            // No se genera respuesta a la Red.
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
        }; // end-if-getstatus

        // Actualizar el Estado de Tratamiento a EXCESO DE RIESGO si correspondiera
        if( szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ )
        {
            if(antoi(szESTADO,2) == DPS_RECHAZO_DEL_BANCO_DE_CREDITO)
                szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_;
            else
                szESTADOTRAT[0] = _DPS_TRAT_REVERSOS_EXCE_C_ ;
        };

        ///////////////////////////////////////////////////////////////////////
        // Si tambien se debe reversar el CREDITO original de la monobanco....
        ///////////////////////////////////////////////////////////////////////
        if( strcmp(szBANDEB,szBANCRE)==0 && 
            ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED == wRespCde ||
              DPS_EJECUTADA == wRespCde)  )
        {
            // Reversar el debito original,
            // invirtiendo las cuentas DEBITO-CREDITO ya que se graba asi en el LOGG.
            wRespCde = ptrBackOffice->ReverseTransaction(
							szRECTYPE, // Tipo de registro, TDE = debito TCE = credito, CHAR(3)
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
							szCODRESP , // Codigo Respuesta (16)
							szCODUSUOFI,// Codigo Usuario Oficial (16)
							/***************************/
							szTIPCUEDEB, // Tipo Cta. Deb. (2)
							szTIPCRE   , // Tipo Cta. Cre. (2)
							szSUCDEB   , // Sucursal DB (4)
							szSUCCRE   , // Sucursal CR (4)
							szIMPORTE  , // Importe (17)
							chMARTIT[0],
							szTIPOPE   ,
							/*************************************/
							(PBYTE) recBATCH
							/*************************************/
                        );
            lRetStatus = ptrBackOffice->GetStatus();
            //  Motivo de rechazo del credito, informado en debito,con error original SQL
	        if(0L == lRetStatus)            
            {
                // No se genera respuesta a la Red.
                wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
                szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
            }
            else
            {
                dpsMsg.SetCauseCode( MapBTFErrors( lRetStatus ), dpsMsg.IsDPSCreditTransfer()); 
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
                wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					        ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					        : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
                szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
            }; // end-if-getstatus
        }; // end-if-monobanco

        // Actualizar el Estado de Tratamiento a EXCESO DE RIESGO si correspondiera
        if( szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ )
            szESTADOTRAT[0] = _DPS_TRAT_REVERSOS_EXCE_C_ ;

    }
    // CREDITO POR VALOR AL COBRO o INFORMATIVO, AUTORIZADO-RECHAZADO POR EJECUTIVO
    else if( strcmp(szRECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO) == 0 
             &&    
             antoi(szESTADO,2) == DPS_VALOR_AL_COBRO  
             &&
             ( szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_  
               ||
               szESTADOTRAT[0] == _DPS_TRAT_SOBREG_APROB_C_  
               ||
               szESTADOTRAT[0] == _DPS_TRAT_SOBREG_RECHA_C_  ) ) 
    {
        // Indicativo de procesamiento de CREDITO VALOR AL COBRO (40)
        bESTADO_VALOR_AL_COBRO = is_true;
        // Si es APROBADO por ejecutivo....o automaticamente es nuevo (aun pendiente)...
        if( szESTADOTRAT[0] == _DPS_TRAT_SOBREG_APROB_C_ 
            ||
            szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ )
        {            
            wRespCde = ptrBackOffice->AuthorizeTransaction(
	            szRECTYPE, // Tipo de registro, TDE = debito TCE = credito, CHAR(3)
	            szBANDEB , //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
	            szFECSOL , //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	            szNUMTRA , //  Numero de transferencia,CHAR(7) 
	            szNUMABO , //  Abonado	, CHAR(7)
	            szNUMCTADEB, //  Numero de cuenta, formato Banco, CHAR(17)
	            szBANCRE ,   //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
	            szCTACRE ,   //  Numero de cuenta -Formato Banco-, CHAR(17)
	            szESTADO ,   //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	            szNUMREF ,   //  Numero de transferencia PC	 []
	            szNUMENV ,   //  Numero de envio del abonadochar(3)
	            szCODRESP,   // Codigo Respuesta 
	            szCODUSUOFI, // Codigo Usuario Oficial 
				/***************************/
				szTIPCUEDEB, // Tipo Cta. Deb. (2)
				szTIPCRE   , // Tipo Cta. Cre. (2)
				szSUCDEB   , // Sucursal DB (4)
				szSUCCRE   , // Sucursal CR (4)
				szIMPORTE  , // Importe (17)
				chMARTIT[0],
				szTIPOPE   ,
				/*************************************/
				(PBYTE) recBATCH
				/*************************************/
                );
            lRetStatus = ptrBackOffice->GetStatus();
	        if(0L == lRetStatus)
            {
                // Ejecutada
		        wRespCde = DPS_EJECUTADA;
                // Actualizar el Estado de Tratamiento a PROCESADA
                szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_;
                // Estado de envio a la red
                szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
            }
            ////////////////////////////////////////////////////////////////////////
            else
            {
                //  Motivo de rechazo del credito, informado con error original SQL
                dpsMsg.SetCauseCode( MapBTFErrors( lRetStatus) , bESTADO_VALOR_AL_COBRO);
                //  Recuperar motivos en formato ASCII
                dpsMsg.GetCauseCode(szRECHCR, szRECHDB );

                // Solo si es otro banco. no nosotros mismos, o RIESGO BANCO
                if( ! IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) 
                    ||
                    'S' == chRIEBCO[0] 
                    &&
                    // Si no es una diferida, no enviar respuestas hacia la red
                    ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA != esSendDate
                  )
                {
                    wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					        ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					        : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;

					// Actualizar el Estado de Tratamiento a PROCESADA
					szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
					/////////////////////////////////////////////////////////////////////////////////////
					// Maximiliano Broinsky 18-08-2006	Cambio a pendiente de envio SI
					//	
					// Estado de envio a la red
					szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
                }
                else
                {
					//2008-12-22	MRB	Demora los creditos que devuelven 13,14 o 21
					if (atoi(szRECHCR) == DPS_SISTEMA_NO_DISPONIBLE ||
						atoi(szRECHCR) == DPS_APLICACION_NO_DISPONIBLE ||
						atoi(szRECHCR) == DPS_OPERACION_NO_HABILITADA_EN_BANCO)
					{
						//Dejamos todo como esta
						////////////////////////////////////////////////////////
						// ...No se genera respuesta a la Red....
	    				wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
					}
					else
					{
						////////////////////////////////////////////////////////
						// Marcarla como rechazada.... PERO....                    
						dpsMsg.SetStatusCode( DPS_RECHAZO_DEL_BANCO_DE_CREDITO );
						// Guardar Estado NUEVO de la transferencia
						COPYFIELD(szESTADO, dpsMsg.input.data.ESTADO );	
						////////////////////////////////////////////////////////
						// ...No se genera respuesta a la Red....
	    				wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;

						// Actualizar el Estado de Tratamiento a PROCESADA
						szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
						/////////////////////////////////////////////////////////////////////////////////////
						// Maximiliano Broinsky 18-08-2006	Cambio a pendiente de envio SI
						//	
						// Estado de envio a la red
						szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
					}
                };
			}; // end-if-getstatus
        }
        // Sino, entonces es RECHAZADA por ejecutivo....
        else
        {
            // Se responde rechazada
            dpsMsg.SetCauseCode( DPS_OPER_RECHAZADA_BANCO_CREDITO,bESTADO_VALOR_AL_COBRO);
            //  Recuperar motivos en formato ASCII
            dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
            wRespCde = DPS_RECHAZO_DEL_BANCO_DE_CREDITO;
            // Actualizar el Estado de Tratamiento a PROCESADA
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_;
        }; //end-if-credito-desde-otros-bancos

        // Actualizar el Estado de Tratamiento a PROCESADA si corresponde
        /*if( _IS_DPS_ERROR_BLOQUEOS_VARIOS_ )
            szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_ ;
        else if( _DPS_TRAT_PENDIENTE_C_ == szESTADOTRAT[0] )
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;*/

        /////////////////////////////////////////////////////////////////////
        // FEC-SOLICITUD vs FEC-ENVIO : difieren a hoy y respuesta pendiente?
        if(VerifySendFlag( esReqDate,esSendDate,bESTADO_VALOR_AL_COBRO))
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ; //  Procesada
        /////////////////////////////////////////////////////////////////////

    }
    // CREDITO POR VALOR AL COBRO o INFORMATIVO, PENDIENTE (opcionalmente al bloque anterior)
    else if( strcmp(szRECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO) == 0 
             &&    
             ( antoi(szESTADO,2) == DPS_VALOR_AL_COBRO  ||
               antoi(szESTADO,2) == DPS_RECHAZO_DEL_BANCO_DE_DEBITO ) ) 
    {
        // Si ESTADO="40" o RIEBCO="S", y quedo ejecutada, se enviara o no
        // la respuesta segun corresponda... No requiere intervencion de banca electronica.
        // (Puedo quedar EJECUTADA porque el banco opera en esta forma, sin acreditar pero 
        // respondiendo los valores al cobro, aunque sean diferidos)
        if( ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate &&
            (antoi(szESTADO,2) == DPS_VALOR_AL_COBRO ||  'S' == chRIEBCO[0]) )
        {
            wRespCde = DPS_APROBADA_SIN_RECHAZO;
            // Marca de envio pendiente
            szPENDENVIO[0]  = _DPS_PEND_ENV_DEFAULT_C_;
            // Tratamiento diferido ahora es procesado, compensa el bacth de diferidas.
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;  
            // Y se marca como EJECUTADA
            wRespCde = DPS_EJECUTADA;
            ////////////////////////////////////////////////////
            // Pero si la hora de corta ya vencio....
            if( IsCutoverTimeElapsed() && bCheckCutoverInCredit ) 
            {
                // Tratamiento diferido
                szESTADOTRAT[0] = _DPS_TRAT_PEND_PROX_DIA_C_ ;
                wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
            }
            ////////////////////////////////////////////////////

        }
        // Si ESTADO="40" o RIEBCO="S", deberá quedar pendiente, a la espera que Banca Electrónica
        // lo active.  Genera aviso a Banca Electrónica, no requiere intervención del oficial de 
        // créditos.
        else if( antoi(szESTADO,2) == DPS_VALOR_AL_COBRO ||  'S' == chRIEBCO[0] )
        {
            // No enviar respuesta
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
            // Dejarlo pendiente
            szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_;
            // Sin respuesta
            wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
        }
        // Si ESTADO="40"+RIEBCO="S", deberá quedar pendiente, a la espera que Banca Electrónica
        // lo active.  Genera aviso a Banca Electrónica, no requiere intervención del oficial de 
        // créditos.
        else if( antoi(szESTADO,2) == DPS_VALOR_AL_COBRO &&  'S' == chRIEBCO[0] )
        {
            // No enviar respuesta
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
            // Dejarlo pendiente
            szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_;
            // Sin respuesta
            wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
        }
        // NOTA: la posibilidad E STADO="40"+RIEBCO="N" que existe, no será recibida por el 
        // Banco por su modalidad de operación.
        else if( antoi(szESTADO,2) == DPS_VALOR_AL_COBRO && 'N' == chRIEBCO[0] )
        {
            // Se responde rechazada
            dpsMsg.SetCauseCode( DPS_OPERACION_NO_HABILITADA_EN_BANCO,dpsMsg.IsDPSCreditTransfer());
            //  Recuperar motivos en formato ASCII
            dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
            // Si es una diferida, no enviar respuestas hacia la red
            if(ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esSendDate)
            {
                szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
                wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
            }
            else
            {
                szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
                wRespCde = DPS_RECHAZO_DEL_BANCO_DE_CREDITO;
            };
            // Dejarlo pendiente
            szESTADOTRAT[0] =  _DPS_TRAT_PENDIENTE_C_;
        }
        else
        {
            // No enviar respuesta
            szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;
            // Dejarla procesada
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
            wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
        };/*end-if-ESTADO-Y-RIESGO-BANCO*/


    }
    // DIFERIDAS YA procesadas pero cuya respuesta queda para proximo dia
    // DEMORADAS pendientes de procesamieno aun
    else if( (strcmp(szRECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO) == 0 ||
              strcmp(szRECTYPE,DPS_RECTYPE_DEBITO_EJECUTADO) == 0)
            &&    
             ((antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO ||
               antoi(szESTADO,2) == DPS_EJECUTADA                       ||
               antoi(szESTADO,2) == DPS_RECHAZO_DEL_BANCO_DE_DEBITO     ||
               antoi(szESTADO,2) == DPS_RECHAZO_DEL_BANCO_DE_CREDITO )
               &&
               (szESTADOTRAT[0] == _DPS_TRAT_PEND_PROX_DIA_C_) )            
            ||
            (antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO 
             &&
             szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ )
           ) 
    {
        // Dia posterior a hoy
        if(ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate)
        {            
            // Dejarla igual puesto que es dia proximo y ya estaba diferida 
            szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_;
            wRespCde = antoi(szESTADO,2);
            szPENDENVIO[0] = _DPS_PEND_ENV_DIFERIDA_C_;  //  Estado de la respuesta es "D"
        }
        // Dia anterior al actual
        else if(ESTL_FECHA_DE_SOLICITUD_MENOR_A_FECHA_DEL_DIA == esReqDate)
        {
            // Dejarla procesada puesto que es de dias anteriores
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_;
            wRespCde = antoi(szESTADO,2);
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;  //  Estado de la respuesta es "N"
        }
        // Dia de hoy en fecha de solicitud y con otra fecha en el envio
        else 
        {
            // El campo "esSendDate" contiene el resultado de comparar 
            // FEC-SOLICITUD vs FEC-ENVIO.
            if(ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esSendDate)
            {      
                wRespCde = antoi(szESTADO,2);
                // Si estaba DEMORADA, sigue pendiente y queda nuevamente en ENVIADA p/ABONADO                                
                if(antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO)
                {
                    szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_;
                    dpsMsg.SetStatusCode( DPS_ENVIADO_POR_EL_ABONADO_ );
                    szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;  //  Estado de la respuesta es aun no enviada
                }
                else
                {
                    // Dejarla procesada puesto que ya avanzo el dia y estaba pendiente
                    szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_;
                    szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;  //  Estado de la respuesta es aun no enviada
                };
            }
            else
            {
                // Dejarla pendiente puesto que es hoy y ya estaba diferida 
                // tal que en el proximo ciclo , la vuelve a ejecutar.
                szESTADOTRAT[0] = _DPS_TRAT_PENDIENTE_C_;
                wRespCde = antoi(szESTADO,2);
                // Envio pendiente? Si, a menos que YA ESTUVIESE enviada
                if(szPENDENVIO[0] != _DPS_PEND_ENV_NO_C_);
                    szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;                
                // Si estaba DEMORADA, pasa a ENVIADA por el ABONADO
                // que es el unico caso donde queda en la base de datos con "20"
                if(antoi(szESTADO,2) == DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO)
                {
                    dpsMsg.SetStatusCode( DPS_ENVIADO_POR_EL_ABONADO_ );
                    wRespCde = antoi(dpsMsg.input.data.ESTADO,2);
                };/* end-if-ESTADO */
            };/*end-if-FECHA_ENVIO_MENOR_A_HOY */
        };/*end-if-FECHA_SOLICITUD_MAYOR-MENOR-IGUAL_A_HOY */
    }
    // DEBITO REVERSADO por RIESGO DE RED, y PENDIENTE de AUTORIZACION
    else if( ( antoi(szESTADO,2) == DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED  
               ||
               antoi(szESTADO,2) == DPS_RECHAZO_DEL_BANCO_DE_CREDITO  )
               &&
               szESTADOTRAT[0] == _DPS_TRAT_REVERSOS_EXCE_C_ ) 
    {
        // No se hace nada, sigue pendiente de riesgo, avanzar a proximo registro
        dpsMsg.SetStatusCode( DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED );
        szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;
        if(antoi(szESTADO,2) == DPS_RECHAZO_DEL_BANCO_DE_CREDITO)
            szESTADOTRAT[0] = _DPS_TRAT_SOBREG_APROB_C_  ;
        else
            szESTADOTRAT[0] = _DPS_TRAT_REVERSOS_EXCE_C_  ;
        wRespCde = DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED;    
    }
	//////////////////////////////////////////////////////////////////
	else if	( strcmp(szRECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO) == 0 
              &&
              antoi(szESTADO,2) == DPS_B_RECHAZO_CUENTA_CREDITO               
              &&
              (szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_    ||
               szESTADOTRAT[0] == _DPS_TRAT_SOBREG_APROB_C_ ||
               szESTADOTRAT[0] == _DPS_TRAT_REVERSOS_EXCE_C_)
            )
	{
        // Reversar el credito original            
        wRespCde = ptrBackOffice->ReverseTransaction(
						szRECTYPE, // Tipo de registro, TDE = debito TCE = credito, CHAR(3)
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
						szCODRESP , // Codigo Respuesta (16)
						szCODUSUOFI,// Codigo Usuario Oficial (16)
						/***************************/
						szTIPCUEDEB, // Tipo Cta. Deb. (2)
						szTIPCRE   , // Tipo Cta. Cre. (2)
						szSUCDEB   , // Sucursal DB (4)
						szSUCCRE   , // Sucursal CR (4)
						szIMPORTE  , // Importe (17)
						chMARTIT[0],
						szTIPOPE   ,
						/*************************************/
						(PBYTE) recBATCH
						/*************************************/
                    );
        lRetStatus = ptrBackOffice->GetStatus();
        //  Motivo de rechazo del credito, informado en debito,con error original SQL
	    if(0L == lRetStatus)            
        {
            // No se genera respuesta a la Red.
            wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
        }
        else
        {
            dpsMsg.SetCauseCode( MapBTFErrors( lRetStatus ), is_true ); 
            //  Recuperar motivos en formato ASCII
            dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
            wRespCde = ( dpsMsg.IsDPSCreditTransfer() )
					    ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					    : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
        }; // end-if-getstatus

        // Actualizar el Estado de Tratamiento 
        szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;
	}
	//////////////////////////////////////////////////////////////////
    // OTROS CASOS QUE SE LOGGEAN COMO DEMORADAS, Y SON RECHAZADAS
    else     
    {
        dpsMsg.SetCauseCode( DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO,dpsMsg.IsDPSCreditTransfer());
        //  Recuperar motivos en formato ASCII
        dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
        szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
        wRespCde = (dpsMsg.IsDPSCreditTransfer())
            ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
            : DPS_RECHAZO_DEL_BANCO_DE_DEBITO;    

        // Actualizar el Estado de Tratamiento a PROCESADA si corresponde
        if( szESTADOTRAT[0] == _DPS_TRAT_PENDIENTE_C_ )
            szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;

    };
    //////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////
    // POST-CONTROL 6 : Establecer codigo de respuesta en campo ESTADO de msg salida
    if(wRespCde != DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED)
    {
        // Guardar Estado ACTUAL de la transferencia
        COPYFIELD(szESTADO_PREV, dpsMsg.input.data.ESTADO );	
        dpsMsg.SetStatusCode( wRespCde , bAPROBADA_FALTAN_DATOS_EN_OPERACION);        
        // Guardar Estado NUEVO de la transferencia
	    COPYFIELD(szESTADO, dpsMsg.input.data.ESTADO );	
    };
    //////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////
    // POST-CONTROL 7 : Si es una transferencia PERO el banco credito no trata las 
    // respuestas, no se debe enviar ninguna respuesta hacia la red.
    if ( 'S' != chDIFCRE[0]                     
        && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) 
        && ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate )
        wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED;
    // POST-CONTROL 8 : Si es una transferencia y ya se encuentra en estado final  
    // no se debe responder, A MENOS que provenga de BANCO INEXISTENTE, y a menos
    // que provenga de un VALOR AL COBRO.
    else if(dpsMsg.IsResponseCodeInFinalState()             && 
            dpsMsg.IsDPSCreditTransfer()                    &&
            is_false == bAPROBADA_FALTAN_DATOS_EN_OPERACION &&
            is_false == bESTADO_VALOR_AL_COBRO              )
    {
        // A priori no se envian rtas. de transferencias
        wRetVal  = wRespCde; // Guardar resultado anterior
        wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED; // Establecer nuevo resultado
        // A menos que el estado anterior no haya sido un estado final
        if(antoi(szESTADO_PREV,2) != antoi(szESTADO,2))  
            wRespCde = wRetVal; // Recuperar respuesta anterior
        // No queda pendiente de envio, entonces.
        szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;    //  Estado de la respuesta es "N"
        // ASIGNARLA como PROCESADA
        szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;//  PROCESADA  
    }
    //////////////////////////////////////////////////////////////////////////////////
    
    //////////////////////////////////////////////////////////////////////////////////
    // POST-CONTROL 9 : Establecer codigo de respuesta hacia la red (enviado, 
    // diferido, no enviado)
    // Banco Debito no trata DIFERIDAS CREDITO, y no es nuestro banco
    ValidatePostingDate( &esReqDate , &esSendDate ,  is_true );  
    if( 'S' != chDIFCRE[0]                                // No trata creditos diferidos
        &&                         
        !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE)  // No es nuestro banco
        &&
        ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate // Es diferida
        &&
        szESTADOTRAT[0] == _DPS_TRAT_PROCESADA_C_  ) // Procesada
    {
        // Fecha de envio y Fecha de Solicitud, difieren? No enviar rtas. al dia siguiente.
        if(ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate
            &&
           ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esSendDate)            
             szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;       //  Estado de la respuesta es "N"
        else
             szPENDENVIO[0]  = _DPS_PEND_ENV_DIFERIDA_C_; //  Estado de la respuesta es "D"
        // ASIGNARLA como DEBITO DIFERIDO
        szESTADOTRAT[0] = _DPS_TRAT_PEND_PROX_DIA_C_ ;//  DIFERIDA
    }     
    // Banco Credito en caso de que seamos nosotros, NO TRATA DIFERIDAS CREDITO
    else if('S' != chDIFCRE[0]                                   // No trata creditos diferidos
            &&
            IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE)  // Si es nuestro banco
            &&
            !IS_ON_OUR_BANKCODE(dpsMsg.input.data.BANDEB)        // No es nuestro banco
            &&
            szPENDENVIO[0] != _DPS_PEND_ENV_NO_C_      //  Estado de la respuesta no es "N"?
            &&        
            is_false == bESTADO_VALOR_AL_COBRO )      // No es un VALOR AL COBRO
    {
        // ASIGNARLA como NO PENDIENTE
        szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;    //  Estado de la respuesta es "N"
        szESTADOTRAT[0] = _DPS_TRAT_PROCESADA_C_ ;//  Procesada
    }        
    else if( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED == wRespCde  // Si no se envia rta.a la red
        &&
        _DPS_PEND_ENV_DIFERIDA_C_ != szPENDENVIO[0]      // Si estado de respuesta no es "D"
        &&
        _DPS_ERROR_BLOQUEO_EXCEDE_SALDO_PARA_GIRAR_ == lRetStatus)  // Si estado de respuesta no es "N"
    {
        // ASIGNARLA como DIFERIDA PENDIENTE
        szPENDENVIO[0]  = _DPS_PEND_ENV_DEFAULT_C_   ; //  Estado de la respuesta es " "
        szESTADOTRAT[0] = _DPS_TRAT_PEND_PROX_DIA_C_ ; //  DIFERIDA
    }
    else if( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED == wRespCde  // Si no se envia rta.a la red
        &&
        _DPS_PEND_ENV_DIFERIDA_C_ != szPENDENVIO[0]  // Si estado de respuesta no es "D"
        &&
        _DPS_TRAT_PEND_PROX_DIA_C_ == szESTADOTRAT[0] )// Si estado tratamiento es "8"
    {
        // ASIGNAR UNA "N" a pendiente de envio, para no enviar nada
        szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_ ; //  Estado de la respuesta es "N"
    }
    else if (_DPS_PEND_ENV_DIFERIDA_C_ != szPENDENVIO[0] // Si estado de respuesta no es "D"
             &&
             _DPS_PEND_ENV_NO_C_ != szPENDENVIO[0])      // Si estado de respuesta no es "N"
    {
        // ASIGNAR UN BLANCO " " a pendiente de envio para que se envie
        szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;     //  Estado de la respuesta es " "
    }
    //////////////////////////////////////////////////////////////////////////////////

 
    ///////////////////////////////////////////////////////////////////////////////////
    // Re-calcular el MAC-2/3 por posible error de banco inexistente
    ///////////////////////////////////////////////////////////////////////////////////    
    wRetVal = CalculateMAC( IS_ON_OUR_BANKCODE(szBANDEB) ? "2" : "3", szMAC );
    if( DPS_APROBADA_SIN_RECHAZO != wRetVal && bValidateMAC ) 
    {
        //  Motivo de rechazo debito/credito
        dpsMsg.SetCauseCode( wRetVal,dpsMsg.IsDPSCreditTransfer());
        wRetVal = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO ;
        dpsMsg.SetStatusCode( wRetVal );
        // Guardar Estado NUEVO de la transferencia
	    COPYFIELD(szESTADO, dpsMsg.input.data.ESTADO );	
        //  Recuperar motivos en formato ASCII
        dpsMsg.GetCauseCode(szRECHCR, szRECHDB );
        wRetVal = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO;
        // No queda pendiente de envio
        szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_ ;
    };
    
    //////////////////////////////////////////////////////////////////////////////////
    // Actualizar la transaccion en la tabla TEF ONLINE
    //////////////////////////////////////////////////////////////////////////////////
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
        szPENDENVIO, // PENDIENTE DE ENVIO
        szRECHDB,    // Motivo de rechazo DEBITO
        szRECHCR,   // Motivo de rechazo CREDITO
        szESTADOTRAT ) ; // Estado tratamiento
 	if( (lRetStatusBkp = ptrBackOffice->GetStatus()) != 0 ) 
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
			hFile = NULL;
		}
		/////////////////////////////////////////////////////////
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
            szPENDENVIO, // PENDIENTE DE ENVIO
            szRECHDB,    // Motivo de rechazo DEBITO
            szRECHCR,   // Motivo de rechazo CREDITO
            szESTADOTRAT ) ; // Estado tratamiento
        // Por ERROR... 
     	if((lRetStatusBkp = ptrBackOffice->GetStatus()) == 0) 
        {                
            // Copia de resultado interno de error BACKEND si hubiera
            if(plExtRetStatus) (*plExtRetStatus) = lRetStatusBkp;
		    return ( wRespCde );
        }; //end-if
        // Por error, copia de resultado interno de error BACKEND si hubiera
        if(plExtRetStatus) (*plExtRetStatus) = lRetStatusBkp;
		return (wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED);
    };
    //////////////////////////////////////////////////////////////////////////////////


    // Copia de resultado interno de error BACKEND si hubiera para DEBITO/CREDITO
    // Atencion que esta variable no debiera ser sobreescrita con el UPDATE/SELECT
    if(plExtRetStatus) 
        (*plExtRetStatus) = lRetStatus;
	// Retornar OK,ERROR,o SIN_RESPUESTA_HACIA_LA_RED

	return ( wRespCde );
}
///////////////////////////////////////////////////////////////////////////////////

#endif // _SYSTEM_DATANET_ON2_

