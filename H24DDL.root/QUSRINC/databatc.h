//
// IT24 Sistemas S.A.
// INTERFACE para DATANET DPS BATCH : Soporte de REGISTRO DPS BATCH y conversion ISO-8583
// 
// D.P.S. BATCH & ISO8583
// DISTRIBUTED PROCESSING SYSTEM - BATCH / ISO-8583
// Version 5.0 
// Version 5.1 (CUIT INCLUIDO)
//
//
// Tarea        Fecha           Autor   Observaciones
// [ALFA]       2006.02.28      mdc     INICIAL
// [ALFA]       2006.03.02      mdc     INICIAL
// [ALFA]       2006.04.03      mdc     DIFCRE no existe en el BATCH. PBYTE pbMAC_Used.
// [ALFA]       2006.05.08      mdc     Interfase de SALDOS y MOVIMIENTOS (Corto y Largo)
// [ALFA]       2006.05.17      mdc     Interfase de HISTORICO DE SALDOS y DIFERIDOS (Corto y Largo)
// [ALFA]       2006.05.22      mdc     Correcciones a MOVCUE e HISTORICO de SALDOS.
// [ALFA]       2006.05.31      mdc     stREGDIA_COMP3_t 
// [ALFA]       2006.06.09      mdc     char TABOFDEB[5] + char TABOFCRE[5] +  char FILLER[2];
// [ALFA]       2006.08.01      mdc     typedef struct ENREGI_AHOSAL_tag
// [ALFA]       2006.08.14      mdc     typedef struct ENREGI_AHOHIS_tag
// [ALFA]       2006.08.15      mdc     ISO_ACCT_LEN = x, DTN_ACCT_LEN = y
// [ALFA]       2007.11.20      mdc     const char  chDB_CR_OPTION = 'D'
//

#ifndef _DATABATC_H_
#define _DATABATC_H_

// DPC System que controla que back-office se usa
#include <qusrinc/dpcsys.h>

// ISO8583 SYSTEM
#include <qusrinc/iso8583.h>
#include <qusrinc/isoapp.h>
#include <qusrinc/extrctst.h>
#include <qusrinc/typedefs.h>
#include <qusrinc/isoredef.h>
#include <qusrinc/trxres.h>


// DPS SYSTEM
#include <qusrinc/datanet.h>



//
// Clase DPS BATCH , con metodos de conversion a ISO8583 y diseño de registro propio.
//
class DPS_i_batch {

public :

    // Constructores x omision y opcionales
    EXPORT DPS_i_batch() { };
    EXPORT DPS_i_batch( string strIpAdress, unsigned short ushIpPort) ;

public:
    // Atributos
    string  strSrvIpAddress ;
    unsigned short ushSrvIpPort;


public :

	// Parsear mensaje DPRE y auditar en stream de salida
	int  EXPORT DPRE_Test(char chBuffer[], int iBuffer, char* szOutFile, bool bTruncate, bool fromBatch);
	// Validacion de trasacciones
	WORD EXPORT DPRE_ProcessInit(char chBuffer[], int iBuffer, boolean_t bIsAuth ,	// Proceso base, validacion		
								 PSTR strDatabase, PSTR strDBUser, PSTR strDBbPassword,
								 PWORD pwProcRespCde , PBYTE pbMAC_Used );

	// Convertir de DPRE a formato ISO8583
	int EXPORT DPRE_ISO8583_Convert(char chBuffer[] , int iBuffer,  // buffer input
                                    char chConvert[], int iConvert, // buffer output convertido
                                    const short nISO_ACCT_LEN ,      // long. cuenta ISO
                                    const short nDTN_ACCT_LEN ,    // long. cuenta DTN
                                    const char  chTrunc_LEFT_RIGHT , //'R','L'
									char  chDB_CR_OPTION = 'D' ); // 'D','C','S','R'
	// Authorizar mensaje ISO8583, ya convertido	
	int EXPORT ISO8583_Authorize( char chISOTrx[] , PWORD pwRespCde);

public :

	// Diseño de registro DPS batch, solo cambian 2 campos con respecto al diseño ONLINE :
	// No existe aqui el campo TIPO_REGISTRO ("TDE"/"TCE") y se suma el campo "MAC2".
	typedef struct data_tag {
	   char TIPOREG	      ,	//TIPO DE REGISTRO
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
			CODMAC2	[  12],	//Message Authentication Code (MAC)	Ver Anexo A
			CODMAC3	[  12],	//Message Authentication Code (MAC)	Ver Anexo A
			NUMREF	[	7],	//Numero de transferencia PC	Nro. De lote (N,3) + nro. de operacion dentro del lote (N,4) 
			NUMENV	[	3],	//Numero de envio del abonado	
			CONSOL	[	1],	//Marca de consolidacion de movimientos ("S" o "N")	"S" se genera un unico movimiento en operaciones de debito con igual fecha de solicitud y  nro. de lote
			MARTIT	[	1],	//Indica que la cuenta de debito y la cuenta de credito son de la misma empresa	
			PRIVEZ	[	1],	//"S" indica que al momento en que se envia el registro al Banco de debito no existe un pago previo aceptado por los dos Bancos que involucre a las cuentas intervinientes."N" indica que los Bancos aceptaron previamente un pago en el que intervenian las mismas cuentas				
			RIEABO	[	1],	//"S" indica que el pago supera el riesgo fijado"N" el pago no supera el riesgo	
			RIEBCO	[	1];	//"S" indica que al ser aceptado el debito la transferencia supero el riesgo establecido"N" la transferencia no supera el riesgo pero tiene tratamiento del credito obligatorio" " indica que la transferencia quedo ejecutada por el debito (no supero el riesgo ni fue tratado el credito)	
	   struct {
		   char CODEST	[	2], //Codigo de estado	
				FECEST	[	6], //Fecha de estado	
				HOREST	[	6];	//Hora del estado	
			} TABEST[  10]; //Tabla de Estados : 10 ocurrencias
	   char INDCUIT	[   1];//Indicativo CUIT
	   char CUITDEB	[  11];//CUIT debito
	   char CUITCRE	[  11];//CUIT credito
       /////////////////////////////////////
       char TABOFDEB[5]; // Tabla Oferentes DB
       char TABOFCRE[5]; // Tabla Oferentes CR
	   char FILLER[2];   // Filler = 2 o 1 ?
       /////////////////////////////////////
		} ;
	struct data_tag data;

	}; // end-DPS-i-batch


//
// -----------------------------------------------------------------------------------------
// C/C++ data types  COBOL data types  
// -----------------------------------------------------------------------------------------
// wchar_t			  DISPLAY-1 (PICTURE N, G)
// wchar_t			  is the processing code whereas DISPLAY-1 is the file code.  
// char				  PIC X.  
// signed char		  No appropriate COBOL equivalent.  
// unsigned char      No appropriate COBOL equivalent.  
// short signed int   PIC S9-S9(4)    COMP-5. Can be COMP, COMP-4, or BINARY if you use the TRUNC(BIN) compiler option.  
// short unsigned int PIC 9-9(4)      COMP-5. Can be COMP, COMP-4, or BINARY if you use the TRUNC(BIN) compiler option.  
// long int			  PIC 9(5)-9(9)   COMP-5. Can be COMP, COMP-4, or BINARY if you use the TRUNC(BIN) compiler option.  
// long long int	  PIC 9(10)-9(18) COMP-5. Can be COMP, COMP-4, or BINARY if you use the TRUNC(BIN) compiler option.  
// float			  COMP-1.  
// double			  COMP-2.  
// enumeration        Equivalent to level 88, but not identical.  
// char(n)            PICTURE X(n).  
// array pointer (*) to type  No appropriate COBOL equivalent.  
// pointer(*) to function     PROCEDURE-POINTER.  
//
// -----------------------------------------
//	TYPE		        byte size
// -----------------------------------------
//	COMP-4.					2
//	COMP-5.					4
//	PIC S9(18) COMP-6.		8
//	PIC  9(09) V99 COMP-6.	8
//	PIC  9(15) V99 COMP-6.	8
//	PIC S9(11) V99 COMP-6.	8 
//  COMP-3            Packed Decimal(N)
// -----------------------------------------
// Example of PACKED DECIMAL : 
// -----------------------------------------
// Value  Comp-3, hex
//     +0           0C
//     +1           1C
//    +12        01 2C 
//   +123        12 3C
//  +1234     01 23 4C
//     -1           1D
//  -1234     01 23 4D
// -----------------------------------------

// -----------------------------------------------------------------------------------------
// COMPUTATIONAL or COMP (binary) 
// This is the equivalent of BINARY. The COMPUTATIONAL phrase is synonymous with BINARY. 
// -----------------------------------------------------------------------------------------
// COMPUTATIONAL-1 or COMP-1 (floating-point) 
// Specified for internal floating-point items (single precision). COMP-1 items are 4 bytes long. 
// COMP-1 data items are affected by the FLOAT(NATIVE|HEX) compiler option. For details, see the COBOL for 
// AIX Programming Guide.
// -----------------------------------------------------------------------------------------
// COMPUTATIONAL-2 or COMP-2 (long floating-point) 
// Specified for internal floating-point items (double precision). COMP-2 items are 8 bytes long. 
// COMP-2 data items are affected by the FLOAT(NATIVE|HEX) compiler option. For details, see the COBOL for 
// AIX Programming Guide.
// -----------------------------------------------------------------------------------------
// COMPUTATIONAL-3 or COMP-3 (internal decimal) 
// This is the equivalent of PACKED-DECIMAL. 
// -----------------------------------------------------------------------------------------
// COMPUTATIONAL-4 or COMP-4 (binary) 
// This is the equivalent of BINARY. 
// -----------------------------------------------------------------------------------------
// COMPUTATIONAL-5 or COMP-5 (native binary) 
// These data items are represented in storage as binary data. The data items can contain values up to 
// the capacity of the native binary representation (2, 4 or 8 bytes), rather than being limited to the 
// value implied by the number of nines in the picture for the item (as is the case for USAGE BINARY data). 
// When numeric data is moved or stored into a COMP-5 item, truncation occurs at the binary field size rather 
// than at the COBOL picture size limit. When a COMP-5 item is referenced, the full binary field size is used 
// in the operation. 
// -----------------------------------------------------------------------------------------
// The TRUNC(BIN) compiler option causes all binary data items (USAGE BINARY, COMP, COMP-4) to be handled as 
// if they were declared USAGE COMP-5. 
// -----------------------------------------------------------------------------------------
// Picture Storage representation Numeric values 
// S9(1) through S9(4) Binary halfword (2 bytes) -32768 through +32767 
// S9(5) through S9(9) Binary fullword (4 bytes) -2,147,483,648 through +2,147,483,647 
// S9(10) through S9(18) Binary doubleword (8 bytes) -9,223,372,036,854,775,808 through +9.223,372,036,854,775,807 
// 9(1) through 9(4) Binary halfword (2 bytes) 0 through 65535 
// 9(5) through 9(9) Binary fullword (4 bytes) 0 through 4,294,967,295 
// 9(10) through 9(18) Binary doubleword (8 bytes) 0 through 18,446,744,073,709,551,615 
// -----------------------------------------------------------------------------------------

//
// Diseño de Registros de OUTPUT 
//
// SALDOS 	-	Saldos de cta. cte. en pesos
//
typedef struct ENREGI_SALDOS_tag
    {	
    char SALDOS_TEXTO				    [6]; // 'SALDOS'
    char CODBAN						    [3];
	char NUMCTA						    [17];
	char CODMON							[3];
	char NOMCUE							[29];
	char NUMEDI							[23];
	char SALCON							[9]; // S9(15)V99 COMP-3	
    char SALINI							[9]; // S9(15)V99 COMP-3	
	char SALOPE							[9]; // S9(15)V99 COMP-3	
	char TOTACU							[9]; // S9(15)V99 COMP-3	
	char MARGEN							[9]; // S9(15)V99 COMP-3	
	char FEULMO							[6];
	char HOULMO							[6];
	char FECAPE							[6];
    char INTDEV							[9]; // S9(15)V99 COMP-3	
	char PAGPROV						[9]; // S9(15)V99 COMP-3	
	char ORDNOP							[2]; // COMP-3
    char CHECOM							[9]; // S9(15)V99 COMP-3	
	char IMPCRE							[9]; // S9(15)V99 COMP-3	
	char IMPDBE							[9]; // S9(15)V99 COMP-3	
	char FILLER							[1815];
	    
    } SALDOS_tag;
	
// 	Descripción del contenido de los campos
//	
//	CODBAN	: 	Código de Banco.
//	NUMCTA 	: 	Número de cuenta según banco.	
//	CODMON	: 	Código de moneda	Tabla 03.	
//	NOMCUE 	: 	Nombre de la cuenta.	
//	NUMEDI 		Número de la cuenta editado	
//	SALCON 	: 	Saldo contable.												
//	SALINI 		Saldo inicial.											
//	SALOPE 	: 	Saldo operativo.	
//	TOTACU 	: 	Total acuerdos del dia.	
//	MARGEN 	: 	Margen de utilización de acuerdos.	
//	FEULMO 	: 	Fecha del último movimiento		AA/MM/DD.	
//	HOULMO 	: 	Hora del último movimiento 		HH/MM/SS.	
//	FECAPE 	: 	Fecha de apertura				AA/MM/DD.	
//	INTDEV 		Intereses devengados.	
//	PAGPRO	: 	Importe de pago aproveedores.	
//	ORDNOP 	: 	Cantidad de ordenes de no pagar.	
//	CHECOM 	: 	Límite de compra de cheques.	
//	IMPCRE 		Total créditos de comercio exterior.	
//	IMPDBE 		Total débitos de comercio exterior.
//

typedef struct HEADER_SALDOS_tag
    {	
    char ASTERISCO1					    [1]; // '*'
    char LETRA_INICIAL		            [1]; // 'I'
    char ASTERISCO2					    [1]; // '*'
    char BANCO                          [3]; // '999'
	char SALDOS_TEXTO				    [6]; // 'SALDOS'
	char FECHA_INICIAL					[6];
    char FILLER                         [sizeof(SALDOS_tag)-3-6-6-3];	    
    } HEADER_SALDOS_tag;

typedef struct FOOTER_SALDOS_tag
    {	
    char ASTERISCO1					    [1]; // '*'
    char LETRA_FINAL  		            [1]; // 'F'
    char ASTERISCO2					    [1]; // '*'
    char BANCO                          [3]; // '999'
	char SALDOS_TEXTO				    [6]; // 'SALDOS'	
    char FILLER                         [sizeof(SALDOS_tag)-3-6-3];	    
    } FOOTER_SALDOS_tag;


//
// AHOSAL 	-	Saldos de cta. ahorro en pesos
//
typedef struct ENREGI_AHOSAL_tag
    {	
    char AHOSAL_TEXTO				[6]; // 'AHOSAL'
	char CODBAN						[3];
	char NUMCTA						[17];
	char TIPCAH						[1];
	char CODMON						[3];
	char NOMCUE						[29];
	char NUMEDI						[23];
	char SALCON						[9]; // S9(15)V99 COMP-3
	char SALINI						[9]; // S9(15)V99 COMP-3
	char SALDIS						[9]; // S9(15)V99 COMP-3
	char FEULMO						[6];
	char HOULMO						[6];
	char FECAPE						[6];
	char INTDEV						[9]; // S9(15)V99 COMP-3
	char EXTRAC						[3];
	char DEPCHE						[3];	
	char FEPRCA						[6];
	char TASNOM						[4]; // 9[05] v9[02]  COMP-3;
	char TASEFE						[4]; // 9[05] v9[02]  COMP-3;
	char SALIND						[9]; // S9(15)V99 COMP-3
	char FILLER						[1841];
    } AHOSAL_tag;

//
// AHSADO 	-	Saldos de cta. ahorro en dolar
//
typedef struct ENREGI_AHSADO_tag
    {	
    char AHSADO_TEXTO				[6]; // 'AHOSAL'
	char CODBAN						[3];
	char NUMCTA						[17];
	// char TIPCAH					[1]; // No corresponde en caja de ahorro en USD, fix 2015.12.22, mdc.
	char CODMON						[3];
	char NOMCUE						[29];
	char NUMEDI						[23];
	char SALCON						[9]; // S9(15)V99 COMP-3
	char SALINI						[9]; // S9(15)V99 COMP-3
	char SALDIS						[9]; // S9(15)V99 COMP-3
	char FEULMO						[6];
	char HOULMO						[6];
	char FECAPE						[6];
	char INTDEV						[9]; // S9(15)V99 COMP-3
	// char EXTRAC					[3]; // No corresponde en caja de ahorro en USD, fix 2015.12.22, mdc.
	char DEPCHE						[3];	
	char FEPRCA						[6];
	char TASNOM						[4]; // 9[05] v9[02]  COMP-3;
	char TASEFE						[4]; // 9[05] v9[02]  COMP-3;
	// char SALIND					[9]; // S9(15)V99 COMP-3 // No corresponde en caja de ahorro en USD, fix 2015.12.22, mdc.
	char TIPCAH						[1];
	char FILLER						[1853]; // [1841 + 1 + 3 + 9]; // Sumar los campos quitados, fix 2015.12.22, mdc.
    } AHSADO_tag;

//
// Diseño de Registros de OUTPUT 
//
// SALHIS -	Saldos Historicos de cta. cte. en pesos
//
//
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

// REGISTRO DE SALDO HISTORICO POR DIA
typedef struct  REGDIA_tag
{
	char SALDIA					[9]; // S9(15)V9(02) COMP-3
	char FECDIA					[6]; // 'YYMMDD'
	char TOTDEB					[9]; // S9(15)V9(02) COMP-3
	char TOTCRE					[9]; // S9(15)V9(02) COMP-3	
} stREGDIA_t ;

// REDEFINICION DE REGISTRO DE SALDO HISTORICO POR DIA, usando COMP-3
typedef struct  REGDIA_COMP3_tag
{
	COMPUTATIONAL_3_t   SALDIA;    // S9(15)V9(02) COMP-3
	DATE_YYMMDD_t       FECDIA;    // 'YYMMDD'
	COMPUTATIONAL_3_t   TOTDEB;    // S9(15)V9(02) COMP-3
	COMPUTATIONAL_3_t   TOTCRE;    // S9(15)V9(02) COMP-3	
} stREGDIA_COMP3_t ;

// TABLA DE SALDOS HISTORICOS POR DIAS
typedef struct TABDIA_ARRAY_tag
{
    stREGDIA_t TABDIA[ 50 ] ;
} stTABDIA_ARRAY_t;

// TABLA DE POSIBLES FERIADOS 
typedef struct TABFER_ARRAY_tag
{
    union
    {
        time_t          tTIME ;       /* BINARY long : 4 bytes (32 bits)       */
        DATE_YYMMDD_t   stYYMMDD;     /* STRUCT "yymmdd" : 6 bytes             */
        char            chDATA[10+1]; /* STRING "yyyy-mm-dd" : 10 BYTES + NULL */
    } TABFER[ 10000 ];
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

// REGISTRO DE SALDOS HISTORICOS MENSUALES Y DIARIOS
typedef struct ENREGI_SALHIS_tag
    {	
    char SALHIS_TEXTO				    [6]; // 'SALHIS'
	char CODBAN							[3];
	char NUMCTA							[17];	
	stTABDIA_ARRAY_t      stTABDIA ;
	stTABMES_ARRAY_t      stTABMES ;
	char FILLER						   [198];
	} SALHIS_tag;

    /**********************************************	
	 Descripción del contenido de los campos
	 **********************************************
	
	CODBAN 	: 	Código de Banco.
	NUMCTA 	: 	Número de cuenta según banco.
	TABDIA		: 	Datos de saldos diarios (50 veces).
	SALDIA 		: 	Saldo contable.
	FECDIA 		: 	Fecha AA/MM/DD
	TOTDEB 	: 	Total de importes de débito.
	TOTCRE 	: 	Total de importes de créditos.
	TABMES 	: 	Datos de saldos mensuales (12 veces).
	SALMEN 	: 	Saldo promedio
	FECMES 	: 	Fecha (Mes).
	***********************************************/	

typedef struct HEADER_SALHIS_tag
    {	
    char ASTERISCO1					    [1]; // '*'
    char LETRA_INICIAL		            [1]; // 'I'
    char ASTERISCO2					    [1]; // '*'
    char BANCO                          [3]; // '999'
	char TEXTO							[6]; // 'SALHIS'
	char FECHA_INICIAL					[6];
    char FILLER                         [sizeof(SALHIS_tag)-3-3-6-6];	    
    } HEADER_SALHIS_tag;

typedef struct FOOTER_SALHIS_tag
    {	
    char ASTERISCO1					    [1]; // '*'
    char LETRA_FINAL  		            [1]; // 'F'
    char ASTERISCO2					    [1]; // '*'
    char BANCO                          [3]; // '999'
	char TEXTO						    [6]; // 'SALHIS'	
    char FILLER                         [sizeof(SALHIS_tag)-3-3-6];	    
    } FOOTER_SALHIS_tag;


// REGISTRO DE SALDOS HISTORICOS MENSUALES Y DIARIOS
typedef struct ENREGI_AHOHIS_tag
    {	
    char AHOHIS_TEXTO				    [6]; // 'AHOHIS'				
	char CODBAN					        [3];
	char NUMCTA					        [17];
	stTABDIA_ARRAY_t        stTABDIA ;
	char FILLER						   [330];
	} AHOHIS_tag;

// REGISTRO DE SALDOS HISTORICOS MENSUALES Y DIARIOS DOLAR
typedef struct ENREGI_AHHIDO_tag
    {	
    char AHOHIS_TEXTO				    [6]; // 'AHOHIS'				
	char CODBAN					        [3];
	char NUMCTA					        [17];
	stTABDIA_ARRAY_t        stTABDIA ;
	char FILLER						   [330];
	} AHHIDO_tag;


//
//	MOVCUE	-	Movimientos de cuentas en pesos
//
typedef struct ENREGI_MOVCUE_tag
    {	
	char MOVCUE_TEXTO				    [6]; // 'MOVCUE'
	char CODBAN	 						[3];
	char NUMCTA							[17];	
	char FECMOV							[6];
	char FECVAL							[6];
	char DEBCRE;
	char IMPORTE						[17]; // PIC 9(15)V99 y ocupa 17 bytes (no es COMP-3)
	char NUMCOR							[2];  // PIC 9(3)     y ocupa 2 bytes  (no es COMP-3)
	char NROCOM							[12];
	char CODOPE							[3];
	char NUMEXT							[4];
	char FECEXT							[6];
	char FECPRO							[6];
	char SUCORI							[5];
	char DEPOSI							[8];
	char MARCOM							[1];
	char DESCRI							[25];
	//char FILLER							[38];	
	char OPEBCO							[05];
	char CUITOR							[11];
	char DENOMI							[40];
	char CBU							[22];
	char FILLER							[194];
    } MOVCUE_tag;

typedef struct HEADER_MOVCUE_tag
    {	
    char ASTERISCO1					    [1]; // '*'
    char LETRA_INICIAL		            [1]; // 'I'
    char ASTERISCO2					    [1]; // '*'
    char BANCO                          [3]; // '999'
	char TEXTO						    [6]; // 'MOVCUE'
	char FECHA_PROCESO					[6]; // FECHA DEL PROCESO
	char FECHA_DESDE					[6]; // FECHA DEDE DONDE SE ENVIAN MOVIMIENTOS
    char FILLER                         [sizeof(MOVCUE_tag)-3-6-6-6-3];	    
    } HEADER_MOVCUE_tag;

typedef struct FOOTER_MOVCUE_tag
    {	
    char ASTERISCO1					    [1]; // '*'
    char LETRA_FINAL  		            [1]; // 'F'
    char ASTERISCO2					    [1]; // '*'
    char BANCO                          [3]; // '999'
	char TEXTO							[6]; // 'MOVCUE'	
    char FILLER                         [sizeof(MOVCUE_tag)-3-6-3];	    
    } FOOTER_MOVCUE_tag;

//	Descripción del contenido de los campos
//	
//	CODBAN 	: 	Código de Banco.
//	NUMCTA 	: 	Número de cuenta según banco.
//	FECMOV 	: 	Fecha movimiento 		AA/MM/DD
//	FECVAL 	: 	Fecha valor 				AA/MM/DD
//	DEBCRE 	:  	Código de débito / crédito	'D' o 'C'
//	IMPORT 		: 	Importe.
//	NUMCOR 	: 	Número corr. técnico para evitar claves dup.
//	NROCOM 	: 	Número de comprobante.
//	CODOPE 	: 	Código de operación 	Tabla 06
//	NUMEXT 	: 	Número de extracto.
//	FECEXT	 	: 	Fecha de extracto 		AA/MM/DD
//	FECPRO 	: 	Fecha proceso 			AA/MM/DD
//	SUCORI	 	: 	Sucursal (Localidad) de origen del movimiento.
//	DEPOSI 		: 	Código de depositante.
//	MARCOM 	: 	Marca de comprobante.
//	DESCRI 		: 	Información adicional libre.


//
//	MOVDIF	-	Movimientos Diferidos de cuentas en pesos
//
typedef struct ENREGI_MOVDIF_tag
    {	
	char MOVDIF_TEXTO				    [6]; // 'MOVDIF'
	char CODBAN	 						[3];
	char NUMCTA							[17];	
	char FECIMP							[6];
	char DEBCRE;
    char FECPRO							[6];	
	char NUMCOR							[4];  
    char IMPORTE						[9]; // PIC 9(15)V99 y ocupa 9 bytes (es COMP-3)
	char NROCOM							[12];
	char CODOPE							[3];
	char CANMOV							[2];
	char SUCORI							[5];
	char DEPOSI							[8];
	char CONTRA							[1];
	char DESCRI							[25];
	char OPEBCO							[5];
	char FILLER							[287];	
    } MOVDIF_tag;

//
//	AHODIF	-	Movimientos Diferidos de cajas en pesos
//
typedef struct ENREGI_AHODIF_tag
    {	
	char AHODIF_TEXTO				    [6]; // 'AHODIF'
	char CODBAN	 						[3];
	char NUMCTA							[17];	
	char FECIMP							[6];
	char DEBCRE;                        // DB-CR
    char FECPRO							[6];	
	char NUMCOR							[2]; // COMPUTATIONAL BINARIO (no COMP-3)
    char IMPORTE						[9]; // PIC 9(15)V99 y ocupa 9 bytes (es COMP-3)
	char NROCOM							[12];
	char CODOPE							[3];
	char CANMOV							[2];
	char SUCORI							[5];
	char DEPOSI							[8];
	char CONTRA							[1];
	char DESCRI							[25];
	char FILLER							[60];	
    } AHODIF_tag;

typedef struct ENREGI_AHDIDO_tag
    {	
	char AHDIDO_TEXTO				    [6]; // 'AHDIDO'
	char CODBAN	 						[3];
	char NUMCTA							[17];	
	char FECIMP							[6];
	char DEBCRE;                        // DB-CR
    char FECPRO							[6];	
	char NUMCOR							[2]; // COMPUTATIONAL BINARIO (no COMP-3)
    char IMPORTE						[9]; // PIC 9(15)V99 y ocupa 9 bytes (es COMP-3)
	char NROCOM							[12];
	char CODOPE							[3];
	char CANMOV							[2];
	char SUCORI							[5];
	char DEPOSI							[8];
	char CONTRA							[1];
	char DESCRI							[25];
	char FILLER							[60];	
    } AHDIDO_tag;

typedef struct HEADER_MOVDIF_tag
    {	
    char ASTERISCO1					    [1]; // '*'
    char LETRA_INICIAL		            [1]; // 'I'
    char ASTERISCO2					    [1]; // '*'
    char BANCO                          [3]; // '999'
	char TEXTO						    [6]; // 'MOVDIF'
	char FECHA_PROCESO					[6]; // FECHA DEL PROCESO
	char FECHA_DESDE					[6]; // FECHA DEDE DONDE SE ENVIAN MOVIMIENTOS
    char FILLER                         [sizeof(MOVDIF_tag)-3-6-6-6-3];	    
    } HEADER_MOVDIF_tag;

typedef struct FOOTER_MOVDIF_tag
    {	
    char ASTERISCO1					    [1]; // '*'
    char LETRA_FINAL  		            [1]; // 'F'
    char ASTERISCO2					    [1]; // '*'
    char BANCO                          [3]; // '999'
	char TEXTO							[6]; // 'MOVDIF'	
    char FILLER                         [sizeof(MOVDIF_tag)-3-6-3];	    
    } FOOTER_MOVDIF_tag;

//////////////////////////////////////////////////////////////////////////////
// Formato DPS BATCH.    
// Mensaje de Salida (OUTPUT)
// Nombre del Campo	Formato	Descripcion
//////////////////////////////////////////////////////////////////////////////
class DPS_o_batch {

public :

	struct {
	   char	TIPOREG		  ,	//TIPO DE REGISTRO
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
// Tabla IV- Estados Batch DPS
// Descripcion = Codigo
//////////////////////////////////////////////////////////////////////////////
typedef enum e_codestad_dps_batch_tag {
	DPS_B_ENVIADO_POR_LA_RED						= 0,
	DPS_B_ENVIADA_AL_BANCO_DE_DEBITO				= 10, 
	DPS_B_PENDIENTE_AUTORIZ_CTA_CREDITO				= 15, 
	DPS_B_DEMORADA_POR_EL_BANCO_DE_DEBITO			= 20,
	DPS_B_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED		= 30,
	DPS_B_VALOR_AL_COBRO							= 40,
	DPS_B_ENVIADA_AL_BANCO_DE_CREDITO				= 50,
	DPS_B_EJECUTADA									= 60,
	DPS_B_RECHAZO_DEL_BANCO_DE_DEBITO				= 70,
	DPS_B_RECHAZO_DEL_BANCO_DE_CREDITO				= 80,
	DPS_B_RECHAZO_CUENTA_CREDITO					= 90,
	DPS_B_NO_ENVIAR_RESPUESTA_PENDIENTE_INT    		= 998,
	DPS_B_NO_ENVIAR_RESPUESTA_HACIA_LA_RED    		= 999
    } e_codestad_dps_batch;

///////////////////////////////////////
#define DPS_B_TIPO_REGISTRO_HEADER '1'
#define DPS_B_TIPO_REGISTRO_BODY   '2'
#define DPS_B_TIPO_REGISTRO_FOOTER '3'
///////////////////////////////////////

typedef enum e_dps_interfases_tag {
    /* CC */
    DPS_INTER_SALDOS ,
	DPS_INTER_SALHIS ,
    DPS_INTER_MOVCUE ,
    DPS_INTER_MOVDIF ,
    /* CA */
    DPS_INTER_AHOSAL ,
    DPS_INTER_AHOHIS ,
    DPS_INTER_AHOMOV ,
    DPS_INTER_AHODIF,
	DPS_INTER_AHMODO,
	DPS_INTER_AHSADO,
	DPS_INTER_AHHIDO,
	DPS_INTER_AHDIDO
} e_dps_interfases;

#endif // _DATABATC_H_





