//
// IT24 Sistemas S.A.
// Application ISO-8583 Messages as stated for ACI BASE24 RELEASE 4 and 6
//
// Libreria Propietaria de Mensajes ISO Aplicados
//
// Tarea        Fecha           Autor   Observaciones
// (Delta)      2005.07.18      mdc     Base24 Release 6.
//

// Header propio
#include <qusrinc/isoapp.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>


///////////////////////////////////////////////////////////////
#if ( _ISO8583_FORMAT_TO_USE_ & _ISO8583_FORMAT_FOR_REDLINK_ )
/////////////////////////////////////////////////////////////////

// PAS?
EXPORT BOOL APPMSG::IsPASTransaction(void)
{
	// El Tipo de Trx esta alguno Ok?
	if( IsPASSuscription()      ||    // Adhesion
		IsPASSuscriptionRvrsl() ||    // Reversa Adhesion
	    IsPASInquiry()          ||    // Consulta
		IsPASPayment()          ||    // Pago		
		IsPASPaymentResponse()  ||    // Rta. a Pago		
		IsPASPaymentRvrsl() )         // Reversa Pago
		return (TRUE);

	// Tipo Trx invalido
    return (FALSE);
}

// ULTMOV?
EXPORT BOOL APPMSG::IsULTMOVTransaction(void)
{
	// El Tipo de Trx esta alguno Ok?
	if( IsULTMOVInquiry() )  // Consulta
		return (TRUE);

	// Tipo Trx invalido
    return (FALSE);
}

// AFJP?
EXPORT BOOL APPMSG::IsAFJPTransaction(void)
{
	// El Tipo de Trx esta alguno Ok?
	if( IsAFJPInquiry()         || // Consulta
		IsAFJPCuilInquiry()     || // Consulta CUIL
		IsAFJPPayment()         || // Pago
		IsAFJPMessageToInst() )    // Mensaje a Institucion
		return (TRUE);

	// Tipo Trx invalido
    return (FALSE);
}

// SOAT?
EXPORT BOOL APPMSG::IsSOATTransaction(void)
{
	// El Tipo de Trx esta alguno Ok?
	if( IsSOATDuplicatedLogon()		||  // Logon de Usuario en Terminal duplicado?		
		IsSOATLogon()				||	// Logon de Usuario en Terminal?
		IsSOATLogoff()				||	// Logoff de Usuario en Terminal?		
		IsSOATGenericInquiry()		||  // Consulta?
		IsSOATGenericInsertion()	||	// Insercion?
		IsSOATGenericDeletion()		||	// Borrado?
		IsSOATGenericUpdate()		||	// Modificacion?
		IsSOATGenericRequest()		||	// Requerimiento-Pedido generico?
		IsSOATSystemCmd()				// Comando del Sistema
		)				
		// Ok
		return (TRUE);

	// Tipo Trx invalido
    return (FALSE);
}



// Es valida la adhesion/suscripcion?
EXPORT BOOL APPMSG::IsPASSuscription(void)
{	
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
	       ((memcmp((PBYTE)&chMsg[iOffset], "890000", 6) == 0) ||
		    (memcmp((PBYTE)&chMsg[iOffset], "800000", 6) == 0) )
		  );
}
// Es valido el reverso de adhesion?
EXPORT BOOL APPMSG::IsPASSuscriptionRvrsl(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0) 
		     ||
		    (memcmp((PBYTE)&chMsg[12], "0421", 4) == 0))
		   &&		  
		   // UserTrxCode [x..y=6]
		   (memcmp((PBYTE)&chMsg[iOffset], "800000", 6) == 0) // Adhesion
		  );
}

// Es valida la consulta?
EXPORT BOOL APPMSG::IsPASInquiry(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
	       (memcmp((PBYTE)&chMsg[iOffset], "830000", 6) == 0) 
		  );
}
// Es valido el pago?
EXPORT BOOL APPMSG::IsPASPayment(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], "0220", 4) == 0) 
		    ||
		    (memcmp((PBYTE)&chMsg[12], "0221", 4) == 0))
		   &&		  
		   // UserTrxCode [x..y=6]
		   ( ((memcmp((PBYTE)&chMsg[iOffset], "881000", 6) == 0) ||   // Pago s/fact
		      (memcmp((PBYTE)&chMsg[iOffset], "882000", 6) == 0) )
			  ||
			 (memcmp((PBYTE)&chMsg[iOffset], "88", 2) == 0)           // Pago s/usu
			  ||
	         ((memcmp((PBYTE)&chMsg[iOffset], "81", 2) == 0) ||       // Default
		      (memcmp((PBYTE)&chMsg[iOffset], "86", 2) == 0) )
		   )
		  );
}

// Es valido el reverso de pago?
EXPORT BOOL APPMSG::IsPASPaymentRvrsl(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0) 
		     ||
		    (memcmp((PBYTE)&chMsg[12], "0421", 4) == 0))
		   &&		  
		   // UserTrxCode [x..y=6]
		   ( ((memcmp((PBYTE)&chMsg[iOffset], "881000", 6) == 0) ||   // Pago s/fact
		      (memcmp((PBYTE)&chMsg[iOffset], "882000", 6) == 0) )
			  ||
			 (memcmp((PBYTE)&chMsg[iOffset], "88", 2) == 0)           // Pago s/usu
			  ||
	         ((memcmp((PBYTE)&chMsg[iOffset], "81", 2) == 0) ||       // Default
		      (memcmp((PBYTE)&chMsg[iOffset], "86", 2) == 0) )
		   )
		  );
}

// Es valida la respuesta al pago?
EXPORT BOOL APPMSG::IsPASPaymentResponse(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], "0230", 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
		   ( ((memcmp((PBYTE)&chMsg[iOffset], "881000", 6) == 0) ||   // Pago s/fact
		      (memcmp((PBYTE)&chMsg[iOffset], "882000", 6) == 0) )
			  ||
			 (memcmp((PBYTE)&chMsg[iOffset], "88", 2) == 0)           // Pago s/usu
			  ||
	         ((memcmp((PBYTE)&chMsg[iOffset], "81", 2) == 0) ||       // Default
		      (memcmp((PBYTE)&chMsg[iOffset], "86", 2) == 0) )
		   )
		  );
}

// Es valido el pago desde un host y terminal POS Teller?
EXPORT BOOL APPMSG::IsPASPaymentFromPOSTerminal(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);
	// Track2 P-35
	if(!IsValidField(35)) 
		return (FALSE);

	int iOffTrxCde = FieldOffset(3) +FieldType(3) ,
	    iOffTrk2   = FieldOffset(35)+FieldType(35);
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], (PBYTE)"0220", 4) == 0) 
		    ||
		    (memcmp((PBYTE)&chMsg[12], (PBYTE)"0221", 4) == 0))
		   &&		  
		    (memcmp((PBYTE)&chMsg[iOffTrxCde], (PBYTE)"86", 2) == 0)
		   &&
			(memcmp((PBYTE)&chMsg[iOffTrk2], (PBYTE)"00", 2) == 0)
		  );
}

// Es valida la consulta?
EXPORT BOOL APPMSG::IsULTMOVInquiry(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
	       ((memcmp((PBYTE)&chMsg[iOffset], "942000", 6) == 0) || //CtaCte$
		    (memcmp((PBYTE)&chMsg[iOffset], "940700", 6) == 0) || //CtaCteUS$
			(memcmp((PBYTE)&chMsg[iOffset], "941000", 6) == 0) || //CajAh$
			(memcmp((PBYTE)&chMsg[iOffset], "941500", 6) == 0) || //CajAhUS$
			(memcmp((PBYTE)&chMsg[iOffset], "941400", 6) == 0) || //CajAhBono
			(memcmp((PBYTE)&chMsg[iOffset], "943000", 6) == 0))   //TCred
		  );
}

// Es valida la consulta?
EXPORT BOOL APPMSG::IsAFJPInquiry(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
	       (memcmp((PBYTE)&chMsg[iOffset], "941300", 6) == 0) 
		  );
}

// Es valida la consulta de CUIL como Id?
EXPORT BOOL APPMSG::IsAFJPCuilInquiry(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
	       ((memcmp((PBYTE)&chMsg[iOffset], "881000", 6) == 0) ||
		    (memcmp((PBYTE)&chMsg[iOffset], "882000", 6) == 0))
		  );
}

// Es valido el pago?
EXPORT BOOL APPMSG::IsAFJPPayment(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
	       (memcmp((PBYTE)&chMsg[iOffset], "210013", 6) == 0) 
		  );
}

// Es valido el mensaje a la institucion?
EXPORT BOOL APPMSG::IsAFJPMessageToInst(void)
{
	// UserTrxCode P-3
	if(!IsValidField(3)) 
		return (FALSE);

	int iOffset = FieldOffset(3);
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // UserTrxCode [x..y=6]
	       (memcmp((PBYTE)&chMsg[iOffset], "911300", 6) == 0) 
		  );
}


// Es valido el logon?
EXPORT BOOL APPMSG::IsSOATLogon(void)
{
	// SOAT-TrxCode S-126
	if(!IsValidField(126)) 
		return (FALSE);

	// Los codigos de transaccion de usuario en el sistema SOAT no
	// respetan el estandard ISO-8583 y vienen informados en un
	// mensaje propietario en el campo 126...

	// SOATUserTrxCode [x..y=6]
	int iOffset = FieldOffset(126),
		iType   = FieldType(126)  ;
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // SOATTrxCode [6]
	       (memcmp((PBYTE)&chMsg[iOffset+iType], "012000", 6) == 0) 
		  );
}

// Es valido el logon duplicado?
EXPORT BOOL APPMSG::IsSOATDuplicatedLogon(void)
{
	// SOAT-TrxCode S-126
	if(!IsValidField(126)) 
		return (FALSE);

	// Los codigos de transaccion de usuario en el sistema SOAT no
	// respetan el estandard ISO-8583 y vienen informados en un
	// mensaje propietario en el campo 126...

	// SOATUserTrxCode [x..y=6]
	int iOffset = FieldOffset(126),
		iType   = FieldType(126)  ;
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // SOATTrxCode [6]
	       (memcmp((PBYTE)&chMsg[iOffset+iType], "012001", 6) == 0) 
		  );
}

// Es valido el logoff?
EXPORT BOOL APPMSG::IsSOATLogoff(void)
{
	// SOAT-TrxCode S-126
	if(!IsValidField(126)) 
		return (FALSE);

	// Los codigos de transaccion de usuario en el sistema SOAT no
	// respetan el estandard ISO-8583 y vienen informados en un
	// mensaje propietario en el campo 126...

	// SOATUserTrxCode [x..y=6]
	int iOffset = FieldOffset(126),
		iType   = FieldType(126)  ;
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // SOATTrxCode [6]
	       (memcmp((PBYTE)&chMsg[iOffset+iType], "012002", 6) == 0) 
		  );
}


EXPORT BOOL APPMSG::IsSOATGenericInquiry(void)	   // Consulta      Generica SOAT?
{
	// SOAT-TrxCode S-126
	if(!IsValidField(126)) 
		return (FALSE);

	// Los codigos de transaccion de usuario en el sistema SOAT no
	// respetan el estandard ISO-8583 y vienen informados en un
	// mensaje propietario en el campo 126...

	// SOATUserTrxCode [x..y=6]
	int iOffset = FieldOffset(126),
		iType   = FieldType(126)  ;
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // SOATTrxCode [2+2+2]
	       (memcmp((PBYTE)&chMsg[iOffset+iType], "05", 2) == 0)
		   &&
	       (isdigit(chMsg[iOffset+iType+2]) && isdigit(chMsg[iOffset+iType+2+1]))
		   &&
	       (memcmp((PBYTE)&chMsg[iOffset+iType+4], "00", 2) == 0)		   		   
		  );}
EXPORT BOOL APPMSG::IsSOATGenericInsertion(void)  // Insercion     Generica SOAT?
{
	// SOAT-TrxCode S-126
	if(!IsValidField(126)) 
		return (FALSE);

	// Los codigos de transaccion de usuario en el sistema SOAT no
	// respetan el estandard ISO-8583 y vienen informados en un
	// mensaje propietario en el campo 126...

	// SOATUserTrxCode [x..y=6]
	int iOffset = FieldOffset(126),
		iType   = FieldType(126)  ;
	// Message-Type-Identifier [12..15=4]    	
	return(( (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			 (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0) )
		   &&		  
		   // SOATTrxCode [2+2+2]
	       (memcmp((PBYTE)&chMsg[iOffset+iType], "02", 2) == 0)
		   &&
	       (isdigit(chMsg[iOffset+iType+2]) && isdigit(chMsg[iOffset+iType+2+1]))
		   &&
	       (memcmp((PBYTE)&chMsg[iOffset+iType+4], "00", 2) == 0)		   		   
		  );
}
EXPORT BOOL APPMSG::IsSOATGenericDeletion(void)   // Borrado       Generico SOAT?
{
	// SOAT-TrxCode S-126
	if(!IsValidField(126)) 
		return (FALSE);

	// Los codigos de transaccion de usuario en el sistema SOAT no
	// respetan el estandard ISO-8583 y vienen informados en un
	// mensaje propietario en el campo 126...

	// SOATUserTrxCode [x..y=6]
	int iOffset = FieldOffset(126),
		iType   = FieldType(126)  ;
	// Message-Type-Identifier [12..15=4]    	
	return(( (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
		     (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0) )
		   &&		  
		   // SOATTrxCode [2+2+2]
	       (memcmp((PBYTE)&chMsg[iOffset+iType], "04", 2) == 0)
		   &&
	       (isdigit(chMsg[iOffset+iType+2]) && isdigit(chMsg[iOffset+iType+2+1]))
		   &&
	       (memcmp((PBYTE)&chMsg[iOffset+iType+4], "00", 2) == 0)		   		   
		  );
}
EXPORT BOOL APPMSG::IsSOATGenericUpdate(void)     // Actualizacion Generica SOAT?
{
	// SOAT-TrxCode S-126
	if(!IsValidField(126)) 
		return (FALSE);

	// Los codigos de transaccion de usuario en el sistema SOAT no
	// respetan el estandard ISO-8583 y vienen informados en un
	// mensaje propietario en el campo 126...

	// SOATUserTrxCode [x..y=6]
	int iOffset = FieldOffset(126),
		iType   = FieldType(126)  ;
	// Message-Type-Identifier [12..15=4]    	
	return(((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
		    (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0) )
		   &&		  
		   // SOATTrxCode [2+2+2]
	       (memcmp((PBYTE)&chMsg[iOffset+iType  ], "03", 2) == 0)
		   &&
	       (isdigit(chMsg[iOffset+iType+2]) && isdigit(chMsg[iOffset+iType+2+1]))
		   &&
	       (memcmp((PBYTE)&chMsg[iOffset+iType+4], "00", 2) == 0)		   		   
		  );
}

EXPORT BOOL APPMSG::IsSOATGenericRequest(void)     // Pedido Generico SOAT?
{
	// SOAT-TrxCode S-126
	if(!IsValidField(126)) 
		return (FALSE);

	// Los codigos de transaccion de usuario en el sistema SOAT no
	// respetan el estandard ISO-8583 y vienen informados en un
	// mensaje propietario en el campo 126...

	// SOATUserTrxCode [x..y=6]
	int iOffset = FieldOffset(126),
		iType   = FieldType(126)  ;
	// Message-Type-Identifier [12..15=4]    	
	return(( (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) ||
			 (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0) )
		   &&		  
		   // SOATTrxCode [6] : Cualquier codigo numerico de 6 digitos
	       ( isdigit(chMsg[iOffset+iType]  ) && isdigit(chMsg[iOffset+iType+1]) &&
		     isdigit(chMsg[iOffset+iType+2]) && isdigit(chMsg[iOffset+iType+3]) &&
			 isdigit(chMsg[iOffset+iType+4]) && isdigit(chMsg[iOffset+iType+5])
		   )
		  );
}

// Es valido el comando del sistema?
EXPORT BOOL APPMSG::IsSOATSystemCmd(void)
{
	// SOAT-TrxCode S-126
	if(!IsValidField(126)) 
		return (FALSE);

	// Los codigos de transaccion de usuario en el sistema SOAT no
	// respetan el estandard ISO-8583 y vienen informados en un
	// mensaje propietario en el campo 126...

	// SOATUserTrxCode [x..y=6]
	int iOffset = FieldOffset(126),
		iType   = FieldType(126)  ;
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // SOATTrxCode [6]
	       (memcmp((PBYTE)&chMsg[iOffset+iType], "010000", 6) == 0) 
		  );
}

// Es valido el logon para cambio de clave?
EXPORT BOOL APPMSG::IsSOATLogonPassword(void)
{
	// SOAT-TrxCode S-126
	if(!IsValidField(126)) 
		return (FALSE);

	// Los codigos de transaccion de usuario en el sistema SOAT no
	// respetan el estandard ISO-8583 y vienen informados en un
	// mensaje propietario en el campo 126...

	// SOATUserTrxCode [x..y=6]
	int iOffset = FieldOffset(126),
		iType   = FieldType(126)  ;
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		   &&		  
		   // SOATTrxCode [6]
	       (memcmp((PBYTE)&chMsg[iOffset+iType], "012003", 6) == 0) 
		  );
}
////////////////////////////////////////////////////////////////////////
#endif 

