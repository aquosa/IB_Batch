//
// IT24 Sistemas S.A.
// Application ISO-8583 Messages as released by ACI-BASE24 Release 4 & 6
//
// Libreria Propietaria de Mensajes ISO Aplicados
//
// Tarea        Fecha           Autor   Observaciones
// (Beta)       2002.08.05      mdc     EXPORT int ISO_Test(BYTE *bBuffer, int iBuffer, bool bFields, FILE* fOutput);
// (Release-1)  2003.08.29      mdc     Verificacion de campo 39 faltante en el mensaje si es reverso
// (Release-2)  2005.08.31      mdc     Descripcion en INGLES de los codigos de Trx
// (Release-3)  2006.02.28      mdc     Mas codigos de trx 
//
// Headers propios
// Header Type Definitions
#include <qusrinc/typedefs.h>
// Aliases
#include <qusrinc/alias.h>
// ISO-8583
#include <qusrinc/iso8583.h>
// ISO-APP Msg
#include <qusrinc/isoapp.h>
// ISO-MSG REDEFINITIONS
#include <qusrinc/isoredef.h>
// Own std.io library 
#include <qusrinc/stdiol.h>

// C Language string library
#include <string.h>


#ifndef BUFFER_SIZE
#define BUFFER_SIZE		(8192)
#endif // BUFFER_SIZE
// Boolean String Constants
const char szTRUE[]  = "TRUE";
const char szFALSE[] = "FALSE";		
// Boolean String Parser
#define BoolStr( x ) ((x) ? szTRUE : szFALSE )

// ISO Test
EXPORT int ISO_Test(BYTE *bBuffer, int iBuffer, bool bFields, 
					char* szOutFile, bool bTruncate)
	{
	// ISO8583 object and secondary fields
	APPMSG        isoMsg;
	BOOL          bResult					= FALSE,
	              bCheckIt					= FALSE;
	WORD          wFieldLength				= 0;
	BYTE          szMsgType[10] 			= {""};
	BYTE          szFieldDesc[64]			= {""};
	BYTE          szFieldValue[BUFFER_SIZE] = {""};
	ADDAMTS_ISO   stAddAmts;
    const ADDAMTS_ISO *pstCBU = &stAddAmts;
	SECRSRVD8_ISO stSecRes8;
	SECRSRVD7_ISO stSecRes7;
	PRIRSRVD1_ISO stRsrvd1;
	FILE          *fOutput = NULL;
	CHAR          szTranCode[10] = {"000000"};
	PCHAR         pszTranCodeDesc = "N/A";


	// Precondition
	if(NULL == szOutFile || iBuffer <= 0 || NULL == bBuffer)
		return (-1);
	// Verify output file
	if ( bTruncate )
	{
		// Open truncating
		if ((fOutput = fopen(szOutFile, "w")) ==NULL )
			return (-2);
	}
	else 
	{
		// Open appending
		if ((fOutput = fopen(szOutFile, "a")) ==NULL )
			return (-2);
	};


	
	// Just do it...	
	bCheckIt = isoMsg.CheckExternParameters(NULL);

	if( !(bResult = isoMsg.Import((PBYTE)bBuffer, iBuffer)) )
	{
		//printf("PAS Message=[%s]\n",bBuffer);
		fprintf(fOutput,"PAS Message=[%s]\n",bBuffer);
		bResult = FALSE;
#if ( _ISO8583_FORMAT_TO_USE_ & _ISO8583_FORMAT_FOR_REDLINK_ )
		bResult=isoMsg.IsPASPaymentFromPOSTerminal();
#endif
		//printf("PAS-Payment-From-POS=%s\n", BoolStr(bResult));
		fprintf(fOutput,"PAS-Payment-From-POS=%s\n", BoolStr(bResult));
	}
		
	bResult = isoMsg.IsValidMessage();	

#if ( _ISO8583_FORMAT_TO_USE_ & _ISO8583_FORMAT_FOR_REDLINK_ )
	if(isoMsg.IsPASTransaction())	
	{
		//printf("PAS Message=[%s]\n",bBuffer);
		fprintf(fOutput,"PAS Message=[%s]\n",bBuffer);
		bResult=isoMsg.IsPASPaymentFromPOSTerminal();
		//printf("PAS-Payment-From-POS=%s\n", BoolStr(bResult));
		fprintf(fOutput,"PAS-Payment-From-POS=%s\n", BoolStr(bResult)); 
	}
	else if(isoMsg.IsAFJPTransaction())	
	{
		//printf("AFJP Message=[%s]\n",bBuffer);
		fprintf(fOutput, "AFJP Message=[%s]\n",bBuffer);
	}
	else if(isoMsg.IsULTMOVTransaction())	
	{
		//printf("AFJP Message=[%s]\n",bBuffer);
		fprintf(fOutput, "AFJP Message=[%s]\n",bBuffer);
	}
	else if(isoMsg.IsSOATTransaction())	
	{
		//printf("SOAT Message=[%s]\n",bBuffer);
		fprintf(fOutput, "\nSOAT Message=[%s]\n",bBuffer);
	}
#endif // ( _ISO8583_FORMAT_TO_USE_ & _ISO8583_FORMAT_FOR_REDLINK_ )

	if(isoMsg.IsATMTransaction())	
	{
		//printf("ATM Message=[%s]\n",bBuffer);
		fprintf(fOutput, "\nATM Message=[%s]\n",bBuffer);
	}
	else
	{
		//printf("ISO-8583 Message=[%s]\n",bBuffer);
		fprintf(fOutput,"\nISO-8583 Message=[%s]\n",bBuffer);
	}

	// Header [0..31]
	//printf("\n#Header=[%-32.32s]\n", (PSTR)bBuffer);	
	bFields?fprintf(fOutput,"\n#Header=[%-32.32s]\n", (PSTR)bBuffer):0;	
	// Primary Bitmap [16..31]
	//printf("#PrimaryBitmap=[%-16.16s]\n", (PSTR)&bBuffer[16]);	
	bFields?fprintf(fOutput,"#PrimaryBitmap=[%-16.16s]\n", (PSTR)&bBuffer[16]):0;	
	
	// MsgType
	wFieldLength = sizeof szMsgType;
	bResult = isoMsg.GetMsgTypeId( szMsgType, wFieldLength );
	//printf("#MsgType=[%s]-Valid?=[%s]\n\n", szFieldValue, BoolStr(bResult));	
	bFields?fprintf(fOutput,"#MsgType=[%s]\n\n", szMsgType):0;	
	
	// For each field present...
	for(int i=1,j=0; i < 128; i++)
	{
		// Check presence...
		if (!isoMsg.IsValidField( i )) continue ;

		// Increment field presence counter
		j++;

		// Field Descriptions		
		switch ( i )
		{			
			case 1  : strcpy( (PSTR)szFieldDesc, "-SecBitmap"); break;
			case 2  : strcpy( (PSTR)szFieldDesc, "-PAN"); break;
			case 3  : strcpy( (PSTR)szFieldDesc, "-TranCode"); break;
			case 4  : strcpy( (PSTR)szFieldDesc, "-Ammount"); break;
			case 7  : strcpy( (PSTR)szFieldDesc, "-XMitDatTim"); break;
			case 11 : strcpy( (PSTR)szFieldDesc, "-TraceNum"); break;
			case 12 : strcpy( (PSTR)szFieldDesc, "-TranTim"); break;
			case 13 : strcpy( (PSTR)szFieldDesc, "-TranDat"); break;
			case 14 : strcpy( (PSTR)szFieldDesc, "-ExpDat"); break;
			case 15 : strcpy( (PSTR)szFieldDesc, "-SetlDat"); break;
			case 17 : strcpy( (PSTR)szFieldDesc, "-PostDate"); break;
			case 19 : strcpy( (PSTR)szFieldDesc, "-CntryCde"); break;
			case 23 : strcpy( (PSTR)szFieldDesc, "-Member"); break;
			case 24 : strcpy( (PSTR)szFieldDesc, "-NetID"); break;
			case 32 : strcpy( (PSTR)szFieldDesc, "-InstID"); break;
			case 34 : strcpy( (PSTR)szFieldDesc, "-PANext"); break;
			case 35 : strcpy( (PSTR)szFieldDesc, "-Track2"); break;
			case 37 : strcpy( (PSTR)szFieldDesc, "-RefNum"); break;
			case 36 : strcpy( (PSTR)szFieldDesc, "-Track3"); break;
			case 39 : strcpy( (PSTR)szFieldDesc, "-RespCode"); break;
			case 41 : strcpy( (PSTR)szFieldDesc, "-TermID"); break;
			case 42 : strcpy( (PSTR)szFieldDesc, "-AccpID"); break;
			case 43 : strcpy( (PSTR)szFieldDesc, "-TermName"); break;
			case 44 : strcpy( (PSTR)szFieldDesc, "-RespData"); break;
			case 46 : strcpy( (PSTR)szFieldDesc, "-AddDataISO"); break;
			case 48 : strcpy( (PSTR)szFieldDesc, "-AddDataPrvt"); break;
			case 49 : strcpy( (PSTR)szFieldDesc, "-Currency"); break;
			case 52 : strcpy( (PSTR)szFieldDesc, "-PIN"); break;
			case 54 : 
                if(isoMsg.IsATMTransferCBU()) // Transferencia x CBU
                    strcpy( (PSTR)szFieldDesc, "-CBU"); 
                else
                    strcpy( (PSTR)szFieldDesc, "-AddAmts"); 
                break;
			case 55 : strcpy( (PSTR)szFieldDesc, "-PriRsrvd1ISO"); break;
			case 60 : strcpy( (PSTR)szFieldDesc, "-PriRsrvd1Prvt"); break;
			case 61 : strcpy( (PSTR)szFieldDesc, "-PriRsrvd2Prvt"); break;
			case 63 : strcpy( (PSTR)szFieldDesc, "-PriRsrvd4Prvt"); break;
			case 70 : strcpy( (PSTR)szFieldDesc, "-NetMngmnt"); break;
			case 100: strcpy( (PSTR)szFieldDesc, "-RcvInstID"); break;
			case 102: strcpy( (PSTR)szFieldDesc, "-Acct1"); break;
			case 103: strcpy( (PSTR)szFieldDesc, "-Acct2"); break;
			case 124: strcpy( (PSTR)szFieldDesc, "-SecRsrvd5"); break;
			case 125: strcpy( (PSTR)szFieldDesc, "-SecRsrvd6"); break;
			case 126: strcpy( (PSTR)szFieldDesc, "-SecRsrvd7"); break;
			case 127: strcpy( (PSTR)szFieldDesc, "-SecRsrvd8"); break;
			default : strcpy( (PSTR)szFieldDesc, ""); break;
			}

		// Specific Field Printing
		if(i == 127)
			{
			// Field #127, Micr-Data
			szFieldValue[0] = 0x00;
			memset((PBYTE)&stSecRes8, 0, sizeof stSecRes8);	
			wFieldLength = sizeof stSecRes8;
			bResult = isoMsg.GetField( 127,
				                    &wFieldLength, 
					                (PBYTE)&stSecRes8,
									sizeof stSecRes8 );
			if( strcmp((char *)szMsgType,"0200")==0 || 
				strcmp((char *)szMsgType,"0220")==0 ||
				strcmp((char *)szMsgType,"0420")==0 )
				// Cotizaciones campo 127, requerimiento
				fprintf(fOutput,
					"#127-Micr-Data[%i]=[Region=%.*s][CambioC=%.*s][CambioV=%.*s]\n"
					"                   [Cambio1=%.*s][Cambio2=%.*s]\n",
					wFieldLength, 
					4,stSecRes8.stATM.stATM_Dl_Rqst.chRegnId  ,
					8,stSecRes8.stATM.stATM_Dl_Rqst.chCambio_C, 
					8,stSecRes8.stATM.stATM_Dl_Rqst.chCambio_V, 
					8,stSecRes8.stATM.stATM_Dl_Rqst.chCambio_1, 
					8,stSecRes8.stATM.stATM_Dl_Rqst.chCambio_2		);
			else
				// Cotizaciones campo 127, respuesta
				fprintf(fOutput,
					"#127-Micr-Data[%i]=[Aplic=%c][CambioC=%.*s][CambioV=%.*s]\n"
					"                   [Cambio1=%.*s][Cambio2=%.*s]\n",
					wFieldLength, 
					stSecRes8.stATM.stATM_Dl_Resp.chCamb_Aplic,
					8,stSecRes8.stATM.stATM_Dl_Resp.chCambio_C, 
					8,stSecRes8.stATM.stATM_Dl_Resp.chCambio_V, 
					8,stSecRes8.stATM.stATM_Dl_Resp.chCambio_1, 
					8,stSecRes8.stATM.stATM_Dl_Resp.chCambio_2		);
			
			}		
		else if(i == 126)
			{
			// Field #126, SecRsrvd7
			szFieldValue[0] = 0x00;
			memset((PBYTE)&stSecRes7, 0, sizeof stSecRes7);
			wFieldLength = sizeof stSecRes7;
			bResult = isoMsg.GetField( 126,
				                    &wFieldLength, 
					                (PBYTE)&stSecRes7,
									sizeof stSecRes7 );

			fprintf(fOutput,"#126-SecRsrv7[%i]=[%s]\n",
				    wFieldLength, stSecRes7.chData);
			

			}
		else if(i == 55)
			{
			// Field#55, AddDataPrvt1
			szFieldValue[0] = 0x00;
			memset((PBYTE)&stRsrvd1, 0, sizeof stRsrvd1);
			wFieldLength = sizeof stRsrvd1;
			bResult = isoMsg.GetField( 55, 
						            &wFieldLength, 
					                (PBYTE)&stRsrvd1, 
									sizeof(stRsrvd1));			
			

			if( strncmp(szTranCode,"39",2)==0 || 
				strncmp(szTranCode,"09",2)==0 ||
				strncmp(szTranCode,"19",2)==0 ||
				strncmp(szTranCode,"29",2)==0 ||
				strncmp(szTranCode,"40",2)==0 )
				fprintf(fOutput,
					"#055-XFER-BANK-DATA[%i]="
					"[Track2=%.*s][CA=%.*s]\n"					
					"  [From Account FIID-TYP-NUM=%.*s-%.*s-%.*s]\n"
					"  [To   Account FIID-TYP-NUM=%.*s-%.*s-%.*s]\n"
					"  [Libre disponibilidad=FromAcct='%c' ToAcct='%c']\n",
					wFieldLength, 
					40,stRsrvd1.stInterbancarias.chTrack2,
					2,stRsrvd1.stInterbancarias.chCA,
					4,stRsrvd1.stInterbancarias.stFrom_Acct.chFiid,
					2,stRsrvd1.stInterbancarias.stFrom_Acct.chTyp,
					19,stRsrvd1.stInterbancarias.stFrom_Acct.chAcct_num,
					4,stRsrvd1.stInterbancarias.stTo_Acct.chFiid,
					2,stRsrvd1.stInterbancarias.stTo_Acct.chTyp,
					19,stRsrvd1.stInterbancarias.stTo_Acct.chAcct_num,
					stRsrvd1.stInterbancarias.chFr_Acct_Typ,
					stRsrvd1.stInterbancarias.chTo_Acct_Typ
					); // arancel de usuario
			else
				bFields?fprintf(fOutput,"#055-PasData[%i]=[%*.*s]\n",	
					wFieldLength, sizeof stRsrvd1.chData, sizeof stRsrvd1.chData,
					(char*)stRsrvd1.chData):0;

			}
		else if(i == 54)
			{

			// Field#54
			szFieldValue[0] = 0x00;
			memset((PBYTE)&stAddAmts, 0, sizeof stAddAmts);
			wFieldLength = sizeof stAddAmts;
			bResult = isoMsg.GetField( 54,
						            &wFieldLength, 
					                (PBYTE)&stAddAmts, 
									sizeof(stAddAmts));
            if(isoMsg.IsATMTransferCBU()) // Transferencia x CBU
			    fprintf(fOutput,
				    "#054-B24-0210-CBU[%i]="
				    "[CBU=%-22.22s]\n",
				    wFieldLength, 
				    pstCBU->stTransferenciaCBU.chCBU ); // CBU
            else
			    fprintf(fOutput,
				    "#054-B24-0210-DEF-DATA[%i]="
				    "[Amt=%.*s]"
				    "[With_adv_avail=%.*s]"
				    "[Int_owe_austral=%.*s]\n"
				    "  [Cash_avail=%.*s]"
				    "[Min_payment=%.*s]"
				    "[Payment_dat=%.*s]"
				    "[Interest_rate=%.*s]\n"
				    "  [Owe_dollar=%.*s]"
				    "[Min_payment_dollar=%.*s]"
				    "[Purchase_dollar=%.*s]"
				    "[Cash_fee=%.*s]\n",
				    wFieldLength, 
				    12,stAddAmts.stB24_0210_Def_Data.chAMT,	 // 12 + reseved 108 bytes
				    2 ,stAddAmts.stB24_0210_Def_Data.chWITH_ADV_AVAIL,	// contador extracciones o adelantos
				    12,stAddAmts.stB24_0210_Def_Data.chINT_OWE_AUSTRAL, // intereses ganados o deuda anterior y LIBRE DISPONIBILIDAD
				    12,stAddAmts.stB24_0210_Def_Data.chCASH_AVAIL, // saldo disponible
				    12,stAddAmts.stB24_0210_Def_Data.chMIN_PAYMENT,
				    6 ,stAddAmts.stB24_0210_Def_Data.chPAYMENT_DAT,
				    6 ,stAddAmts.stB24_0210_Def_Data.chINTEREST_RATE,
				    10,stAddAmts.stB24_0210_Def_Data.chOWE_DOLAR, // deuda en dolares y ACUMULADOR DE EXTRACCIONES
				    10,stAddAmts.stB24_0210_Def_Data.chMIN_PAYMENT_DOLAR,
				    10,stAddAmts.stB24_0210_Def_Data.chPURCHASE_DOLAR,
				    8 ,stAddAmts.stB24_0210_Def_Data.chCASH_FEE); // arancel de usuario
			}
		else
			{
			// Field#i
			szFieldValue[0] = 0x00;
			wFieldLength = sizeof szFieldValue;
			bResult = isoMsg.GetField( i, 
						            &wFieldLength, 
					                szFieldValue, 
									sizeof(szFieldValue));

			// TranCode resguardado
			if(3==i)
			{
				strcpy(szTranCode,(char *)szFieldValue);			

				pszTranCodeDesc = "N/A"; 


				/***********************************************/
				/* CONVERT TO DECIMAL - DO NOT USE STD.LIB. */
				j = (szTranCode[0]-'0')*10 + (szTranCode[1]-'0');  /* CBU=1B : 10+18 = 28 */
				/***********************************************/

				switch ( j )
				{
				case   B24_OPCODE_WITHDRAWAL				  : 
					pszTranCodeDesc = "WITHDRAWAL"; break;
				case   B24_OPCODE_WITHDRAWAL_FASTCASH		  : 
					pszTranCodeDesc = "WITHDRAWAL_FASTCASH"; break;
				case   B24_OPCODE_WITHDRAWAL_FRGN_CURRENCY	  : 
					pszTranCodeDesc = "WITHDRAWAL_FRGN_CURRENCY / BUYING_W_ACCT_DEBIT_CANCEL"; 
					break;
				case   B24_OPCODE_DEPOSIT                 	  : 
					pszTranCodeDesc = "DEPOSIT"; break;
				case   B24_OPCODE_BALANCE_INQUIRY			  : 
					pszTranCodeDesc = "BALANCE_INQ"; break;
				case   B24_OPCODE_PIN_CHANGE     			  : 
					pszTranCodeDesc = "PIN_CHANGE"; break;
				case   B24_OPCODE_PAYMENT_W_DEBIT 			  : 
					pszTranCodeDesc = "PAYMENT_W_DEBIT"; break;
				case   B24_OPCODE_PAYMENT_W_STATEMENT		  : 
					pszTranCodeDesc = "PAYMENT_W_STATEMENT"; break;
				case   B24_OPCODE_PAYMENT_W_CASH 			  : 
					pszTranCodeDesc = "PAYMENT_W_CASH"; break;
				case   B24_OPCODE_TRANSFER				      : 
					pszTranCodeDesc = "TRANSFER"; break;
				case   B24_OPCODE_TRANSFER_FRGN_CURRENCY	  : 
					pszTranCodeDesc = "TRANSFER_FRGN_CURRENCY"; break;
				case   B24_OPCODE_TRANSFER_CBU                : 
					pszTranCodeDesc = "TRANSFER_CBU"; break;
				case   B24_OPCODE_TRANSFER_CBU_ACH            : 
					pszTranCodeDesc = "TRANSFER_CBU_ACH"; break;
				case   B24_OPCODE_BUYING_W_ACCT_DEBIT         : 
					pszTranCodeDesc = "BUYING_ACCT_DEBIT"; break;
				case   B24_OPCODE_BUYING_DEVOLUTION           : 
					pszTranCodeDesc = "BUYING_DEVOLUTION"; break;
				case   B24_OPCODE_BUYING_DEVOLUTION_CANCEL    : 
					pszTranCodeDesc = "BUYING_DEVOLUTION_CANC"; break;
				case   B24_OPCODE_BUYING_CASH_BACK            : 
					pszTranCodeDesc = "BUYING_CASHBACK"; break;
				case   B24_OPCODE_BUYING_CASH_BACK_CANCEL     : 
					pszTranCodeDesc = "BUYING_CASHBACK_CANC"; break;
				/**************************************/
				case   B24_OPCODE_DEBIT_ORDER_XFER     : 
					pszTranCodeDesc = "DEBIT_ORDER_XFER"; break;
				case   B24_OPCODE_CREDIT_ORDER_XFER     : 
					pszTranCodeDesc = "CREDIT_ORDER_XFER"; break;
				default: pszTranCodeDesc = "ERROR"; break;
				}
				/**************************************/

				bFields?fprintf(fOutput,"#%3.3i%s=[%s]-(%s)\n",
					i, szFieldDesc, szFieldValue, pszTranCodeDesc):0;

			}
			else
			{
				bFields?fprintf(fOutput,"#%3.3i%s=[%s]\n",
					i, szFieldDesc, szFieldValue):0;	
			}

			/*********************************************/

			
		};//switch

	};//for-i

    /////////////////////////////////////////////////////////////////////////
    // Falta campo 39 ?
    if(strncmp((char *)szMsgType,"0420",4)==0 && !isoMsg.IsValidField( 39 ) )
    {
        // Falta campo 39
	    fprintf(fOutput,"\n#Falta campo #39 : Lo correcto deberia ser :\n");	
		bResult = isoMsg.InsertField( 39, 
				            &(wFieldLength=2), 
			                (unsigned char *)"01", 
							wFieldLength=2 );
        isoMsg.GetHeader((PSTR)bBuffer, 32);
        isoMsg.GetBitmaps((PSTR)&bBuffer[16], (PSTR)&bBuffer[32]);
		bResult = isoMsg.GetField( 39, 
				            &(wFieldLength=2), 
			                szFieldValue, 
							sizeof(szFieldValue));
	    // Header [0..31]
	    //printf("\n#Header=[%-32.32s]\n", (PSTR)bBuffer);	
	    bFields?fprintf(fOutput,"\n#Header=[%-32.32s]\n", (PSTR)bBuffer):0;	
	    // Primary Bitmap [16..31]
	    //printf("#PrimaryBitmap=[%-16.16s]\n", (PSTR)&bBuffer[16]);	
	    bFields?fprintf(fOutput,"#PrimaryBitmap=[%-16.16s]\n", (PSTR)&bBuffer[16]):0;	
        strcpy( (PSTR)szFieldDesc, "-RespCode"); 
		bFields?fprintf(fOutput,"#%3.3i%s=[%s]\n",
				39, szFieldDesc, szFieldValue):0;	
    }
    /////////////////////////////////////////////////////////////////////////


	// Cerrar archivo de salida
	fclose( fOutput );
	fOutput = NULL;
	// Ok !
	return (0);		

	}

