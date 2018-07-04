//
// IT24 Sistemas S.A.
// Application ISO-8583 Messages
//
// Libreria Propietaria de Mensajes ISO8583 Aplicados
//
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.06.23      mdc     Base (PAS,AFJP,ULTMOV), ACI BASE24 RELEASE 4
// (Inicial)    1998.07.23      mdc     SOAT.Logon.
// (Inicial)    1998.07.23      mdc     SOAT.Logoff. Etc.
// (Inicial)    1998.07.31      mdc     Constructor opcional.
// (Alfa)       1998.08.04      mdc     CheckExternParameters
// (Beta)       1998.08.11      mdc     GenericInquiry/Update/Delete/Insert
// (Beta)       1998.10.15      mdc     SystemCmd
// (Beta)       1999.03.31      mdc     BOOL IsSOATLogonPassword(void)
// (Beta)       2001.12.31      mdc     ATM Messages y BOOL IsControlCenterCommand(void).
// (Beta)       2002.02.01      mdc     Control Center Commands
// (Beta)       2002.03.12      mdc     EXPORT BOOL IsATMLastMovInquiry(void);		// Ult. Movimientos
// (Beta)       2002.04.17      mdc     ::IsATMExchangeRateInquiry()
// (Beta)       2002.07.23      mdc     ::IsReversalOrAdviceRepeat(void);// Forzado (0221,0421)
// (1.0.1.8)    2003.11.10      mdc     OPERATION CODES FOR BASE24-R4 P.O.S. BUYING TRANSACTIONS
// (1.0.1.9)    2005.01.01      mdc     TOKENS BASE24 R6.0 for POSNET
// (1.0.1.10)   2005.07.18      mdc     OPERATION CODES FOR BASE24-R6 A.T.M. TRANSACTIONS
// (1.0.1.11)   2005.09.05      mdc     WORD APPMSG::ExpandFields(void)... and other improvements.
// (1.0.2.2a)   2006.02.28      mdc     #define  B24_OPCODE_CREDIT_ORDER_XFER
// (1.0.2.2b)   2006.03.06      mdc     #if defined( _BASE24_CURRENT_VERSION_ )

//

#ifndef _APPMSG_H_
#define _APPMSG_H_

// Header ISO-8583 Message
#include <qusrinc/iso8583.h>
// Header ISO-8583 Redefined Fix-Size and Variable-Size Fields 
#include <qusrinc/isoredef.h>

/////////////////////////////////////////////////////////////////////////
// CURRENCY CODES FOR THE MONEY INVOLVED
#define _B24_CURRENCY_ARGENTINA_	32   // PESOS ARGENTINA
#define _B24_CURRENCY_USA_			840  // DOLARES	USA
#define _B24_CURRENCY_BRAZIL_		76   // REALES BRAZIL
#define _B24_CURRENCY_URUGUAY_		858  // URUGUAYOS URUGUAY
#define _B24_CURRENCY_LECOP_		996  // LECOP ARGENTINA

// MDC 2006.03.07 ///////////////////////////////////////////////////////
#ifndef  _BASE24_CURRENT_VERSION_ 
#error "Sin definir VERSION DE BASE24"
#else

#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )
#pragma comment(user, "VERSION 4.0 DE BASE24")
#elif ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE6_ )
#pragma comment(user, "VERSION 6.0 DE BASE24")
#else
#error "VERSION DESCONOCIDA DE BASE24"
#endif

#endif
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// REVERSAL CODES BASE24 RELEASE 4? RELEASE 6?
#if ( _BASE24_CURRENT_VERSION_ >= _BASE24_RELEASE4_  )
#define CISO_REV_NO_APPLICABLE		0
#define CISO_REV_CANCELLED_TRX		17
#define CISO_REV_COMMAND_REJECTED	20
#define CISO_REV_HARDWARE_ERROR		21
#define CISO_REV_SUSPICIUS_TRX		22
#define CISO_REV_PARTIAL			32
#define CISO_REV_TIMEOUT			68
#define CISO_REV_DEST_NOT_AVAIL		82
#define CISO_REV_TIMEOUT_ADVICE		168
///2005-07-18/////////////////////////////////////////////////////////////
#elif 
///2005-07-18/////////////////////////////////////////////////////////////
#define CISO_REV_NO_APPLICABLE		0
#define CISO_REV_CANCELLED_TRX		17
#define CISO_REV_COMMAND_REJECTED	20
#define CISO_REV_HARDWARE_ERROR		21
#define CISO_REV_SUSPICIUS_TRX		22
#define CISO_REV_PARTIAL			32
#define CISO_REV_TIMEOUT			68
#define CISO_REV_TIMEOUT_HOST		99
#define CISO_REV_DEST_NOT_AVAIL		82
#define CISO_REV_TIMEOUT_ADVICE		168
#endif
/////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
// OPERATION CODES FOR BASE24-R4 and R6 A.T.M. TRANSACTIONS
#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )
#define  B24_OPCODE_WITHDRAWAL					(01) // Extraccion
#define  B24_OPCODE_WITHDRAWAL_FASTCASH         (999) // NOT AVAIL
#define  B24_OPCODE_WITHDRAWAL_FRGN_CURRENCY    (998) // NOT AVAIL
#define  B24_OPCODE_DEPOSIT                 	(21) // Deposito
#define  B24_OPCODE_BALANCE_INQUIRY				(31) // Consulta de Saldo
#define  B24_OPCODE_BALANCE_INQUIRY_POS			(73) // Consulta de Saldo POS
#define  B24_OPCODE_PIN_CHANGE     			    (32) // Cambio de PIN - codigo
#define  B24_OPCODE_TRANSFER				    (40) // Transferencia
#define  B24_OPCODE_TRANSFER_FRGN_CURRENCY      (997) // NOT AVAIL
#define  B24_OPCODE_TRANSFER_CBU                (28) /* "1B" */
#define  B24_OPCODE_TRANSFER_CBU_ACH            (995) // NOT AVAIL
#define  B24_OPCODE_DEBIT_ORDER_XFER            (9)  // Orden de Debito x transferencia
#define  B24_OPCODE_CREDIT_ORDER_XFER           (19) // Orden de Credito x transferencia
#define  B24_OPCODE_PAYMENT_W_DEBIT 			(50) // Pago con debito en cuenta
#define  B24_OPCODE_PAYMENT_W_CASH 				(51) // Pago con sobre y dinero efevo.
#define  B24_OPCODE_PAYMENT_W_STATEMENT         (996) // NOT AVAIL
#define  B24_OPCODE_BUYING_W_ACCT_DEBIT         (71) // Compra con Debito
#define  B24_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL  (72) // Anulacion compra (credito)
#define  B24_OPCODE_BUYING_DEVOLUTION           (74) // Devolucion compra (credito NO se procesa)
#define  B24_OPCODE_BUYING_DEVOLUTION_CANCEL    (75) // Anul. Devolucion compra (debito NO se procesa)
#define  B24_OPCODE_BUYING_CASH_BACK            (76) // Compra cash-back
#define  B24_OPCODE_BUYING_CASH_BACK_CANCEL     (77) // Anulacion Compra cash-back
#define  B24_OPCODE_BUYING_PREAUTH              (78) // Preautorizacion NO se procesa
#define  B24_OPCODE_BUYING_PREAUTH_CANCEL       (79) // Anul. Preautorizacion NO se procesa
#define  B24_OPCODE_PAYMENT_W_DEBIT_ORDER		(90) // Pago con orden de debito
#define  B24_OPCODE_PAYMENT_TELEPHONY_SRVCS	    (17) // Pagos Serv.Telef.
#define  B24_OPCODE_AUT_PAYMENT_DEBIT  		     (81) // Pagos Serv (80..89)
#define  B24_OPCODE_AUT_PAYMENT_DEBIT_ORDER	     (86) // Pagos Serv (80..89)
#define  B24_OPCODE_AUT_PAYMENT_CRED_CARD		 (88) // Pagos Serv (80..89)

#define  _B24_OPCODE_WITHDRAWAL_				  "01" // Extraccion
#define  _B24_OPCODE_WITHDRAWAL_FASTCASH_		  "11" // Extraccion FastCash
#define  _B24_OPCODE_WITHDRAWAL_FRGN_CURRENCY_    "??"
#define  _B24_OPCODE_DEPOSIT_                 	  "21" // Deposito
#define  _B24_OPCODE_BALANCE_INQUIRY_			  "31" // Consulta de Saldo
#define  _B24_OPCODE_BALANCE_INQUIRY_POS_		  "73" // Consulta de Saldo POS
#define  _B24_OPCODE_PIN_CHANGE_   			      "32" // Cambio de PIN - codigo
#define  _B24_OPCODE_TRANSFER_				      "40" // Transferencia
#define  _B24_OPCODE_DEBIT_ORDER_XFER_            "09" // Orden de Debito x transferencia
#define  _B24_OPCODE_CREDIT_ORDER_XFER_           "19" // Orden de Credito x transferencia
#define  _B24_OPCODE_QUERY_ORDER_XFER_            "29" // Consulta x transferencia
#define  _B24_OPCODE_VERIF_ACCT_XFER_             "39" // Consulta x verif. transferencia
#define  _B24_OPCODE_PAYMENT_W_DEBIT_ 			  "50" // Pago con debito en cuenta
#define  _B24_OPCODE_PAYMENT_W_CASH_ 			  "51" // Pago con sobre y dinero efevo.
#define  _B24_OPCODE_PAYMENT_W_STATEMENT_         "??"
#define  _B24_OPCODE_BUYING_W_ACCT_DEBIT_         "71" // Compra con Debito
#define  _B24_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL_  "72" // Anulacion compra "credito"
#define  _B24_OPCODE_BUYING_DEVOLUTION_           "74" // Devolucion compra "credito NO se procesa"
#define  _B24_OPCODE_BUYING_DEVOLUTION_CANCEL_    "75" // Anul. Devolucion compra "debito NO se procesa"
#define  _B24_OPCODE_BUYING_CASH_BACK_            "76" // Compra cash-back
#define  _B24_OPCODE_BUYING_CASH_BACK_CANCEL_     "77" // Anulacion Compra cash-back
#define  _B24_OPCODE_BUYING_PREAUTH_              "78" // Preautorizacion NO se procesa
#define  _B24_OPCODE_BUYING_PREAUTH_CANCEL_       "79" // Anul. Preautorizacion NO se procesa
#define  _B24_OPCODE_PAYMENT_W_DEBIT_ORDER_		  "90" // Pago con orden de debito
#define  _B24_OPCODE_TRANSFER_CBU_                "1B" // Transferencia CBU
#define  _B24_OPCODE_TRANSFER_CBU_ACH_            _B24_OPCODE_TRANSFER_CBU_ // Transferencia ACH
#define  _B24_OPCODE_TRANSFER_FRGN_CURRENCY_      "??"
#define  _B24_OPCODE_PAYMENT_TELEPHONY_SRVCS_  	  "17" // Pagos Serv.Telef.
#define  _B24_OPCODE_QUERY_LAST_MOVEMENTS_		  "94" // Ultimos Movs.
#define  _B24_OPCODE_HBANK_USER_   			      "34" // Usuario HB-HomeBanking
#define  _B24_OPCODE_INQUIRY_EXCH_RATE_		      "35" // Consulta Cambio U$S
#define  _B24_OPCODE_AUT_PAYMENT_SRVCS_  		  "8*" // Pagos Serv (80..89)
#define  _B24_OPCODE_MSG_TO_INSTITUTION_  		  "9*" // Msg to Bank 
#define  _B24_OPCODE_AUT_PAYMENT_DEBIT_  		  "81" // Pagos Serv (80..89)
#define  _B24_OPCODE_AUT_PAYMENT_DEBIT_ORDER_	  "86" // Pagos Serv (80..89)
#define  _B24_OPCODE_AUT_PAYMENT_CRED_CARD_		  "88" // Pagos Serv (80..89)

///2005-07-18//////////////////////////////////////////////////////////////////////
#elif ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE6_ )
///2005-07-18//////////////////////////////////////////////////////////////////////

#define  B24_OPCODE_WITHDRAWAL					(01) // Extraccion
#define  B24_OPCODE_WITHDRAWAL_FASTCASH		    (11) // Extraccion FastCash
#define  B24_OPCODE_WITHDRAWAL_FRGN_CURRENCY	(02) // Extraccion Moneda Extranjera
#define  B24_OPCODE_DEPOSIT                 	(21) // Deposito
#define  B24_OPCODE_BALANCE_INQUIRY				(31) // Consulta de Saldo
#define  B24_OPCODE_BALANCE_INQUIRY_POS			B24_OPCODE_BALANCE_INQUIRY // Consulta de Saldo
#define  B24_OPCODE_PIN_CHANGE     			    (96) // Cambio de PIN - codigo
#define  B24_OPCODE_PAYMENT_W_DEBIT 			(50) // Pago con debito en cuenta, s/deuda
#define  B24_OPCODE_PAYMENT_W_STATEMENT			(51) // Pago con debito, con deuda informada
#define  B24_OPCODE_PAYMENT_W_CASH 				(90) // Pago con sobre y dinero efevo.
#define  B24_OPCODE_PAYMENT_W_DEBIT_ORDER		B24_OPCODE_PAYMENT_W_CASH
#define  B24_OPCODE_TRANSFER				    (40) // Transferencia
#define  B24_OPCODE_TRANSFER_FRGN_CURRENCY	    (41) // Transferencia moneda extranjera
#define  B24_OPCODE_TRANSFER_CBU                (47) // Transferencia CBU
#define  B24_OPCODE_TRANSFER_CBU_ACH            (48) // Transferencia hacia ACH
#define  B24_OPCODE_BUYING_W_ACCT_DEBIT         (00) // Compra con Debito
#define  B24_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL  (02) // Anulacion compra (credito)
#define  B24_OPCODE_BUYING_DEVOLUTION           (20) // Devolucion compra (credito NO se procesa)
#define  B24_OPCODE_BUYING_DEVOLUTION_CANCEL    (22) // Anul. Devolucion compra (debito NO se procesa)
#define  B24_OPCODE_BUYING_CASH_BACK            ( 9) // Compra cash-back
#define  B24_OPCODE_BUYING_CASH_BACK_CANCEL     (19) // Anulacion Compra cash-back
#define  B24_OPCODE_DEBIT_ORDER_XFER            B24_OPCODE_TRANSFER_CBU  // Orden de Debito x transferencia
#define  B24_OPCODE_PAYMENT_TELEPHONY_SRVCS	    (17) // Pagos Serv.Telef.
#define  B24_OPCODE_AUT_PAYMENT_SRVCS  		    (8)  // Pagos Serv (80..89)


#define  _B24_OPCODE_WITHDRAWAL_				"01" // Extraccion
#define  _B24_OPCODE_WITHDRAWAL_FASTCASH_		"11" // Extraccion FastCash
#define  _B24_OPCODE_WITHDRAWAL_FRGN_CURRENCY_	"02" // Extraccion Moneda Extranjera
#define  _B24_OPCODE_DEPOSIT_                 	"21" // Deposito
#define  _B24_OPCODE_BALANCE_INQUIRY_			"31" // Consulta de Saldo
#define  _B24_OPCODE_BALANCE_INQUIRY_POS_		_B24_OPCODE_BALANCE_INQUIRY_ // Consulta de Saldo
#define  _B24_OPCODE_PIN_CHANGE_     			"96" // Cambio de PIN - codigo
#define  _B24_OPCODE_PAYMENT_W_DEBIT_ 			"50" // Pago con debito en cuenta, s/deuda
#define  _B24_OPCODE_PAYMENT_W_STATEMENT_		"51" // Pago con debito, con deuda informada
#define  _B24_OPCODE_PAYMENT_W_DEBIT_ORDER_		"90" // Pago con sobre y dinero efevo.
#define  _B24_OPCODE_PAYMENT_W_CASH_			_B24_OPCODE_PAYMENT_W_DEBIT_ORDER_
#define  _B24_OPCODE_TRANSFER_				    "40" // Transferencia
#define  _B24_OPCODE_TRANSFER_FRGN_CURRENCY_    "41" // Transferencia moneda extranjera
#define  _B24_OPCODE_TRANSFER_CBU_               "47" // Transferencia CBU
#define  _B24_OPCODE_TRANSFER_CBU_ACH_           "48" // Transferencia hacia ACH
#define  _B24_OPCODE_BUYING_W_ACCT_DEBIT_        "00" // Compra con Debito
#define  _B24_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL_ "02" // Anulacion compra "credito"
#define  _B24_OPCODE_BUYING_DEVOLUTION_          "20" // Devolucion compra "credito NO se procesa"
#define  _B24_OPCODE_BUYING_DEVOLUTION_CANCEL_   "22" // Anul. Devolucion compra "debito NO se procesa"
#define  _B24_OPCODE_BUYING_CASH_BACK_           "09" // Compra cash-back
#define  _B24_OPCODE_BUYING_CASH_BACK_CANCEL_    "19" // Anulacion Compra cash-back
#define  _B24_OPCODE_AUT_PAYMENT_SRVCS_  		 "8*" // Pagos Serv (80..89)
#define  _B24_OPCODE_MSG_TO_INSTITUTION_  		 "9*" // Msg to Bank (90..99)
#define  _B24_OPCODE_DEBIT_ORDER_XFER_           "??" // NOT AVAIL  
#define  _B24_OPCODE_CREDIT_ORDER_XFER_          "??" // NOT AVAIL
#define  _B24_OPCODE_QUERY_ORDER_XFER_           "??" // NOT AVAIL
#define  _B24_OPCODE_VERIF_ACCT_XFER_            "??" // NOT AVAIL
#define  _B24_OPCODE_PAYMENT_TELEPHONY_SRVCS_  	 "??" // NOT AVAIL
#define  _B24_OPCODE_QUERY_LAST_MOVEMENTS_		 "??" // NOT AVAIL
#define  _B24_OPCODE_HBANK_USER_   			     "??" // NOT AVAIL
#define  _B24_OPCODE_INQUIRY_EXCH_RATE_		     "??" // NOT AVAIL
#define  _B24_OPCODE_BUYING_PREAUTH_             "??" // NOT AVAIL
#define  _B24_OPCODE_BUYING_PREAUTH_CANCEL_      "??" // NOT AVAIL
#endif


/////////////////////////////////////////////////////////////////////////
// ACCOUNT TYPES DEPENDING ON BASE24 RELEASE 
#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )
#define B24_CHECKINGS_ACCOUNT_USD	"07"    // CUENTA CORRIENTE EN DOLARES
#define B24_SAVINGS_ACCOUNT			"10"    // CAJA DE AHORROS EN PESOS
#define B24_SPECIAL_ACCOUNT_AFJP	"13"    // CUENTA AFJP
#define B24_SAVINGS_ACCOUNT_BOND	"14"    // CUENTA BONO-C.A.	*** por ahora no hay diferencia con C.C. ***
#define B24_SAVINGS_ACCOUNT_USD		"15"	// CAJA DE AHORROS EN DOLARES
#define B24_SPECIAL_ACCOUNT_LECOP	"16"    // CUENTA LECOP
#define B24_CHECKINGS_ACCOUNT		"20"	// CUENTA CORRIENTE EN PESOS
#define B24_CHECKINGS_ACCOUNT_BOND	"14"	// CUENTA BONO-C.C. *** por ahora no hay diferencia con C.A. ***
#define B24_CREDIT_CARD_ACCOUNT		"30"	// CUENTA TARJETA DE CREDITO EN PESOS
#define B24_CREDIT_LINE_ACCOUNT		"32"	// CUENTA CREDITO VISTA EN PESOS
///2005-07-18//////////////////////////////////////////////////////////////////////
#else
#define B24_SAVINGS_ACCOUNT			"10"   // CAJA DE AHORROS PESOS
#define B24_SAVINGS_ACCOUNT_USD		"11"   // CAJA DE AHORROS DOLARES
#define B24_SAVINGS_ACCOUNT_BOND	"14"   // CUENTA BONO EN PESOS
#define B24_CHECKINGS_ACCOUNT		"20"   // CUENTA CORRIENTE PESOS
#define B24_CHECKINGS_ACCOUNT_USD	"21"   // CUENTA CORRIENTE DOLARES
#define B24_CREDIT_CARD_ACCOUNT		"31"   // CUENTA TARJETA DE CREDITO EN PESOS
#define B24_CREDIT_ACCOUNT			"30"   // CUENTA CREDITO EN PESOS
#define B24_CREDIT_LINE_ACCOUNT		B24_CREDIT_ACCOUNT	
#define B24_TRANSFER_ACCOUNT_CBU	"40"   // CUENTA TRANSFERENCIA CBU
#endif
///2005-07-18//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// ACCOUNT TYPES DEPENDING ON BASE24 RELEASE 
#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )
#define _B24_SAVINGS_ACCOUNT_			10   // CAJA DE AHORROS EN PESOS
#define _B24_SAVINGS_ACCOUNT_USD_		15	 // CAJA DE AHORROS EN DOLARES
#define _B24_SAVINGS_ACCOUNT_BOND_		14   // CUENTA BONO
#define _B24_SPECIAL_ACCOUNT_AFJP_		13   // CUENTA AFJP
#define _B24_SPECIAL_ACCOUNT_LECOP_		16   // CUENTA LECOP
#define _B24_CHECKINGS_ACCOUNT_			20	 // CUENTA CORRIENTE EN PESOS
#define _B24_CHECKINGS_ACCOUNT_USD_		7    // CUENTA CORRIENTE EN DOLARES
#define _B24_CHECKINGS_ACCOUNT_BOND_	14	 // CUENTA CORRIENTE EN BONOS *** por ahora no hay diferencia con C.A. ***
#define _B24_CREDIT_CARD_ACCOUNT_		30	 // CUENTA TARJETA DE CREDITO EN PESOS
#define _B24_CREDIT_LINE_ACCOUNT_		32	 // CUENTA CREDITO VISTA EN PESOS
#define _B24_CREDIT_ACCOUNT_			98   // NOT USED -- VALID UNDER R6
#define _B24_TRANSFER_ACCOUNT_CBU_      99   // NOT USED -- VALID UNDER R6
///2005-07-18//////////////////////////////////////////////////////////////////////
#else
#define _B24_SAVINGS_ACCOUNT_			10   // CAJA DE AHORROS PESOS
#define _B24_SAVINGS_ACCOUNT_USD_		11   // CAJA DE AHORROS DOLARES
#define _B24_SAVINGS_ACCOUNT_BOND_		14   // CUENTA BONO EN PESOS
#define _B24_CHECKINGS_ACCOUNT_			20   // CUENTA CORRIENTE PESOS
#define _B24_CHECKINGS_ACCOUNT_USD_		21   // CUENTA CORRIENTE DOLARES
#define _B24_CREDIT_CARD_ACCOUNT_		31	 // CUENTA TARJETA DE CREDITO EN PESOS
#define _B24_CREDIT_ACCOUNT_			30	 // CUENTA CREDITO EN PESOS
#define _B24_CREDIT_LINE_ACCOUNT_		_B24_CREDIT_ACCOUNT_	
#define _B24_TRANSFER_ACCOUNT_CBU_      40   // CUENTA TRANSFERENCIA CBU	
#endif
///2005-07-18//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
// SOME LOGICAL-NET CODES 
#define B24_LNET_PRODUCTION "PRO1"  // ACTUAL PRODUCCION - SIEMPRE!
#define B24_LNET_TEST       "TES1"  // ACTUAL PRUEBAS    - SIEMPRE!
#define B24_LNET_REDLINK    "LINK"  // RED LINK - ARGENTINA
#define B24_LNET_BANELCO    "BANE"  // BANELCO - ARGENTINA
#define B24_LNET_POSNET     "PSNT"  // POSNET - ARGENTINA
#define B24_LNET_VISA       "4501"  // VISA - ARGENTINA
/////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// SPECIFY WHICH FORMAT SHOULD BE COMPILED AND USED
//////////////////////////////////////////////////////////////////////
#define _ISO8583_FORMAT_FOR_ATM_			0x01
#define _ISO8583_FORMAT_FOR_POS_			0x02
#define _ISO8583_FORMAT_FOR_TELLER_			0x03
#define _ISO8583_FORMAT_FOR_MAINTENANCE_	0x08
#define _ISO8583_FORMAT_FOR_EMS_			0x0B
#define _ISO8583_FORMAT_FOR_EFTPOS_			0x0C
//////////////////////////////////////////////////////////////////////
#define _ISO8583_FORMAT_FOR_AFJP_			0x10
#define _ISO8583_FORMAT_FOR_SOAT_			0x20
#define _ISO8583_FORMAT_FOR_ULTMOV_			0x30
//////////////////////////////////////////////////////////////////////
#define _ISO8583_FORMAT_FOR_CB2A_			0x40
//////////////////////////////////////////////////////////////////////
#define _ISO8583_FORMAT_TO_USE_		  ( _ISO8583_FORMAT_FOR_ATM_  | _ISO8583_FORMAT_FOR_POS_ )
#define _ISO8583_FORMAT_FOR_REDLINK_  ( _ISO8583_FORMAT_FOR_AFJP_ | \
										_ISO8583_FORMAT_FOR_SOAT_ | \
										_ISO8583_FORMAT_FOR_ULTMOV_  )
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////

// Clase Mensaje de Aplicacion (PAS,ULTMOV,AFJP,SOAT,MEP,etc.)
class APPMSG : public ISO8583MSG
    {
    public:
        
        // Metodos publicos de la Clase        
        EXPORT APPMSG() : ISO8583MSG() {}					// Constructor default
        EXPORT APPMSG(PBYTE pbBuff, WORD wLen, PSTR szIni)
					  : ISO8583MSG(pbBuff, wLen, szIni) {}  // Constructor opcional

		// Verificacion externa de parametros
		EXPORT BOOL CheckExternParameters(PBYTE=NULL);

		// Mensajes especificos de Manejo de Red
		EXPORT BOOL IsNetworkMgmtRequest(void);   // EchoTest/Logon/Logoff
		EXPORT BOOL IsNetworkMgmtResponse(void);  // EchoTest/Logon/Logoff
		EXPORT BOOL IsControlCenterCommand(void); // Control Center Command
		EXPORT BOOL IsFinancialAdvice(void);	  // Forzado (0220,0221)
		EXPORT BOOL IsReversalOrAdviceRepeat(void);// Forzado (0221,0421)

#if ( _ISO8583_FORMAT_TO_USE_ & _ISO8583_FORMAT_FOR_REDLINK_ )
		// Transacciones en general de PAS/ULTMOV/AFJP
		EXPORT BOOL IsPASTransaction(void);      // PAS? 
		EXPORT BOOL IsULTMOVTransaction(void);   // ULTMOV? 
		EXPORT BOOL IsAFJPTransaction(void);     // AFJP? 
   		EXPORT BOOL IsSOATTransaction(void);     // SOAT? 

		// Transacciones especificas PAS/ULTMOV/AFJP
		EXPORT BOOL IsPASSuscription(void);     // Adhesion PAS?
		EXPORT BOOL IsPASSuscriptionRvrsl(void);// Reversa Adhesion PAS?	
		EXPORT BOOL IsPASInquiry(void);         // Consulta PAS?
		EXPORT BOOL IsPASPayment(void);         // Pago PAS?
		EXPORT BOOL IsPASPaymentResponse(void); // Respuesta a Pago PAS?
		EXPORT BOOL IsPASPaymentRvrsl(void);    // Reversa pago PAS?	
		// Pago PAS desde Host y Terminal POS?
		EXPORT BOOL IsPASPaymentFromPOSTerminal(void);
												
		
		EXPORT BOOL IsULTMOVInquiry(void);      // Consulta ULTMOV?

		// Transacciones especificas AFJP
		EXPORT BOOL IsAFJPInquiry(void);        // Consulta AFJP?
		EXPORT BOOL IsAFJPCuilInquiry(void);    // Consulta CUIL AFJP?
		EXPORT BOOL IsAFJPPayment(void);        // Pago AFJP?
		EXPORT BOOL IsAFJPMessageToInst(void);  // Mensaje a Inst. AFJP?


		// Transacciones especificas SOAT
		EXPORT BOOL IsSOATDuplicatedLogon(void);	// Logon duplicado SOAT?
		EXPORT BOOL IsSOATInstInquiry(void);		// Consulta Inst. SOAT?
		EXPORT BOOL IsSOATLogon(void);				// Logon SOAT?
		EXPORT BOOL IsSOATLogoff(void);				// Logoff SOAT?		
		EXPORT BOOL IsSOATLogonPassword(void);		// Logon de cambio de clave SOAT?
		EXPORT BOOL IsSOATGenericInquiry(void);		// Consulta Generica SOAT?
		EXPORT BOOL IsSOATGenericInsertion(void);	// Insercion Generica SOAT?
		EXPORT BOOL IsSOATGenericDeletion(void);	// Borrado Generico SOAT?
		EXPORT BOOL IsSOATGenericUpdate(void);		// Actualizacion Generica SOAT?
		EXPORT BOOL IsSOATGenericRequest(void);		// Pedido generico SOAT?
		EXPORT BOOL IsSOATSystemCmd(void);			// Comando de Sistema SOAT?
#endif  // ISO formats for RED LINK

		EXPORT BOOL IsATMTransaction(void);      // ATM? 
		EXPORT BOOL IsPOSTransaction(void);      // POS? 

		// Transacciones especificas ATM
		EXPORT BOOL IsATMBalanceInquiry(void);		// Consulta de saldos
		EXPORT BOOL IsATMTransfer(void);			// Transferencia
		EXPORT BOOL IsATMTransferCBU(void);			// Transferencia x CBU
		EXPORT BOOL IsATMTransferInterbank(void);	// Transferencia interbancaria
		EXPORT BOOL IsATMWithdrawal(void);			// Extraccion
		EXPORT BOOL IsATMDeposit(void);				// Deposito
		EXPORT BOOL IsATMFastCash(void);			// FastCash o compra
		EXPORT BOOL IsATMAccountVerification(void);	// Verificacion cuenta destino
		EXPORT BOOL IsATMPayment(void);				// Pagos
		EXPORT BOOL IsATMPaymentCashCheck(void);	// Pagos Efec.
		EXPORT BOOL IsATMBuying(void);				// Compras y relacionados
		EXPORT BOOL IsATMAutServicePayment(void);	// Pagos Automatico de Servs.
		EXPORT BOOL IsATMOthersMsgs(void);			// Otros mensajes
		EXPORT BOOL IsATMLastMovInquiry(void);		// Ult. Movimientos
		EXPORT BOOL IsATMPinChange(void);           // Cambio de PIN
		EXPORT BOOL IsATMAssignHomeBankUser(void);  // Asignar usuario HOME BANKING
		EXPORT BOOL IsATMExchangeRateInquiry(void); // Cons.Tipo Cambio

		EXPORT BOOL IsValidReversal(void);			// Reverso valido?

		///////////////////////////////////////////////////////
		// Expanded ISO fields , full size, fix format not variable
		EXPORT WORD ExpandFields( APPFIELDS &isoExternFields );
		///////////////////////////////////////////////////////


	protected:

		BOOL IsValidFromAccountType(int iOffset); // Tipo de Cta valido ?
		BOOL IsValidToAccountType(int iOffset); // Tipo de Cta valido ?
		
		//////////////////////////////
		// Expanded fields container
        APPFIELDS   isoFields;
		//////////////////////////////
        
    };


#endif // _APPMSG_H_


