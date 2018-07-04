//////////////////////////////////////////////////////////////////////////////////////////////////
// IT24 OUTSOURCING S.A. / COA S.A.
// RECORD DEFINITION FOR BANCA EMPRESA 
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2011.05.02      mdc     inicial SALDOS y MOVIMIENTOS CONFORMADOS
// (Inicial)    2011.05.10      mdc     inicial DIFERIDOS
// (Inicial)    2011.06.21      mdc     inicial REFRESH CAF
// (Inicial)    2011.07.05      mdc     Fix cuenta corriente USD $7
// (Inicial)    2011.08.01      mdc     stEXTRACT_XFER_t & stEXTRACT_XFER_RESULT_t
// (Inicial)    2011.08.09      mdc     stEXTRACT_XFER_Pie_t
// (Inicial)    2011.08.29      mdc     ACH, NACHA y similares
// (Inicial)    2012.02.10      mdc     Aumenta en 3 posiciones el campo importe.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BANCAEMPRESA_H_
#define _BANCAEMPRESA_H_

#define _CEXTR_BE_CHECKINGS_ACCOUNT_           (3) // CUENTA CORRIENTE EN PESOS
#define _CEXTR_BE_CHECKINGS_ACCOUNT_USD_       (4) // CUENTA CORRIENTE EN DOLARES
#define _CEXTR_BE_SAVINGS_ACCOUNT_             (1) // CAJA DE AHORROS EN PESOS
#define _CEXTR_BE_SAVINGS_ACCOUNT_USD_         (2) // CAJA DE AHORROS EN DOLARES
#define _CEXTR_BE_SPECIAL_ACCOUNT_PATAC_       (5) // CUENTA CORRIENTE EN PATACONES
#define _CEXTR_BE_SPECIAL_ACCOUNT_PATAC2_      (8) // CUENTA CORRIENTE EN PATACONES 2
#define _CEXTR_BE_SPECIAL_ACCOUNT_LEGAL_       (6) // CUENTA ESPECIAL PERS.JURIDICA PESOS
#define _CEXTR_BE_SPECIAL_ACCOUNT_LEGAL_USD_   (7) // CUENTA ESPECIAL PERS.JURIDICA DOLARES
#define _CEXTR_BE_SPECIAL_ACCOUNT_LEGAL_LECOP_ (10) // CUENTA ESPECIAL PERS.JURIDICA LECOP
#define _CEXTR_BE_BONDS_ACCOUNT_               (9) // CUENTA BONOS LECOP

#define CEXTR_BE_CHECKINGS_ACCOUNT           '3' // CUENTA CORRIENTE EN PESOS
#define CEXTR_BE_CHECKINGS_ACCOUNT_USD       '4' // CUENTA CORRIENTE EN DOLARES
#define CEXTR_BE_SAVINGS_ACCOUNT             '1' // CAJA DE AHORROS EN PESOS
#define CEXTR_BE_SAVINGS_ACCOUNT_USD         '2' // CAJA DE AHORROS EN DOLARES
#define CEXTR_BE_SPECIAL_ACCOUNT_PATAC       '5' // CUENTA CORRIENTE EN PATACONES
#define CEXTR_BE_SPECIAL_ACCOUNT_PATAC2      '8' // CUENTA CORRIENTE EN PATACONES 2
#define CEXTR_BE_SPECIAL_ACCOUNT_LEGAL       '6' // CUENTA ESPECIAL PERS.JURIDICA PESOS
#define CEXTR_BE_SPECIAL_ACCOUNT_LEGAL_USD   '7' // CUENTA ESPECIAL PERS.JURIDICA DOLARES
#define CEXTR_BE_SPECIAL_ACCOUNT_LEGAL_LECOP 'A' // CUENTA ESPECIAL PERS.JURIDICA LECOP
#define CEXTR_BE_BONDS_ACCOUNT               '9' // CUENTA BONOS LECOP

#define CEXTR_BE_CHECKINGS_ACCOUNT_STR           "03" // CUENTA CORRIENTE EN PESOS
#define CEXTR_BE_CHECKINGS_ACCOUNT_USD_STR       "04" // CUENTA CORRIENTE EN DOLARES
#define CEXTR_BE_SAVINGS_ACCOUNT_STR             "01" // CAJA DE AHORROS EN PESOS
#define CEXTR_BE_SAVINGS_ACCOUNT_USD_STR         "02" // CAJA DE AHORROS EN DOLARES
#define CEXTR_BE_SPECIAL_ACCOUNT_PATAC_STR       "05" // CUENTA CORRIENTE EN PATACONES
#define CEXTR_BE_SPECIAL_ACCOUNT_PATAC2_STR      "08" // CUENTA CORRIENTE EN PATACONES 2
#define CEXTR_BE_SPECIAL_ACCOUNT_LEGAL_STR       "06" // CUENTA ESPECIAL PERS.JURIDICA PESOS
#define CEXTR_BE_SPECIAL_ACCOUNT_LEGAL_USD_STR   "07" // CUENTA ESPECIAL PERS.JURIDICA DOLARES
#define CEXTR_BE_SPECIAL_ACCOUNT_LEGAL_LECOP_STR "10" // CUENTA ESPECIAL PERS.JURIDICA LECOP
#define CEXTR_BE_BONDS_ACCOUNT_STR               "09" // CUENTA BONOS LECOP

// 2	Interfaces REFRESH
// 2.1	SALDOS
// El archivo se compone de un registro de cabecera y a continuación los registros de detalle.
// 
// Longitud de registro = 157
// Nombre del archivo: RBESAddmmeeee.DAT (1)
//
// 2.1.1	Registro de cabecera
typedef struct stSALDOS_Cabecera_t
{
	char Tipo_registro	[6];	// 1	6	 Tipo de registro del archivo	“SALDOS”	
	char Banco	        [3];	// 7	9	 Código de Entidad (s/BCRA).		Completar con ceros a izquierda
	char Fecha_proceso	[8];	// 10	17	 Fecha de proceso	Formato AAAAMMDD	
	char Cant_registros	[8];	// 18	25	 Cantidad de registros incluyendo el de cabecera		Completar con ceros a izquierda
	char Filler	        [132];  // 26	157	 132 espacios	
} stSALDOS_Cabecera;


// 2.1.2	Registro de detalle
typedef struct stSALDOS_Detalle_t
{
	char Tipo_registro	    [6];	//1	6	Tipo de registro del archivo	“SALDOS”	
	char Banco 			    [3];	//7	9	Código de Entidad [s/BCRA].		Completar con ceros a izquierda
	char Tipo_cuenta 	    ;		//10	10	Tipo de cuenta 	VER: Tabla de tipos de cuenta	
	char Moneda			    [2];	//11	12	Código de moneda de la cuenta	VER:	Tabla de monedas	Completar con ceros a izquierda
	char Numero_de_cuenta	[19];	//13	31	Numero de cuenta		Completar con espacios a derecha
	char CBU				[23];	//32	54	CBU de la cuenta		Completar con ceros a izquierda
	char Fecha_ult_mov		[8]; 	//55	62	Fecha de ultima operación de la cuenta. 	Formato AAAAMMDD	
	char Fecha_prox_capit	[8]; 	//63	70	Fecha de próxima capitalización de intereses.	En blanco, o fecha válida en formato AAAAMMDD	
	char Saldo_contable		[15+2]; //71	87	15 enteros y 2 decimales.	Saldo contable al cierre del proceso.	Completar con ceros a izquierda Sin punto decimal
	char Signo_contable	    ; 		//88	88	+: positivo -: negativo	“+” o “-“	
	char Saldo_apertura	    [15+2]; //89	105	15 enteros y 2 decimales.	Saldo operativo al cierre del proceso	Completar con ceros a izquierda Sin punto decimal
	char Signo_apertura	    ; 		//106	106	+: positivo -: negativo	“+” o “-“	
	char Saldo_bloqueado	[15+2] ;//107	123	15 enteros y 2 decimales.		Completar con ceros a izquierda Sin punto decimal
	char Acuerdos	        [15+2] ;//124	140	15 enteros y 2 decimales.		Completar con ceros a izquierda Sin punto decimal
	char Intereses	        [15+2] ;//141	157	15 enteros y 2 decimales.		Completar con ceros a izquierda Sin punto decimal
} stSALDOS_Detalle;

// 2.2	MOVIMIENTOS CONFORMADOS
// 
// El archivo se compone de un registro de cabecera y a continuación los registros de detalle.
// 
// Longitud de registro = 155
// 
// Nombre del archivo: RBEMCddmmeeee.DAT (1)
// 
// 2.2.1	Registro de cabecera
// 
typedef struct stMOVCUE_Cabecera_t
{
	char Tipo_registro	[6];		//	1	6	Tipo de registro del archivo	“CONFOR”	
	char Banco			[3];		// 7	9	Código de Entidad (s/BCRA).		Completar con ceros a izquierda
	char Fecha_proceso	[8];		// 10	17	Fecha de proceso	Formato AAAAMMDD	
	char Cant_registros	[8];		// 18	25	Cantidad de registros incluyendo el de cabecera		Completar con ceros a izquierda
	char Filler			[130];	   // 26	155		130 espacios	
} stMOVCUE_Cabecera;

// 2.2.2	Registro de detalle
//
typedef struct stMOVCUE_Detalle_t
{
	char Tipo_registro		[6];	// 1	6	Tipo de registro del archivo	“CONFOR”	
	char Banco 				[3];	// 7	9	Código de Entidad [s/BCRA];.		Completar con ceros a izquierda
	char Tipo_cuenta		;		// 10	10	Tipo de cuenta	
	char Moneda				[2]; 	// 11	12	Código de moneda de la cuenta	
	char Numero_de_cuenta	[19];	// 13	31	Numero de cuenta		Completar con espacios a derecha
	char CBU				[23];	// 32	54	CBU de la cuenta		Completar con ceros a izquierda
	char Fecha_mov			[8]; 	// 55	62	Fecha de imputación	Formato AAAAMMDD	
	char Fecha_valor		[8]; 	// 63	70	Fecha de contabilización	Formato AAAAMMDD	
	char Tipo_de_operacion	;	    // 71	71	Signo de la operación	D = débito,  C = crédito	
	char Monto				[15+2]; // 	72	88	15 enteros y 2 decimales		Completar con ceros a izquierda 
	char Secuencia			[4];	// 89	92	Número secuencial para diferenciar operaciones	Comenzando a partir del 0001 cada vez que cambie el número de cuenta	Completar con ceros a izquierda
	char Referencia			[13]; 	// 93	93	Número de comprobante		Completar con ceros a izquierda
	char Depositante		[50];	// 106	155	Depositante o descripción de la operación		Completar con espacios a derecha
} stMOVCUE_Detalle;

// 2.3	MOVIMIENTOS DIFERIDOS
// 
// El archivo se compone de un registro de cabecera y a continuación los registros de detalle.
// 
// Longitud de registro = 155
// 
// Nombre del archivo: RBEMDddmmeeee.DAT (1)
// 
// 2.3.1	Registro de cabecera
// 
typedef struct stMOVDIF_Cabecera_t
{
	char Tipo_registro	[6];		//	1	6	Tipo de registro del archivo	“DIFERI”	
	char Banco			[3];		// 7	9	Código de Entidad (s/BCRA).		Completar con ceros a izquierda
	char Fecha_proceso	[8];		// 10	17	Fecha de proceso	Formato AAAAMMDD	
	char Cant_registros	[8];		// 18	25	Cantidad de registros incluyendo el de cabecera		Completar con ceros a izquierda
	char Filler			[130];	   // 26	155		130 espacios	
} stMOVDIF_Cabecera;

// 2.3.2	Registro de detalle
//
typedef struct stMOVDIF_Detalle_t
{
	char Tipo_registro		[6];	// 1	6	Tipo de registro del archivo	“DIFERI”	
	char Banco 				[3];	// 7	9	Código de Entidad [s/BCRA];.		Completar con ceros a izquierda
	char Tipo_cuenta		;		// 10	10	Tipo de cuenta	
	char Moneda				[2]; 	// 11	12	Código de moneda de la cuenta	
	char Numero_de_cuenta	[19];	// 13	31	Numero de cuenta		Completar con espacios a derecha
	char CBU				[23];	// 32	54	CBU de la cuenta		Completar con ceros a izquierda
	char Fecha_credito		[8]; 	// 55	62	Fecha de imputación	Formato AAAAMMDD	
	char Fecha_carga		[8]; 	// 63	70	Fecha de contabilización	Formato AAAAMMDD	
	char Secuencia			[4];	// 71	74	Número secuencial para diferenciar operaciones	Comenzando a partir del 0001 cada vez que cambie el número de cuenta	Completar con ceros a izquierda
	char Monto				[15+2]; // 75	91	15 enteros y 2 decimales		Completar con ceros a izquierda 
	char Tipo_de_operacion	;	    // 92	92	Signo de la operación	D = débito,  C = crédito	
	char Referencia			[13]; 	// 93	105	Número de comprobante		Completar con ceros a izquierda
	char Depositante		[50];	// 106	155	Depositante o descripción de la operación		Completar con espacios a derecha
} stMOVDIF_Detalle;

// 4	Extract de Cuentas
// a.	Ancho de registro = 150 bytes
// b.	Nombre de archivo : EBECUddmm.eeee 
//
typedef struct stEXTRACT_CU_t
{
	char Cuit	            [13]; // 1	13	Cuit del cliente		Completar con ceros a izquierda
	char Tipo_cuenta	    [2] ; // 14	15	Tipo cuenta débito.  
	char Numero_cuenta_DB	[19]; // 16	34	Numero de cuenta débito, Completar con ceros a izquierda
	char Estado	            [2];  // 35	36	Estado de la cuenta : VE: verificado, BA: baja	
	char Cuenta_Principal	;	  // 37	37	Indica la cuenta seleccionada por el cliente como principal. S: es cuenta principal, N: no es cuenta principal.	
	char Producto	        ;	  // 38	38	Indica el producto contratado por el Cliente	Los valores posibles son 1; 2 ó 3. , 
								  // Si la Entidad no opta por el armado de productos se informa siempre el valor “1”.
	char Filler	            [112];// 39	150		112 espacios	
} stEXTRACT_CU;

// 5	Extract de CAF
// a.	Ancho de registro = 500 bytes
// b.	Nombre de archivo : EBECFaaaammddeeee.DAT 
//
typedef struct stEXTRACT_CAF_t
{

	char Tarjeta_numero	[19];	  // 1	19	Número de tarjeta creada en la aplicación BEE.	FORMATO: 504620EEENNNNNNN. (16 posiciones)
								  // Donde,	504620=FIJO,	EEE= Nro.Entidad, 	NNNNNNN=Nro. de Tarjeta.
	char Miembro_Numero	[3];	  // 20	22		FIJO: 000 (CEROS)
	char Tarjeta_tipo	[2];	  // 23	24		FIJO: PE
	char Tarjeta_Estado	;		  // 25	25	Estado de la tarjeta	FIJO:  1 (Activa)
	char Cuentas_relacionadas[2]; // 26	27	Indica la cantidad de cuentas que tiene relacionada la tarjeta	Valor mínimo = 0, Máximo = 16.
	struct VecCuentas_t
	{
		char Tipo	[2]	; // 28	29	Indica el tipo de cuenta asociada a la tarjeta	Ver:
		char Numero	[19]; // 30	48	Número de la cuenta asociado a la tarjeta.	Se informa en el mismo formato en que se ingresó al sistema BEE
		char Estado	;	  // 49	49	FIJO:  1 (Activa)
	} VecCuentas[16];
	char Fecha_generacion [6]	; // 	380	385	Fecha de generación del archivo:	Formato AAMMDD
	char Filler	          [115]	; // 	386	500		115 espacios
} stEXTRACT_CAF; 


// 3.	EXTRACT DE TRANSFERENCIAS
// a.	Ancho de registro = 500 bytes
// b.	Nombre de archivo : EBETRddmm.eeee
// c.	Diseño de registro a imputar
//

typedef struct stEXTRACT_XFER_Cabecera_t
{
	char Codigo_Banco  [4];	   // 	Código de Entidad s/BCRA
	char Fecha_Proceso [8];	   // 	Fecha Imputacion
	char Filler [500-8-4];     //   FILLER
} stEXTRACT_XFER_Cabecera; 

typedef struct stEXTRACT_XFER_Pie_t
{

	char Codigo_Banco [4];  // Código de Entidad s/BCRA
	char Fecha_Proceso [8]; // Fecha de proceso AAAAMMDD
	char Cant_registros[5]; // Cantidad de registros detalle 
	char Cant_Mnt_deb[17];  // 9(15)v99, Monto total de los débitos
	char Cant_Mnt_cred[17]; // 9(15)v99, Monto total de los créditos
	char Filler [449];      // 449 espacios
} stEXTRACT_XFER_Pie; 

typedef struct stEXTRACT_XFER_t
{

	char Transferencia[9];			// 1	9	Número de transferencia	
	char Cuit	[13];				// 	10	22	CUIT del cliente	
	char Banco_debito	[4];		// 	23	26	Código de Entidad de la cuenta débito [s/BCRA];.	
	char Tipo_cuenta_DB	[2];		// 	27	28	Tipo cuenta débito.	Tabla de relación tipo de cuenta / monedas
	char Numero_cuenta_DB	[19];	// 		29	47	Número de cuenta débito	
	char Nombre_cuenta_DB	[50];	// 	48	97	Nombre de la cuenta débito	
	char CBU_DB	[23];				// 	98	120	CBU de la cuenta débito	CBU o CEROS
	char Banco_CR	[4];			// 	121	124	Código de Entidad de la cuienta crédito [s/BCRA];.	
	char Tipo_cuenta_CR	[2];		// 	125	126	Tipo cuenta crédito. Tabla de relación tipo de cuenta/monedas
	char Numero_cuenta_CR	[19];	// 	127	145	Numero de cuenta crédito	Número de la cuenta crédito o espacios si es una transferencia por CBU.
	char Nombre_cuenta_CR	[50];	// 	146	195	Nombre de la cuenta crédito	
	char CBU_CR	[23];				// 	196	218	CBU de la cuenta crédito	CBU de la cuenta crédito o espacios [si es una transferencia entre cuentas vinculadas];.
	char Importe	[17];	 		// 	219	235	Importe	9(15)V99
	char Concepto	[50];			// 	236	285	Concepto	
	char Fecha_inputacion 	[8]; 	// 	286	293	Reservado para uso futuro	8 espacios
	char Fecha_alta	[8]; 			// 	294	301	Fecha de alta 	Formato AAAAMMDD
	char Hora_alta	[6]; 			// 	302	307	Hora del alta	Formato hhmmss
	char Usuario_alta	[13];		// 	308	320	Usuario del alta	
	char Usuario_aut_1	[13];		// 	321	333	Usuario autorizante 1	
	char Fecha_aut_1	[8]; 		// 	334	341	Fecha autorización 1	Formato AAAAMMDD
	char Hora_aut_1	[6]; 			// 	342	347	Hora autorización 1	Formato Hhmmss
	char Usuario_aut_2	[13];		// 	348	360	Usuario autorizante 2	Usuario o espacios si no posee segunda autorización
	char Fecha_hora_aut_2	[8]; 	// 	361	368	Fecha autorización 2	Formato AAAAMMDD o espacios si no posee segunda autorización
	char Hora_aut_2	[6]; 			// 	369	374	Hora autorización 2	Formato hhmmss o espacios si no posee segunda autorización
	char Usuario_aut_3	[13];		// 	375	387	Usuario autorizante 3	Usuario o espacios si no posee tercera autorización
	char Fecha_hora_aut_3	[8]; 	// 	388	395	Fecha autorización 3	Formato AAAAMMDD o espacios si no posee tercera autorización
	char Hora_aut_3	[6]; 			// 	396	401	Hora autorización 3	Formato hhmmss o espacios si no posee tercera autorización
	char Fecha_envio	[8]; 		// 	402	409	Fecha de envío a las entidades.	espacios o fecha válida en formato AAAAMMDD si es reproceso
	char CUIT_CUIL_CDI_CR	[13];	// 	410	422		CUIT, CUIL o CDI de la cuenta de crédito.
	char Titularidad_Cta_CR	;		// 	423	423	Respuesta del banco	P = Propia, N = No Propia
	char Motivo	[3];				// 	424	426		Tabla de Motivos de la Transferencia.
	char Referencia	[12];			// 	427	438	Referencia.	Texto libre ingresado por el originante de la transferencia.
	char Filler	[62];				// 	439	500		62 espacios
} stEXTRACT_XFER;

typedef struct stEXTRACT_XFER_RESULT_t
{

	char Transferencia[9];			// 1	9	Número de transferencia	
	char Cuit	[13];				// 	10	22	CUIT del cliente	
	char Banco_debito	[4];		// 	23	26	Código de Entidad de la cuenta débito [s/BCRA];.	
	char Tipo_cuenta_DB	[2];		// 	27	28	Tipo cuenta débito.	Tabla de relación tipo de cuenta / monedas
	char Numero_cuenta_DB	[19];	// 		29	47	Número de cuenta débito	
	char Nombre_cuenta_DB	[50];	// 	48	97	Nombre de la cuenta débito	
	char CBU_DB	[23];				// 	98	120	CBU de la cuenta débito	CBU o CEROS
	char Banco_CR	[4];			// 	121	124	Código de Entidad de la cuienta crédito [s/BCRA];.	
	char Tipo_cuenta_CR	[2];		// 	125	126	Tipo cuenta crédito. Tabla de relación tipo de cuenta/monedas
	char Numero_cuenta_CR	[19];	// 	127	145	Numero de cuenta crédito	Número de la cuenta crédito o espacios si es una transferencia por CBU.
	char Nombre_cuenta_CR	[50];	// 	146	195	Nombre de la cuenta crédito	
	char CBU_CR	[23];				// 	196	218	CBU de la cuenta crédito	CBU de la cuenta crédito o espacios [si es una transferencia entre cuentas vinculadas];.
	char Importe	[17];	 		// 	219	235	Importe	9(15)V99
	char Concepto	[50];			// 	236	285	Concepto	
	char Fecha_inputacion 	[8]; 	// 	286	293	Reservado para uso futuro	8 espacios
	char Fecha_alta	[8]; 			// 	294	301	Fecha de alta 	Formato AAAAMMDD
	char Hora_alta	[6]; 			// 	302	307	Hora del alta	Formato hhmmss
	char Usuario_alta	[13];		// 	308	320	Usuario del alta	
	char Usuario_aut_1	[13];		// 	321	333	Usuario autorizante 1	
	char Fecha_aut_1	[8]; 		// 	334	341	Fecha autorización 1	Formato AAAAMMDD
	char Hora_aut_1	[6]; 			// 	342	347	Hora autorización 1	Formato Hhmmss
	char Usuario_aut_2	[13];		// 	348	360	Usuario autorizante 2	Usuario o espacios si no posee segunda autorización
	char Fecha_hora_aut_2	[8]; 	// 	361	368	Fecha autorización 2	Formato AAAAMMDD o espacios si no posee segunda autorización
	char Hora_aut_2	[6]; 			// 	369	374	Hora autorización 2	Formato hhmmss o espacios si no posee segunda autorización
	char Usuario_aut_3	[13];		// 	375	387	Usuario autorizante 3	Usuario o espacios si no posee tercera autorización
	char Fecha_hora_aut_3	[8]; 	// 	388	395	Fecha autorización 3	Formato AAAAMMDD o espacios si no posee tercera autorización
	char Hora_aut_3	[6]; 			// 	396	401	Hora autorización 3	Formato hhmmss o espacios si no posee tercera autorización
	char Fecha_envio	[8]; 		// 	402	409	Fecha de envío a las entidades.	espacios o fecha válida en formato AAAAMMDD si es reproceso
	char Estado			[2];		// 	410	411	Respuesta del banco	AC: aceptada , RE: rechazada	
	char Motivo	[3];				// 	412	414	VER: Tabla de Motivos de la Transferencia.
	char Referencia	[12];			// 	415	426	Referencia.	Texto libre ingresado por el originante de la transferencia.
	char Filler	[74];				// 	427	500 Espacios
} stEXTRACT_XFER_RESULT;

// Computational-3 = "PIC S9(15)V9(02) COMP-3" = 9 bytes
typedef struct COMPUTATIONAL_3_tag  
{
    char chDATA[9] ;
} COMPUTATIONAL_3_t;

// Date-YYMMDD = 6 bytes
typedef struct DATE_YYMMDD_tag  
{
    char chYYMMDD[6] ;
} DATE_YYMMDD_t;

// TABLA DE POSIBLES FERIADOS 
typedef struct TABFER_ARRAY_tag
{
    union
    {
        time_t          tTIME ;       /* BINARY long : 4 bytes (32 bits)       */
        DATE_YYMMDD_t   stYYMMDD;     /* STRUCT "yymmdd" : 6 bytes             */
        char            chDATA[10+1]; /* STRING "yyyy-mm-dd" : 10 BYTES + NULL */
    } TABFER[25];
} stTABFER_ARRAY_t;

// TABLA DE SALDOS HISTORICOS MENSUALES
typedef struct TABDMES_ARRAY_tag
{
    struct 
    {
		    char SALMEN					[9]; // S9(15)V9 (02) COMP-3
	        char FECMES					[2];
    } TABMES [ 12 ] ;
} stTABMES_ARRAY_t;

#include <dnetbcra.h>
// Macro de verificacion de codigo de banco emisor/receptor , si es banco propio
#define IS_ON_OUR_BANKCODE(x)   (BCRA_ID_DEFAULT_I == antoi(x,4) || BCRA_ALTERNATE_ID_I == antoi(x,4)) 


typedef struct stEXTRACT_ACH_tag
{
	//	Formato de registro de datos 	
	struct PK_tag
	{
		char FIID_CARD	  [04]; // 	Fiid del emisor de la tarjeta
		char PAN		  [19]; // 	Nro. de Tarjeta
		char MEMBER		  [03]; // 	Miembro
		char FIID_TERM	  [04]; // 	Fiid del cajero donde se realizó la transacción
		char TERM_ID	  [06]; // 	Terminal donde se realizó la transacción
		char TERM_TYPE	  [02]; // 	Tipo de terminal [cajero, homebanking,etc.]; // 
		char SEQ_NUM	  [06]; // 	Nro. de secuencia
		char TYP		  [04]; // 	Tipo de mensaje 0210 o 0420 [para reversos]; // 
	} PK;
	char POST_DAT	  [06]; // 	Fecha de posteo del cajero
	char TRAN_DAT	  [06]; // 	Fecha calendario 
	char TRAN_TIM	  [06]; // 	Hora
	char TRAN_CDE	  [06]; // 	Código de transacción
	char MONEDA		  [03]; // 	Código de Moneda, por ahora siempre 032
	char FROM_ACCT	  [19]; // 	Nro. de cuenta desde
	char CBU		  [22]; // 	CBU
	char AMT_1		  [10]; // 	Monto de la transacción
	char TIPO_CAMBIO  [10]; // 	Tipo de Cambio _ Por ahora sin uso
	char RESP_CDE 	  [03]; // 	Código de respuesta [sólo las aprobadas]; // 
	char MOTIVO		  [03]; // 	Motivo de la transferencia
	char REFERENCIA	  [12]; // 	Referencia de la transferencia
	char TIPO_CTA_DEST	  [02]; // 	Tipo de cuenta destino
	char PROPIA       [01]; // 	Se indica si el titular de la cuenta de destino es el mismo titular que el de la cuenta de origen. Los valores posibles son S o N.
	char NOMBRE       [22]; //           Nombre del titular de la cuenta de origen de los fondos.
	char DNI_DEST     [11]; //          CUIT/CUIL/CDI/DNI del destinatario de la transferencia
	char FILLER       [54]; // 
} stEXTRACT_ACH_t; 

#endif /* BANCAEMPESA_H */