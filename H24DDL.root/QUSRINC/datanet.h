//
// IT24 ASAP S.A. - COA S.A. - Martin Dario Cernadas
// INTERFACE para DATANET E_SETTLEMENT y DPS 
// 
// D.P.S. - DISTRIBUTED PROCESSING SYSTEM - Version 5.0 
// D.P.S. - DISTRIBUTED PROCESSING SYSTEM - Version 5.1 ( + CUIT para CBU)
//
//
// Clase Mensaje Financiero DATANET [Version 6.2,Agosto_2002], E-Settlement y DPS.
// Definiciones para plataforma CICS y NO-CICS.
//
// Tarea        Fecha           Autor   Observaciones
// [Inicial]    2002.12.27      mdc     Base E-Settlement
// [Inicial]    2003.01.21      mdc     Base E-Settlement + headers
// [Inicial]    2003.01.31      mdc     Base DPS Refresh batch y online
// [Inicial]    2003.03.21      mdc     Metodos con logica propia en mensaje DPS
// [Inicial]    2003.04.15      mdc     friend class TrxResDATANET
// [Alfa]       2003.09.16      mdc     parametros completos en ejecuciones SP
// [Alfa]       2003.09.30      mdc     MACDATA::IMPORTE es PIC 9(15)V99, faltaban 2 digitos.
// [Alfa]       2003.10.27      mdc     DPS_DEMORADA_NO_ENVIAR_RESPUESTA=998
// [Alfa]       2003.12.04      mdc     boolean_t GetCauseCode( char szRECHCR[], char szRECHDB );
// [Alfa]       2004.01.08      mdc     ::GetRecType()
// [Beta]       2004.02.23      mdc     DPIFCOMM incluida como estructura para REFRESH ONLINE
// [Delta]      2004.04.30      mdc     #define _DPS_COBIS_ERROR_BLOQUEO_DEPOSITO_  
// [Delta]      2004.08.11      mdc     Campos nuevos en REGISTRO DPS, CUIT-debito y credito
// [Delta]      2004.09.07      mdc     EXPORT DPIFCOMM( const char *cszMessage );
// [Delta]      2004.09.09      mdc     Cobranzas : IMPORTE	es PIC S9(16)V99	
// [Release-1]  2005.01.14      mdc     DPS_PAGOS_AFIP=14
// [Release-2]  2005.08.09      mdc     friend class TrxResJERONIMO
// [Release-2]  2005.10.21      mdc     friend class XML_CONNECTOR;
// [Release-2]  2006.02.22      mdc     Varios atributos "EXPORT" agregados.
// [Release-2]  2006.03.06      mdc     friend class DPS_i_batch;
// [Release-3]  2006.03.06      mdc     class DPIBCOMM para BATCH CICHERO en FINANSUR, y BCBA.
// [Release-3]  2006.07.28      mdc     ReadString(char * lpValueName, char * lpData, int maxlen);
// [Release-3]  2008.07.03      mdc     Se actualizaron MAS codigos de error online.
//

#ifndef _DATANET_H_
#define _DATANET_H_

// Alias y tipificaciones de datos propias 
#include <qusrinc/alias.h>
#include <qusrinc/typedefs.h>
// Identificacion de Bancos ante el Banco Central / Autoridad de Competencia
#include <qusrinc/dnetbcra.h>
// DPS SYSTEM
#include <qusrinc/dpcsys.h>

// Longitud de la MAC
#define DNET_MAC_LENGTH (12)
// Longitud de la CTA
#define DNET_CTA_LENGTH (17)
// Longitud del IMPORTE
#define DNET_AMMT_LENGTH (17)
// Longitud de la clave abonado-banco
#define DNET_BANK_KEY_LENGTH (8)

//////////////////////////////////////////////////////////////////////////////
// Formato E_SETTLEMENT.    
// Mensaje de Entrada 
// Formato Header.
// Nombre del Campo	Formato	Descripcion
// Datos de Cabecera		Longitud Total Header 80 Bytes.
//////////////////////////////////////////////////////////////////////////////
class E_Settlement_hdr_i {
	public :
		EXPORT E_Settlement_hdr_i();
		EXPORT ~E_Settlement_hdr_i();	

	struct {
		char	HDR_CICS_TRAN	[4],	// ECOM
				HDR_LONGITUD	[5],	// Longitud del Mensaje incluyendo HDR
				HDR_CICS_NODO	[4],	// Nombre del Nodo DPS de origen
				HDR_COD_MENSAJE	[16],	// TEF_DEB_AUT
				HDR_COD_RETORNO	[4],	// Codigo de Retorno
				HDR_CANT		[3],	// '000'
				HDR_ULT_REGISTRO[1],	// '0'=Ultimo mensaje  
				HDR_CONN_TIPO	[1],	// 'L' 
				HDR_CODE_PAGE	[1],	// Blancos
				HDR_FILLER		[41];	// Blancos
		} data;
	} ;

//////////////////////////////////////////////////////////////////////////////
// Area de Datos.
// Mensaje tipo TCE		Longitud Datos 8192
//////////////////////////////////////////////////////////////////////////////
class E_Settlement_i {
	public :
		EXPORT E_Settlement_i();
		EXPORT ~E_Settlement_i();	
	
	struct {
		char	NUMABO	[7],	// 	Numero de abonado
				NUMREF	[7],	// 	Numero de transferencia asignado por el Banco 
				FECSOL	[8],	// 	Fecha de solicitud     [AAAA/MM/DD],  // 
				IMPORTE	[17],	//  9(15)V99	Importe 
				TIPOPE	[2],	// 	Tipo de Operacion    [debe ser 15],  // 
				BANDEB	[3],	// 	Codigo de Banco de debito
				TIPCUE	[2],	// 	Codigo de tipo de cuenta debito
				NCUECM  [2],	// 	Numero corto Datanet de la cuenta de debito
				NUMCTA	[17],	// 	Numero de cuenta de debito
				NCUIDB  [11],	// 	Numero de CUIT de la cuenta de Debito Real
				NOMSOL  [29],	// 	Nombre de la cuenta de debito
				CBUDEB	[22],	// 	CBU de la cuenta REAL de debito
				BANCRE	[3],	// 	Codigo del Banco de credito
				TIPCRE	[2],	// 	Codigo de la cuenta de credito
				NCRECM  [2],	// 	Numero corto Datanet de cuenta de credito
				CTACRE	[17],	// 	Numero de cuenta de credito
				NCUICR  [11],	// 	Numero de CUIT de la cuenta de credito
				NOMBEN  [29],	// 	Nombre de la cuenta de credito
				CBUCRE  [22],	// 	Numero de CBU de la cuenta de credito
				MACUNO	[12],	// 	Firma digital [MAC_UNO],  // 
				MACDOS	[12],	// 	Firma digital [MAC_DOS],  // 
				OPECON  [2],	// 	Codigo de operador que confecciono la operacion
				OPEAU1  [2],	// 	Codigo de operador del primer Autorizante
				OPEAU2  [2],	// 	Codigo de operador del segundo Autorizante
				OPEAU3  [2],	// 	Codigo de operador del  tercer Autorizante
				NUMESQ  [2],	// 	Numero de Esquema de Firmas
				OPEDB1  [2],	// 	Codigo de operador del Autorizante de Input
				OPEDB2  [2],	// 	Codigo de operador del Autorizante de Output
				OBSER1	[60],	// 	Observaciones o blancos
				OBSER2	[100],  // 	Datos del pago.  
				FILLER	[7785]; // 	Relleno con Blancos  
		} data;
	};

 
//////////////////////////////////////////////////////////////////////////////
// Mensaje de Salida E_SETTLEMENT
// Formato Header.
// Nombre del Campo	Formato	Descripcion
// Datos de Cabecera		Longitud Total Header 80 Bytes.
//////////////////////////////////////////////////////////////////////////////
class E_Settlement_hdr_o {
	public :
		EXPORT E_Settlement_hdr_o();
		EXPORT ~E_Settlement_hdr_o();	

	struct {
		char	HDR_CICS_TRAN	 [4],  //	ECOM
				HDR_LONGITUD	 [5],  //	Longitud del Mensaje incluyendo HDR
				HDR_CICS_NODO	 [4],  //	Nombre del Nodo DPS de origen
				HDR_COD_MENSAJE	 [16], //	TEF_DEB_AUT
				HDR_COD_RETORNO	 [4],  //	Codigo de Retorno
				HDR_CANT		 [3],  //	'000'
				HDR_ULT_REGISTRO [1],  //	'0'=Ultimo mensaje  
				HDR_CONN_TIPO	 [1],  //	'L' 
				HDR_CODE_PAGE	 [1],  //	Blancos
				HDR_FILLER		 [41]; //	Blancos
		} data;
	} ;

//////////////////////////////////////////////////////////////////////////////
// Area de Datos.
// Mensaje tipo TCE		Longitud de salida 27 Bytes
//////////////////////////////////////////////////////////////////////////////
class E_Settlement_o {
	public :
		EXPORT E_Settlement_o();
		EXPORT ~E_Settlement_o();	

	struct {
		char	RETORNO				[4],	//	Codigo de Retorno 
				REFERENCIA			[7],	//	Nro. de Transferencia asignado por el Banco
				NUMTRA				[7],	//	Nro. de Transferencia asignado por Datanet
				ESTADO_TR			[2],	//	Estado de la transferencia
				ESTADO_HORA			[6],	//	Hora en que la transferencia paso a ese estado.
				ESTADO_DPS			[1],	//	Estado Banco debito  'O' On_Line    'M' Manual
				CUENTA_PROPIA		[1],	//	Marca de cuentas propias
				EXCEDE_RIESGO_BANCO	[1],	//	Marca de excede riesgo Banco
				EXCEDE_RIESGO_RED	[1],	//	Marca de excede riesgo red
				FILLER				[8162]; //	Relleno con blancos
		} data;
	};

//////////////////////////////////////////////////////////////////////////////
// Estructura E-SETTLEMENT de INPUT y OUTPUT
//////////////////////////////////////////////////////////////////////////////
class E_Settlement_INPUT {
	public :

	E_Settlement_hdr_i	header;
	E_Settlement_i 		body;
	} ;

//////////////////////////////////////////////////////////////////////////////
// Estructura E-SETTLEMENT de INPUT y OUTPUT
//////////////////////////////////////////////////////////////////////////////
class E_Settlement_OUTPUT {
	public :

	E_Settlement_hdr_o	header;
	E_Settlement_o 		body;
	} ;

//////////////////////////////////////////////////////////////////////////////
// Codigos de Retorno E_SETTLEMENT
//////////////////////////////////////////////////////////////////////////////
typedef enum e_settlement_retcode {
	 ESTL_TRANSFERENCIA_OK					=	0,
	 ESTL_ERROR_GRAVE_DEL_SISTEMA			=	99,
	 ESTL_ERROR_DEL_SISTEMA					=	501,
	 ESTL_DATOS_NO_NUMERICOS				=	502	,
	 ESTL_TIPO_DE_CUENTA_DEBITO_INEXISTENTE	=	503,
	 ESTL_TIPO_DE_CUENTA_CREDITO_INEXISTENTE=	504,
	 ESTL_BANCO_DEBITO_NO_VALIDO			=	505,
	 ESTL_BANCO_CREDITO_NO_VALIDO			=	506,
	 ESTL_FECHA_DE_SOLICITUD_ES_DIA_FERIADO	=	507,
	 ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA	=	508,
	 ESTL_FECHA_DE_SOLICITUD_MENOR_A_FECHA_DEL_DIA			=	509,
	 ESTL_ABONADO_NO_OPERATIVO_CON_BANCO					=	510,
	 ESTL_CUENTA_CORTA_DEBITO_NO_EXISTE						=	511,
	 ESTL_NUMERO_DE_CUENTAS_LARGO_DE_DEBITO_NO_CONCUERDA	=	512,
	 ESTL_OBSERVACION_SERVICIOS_NO_VALIDO					=	513,
	 ESTL_OPERADOR_QUE_CONFECCIONO_NO_VALIDO				=	514,
	 ESTL_CUENTA_DEBITO_NO_OPERATIVA_PARA_TRANSFERENCIA		=	515,
	 ESTL_IMPORTE_DE_PAGO_MAYOR_AL_PERMITIDO				=	516,
	 ESTL_CUENTA_CREDITO_INVALIDA_O_DE_BAJA					=	517,
	 ESTL_IMPORTE_IGUAL_A_CEROS								=	518,
	 ESTL_INHABILITADO_RIESGO_BANCARIO						=	519,
	 ESTL_OPERACION_FUERA_DE_HORA							=	520,
	 ESTL_OPERADOR_QUE_CONFECCIONO_LA_OPERACION_NO_EXISTE	=	521,
	 ESTL_OPERADOR_FIRMA_1_NO_EXISTE						=	522,
	 ESTL_OPERADOR_FIRMA_2_NO_EXISTE						=	523,
	 ESTL_OPERADOR_FIRMA_3_NO_EXISTE						=	524,
	 ESTL_OPERADOR_QUE_CONFECCIONO_INHABILITADO_POR_BANCO	=	525,
	 ESTL_OPERADOR_FIRMA_1_INHABILITADO_POR_EL_BANCO		=	526,
	 ESTL_OPERADOR_FIRMA_2_INHABILITADO_POR_EL_BANCO		=	527,
	 ESTL_OPERADOR_FIRMA_3_INHABILITADO_POR_EL_BANCO		=	528,
	 ESTL_OPERADOR_FIRMA_1_SIN_FUNCION_FIRMA				=	529,
	 ESTL_OPERADOR_FIRMA_2_SIN_FUNCION_FIRMA				=	530,
	 ESTL_OPERADOR_FIRMA_3_SIN_FUNCION_FIRMA				=	531,
	 ESTL_ESQUEMA_DE_FIRMA_NO_VALIDO						=	532,
	 ESTL_ESQUEMA_DE_FIRMA_NO_EXISTE						=	533,
	 ESTL_NO_CONCUERDA_NOMBRE_CUENTA_CREDITO				=	534,
	 ESTL_CUENTA_SIN_RELACION								=	535,
	 ESTL_OPERADOR_CONFECCION_BLOQUEADO						=	539,
	 ESTL_OPERADOR_FIRMA_1_BLOQUEADO						=	540,
	 ESTL_OPERADOR_FIRMA_2_BLOQUEADO						=	541,
	 ESTL_OPERADOR_FIRMA_3_BLOQUEADO						=	542,
	 ESTL_USO_DE_CUENTA_DE_CREDITO_NO_VALIDO				=	544,
	 ESTL_INCONGRUENCIA_ENTRE_TIPOS_DE_CUENTA				=	545,
	 ESTL_INCONGRUENCIA_ENTRE_TIPO_DE_CUENTA_Y_BANCO_CREDITO=	546,
	 ESTL_CUENTA_CREDITO_NO_HABILITADA_POR_BANCO			=	558,
	 ESTL_SUCURSAL_NO_OPERATIVA								=	559,
	 ESTL_TIPO_DE_OPERACION_NO_VALIDA						=	560,
	 ESTL_ABONADO_BLOQUEADO									=	567,
	 ESTL_NO_EXISTE_ABONADO									=	568,
	 ESTL_TRANSFERENCIA_DUPLICADA							=	569,
	 ESTL_BANCO_DE_DEBITO_NO_OPERATIVO_EN_TRANSFERENCIAS	=	570,
	 ESTL_BANCO_DE_CREDITO_NO_OPERATIVO_EN_TRANSFERENCIAS	=	571,
	 ESTL_EXCEDE_RIESGO_RED									=	572,
	 ESTL_CUENTA_DEBITO_NO_PUEDE_SER_RECAUDADORA			=	573,
	 ESTL_NUMERO_DE_CUIT_NO_VALIDO							=	574,
	 ESTL_ABONADO_NO_ES_ABONADO_CCI							=	575,
	 ESTL_BANCO_DE_DEBITO_NO_COINCIDE_CON_ABONADO_CCI		=	576,
	 ESTL_NODO_NO_VALIDO										=	577,
	 ESTL_NODO_DE_RECEPCION_NO_COINCIDE_CON_CODIGO_BANCO_DEBITO	=	578,
	 ESTL_DIGITO_VERIFICADOR_DEL_CUIT_DE_DEBITO_NO_VALIDO		=	579,
	 ESTL_DIGITO_VERIFICADOR_DEL_CUIT_DE_CREDITO_NO_VALIDO		=	580,
	 ESTL_NUMERO_DE_TRANSFERENCIA_NO_VALIDO						=	581,
	 ESTL_NUMERO_DE_REFERENCIA_FUERA_DE_RANGO					=	582,
	 ESTL_BANCO_DE_DEBITO_NO_OPERA_EN_LIBRE_DISPONIBILIDAD		=	583,
	 ESTL_BANCO_DE_CREDITO_NO_OPERA_EN_LIBRE_DISPONIBILIDAD		=	584
	} ;

//////////////////////////////////////////////////////////////////////////////
// Formato DPS.    
// Mensaje de Entrada  (INPUT)
// Nombre del Campo	Formato	Descripcion
//////////////////////////////////////////////////////////////////////////////

typedef struct DPS_header_tag {
   char TPNAME	 [	4],	//Nombre nodo emisor	ej."DPRE" : Datanet
		NDNAME	 [	4];	//Nombre nodo receptor	ej."DNET" : 
   char HORA     [  4]; // Hora del sistema COMP(7) SIGN
   char SECUENCIA[  2]; // Nro. Secuencia COMP(3) SIGN
	} DPS_header;


class DPS_i {

public :
	EXPORT DPS_i();
	EXPORT ~DPS_i();	

	struct {
       DPS_header   header; //Header ruteador DPS
	   char FRNAME	[	4],	//Nombre nodo emisor	ej."DNET" : Datanet
			TONAME	[	4],	//Nombre nodo receptor	ej."BSUD" : Bansud
			RECTYPE	[	3],	//Tipo de registro	TDE = debito TCE = credito
			ACTIVITY[   1];	//Actividad	"N"
	   char COUNTER [   2];	//Longitud del registro	580 (HEXA'0244'), en BINARIO, 2 bytes
	   char FILLER1	[	6],	//Reservado	Uso Datanet
			BANDEB	[	3],	//Codigo Banco segun BCRA	ej."067":BANSUD
			FECSOL	[	8],	//Fecha de compensacion	Del dia o prox. Habil -aaaammdd-
			NUMTRA	[	7],	//Numero de transferencia	
			NUMABO	[	7],	//Abonado	
			TIPOPE	[	2],	//Tipo de operacion	Ver tabla I
			IMPORTE [  17];//Importe de la transferencia	15 enteros y 2 decimales
	   struct {		
		   char SUCDEB	[	4],	//Sucursal del Banco que tratara el debito	No es necesariamente la de la cuenta
				NOMSOL	[  29],	//Nombre de la cuenta	
				TIPCUE	[	2],	//Tipo de cuenta	Ver tabla II
				NCUECM	[	2],	//Numero de cuenta corto	Interno Datanet
				NUMCTA	[  17],	//Numero de cuenta	Formato Banco
				FSENDB	[	6],	//Fecha de envio de la transf.  al Banco	Aammdd
				HSENDB	[	4],	//Hora de envio	Hhmm
				OPEDB1	[	2],	//Identificacion del operador #1	Primer autorizante del Banco de db.
				OPEDB2	[	2],	//Identificacion del operador #2 	Segundo autorizante
				RECHDB	[	4];	//Motivo del rechazo del Banco de debito	Ver tabla III
			}  DATDEB;      //Datos del debito	
	   struct {
		   char BANCRE	[	3],	//Codigo de Banco segun BCRA	
				SUCCRE	[	4],	//Sucursal del Banco que tratara el credito	No es necesariamente la de la cuenta
				NOMBEN	[  29],	//Nombre de la cuenta a acreditar	
				TIPCRE	[	2],	//Tipo de cuenta	Ver tabla II
				CTACRE	[  17],	//Numero de cuenta	Formato Banco
				FSENCR	[	6],	//Fecha de envio de la transf.  al Banco	Aammdd
				HSENCR	[	4],	//Hora de envio	Hhmm
				OPECR1	[	2],	//Identificacion del operador #1	Primer autorizante del Banco de cr.
				OPECR2	[	2],	//Identificacion del operador #2 	Segundo autorizante
				RECHCR	[	4];	//Motivo del rechazo del Banco de credito	Ver tabla III
			} DATCRE	;			//Datos del credito	
	   char OPECON	[	2],	//Operador que confecciono la transferencia	
			OPEAU1	[	2],	//Id. Firmante #1 (primer autorizante de la Empresa	
			OPEAU2	[	2],	//Id. Firmante #2 (segundo autorizante de la Empresa	
			OPEAU3	[	2],	//Id. Firmante #3 (tercer autorizante de la Empresa	
			FECAUT	[	6],	//Fecha de autorizacion (envio del pago a la red)	Aammdd
			HORAUT	[	4],	//Hora de autorizacion (envio del pago a la red)	Hhmm
			ESTADO	[	2],	//Estado de la transferencia	Ver tabla IV
			FECEST	[	6],	//Fecha ultima modificacion de ESTADO	
			OBSER1	[  60],	//Texto libre ingresado por el abonado	Opcional
			OBSER2	[ 100],	//Datos adicionales ingresados por el abonado	Opcional, varia segun TIPOPE. Ver Tabla V
			CODMAC	[  12],	//Message Authentication Code (MAC)	Ver Anexo A
			NUMREF	[	7],	//Numero de transferencia PC	Nro. De lote (N,3) + nro. de operacion dentro del lote (N,4) 
			NUMENV	[	3],	//Numero de envio del abonado	
			CONSOL	[	1],	//Marca de consolidacion de movimientos ("S" o "N")	"S" se genera un unico movimiento en operaciones de debito con igual fecha de solicitud y  nro. de lote
			MARTIT	[	1],	//Indica que la cuenta de debito y la cuenta de credito son de la misma empresa	
			PRIVEZ	[	1],	//"S" indica que al momento en que se envia el registro al Banco de debito no existe un pago previo aceptado por los dos Bancos que involucre a las cuentas intervinientes."N" indica que los Bancos aceptaron previamente un pago en el que intervenian las mismas cuentas	
			DIFCRE	[	1],	//"S" habilita al Banco de debito que asi lo desee a tratar la transferencia sin importar que sea diferida (prox. habil)."N" indica que el Banco de debito no puede tratar la transferencia diferida porque el Banco de credito no las puede manejar. 	
			RIEABO	[	1],	//"S" indica que el pago supera el riesgo fijado"N" el pago no supera el riesgo	
			RIEBCO	[	1];	//"S" indica que al ser aceptado el debito la transferencia supero el riesgo establecido"N" la transferencia no supera el riesgo pero tiene tratamiento del credito obligatorio" " indica que la transferencia quedo ejecutada por el debito (no supero el riesgo ni fue tratado el credito)	
	   struct {
		   char CODEST	[	2], //Codigo de estado	
				FECEST	[	6], //Fecha de estado	
				HOREST	[	6];	//Hora del estado	
			} TABEST[  10]; //Tabla de Estados : 10 ocurrencias
	   /********************************************************/
	   char INDCUIT	[   1];//Indicativo CUIT
	   char CUITDEB	[  11];//CUIT debito
	   char CUITCRE	[  11];//CUIT credito
	   /********************************************************/
	   char FILLER2	[  14];//Reservado
		} data;

	}; // end-DPS-i

//////////////////////////////////////////////////////////////////////////////
// Formato DPS.    
// Mensaje de Salida (OUTPUT)
// Nombre del Campo	Formato	Descripcion
//////////////////////////////////////////////////////////////////////////////
class DPS_o {

public :
	EXPORT DPS_o();
	EXPORT ~DPS_o();	

	struct {
       DPS_header   header; //Header ruteador DPS
	   char FRNAME	[	4],	//Nombre nodo emisor	ej."DNET" : Datanet
			TONAME	[	4],	//Nombre nodo receptor	ej."BSUD" : Bansud
			RECTYPE	[	3],	//Tipo de registro	TDE = debito TCE = credito
			ACTIVITY[   1];	//Actividad	"N"
	   char COUNTER [   2];	//Longitud del registro	100 (X'0064')
	   char	FILLER3	[	6],	//Reservado	Uso Datanet
			BANDEB	[	3],	//Codigo Banco segun BCRA	"067" si corresponde
			FECSOL	[	8],	//Fecha de compensacion	Del dia o prox. Habil -aaaammdd-
			NUMTRA	[	7],	//Numero de transferencia	
			NUMABO	[	7],	//Abonado	
			IMPORTE	[  17],	//Importe de la transferencia	15 enteros y 2 decimales
			BANCRE	[	3],	//Codigo Banco segun BCRA	"067" si corresponde
			ESTADO	[	2],	//Estado en que queda la operacion	Ver tabla IV
			OPEEJ1	[	2],	//Primer operador que autoriza el cambio de estado	"99"
			OPEEJ2	[	2],	//Segundo operador que autoriza el cambio de estado	"00"
			CODRCH	[	4],	//Motivo del rechazo del Banco de debito	Ver tabla III
			CODMAC	[  12],	//Message Authentication Code (MAC)	Ver Anexo A
			FECPRO	[	6],	//Fecha de proceso	
			FILLER	[  27];	//Reservado
		} data;
	};


//////////////////////////////////////////////////////////////////////////////
// Maxima longitud de importe
#define DPS_MAX_AMMOUNT_LENGTH	(17)

//////////////////////////////////////////////////////////////////////////////
// Tipos de TRANSACTION PROGRAMS utilizables por DPS
#define DPS_TPNAME_DPLT	    "DPLT" //	Transacción de inicialización.
#define DPS_TPNAME_DPSE     "DPSE" //	Transacción de envío de registros.
#define DPS_TPNAME_DPRE 	"DPRE" // 	Transacción de recepción de registros.
#define DPS_TPNAME_DPSM 	"DPSM" // 	Transacción de monitoreo del sistema.
#define DPS_TPNAME_DPSS 	"DPSS" // 	Transacción de cambio de estado del sistema.
#define DPS_TPNAME_DPIR 	"DPIR" // 	Transacción de interfase de registros recibidos.
#define DPS_TPNAME_LOGD 	"LOGD" //	Transacción de display de los mensajes del sistema.
#define BATCH_TPNAME_ONLINE	"BT01" // 	Transacción de refresh online 
#define DPS_TPNAME_DEFAULT  DPS_TPNAME_DPRE

///////////////////////////////////////////////////////////////////////////////
// Respuestas de los programas de CONVERSACION del EBDPSER
#define DPS_SNA_CONVERSE_ABEND    ""
#define DPS_SNA_CONVERSE_NORMAL_N "NXXXX" // ERROR, donde "XXXX" indica un nro de error
#define DPS_SNA_CONVERSE_NORMAL_Y "Y"     // OK, recibido

///////////////////////////////////////////////////////////////////////////////
//Tipo de registro	TDE = debito TCE = credito
#define DPS_RECTYPE_DEBITO_EJECUTADO	"TDE"
#define DPS_RECTYPE_CREDITO_EJECUTADO	"TCE"
#define DPS_RECTYPE_DEBITO_RESPUESTA	"TDR"
#define DPS_RECTYPE_CREDITO_RESPUESTA	"TCR"

//////////////////////////////////////////////////////////////////////////////
// Tabla 0 - RECTYPE - Tipo de Registro Numerico
// Descripcion = Codigo
//////////////////////////////////////////////////////////////////////////////
typedef enum e_rectype_dps_tag {
    DPS_DEBITO_EJECUTADO	 = 1,
    DPS_CREDITO_EJECUTADO	 = 2,
    DPS_DEBITO_RESPUESTA	 = 3,
    DPS_CREDITO_RESPUESTA	 = 4,
    DPS_DEBITO_CREDITO_INVAL = 999
    } e_rectype_dps;

//////////////////////////////////////////////////////////////////////////////
// Tabla I - TIPOPE - Tipo de Operacion
// Descripcion = Codigo
//////////////////////////////////////////////////////////////////////////////
typedef enum e_tipoope_dps_tag {
		DPS_CUENTAS_PROPIAS		= 1,
		DPS_SUELDOS				= 2,
		DPS_PROVEEDORES			= 3,
		DPS_ANA_TRADICIONAL		= 4,
		DPS_ANA_SISTEMA_MARIA	= 5,
		DPS_DGI					= 6,
		DPS_FACTURACION			= 7, // Movimiento de facturacion generado automaticamente
		DPS_CUENTA_RECAUDADORA	= 8, // Movimiento automatico generado a pedido de los Bancos de debito y credito para transferir fondos de la cuenta pozo a la cuenta del abonado.
		DPS_SERVICIOS			= 9,
		DPS_TESTING				= 10,
		DPS_NO_ASIGNADO_11		= 11,
		DPS_INTERPAGOS			= 12, // Producto B to B ofrecido por Interbanking.
		DPS_PAGO_DE_SEGUROS		= 13,
		DPS_PAGOS_AFIP /*NO_ASIGNADO_14*/		= 14,
		DPS_TRANSFERENCIAS_CCI	= 15, // CCI = Camara Compensadora Interbanking,  que permite Transferencias de los Bancos por cuenta y orden de terceros (Clientes no abonados al Servicio).
		DPS_NO_ASIGNADO_16		= 16,
		DPS_NO_ASIGNADO_17		= 17,
		DPS_NO_ASIGNADO_18		= 18,
		DPS_NO_ASIGNADO_19		= 19
    } e_tipoope_dps;

//////////////////////////////////////////////////////////////////////////////
// Tabla II - TIPCUE - Tipos de Cuenta
// Descripcion = Codigo
//////////////////////////////////////////////////////////////////////////////
typedef enum e_tipcue_dps_tag {
	DPS_CUENTA_CORRIENTE_ARG = 1 ,
	DPS_CAJA_DE_AHORROS_ARG  = 2 ,
	DPS_CUENTA_CORRIENTE_USD = 13,
	DPS_CAJA_DE_AHORROS_USD  = 15
    } e_tipcue_dps;

#define  _E_TIPCUE_DPS_TAG_

//////////////////////////////////////////////////////////////////////////////
// Tabla III- Motivos de rechazo
// Descripcion = Codigo
//////////////////////////////////////////////////////////////////////////////
typedef enum e_motrech_dps_tag {
	DPS_APROBADA_SIN_RECHAZO			= 0,
	DPS_CUENTA_INVALIDA					= 1,
	DPS_CTA_CRE_Y_NOM_NO_COINCIDEN		= 2,
	DPS_CUENTA_EN_VIA_DE_CIERRE			= 3,
	DPS_EXCEDE_LIMITE_CH_RECHAZADOS		= 4,
	DPS_CTA_CERRADA_SOLICITUD_CLIENTE	= 5,
	DPS_CTA_CERRADA_SOLICITUD_BANCO		= 6,
	DPS_CTA_CERRADA_SOLICITUD_BCRA		= 7,
	DPS_CUENTA_CERRADA					= 8,
	DPS_NO_RESPONDE_CONSULTA_ESA_SUC	= 9,
	DPS_SALDO_INSUFICIENTE				= 10,
	DPS_CUENTA_EMBARGADA				= 11,
	DPS_CORRESPONDE_IMPUESTO			= 12,
	DPS_SISTEMA_NO_DISPONIBLE			= 13,
	DPS_APLICACION_NO_DISPONIBLE		= 14,
	DPS_SUCURSAL_INVALIDA				= 15,
	DPS_UTILIZA_SALDOS_NO_CONFORMADOS	= 16,
	DPS_CTA_NO_DEBITABLE_EN_TRANSFERENCIA	= 17,
	DPS_CUENTA_NO_CORRESPONDE_A_SUCURSAL	= 18,
	DPS_FALTAN_DATOS_EN_DETALLE_OPERACION	= 19,
	DPS_OPERACION_INEXISTENTE				= 20,
	DPS_OPERACION_NO_HABILITADA_EN_BANCO	= 21,
	DPS_CUENTA_CREDITO_DE_TERCEROS			= 22,
	DPS_FUERZA_MAYOR						= 24,
	DPS_OPER_RECHAZADA_BANCO_CREDITO		= 25,
	DPS_TIPO_DE_CUENTA_INVALIDA				= 26,
	DPS_EXCEDE_CANTIDAD_DE_EXTRACCIONES		= 27,
	DPS_NUMERO_DE_OBLIGACION_ERRONEA		= 28,
	DPS_NO_COINCIDE_FORMA_DE_PAGO			= 29,
	DPS_NO_COINCIDE_IMPORTE_DECLARADO		= 30,
	DPS_CUENTA_BLOQUEADA					= 31,
	DPS_POR_SOLICITUD_DEL_CLIENTE			= 32,
	DPS_MAC_ERRONEO							= 33,
    /*********************************************/
    DPS_CUENTA_NO_HABILITADA_BANCO          = 34,
    DPS_IMP_EXP_INHABILITADO                = 35,
    DPS_DESPACHANTE_INHABILITADO            = 36,
    DPS_TEF_INTRABANCO_SIN_COMISION         = 37,
    DPS_ERROR_EN_CTA_CREDITO                = 38,
    DPS_SALDO_LIBRE_DISP_INSUFICIENTE       = 39,
    DPS_MAC_ERRONEO_RED_NO_CORRESP_MOVIM    = 40,
    /*********************************************/
    DPS_MAC_ERRONEO_NETWORK					= 40,
	DPS_PASADA_A_PROCESO_BATCH          	= 997, // CASOS QUE SE RESUELVE EN EL BATCH
    DPS_DEMORADA_NO_ENVIAR_RESPUESTA    	= 998, // CASOS PARA LOS QUE NO SE ENVIA RTA.
    DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED	= 999  // CASOS PARA LOS QUE NO SE ENVIA RTA.
    } e_motrech_dps;

//////////////////////////////////////////////////////////////////////////////
// VALORES POR OMISION de codigos de rechazo
#define DPS_MOTIVO_RECHAZO_DEFAULT	    (DPS_OPERACION_NO_HABILITADA_EN_BANCO)
#define DPS_MOTIVO_RECHAZO_DEFAULT_STR	"0021"

//////////////////////////////////////////////////////////////////////////////
// Tabla IV- Estados
// Descripcion = Codigo
//////////////////////////////////////////////////////////////////////////////
typedef enum e_codestad_dps_tag {
	DPS_ENVIADO_POR_EL_ABONADO_						= 0,
	DPS_ENVIADA_AL_BANCO_DE_DEBITO					= 10, //(*)
	DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO				= 20, //(1)
	DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED		= 30, //(2)
	DPS_VALOR_AL_COBRO								= 40, //(TRATAMIENTO_PENDIENTE)(3)
	DPS_ENVIADA_AL_BANCO_DE_CREDITO					= 50, //(*)
	DPS_EJECUTADA									= 60,
	DPS_RECHAZO_DEL_BANCO_DE_DEBITO					= 70,
	DPS_RECHAZO_DEL_BANCO_DE_CREDITO				= 80, //(4)
	DPS_RECHAZO_CUENTA_CREDITO						= 90,
	DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED    		= 999 // CASOS PARA LOS QUE NO SE ENVIA RTA.
    } e_codestad_dps;

//////////////////////////////////////////////////////////////////////////////
// Codigos de Estado (tabla IV) en formato ASCII string
//////////////////////////////////////////////////////////////////////////////
#define _DPS_ENVIADO_POR_EL_ABONADO_				  "00"
#define _DPS_ENVIADA_AL_BANCO_DE_DEBITO_			  "10"
#define _DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO_		  "20"
#define _DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED_	  "30"
#define _DPS_VALOR_AL_COBRO_						  "40"
#define _DPS_ENVIADA_AL_BANCO_DE_CREDITO_			  "50"
#define _DPS_EJECUTADA_								  "60"
#define _DPS_RECHAZO_DEL_BANCO_DE_DEBITO_			  "70"
#define _DPS_RECHAZO_DEL_BANCO_DE_CREDITO_			  "80" 

//////////////////////////////////////////////////////////////////////////////
// Codigos de Estado Tratamiento (tabla de uso interno) 
//////////////////////////////////////////////////////////////////////////////
#define _DPS_TRAT_PENDIENTE_     "0" // PENDIENTE de tratamiento 
#define _DPS_TRAT_SOBREG_APROB_  "1" // Con sobregiro APROBADO y pendiente de aplicación y respuesta. 
#define _DPS_TRAT_SOBREG_RECHA_  "2" // Con sobregiro RECHAZADO y pendiente de rechazo y respuesta. 
#define _DPS_TRAT_AUT_APROB_	_DPS_TRAT_SOBREG_APROB_ // Con sobregiro APROBADO y pendiente de aplicación y respuesta. 
#define _DPS_TRAT_AUT_RECHA_    _DPS_TRAT_SOBREG_RECHA_ // Con sobregiro RECHAZADO y pendiente de rechazo y respuesta. 
#define _DPS_TRAT_REVERSOS_EXCE_ "3" // Reversas de débitos que, según Datanet, SUPERAN límite de Riesgo Banco y están pendientes de respuesta a la red. 
#define _DPS_TRAT_DEMORADA_      "4" // Debitos demorados que reintentan
#define _DPS_TRAT_REQ_AUTORIZ_   _DPS_TRAT_DEMORADA_
#define _DPS_TRAT_PEND_PROX_DIA_ "8" // Valores al cobro APROBADOS EN EL DIA y PENDIENTES de aplicación para el día  siguiente.                     
#define _DPS_TRAT_PROCESADA_     "9" // Procesada  (toda Transferencia rechazada de antemano y respondida DEBE ser insertada en TEF_ONLINE con ESTADO_TRATAMIENTO = 9).
//////////////////////////////////////////////////////////////////////////////
#define _DPS_TRAT_PENDIENTE_I_     0 // PENDIENTE de tratamiento 
#define _DPS_TRAT_SOBREG_APROB_I_  1 // Con sobregiro APROBADO y pendiente de aplicación y respuesta. 
#define _DPS_TRAT_SOBREG_RECHA_I_  2 // Con sobregiro RECHAZADO y pendiente de rechazo y respuesta. 
#define _DPS_TRAT_AUT_APROB_I_	  _DPS_TRAT_SOBREG_APROB_I_ // Con sobregiro APROBADO y pendiente de aplicación y respuesta. 
#define _DPS_TRAT_AUT_RECHA_I_    _DPS_TRAT_SOBREG_RECHA_I_ // Con sobregiro RECHAZADO y pendiente de rechazo y respuesta. 
#define _DPS_TRAT_REVERSOS_EXCE_I_ 3 // Reversas de débitos que, según Datanet, SUPERAN límite de Riesgo Banco y están pendientes de respuesta a la red. 
#define _DPS_TRAT_DEMORADA_I_      4 // Debitos demorados que reintentan
#define _DPS_TRAT_REQ_AUTORIZ_I_  _DPS_TRAT_DEMORADA_I_
#define _DPS_TRAT_PEND_PROX_DIA_I_ 8 // Valores al cobro APROBADOS EN EL DIA y PENDIENTES de aplicación para el día  siguiente.                     
#define _DPS_TRAT_PROCESADA_I_     9 // Procesada  (toda Transferencia rechazada de antemano y respondida DEBE ser insertada en TEF_ONLINE con ESTADO_TRATAMIENTO = 9).
//////////////////////////////////////////////////////////////////////////////
#define _DPS_TRAT_PENDIENTE_C_      '0' // PENDIENTE de tratamiento 
#define _DPS_TRAT_SOBREG_APROB_C_   '1' // Con sobregiro APROBADO y pendiente de aplicación y respuesta. 
#define _DPS_TRAT_SOBREG_RECHA_C_   '2' // Con sobregiro RECHAZADO y pendiente de rechazo y respuesta. 
#define _DPS_TRAT_AUT_APROB_C_      _DPS_TRAT_SOBREG_APROB_C_ // Con sobregiro APROBADO y pendiente de aplicación y respuesta. 
#define _DPS_TRAT_AUT_RECHA_C_      _DPS_TRAT_SOBREG_RECHA_C_ // Con sobregiro RECHAZADO y pendiente de rechazo y respuesta. 
#define _DPS_TRAT_REVERSOS_EXCE_C_  '3' // Reversas de débitos que, según Datanet, SUPERAN límite de Riesgo Banco y están pendientes de respuesta a la red. 
#define _DPS_TRAT_DEMORADA_C_       '4' // Debitos demorados que reintentan
#define _DPS_TRAT_REQ_AUTORIZ_C_    _DPS_TRAT_DEMORADA_C_
#define _DPS_TRAT_PEND_PROX_DIA_C_  '8' // Valores al cobro APROBADOS EN EL DIA y PENDIENTES de aplicación para el día  siguiente.
#define _DPS_TRAT_PROCESADA_C_      '9' // Procesada  (toda Transferencia rechazada de antemano y respondida DEBE ser insertada en TEF_ONLINE con ESTADO_TRATAMIENTO = 9).


//////////////////////////////////////////////////////////////////////////////
// Codigos de Pendiente de ENVIO (Si,No,Diferida)
//////////////////////////////////////////////////////////////////////////////
#define _DPS_PEND_ENV_DEFAULT_      " " // PENDIENTE POR OMISION
#define _DPS_PEND_ENV_SI_           "S" // PENDIENTE 
#define _DPS_PEND_ENV_NO_           "N" // NO PENDIENTE
#define _DPS_PEND_ENV_DIFERIDA_     "D" // DIFERIDA PROX DIA (LA CAMBIA EL BATCH)
#define _DPS_PEND_ENV_FALLIDA_      "F" // FALLIDA (FALTA DATO BANCO-ABONADO)
//////////////////////////////////////////////////////////////////////////////
#define _DPS_PEND_ENV_DEFAULT_C_    ' ' // PENDIENTE POR OMISION
#define _DPS_PEND_ENV_SI_C_         'S' // PENDIENTE 
#define _DPS_PEND_ENV_NO_C_         'N' // NO PENDIENTE
#define _DPS_PEND_ENV_DIFERIDA_C_   'D' // DIFERIDA PROX DIA (LA CAMBIA EL BATCH)
#define _DPS_PEND_ENV_FALLIDA_C_    'F' // FALLIDA (FALTA DATO BANCO-ABONADO)
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Observaciones :
//(*)  Uso interno Datanet
//(1)	Indica al cliente que el Banco esta analizando el pago.
//(2)	El Banco imputo el debito y le respondio a Datanet con estado "60". 
//      Datanet, al recalcular el Riesgo Red y por excederlo reenvia el movimiento para que 
//      el Banco reverse el debito. El Banco queda en condicion de imputarla y reenviarla con 
//      posterioridad por haber incrementado el tope asignado o por haber recibido creditos 
//      que hayan variado su saldo en el Sistema.
//(3)	El debito ejecutado que excede el Riesgo Banco (fijado por el Banco de credito para 
//      operar con el Banco de debito), provoca que el credito sea enviado como Valor al cobro
//      (ESTADO = 40 y RIEBCO = S). Lo mismo ocurre si el Banco de credito exige el 
//      tratamiento del pago (ESTADO = 40 y RIEBCO = N).
//(4)	Notifica al Banco de debito que el credito fue rechazado por el Banco de Credito para
//      que efectue sus ajustes.
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//Tabla V- Contenido de OBSER2
//TIPOPE	OBSER2	Formato
/////////////////////////////////////////////////////////////////////
//03	Tipo de documento a cancelar Codigo de documento a cancelar 
//		Tipo de orden de pagoCodigo de cliente (interno de la empresa)
//		Tipo de retencionImporte de retencionCodigo de nota de credito
//		Importe de nota de credito	
//		N,3X,12X,2X,12X,2N,12 (2d)X,2N,10 (2d)
//05	Codigo de ANA
//		Cuit importador/exportador
//		Cuit despachanteCuit abonado
//		Situacion frente al IVA
//		Comision del Banco
//		IVA sobre la comision
//		Numero de transaccion	
//		N,3N,11N,11N,11X,2N,13 (2d)N,13 (2d)N,7
//06	Numero de obligacion
//		Razon social
//		Domicilio	
//		N,11X,24X,25
//09	Tipo/empresa de servicio
//		Identificador de servicio
//		Tipo de documento a cancelar
//		Codigo de documento a cancelar
//		Tipo de orden de pago
//		Codigo de orden de pago
//		Codigo de cliente
//		Tipo de retencion 1
//		Importe de retencion 1
//		Tipo de retencion 2
//		Importe de retencion 2	
//		X,4X,20X,2X,12X,2X,12X,12X,2N,12 (2d)X,2N,12 (2d)
/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Formato DPS i-o.    
// Mensaje DPS (OUTPUT - INPUT)
//////////////////////////////////////////////////////////////////////////////

class DPS_in_out {

public :

	// Constructor
	EXPORT DPS_in_out();

	// Metodos Publicos generales de validacion
	boolean_t IsDPSTransaction();
	boolean_t IsDPSCreditTransfer();
	boolean_t IsDPSDebitTransfer();
	boolean_t IsDPSCreditAndDebitTransfer();
	boolean_t IsDPSAccountVerification();
	boolean_t IsDPSOtherMessages();
    boolean_t IsRefreshMovement();
	boolean_t IsValidReversal();    
	// De establecimiento de respuesta
	boolean_t FormatResponseMessage(PCHAR pszMAC2, PCHAR pszMAC3);
    WORD      GetStatusCode( );        
    // Establecer nuevo ESTADO, con indicativo de BANCO inexistente ya reincoporado
	boolean_t SetStatusCode(WORD  wRespCode ,
                            boolean_t bAPROBADA_FALTAN_DATOS_EN_OPERACION = is_false);    
    // Establecer nuevo ESTADO, con indicativo de BANCO inexistente ya reincoporado
	boolean_t SetStatusCode(PCHAR szRespCode ,
                            boolean_t bAPROBADA_FALTAN_DATOS_EN_OPERACION = is_false);
	boolean_t SetCauseCode( WORD  wCauseCode, boolean_t bIsCredit );
    boolean_t GetCauseCode( char szRECHCR[], char szRECHDB[] );
	short     GetOperType( PCHAR szOperType );
	boolean_t GetMAC( PCHAR pszMAC );
	// Establece el tipo de registro a responder
	boolean_t SetRecType( PCHAR szRecType );
    // Recupera el tipo de registro a procesar o siendo procesado
    e_rectype_dps GetRecType();
    // Esta en un estado final 60,70 u 80 ?
    boolean_t IsResponseCodeInFinalState( );

	// De copia y validacion externas
	EXPORT boolean_t Import( PBYTE pcbBuffer, WORD iBuffLen );
	EXPORT boolean_t Export( PBYTE pcbBuffer, PWORD piBuffLen );
	EXPORT boolean_t IsValidMessage( void );

protected :
	// Miembros protegidos
	DPS_i	input;
	DPS_o	output;

	// Armar la cadena de entrada a la funcion de generacion de MACs
	// Posteriormente se invocara a NDES y se calculara la MAC
	PCHAR     GetMACTextToNDES( PCHAR pszMACinputNDES, SHORT iLen, 
                                SHORT iMAC_type , PSHORT pshBlocks,
                                char chValidateOrCalculate );
	boolean_t SetMACFromNDES( PCHAR pszMACoutputNDES, SHORT iMAC_type );
    // Lectura parametrica desde archivo CFG
    int       ReadString(char * lpValueName, char * lpData, int maxlen);

	// Estructura parametro de calculo de la MAC 1, 2 o 3
	typedef struct macData_tag {
		char   BANDEB            [  3];                      
		char   FECSOL            [  8];                      
		char   NUMABO            [  7];                      
		char   IMPORTE           [ 17]; // PIC 9(15)V99
		char   WNOMSOL           [ 15];                      
		char   TIPCUE            [  2];                      
		char   NUMCTA            [ 17];                      
		char   BANCRE            [  3];                      
		char   WNOMBEN           [ 15];                      
		char   TIPCRE            [  2];                      
		char   CTACRE            [ 17];                      
		char   OPEAU1            [  2];                      
		char   OPEAU2            [  2];                      
		char   OPEAU3            [  2];                      
		char   OBSER1            [ 60];                      
		char   OBSER2            [ 100];                     
		char   NUMREF            [  7];                      
		char   NUMENV            [  3];                      
		union {
			char VARIABLES         [ 22];
			struct {
				char  RESREG_1      [  6];           
				char  FILLER_1      [ 16];           
			} stMAC1;
			struct {
				char  NUMTRA        [  7];           
				char  OPEDB1        [  2];           
				char  OPEDB2        [  2];           
				char  ESTADO        [  2];           
				union {
					struct {
						char  RESREG_D  [  1];           
						char  FILLER_2  [  8];           
					} stMAC2_DEB; 
					struct { 
						char  OPECR1    [  2];           
						char  OPECR2    [  2];           
						char  RESREG_C  [  5];           		
					} stMAC2_CRE;
				} uMAC2;
			} stMAC2;			
		} varMAC;
		char   FILLER            [ 720];          
    } stMAC;		
    macData_tag macData;		

	// MACs calculadas o copiadas
	CHAR	achMAC1[12],
			achMAC2[12],
			achMAC3[12];

    /////////////////////////////////////////////////
    // Marca de Controles, si se hacen o no
    BOOL    bCONTROL_MAC_123					,
            bCONTROL_BANK_EXISTENCE			    ,
            bCONTROL_ACCOUNT_EXISTENCE			,
            bCONTROL_ACCOUNT_STATUS				,
            bCONTROL_ASSOCIATE_EXISTENCE		,
            bCONTROL_POSTING_DATE				;
    /////////////////////////////////////////////////
	
	// Clases amigas con acceso a miembros protegidos
	friend class TrxResDATANET;
	friend class TrxResCOBIS;
	friend class TrxResJERONIMO;
	friend class TrxResON2;
	friend class DPS_i_batch;
	friend class TrxResBATCH;
	friend class TrxResBTF;

	};

///////////////////////////////////////////////////////////////////////////////
//Tipo de registro	REFRESH ONLINE
#define DPS_RECTYPE_CUENTAS_CTA	    "CTA"
#define DPS_RECTYPE_COBRANZAS_COB	"COB"
#define DPS_RECTYPE_PAGOS_PGO   	"PGO"
#define DPS_RECTYPE_COBRANZAS_CCB	"CCB"
#define DPS_RECTYPE_PAGOS_CCP	    "CCP"


//////////////////////////////////////////////////////////////////////////////
// Formato REFRESH MOVIMIENTOS DATANET.    
// Nombre del Campo	Formato	Descripcion
//////////////////////////////////////////////////////////////////////////////
class DPIFCOMM {

public :
	EXPORT DPIFCOMM();
	EXPORT DPIFCOMM( const char *cszMessage , short shSize );
	EXPORT ~DPIFCOMM();	

	// De establecimiento de respuesta
	boolean_t FormatResponseMessage(LPCSTR pcszRECTYPE);

	// Mov.Cuentas
	typedef struct CTA_tag {
		char CODBAN [ 3];
		char NUMCTA [17];
		char FEIMP  [ 6];
		char NROCOR [12];
		char DEBCRE;
		char IMPORTE[17]; // PIC 9(15)V99
		char NROCOM [12];
		char DIFMOV;
		char CODOPE [ 3];
		char HORMOV [ 6];
		char SUCORI [ 5];
		char FECPRO [ 6];
		char CONTRA;
		char TIPCUE [ 2];
		char DEPOSI [ 8];
		char DESCRI [25];
		char FILLER [20];
		} dataCTA;
	// Mov.Cobranzas
	typedef struct COB_tag {
		char BANK		[ 3];
		char ACCOUNT	[17];
		char NUMEDI		[23];
		char FEPRO		[ 6];	
		char MONEDA		[ 3];
		char TIPO		;
       		//	DPIF_FACTURA		'F';
			//	DPIF_NOTA_CREDITO	'C';
			//	DPIF_BONIFICACION	'B';
			//	DPIF_REVERSO		'R';
		char FEIMP		[ 6];
		char CLEARING	;
			//	DPIF_EFECTIVO	  '0';
			//	DPIF_HORAS_24     '1';
			//	DPIF_HORAS_48	  '2';
			//	DPIF_HORAS_72	  '3';
			//	DPIF_HORAS_96	  '4';
			//	DPIF_HORAS_120	  '5';
			//  DPIF_VALOR_COBRO  '9';
		char NRO_CHEQUE	[ 4];	
		char COD_CLIENTE[10];
		char NOM_CLIENTE[24];
		char FACTURA	[16];
		char VL_COBRO	;	
		char IMPORTE	[18]; // PIC S9(16)V99	
		char SUCURSAL	[ 3];	
		char DESCRI		[79];	
		char FILLER		[20];
		} dataCOB;
	// Mov.Pagos
	typedef struct PGO_tag {
		char BANK			 [ 3];
		char ACCOUNT         [17];
		char NUMEDI			 [23];
		char FECHA_EMISION	 [ 6];
		char MONEDA          [ 3];
		char TIPO_PAGO      ;
		char FEIMP			 [ 6];
		char FLOATING        [ 3];
		char COD_CLIENTE     [10];
		char FACTURA         [12];
		char REFER	         [12];
		char REVERSO		;
		char IMPORTE	     [18]; // PIC S9(16)V99
		char DESCRI		 	 [79];
		char FILLER			 [20];
	} dataPGO;
	// Mov.Cuentas y Cobranzas
	typedef struct CCB_tag {
		char	CODBAN      [ 3];
 		char	NUMCTA 	    [17];
		char	NUMEDI		[23];
		char	FEIMP		[ 6];
		char	NROCOR      [12];
		char	DEBCRE	    ;
		char    IMPORTE	    [18]; // PIC S9(16)V99
		char	NROCOM    	[12];
		char	DIFMOV	    ;
		char	CODOPE      [ 3];
		char	HORMOV	    [ 6];
		char	FECPRO      [ 6];
		char	CONTRA      ;
		char	TIPCUE		[ 2];
		char	DEPOSI		[ 8];
		char	MONEDA		[ 3];
		char	CCB_TIPO	;
 		   	//	DPIF_FACTURA				 'F';
			//	DPIF_NOTA_CREDITO			 'C';
			//	DPIF_BONIFICACION			 'B';
			//	DPIF_REVERSO				 'R';
		char	CLEARING	;
			//	DPIF_EFECTIVO				 '0';
			//	DPIF_HORAS_24      			 '1';
			//	DPIF_HORAS_48				 '2';
			//	DPIF_HORAS_72				 '3';
			//	DPIF_HORAS_96				 '4';
			//	DPIF_HORAS_120				 '5';
			//  DPIF_VALOR_COBRO			 '9';
		char	NRO_CHEQUE	[ 4];	
		char	COD_CLIENTE	[10];
		char	NOM_CLIENTE	[24];
		char	FACTURA		[16];
		char	VL_COBRO	;	
		char	SUCURSAL	[ 3];
		char	DESCRI		[79];	
		char	FILLER		[20];		
	} dataCCB;

	// Mov.Cuentas y Pagos
	typedef struct CCP_tag {
 		char CODBAN    		 [3];
    	char NUMCTA    		 [17];
		char NUMEDI			 [23];
		char FEIMP			 [6];
		char NROCOR     	 [12];
		char DEBCRE	    	;
		char IMPORTE	     [18]; // PIC S9(16)V99
		char NROCOM    	     [12];
		char DIFMOV	        ;
		char CODOPE          [3];
		char HORMOV	         [6];
		char SUCORI			 [5];
		char FECPRO          [6];
		char CONTRA         ;
		char TIPCUE			 [2];
		char DEPOSI			 [8];
		char FECHA_EMISION   [6];
		char MONEDA          [3];
		char TIPO_PAGO      ;
		char FLOATING        [3];
		char COD_CLIENTE     [10];
		char FACTURA	     [12];
		char DESCRI			 [79];                     
		char FILLER			 [20];
	} dataCCP;

	// Estructura en comun para REFRESH ONLINE
	struct {
       DPS_header   header; //Header ruteador DPS-RefreshOnline
	   char FRNAME	[	4],	//Nombre nodo emisor	ej."DNET" : Datanet
			TONAME	[	4],	//Nombre nodo receptor	ej."BSUD" : Bansud
			RECTYPE	[	3],	//Tipo de registro	CTA,COB,PGO,CCB,CCP
			ACTIVITY[   1];	//Actividad	"N"
       char COUNTER [   2];	//Longitud del registro	145=CTA,235=COB,214=PGO,281=CCB,258=CCP
	   char FILLER	[   6];	//Reservado
	   union buffer_tag {
		   dataCOB	COB;
		   dataCTA	CTA;
		   dataPGO  PGO;
		   dataCCB  CCB;
		   dataCCP  CCP;
		   char BUFFER [ 4000]; //REDEFINE-buffer
		} BUFFER;
	} data;

	// Clases amigas con acceso a miembros protegidos
	friend class TrxResCOBIS;
	friend class TrxResDATANET;
    friend class TrxResJERONIMO;
	friend class TrxResON2;

	friend class XML_CONNECTOR;
	friend class XML_CONNECTOR_DNET;

	}; // DPIFCOMM

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Formato REFRESH ABONADOS-CUENTAS DESDE DATANET HACIA EL BANCO.    
// Nombre del Campo	Formato	Descripcion
//////////////////////////////////////////////////////////////////////////////
class DPIBCOMM {

public :

	/* INTERFASE BATCH DE CUENTAS */
	typedef struct ABOCTA_tag 
	{      
              char CODBAN		 [3];
              char NUMABO		 [7];
              char TIPCUE		 [2];
              char NCUECM		 [2];
              char NUMCTA		 [17];
              char TABOFF_TIPOFF [5];	/* TABLA DE OFERTAS : 5 OCURRENCIAS */
              char FILLER		 [4];
	};
    ABOCTA_tag  data;

	/*************************************************************
     *	Descripción del contenido de los campos
	 *************************************************************
		CODBAN	:	codigo de banco segun BCRA
		NUMABO	:	numero de abonado (nro. de cliente)
		TIPCU		:	tipo de cuenta segun tabla 04
		NCUECM	:	numero de cuenta corto
		NUMCTA	:	numero de cuenta segun banco
		TIPOFF	:	código de offering contratado por el cliente
	 *************************************************************/

	}; // DPIBCOMM



#define DPS_NDNAME_DATANET      "DNET" // NODO DE LA RED DATANET / DATACASH
#define DPS_NDNAME_BANSUD 	    "BSUD" // NODO BANCO BANSUD S.A.
#define DPS_NDNAME_MACRO 	    "BMAC" // NODO BANCO MACRO S.A.
#define DPS_NDNAME_MARIVA 	    "BMAR" // NODO BANCO MARIVA S.A.
#define DPS_NDNAME_INDUSTRIAL 	"BIND" // NODO BANCO INDUSTRIAL S.A. (IND. DE AZUL)
#define DPS_NDNAME_PRIVADO 	    "BPRI" // NODO BANCO PRIVADO S.A. (B.P.I.)
#define DPS_NDNAME_SUQUIA 	    "BSUQ" // NODO BANCO SUQUIA S.A.
#define DPS_NDNAME_BCBA 	    "BCIU" // NODO BANCO CIUDAD DE BS.AS.
#define DPS_NDNAME_FINANSUR	    "BFIN" // NODO BANCO FINANSUR

#define BATCH_EVOKE_BANSUD	    "P067" // Evoke APPC Bansud
#define BATCH_EVOKE_MACRO	    "P285" // Evoke APPC Macro 
#define BATCH_EVOKE_SUQUIA	    "P255" // Evoke APPC Suquia
#define BATCH_EVOKE_FINANSUR    "P303" // Evoke APPC Finansur


// Nodo siendo implementado (para Refresh Online)
#define DPS_NDNAME_DEFAULT  DPS_NDNAME_FINANSUR


#endif // _DATANET_H_





