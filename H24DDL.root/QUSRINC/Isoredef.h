//
// IT24 SISTEMAS S.A.
// ISO-8583 Structures Redefinitions Header
// Automated Teller Machines (ATM) , Point of Sale (POS) for BASE24 Release 4 and 6
//
// Proyecto Pago Automatico de Servicios (PAS)
// Proyecto Ultimos Movimientos (ULTMOV)
// Proyecto Administradoras de Fondos de Jubilacion y Pension (AFJP)
// Proyecto OnLine de Administracion de Tarjetas (SOAT)
//
//	 Estructuras de redefinicion de campos de mensajes ISO-8583, sin considerar la 
// longitud implicita que se informa en el buffer, ya que son solo estructuras de 
// parsing y remapeo. 
//   En base a la DDL del External-Message-Format (EMF) de Base24 Release 4.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1997.11.05      mdc     Add_Ammounts/Primary_Reserved_1_ISO
// (Inicial)    1997.11.17      mdc     Secondary_Reserved_8_Private
// (Inicial)    1997.12.15      mdc     Extension Add_Ammounts
// (Inicial)    1997.12.17      mdc     Extension Primary_Reserved_1_ISO
// (Inicial)    1998.01.30      mdc     Correccion Discrecional Facturacion
// (Inicial)    1998.06.24      mdc     AFJP,ULTMOV. Primary_Reserved_2_Private.
// (Inicial)    1998.07.23      mdc     SOAT. Secondary_Reserved_7_Private.
// (Inicial)    1998.09.02      mdc     Permisos x Trx en respuesta de Logon
// (Inicial)    1998.09.09      mdc     Ampliacion del buffer del campo 126
// (Inicial)    1998.09.14      mdc     RespTxt en Logon a posteriori
// (Inicial)    1998.10.15      mdc     SystemCmd
// (Inicial)    1998.10.28      mdc     ReqResp_t agregado en forma generica
// (Alfa)       1998.12.07      mdc     Response code, field #39
// (Alfa)       1999.06.11      mdc     Logon_t agrega chPIN_Ext_1 y chPIN_Ext_2 (16 y 32 bytes) y
//                                      chQTablas (faltante en definicion original) Total=50 bytes
// (Beta)       2001.12.31      mdc     Agregadas estructuras para ATM (consulta, extraccion, xfer,...)
// (Beta)       2002.11.08      mdc     Agregados a transferencias de LIBRE DISPONIBILIDAD
// (Beta)       2003.08.14      mdc     Adelantos en cuotas, campo 126, transacciones c/cta.credito.
// (Beta)       2003.11.17      mdc     stCrdAccptNameLoc_Alt
// (Delta)      2005.07.18      mdc     NET_MGMT_CODE, FIELD70. NEW TOKENS 06, P7 & others. 
// (Delta)      2005.08.25      mdc     +PRODUCT indicator: '1'=ATM,'2'=POS...
//

#ifndef _ISOREDEF_H_
#define _ISOREDEF_H_

typedef struct Cod_Usuario_t
	{
	char chCod_Ente[3];			// Codigo de Ente
	char chCod_Usr[19];			// Codigo de Usuario
	} stCod_Usuario;            // Estructura Usuario

// PAN, field #2
typedef	struct 
		{
		char chPAN[19];				// Personnal Account Number
		} PAN, FIELD2;

// Processing Code, field #3
typedef	struct 
		{
		union uProcCde_tag {
			struct Tran_cde_tag
			{
				char chTran_Cde[2];				// Cod. Procesamiento
				char chFrom_Acct_Typ[2];		// Tipo Cta desde
				char chTo_Acct_Typ[2];			// Tipo Cta hasta
			} stTranCde;
			char chData[6];
		} uProcCde;
		char chFiller[8];				// Filler
		} PROCCDE_ISO, FIELD3;

// Tran Ammmount, field #4
typedef struct 
		{
		char chAmmount[12];
		} TRAN_AMT , FIELD4;

// Setl. Amount, field #5
typedef struct 
		{
		char chData[12]	;
		} SETL_AMT , FIELD5;

// Billing Amount, field #6
typedef struct 
		{
		char chData[12]	  ;
		} BILLING_AMT , FIELD6;

// Xmit Date Time, field #7
typedef struct 
		{
		char chMMDD[4],
			 chHHMMSS[6] ;
		} XMIT_DAT_TIM , FIELD7;

// Bill Fee, field #8
typedef struct 
		{
		char chData[8];
		} BILL_FEE , FIELD8;

// Setl. Conv. Rate, field #9
typedef struct 
		{
		char chData[8]	;
		} SETL_CONV_RATE , FIELD9;

// Bill. Conv. Rate, field #10
typedef struct 
		{
		char chData[8]	;
		} BILL_CONV_RATE , FIELD10;

// Trace num, field #11
typedef struct 
		{
		char chData[6]	;
		} TRACE_NUM , FIELD11;

// Tran. time, field #12
typedef struct 
		{
		char chHHMMSS[6]  ;
		} TRAN_TIM , FIELD12;

// Tran. date, field #13
typedef struct 
		{		
		char chMMDD[4]	; // En el manual a veces figura como "char chYYMM[4];"
		} TRAN_DAT , FIELD13;

// Exp. date, field #14
typedef struct 
		{
		char chYYMM[4]	;
		} EXP_DAT , FIELD14;

// Setl. date, field #15
typedef struct 
		{
		char chMMDD[4]	;
		} SETL_DAT , FIELD15;

// Conv. date, field #16
typedef struct 
		{
		char chMMDD[4]	;
		} CONV_DAT , FIELD16;

// Posting. date, field #17
typedef struct 
		{
		char chMMDD[4]	 ;
		} POSTING_DAT , CAP_DAT , FIELD17;

// Merchant type code, field #18
typedef struct 
		{
		char chData[4]	;
		} MRCHT_TYP_CDE , FIELD18;

// Acq. Institution country code, field #19
typedef struct 
		{
		char chData[3] ;
		} ACQ_INST_CNTRY_CDE , FIELD19;

// PAN extended country code, field #20
typedef struct 
		{
		char chData[3] ;
		} PAN_EXT_CNTRY_CDE , FIELD20;

// Frwd. Instit. country code, field #21
typedef struct 
		{
		char chData[3];
		} FRWD_INST_CNTRY_CDE , FIELD21;

// POS Entry Mode, field #22
typedef struct 
        {
		char chData[3];
		} ENTRY_MODE , FIELD22;

// Member num., field #23
typedef struct 
		{
		char chData[3] ;
		} MEMBER_NUM , FIELD23;

// Net Intl. ID, field #24
typedef struct 
		{
		char chData[3] ;
		} NETW_INTL_ID , FIELD24;

// Special cond. code, field #25
typedef struct 
		{
		char chData[2]	;
		} PT_TRAN_SPCL_CDE  , FIELD25;

// POS Pin cap. code, field #26
typedef struct 
		{
		char chData[2] ;
		} POS_PIN_CAPTURE_CDE , FIELD26;

// POS Pin cap. code, field #27
typedef struct 
		{
		char chAuthIDRespLen;
		} AUTH_ID_RESP_LEN  , FIELD27;

// Tran fee, field #28
typedef struct 
		{
		char chInd;
		char chAmmount[8];
		} TRAN_FEE , FIELD28;

// Setl. fee, field #29
typedef struct 
		{
		char chInd;
		char chAmmount[8];
		} SETL_FEE , FIELD29;

// Tran Proc. fee, field #30
typedef struct 
		{
		char chInd;
		char chAmmount[8];
		} TRAN_PROC_FEE , FIELD30;

// Setl Proc. fee, field #31
typedef struct 
		{
		char chInd;
		char chAmmount[8];
		} SETL_PROC_FEE , FIELD31;

// Acq. Inst. Id, field #32
typedef struct 
		{		
		char chNum[11];				 
		} ACQ_INST_ID , FIELD32;

// Frwrd. Inst. Id, field #33
typedef struct 
		{		
		char chNum[11];
		} FRWD_INST_ID, FIELD33;

// PAN Extended, field #34
typedef struct 
		{		
		char chNum[28];
		} PAN_EXTENDED, FIELD34;

// Track2, field #35
typedef union
	{	
	struct Track2_t
		{
		char chData[37];				// Datos en forma plana		
		char chFiller[63];				// Datos a futuro (no-ISO)
		} stTrack2;						// Estructura Track2 cruda
	struct AFJPMember_t
		{
		char chPrefix[6];				// Prefijo al Numero Afiliado
		char chMemberNumber[12];		// Numero de Afiliado o Miembro
		char chFiller[19];				// Datos a futuro
		} stAFJPMemberData;				// Estructura Num.Afiliado
	struct Track2_separator_t
		{
		char chSeparator;				// Separador
		char chData[36];				// Datos en forma plana, menos el separador inicial
		char chFiller[63];				// Datos a futuro (no-ISO)
		} stTrack2_separator;			// Estructura Track2 cruda
	} TRACK2_ISO, FIELD35;

// Track3, field #36
typedef struct
	{			
		char chNum[104];
	} TRACK3_ISO, FIELD36;

// Retrieval ref. number, field #37
typedef struct
	{			
		char chNum[12];
	} RETRVL_REF_NUM, FIELD37;

// Auth ID resp, field #38
typedef struct
	{			
		char chNum[6];
	} AUTH_ID_RESP, FIELD38;

// Response code, field #39
typedef struct Resp_Cde_t
	{
	char chRespCde[2];
	} RESP_CDE_ISO, FIELD39;

// Service restrict. code, field #40
typedef struct Service_Cde_t
	{
	char chServiceCde[3];
	} SERVICE_CDE, FIELD40;

// Term id, field #41
typedef struct Term_Id_t
	{
	char chData[16];
	} TERM_ID, FIELD41;

// Card Accpt Id code, field #42
typedef struct 
	{
	char chData[16];
	} CRD_ACCPT_ID_CDE, FIELD42;

// Card-Acceptor-Name-Loc, field #43
typedef union
	{	
	char   chData[30];					// Datos planos para  "?REVERSAL FOR LATE/UNSOLV RESP?"
	struct CrdAccptNameLoc_t
		{
		char chOwner[22];				// Propietario
		char chCity[13];				// Ciudad
		char chState[3];                // Estado-Provincia
		char chCountry[2];				// Pais
		char chFiller[8];				// Datos a futuro
		} stCrdAccptNameLoc;			// Estructura CrdAcceptorName
	struct CrdAccptNameLoc_Alt_t
		{
		char chOwner[25];				// Propietario
		char chCity[13];				// Ciudad		
		char chCountry[2];				// Pais
		char chFiller[8];				// Datos a futuro
		} stCrdAccptNameLoc_Alt ;		// Estructura CrdAcceptorName VISA alternativa
	struct AFJPMember_t
		{
		char chPrefix[3];				// Prefijo Aporte AFJP
		char chMemberNumber[12];		// Numero de Afiliado o Miembro
		char chFiller[25];				// Datos a futuro
		} stAFJPMemberData;				// Estructura Num.Afiliado
	} CRDACCPTNAMELOC_ISO, FIELD43;

// Response data, field #44
typedef union 
	{	
	char chData[25];
	struct stAvail_tag
	{
		char	chType; /* 0,1,2,3,4 o ' ' */		
		char	chOpenAmmount[12];  /* 9(10).99 : saldo apertura */
		char	chAvailAmmount[12]; /* 9(10).99 : disponible     */
	} stAvail;
	} RESP_DATA, FIELD44;

// Track1 ISO, field #45
typedef struct 
	{	
	char chNum[76];
	} TRACK1_ISO , FIELD45;

// Add data ISO, field #46
typedef struct 
	{	
	char chData[100];
	} ADD_DATA_ISO	, FIELD46;

// Add data national, field #47
typedef struct 
	{
    char chData[100];
	} ADD_DATA_NATL , FIELD47;

// Add data private, field #48
typedef union
	{
	struct {
		char chSHRG_GRP[24];
		char chTERM_TRAN_ALLOWED;
		char chTERM_ST[2];
		char chTERM_CNTY[3];
		char chTERM_CNTRY[3];
		char chTERM_RTE_GRP[11];
		char chRESERVED[56];
		} ATM;
	struct {		
		char chRETL_ID[19];
 		char chRETL_GRP[4];
 		char chRETL_REGN[4];
		char chFILLER;
		} POS;
	} ADD_DATA_PRIVATE , FIELD48;

// Currency code, field #49
typedef struct 
	{
    char chData[3];
	} CRNCY_CDE , FIELD49;

// Setl Currency code, field #50
typedef struct 
	{
    char chData[3];
	} SETL_CRNCY , FIELD50;

// Bill Currency code, field #51
typedef struct 
	{
    char chData[3];
	} BILL_CRNCY , FIELD51;

// PIN , field #52
typedef struct 
	{
    char chData[16];
	} PIN , FIELD52;

// Security control info , field #53
typedef struct 
	{
    char chData[16];
	} SEC_CNTRL_INFO , FIELD53;

// Add Ammounts , field #54
typedef union
	{
	char chData[100];					// Datos en forma plana
	struct 
		{
		char chCod_Preauth[2];			// Codigo de Preautorizacion
		Cod_Usuario_t stCod_Usuario;    // Estructura Usuario
		char chCuota_Ano[5];			// Cuota-Ano formato CCCYY
		char chFiller[91];				// Filler de extension
		} stPASData;                    // Estructura PAS-DATA
	struct
		{			
			Cod_Usuario_t stCod_Usuario;
            char chCTA_ANIO[5];
            char chNRO_OPC;         
		} stPASData87_Acquirer;
	struct 
		{
			char chAMT[12];	 // 12 + reseved 108 bytes
            char chTIPO_DEP; // Indica el tipo de deposito, "E"  Efectivo, "C"  Cheque
            char chINVOICES_QTY[2];
            char chPIN_1[4];
            char chPIN_2[4];
            char chFILLER[97];			
		} stB24_0200_Def_Data;
	struct 
		{
			char chAMT[12];	 // 12 + reseved 108 bytes
            char chWITH_ADV_AVAIL[2];	// contador extracciones o adelantos
			char chINT_OWE_AUSTRAL[12]; // intereses ganados o deuda anterior y LIBRE DISPONIBILIDAD
            char chCASH_AVAIL[12]; // saldo disponible
            char chMIN_PAYMENT[12];
            char chPAYMENT_DAT[6];
            char chINTEREST_RATE[6];
            char chOWE_DOLAR[10]; // deuda en dolares y ACUMULADOR DE EXTRACCIONES
            char chMIN_PAYMENT_DOLAR[10];
            char chPURCHASE_DOLAR[10];
            char chCASH_FEE[8]; // arancel de usuario
            char chFILLER[20];
		} stB24_0210_Def_Data;
	struct
		{
			char chCBU[22];	// CBU de cuenta destino a transferir fondos
			char chFiller[98];
		} stTransferenciaCBU;
	} ADDAMTS_ISO , FIELD54;


// Primary Reserved 1 , field #55
typedef union 
	{
	char chData[120];					// Datos en forma plana
	struct 
		{
		char chNro_Usuario[19];			// Codigo de Usuario
		char chOpcion;					// Opcion de Menu Host
		char chCuota_Ano[5];			// Cuota-Ano Host
		char chDef_Usr;					// Default Usuario = "1"
		char chCod_Serv[2];				// Codigo Servicio
		char chQLineas[2];				// Cantidad de Lineas
		char chDiscr_Usu[30];           // Discrecional Usuario fijo
		union 
			{
			char chData[66];	        // Datos en forma plana
			struct DatosFact_t
				{
				char  chCuota_Ano[5];	// Cuota-Ano
				char  chPagado;			// Pagado S/N
				char  chFecha_Vto1[6];	// Fecha Vto 1
				char  chImporte_Vto1[8];// Importe Vto 1
				char  chFecha_Vto2[6];	// Fecha Vto 2
				char  chImporte_Vto2[8];// Importe Vto 2				
				} stDatosFact;          // Estructura Datos Fact.
			struct DatosFactDiscr_t
				{
				char  chCuota_Ano[5];	// Cuota-Ano
				char  chPagado;			// Pagado S/N
				char  chFecha_Vto1[6];	// Fecha Vto 1
				char  chImporte_Vto1[8];// Importe Vto 1
				char  chFecha_Vto2[6];	// Fecha Vto 2
				char  chImporte_Vto2[8];// Importe Vto 2
				char  chDiscr_Fact[32]; // Discrecional Facturacion maximo
				} stDatosFactDiscr;     // Estructura Datos Fact. c/Discrec.
			} stLinea[7];				// Estructura plana Linea
		char chFiller[64];				// Filler de extension
		} stAdhesion ;					// Estructura de Adhesion
	struct 
		{
		char chCod_Abr[2];				// Codigo Abreviado
		char chNro_Cliente[24];			// Numero de Cliente
		char chNombre_Ente[24];			// Nombre del Ente
		char chQLineas[2];				// Cantidad de Lineas
		struct
			{
			char   chData[40];
			} stLinea[7];				// Estructura plana Linea
		char chFiller[28];				// Filler de extension
		} stConsulta ;					// Estructura de Consulta
	struct 
		{
		char chQLineas[2];				// Cantidad de Lineas
		struct
			{
			char   chData[32];
			} stLinea[9];				// Estructura plana Linea
		char chFiller[70];				// Filler de extension
		} stPendiente ;					// Estructura de Pendientes
	struct 
		{
			 char chCOD_ABR[2];
			 char chNRO_ENTE[3];
			 char chIMPORTE[13];
			 char chNRO_CLIENTE[24];
			 char chNOMBRE_ENTE[24];
			 char chVTO[6];
			 char chCUOTA[5];
			 char chLINEA[32];
			 char chCOD_SEG[3];
			 char chDEUDAS_ADIC;
			 char chFILLER[247];
		} stPagos;
	struct
		{      
         char chTrack2[40];
         char chCA[2];
         struct from_acct_t
			{
            char chFiid[4];
            char chTyp[2];
            char chAcct_num[19];
			} stFrom_Acct;
         struct to_acct_t
			{
            char chFiid[4];
            char chTyp[2];
            char chAcct_num[19];
            char chFiid_desc[13];
			} stTo_Acct;
		 //////////////////////////////////////////////////////////////////////////
		 // Agregados para transferencias para transferencias LIBRE DISPONIBILIDAD
		 char chFr_Acct_Typ;
		 char chTo_Acct_Typ;
         char filler[253];
		 //////////////////////////////////////////////////////////////////////////
		} stInterbancarias;
	} PRIRSRVD1_ISO, FIELD55;

// Prim. Rsrv. 2 ISO , field #56
typedef struct 
	{
    char chData[100];
	} PRI_RSRVD2_ISO, FIELD56;

// Prim. Rsrv. 1 National , field #57
typedef struct 
	{
    char chData[100];
	} PRI_RSRVD1_NAT, FIELD57;

// Prim. Rsrv. 2 National , field #58
typedef struct 
	{
    char chData[100];
	} PRI_RSRVD2_NAT, FIELD58;

// Prim. Rsrv. 3 National , field #59
typedef struct 
	{
    char chData[100];
	} PRI_RSRVD3_NAT, FIELD59;

// Primary Reserved 1 Private , field #60
typedef union 
	{
	char chData[200];					// Datos en forma plana
	struct ATM_Term_t
		{
		char chTermFIID[4];             // FIID 
		char chTermLNet[4];				// Logical Net
		char chTermTimeOffset[4];		// Time offset		
		char chReserved[88];            // Reservado
		} stATMTerm;					// Estructura ATM Terminal
	struct POS_Term_t
		{
		char chTermFIID[4];             // FIID 
		char chTermLNet[4];				// Logical Net
		char chTermTimeOffset[4];		// Time offset
		char chPseudoTermID[4];         // Pseudo TermID 
		char chReserved[84];            // Reservado
		} stPOSTerm;					// Estructura POS Terminal
	} PRIRSRVD1PRVT_ISO, FIELD60;


// Primary Reserved 2 Private , field #61
typedef union 
	{
	char chData[200];					// Datos en forma plana
	struct ATM_Data_t
		{
		char chCrdFIID[4];              // FIID 
		char chCrdLNet[4];				// Logical Net
		char chSaveAcctType[4];			// Account Type
		char chAutorizer;				// Autorizador
		char chOrigTranCde[2];			// TranCde
		union
			{
			char chFiller[85];			// Filler
			char chPrnMoneySym[3];		// Simbolos Moneda imprimibles
			} stRedefATMFiller;
		} stATM_Data;					// Estructura ATM
	} PRIRSRVD2_ISO, FIELD61;

// Primary Reserved 3 Private , field #62
typedef union 
	{
	char chData[100];					// Datos en forma plana
	char chPostalCode[10];				// B24-atm postal code
	} PRIRSRVD3_ISO, FIELD62;

// Primary Reserved 4 Private , field #63
typedef union 
	{
	char chData[597];					// Datos en forma plana
	char chNEW_PIN_OFST[16];			// PIN select, new PIN offset
	struct POS_TOKEN_t
		{
		char chHdrIndicator;					// "&"
		char chHdrFiller1;						// " "
		char chHdrQuantity[5];					// 99999
		char chHdrLength[5];					// 99999
		char chSpcIndicator;					// "!"
		char chSpcFiller1;						// " "
		char chSpcId[2];						// XX : e.g. : "06", "P7", ...
		char chSpcLength[5];					// 99999
		char chSpcFiller2;						// " "
		} stPOS_TOKEN ;						// Estructura POS TOKENS, B24R6
    ////////////////////////////////////////////////////////////////////
	} PRIRSRVD4_ISO, FIELD63;

// Primary MAC Code, field #64
typedef struct
	{
	char chData[16];
	} PRI_MAC_CODE, FIELD64;

// Network Management Code, field #70
typedef struct
	{
	char chNMC[3];
	} NET_MGMT_CODE, FIELD70;


// Reversal Info, field #90
typedef union 
	{
	char chData[42];					// Datos en forma plana
	struct
		{
		char    chTyp[4],
				chTrace_Num[6],
				chXmit_Dat_Tim[10],
				chAcq_Inst_Id_Num[11],
				chFwd_Inst_Id_Num[11];
		} stOrigInfo;
	struct
		{
		char    chTyp[4],
				chSeq_Num[12],
				chXmit_Dat[4],
				chXmit_Tim[8],
				chPosting_Dat[4];				
		char	chReserved[10];
		} stB24Orig;
	} ORIG_INFO, FIELD90;


// Replacement for reversals, field #95
typedef struct
	{
	char chReversalAmmount[12];
	char chReserved[30];
	} REPLACEMENT, FIELD95;

// Setl. Inst. ID, field #99
typedef struct
	{
	char chFIID[4];	
	} SETL_INST, FIELD99;

// Rcv. Inst. ID, field #100
typedef struct
	{
	char chFIID[4];	
	} RCV_INST, FIELD100;

// From account 1, field #102
typedef struct
	{
	char chNum[28];	
	} ACCT1, FIELD102;

// To account 2, field #103
typedef struct
	{
	char chNum[28];	
	} ACCT2, FIELD103;

// Secondary Reserved 1 Private , field #120
typedef union 
	{
	char chData[150];					// Datos en forma plana
	struct ATM_Data_t
		{
		char	chTerm_Nam_Loc[25],
				chBrch_Id[4],
				chRegn_Id[4],
				chReserved[117];
		} stATM;
	} SECNDRY_RSRVD1_PRVT, FIELD120;

// Secondary Reserved 3 Private , field #122
typedef  struct
	{
	char chCardIssuerIdentCode[11];		// ID del Host
	} SECNDRY_RSRVD3_PRVT, FIELD122;

// Secondary Reserved 4 Private , field #123
typedef  struct
	{
	char chDepositCreditAmmount[12];		// Monto del Deposito
	} SECNDRY_RSRVD4_PRVT, FIELD123;

// Depository Type, field #124
typedef  struct
	{
    char chDepositoryType;		// Tipo de depositorio en donde se realiza
	} DEPOSITORY_TYPE, FIELD124;

// Secondary Reserved 6 Private , field #125
typedef union 
	{
	char chData[800];					// Datos en forma plana
	union 
		{		
		struct ATM_Data_t
			{
			char chProc_Acct_Ind;
			char chFiller[679];
			} stATM;  // Estructura ATM
		struct STMT_Data_t
			{
			char chPage_Ind[2];
			char chLst_Stmt_Dat[6];
			char chHeader_Lines[2];
			char chColumn_Lines[2];
			union STMT_Fld_t
				{
				char chStmt_Fld[360];
				struct UltMov_t
					{
					char chHeader[30];
					struct UMovLine_t
						{
						char chLine[30];					
						} stUMovLine[10];					
					char chFooter[30];
					} stUltMov;
				struct Deudas_t
					{
					char chCod_Abr[2];
					char chCod_Ente[3];
					char chCod_Usuario[19];
					char chCod_Servicio[24];
					char chDesc_Servicio[24];
					char chCant_Deudas;
					struct Deudas_Vs_t
						{
						char chNro_Opcion;
						char chFiller_1[4];
						char chCuenta[3];
						char chFiller_2;
						char chAno[2];
						char chFiller_3[18];
						} stDeudas_Vs[7];
					char chFiller[84];
					} stDeudas;
				struct EstadoAportes_t
					{
					char chEstado;
					char chResp_Cde[2];
					} stEstadoAportes;
				} stSTMT_Fld ;
			char chUser_Fld[308];
			} stSTMT;	// Estructura Statement
		} stATM_STMT ;	// Estructura ATM-Statement
	} SECRSRVD6_ISO, FIELD125;

///////////////////////////////////////////////////////////////
typedef struct CreditCardAcct_t
        { 
        char DISP_CTA_DOLAR	[12];//Disponible en cuotas en dolares.	
        char MIN_ADEL_PESOS	[12];//Monto mÌnimo de adelanto en pesos.	
        union 
            {
            char MIN_ADEL_DOLAR	[12];//Monto mÌnimo de adelanto en dolares.	
            char SALDO_CTA_PESOS[12];//REDEFINE MIN_ADEL_DOLAR. Saldo en pesos.			            
            } uRedefMin;
        union 
            {
            char MAX_CTA_PESOS	[12];//Monto m·ximo para adelanto en cuotas en pesos.	
            char SALDO_CTA_DOLAR[12];//REDEFINE MAX_CTA_PESOS. Saldo en dolares.	 		            
            } uRedefMax;
        char MAX_CTA_DOLAR	[12];//Monto m·ximo para adelanto en cuotas en dolares.	
        struct CUOTAS_tag
            {
	        char CANT_MAX_PESOS	[2];//Cantidad m·xima de cuotas en pesos.	
	        char CANT_MAX_DOLAR	[2];//Cantidad m·xima de cuotas en dolares.	
	        char FILLER		[2];		            
            } stCUOTAS;
        char TNA_PESOS	[12];//Tasa nominal anual en pesos a aplicar.	
        char TNA_DOLAR	[12];//Tasa nominal anual en dolares a aplicar.	
        char TEM_PESOS	[12];//Tasa estimada mensual en pesos a aplicar.	
        char DISP_CTA_PESOS	[12];//Disponible en cuotas en pesos.	
        char TEM_DOLAR	[12];//Tasa estimada mensual en dolares a aplicar.	
        char CANT_CTA	[2];//Cantidad de cuotas seleccionadas por el usuario.	
        union 
            {
            char TIPO_DEB ;//Flag para imputacion de pagos.	
            char TIPO_TRAN;//REDEFINE TIPO_DEB.  
                           //Flag de adelanto en cuotas. '0' _ transaccion normal'1' _ transaccion en cuotas
            } uRedefTipo;
        char NEW_PIN_DATA	[4];//Nuevo pin seleccionado por el cliente.	
        char CODIGO_BCRA	[3];//Codigo del administrador del cajero.	
        char SERVICE_CDE	[3];//Service code de la tarjeta.	
        } stCreditCardAcct;                 // Estructura para CTA CREDITO - CUOTAS
///////////////////////////////////////////////////////////////

// Secondary Reserved 7 Private , field #126
typedef union 
	{
	char chData[680];						// Datos en forma plana, sin considerar al mensaje SOAT
    ///////////////////////////////////////////////////////////////
	struct POS_t
		{
		char chPre_Auth_Hld[3];
		char chPre_Auth_Hld_SeqNum[12];
		char chRefr_Phone[20];
		char chRea_for_Chrgbck[2];
		char chNum_of_Chrgbck;			
		char chFiller[642];
		} stPOS ;						// Estructura POS
    ///////////////////////////////////////////////////////////////
	struct FhmCAFAcct_t
		{
		char chAcct_Cnt[2];
		struct Acct_t
			{
			char chTyp[2];
			char chNum[28];
			char chStat;
			char chDescr[10];
			char chCorp_PBF_Inx;
			} stAcct[16];
		char chReserved[6];
		} stFhmCAFAcct ;				// Estructura FHM CAF Acct
    ///////////////////////////////////////////////////////////////
	// Estructura para CTA CREDITO - CUOTAS
    struct CreditCardAcct_t stCreditCardAcct;                 
    ////////////////////////////////////////////////////////////////////
	struct ATM_TOKEN_t
		{
		char chHdrIndicator;					// "&"
		char chHdrFiller1;						// " "
		char chHdrQuantity[5];					// 99999
		char chHdrLength[5];					// 99999
		char chSpcIndicator;					// "!"
		char chSpcFiller1;						// " "
		char chSpcId[2];						// XX : e.g. : "06", "P7", ...
		char chSpcLength[5];					// 99999
		char chSpcFiller2;						// " "
		} stATM_TOKEN ;						// Estructura ATM TOKENS, B24R6
    ////////////////////////////////////////////////////////////////////
	} SECRSRVD7_ISO, FIELD126;


// Secondary Reserved 8 Private , field #127
typedef union 
	{
	char chData[100];					// Datos en forma plana
	union 
		{
		char chMicr_Data[43];			// Microdevice-Data 
		struct ATM_Dl_Rqst_t
			{
			char chRegnId[4];
			char chCambio_C[8];
			char chCambio_V[8];
			char chCambio_1[8];
			char chCambio_2[8];
			char chFiller[7];
			} stATM_Dl_Rqst;
		struct ATM_Dl_Resp_t
			{			
			char chCamb_Aplic;
			char chCambio_C[8];
			char chCambio_V[8];
			char chCambio_1[8];
			char chCambio_2[8];
			char chFiller[10];
			} stATM_Dl_Resp;
		struct ATM_Dl_Resp_HISO_t
			{			
			char chCambio_C[8];
			char chCambio_V[8];
			char chFiller[27];
			} stATM_Dl_Resp_HISO;
		struct ATM_AFJP_Resp_HISO_t
			{			
			char chNombre_AFJP[11];
			char chValor_Cuota_Parte[11];
			char chSdo_Anterior[12];
			char chSave_Post_Date[6];
			char chFiller[3];
			} stATM_AFJP_Resp_HISO;
		} stATM ;						// Estructura ATM
	} SECRSRVD8_ISO, FIELD127;

// Estructura contenedora de todos los campos ISO redefinidos,
// listos para usar y parsear
typedef struct APPFIELDS_tag
	{
	boolean_t bLoaded; // Indicador de campos vacios o llenos (parseados o no)
	FIELD2	field2;
	FIELD3	field3;
	FIELD4	field4;
	FIELD5	field5;
	FIELD6	field6;
	FIELD7	field7;
	FIELD8	field8;
	FIELD9	field9;
	FIELD10	field10;
	FIELD11	field11;
	FIELD12	field12;
	FIELD13	field13;
	FIELD14	field14;
	FIELD15	field15;
	FIELD16	field16;
	FIELD17	field17;
	FIELD18	field18;
	FIELD19	field19;
	FIELD20	field20;
	FIELD21	field21;
	FIELD22	field22;
	FIELD23	field23;
	FIELD24	field24;
	FIELD25	field25;
	FIELD26	field26;
	FIELD27	field27;
	FIELD28	field28;
	FIELD29	field29;
	FIELD30	field30;
	FIELD31	field31;
	FIELD32	field32;
	FIELD33	field33;
	FIELD34	field34;
	FIELD35	field35;
	FIELD36	field36;
	FIELD37	field37;
	FIELD38	field38;
	FIELD39	field39;
	FIELD40	field40;
	FIELD41	field41;
	FIELD42	field42;
	FIELD43	field43;
	FIELD44	field44;
	FIELD45	field45;
	FIELD46	field46;
	FIELD47	field47;
	FIELD48	field48;
	FIELD49	field49;
	FIELD50	field50;
	FIELD51	field51;
	FIELD52	field52;
	FIELD53	field53;
	FIELD54	field54;
	FIELD55	field55;
	FIELD56	field56;
	FIELD57	field57;
	FIELD58	field58;
	FIELD59	field59;
	FIELD60	field60;
	FIELD61	field61;
	FIELD62	field62;
	FIELD63	field63;
	FIELD64	field64;
	FIELD90	field90;
	FIELD95	field95;
	FIELD99	field99;
	FIELD100	field100;
	FIELD102	field102;
	FIELD103	field103;
	FIELD120	field120;
	FIELD122	field122;
	FIELD123	field123;
    FIELD124	field124;
	FIELD125	field125;
	FIELD126	field126;
	FIELD127	field127;
	CHAR		chPRODUCT;  // PRODUCT : '1'=ATM,'2'=POS...etc.
	} APPFIELDS ;

/* TOKENS FOR BASE24 R6.0 POS */
typedef struct stToken_04_tag {
	char ERR_FLG,
		 RTE_GRP[11],
		 CRD_VRFY_FLG,
		 CITY_EXT[5],
		 TRACK2_DATA,
		 UAF_FLAG;
} stToken_04 ;

typedef struct stToken_C0_tag {
	char CVD_FLD[4],
		 RESUB_STAT,
		 RESUB_CNTR[3],
		 TERM_POSTAL_CDE[10],
		 E_COM_FLG,
		 CMRCL_CRD_TYP,
		 ADNL_DATA_IND,
		 CVD_FLD_PRESENT,
		 SAF_OR_FORCE_POST,
		 AUTHN_COLL_IND,
		 FRD_PRN_FLAG,
		 CAVV_AAV_RSLT_CDE;
} stToken_C0 ;

typedef struct stToken_C4_tag {
	char TERM_ATTEND_IND,
		 TERM_OPER_IND,
		 TERM_LOC_IND,
		 CRDHLDER_PRESENT_IND,
		 CRD_PRESENT_IND,
		 CRD_CAPTR_IND,
		 TRX_STAT_IND,
 		 TRX_SEC_IND,
		 TRX_RTN_IND,
		 CRDHLDR_ACTVT_TERM_IND,
		 TERM_INPUT_CAP_IND,
		 CRDHLDR_ID_MTHOD;
} stToken_C4 ;

typedef struct stToken_R2_tag {
	char CANT_CUOTAS[2],
		 ENTIDAD_EMISORA[4],
		 PLAN,
		 SITUACION_IVA,
		 FOTO_FLAG,
		 NRO_FACTURA[12],
		 TIPO_ID,
		 NRO_ID[11],
		 TCC,
		 TIPO_FINANCIACION[2],
		 USER_FLD[10];
} stToken_R2 ;

typedef struct stToken_P3_tag {
	char LEN[3],
		 DATOS[199];
	char NUL;
} stToken_P3 ;

// PIN CHANGE TOKEN, BANELCO 
// HEADER : "!"	+  " "	+  "06"	+ "00052" + " "		
typedef struct stToken_06_tag {
	char NEW_PIN_FRMT, // "1"
		 NEW_PIN_OFST[16], // "0000000000000000"
		 PIN_CNT, // "1"
		 NEW_PIN_SIZE[2], // "2"
		 NEW_PIN_1[16],
		 NEW_PIN_2[16]; // "0000000000000000"
} stToken_06 ;

/* TOKENS FOR BASE24 R6.0 ATM CREDIT-CARD */
typedef struct stCreditCardAcct_tag {
	char  
	  DEUDA_DOLARES[12],	   
      DEUDA_DOLARES_CONV[12],	
      DEUDA_PESOS[12],
      PAGO_MINIMO[12],
      PAGO_PESOS[12],	    
      FECHA_VTO[6],
	  PAGO_DOLARES[12],	
	  CONSUMO_DOLARES[12],	
      CONSUMO_PESOS[12],
      DISP_ADELANTOS[12],	      
      LIM_ADELANTOS[12],	   
      CANT_ADELANTOS[2],	
      TIPO_DEBITO,     // 1= AL VENCIMIENTO, 2= EN EL DÌA
      TIPO_PAGO,       // 0=OTRO IMPORTE, 1=PAGO MINIMO, 2=PAGO SALDO, 3=PAGO TOTAL
				       // 1 O 6=EFECTIVO, 2 O 7=CHEQUE
      NEW_PIN_DATA[4],	
      CODIGO_BCRA[3],	
      SERVICE_CDE[3];	
} stCreditCardAcctRedef ;

// CREDIT CARD TOKEN, BANELCO
// HEADER : "!"	+  " "	+  "P7"	+ "00140" + " "		
typedef union stToken_P7_tag {
	stCreditCardAcct	  stCreditCard;
	stCreditCardAcctRedef stCreditCardRedef;
} stToken_P7 ;

/* TOKENS FOR BASE24 R6.0 ATM DEBIT-CARD */
// HEADER : "!"	+  " "	+  "QB"	+ "00090" + " "		
typedef struct stToken_QB_tag {
	char  
        NUMERO_CBU_1[8]  ,	
        NUMERO_CBU_2[14] ,	
        CUIT_CUIL_CDI[11], // Mantisa de 11 digitos para soportar EJ. "20-23606191-5"
        CONCEPTO         , // 1-ALQUILERES,2-CUOTA,3-ExPENSAS,4-FACTURA,5-PRÈSTAMO,
                           // 6-SEGURO,7-HONORARIOS,8-VARIOS
	    TIPO_DOCUMENTO,
        TIPO_CUENTA   , // 1-CUENTA CORRIENTE PROPIA,2-CUENTA CORRIENTE NO PROPIA,
                        // 3-CAJA DE AHORRO PROPIA, 4-CAJA DE AHORRO NO PROPIA.
        IMPORTE_ACUERDO_SOBREGIRO[12],
        INDICACION_SOBREGIRO         , // "S" PRESENTE, "N" NO PRESENTE.	
        INDICACION_INTERESES         ,
        IMPORTE_INTERESES[12]        , 
        IMPORTE_COMISION[12]         ,
        INSTITUCION_TRANSFERIDA[4]   ;
		union uREF_tag
		{ 
			char REFERENCIA[12];
			struct stCREDIT_tag
			{
				char SIN_USO_1                    ,
					 DISPONIBLE[8]                ,
					 INDICACION_DISPONIBLE        ,
					 SIN_USO_2[2]                 ;
			} stCREDIT;
		} uREF;
        char NUL ;
} stToken_QB ;

// HEADER : "!"	+  " "	+  "P2"	+ "00028" + " "		
typedef struct stToken_P2_tag {
	char  
        CODIGO_EMPRESA[4]		,	
        IDENTIFICADOR_DEUDA[19] ,	
        NUMERO_CONTROL[4]       ,
		TIPO_PAGO               ;
} stToken_P2 ;

// HEADER : "!"	+  " "	+  "PC"	+ "00094" + " "		
typedef struct stToken_PC_tag {
	char  
        DATOS_RECIBO[80],	
        EMPRESA[13]     ,	
        SIN_USO[1]      ;
} stToken_PC ;

// HEADER : "!"	+  " "	+  "P6"	+ "00072" + " "		
typedef struct stToken_P6_tag {
	char  
        CODIGO_MONEDA[3],	
        ZONA[4],	
        SIN_USO_1,	
        COTIZACION_COMPRA[12],
        COTIZACION_VENTA[12],
        IMPORTE_CONVERTIDO[12], // IMPORTE CONVERTIDO EN LA MONEDA CONTRARIA A LA QUE SE REALIzÛ LA OPERACIÛN. (CAMPO P-049).
        IMPORTE_CONVERTIDO_DISPENSADO[12], // IMPORTE CONVERTIDO REALMENTE DISPENSADO EN LA 
        // MONEDA CONTRARIA A LA QUE SE REALIzÛ LA OPERACIÛN. (CAMPO P-049).
        // PARA LOS REVERSOS (0420/0421) PARCIALES (CÛDIGO: "32")
        CODIGO_MONEDA_ORIGINAL[3],	
        SIN_USO_2,	
        IMPORTE_MONEDA_ORIGINAL[12];
} stToken_P6 ;

// POS TRX. : CURRENCY CONVERSION RATE
// HEADER : "!"	+  " "	+  "PD"	+ "00048" + " "		
typedef struct stToken_PD_tag {
	char  
        COTIZACION_COMPRA[12],
        COTIZACION_VENTA[12],
        IMPORTE_CONVERTIDO[12], // IMPORTE CONVERTIDO EN LA MONEDA CONTRARIA A LA QUE SE REALIzÛ LA OPERACIÛN. (CAMPO P-049).
        IMPORTE_CONVERTIDO_AJUSTADO[12]; // IMPORTE CONVERTIDO REALMENTE AJUSTADO
} stToken_PD ;

// POS : CASHBACK TRX. : CURRENCY CONVERSION RATE
// HEADER : "!"	+  " "	+  "R6"	+ "00020" + " "		
typedef struct stToken_R6_tag {
	char  
        DISPONIBLE[8],	
        INDICADOR_DISPONIBLE,	
        SIN_USO[11];
} stToken_R6 ;

#endif // ISOREDEF_H

