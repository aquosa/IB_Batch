/////////////////////////////////////////////////////////////////////////////////////////////
// IT24 Sistemas S.A.
// DATANET E_SETTLEMENT & DPS INTERFACE
//
// Clase Mensaje Financiero DATANET [Version 6.2,Agosto_2002],
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2002.12.27      mdc     Base
// (Inicial)    2003.01.16      mdc     Base rediseñada en 2 registros input/output
// (Inicial)    2003.03.21      mdc     DPS
// (Alfa)       2003.09.18      mdc     Calculo de MAC , informa nro. bloques
// (Alfa)       2003.10.21      mdc     Calculo de MAC , contemplado SISTEMA MARIA de ADUANA
// (Alfa)       2003.10.29      mdc     Forzar "99" en operador DEBITO y CREDITO-1.
// (Alfa)       2003.11.07      mdc     boolean_t DPS_in_out::IsResponseCodeInFinalState( )
// (Alfa)       2003.11.21      mdc     ::GetStatusCode()
// (Beta)       2003.11.25      mdc     Precondicion de ESTADO valido en ::SetStatusCode()
// (Beta)       2003.12.05      mdc     GetCauseCode()
// (Beta)       2004.01.08      mdc     GetRecType()
// (Beta)       2004.02.23      mdc     Metodos Publicos de clase DPIFCOMM (Refresh Online)
// (1.0.2.10)   2004.05.27      MDC     ::SetCauseCode() incluye respuestas de CREDITOS
// (1.0.2.11)   2004.06.16      mdc     Que no se repita el motivo de rechazo tanto para CR como DB
// (1.0.2.12)   2004.08.31      mdc     Refresh de Cobranzas
// (1.0.2.13)   2005.11.11      mdc     #define IS_ON_OUR_BANKCODE(x)   
// (1.0.2.14)   2006.07.28      mdc     ReadString(char * lpValueName,char * lpData, int maxlen)
/////////////////////////////////////////////////////////////////////////////////////////////

// Headers estandards del lenguaje
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
// Headers propios
#include <qusrinc/datanet.h>
#include <qusrinc/trxdnet.h>
#include <qusrinc/stdiol.h>

#ifndef APPC_FLIPI
#include <QUSRINC/appcsna.h>
#endif // APPC_FLIPI

// Macro de verificacion de codigo de banco emisor/receptor , si es banco propio
#define IS_ON_OUR_BANKCODE(x)   (BCRA_ID_DEFAULT_I == antoi(x,3) || BCRA_ALTERNATE_ID_I == antoi(x,3)) 
// Macro simplificada de copia de memoria campo a campo
#define COPYFIELD( to, from )   memcpy( to,from,min(sizeof(to),sizeof(from)) );
// Macro de asignacion de ceros a campo de estructura
#define SETFIELDZEROED(dst)   memset(dst ,'0',sizeof(dst))
#define SETFIELDBLANKED(dst)  memset(dst ,' ',sizeof(dst))

////////////////////////////////////////////////////////////////
// E-SETTLEMENT
////////////////////////////////////////////////////////////////
// INPUT
E_Settlement_hdr_i::E_Settlement_hdr_i()
{
	memset( &data, ' ', sizeof data);
	memcpy( data.HDR_CICS_TRAN, "ECOM", 4);
	// HDR_LONGITUD	[5],	// Longitud del Mensaje incluyendo HDR
	// HDR_CICS_NODO	[4],	// Nombre del Nodo DPS de origen
	memcpy( data.HDR_COD_MENSAJE  ,"TEF-DEB-AUT     ", 16 );	// TEF_DEB_AUT
	// HDR_COD_RETORNO	[4],	// Código de Retorno
	memcpy( data.HDR_CANT         ,"000", 3);
	memcpy( data.HDR_ULT_REGISTRO ,"0", 1);
	memcpy( data.HDR_CONN_TIPO    ,"L", 1);
	memcpy( data.HDR_CODE_PAGE	  ,"E", 1);
	// HDR_FILLER		[41];	// Blancos
}
E_Settlement_hdr_i::~E_Settlement_hdr_i()
{
}
E_Settlement_i::E_Settlement_i()
{
	memset( &data, ' ', sizeof data);
}
E_Settlement_i::~E_Settlement_i()
{
	memset( &data, ' ', sizeof data);
}
////////////////////////////////////////////////////////////////
// OUTPUT
E_Settlement_hdr_o::E_Settlement_hdr_o()
{
	memset( &data, ' ', sizeof data);
	memcpy( data.HDR_CICS_TRAN, "ECOM", 4);
	// HDR_LONGITUD	[5],	// Longitud del Mensaje incluyendo HDR
	// HDR_CICS_NODO	[4],	// Nombre del Nodo DPS de origen
	memcpy( data.HDR_COD_MENSAJE  ,"TEF-DEB-AUT     ", 16 );	// TEF_DEB_AUT
	// HDR_COD_RETORNO	[4],	// Código de Retorno
	memcpy( data.HDR_CANT         ,"000", 3);
	memcpy( data.HDR_ULT_REGISTRO ,"0", 1);
	memcpy( data.HDR_CONN_TIPO    ,"L", 1);
	memcpy( data.HDR_CODE_PAGE	  ,"E", 1);
	// HDR_FILLER		[41];	// Blancos
}
E_Settlement_hdr_o::~E_Settlement_hdr_o()
{
}
E_Settlement_o::E_Settlement_o()
{
	memset( &data, ' ', sizeof data);
}
E_Settlement_o::~E_Settlement_o()
{
	memset( &data, ' ', sizeof data);
}

////////////////////////////////////////////////////////////////
// DISTRIBUTED PROCESSING SYSTEM (DPS)
////////////////////////////////////////////////////////////////
// Metodos Publicos de clase DPS_i
DPS_i::DPS_i()
{
    // Valores por omision 
	memset( &data, ' ', sizeof data);
}
DPS_i::~DPS_i()
{
}
// Metodos Publicos de clase DPS_o
DPS_o::DPS_o()
{
    // Valores por omision 
	memset( &data, ' ', sizeof data);
}
DPS_o::~DPS_o()
{
}

// Metodos Publicos de clase DPIFCOMM (Refresh Online)
DPIFCOMM::DPIFCOMM()
{
    // Valores por omision para "CUENTAS"
	memset( &data, ' ', sizeof data);
	memcpy( data.FRNAME, "****", 4);
    memcpy( data.TONAME, "DNET", 4);
    memcpy( data.RECTYPE, "CTA", 3);
    data.ACTIVITY[0] = 'N';
    // Por omision la longitud es la del registro de CUENTAS
	data.COUNTER[0] = 0x00; //Longitud del registro	145 (X'0091')
    data.COUNTER[1] = 0x6A;	//Longitud en EBCDIC traducido es '6A'    
    memset( data.FILLER, ' ', 6);
}
DPIFCOMM::~DPIFCOMM()
{
}

////////////////////////////////////////////////////////////////
// Constructor
DPS_in_out::DPS_in_out()
{
    char szValue[] = "FALSE" ; // FALSE

	// Blanqueo de estructuras
	memset( &macData    , ' ', sizeof macData);
	memset( &input.data , ' ', sizeof input.data );
	memset( &output.data, ' ', sizeof output.data);
	memset( achMAC1, ' ', sizeof achMAC1 );
	memset( achMAC2, ' ', sizeof achMAC2 );
	memset( achMAC3, ' ', sizeof achMAC3 );
    /////////////////////////////////////////
    // Marca de Controles, si se hacen o no
    bCONTROL_MAC_123					=
    bCONTROL_BANK_EXISTENCE			    =
    bCONTROL_ACCOUNT_EXISTENCE			=
    bCONTROL_ACCOUNT_STATUS				=
    bCONTROL_ASSOCIATE_EXISTENCE		=
    bCONTROL_POSTING_DATE				= FALSE ;
    /////////////////////////////////////////
    ReadString( "CONTROL_MAC_123", szValue, 1 );
    bCONTROL_MAC_123 = atoi( szValue );
    ReadString( "CONTROL_BANK_EXISTENCE", szValue, 1 );
    bCONTROL_BANK_EXISTENCE = atoi( szValue );
    ReadString( "CONTROL_ACCOUNT_EXISTENCE", szValue, 1 );
    bCONTROL_ACCOUNT_EXISTENCE = atoi( szValue );
    ReadString( "CONTROL_ACCOUNT_STATUS", szValue, 1 );
    bCONTROL_ACCOUNT_STATUS = atoi( szValue );
    ReadString( "CONTROL_ASSOCIATE_EXISTENCE", szValue, 1 );
    bCONTROL_ASSOCIATE_EXISTENCE = atoi( szValue );
    ReadString( "CONTROL_POSTING_DATE", szValue, 1 );
    bCONTROL_POSTING_DATE = atoi( szValue );
    /////////////////////////////////////////
};

////////////////////////////////////////////////////////////////
// Metodos Publicos de clase DPS_in_out
boolean_t DPS_in_out::IsDPSTransaction()
{
	// Es DEBITO/CREDITO ?
	return (IsDPSCreditTransfer()		  ||
			IsDPSDebitTransfer()		  || 
			IsDPSCreditAndDebitTransfer() ||
			IsDPSAccountVerification()	  ||
			IsDPSOtherMessages() 
		) 
		? is_true
		: is_false;
}
//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::IsDPSCreditTransfer()
{
	// Es CREDITO aprobado o rechazado?
	return ( memcmp(input.data.RECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO,3)==0
			 &&
			 memcmp(input.data.DATCRE.BANCRE,BCRA_ID_DEFAULT,3)==0)
		? is_true
		: is_false;
}
//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::IsDPSDebitTransfer()
{
	// Es DEBITO aprobado o rechazado?
	return ( memcmp(input.data.RECTYPE,DPS_RECTYPE_DEBITO_EJECUTADO,3)==0			 
			 &&
			 memcmp(input.data.BANDEB,BCRA_ID_DEFAULT,3)==0)
		? is_true
		: is_false;
}
//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::IsDPSCreditAndDebitTransfer()
{
	// Es CREDITO y DEBITO entre cuentas relacionadas ?
	return ( memcmp(input.data.RECTYPE,DPS_RECTYPE_CREDITO_RESPUESTA,3)==0
			 &&
			 memcmp(input.data.DATCRE.BANCRE,BCRA_ID_DEFAULT,3)==0 
			 &&
			 memcmp(input.data.BANDEB,BCRA_ID_DEFAULT,3)==0 )
		? is_true
		: is_false;
}
//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::IsDPSAccountVerification()
{
    // Sin implementar por omision
	return is_false;
}
//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::IsDPSOtherMessages()
{
	// Es CREDITO RECHAZADO o DEBITO RECHAZADO u OTRO mas?
	if (memcmp(input.data.RECTYPE,DPS_RECTYPE_CREDITO_RESPUESTA,3)==0
		||
	    memcmp(input.data.RECTYPE,DPS_RECTYPE_DEBITO_RESPUESTA,3)==0
			 )
		return is_true;
    // Por omision, siempre TRUE para loggear cualquier otro mensaje
	return is_true;
}
//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::IsValidReversal()
{
	// Es DEBITO aprobado o rechazado? y REVERSO por riesgo red?
	return ( memcmp(input.data.RECTYPE,DPS_RECTYPE_DEBITO_EJECUTADO,3)==0			 
			 &&
			 memcmp(input.data.BANDEB,BCRA_ID_DEFAULT,3)==0
             &&
             memcmp(input.data.ESTADO,_DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED_,2)==0
            )
		? is_true
		: is_false;
}

//////////////////////////////////////////////////////////////////
// Formatear mensaje de respuesta en base a mensaje de entrada
boolean_t DPS_in_out::FormatResponseMessage(PCHAR pszMAC2, PCHAR pszMAC3)
{

	// Precondicion : Parametros de MAC-2 o MAC-3 calculadas previamente
	if(NULL == pszMAC2 || NULL == pszMAC3)
		return is_false;

	// Copiar las MACs calculadas, para referencias futuras
	memcpy( achMAC2, pszMAC2, sizeof(achMAC2) );
	memcpy( achMAC3, pszMAC3, sizeof(achMAC3) );

	// Formatear el mensaje saliente a partir del mensaje entrante autorizado
	// Para informacion de los contenidos de los campos , ver documentacion
	// interna del BANSUD-MACRO sobre el proyecto DATANET
    memcpy(output.data.header.TPNAME,DPS_TPNAME_DPRE  ,	4);	//Nombre del programa autorizante "DPRE"
    memcpy(output.data.header.NDNAME,input.data.TONAME,	4);	//Nombre nodo receptor	ej."BSUD" : Bansud    
    memset(output.data.header.HORA, 0x00,               4); //TimeStamp local "1" en PACKED-DECIMAL
    output.data.header.HORA[0] = 0x00;
    output.data.header.HORA[1] = 0x00;
    output.data.header.HORA[2] = 0x00;
    output.data.header.HORA[3] = 0x1C;
    memset(output.data.header.SECUENCIA, 0x00,          2); //Secuencia local "1" en PACKED-DECIMAL
    output.data.header.SECUENCIA[0] = 0x00;
    output.data.header.SECUENCIA[1] = 0x1C;
    // Se invierten nodos receptor-emisor
	memcpy(output.data.FRNAME	,input.data.TONAME,	4);	//Nombre nodo emisor	ej."DNET" : Datanet
	memcpy(output.data.TONAME	,input.data.FRNAME,	4);	//Nombre nodo receptor	ej."BSUD" : Bansud    
	memcpy(output.data.RECTYPE	,input.data.RECTYPE,3);	//Tipo de registro	TDE = débito TCE = crédito
	output.data.ACTIVITY[0]    = input.data.ACTIVITY[0];//Actividad	"N"
    // Longitud es 100, en Hexadecimal es "0064"
	output.data.COUNTER[0] = 0x00;  //Longitud del registro	100 (X'0064')
    output.data.COUNTER[1] = 0xB4;	//Longitud en EBCDIC traducido es 'B4'
	memcpy(output.data.FILLER3	,input.data.FILLER1,6);	//Reservado	Uso Datanet
	memcpy(output.data.BANDEB	,input.data.BANDEB,	3);	//Código Banco según BCRA	"067" si corresponde
	memcpy(output.data.FECSOL	,input.data.FECSOL,	8); //Fecha de compensación	Del día o prox. Hábil -aaaammdd-
	memcpy(output.data.NUMTRA	,input.data.NUMTRA,	7); //Número de transferencia	
	memcpy(output.data.NUMABO	,input.data.NUMABO,	7); //Abonado	
	memcpy(output.data.IMPORTE	,input.data.IMPORTE,17);	//Importe de la transferencia	15 enteros y 2 decimales
	memcpy(output.data.BANCRE	,input.data.DATCRE.BANCRE, 3); //Código Banco según BCRA	"067" si corresponde
	memcpy(output.data.ESTADO	,input.data.ESTADO,	2);	//Estado en que queda la operación	Ver tabla IV
	memset(output.data.OPEEJ1	,'9'              ,	2);	//Primer operador que autoriza el cambio de estado	"99"
	memset(output.data.OPEEJ2	,'0'              ,	2);	//Segundo operador que autoriza el cambio de estado	"00"
	// Motivo de Rechazo/Aprobacion segun ESTADO
	if( strncmp(input.data.ESTADO,_DPS_EJECUTADA_,2) == 0 )
    {
		memset(output.data.CODRCH, '0' , 4);	//Motivo del rechazo del Banco de débito	Ver tabla III
    }
	else
	{
        //Motivo del rechazo del Banco de débito	(Ver tabla III)
		if( (strncmp(input.data.RECTYPE,DPS_RECTYPE_DEBITO_RESPUESTA,3)==0 
             ||
             strncmp(input.data.RECTYPE,DPS_RECTYPE_DEBITO_EJECUTADO,3)==0)
            &&
            antoi(input.data.DATDEB.RECHDB,4) > 0 )
        {
			memcpy(output.data.CODRCH,  input.data.DATDEB.RECHDB, 4);	//Motivo del rechazo del Banco de débito
        }
        //Motivo del rechazo del Banco de credito , monobanco
		else if( (strncmp(input.data.RECTYPE,DPS_RECTYPE_DEBITO_RESPUESTA,3)==0 
                    ||
                  strncmp(input.data.RECTYPE,DPS_RECTYPE_DEBITO_EJECUTADO,3)==0 )
            &&
            antoi(input.data.DATCRE.RECHCR,4) > 0 )
        {
			memcpy(output.data.CODRCH,  input.data.DATCRE.RECHCR, 4);	//Motivo del rechazo del Banco de credito
        }
        //Motivo del rechazo del Banco de credito, otros bancos
		else if( (strncmp(input.data.RECTYPE,DPS_RECTYPE_CREDITO_RESPUESTA,3)==0 
                    ||
                  strncmp(input.data.RECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO,3)==0 )
            &&
            antoi(input.data.DATCRE.RECHCR,4) > 0 )
        {
			memcpy(output.data.CODRCH,  input.data.DATCRE.RECHCR, 4);	//Motivo del rechazo del Banco de credito
        }
        //Motivo del rechazo del Banco de credito, otros bancos
		else if( (strncmp(input.data.RECTYPE,DPS_RECTYPE_CREDITO_RESPUESTA,3)==0 
                    ||
                  strncmp(input.data.RECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO,3)==0 )
            &&
            antoi(input.data.DATDEB.RECHDB,4) > 0 )
        {
			memcpy(output.data.CODRCH,  input.data.DATDEB.RECHDB, 4);	//Motivo del rechazo del Banco de debito
        }
        else
        {
            // DPS_OPERACION_NO_HABILITADA_EN_BANCO	= 21,            
            memcpy(output.data.CODRCH,  DPS_MOTIVO_RECHAZO_DEFAULT_STR, 4);	//Motivo del rechazo por omision
        };
	}
	// Copiar la MAC segun codigo de BANCO propio
	// Ya debe estar calculada para su copia ... ATENCION
	if( IS_ON_OUR_BANKCODE(input.data.BANDEB) )
		memcpy(output.data.CODMAC, pszMAC2, 12);//Message Authentication Code (MAC)	Ver Anexo A
	else
		memcpy(output.data.CODMAC, pszMAC3, 12);//Message Authentication Code (MAC)	Ver Anexo A
	memcpy(output.data.FECPRO, input.data.FECAUT, 6);//Fecha de proceso	es AUTORIZADO
    memcpy(output.data.FECPRO, input.data.FECEST, 6);//Fecha de proceso	es la del ESTADO
	memset(output.data.FILLER, ' ',  27);//Reservado
    // Postcondicion : siempre Ok
	return is_true;
}
//////////////////////////////////////////////////////////////////
// Establece el tipo de registro de respuesta a enviar
boolean_t DPS_in_out::SetRecType( PCHAR szRecType )
{
	if( strncmp(szRecType,DPS_RECTYPE_DEBITO_RESPUESTA ,3)==0 ||
		strncmp(szRecType,DPS_RECTYPE_CREDITO_RESPUESTA,3)==0 ||
		strncmp(szRecType,DPS_RECTYPE_CREDITO_EJECUTADO,3)==0 ||
		strncmp(szRecType,DPS_RECTYPE_DEBITO_EJECUTADO ,3)==0 )
	{
		strncpy(input.data.RECTYPE,szRecType,3);
		return is_true;
	}
	else
		return is_false;
}
//////////////////////////////////////////////////////////////////
// Establece el motivo - causal de rechazo segun debito - credito
boolean_t DPS_in_out::SetCauseCode( WORD  wCauseCode , boolean_t bIsCredit )
{
    char  szCauseCode[sizeof(input.data.DATCRE.RECHCR)+1] = {"00"};
    short shRespCde      = antoi(input.data.ESTADO, 2 ),
          shCurCauseCode = 0;
    // Precondicion : Obtener causa actual
	if( bIsCredit )
		shCurCauseCode = antoi(input.data.DATCRE.RECHCR,sizeof(input.data.DATDEB.RECHDB));    
	else
        shCurCauseCode = antoi(input.data.DATDEB.RECHDB,sizeof(input.data.DATDEB.RECHDB));		
    // Precondicion : No cambiar ESTADO si el que se informa es un estado FINAL,
    // a menos que se aun "FALTAN DATOS EN DETALLE OP" (BANCO INEX.)
    // a menos que sea un CREDITO EJECUTADO rechazado por CTA. INEX./BLOQ./ y similares
    if((DPS_EJECUTADA					 == shRespCde // 60
        ||
	    DPS_RECHAZO_DEL_BANCO_DE_DEBITO	 == shRespCde // 70
        ||
	    DPS_RECHAZO_DEL_BANCO_DE_CREDITO == shRespCde) // 80
        &&
        // A menos que se aun "FALTAN DATOS EN DETALLE OP" (BANCO INEX.)
        DPS_FALTAN_DATOS_EN_DETALLE_OPERACION != shCurCauseCode
        &&
        // A menos que sea un CREDITO EJECUTADO/RESP. rechazado por CTA. INEX./BLOQ./ y similares
        (GetRecType() != DPS_CREDITO_EJECUTADO && 
         GetRecType() != DPS_CREDITO_RESPUESTA)
        )
    {
        // Postcondicion : FALSE
        return is_false;
    }
    else
    {
        // Validar y consistir el codigo de rechazo
        if( wCauseCode >= DPS_APROBADA_SIN_RECHAZO 
            && 
            wCauseCode <= DPS_MAC_ERRONEO_NETWORK)
        {
            // Convertir a string ASCII desde INTEGER
	        sprintf(szCauseCode, "%0*i", sizeof(input.data.DATCRE.RECHCR), wCauseCode );
            // Continuar.... 
        }
        else
        {
            // Postcondicion : FALSE
            return is_false;
        };
        // Es credito? actualizar campo respectivo
	    if( bIsCredit )
        {
            memcpy(input.data.DATCRE.RECHCR, szCauseCode, sizeof(input.data.DATCRE.RECHCR) );
            memset(input.data.DATDEB.RECHDB, '0', sizeof(input.data.DATDEB.RECHDB) );
        }
        // Sino es debito, actualizar campo respectivo
	    else
        {
		    memcpy(input.data.DATDEB.RECHDB, szCauseCode, sizeof(input.data.DATDEB.RECHDB) );
            memset(input.data.DATCRE.RECHCR, '0', sizeof(input.data.DATCRE.RECHCR) );
        };
        // Postcondicion : TRUE
	    return is_true;
    }; /* end-if-ESTADO-FINAL */
}
/////////////////////////////////////////////////////////////////////
// Recupera los 2 causales - motivos de rechazo
boolean_t DPS_in_out::GetCauseCode( char szRECHCR[], char szRECHDB[] )
{
    // Precondicion : parametros informados
    if(	!szRECHCR || !szRECHDB)
        // Postcondicion : FALSE
        return is_false;
    // Copiar los 4 digitos del motivo de rechazo, debito y credito
    strncpy(szRECHCR, input.data.DATCRE.RECHCR, sizeof(input.data.DATCRE.RECHCR) );
    strncpy(szRECHDB, input.data.DATDEB.RECHDB, sizeof(input.data.DATDEB.RECHDB) );
    // Postcondicion : TRUE
    return is_true;
}
//////////////////////////////////////////////////////////////////
// Modifica el ESTADO ACTUAL
// WORD  wRespCode = Nuevo Estado 
// boolean_t bAPROBADA_FALTAN_DATOS_EN_OPERACION : indicativo de si 
//           se debe forzar por banco inexistente.
boolean_t DPS_in_out::SetStatusCode(WORD  wRespCode ,
                                    boolean_t bAPROBADA_FALTAN_DATOS_EN_OPERACION)
{
  	char  szState[sizeof(input.data.ESTADO)+1] = {"00"};
    short shRespCde = antoi(input.data.ESTADO, 2 );
    // Precondicion : No cambiar ESTADO si el que se informa es un estado FINAL
    // Precondicion : El estado final "60" puede pasar a "80"
    if((DPS_EJECUTADA					 == shRespCde  // 60
        ||
	    DPS_RECHAZO_DEL_BANCO_DE_DEBITO	 == shRespCde  // 70
        ||
	    DPS_RECHAZO_DEL_BANCO_DE_CREDITO == shRespCde)  // 80
	   &&
       is_false == bAPROBADA_FALTAN_DATOS_EN_OPERACION
       &&
       GetRecType() != DPS_CREDITO_EJECUTADO
      )
    {
        // Postcondicion : FALSE
        return is_false;
    }
	//2008-04-07	MRB	Agregado para evitar que el estado 90
	//					se pisa.
	else if (DPS_RECHAZO_CUENTA_CREDITO      == shRespCde) // 90
	{
		return is_false;
	}
    else
    {
        // Precondicion : ESTADO valido
        if( wRespCode != DPS_ENVIADO_POR_EL_ABONADO_			    &&
	        wRespCode != DPS_ENVIADA_AL_BANCO_DE_DEBITO		        &&
	        wRespCode != DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO	    &&
	        wRespCode != DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED   &&
	        wRespCode != DPS_VALOR_AL_COBRO				            &&
	        wRespCode != DPS_ENVIADA_AL_BANCO_DE_CREDITO	        &&
	        wRespCode != DPS_EJECUTADA						        &&
	        wRespCode != DPS_RECHAZO_DEL_BANCO_DE_DEBITO            &&
	        wRespCode != DPS_RECHAZO_DEL_BANCO_DE_CREDITO           )
            return is_false;
        /////////////////////////////////////////////////////////
        // Precondicion : El estado final "60" puede pasar a "80"
        if( GetRecType() == DPS_CREDITO_EJECUTADO 
            &&
            DPS_RECHAZO_DEL_BANCO_DE_CREDITO != wRespCode)
            return is_false;
        /////////////////////////////////////////////////////////
        // Convertir a string ASCII
	    sprintf(szState, "%02i", wRespCode );
        // Actualizar
	    memcpy(input.data.ESTADO, szState, 2 );
        // Postcondicion : TRUE
	    return is_true;
    };
}
//////////////////////////////////////////////////////////////////
// Recupera el ESTADO actual de la transferencia valida
WORD DPS_in_out::GetStatusCode(  )
{
    short shRespCde = antoi(input.data.ESTADO, 2 );
    // Postcondicion : ESTADO DE RESPUESTA
    return (IsValidMessage())
        ? shRespCde
        : ( ! DPS_ENVIADO_POR_EL_ABONADO_ ) ;
}
//////////////////////////////////////////////////////////////////
// Modifica el ESTADO ACTUAL
// PCHAR     pszRespCode = Nuevo Estado en ASCII
// boolean_t bAPROBADA_FALTAN_DATOS_EN_OPERACION : indicativo de si 
//           se debe forzar por banco inexistente.
boolean_t DPS_in_out::SetStatusCode(PCHAR pszRespCode ,
                                    boolean_t bAPROBADA_FALTAN_DATOS_EN_OPERACION )
{
  	char  szState[sizeof(input.data.ESTADO)+1] = {"00"};
    short shRespCde = antoi(input.data.ESTADO, 2 );
    // Precondicion : No cambiar ESTADO si el que se informa es un estado FINAL
    if((DPS_EJECUTADA					 == shRespCde  // 60
        ||
	    DPS_RECHAZO_DEL_BANCO_DE_DEBITO	 == shRespCde  // 70
        ||
	    DPS_RECHAZO_DEL_BANCO_DE_CREDITO == shRespCde)  // 80
       &&
       is_false == bAPROBADA_FALTAN_DATOS_EN_OPERACION
      )
    {
        // Postcondicion : FALSE
        return is_false;
    }
	//2008-04-07	MRB	Agregado para evitar que el estado 90
	//					se pisa.
	else if (DPS_RECHAZO_CUENTA_CREDITO      == shRespCde) // 90
	{
		return is_false;
	}
    else
    {
        // Precondicion : Si se informa el ESTADO nuevo, copiarlo
	    if(pszRespCode)
	    {
		    memcpy(input.data.ESTADO, pszRespCode, 2 );
            // Postcondicion : Es OK
		    return is_true;
	    }
	    else
            // Postcondicion : Es ERROR, no se copio nada
		    return is_false;
    };
}
//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::IsResponseCodeInFinalState( )
{
    short shRespCde = antoi(input.data.ESTADO, 2 );
    // Es estado 60,70 u 80?
    return (		
	        DPS_EJECUTADA					 == shRespCde // 60
            ||
	        DPS_RECHAZO_DEL_BANCO_DE_DEBITO	 == shRespCde // 70
            ||
	        DPS_RECHAZO_DEL_BANCO_DE_CREDITO == shRespCde // 80
            )
            ? is_true
            : is_false;
}

//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::Import( PBYTE pcbBuffer, WORD wBuffLen )
{
	// Precondicion
	if(NULL == pcbBuffer)
		return is_false;	
	memcpy( &input, pcbBuffer, min(wBuffLen,sizeof(input)) );
	return is_true;
}
//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::IsValidMessage()
{	
	return is_true;
}
//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::Export( PBYTE pcbBuffer, PWORD pwBuffLen )
{
	// Precondicion
	if(NULL == pwBuffLen || NULL == pcbBuffer)
		return is_false;
	// Armado y copia de mensaje saliente
	memcpy( &output, &input, min(sizeof(input),sizeof(output)) );
	memcpy( pcbBuffer, &output, min(sizeof(output),*pwBuffLen) );
	return is_true;
}

//////////////////////////////////////////////////////////////////
// Recuperar tipo de operacion de entrada
short DPS_in_out::GetOperType( PCHAR szOperType )
{
	// Precondicion
	if(szOperType)
	{
		// Copiar en formato ASCII TEXTO
		memcpy( szOperType, input.data.TIPOPE, sizeof input.data.TIPOPE );
		szOperType[ sizeof input.data.TIPOPE ] = 0x00;
	}
	// Retornar en formato BINARIO
	return antoi(input.data.TIPOPE,sizeof input.data.TIPOPE);
}
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// Recuperar MAC de entrada
// PCHAR pszMAC : argumento de copia de la MAC
//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::GetMAC( PCHAR pszMAC )
{
	// Precondicion
	if(pszMAC)
	{
		memcpy( pszMAC, input.data.CODMAC, sizeof input.data.CODMAC );
		pszMAC[sizeof input.data.CODMAC] = 0x00;		
		return is_true;
	}
	else
		return is_false;
}
//////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Formatear los parametros de entrada para la rutina NDES de 
// validacion de MAC, dentro de la base de datos SQL como Std.Proc.,
// o como funcion en libreria anexa (runtime-library).
// Ver documentacion de Interbanking S.A. para mas detalles.
// PCHAR pszMACinputNDES : Texto de salida formateado
// short iLen       : Longitud del texto a formatear
// short iMAC_type  : Tipo de MAC : 1,2 o 3
// PSHORT pshBlocks : Cantidad de bloques de 8 bytes
/////////////////////////////////////////////////////////////////////
PCHAR DPS_in_out::GetMACTextToNDES(PCHAR pszMACinputNDES, 
                                   short iLen, short iMAC_type,
                                   PSHORT pshBlocks,
                                   char chValidateOrCalculate )
{
    BOOL  bDEBUG_PRINT_PARAMS = _DPS_DEBUG_PRINT_PARAMS_;        
	FILE			   *hFile = fopen("debug.mac", "at");
    ////////////////////////////////////////////////////////////////////
    // Estructura auxiliar para "NOMBRE", tomando un SUBSTRING de 15.
    union WNOMSOL
    {
        char DATA           [29];
        struct 
        {
            char FILLER1    [ 5];
            char W_R_NOMBRE [15];
            char FILLER2    [ 9];
        } stWNOMBRE;
    } uWNOMBRE;
    ////////////////////////////////////////////////////////////////////
    // Estructura auxiliar para "OBSERV2", pisando con ceros y blancos.
    union WOBSER2
    {
        char DATA          [100];                              
        struct
        {
            char  CODANA         [ 3];                               
            char  CTIMPEXP       [11];                               
            char  CTDESPA        [11];                               
            char  CTABONADO      [11];                               
            char  SITUIVA        [ 2];                               
            char  COMBANCO       [13];                            
            char  IVACOMI        [13];                            
            char  NROTRAN        [ 7];     
        } stWOBSER2;
    } uWOBSER2;
    ////////////////////////////////////////////////////////////////////
    short shTxtLen = 0;
    
    
    ////////////////////////////////////////////////////////////////////
	// Precondicion
	if(NULL == pszMACinputNDES || iLen < sizeof(macData) || 
		iMAC_type < 1 || iMAC_type > 3 || NULL == pshBlocks)
		return NULL;
    ////////////////////////////////////////////////////////////////////
	
	// Calcular MAC-1-2 o 3
	// Completar parametros de invocacion de funcion NDES3
	memset( &macData, 0x00, sizeof macData);
	// MAC 1 ?
	if( 1==iMAC_type )
	{
		// Armar texto de entrada de la funcion NDES3
		COPYFIELD( macData.BANDEB , input.data.BANDEB );
		COPYFIELD( macData.FECSOL , input.data.FECSOL  );
		COPYFIELD( macData.NUMABO , input.data.NUMABO  );
		COPYFIELD( macData.IMPORTE, input.data.IMPORTE ); // PIC 9(15)V99
		COPYFIELD( uWNOMBRE.DATA  , input.data.DATDEB.NOMSOL );
        COPYFIELD( macData.WNOMSOL, uWNOMBRE.stWNOMBRE.W_R_NOMBRE );        
		COPYFIELD( macData.TIPCUE , input.data.DATDEB.TIPCUE  );
		COPYFIELD( macData.NUMCTA , input.data.DATDEB.NUMCTA  );
		COPYFIELD( macData.BANCRE , input.data.DATCRE.BANCRE  );
		COPYFIELD( uWNOMBRE.DATA  , input.data.DATCRE.NOMBEN );
		COPYFIELD( macData.WNOMBEN, uWNOMBRE.stWNOMBRE.W_R_NOMBRE );
		COPYFIELD( macData.TIPCRE , input.data.DATCRE.TIPCRE  );
		COPYFIELD( macData.CTACRE , input.data.DATCRE.CTACRE  );
		COPYFIELD( macData.OPEAU1 , input.data.OPEAU1  );
		COPYFIELD( macData.OPEAU2 , input.data.OPEAU2  );
		COPYFIELD( macData.OPEAU3 , input.data.OPEAU3  );
		COPYFIELD( macData.OBSER1 , input.data.OBSER1  );        
		COPYFIELD( macData.OBSER2 , input.data.OBSER2  );
        // Si es SISTEMA MARIA de ADUANA, tratarlo especialmente
        if( antoi(input.data.TIPOPE,2) == DPS_ANA_SISTEMA_MARIA )
        {
            COPYFIELD( uWOBSER2.DATA  , input.data.OBSER2 );
            SETFIELDZEROED( uWOBSER2.stWOBSER2.CTABONADO );
            SETFIELDZEROED( uWOBSER2.stWOBSER2.NROTRAN );
            SETFIELDBLANKED( uWOBSER2.stWOBSER2.SITUIVA );
            COPYFIELD( macData.OBSER2 , uWOBSER2.DATA  );
        };
		COPYFIELD( macData.NUMREF , input.data.NUMREF  );
		COPYFIELD( macData.NUMENV , input.data.NUMENV  );
		COPYFIELD( macData.varMAC.stMAC1.RESREG_1, "A123.." );
		// *** Bloque de 36 bytes de longitud ***		
        *pshBlocks = 36;
	}
	else // MAC 2 o 3....
	{
		// Armar texto de entrada de la funcion NDES3
        // Desde aqui, es igual a la MAC-1....
		COPYFIELD( macData.BANDEB , input.data.BANDEB );
		COPYFIELD( macData.FECSOL , input.data.FECSOL  );
		COPYFIELD( macData.NUMABO , input.data.NUMABO  );
		COPYFIELD( macData.IMPORTE, input.data.IMPORTE ); // PIC 9(15)V99
		COPYFIELD( uWNOMBRE.DATA  , input.data.DATDEB.NOMSOL );
        COPYFIELD( macData.WNOMSOL, uWNOMBRE.stWNOMBRE.W_R_NOMBRE );        
		COPYFIELD( macData.TIPCUE , input.data.DATDEB.TIPCUE  );
		COPYFIELD( macData.NUMCTA , input.data.DATDEB.NUMCTA  );
		COPYFIELD( macData.BANCRE , input.data.DATCRE.BANCRE  );
		COPYFIELD( uWNOMBRE.DATA  , input.data.DATCRE.NOMBEN );
		COPYFIELD( macData.WNOMBEN, uWNOMBRE.stWNOMBRE.W_R_NOMBRE );
		COPYFIELD( macData.TIPCRE , input.data.DATCRE.TIPCRE  );
		COPYFIELD( macData.CTACRE , input.data.DATCRE.CTACRE  );
		COPYFIELD( macData.OPEAU1 , input.data.OPEAU1  );
		COPYFIELD( macData.OPEAU2 , input.data.OPEAU2  );
		COPYFIELD( macData.OPEAU3 , input.data.OPEAU3  );
		COPYFIELD( macData.OBSER1 , input.data.OBSER1  );
		COPYFIELD( macData.OBSER2 , input.data.OBSER2  );
		COPYFIELD( macData.NUMREF , input.data.NUMREF  );
		COPYFIELD( macData.NUMENV , input.data.NUMENV  );
        // ...hasta aqui. De aqui en mas, son datos propios a MAC-2
		COPYFIELD( macData.varMAC.stMAC2.NUMTRA  , input.data.NUMTRA  );		
		COPYFIELD( macData.varMAC.stMAC2.OPEDB1  , input.data.DATDEB.OPEDB1  );		
		COPYFIELD( macData.varMAC.stMAC2.OPEDB2  , input.data.DATDEB.OPEDB2  );		
		// Valor al Cobro o Enviada al Banco de Credito ?
		if( antoi(input.data.ESTADO,2) == DPS_VALOR_AL_COBRO ||
			antoi(input.data.ESTADO,2) == DPS_ENVIADA_AL_BANCO_DE_CREDITO || 
            (antoi(input.data.ESTADO,2) == DPS_EJECUTADA && input.data.RIEBCO[0] == 'N' ) )
		{
			COPYFIELD( macData.varMAC.stMAC2.ESTADO, _DPS_EJECUTADA_ );            
		}
		else 
		{
			COPYFIELD( macData.varMAC.stMAC2.ESTADO, input.data.ESTADO );            
        };

		// Segun sea MAC 2 o 3 por ESTADO y RIESGO BANCO...
		if( 2==iMAC_type )
		{
            // Se fuerza un "99" en operador-debito-1 cuando se calcula el MAC-2, no el MAC-3
            if('C'==chValidateOrCalculate && 3 != iMAC_type  )
//2008-03-17	MRB	Corregida la forma en que pisa el operador para el online.
//#if ( !defined( _SYSTEM_DATANET_BATCH_ ) )
                COPYFIELD( macData.varMAC.stMAC2.OPEDB1  , "99"  );		
//#else
//			;
//#endif // _SYSTEM_DATANET_BATCH_
			// *** Bloque de 37 bytes de longitud ***
			COPYFIELD( macData.varMAC.stMAC2.uMAC2.stMAC2_DEB.RESREG_D, "." );
            *pshBlocks = 37;
		}
		else if( 3==iMAC_type )
		{
            if('V'==chValidateOrCalculate && 3 == iMAC_type  )
//#if ( !defined( _SYSTEM_DATANET_BATCH_ ) )
                COPYFIELD( macData.varMAC.stMAC2.OPEDB1  , "99"  );		
//#else
//			;
//#endif // _SYSTEM_DATANET_BATCH_
			COPYFIELD( macData.varMAC.stMAC2.uMAC2.stMAC2_CRE.OPECR1, input.data.DATCRE.OPECR1 );
			COPYFIELD( macData.varMAC.stMAC2.uMAC2.stMAC2_CRE.OPECR2, input.data.DATCRE.OPECR2 );
            // Se fuerza un "99" en operador-credito-1 cuando se calcula el MAC-3, no el MAC-2
            if('C'==chValidateOrCalculate && 3 == iMAC_type)                                
//#if ( !defined( _SYSTEM_DATANET_BATCH_ ) )
                COPYFIELD( macData.varMAC.stMAC2.uMAC2.stMAC2_CRE.OPECR1  , "99"  );		
//#else
	//		;
//#endif	// _SYSTEM_DATANET_BATCH_
			// *** Bloque de 38 bytes de longitud ***
			COPYFIELD( macData.varMAC.stMAC2.uMAC2.stMAC2_CRE.RESREG_C, "12.BC" );
            *pshBlocks = 38;
		}
		else
        {
			// NULL por error
            *pshBlocks = 0;
            // NULL por error
			return NULL;
        };

	};

	/********************************************************/
	if (hFile && bDEBUG_PRINT_PARAMS)
	{
	  fprintf(hFile,
		"\r\n\r\nMAC DATA, NUMTRA #[%.7s]\r\n"
		"    BANDEB            [%.3s]\r\n"                      
		"    FECSOL            [%.8s]\r\n"                      
		"    NUMABO            [%.7s]\r\n"                      
		"    IMPORTE           [%.17s]\r\n" 
		"    WNOMSOL           [%.15s]\r\n"                      
		"    TIPCUE            [%.2s]\r\n"                      
		"    NUMCTA            [%.17s]\r\n"                      
		"    BANCRE            [%.3s]\r\n"                      
		"    WNOMBEN           [%.15s]\r\n"                      
		"    TIPCRE            [%.2s]\r\n"                      
		"    CTACRE            [%.17s]\r\n"                      
		"    OPEAU1            [%.2s]\r\n"                      
		"    OPEAU2            [%.2s]\r\n"                      
		"    OPEAU3            [%.2s]\r\n"                      
		"    OBSER1            [%.60s]\r\n"                      
		"    OBSER2            [%.100s]\r\n"                     
		"    NUMREF            [%.7s]\r\n"                      
		"    NUMENV            [%.3s]\r\n" ,                     
         // Desde aqui, es igual a la MAC-1....
		 input.data.NUMTRA , 
		 macData.BANDEB , 
		 macData.FECSOL , 
		 macData.NUMABO , 
		 macData.IMPORTE, 		 
         macData.WNOMSOL, 
		 macData.TIPCUE , 
		 macData.NUMCTA , 
		 macData.BANCRE , 		 
		 macData.WNOMBEN, 
		 macData.TIPCRE , 
		 macData.CTACRE , 
		 macData.OPEAU1 , 
		 macData.OPEAU2 , 
		 macData.OPEAU3 , 
		 macData.OBSER1 , 
		 macData.OBSER2 , 
		 macData.NUMREF , 
		 macData.NUMENV );

         // ...hasta aqui. De aqui en mas, son datos propios a MAC-2
	     if( 1==iMAC_type )
		 {
		  fprintf(hFile,
		    "    RESREG_1           [%.6s]\r\n"  
			,
			macData.varMAC.stMAC1.RESREG_1 );
		 }
		 else if( 2==iMAC_type )
		 {
		   fprintf(hFile,
			 "    NUMTRA            [%.7s]\r\n"  
			 "    OPEDB1            [%.2s]\r\n"  
			 "    OPEDB2            [%.2s]\r\n"  
			 "    ESTADO            [%.2s]\r\n"  
			 "    RESREG_DEBITO     [%.1s]\r\n"  
			 ,
			 macData.varMAC.stMAC2.NUMTRA  , 
			 macData.varMAC.stMAC2.OPEDB1  , 
			 macData.varMAC.stMAC2.OPEDB2  ,			
		     macData.varMAC.stMAC2.ESTADO  , 			
			 macData.varMAC.stMAC2.uMAC2.stMAC2_DEB.RESREG_D);
		 }
		 else if( 3==iMAC_type )
		 {
		   fprintf(hFile,		
			 "    OPEDB1            [%.2s]\r\n"  
			 "    OPECR1            [%.2s]\r\n"  
			 "    OPECR2            [%.2s]\r\n"  
			 "    RESREG_CREDITO    [%.5s]\r\n"  
			 ,
			 macData.varMAC.stMAC2.OPEDB1  , 
			 macData.varMAC.stMAC2.uMAC2.stMAC2_CRE.OPECR1, 
			 macData.varMAC.stMAC2.uMAC2.stMAC2_CRE.OPECR2,              
			 macData.varMAC.stMAC2.uMAC2.stMAC2_CRE.RESREG_C );
		 }
		 fclose(hFile);        
	}
	/********************************************************/

	// Copiar hacia el exterior
    shTxtLen = strlen((char *)&macData) ;
	memcpy( pszMACinputNDES, &macData, min(iLen,shTxtLen) );
	// Retornar el mismo buffer referenciado
	return pszMACinputNDES;
};
//////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Tipo de Registro
e_rectype_dps DPS_in_out::GetRecType()
{
	if( memcmp(input.data.RECTYPE,DPS_RECTYPE_CREDITO_EJECUTADO,3)==0)      // TCE
        return DPS_CREDITO_EJECUTADO;
	else if( memcmp(input.data.RECTYPE,DPS_RECTYPE_CREDITO_RESPUESTA,3)==0) // TCR
        return DPS_CREDITO_RESPUESTA;
	else if( memcmp(input.data.RECTYPE,DPS_RECTYPE_DEBITO_EJECUTADO,3)==0)  // TDE
        return DPS_DEBITO_EJECUTADO;
	else if( memcmp(input.data.RECTYPE,DPS_RECTYPE_DEBITO_RESPUESTA,3)==0)  // TDR
        return DPS_DEBITO_RESPUESTA;
    else
        return DPS_DEBITO_CREDITO_INVAL;
}
/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// Formatear mensaje de respuesta en base a mensaje de entrada
DPIFCOMM::DPIFCOMM(const char *cptrMessage, short shSize)
{   
    // Precondicion
    if(NULL != cptrMessage && shSize > 0)
	{
		FormatResponseMessage("COB"); // Inicializar con cualquier HEADER
		// Copiar en forma plana los datos
	    memcpy( &data.BUFFER, cptrMessage, min(shSize,sizeof(data.BUFFER)));
	}
}

//////////////////////////////////////////////////////////////////
// Formatear mensaje de respuesta en base a mensaje de entrada
boolean_t DPIFCOMM::FormatResponseMessage(const char *cszRECTYPE)
{   
    // Precondicion
    if(NULL == cszRECTYPE)
        return is_false;

	// Formatear el mensaje saliente a partir del mensaje entrante autorizado
	// Para informacion de los contenidos de los campos , ver documentacion
	// interna del BANSUD-MACRO sobre el proyecto DATANET
    memcpy(data.header.TPNAME,DPS_TPNAME_DPRE   ,4);//Nombre del programa autorizante "DPRE"
    memcpy(data.header.NDNAME,DPS_NDNAME_DEFAULT,4);	//Nombre nodo receptor	ej."BSUD" : Bansud    
    memset(data.header.HORA, 0x00, 4); //TimeStamp local "1" en PACKED-DECIMAL
    data.header.HORA[0] = 0x00;
    data.header.HORA[1] = 0x00;
    data.header.HORA[2] = 0x00;
    data.header.HORA[3] = 0x1C;
    memset(data.header.SECUENCIA, 0x00, 2); //Secuencia local "1" en PACKED-DECIMAL

    // Resto del mensaje
    data.header.SECUENCIA[0] = 0x00;            //Secuencia local "1" en PACKED-DECIMAL
    data.header.SECUENCIA[1] = 0x1C;            //Que en hexadecimal es 001C.
    // Se invierten nodos receptor-emisor
	memcpy(data.FRNAME	,DPS_NDNAME_DEFAULT, 4);//Nombre nodo emisor	ej."DNET" : Datanet
	memcpy(data.TONAME	,DPS_NDNAME_DATANET, 4);//Nombre nodo receptor	ej."BSUD" : Bansud    
	memcpy(data.RECTYPE	,cszRECTYPE, 3);	    //Tipo de registro	CTA = cuentas
	data.ACTIVITY[0]    = 'N';                  //Actividad	"N"
    // Longitud del registro	145=CTA,235=COB,214=PGO,281=CCB,258=CCP
    if(strcmp(cszRECTYPE,"CTA")==0)
    {
	    data.COUNTER[0] = 0x00; //Longitud del registro	145 (X'0091')
        data.COUNTER[1] = 0x6A;	//Longitud en EBCDIC traducido es '6A'
    }
    else if(strcmp(cszRECTYPE,"COB")==0)
    {
	    data.COUNTER[0] = 0x00; //Longitud del registro	235 (X'00EB')
        data.COUNTER[1] = 0xF5;	//Longitud en EBCDIC traducido es 'F5'
    }
    else if(strcmp(cszRECTYPE,"PGO")==0)
    {
	    data.COUNTER[0] = 0x00; //Longitud del registro	214 (X'00D6')
        data.COUNTER[1] = 0x4F;	//Longitud en EBCDIC traducido es '4F'
    }
    else if(strcmp(cszRECTYPE,"CCB")==0)
    {
	    data.COUNTER[0] = 0x01; //Longitud del registro	281 (X'0119')
        data.COUNTER[1] = 0x19;	//Longitud en EBCDIC traducido es '1' y '19'
    }
    else if(strcmp(cszRECTYPE,"CCP")==0)
    {
	    data.COUNTER[0] = 0x01; //Longitud del registro	281 (X'0102')
        data.COUNTER[1] = 0x02;	//Longitud en EBCDIC traducido es '1' y '02'
    }
    else 
    {
	    data.COUNTER[0] = 0x00; //Longitud del registro	145 (X'0091')
        data.COUNTER[1] = 0x6A;	//Longitud en EBCDIC traducido es '6A'
    };
    // Postcondicion : siempre Ok
	return is_true;
}
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
boolean_t DPS_in_out::IsRefreshMovement()
{
    boolean_t bIsRefresh = is_false;
	// Es CTA,PGO,COB del refresh online?
	bIsRefresh = (memcmp(input.data.RECTYPE,DPS_RECTYPE_CUENTAS_CTA,3)==0
                ||
                memcmp(input.data.RECTYPE,DPS_RECTYPE_COBRANZAS_COB,3)==0
                ||
                memcmp(input.data.RECTYPE,DPS_RECTYPE_PAGOS_PGO,3)==0
                ||
                memcmp(input.data.RECTYPE,DPS_RECTYPE_COBRANZAS_CCB,3)==0
                ||
                memcmp(input.data.RECTYPE,DPS_RECTYPE_PAGOS_CCP,3)==0
            )
		    ? is_true
		    : is_false;
    if(bIsRefresh)
        return bIsRefresh;
	// Es CTA,PGO,COB del refresh online?
	else
        return (memcmp(output.data.RECTYPE,DPS_RECTYPE_CUENTAS_CTA,3)==0
                ||
                memcmp(output.data.RECTYPE,DPS_RECTYPE_COBRANZAS_COB,3)==0
                ||
                memcmp(output.data.RECTYPE,DPS_RECTYPE_PAGOS_PGO,3)==0
                ||
                memcmp(output.data.RECTYPE,DPS_RECTYPE_COBRANZAS_CCB,3)==0
                ||
                memcmp(output.data.RECTYPE,DPS_RECTYPE_PAGOS_CCP,3)==0
             )
		    ? is_true
		    : is_false;
}
//////////////////////////////////////////////////////////////////


/*****************************************************************************/
/* ReadString - Get a line of text from the config file.                     */
/*****************************************************************************/
int DPS_in_out::ReadString(char * lpValueName, char * lpData, int maxlen)
{
   char       buffer[200]={0x00};
   char      *ptr     = NULL;
   FILE      *hLog  = NULL;
   BOOL       match = FALSE;
   BOOL       eof   = FALSE;
   int        rc = 0;
   int        ch = 0;
   int        i = 0;
   BOOL       gotdata = FALSE;
   char       separators[] = " =\t\n";
   char       szCfgFile[] =  ".\\datanet.cfg";

   /* precondition */
   hLog = fopen(szCfgFile, "r");
   if (hLog == NULL)
       return 0;

   if (hLog != NULL)
   {
      while ((!match) && (!eof))
      {
         /********************************************************************/
         /* Use fgetc to read a line of text from the file.                  */
         /********************************************************************/
         for (i=0; (i<sizeof(buffer))     &&
                   ((ch=getc(hLog)) != EOF)  &&
                   ((char)ch != '\n');
                                      i++)
         {
            buffer[i] = (char)ch;
         }
         if ((char)ch == '\n')
         {
            buffer[i++] = (char)ch;
         }
         if (ch == EOF)
         {
            eof = TRUE;
         }
         else
         {
            /*****************************************************************/
            /* Compare the 1st token in the line read with the requested     */
            /* param.                                                        */
            /*****************************************************************/
            if (!strcmpi(strupr(strtok(buffer, separators)), lpValueName))
            {
               match = TRUE;
               /**************************************************************/
               /* Get a pointer to the 2nd token (the value we want)         */
               /**************************************************************/
               ptr = strtok(NULL, separators);

               /**************************************************************/
               /* Copy the data IF there is some.                            */
               /**************************************************************/
               if (ptr != NULL)
               {
                  /***********************************************************/
                  /* Force a NULL after the 2nn token                        */
                  /***********************************************************/
                  strtok(NULL, separators);

                  /***********************************************************/
                  /* Copy the data                                           */
                  /***********************************************************/
                  strncpy(lpData, ptr, maxlen);
                  gotdata = TRUE;
               }
               else
               {
                  gotdata = FALSE;
               }
            }
         }
      }

      if (gotdata)
      {
         rc = 1;
      }

      fclose(hLog);

   }
   /* return RespCde */
   return(rc);
}
//////////////////////////////////////////////////////////////////
