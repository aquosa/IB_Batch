//////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A.
// Proyecto SIOP-atm. Estructura de EXTRACT BASE24 RELEASE 4.
//   
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2002.05.09      mdc     EXTRACT	BASE24 RELEASE 4
// (Inicial)    2002.06.04      mdc     REVERSAL REASONS 
// (Alfa)       2003.08.26      mdc     CREDIT CARD TRX (FILLER6 = ISO-FIELD-126)
// (Beta)       2005.07.18      mdc     Base24 Release 6. Proyecto COBIS-atm Banelco.
// (Beta)       2005.09.20      mdc     _CEXTR_TRANSFER_ACCOUNT_CBU_   (20) 
// (Beta)       2006.03.02      mdc     CEXTR_OPCODE_CREDIT_ORDER_XFER 
//
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _EXTRACT_STRUCT_
#define _EXTRACT_STRUCT_


/********************************************************************************************/
/* Esta es la descripcion de la estructura segun documentacion interna de BANCO BANSUD.     */
/********************************************************************************************/
/*

REGISTRO DE DATOS (extractado del TLF)

POSIC.
RELAT.  NIVEL  NOMBRE DE CAMPO       CONTENIDO DEL CAMPO                            TIPO   PICTURE

001-008	02		PREFIX	Caracteres adicionados.	AN	X(08)

		02		HEAD	HEADER.

009-027	04		DAT-TIM	Fecha y hora en que el registro	N	9(19)
					fue grabado.
					(JULIANTIMESTAMP)

028-029	04		REC-TYP	Tipo de registro.	AN	X(02)
					Los valores posibles son:

					'01' (Transaccion de cliente).
					'04' (Transaccion administrativa).
					'20' (Excepcion, dato invalido en 
					        el STM).
					'21' (Excepcion).
					'22' (Excepcion).

030-033	04		AUTH-PPD	Nombre (PPD) del proceso de 	AN	X(04)
					autorizacion.

		04		TERM	IDENTIFICACION DE LA TERMINAL.

034-037	06		LN	Nombre de la red logica asociada	AN	X(04)
					con la terminal (ATM).
		                                    Los valores posibles son los indicados
		                                    en el Capitulo V.8 Codigos del Sistema
					Redes asociadas con la terminal (atm)

038-041	06		FIID	Identifica la Institucion a la que	AN	X(04)
					pertenece la terminal.
					(Para transacciones HOME BANKING
					 el pagador sera identificado como LINK)

042-057	06		TERM-ID	Identificacion de la terminal que origino	AN	X(16)
					la transaccion.

		04		CRD	IDENTIFICACION DE LA TARJETA.

058-061	06		LN	Nombre de la red logica asociada	AN	X(04)
					con la tarjeta.
					Los valores posibles son los indicados
		                                    en el Capitulo V.8 Codigos del Sistema
					 Redes asociadas con la terminal (atm)

2-065	06		FIID	Identifica a la Institucion emisora	AN	X(04)
					de la tarjeta.

066-093	06		PAN	Numero de tarjeta, tal cual aparece	AN	X(28)
					en el TRACK 2 de la TARJETA.
					Esta alineado a izquierda y relleno
					con BLANCOS hacia la derecha.

094-096	06		MBR-NUM	Numero de miembro de la tarjeta.	N	9(03)
					Si no existe, se informa CEROS.

097-100	04		BRCH-ID	Identifica la SUCURSAL asociada a	AN	X(04)
					la terminal.

101-104	04		REGN-ID	Identifica la REGION asociada a la	AN	X(04)
					terminal.

105-106	04		USER-FLD1X	Se informa CEROS.	AN	X(02)

		02		AUTH	TRANSACCION FINANCIERA.

107-108	04		TYP-CDE	Codigo que identifica si un sobre	AN	X(02)
					fue utilizado en la transaccion
					(depositos).
					Los valores posibles son:

					'30' (Se utilizo sobre).
					'31' (No se utilizo sobre).

109-112	04		TYP	Tipo de mensaje asociado con el	N	9(04)
					registro.
					Los valores posibles son:

					'0210' (Respuesta de Autorizacion).
					'0220' (Mensaje forzado. Acreditac.
					           transf. interbancarias).
					'0420' (Reversa).


113-114	04		RTE-STAT	Estado del host habilitado para	N	9(02)
					procesar la transaccion (solo para 
					conexion host-to-host).
	
					Los valores posibles son:

					'00' (Sin error).
					'11' (Host no disponible).
					'12' (Linea caida).
 

115-115	04		ORIGINATOR	Indica donde fue originada la	AN	X(01)
					transaccion.

116-116	04		RESPONDER	Indica donde se responde la	AN	X(01)
					transaccion que fue generada.

117-135	04		ENTRY-TIM	Timestamp del sistema Tandem	N	9(19)
					de cuando la transaccion ingresa
					a Base24, expresado en GMT.

136-154	04		EXIT-TIM	Timestamp de cuando Base24	N	9(19)
					transmite el requerimiento a la 
					entidad autorizadora, expresado
					en GMT.

155-173	04		RE-ENTRY-TIM	Timestamp de cuando Base24	N	9(19)
					recibe la respuesta del requeri-
					miento original desde la entidad
					autorizadora, expresado en GMT.

174-179	04		TRAN-DAT	Fecha calendario de la transaccion.	AN	X(06)
					El formato es AAMMDD.

180-187	04		TRAN-TIM	Hora de la transaccion.	AN	X(08)
					El formato es HHMMSSHH.

188-193	04		POST-DAT	Fecha de negocios de la transaccion.	AN	X(06)
					El formato es AAMMDD.

194-199	04		ACQ-ICHG-SETL-DAT	Fecha de la transaccion que sera	AN	X(06)
					seteada por la red originadora, si
					hubo una (CIRRUS, PLUS, etc.)
					en el procesamiento de la misma.
					El formato es AAMMDD.

200-205	04		ISS-ICHG-SETL-DAT	Fecha de la transaccion que sera	AN	X(06)
					seteada por la red autorizadora, si
					hubo una (CIRRUS, PLUS, etc.)
					en el procesamiento de la misma.
					El formato es AAMMDD.

206-217	04		SEQ-NUM	Numero de secuencia de la tran-	AN	X(12)
					saccion (numero de recibo en los
					ATM). Alineado a izquierda y relleno
					con BLANCOS hacia la derecha.


218-219	04	TERM-TYP	Tipo de terminal.	N	9(02)
		                                    Los valores posibles son los indicados
		                                    en el Capitulo V.9 Codigos del Sistema
			                                              Codigos de Tipo de Terminal 

220-224	04		TIM-OFST	Contiene la diferencia de tiempo 	N	9(05)	
					entre la terminal que inicia la tran-
					saccion y el procesador Tandem.
					Es el numero con signo de los
					minutos que se debe adicionar al
					sistema Base24 para obtener la
					hora de la terminal.

	225-235	04		ACQ-INST-ID-NUM	Numero interno de la institucion	N	9(11)
					pagadora.		

236-246	04		RCV-INST-ID-NUM	Numero interno de la institucion	N	9(11)
					emisora.		
		
	247-252	04		TRAN-CDE	Codigo de transaccion.	AN	X(06)

		04		TRAN-CDE-R	Redefines TRAN-CDE

247-248	06		T-CDE	Tipo de transaccion.	AN	X(02)
					Los valores posibles son:
					(Ver codigos de transacciones seccion V.2)    

249-250	06		T-FROM	Tipo de cuenta DESDE.	AN	X(02)
					 Los valores posibles son los indicados
		                                    en el Capitulo V.6 Codigos del Sistema
					 Codigos de Tipo de Tarjeta

251-252	06		T-TO	Tipo de cuenta HACIA.	AN	X(02)
					Los valores posibles son los indicados
		                                    en el Capitulo V.6 Codigos del Sistema
					 Codigos de Tipo de Cuenta

					
253-280	04		FROM-ACCT	Numero de cuenta DESDE.	AN	X(28)
					Alineado a izquierda y relleno con
					BLANCOS hacia la derecha.
					Si no es necesaria, se informa
					28 BLANCOS.


281-281	04		TIPO-DEP	Tipo de depositos.	AN	X(01)
					Los valores posibles son los indicados
		                                    en el Capitulo V.10 Codigos del Sistema
					Codigos de Tipos de Depositos

282-309	04		TO-ACCT	Numero de cuenta HACIA.	AN	X(28)
					Alineado a izquierda y relleno con
					BLANCOS hacia la derecha.
					Si no es necesaria, se informa
					19 CEROS y el resto BLANCOS).

310-310	04		MULT-ACCT	Se usa para diferenciar una tran-	N	9(01)
					saccion de extraccion normal de 
					una FAST-CASH.
					Los valores posibles son:

					'0' (La terminal no maneja multiples cuentas).
					'1' (La terminal maneja multiples cuentas).
					'2' (Indica que la transaccion es una
					      extraccion FAST-CASH).

311-329	04		AMT-1	Monto requerido de la transaccion.	N	9(19)
					Para transacciones correspondientes 
					a REVERSAS (valor del campo TYP
					igual a 0420) contiene el monto de
					la transaccion original.
					

330-348	04		AMT-2	Monto de la transaccion que depende	N	9(19)
					del tipo de transaccion.
					Para transacciones correspondientes
					a REVERSAS (valor del campo TYP
					igual a  0420), contiene el monto
					dispensado de la transaccion.
					Para transacciones correspondientes
					a RESPUESTAS (valor del campo 
					TYP igual a 0210), contiene el saldo
					contable.
					
349-367	04		AMT-3	Monto de saldo o credito disponible	N	9(19)
					dependiendo del tipo de operacion.

368-377	04		FILLER	Campo no utilizado.	AN	X(10)

378-378	04		DEP-TYP	Indica el tipo de buzon que debe	N	9(01)
					ser usado en el ATM dependiendo
					del objetivo del tipo de tarjeta que
					inicia la transaccion.
					Los valores posibles son:

					'0' (Sobre normal de deposito).
					'1' (Deposito comercial).

	379-381	04		RESP-CDE	Codigo de respuesta.	AN	X(03)

		04		RESP-CDE-R	Redefines RESP-CDE	

379-379	06		RESP-BYTE-1	Indicativo de tarjeta capturada.	AN	X(01)
					Los valores posibles son:

					'0' (No capturada).
					'1' (Capturada).

380-381	06		RESP-BYTE-2	Codigo de resultado de la operacion.	AN	X(02)
					Ver capitulo correspondiente.

382-406	04		TERM-NAME-LOC	Describe el nombre y localizacion 	AN	X(25)
					de la terminal para saber donde se 
					origino la transaccion. 

		04		NOMBRE-ENTE	Redefines TERM-NAME-LOC.	AN	X(25)
					Nombre del Ente. Se utiliza en P.A.S.

		04		INTER-CONTRA	Redefines TERM-NAME-LOC.	AN	X(25)
					Se utiliza en transacciones interban- 
					carias. En el caso de un debito indica
					los datos de la institucion a la que se 
					le debe realizar el credito y viceversa.

	382-385	06		FIID	Codigo de institucion	AN	X(04)

386-387	06		ACCT-TYP	Tipo de cuenta	AN	X(02)
					Los valores posibles son los indicados
		                                    en el Capitulo V.6 Codigos del Sistema
					Codigos de Tipo de Cuenta

	388-406	06		ACCT-NUM	Numero de cuenta.	AN	X(19)

 
407-428	04		TERM-OWNER-NAME	Nombre de la institucion financiera	AN	X(22)
					a la que pertenece la terminal.

		04		USERRPQ	Redefines TERM-OWNER-NAME.
					Se utiliza para P.A.S.

407-409	06		NRO-ENTE	Codigo del ente.	AN	X(03)
	
410-414	06		CUOTA	Cuota/ano abonada, con	AN	X(05)
					formato CCCAA.

415-428	06		FILLER	Campo no utilizado.	AN	X(14)

		04		CREDITO-CUOTA	Redefines TERM-OWNER-NAME.									Se utiliza para adelantos de efectivo
					en cuotas.

407-408	06		CANT-CTA	Cantidad de cuotas.	AN	X(02)

409-409	06		TIPO-TRAN	Se informa 1 para indicar que la
					transaccion es en cuotas.	AN	X(01)

410-428	06		FILLER	Campo no utilizado.	AN	X(19)

		04		NUM-CBU	Redefines TERM-OWNER-NAME.	N	9(22)
					Se utiliza para guardar el CBU.

		04		PAY-KEY	Redefines TERM-OWNER-NAME.
					Se utiliza para Pay Key.

407-409	06		COD-EMPRESA	Codigo de empresa.	AN	X(03)

410-412	06		COD-PRODUCTO	Codigo de producto.	AN	X(03)

413-428	06	FACTURA	Valor alfanumerico informado por el	AN	X(16)
					comercio que identifica la transaccion.
					(Nro. de Orden de Compra)

		04		MOBIL-BANKING	Redefines TERM-OWNER-NAME.

407-426	06		EMPRESA	Nombre de la empresa por la que se	AN	X(20)
					envia el mensaje.

427-427	06		FRECUENCIA	Codigo de frecuencia del mensaje.	AN	X(01)
					Los valores posibles son:

					'D' (Diaria).
					'S' (Semanal).

428-428	06		DIA	Codigo de frecuencia del mensaje.	AN	X(01)
					Si el campo FRECUENCIA contiene 
					'S' los valores posibles son:

					'1' (Dia Lunes).
					'2' (Dia Martes).
					'3' (Dia Miercoles).
					'4' (Dia Jueves).
					'5' (Dia Viernes).

		04		DONACIONES	Redefines TERM-OWNER-NAME.
					Se utiliza para Donaciones

407-409	06		ENTE	Codigo que identifica a la entidad de 	AN	X(03)
					bien publico	

410-428	06		FILLER	Campo no utilizado	AN	X(19)

429-441	04		TERM-CITY	Ciudad donde esta instalada la	AN	X(13)
					terminal.

442-444	04		TERM-ST-X	Codigo de provincia o estado donde	AN	X(03)
					se encuentra la terminal.

445-446	04		TERM-CNTRY-X	Codigo de pais donde se encuentra	AN	X(02)
					la terminal.

		04		ORIG	Tiene informacion sobre la transaccion
					original si esta involucrada otra red.

447-458	06		OSEQ-NUM	Numero de secuencia que identifica	AN	X(12)
					la transaccion original.
 
459-462	06		OTRAN-DAT	Fecha de transaccion original.	AN	X(04)

463-470	06		OTRAN-TIM	Hora de la transaccion original.	AN	X(08)

471-474	06		B24-POST-DAY	Fecha de negocios.	AN	X(04)
 

475-477	04		ORIG-CRNCY-CDE	Codigo de moneda origen.	N	9(03)
					Los valores posibles son:
					
					'032' (Pesos).
					'076' (Reales).

					'840' (Dolares).
					'858' (Pesos Uruguayos).

478-499	04		DATOS	Informacion para distintos codigos.	AN	X(22)
	
		04		DATOS1	Redefines DATOS. 
					(Solo para codigos de transacciones 
					  '80', '81', '83', '85' y '86').

478-480	06		FILLER	Campo no utilizado.	AN	X(03)


481-499	06		NRO-CLIENTE	Numero de cliente.	AN	X(19)

		04		DATOS2	Redefines DATOS. 
					(Resto de codigos de transacciones).

478-479	06		TIPO-CLEARING	Para depositos en cheque, cuando	AN	X(02)
					el campo TERM-TYP sea '60' se
					informa el valor del clearing.
					Los valores posibles son:
					'24' - '48' - '72' - 'OT'

	480-483	06		FILLER	Campo no utilizado.	AN	X(04)

484-491	06		TIP-EXCHA-COMP	Tipo de cambio comprador para la	N	9(05)v(03)
					venta de dolares.

492-499	06		ARBITRAJE	Arbitraje de pesos contra la	N	9(05)v(03)
					moneda indicada en el campo
					ORIG-CRNCY-CDE.

500-507	04		TIP-EXCHA-VEND	Tipo de cambio vendedor para la	N	9(05)v(03)
					compra de dolares.
 
508-518	04		FILLER	Campo no utilizado.	AN	X(11)
 

519-520	04		RVSL-RSN	Indicativo de reversa.	N	9(02)
					Los valores posibles son los indicados
		                                    en el Capitulo V.7 Codigos del Sistema
					Codigos Indicativos de Reversos
	
521-536	04		PIN-OFST	PIN offset.	AN	X(16)

537-537	04		SHRG-GRP	Agrupador	AN	X(01)
POSIC.

538-608	04		FILLER	Campo no utilizado.	AN	X(71)

609-902	04		USER-FLD2X		AN	X(294)

		04		CAMPOS-NUEVOS2	Redefines USER-FLD2X

609-609	06		TIPO-DEP	Tipo de deposito.	AN	X(01)
					
					Los valores posibles son los indicados
		                                    en el Capitulo V.10 Codigos del Sistema
					Codigos de Tipos de Depositos
					
					Si no es un deposito, se informa CERO.

610-613	06		ISSUER-FIID	Numero de Institucion del Banco	AN	X(04)
					emisor de la tarjeta.

614-619	06		INTEREST-RATE	Tasa de interes para cuenta de	N	9(04)v(02)
					de credito.

620-627	06		CASH-FEE	Arancel de CASH ADVANCE para	N	9(06)v(02)
					cuenta de credito.

	628-629	06		CARD-TYPE	Tipo de tarjeta.	AN	X(02)

	630-630	06		SITUACION IVA	Situacion del titular de la cuenta ante 	N	9(1)
					el IVA,
					Los valores posibles son:

					"1" Consumidor Final
					"0" No consumidor Final
	631-642 06		NRO REFERENCIA 

	643-902	06		FILLER	Campo no utilizado.	AN	X(261)
*/

/*************************************************/
/*** CREDIT CARD TRX (FILLER6 = ISO-FIELD-126) ***/
#include <qusrinc/isoredef.h>
/*************************************************/


/* Header EXTRACT BASE24  R4.0 RED LINK */
typedef struct stHeaderExtractB24_t
{
	char PREFIX1[6];
	char PREFIX2[2];
	char DAT_TIM[19]; 
	char REC_TYP[2]; 
	char AUTH_PPD[04];
	char TERM_LN[04];
	char TERM_FIID[4];
	char TERM_ID[16];
	char CARD_LN[4];
	char CARD_FIID[4];
	char CARD_PAN[28];
	char CARD_MBR_NUM[3];
	char BRCH_ID[4];
	char REGN_ID[4];
	char USER_FLD1X[2];
} stHeaderExtractB24;

/* Datos de AUTH del  EXTRACT BASE24 R4.0 RED LINK */
typedef struct	stAuthExtractB24_t 
{ 
	char TYP_CDE[2];
	char TYP[4];
	char RTE_STAT[02];
	char ORIGINATOR;
	char RESPONDER;
	char ENTRY_TIM[19];
	char EXIT_TIM[19];
	char RE_ENTRY_TIM[19];
	char TRAN_DAT[6];
	char TRAN_TIM[8];
	char POST_DAT[6];
	char ACQ_ICHG_SETL_DAT[6];
	char ISS_ICHG_SETL_DAT[6];
	char SEQ_NUM[12];
	char TERM_TYP[2];
	char TIM_OFST[5];
	char ACQ_INST_ID_NUM[11];
	char RCV_INST_ID_NUM[11];
	struct TRAN_CDE_tag {
		char T_CDE[2];
		char T_FROM[2];
		char T_TO[2];
	} stTRAN_CDE;
	char FROM_ACCT[28];
	char TIPO_DEP;
	char TO_ACCT[28];
	char MULT_ACCT;
	char AMT_1[19];
	char AMT_2[19];
	char AMT_3[19];
	char FILLER1[10];
	char DEP_TYP;
	char RESP_CDE[3];
	union  TERM_NAME_LOC_tag {
		char TERM_NAME_LOC [25];
		char NOMBRE_ENTE[25];
		struct INTER_CONTRA_tag {
			char FIID[4];
			char ACCT_TYP[2];
			char ACCT_NUM[19];
		} stINTER_CONTRA;
	} uTERM_NAME_LOC;
	union TERM_OWNER_NAME_tag {
		char TERM_OWNER_NAME[22];
		char NRO_ENTE_CUOTA[3+5];
		char CREDITO_CUOTAS[2+1];
		char NUM_CBU[22];
		char PAYKEY_EMPRESA_PRODUCTO_FACTURA[3+3+16];
		char MBANKING_EMPRESA_FRECUENCIA_DIA[20+1+1];
		char DONACIONES[3];
	} uTERM_OWNER_NAME;
	char TERM_CITY[13];
	char TERM_ST_X[3];
	char TERM_CNTRY_X[2];
	char OSEQ_NUM[12];
	char OTRAN_DAT[4];
	char OTRAN_TIM[8];
	char B24_POST_DAY[4];
	char ORIG_CRNCY_CDE[3];
	union DATOS_tag {
		char DATA[22];
		struct DATOS1_tag {
			char ENTE[3];
			char CLIENTE[19];
		} stDATOS1;
		struct DATOS2_tag {
			char TIPO_CLEARING[2];
			char FILLER2[4];
			char TIP_EXCHA_COMP[8];
			char ARBITRAJE[8];
		} stDATOS2;
	} uDATOS;		
	char TIP_EXCHA_VEND[8];
	char FILLER3[11];
	char RVSL_RSN[2];
	char PIN_OFST[16];
	char SHRG_GRP;
	union USER_FLD3X_tag{
		char FILLER4[71];
		struct DATOS3_tag{
			char CONSOLIDADO	[1];
			char CUIT_DEBITO	[11];
			char CUIT_CREDITO	[11];
			char NOMBRE_SOLICITANTE 	[24];
			char NOMBRE_BENEFICIARIO	[24];
		}	stDATOS3;
	}tCAMPOS_NUEVOS_3;
	union USER_FLD2X_tag {
		char FILLER5[294];
		struct CAMPOS_NUEVOS_2_tag {
			char TIPO_DEP;
			char ISSUER_FIID[4];
			char INTEREST_RATE[4];
			char CASH_FEE[6];
			char CARD_TYPE[2];
			char SITUACION_IVA;
			// 2013.08.15 - JAF - Agrego nro referencia antes de filler
			char FILLER;
			char NRO_REFERENCIA[11];
			/*************************************************/
			/*** CREDIT CARD TRX (FILLER6 = ISO-FIELD-126) ***/
			union uFILLER6_tag { 				
				char                    FILLER6[260];
				struct CreditCardAcct_t stCreditCardAcct;
			} uFILLER6;
			/*************************************************/
		} tCAMPOS_NUEVOS_2;
	} uUSER_FLD2X;	
} stAuthExtractB24 ;

/* mensaje SIOP completo para AS/400, identico al EXTRACT */
/* header mas cuerpo de mensaje */
typedef struct msgEXTRACT_t
{
	// FIX: SACAMOS BLOCK INIT - 2013.08.13
	// char         chBLOCK_INI_TLF[6]; /* contador de reg. TLF */
	stHeaderExtractB24 header;
	stAuthExtractB24   cuerpo;
	char		 chETX;
	char		 chEOF;
	char		 chNUL;
} msgEXTRACT;


/* B24-EXTRACT'S REVERSAL REASONS, RELEASE 4.0 and 6.0 */
#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ || \
	  _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE6_ )
#ifndef CEXTR_REV_TIMEOUT
#define CEXTR_REV_TIMEOUT 				"01"
#define CEXTR_REV_COMMAND_REJECTED 		"02"
#define CEXTR_REV_DEST_NOT_AVAIL 		"03"
#define CEXTR_REV_CANCELLED_TRX 		"08"
#define CEXTR_REV_HARDWARE_ERROR 		"10"
#define CEXTR_REV_SUSPICIUS_TRX 		"20"
#endif // CEXTR_REV_TIMEOUT
#endif // _BASE24_CURRENT_VERSION_

/* B24-EXTRACT'S BINARY VALUES for the REVERSAL REASONS, RELEASE 4.0 */
#ifndef CEXTR_REV_TIMEOUT_I
#define CEXTR_REV_TIMEOUT_I 			(1)
#define CEXTR_REV_COMMAND_REJECTED_I	(2)
#define CEXTR_REV_DEST_NOT_AVAIL_I 		(3)
#define CEXTR_REV_CANCELLED_TRX_I 		(8)
#define CEXTR_REV_HARDWARE_ERROR_I 		(10)
#define CEXTR_REV_SUSPICIUS_TRX_I 		(20)
#endif // CEXTR_REV_TIMEOUT_I

/* B24-EXTRACT'S ACCOUNT TYPES, RELEASE 4.0 */
#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )

#define _CEXTR_CHECKINGS_ACCOUNT_USD_   (7) //  7   - CUENTA CORRIENTE EN DOLARES
#define _CEXTR_SAVINGS_ACCOUNT_        (11) // 10   - CAJA DE AHORROS EN PESOS
#define _CEXTR_SPECIAL_ACCOUNT_AFJP_   (13) // 13   - CUENTA AFJP
#define _CEXTR_SAVINGS_ACCOUNT_BOND_   (14) // 14   - CUENTA BONOS
#define _CEXTR_SAVINGS_ACCOUNT_USD_    (15) // 15	- CAJA DE AHORROS EN DOLARES
#define _CEXTR_CHECKINGS_ACCOUNT_       (1) // 20	- CUENTA CORRIENTE EN PESOS
#define _CEXTR_CREDIT_CARD_ACCOUNT_    (31) // 30	- CUENTA TARJETA DE CREDITO EN PESOS
#define _CEXTR_CREDIT_LINE_ACCOUNT_    (30) // 32	- CUENTA CREDITO VISTA EN PESOS
#define _CEXTR_CREDIT_ACCOUNT_		   _CEXTR_CREDIT_LINE_ACCOUNT_
#define _CEXTR_TRANSFER_ACCOUNT_CBU_   (20) // 40   - CUENTA TRANSFERENCIA CBU

#define CEXTR_CHECKINGS_ACCOUNT_USD    "07" //  7   - CUENTA CORRIENTE EN DOLARES
#define CEXTR_SAVINGS_ACCOUNT          "11" // 10   - CAJA DE AHORROS EN PESOS
#define CEXTR_SPECIAL_ACCOUNT_AFJP     "13" // 13   - CUENTA AFJP
#define CEXTR_SAVINGS_ACCOUNT_BOND     "14" // 14   - CUENTA BONOS
#define CEXTR_SAVINGS_ACCOUNT_USD      "15" // 15	- CAJA DE AHORROS EN DOLARES
#define CEXTR_CHECKINGS_ACCOUNT        "01" // 20	- CUENTA CORRIENTE EN PESOS
#define CEXTR_CREDIT_CARD_ACCOUNT      "31" // 30	- CUENTA TARJETA DE CREDITO EN PESOS
#define CEXTR_CREDIT_LINE_ACCOUNT      "30" // 32	- CUENTA CREDITO VISTA EN PESOS
#define CEXTR_TRANSFER_ACCOUNT_CBU	   "20" // 40   - CUENTA TRANSFERENCIA CBU

#define  _CEXTR_OPCODE_WITHDRAWAL_				   "10" // Extraccion
#define  _CEXTR_OPCODE_DEPOSIT_                    "20" // Deposito
#define  _CEXTR_OPCODE_BALANCE_INQUIRY_			   "30" // Consulta de Saldo
#define  _CEXTR_OPCODE_BALANCE_INQUIRY_POS_		   "73" // Consulta de Saldo POS
#define  _CEXTR_OPCODE_TRANSFER_				   "40" // Transferencia
#define  _CEXTR_OPCODE_PAYMENT_W_DEBIT_ 		   "50" // Pago con debito en cuenta
#define  _CEXTR_OPCODE_PAYMENT_W_CASH_ 			   "51" // Pago con sobre y dinero efevo.
#define  _CEXTR_OPCODE_BUYING_W_ACCT_DEBIT_        "71" // Compra con Debito
#define  _CEXTR_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL_ "72" // Anulacion compra "credito"
#define  _CEXTR_OPCODE_BUYING_DEVOLUTION_          "74" // Devolucion compra "credito NO se procesa"
#define  _CEXTR_OPCODE_BUYING_DEVOLUTION_CANCEL_   "75" // Anul. Devolucion compra "debito NO se procesa"
#define  _CEXTR_OPCODE_BUYING_CASH_BACK_           "76" // Compra cash-back
#define  _CEXTR_OPCODE_BUYING_CASH_BACK_CANCEL_    "77" // Anulacion Compra cash-back
#define  _CEXTR_OPCODE_BUYING_PREAUTH_             "78" // Preautorizacion NO se procesa
#define  _CEXTR_OPCODE_BUYING_PREAUTH_CANCEL_      "79" // Anul. Preautorizacion NO se procesa
#define  _CEXTR_OPCODE_DEBIT_ORDER_XFER_           "19" // Orden de Debito x transferencia
#define  _CEXTR_OPCODE_CREDIT_ORDER_XFER_          "29" // Orden de Credito x transferencia

#define  CEXTR_OPCODE_WITHDRAWAL				 10  // Extraccion
#define  CEXTR_OPCODE_DEPOSIT                    20  // Deposito
#define  CEXTR_OPCODE_BALANCE_INQUIRY			 30  // Consulta de Saldo
#define  CEXTR_OPCODE_PIN_CHANGE     			 31  // Cambio de PIN
#define  CEXTR_OPCODE_BALANCE_INQUIRY_POS		 73  // Consulta de Saldo POS
#define  CEXTR_OPCODE_TRANSFER				     40  // Transferencia
#define  CEXTR_OPCODE_PAYMENT_W_DEBIT 		     50  // Pago con debito en cuenta
#define  CEXTR_OPCODE_PAYMENT_W_STATEMENT	     CEXTR_OPCODE_PAYMENT_W_DEBIT
#define  CEXTR_OPCODE_PAYMENT_W_CASH 		     51  // Pago con sobre y dinero efevo.
#define  CEXTR_OPCODE_BUYING_W_ACCT_DEBIT        71  // Compra con Debito
#define  CEXTR_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL 72  // Anulacion compra credito
#define  CEXTR_OPCODE_BUYING_DEVOLUTION          74  // Devolucion compra credito NO se procesa
#define  CEXTR_OPCODE_BUYING_DEVOLUTION_CANCEL   75  // Anul. Devolucion compra debito NO se procesa
#define  CEXTR_OPCODE_BUYING_CASH_BACK           76  // Compra cash-back
#define  CEXTR_OPCODE_BUYING_CASH_BACK_CANCEL    77  // Anulacion Compra cash-back
#define  CEXTR_OPCODE_BUYING_PREAUTH             78  // Preautorizacion NO se procesa
#define  CEXTR_OPCODE_BUYING_PREAUTH_CANCEL      79  // Anul. Preautorizacion NO se procesa
#define  CEXTR_OPCODE_DEBIT_ORDER_XFER           19  // Orden de Debito x transferencia
#define  CEXTR_OPCODE_CREDIT_ORDER_XFER          29  // Orden de Credito x transferencia
#define  CEXTR_OPCODE_WITHDRAWAL_FRGN_CURRENCY	 CEXTR_OPCODE_WITHDRAWAL
#define  CEXTR_OPCODE_TRANSFER_FRGN_CURRENCY	 CEXTR_OPCODE_TRANSFER

#elif ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE6_ )

#define _CEXTR_SAVINGS_ACCOUNT_			(11)   // CAJA DE AHORROS PESOS
#define _CEXTR_SAVINGS_ACCOUNT_USD_		(12)   // CAJA DE AHORROS DOLARES
#define _CEXTR_CHECKINGS_ACCOUNT_		(1)    // CUENTA CORRIENTE PESOS
#define _CEXTR_CHECKINGS_ACCOUNT_USD_	(2)    // CUENTA CORRIENTE DOLARES
#define _CEXTR_CREDIT_CARD_ACCOUNT_		(31)   // CUENTA TARJETA DE CREDITO EN PESOS
#define _CEXTR_CREDIT_ACCOUNT_			(30)   // CUENTA CREDITO EN PESOS
#define _CEXTR_TRANSFER_ACCOUNT_CBU_	(20)   // CUENTA TRANSFERENCIA CBU

#define CEXTR_SAVINGS_ACCOUNT			"11"   // CAJA DE AHORROS PESOS
#define CEXTR_SAVINGS_ACCOUNT_USD		"12"   // CAJA DE AHORROS DOLARES
#define CEXTR_CHECKINGS_ACCOUNT		    "01"   // CUENTA CORRIENTE PESOS
#define CEXTR_CHECKINGS_ACCOUNT_USD	    "02"   // CUENTA CORRIENTE DOLARES
#define CEXTR_CREDIT_CARD_ACCOUNT		"31"   // CUENTA TARJETA DE CREDITO EN PESOS
#define CEXTR_CREDIT_ACCOUNT			"30"   // CUENTA CREDITO EN PESOS
#define CEXTR_TRANSFER_ACCOUNT_CBU	    "20"   // CUENTA TRANSFERENCIA CBU

#define  _CEXTR_OPCODE_WITHDRAWAL_				    "10" // Extraccion
#define  _CEXTR_OPCODE_WITHDRAWAL_FRGN_CURRENCY_	"16" // Extraccion Moneda Extranjera
#define  _CEXTR_OPCODE_DEPOSIT_                 	"20" // Deposito
#define  _CEXTR_OPCODE_BALANCE_INQUIRY_			    "30" // Consulta de Saldo
#define  _CEXTR_OPCODE_BALANCE_INQUIRY_POS_		    _CEXTR_OPCODE_BALANCE_INQUIRY_ // Consulta de Saldo
#define  _CEXTR_OPCODE_PAYMENT_W_DEBIT_ 			"57" // Pago con debito en cuenta, s/deuda
#define  _CEXTR_OPCODE_PAYMENT_W_STATEMENT_		    "58" // Pago con debito, con deuda informada
#define  _CEXTR_OPCODE_PAYMENT_W_CASH_			    "51"
#define  _CEXTR_OPCODE_TRANSFER_				    "40" // Transferencia
#define  _CEXTR_OPCODE_TRANSFER_FRGN_CURRENCY_     "41" // Transferencia moneda extranjera
#define  _CEXTR_OPCODE_TRANSFER_CBU_               "47" // Transferencia CBU
#define  _CEXTR_OPCODE_TRANSFER_CBU_ACH_           "48" // Transferencia hacia ACH
#define  _CEXTR_OPCODE_BUYING_W_ACCT_DEBIT_        "15" // Compra con Debito
#define  _CEXTR_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL_ "23" // Anulacion compra "credito"
#define  _CEXTR_OPCODE_BUYING_DEVOLUTION_          "18" // Devolucion compra "credito NO se procesa"
#define  _CEXTR_OPCODE_BUYING_DEVOLUTION_CANCEL_   "22" // Anul. Devolucion compra "debito NO se procesa"
#define  _CEXTR_OPCODE_BUYING_CASH_BACK_           "26" // Compra cash-back
#define  _CEXTR_OPCODE_BUYING_CASH_BACK_CANCEL_    "24" // Anulacion Compra cash-back
#define  _CEXTR_OPCODE_DEBIT_ORDER_XFER_           _CEXTR_OPCODE_TRANSFER_CBU_ // Orden de Debito x transferencia

#define  CEXTR_OPCODE_WITHDRAWAL				 10 // Extraccion
#define  CEXTR_OPCODE_WITHDRAWAL_FRGN_CURRENCY	 16 // Extraccion Moneda Extranjera
#define  CEXTR_OPCODE_DEPOSIT                 	 20 // Deposito
#define  CEXTR_OPCODE_BALANCE_INQUIRY			 30 // Consulta de Saldo
#define  CEXTR_OPCODE_BALANCE_INQUIRY_POS		 CEXTR_OPCODE_BALANCE_INQUIRY // Consulta de Saldo
#define  CEXTR_OPCODE_PIN_CHANGE     			 81 // Cambio de PIN - codigo
#define  CEXTR_OPCODE_PAYMENT_W_DEBIT 		     57 // Pago con debito en cuenta, s/deuda
#define  CEXTR_OPCODE_PAYMENT_W_STATEMENT	     58 // Pago con debito, con deuda informada
#define  CEXTR_OPCODE_PAYMENT_W_CASH		     51 // Pago con sobre y dinero efevo.
#define  CEXTR_OPCODE_TRANSFER				     40 // Transferencia
#define  CEXTR_OPCODE_TRANSFER_FRGN_CURRENCY     41 // Transferencia moneda extranjera
#define  CEXTR_OPCODE_TRANSFER_CBU               47 // Transferencia CBU
#define  CEXTR_OPCODE_TRANSFER_CBU_ACH           48 // Transferencia hacia ACH
#define  CEXTR_OPCODE_BUYING_W_ACCT_DEBIT        15 // Compra con Debito
#define  CEXTR_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL 23 // Anulacion compra credito
#define  CEXTR_OPCODE_BUYING_DEVOLUTION          18 // Devolucion compra credito NO se procesa
#define  CEXTR_OPCODE_BUYING_DEVOLUTION_CANCEL   22 // Anul. Devolucion compra debito NO se procesa
#define  CEXTR_OPCODE_BUYING_CASH_BACK           26 // Compra cash-back
#define  CEXTR_OPCODE_BUYING_CASH_BACK_CANCEL    24 // Anulacion Compra cash-back
#define  CEXTR_OPCODE_DEBIT_ORDER_XFER           CEXTR_OPCODE_TRANSFER_CBU // Orden de Debito x transferencia
#endif


#endif /*_EXTRACT_STRUCT_*/


