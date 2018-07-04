//
// IT24 Sistemas S.A.
// Application ISO-8583 Messages as stated for ACI BASE24 RELEASE 4 and 6
//
// Libreria Propietaria de Mensajes ISO Aplicados
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.06.23      mdc     Base. PAS y ULTMOV. Base24 Release 4.
// (Alfa)       1998.07.02      mdc     Base. AFJP.
// (Beta)       1998.07.23      mdc     Extensiones SOAT. Logon.
// (Beta)       1998.07.27      mdc     Extensiones SOAT. Logoff.
// (Beta)       1998.07.31      mdc     Public-Protected-Private revisado
// (Beta)       1998.08.04      mdc     CheckExternParameters
// (Beta)       1998.08.11      mdc     SOAT: GenericRequest/Inquiry/Update/Delete/Insert
// (Beta)       1998.10.13      mdc     SOAT: GenericRequest/Inqueiry/Update/Delete/Insert Tarj
// (Beta)       1998.10.15      mdc     SOATSystemCmd
// (Beta)       1998.10.20      mdc     Mas transacciones SOAT
// (Beta)       1998.10.27      mdc     Reversos contemplados en trxs SOAT
// (Beta)       1998.11.05      mdc     TranCodes genericos de 6 digitos numericos
// (Beta)       1999.03.31      mdc     SOATLogonPassword();
// (Beta)       2000.02.15      mdc     CheckExternParameters() ampliada.
// (Beta)       2001.12.31      mdc     ATM Messages y BOOL IsControlCenterCommand(void).
// (Delta)      2005.07.18      mdc     Base24 Release 6. BANELCO (Argentina)
// (Delta)      2005.09.01      mdc     Varios codigos para TRANSFERENCIAS. Diferencia x PRODUCTO.
// (Delta)      2005.09.05      mdc     WORD APPMSG::ExpandFields(void)... and other improvements.
// (Delta)      2006.03.07      mdc     IGNORE FOR ACQUIRING ATM NETWORKS
//

// Header propio
#include <qusrinc/isoapp.h>
// Header formatos ISO8583 RED LINK
#include <qusrinc/isoredef.h>


// Header ANSI C/C++
#include <memory.h>
// Header Types ANSI
#include <ctype.h>
// Header STRING
#include <string.h>


// Archivo de inicializacion por deafult ISO8583
#define ISO8583_PROFILE_NAME "ISO8583.INI"


// Network Management Request
EXPORT BOOL APPMSG::IsNetworkMgmtRequest(void)
{	
	// Network request?
	return( ISO8583MSG::IsNetworkMgmtRequest() );
}

// Network Management Response
EXPORT BOOL APPMSG::IsNetworkMgmtResponse(void)
{
	// Network response?
	return( ISO8583MSG::IsNetworkMgmtResponse() );
}

// Control Center Command
EXPORT BOOL APPMSG::IsControlCenterCommand(void)
{
	// Not implemented
	return( FALSE );
}


// Verificacion externa de parametros
EXPORT BOOL APPMSG::CheckExternParameters(PBYTE pbFileName)
{
	// Temp. return value
	BOOL bReturnValue = FALSE;
	// Retorno de configuracion parametrica de bitmaps
	bReturnValue = (pbFileName == NULL)
		? GetAllBitmapsDescriptions((PSTR)ISO8583_PROFILE_NAME)
		: GetAllBitmapsDescriptions((PSTR)pbFileName);
	// Retorno de configuracion parametrica de campos 
	bReturnValue = (pbFileName == NULL)
		? GetFieldsDescription((PSTR)ISO8583_PROFILE_NAME)
		: GetFieldsDescription((PSTR)pbFileName);
	// Return value
	return (bReturnValue);
}

EXPORT BOOL APPMSG::IsATMTransaction(void)     // ATM? 
{
	// El Tipo de Trx esta alguno Ok?
	if(  IsATMBalanceInquiry()		||  // Consulta de saldos
		 IsATMTransfer()			||  // Transferencia
		 IsATMTransferCBU()			||  // Transferencia x CBU
		 IsATMTransferInterbank()	||  // Transferencia interbancaria
		 IsATMWithdrawal()			||  // Extraccion
		 IsATMDeposit()				||  // Deposito
		 IsATMFastCash()			||  // FastCash o compra
		 IsATMAccountVerification()	||  // Verificacion cuenta destino
		 IsATMPayment()				||  // Pagos
		 IsATMBuying()				||  // Compras y relacionados
		 IsATMAutServicePayment()	||  // Pagos Automatico de Servs.
		 IsATMOthersMsgs()			||  // Otros mensajes
		 IsATMLastMovInquiry()		||  // Ult. Movimientos
		 IsATMPinChange()           ||  // Cambio de PIN
		 IsATMAssignHomeBankUser()  ||  // Asigancion usuario HB
		 IsATMExchangeRateInquiry()		// Consulta tipo de cambio
		)
		return (TRUE);

	// Tipo Trx invalido
    return (FALSE);
}

BOOL APPMSG::IsValidFromAccountType(int iOffset) // Tipo de Cta valido ?
{
#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )
	return (	(memcmp((PBYTE)&chMsg[iOffset+2], B24_CHECKINGS_ACCOUNT_USD , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_SAVINGS_ACCOUNT       , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_SPECIAL_ACCOUNT_AFJP  , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_SAVINGS_ACCOUNT_BOND  , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_SAVINGS_ACCOUNT_USD   , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_CHECKINGS_ACCOUNT     , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_CHECKINGS_ACCOUNT_BOND, 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_CREDIT_CARD_ACCOUNT   , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_CREDIT_LINE_ACCOUNT   , 2) == 0) ||
				// Default efectivo
				(memcmp((PBYTE)&chMsg[iOffset+2], "00"                      , 2) == 0));
#else
	return (	(memcmp((PBYTE)&chMsg[iOffset+2], B24_CHECKINGS_ACCOUNT_USD , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_SAVINGS_ACCOUNT       , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_SAVINGS_ACCOUNT_USD   , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_CHECKINGS_ACCOUNT     , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_CREDIT_CARD_ACCOUNT   , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+2], B24_CREDIT_ACCOUNT		, 2) == 0) ||
                (memcmp((PBYTE)&chMsg[iOffset+2], B24_TRANSFER_ACCOUNT_CBU  , 2) == 0) ||
				// Default efectivo
				(memcmp((PBYTE)&chMsg[iOffset+2], "00"                      , 2) == 0));
#endif

}

BOOL APPMSG::IsValidToAccountType(int iOffset)	// Tipo de Cta valido ?
{
#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )
	return (	(memcmp((PBYTE)&chMsg[iOffset+4], B24_CHECKINGS_ACCOUNT_USD , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_SAVINGS_ACCOUNT       , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_SPECIAL_ACCOUNT_AFJP  , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_SAVINGS_ACCOUNT_BOND  , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_SAVINGS_ACCOUNT_USD   , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_CHECKINGS_ACCOUNT     , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_CHECKINGS_ACCOUNT_BOND, 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_CREDIT_CARD_ACCOUNT   , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_CREDIT_LINE_ACCOUNT   , 2) == 0) ||
				// "02" : Caso especial RED LINK para operaciones en moneda US$, DOLARES
				// No es estandard, como ya es constumbre, y viola el estandard
				(memcmp((PBYTE)&chMsg[iOffset+4], "02"                      , 2) == 0) ||
				// Default efectivo
				(memcmp((PBYTE)&chMsg[iOffset+4], "00"                      , 2) == 0));
#else
	return (	(memcmp((PBYTE)&chMsg[iOffset+4], B24_CHECKINGS_ACCOUNT_USD , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_SAVINGS_ACCOUNT       , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_SAVINGS_ACCOUNT_USD   , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_CHECKINGS_ACCOUNT     , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_CREDIT_CARD_ACCOUNT   , 2) == 0) ||
				(memcmp((PBYTE)&chMsg[iOffset+4], B24_CREDIT_ACCOUNT		, 2) == 0) ||
                (memcmp((PBYTE)&chMsg[iOffset+4], B24_TRANSFER_ACCOUNT_CBU  , 2) == 0) ||
				// Default efectivo
				(memcmp((PBYTE)&chMsg[iOffset+4], "00"                      , 2) == 0));
#endif

}

EXPORT BOOL APPMSG::IsATMWithdrawal(void)		// Extraccion
{
	// BASE24-product. Values are :
	// 00-GENERIC Base/Propietary
	// 01-B24 ATM
	// 02-B24 POS
	// 03-B24 Teller
	// 08-From Host Maintenance
	// 11-B24 EMS
	// 12-B24 EFTPOS	
	char chPRODUCT = 0x00;
	int  iOffset   = 0;

	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	iOffset = FieldOffset(3);

	///////////////////////////////////////////////////////////
	// PRECONDITION : CHECK THE PRODUCT CODE : 
	if( isdigit(chMsg[3]) && isdigit( chMsg[4]) )
	{
		// 1ST. OF ALL : CHECK THE PRODUCT CODE : ATM,POS,etc.
		chPRODUCT = ((chMsg[3]-'0') * 10) + (chMsg[4]-'0') ;
		if( _ISO8583_FORMAT_FOR_ATM_ != chPRODUCT   )
			return FALSE;
	}
	else
		chPRODUCT = 0x00;
	////////////////////////////////////////////////////////

	// Message-Type-Identifier [12..15=4]    	
	return( ((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0)  ||
			 (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)		   ||
			(memcmp((PBYTE)&chMsg[12] , CISO_MSGTYPE_REVERSAL_REP, 4) == 0)     ||
			(memcmp((PBYTE)&chMsg[12] , CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)    ||
			(memcmp((PBYTE)&chMsg[12] , CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   ( (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_WITHDRAWAL_, 2) == 0
			  ||
			  memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_WITHDRAWAL_FRGN_CURRENCY_, 2) == 0)
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}
EXPORT BOOL APPMSG::IsATMBalanceInquiry(void)	// Consulta de saldos
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   ( (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_BALANCE_INQUIRY_    , 2) == 0 
			  ||
		      memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_BALANCE_INQUIRY_POS_, 2) == 0)
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}
EXPORT BOOL APPMSG::IsATMTransfer(void)			// Transferencia en cuentas relacionadas
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)  ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   ( ( memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_TRANSFER_, 2) == 0
				|| 
			   memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_TRANSFER_FRGN_CURRENCY_, 2) == 0) 
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}

EXPORT BOOL APPMSG::IsATMTransferInterbank(void)			// Transferencia interbancaria
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)  ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   ( ( (memcmp((PBYTE)&chMsg[iOffset] , _B24_OPCODE_DEBIT_ORDER_XFER_ , 2) == 0) 
				||
				(memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_CREDIT_ORDER_XFER_, 2) == 0) 
				||
				(memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_QUERY_ORDER_XFER_ , 2) == 0) )
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}

EXPORT BOOL APPMSG::IsATMTransferCBU(void)			// Transferencia minorista x CBU
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)  ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   ( ( memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_TRANSFER_CBU_, 2) == 0 
				||
		       memcmp((PBYTE)&chMsg[iOffset],_B24_OPCODE_TRANSFER_CBU_ACH_, 2) == 0 ) 
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}

EXPORT BOOL APPMSG::IsATMAccountVerification(void)			// Verificacion cuenta destino x transferencia
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)  ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   ( (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_VERIF_ACCT_XFER_, 2) == 0) 
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}

EXPORT BOOL APPMSG::IsATMDeposit(void)			// Deposito
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)  ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   ( ( (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_DEPOSIT_    , 2) == 0) )
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}

EXPORT BOOL APPMSG::IsATMFastCash(void)			// Fast Cash o compra
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)  ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   ( (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_WITHDRAWAL_FASTCASH_, 2) == 0)		       
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}


EXPORT BOOL APPMSG::IsATMPayment(void)			// Pagos
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);

	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)  ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   ( ((memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_PAYMENT_TELEPHONY_SRVCS_, 2) == 0) 
				||
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_PAYMENT_W_DEBIT_, 2) == 0) 
				||
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_PAYMENT_W_STATEMENT_, 2) == 0) 
				||
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_PAYMENT_W_CASH_ , 2) == 0) 
				||
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_PAYMENT_W_DEBIT_ORDER_, 2) == 0) 
			  )
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}

EXPORT BOOL APPMSG::IsATMPaymentCashCheck(void)			// Pagos  Efec.
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)  ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   ( ((memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_PAYMENT_W_CASH_, 2) == 0) 
				||
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_PAYMENT_W_DEBIT_ORDER_, 2) == 0) )
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}


EXPORT BOOL APPMSG::IsATMBuying(void)			// Compras y relacionadas
{
	// BASE24-product. Values are :
	// 00-GENERIC Base/Propietary
	// 01-B24 ATM
	// 02-B24 POS
	// 03-B24 Teller
	// 08-From Host Maintenance
	// 11-B24 EMS
	// 12-B24 EFTPOS	
	char chPRODUCT = 0x00;
	int  iOffset   = 0;

	///////////////////////////////////////////////////////////
	// PRECONDITION : CHECK THE PRODUCT CODE : 
	if( isdigit(chMsg[3]) && isdigit( chMsg[4]) )
	{
		// 1ST. OF ALL : CHECK THE PRODUCT CODE : ATM,POS,etc.
		chPRODUCT = ((chMsg[3]-'0') * 10) + (chMsg[4]-'0') ;
		if( _ISO8583_FORMAT_FOR_POS_ != chPRODUCT   )
			// IGNORE FOR ACQUIRING ATM NETWORKS
			if( _ISO8583_FORMAT_FOR_ATM_ != chPRODUCT   )
				return FALSE;
	}
	else
		chPRODUCT = 0x00;
	////////////////////////////////////////////////////////

	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	// Offset of the field #3
	iOffset = FieldOffset(3);

	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)  ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   ( ((memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_BUYING_W_ACCT_DEBIT_, 2) == 0) 
				||
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL_, 2) == 0) 
				||			  
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_BUYING_DEVOLUTION_, 2) == 0) 
				||
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_BUYING_DEVOLUTION_CANCEL_, 2) == 0) 
				||
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_BUYING_CASH_BACK_, 2) == 0) 
				||
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_BUYING_CASH_BACK_CANCEL_, 2) == 0) 
				||
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_BUYING_PREAUTH_, 2) == 0) 
				||
			  (memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_BUYING_PREAUTH_CANCEL_, 2) == 0) 
		     )
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}

EXPORT BOOL APPMSG::IsATMAutServicePayment(void)			// Pagos automaticos serv.
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)  ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
		   &&		  
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   // (COMPARE FIRST CHARACTER (RANGE "80".."89"
		   ( ((memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_AUT_PAYMENT_SRVCS_, 1) == 0)  			  
		     )
			  && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}

EXPORT BOOL APPMSG::IsATMOthersMsgs(void)			// Otros mensajes
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0)       ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0)   ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV, 4) == 0)  ||
			(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_ADV_REP, 4) == 0) )
			&&
		   // UserTrxCode [xxyyzz=6] : xx=trx_code,yy=from_acct_type,zz=to_acct_type
		   // COMPARE FIRST CHARACTER, RANGE  "90" .. "99"
		   ( ((memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_MSG_TO_INSTITUTION_, 1) == 0) ) 
		      && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
		   )
		  );
}

EXPORT BOOL APPMSG::IsValidReversal(void) // Reverso ?
{
	return ( IsValid420() || IsValid421() );  
}


EXPORT BOOL APPMSG::IsATMLastMovInquiry(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
	       ((memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_QUERY_LAST_MOVEMENTS_, 2) == 0)    
		      && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
			)
		  );
}

EXPORT BOOL APPMSG::IsATMPinChange(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
	       ((memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_PIN_CHANGE_, 2) == 0)    
		      && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
			)
		  );
}

EXPORT BOOL APPMSG::IsATMAssignHomeBankUser(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
	       ((memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_HBANK_USER_, 2) == 0)    
		      && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
			)
		  );
}

EXPORT BOOL APPMSG::IsATMExchangeRateInquiry(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
	       ((memcmp((PBYTE)&chMsg[iOffset], _B24_OPCODE_INQUIRY_EXCH_RATE_, 2) == 0)    
		      && IsValidFromAccountType( iOffset )
			  && IsValidToAccountType( iOffset ) 
			)
		  );
}

EXPORT BOOL APPMSG::IsFinancialAdvice(void)	  // Forzado (0220,0221)
{
	return ( IsValid220() || IsValid221() );
}

EXPORT BOOL APPMSG::IsReversalOrAdviceRepeat(void)	  // Repeticion de Forzado-Reverso (0221,0421)
{
	return ( IsValid221() || IsValid421() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Expand variable fields to fix-size full format
WORD APPMSG::ExpandFields( APPFIELDS &isoExternFields)
{
    WORD    wLen     = 0;
    PBYTE   pbBuffer = NULL;
	/*****************************/
	char	achHeader[16] = {0x00}  ;
	/*****************************/

    // Inicializar los campos extractados del mensaje ISO
    memset( &isoFields, ' ', sizeof isoFields);
    // Senal de carga a falso
    isoFields.bLoaded = is_false;


    // Extract andExpand each useful field from the message
    // From the SECONDARY-BITMAP, field #1 up to #127
    for(short iIndex=2; iIndex < 128; iIndex++)
    {
        switch(iIndex)
        {        
		case 2 : pbBuffer = (PBYTE)&isoFields.field2; wLen = sizeof(isoFields.field2); break; /* 2005.08.24,mdc */
        case 3 : pbBuffer = (PBYTE)&isoFields.field3; wLen = sizeof(isoFields.field3); break;
        case 4 : pbBuffer = (PBYTE)&isoFields.field4; wLen = sizeof(isoFields.field4); break;
        case 5 : pbBuffer = (PBYTE)&isoFields.field5; wLen = sizeof(isoFields.field5); break;
        case 6 : pbBuffer = (PBYTE)&isoFields.field6; wLen = sizeof(isoFields.field6); break;
        case 7 : pbBuffer = (PBYTE)&isoFields.field7; wLen = sizeof(isoFields.field7); break;
        case 8 : pbBuffer = (PBYTE)&isoFields.field8; wLen = sizeof(isoFields.field8); break;
        case 9 : pbBuffer = (PBYTE)&isoFields.field9; wLen = sizeof(isoFields.field9); break;
        case 10: pbBuffer = (PBYTE)&isoFields.field10; wLen = sizeof(isoFields.field10); break;
        case 11: pbBuffer = (PBYTE)&isoFields.field11; wLen = sizeof(isoFields.field11); break;
        case 12: pbBuffer = (PBYTE)&isoFields.field12; wLen = sizeof(isoFields.field12); break;
        case 13: pbBuffer = (PBYTE)&isoFields.field13; wLen = sizeof(isoFields.field13); break;
        case 14: pbBuffer = (PBYTE)&isoFields.field14; wLen = sizeof(isoFields.field14); break;
        case 15: pbBuffer = (PBYTE)&isoFields.field15; wLen = sizeof(isoFields.field15); break;
        case 16: pbBuffer = (PBYTE)&isoFields.field16; wLen = sizeof(isoFields.field16); break;
        case 17: pbBuffer = (PBYTE)&isoFields.field17; wLen = sizeof(isoFields.field17); break;
        case 18: pbBuffer = (PBYTE)&isoFields.field18; wLen = sizeof(isoFields.field18); break;
        case 19: pbBuffer = (PBYTE)&isoFields.field19; wLen = sizeof(isoFields.field19); break;
        case 20: pbBuffer = (PBYTE)&isoFields.field20; wLen = sizeof(isoFields.field20); break;
        case 21: pbBuffer = (PBYTE)&isoFields.field21; wLen = sizeof(isoFields.field21); break;
        case 22: pbBuffer = (PBYTE)&isoFields.field22; wLen = sizeof(isoFields.field22); break;
        case 23: pbBuffer = (PBYTE)&isoFields.field23; wLen = sizeof(isoFields.field23); break;
        case 24: pbBuffer = (PBYTE)&isoFields.field24; wLen = sizeof(isoFields.field24); break;
        case 25: pbBuffer = (PBYTE)&isoFields.field25; wLen = sizeof(isoFields.field25); break;
        case 26: pbBuffer = (PBYTE)&isoFields.field26; wLen = sizeof(isoFields.field26); break;
        case 27: pbBuffer = (PBYTE)&isoFields.field27; wLen = sizeof(isoFields.field27); break;
        case 28: pbBuffer = (PBYTE)&isoFields.field28; wLen = sizeof(isoFields.field28); break;
        case 29: pbBuffer = (PBYTE)&isoFields.field29; wLen = sizeof(isoFields.field29); break;
        case 30: pbBuffer = (PBYTE)&isoFields.field30; wLen = sizeof(isoFields.field30); break;
        case 31: pbBuffer = (PBYTE)&isoFields.field31; wLen = sizeof(isoFields.field31); break;
        case 32: pbBuffer = (PBYTE)&isoFields.field32; wLen = sizeof(isoFields.field32); break;
        case 33: pbBuffer = (PBYTE)&isoFields.field33; wLen = sizeof(isoFields.field33); break;
        case 34: pbBuffer = (PBYTE)&isoFields.field34; wLen = sizeof(isoFields.field34); break;
        case 35: pbBuffer = (PBYTE)&isoFields.field35; wLen = sizeof(isoFields.field35); break;
        case 36: pbBuffer = (PBYTE)&isoFields.field36; wLen = sizeof(isoFields.field36); break;
        case 37: pbBuffer = (PBYTE)&isoFields.field37; wLen = sizeof(isoFields.field37); break;
        case 38: pbBuffer = (PBYTE)&isoFields.field38; wLen = sizeof(isoFields.field38); break;
        case 39: pbBuffer = (PBYTE)&isoFields.field39; wLen = sizeof(isoFields.field39); break;
        case 40: pbBuffer = (PBYTE)&isoFields.field40; wLen = sizeof(isoFields.field40); break;
        case 41: pbBuffer = (PBYTE)&isoFields.field41; wLen = sizeof(isoFields.field41); break;
        case 42: pbBuffer = (PBYTE)&isoFields.field42; wLen = sizeof(isoFields.field42); break;
        case 43: pbBuffer = (PBYTE)&isoFields.field43; wLen = sizeof(isoFields.field43); break;
        case 44: pbBuffer = (PBYTE)&isoFields.field44; wLen = sizeof(isoFields.field44); break;
        case 45: pbBuffer = (PBYTE)&isoFields.field45; wLen = sizeof(isoFields.field45); break;
        case 46: pbBuffer = (PBYTE)&isoFields.field46; wLen = sizeof(isoFields.field46); break;
        case 47: pbBuffer = (PBYTE)&isoFields.field47; wLen = sizeof(isoFields.field47); break;
        case 48: pbBuffer = (PBYTE)&isoFields.field48; wLen = sizeof(isoFields.field48); break;
        case 49: pbBuffer = (PBYTE)&isoFields.field49; wLen = sizeof(isoFields.field49); break;
        case 50: pbBuffer = (PBYTE)&isoFields.field50; wLen = sizeof(isoFields.field50); break;
        case 51: pbBuffer = (PBYTE)&isoFields.field51; wLen = sizeof(isoFields.field51); break;
        case 52: pbBuffer = (PBYTE)&isoFields.field52; wLen = sizeof(isoFields.field52); break;
        case 53: pbBuffer = (PBYTE)&isoFields.field53; wLen = sizeof(isoFields.field53); break;
        case 54: pbBuffer = (PBYTE)&isoFields.field54; wLen = sizeof(isoFields.field54); break;
        case 55: pbBuffer = (PBYTE)&isoFields.field55; wLen = sizeof(isoFields.field55); break;
        case 56: pbBuffer = (PBYTE)&isoFields.field56; wLen = sizeof(isoFields.field56); break;
        case 57: pbBuffer = (PBYTE)&isoFields.field57; wLen = sizeof(isoFields.field57); break;
        case 58: pbBuffer = (PBYTE)&isoFields.field58; wLen = sizeof(isoFields.field58); break;
        case 59: pbBuffer = (PBYTE)&isoFields.field59; wLen = sizeof(isoFields.field59); break;
        case 60: pbBuffer = (PBYTE)&isoFields.field60; wLen = sizeof(isoFields.field60); break;
        case 61: pbBuffer = (PBYTE)&isoFields.field61; wLen = sizeof(isoFields.field61); break;
        case 62: pbBuffer = (PBYTE)&isoFields.field62; wLen = sizeof(isoFields.field62); break;
        case 63: pbBuffer = (PBYTE)&isoFields.field63; wLen = sizeof(isoFields.field63); break;
        case 64: pbBuffer = (PBYTE)&isoFields.field64; wLen = sizeof(isoFields.field64); break;
        case 90: pbBuffer = (PBYTE)&isoFields.field90; wLen = sizeof(isoFields.field90); break;
        case 95: pbBuffer = (PBYTE)&isoFields.field95; wLen = sizeof(isoFields.field95); break;
        case 99: pbBuffer = (PBYTE)&isoFields.field99; wLen = sizeof(isoFields.field99); break;
        case 100: pbBuffer = (PBYTE)&isoFields.field100; wLen = sizeof(isoFields.field100); break;
        case 102: pbBuffer = (PBYTE)&isoFields.field102; wLen = sizeof(isoFields.field102); break;
        case 103: pbBuffer = (PBYTE)&isoFields.field103; wLen = sizeof(isoFields.field103); break;
        case 120: pbBuffer = (PBYTE)&isoFields.field120; wLen = sizeof(isoFields.field120); break;
        case 122: pbBuffer = (PBYTE)&isoFields.field122; wLen = sizeof(isoFields.field122); break;
        case 123: pbBuffer = (PBYTE)&isoFields.field123; wLen = sizeof(isoFields.field123); break;
	    case 124: pbBuffer = (PBYTE)&isoFields.field124; wLen = sizeof(isoFields.field124); break;
        case 125: pbBuffer = (PBYTE)&isoFields.field125; wLen = sizeof(isoFields.field125); break;
        case 126: pbBuffer = (PBYTE)&isoFields.field126; wLen = sizeof(isoFields.field126); break;
        case 127: pbBuffer = (PBYTE)&isoFields.field127; wLen = sizeof(isoFields.field127); break;
        default: wLen = 0; pbBuffer = NULL; break;
        }
        
        if(!GetField( iIndex, &wLen, pbBuffer, wLen ))
        {
			/* Oooooppssss must do something ? */
        }
    }
    // Flag to TRUE
    isoFields.bLoaded = is_true;

	/**************************************************************************/
	// ISO8583 PRODUCT INDICATOR	
	isoFields.chPRODUCT = _ISO8583_FORMAT_FOR_ATM_;  // PRODUCT ATM BY DEFAULT
	GetHeader( achHeader, sizeof(achHeader) );
	// SAVE PRODUCT IN BINARY FORMAT, CONVERT TO DECIMAL
	if( isdigit(achHeader[3]) && isdigit( achHeader[4]) )
		isoFields.chPRODUCT = ((achHeader[3]-'0') * 10) + (achHeader[4]-'0') ;
	/**************************************************************************/
	// Values are :
	// 00-GENERIC Base/Propietary
	// 01-B24 ATM
	// 02-B24 POS
	// 03-B24 Teller
	// 08-From Host Maintenance
	// 11-B24 EMS
	// 12-B24 EFTPOS	
	/**************************************************************************/

	/* External Copy */
	isoExternFields = isoFields;

    // Ok, APROBADO por default
    return 0;
}

EXPORT BOOL APPMSG::IsPOSTransaction(void)     // ATM? 
{
	// BASE24-product. Values are :
	// 00-GENERIC Base/Propietary
	// 01-B24 ATM
	// 02-B24 POS
	// 03-B24 Teller
	// 08-From Host Maintenance
	// 11-B24 EMS
	// 12-B24 EFTPOS	
	char chPRODUCT = 0x00;

	///////////////////////////////////////////////////////////
	// PRECONDITION : CHECK THE PRODUCT CODE : 
	if( isdigit(chMsg[3]) && isdigit( chMsg[4]) )
	{
		// 1ST. OF ALL : CHECK THE PRODUCT CODE : ATM,POS,etc.
		chPRODUCT = ((chMsg[3]-'0') * 10) + (chMsg[4]-'0') ;
		if( _ISO8583_FORMAT_FOR_POS_ != chPRODUCT   )
			return FALSE;
	};
	////////////////////////////////////////////////////////

	// El Tipo de Trx esta alguno Ok?
	if(  IsATMBalanceInquiry()	||  // Consulta de saldos
		 IsATMBuying()			)   // Compras y relacionados
		return (TRUE);

	// Tipo Trx invalido
    return (FALSE);
}

//////////////////////////////////////////////////////
