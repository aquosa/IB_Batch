//
// IT24 Sistemas S.A.
// ISO-8583.0.1987 Propietary Message Class
//
// Clase Mensaje Financiero ISO-8583 propietaria. (Version 0, 1987)
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1997.09.23      mdc     Base 24 Release 4
// (Inicial)    1997.09.29      mdc     Extensiones
// (Inicial)    1997.10.08      mdc     Extensiones de Usuario RED LINK, BANELCO
// (Inicial)    1997.10.15      mdc     Recalcular Offsets dinamicamente
// (Inicial)    1997.10.28      mdc     Tipo de Campo (0=Fijo,1,2,3=Variable)
// (Inicial)    1997.10.30      mdc     Defaults de Campos
// (Inicial)    1997.10.31      mdc     Longitud de mensaje
// (Inicial)    1997.11.06      mdc     Extensiones de Funcionalidad
// (Inicial)    1997.11.07      mdc     Extensiones de Tipos de Mensaje
// (Inicial)    1997.11.18      mdc     Tipo de Mensaje de respuesta
// (Inicial)    1998.01.19      mdc     Codigo de Respuestas ISO predefinidos en header
// (Inicial)    1998.06.22      mdc     Reemplazo de CHAR[] por BYTE[], e
//                                      inclusion de typedefs.h        
// (Alfa)       1998.06.24      mdc     ISO-MsgTypes en defines
// (Beta)       1998.07.23      mdc     CISO_NOT_AUTHORIZED_TERM="12"
// (Beta)       1998.07.31      mdc     BitMapList para cada TranTyp ahora parametrizable.
// (Beta)       1998.09.03      mdc     Buffer en 4 kbytes
// (Beta)       1999.01.06      mdc     Header <bitmapl.h>
// (Beta)       2000.02.15      mdc     CISOMESSAGESIZE de 4096 a 8192
// (Beta)       2002.08.05      mdc     EXPORT int ISO_Test(BYTE *bBuffer, int iBuffer, bool bFields, FILE* fOutput);
// (Beta)       2002.09.05      mdc     #define _B24_CURRENCY_LECOP_ 996  
// (Beta)       2002.11.11      mdc     FREE-AVAILABILITY ACCOUNT TYPE (ARGENTINA) 
// (Beta)       2002.11.25      mdc     CISO_REV_TIMEOUT_ADVICE=168
// (Beta)       2003.01.21      mdc     PUBLIC : EXPORT BOOL SaveAllBitmapsDescriptions(PSTR);    // Descriptor de todos los Bitmaps
// (Beta)       2003.05.13      mdc     PUBLIC : EXPORT BOOL GetHeader()
// (Delta)      2005.07.18      mdc     ACCOUNT TYPES DEPENDING ON ACI-BASE24 RELEASE = 4,5, or 6
//

#ifndef _ISO8583_H_
#define _ISO8583_H_

// Headers propios
// Header Type Definitions
#include <qusrinc/typedefs.h>
// Aliases
#include <qusrinc/alias.h>
// Bitmap Strings List
#include <qusrinc/bitmapl.h>  
// Field ISO-8583
#include <qusrinc/field.h>
// Std. IO
#include <stdio.h>


//////////////////////////////////////////////////////////////////////////////////////////
// Constante Maximo de Tamano del Mensaje ISO-8583 soportado
#define CISOMESSAGESIZE   (8192)
// Constante de Maximo de Campos dentro del Mensaje
#define CISOFIELDS        (128)
// Version de ACI BASE24 que se utiliza : RELEASE 4, 5 o 6
#define _BASE24_RELEASE4_			(4)
#define _BASE24_RELEASE5_			(5)
#define _BASE24_RELEASE6_			(6)
#define _BASE24_CURRENT_VERSION_	_BASE24_RELEASE4_	



#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )
//////////////////////////////////////////////////////////////////////////////////////////
// Lista de Codigos de Respuesta ISO vs. B24 de Transascciones Aprobadas:
// Codigos de Respuesta ISO vs B24:	ISO  =  B24 Descripcion ampliada 
#define CISO_APPROVED				"00" //	00	Aprobada con saldo disponible para Deposito,
#define CISO_APPROVED_I				  0
#define CISO_APPROVED_VIP           "01" //	01	Aprobada sin saldo disponible para Movimiento 
										 //		de Fondos/Pagos/Mensaje al Banco.Aprobada VIP.
#define CISO_APPROVED_VIP_I           1	
// Lista de Codigos de Respuesta ISO vs. B24 de Transascciones no Aprobadas:
// Atencion: Multiples codigos ISO pueden corresponderse con un solo codigo B24...
// Codigos de Respuesta ISO vs B24:	ISO  =  B24 Descripcion ampliada 
#define CISO_NOT_ALLOWED_CARD		"62" 
#define CISO_RESTRICTED_CARD        "36"
#define CISO_LOST_CARD				"41"
#define CISO_STOLEN_CARD  			"43"
#define CISO_INVALID_TRAN_CARD		"57" //	50	Uso no autorizado de la tarjeta en ciertas 
										 //		operaciones.Tarjeta restringida, capturada.
										 //		Tarjeta perdida o robada, capturada.
										 //		Tarjeta perdida o robada, capturada.
										 //		Transaccion invalida para la tarjeta.
#define CISO_OUT_OF_DATE			"54"
#define CISO_OUT_OF_DATE_CAPT		"33" //	51	Tarjeta vencida detectada por el autorizador.
										 //		Tarjeta vencida capturada.
#define CISO_CARD_NOT_IN_FILE	 	"14"
#define CISO_NOT_REGISTERED_CARD	"56" //	52	Tarjeta erronea, no se halla en el Cardholder-
										 //		Authorization-File.Sin registro de la Tarjeta.
#define CISO_INVALID_PIN			"55" //	53	La clave ingresada (PIN) es incorrecta.
#define CISO_DATABASE_ERROR			"89" //	54	Error de Base de Datos. No puede procesarse la 
										 //		operacion por:1. No se encuentra la institucion
										 //		o sus datos son erroneos.2. La operacion no 
										 //		esta habilitada.3. Transaction-Logging-File 
										 //		completo.4. No existe adhesion previa al Ente 
										 //		en P.A.S. (ver codigos 80,81,82).5. No existe 
										 //		partida del Ente en P.A.S.6. Imposible armar 
										 //		Respuesta completa en P.A.S.
#define CISO_INVALID_TRANSACTION  	"12"
#define CISO_INVALID_TRANSACTION_I	 12
#define CISO_TRANSACTION_NOT_SUPP   "40" //	55	Operacion erronea por:1. Tarjeta Administrativa
										 //		operando en Cajero que no corresponde.
										 //		2. Operacion no soportada por el Sistema.
										 //		3. Transaccion irreconocible en P.A.S.
										 //		4. Funcion no soportada.
#define CISO_TRANSACTION_NOT_SUPP_I  40 
#define CISO_INVALID_ACCOUNT  		"76"
#define CISO_NO_ACCOUNT_AVAILABLE	"39"
#define CISO_NO_CREDIT_ACCOUNT		"42"
#define CISO_NO_INVSTMENT_ACCOUNT	"44"
#define CISO_NO_CHECKINGS_ACCOUNT	"52"
#define CISO_NO_SAVINGS_ACCOUNT		"53" //	56	Cuenta invalida por:1. El proceso autorizador 
										 //		no puede obtener cuentas validas o cuentas 
										 //		duplicadas.2. El proceso de autorizacion no 
										 //		puede obtener todos los registros del Positive-
										 //		Balance-File.3. Cuenta no asociada a la Tarjeta
										 //		.Sin Cuenta de Credito.Sin Cuenta Universal.
										 //		Sin Cuenta de Inversion.Sin Cuenta Corriente.
										 //		Sin Caja de Ahorros.
#define CISO_NOT_AUTHORIZED_ATM		"12"
#define CISO_NOT_AUTHORIZED_TERM	"12"
#define CISO_NOT_AUTHORIZED_CARD	"58" //	57	No autorizado por:1. La Institucion a cargo 
										 //		del Cajero no lo permite.2. No lo permite la 
										 //		Institucion que emite la Tarjeta.3. No esta 
										 //		asociado el Ente a la Institucion en P.A.S.
										 //		4. Transaccion invalida para la Terminal.
#define CISO_INSUFFICIENT_FOUNDS	"51" //	58	Fondos insuficientes, menores/iguales a cero.
										 //		Monto disponible no retornado por el Host.
										 //	59	Fondos insuficientes, mayores a cero pero 
										 //		menores al de la transaccion.
#define CISO_WITHDRAWAL_LIMIT_EXC	"65" //	60	No son permitidas mas extracciones, se ha 
										 //		llegado al limite diario prefijado.
#define CISO_AMMOUNT_LIMIT_EXC		"69" //	61	Se excederia el limite del monto prefijado 
										 //		para la Red o la institucion,  
										 //		debe reingresarlo.
#define CISO_PIN_TRIES_EXCEDDED  	"75"
#define CISO_PIN_TRIES_EXC_CAPT		"38" //	62	Se excede el limite de intentos de ingreso 
										 //		de la clave prefijado para el dia en curso.
										 //		Exceso de ingreso de PIN, capturada.
#define CISO_WITHD_AMMOUNT_LIMIT	"61" //	63	Limite de monto diario de extraccion alcanzado,
										 //		no puede extraer mas.
#define CISO_INVALID_AMMOUNT		"13" //	64	Monto invalido, menor al minimo prefijado o no
										 //		es multiplo de los minimos prefijados.
#define CISO_NO_ACCOUNT_INFO		"86" // 65	Sin informacion para la cuenta solicitada, 
										 //		en Ultimos Movimientos.
#define CISO_NO_CARD_INFO			"87" //	66	Sin informacion de estado de la tarjeta en 
										 //		una consulta.
#define CISO_INVALID_CBACK_AMMOUNT	"13" //	67	Monto de Reintegro invalido.
#define CISO_EXTERNAL_DECLINE		"05" //	68	Declinacion del Host o Switch externo. Otro error.
#define CISO_EXTERNAL_DECLINE_I		  5
#define CISO_GENERIC_ERROR			"06" //	68	Declinacion del Host o Switch externo. 
										 //		Todo otro error no contemplado en esta tabla.
#define CISO_GENERIC_ERROR_I		  6
#define CISO_INVALID_SHARING_GROUP	"77" //	69	Distinto codigo de grupo para operar. 
										 //		Sin compartir grupo/area.
#define CISO_SYSTEM_ERROR  			"88"
#define CISO_SYSTEM_ERROR_I			 88
#define CISO_SYSTEM_FAILURE 		"96" //	70	Error del Sistema, intentar repetir la 
										 //		operacion por un error temporario.
										 //		Falla del Sistema.
#define CISO_SYSTEM_FAILURE_I		 96
#define CISO_ATM_NOT_ALLOWED_ISSUER	"78"
#define CISO_CONTACT_ISSUER_1		"01"
#define CISO_CONTACT_ISSUER_2		"02" //	71	Error del Sistema, comunicarse con el Emisor, 
										 //		pque no hay acuerdo de utilizacion del cajero.
										 //		Referirse a Emisor de Tarjeta.Referirse a 
										 //		Emisor de Tarjeta.
#define CISO_CUTOVER_IN_PROGRESS	"90" //	72	Error del Sistema, destino no esta disponible.
										 //		Corte de Red en progreso.
#define CISO_CUTOVER_IN_PROGRESS_I	 90 //	72	Error del Sistema, destino no esta disponible.
										 //		Corte de Red en progreso.
#define CISO_INVALID_DESTINY		"92" //
#define CISO_NOT_SUPPORTED_INST		"31" //	73	Error del Sistema, destino invalido, no se 
										 //		encuentra, ni puede rutearse.Institucion no 
										 //		soportada.
#define CISO_INVALID_FORMAT			"30"
#define CISO_INVALID_FORMAT_I		 30
#define CISO_INVALID_TRAN_AMMOUNT	"13" //	74	Error del Sistema, en la edicion/formato del 
										 //		mensaje.Monto invalido
#define CISO_MONTH_WITHD_LIMIT_EXC	"45" //	76	Excede limite mensual de extracciones.
#define CISO_PAS_INVALID_SERV_CODE	"80" //	80	Codigo de Servicio /Numero de Usuario invalido 
										 //		en Partida de P.A.S.
#define CISO_PAS_INVALID_REF_CODE	"81" //	81	Partida ya pagada, Deuda ya ha sido cancelada 
										 //		en P.A.S.
#define CISO_PAS_INVALID_USER_CODE	"82" //	82	Numero de Usuario invalido en Partida de P.A.S.
#define CISO_ADM_APPROVED			"79" //	90	Administrativa aprobada en horario.
#define CISO_ADM_APPROVED_OUT_TIME	"80" //	91	Administrativa aprobada fuera de horario.
#define CISO_ADM_APPROVED_TERMINAL	"81" //	92	Administrativa aprobada generada en terminales
#define CISO_HBANKING_DENIED		"47" //	xx	Usuario y DNI no coinciden en HomeBanking
#define CISO_DESTINY_NOT_AVAILABLE		  "91" 
#define CISO_DESTINY_NOT_AVAILABLE_I	   91
#endif // ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE6_ )

///2005-08-31//////////////////////////////////////////////////////////////////////
// REVERSAL CODES BASE24 RELEASE 4? RELEASE 6?
#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE6_ )
#define CISO_APPROVED					  "00"
#define CISO_APPROVED_I					   0
#define CISO_APPROVED_VIP				   "01" 
#define CISO_APPROVED_VIP_I					1	
#define CISO_EXTERNAL_DECLINE			  "05" 
#define CISO_EXTERNAL_DECLINE_I			    5
#define CISO_INVALID_TRANSACTION  		  "12"
#define CISO_INVALID_TRANSACTION_I		   12
#define CISO_INVALID_FORMAT				  "30"
#define CISO_INVALID_FORMAT_I			   30
#define CISO_TRANSACTION_NOT_SUPP		  "40" 
#define CISO_TRANSACTION_NOT_SUPP_I		   40 
#define CISO_NOT_ENOUGH_AVAIL_ZERO        "58"  
#define CISO_NOT_ENOUGH_AVAIL_ABOVE_ZERO  "59"  
#define CISO_INSUFFICIENT_FOUNDS		  CISO_NOT_ENOUGH_AVAIL_ZERO
#define CISO_WITHDRAWAL_LIMIT_EXC		  CISO_NOT_ENOUGH_AVAIL_ABOVE_ZERO
#define CISO_INVALID_ACCOUNT  			  "76"
#define CISO_NON_EXISTENT_ACCOUNT 		  "56"
#define CISO_NO_ACCOUNT_AVAILABLE		  CISO_NON_EXISTENT_ACCOUNT
#define CISO_NO_CHECKINGS_ACCOUNT		  CISO_NON_EXISTENT_ACCOUNT
#define CISO_NO_SAVINGS_ACCOUNT			  CISO_NON_EXISTENT_ACCOUNT
#define CISO_SYSTEM_ERROR  				  "88"
#define CISO_SYSTEM_ERROR_I				   88
#define CISO_SYSTEM_FAILURE				  "96"
#define CISO_SYSTEM_FAILURE_I			   96
#define CISO_CUTOVER_IN_PROGRESS		  "90" 
#define CISO_CUTOVER_IN_PROGRESS_I		   90 
#define CISO_DESTINY_NOT_AVAILABLE		  "98" 
#define CISO_DESTINY_NOT_AVAILABLE_I	   98
#endif // ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE6_ )
///2005-08-31//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Tipos de Mensajes ISO-8583, version 0, 1987
#define CISO_MSGTYPE_FINANCIAL_RQST     "0200"  // Financial Transactions
#define CISO_MSGTYPE_FINANCIAL_RESP     "0210"
#define CISO_MSGTYPE_FINANCIAL_ADV      "0220"
#define CISO_MSGTYPE_FINANCIAL_ADV_REP  "0221"
#define CISO_MSGTYPE_FINANCIAL_ACK      "0230"

#define CISO_MSGTYPE_STMT_PRINT_RQST    "0205" // Statement Prints
#define CISO_MSGTYPE_STMT_PRINT_RESP    "0215"

#define CISO_MSGTYPE_REVERSAL           "0420" // Reversals
#define CISO_MSGTYPE_REVERSAL_REP       "0421" 
#define CISO_MSGTYPE_REVERSAL_ACK       "0430" 

#define CISO_MSGTYPE_SAF_ADV            "0220" // SAF
#define CISO_MSGTYPE_SAF_ADV_REP        "0221"
#define CISO_MSGTYPE_SAF_ACK            "0230"

#define CISO_MSGTYPE_NETMGMT_RQST       "0800" // Network Management
#define CISO_MSGTYPE_NETMGMT_RESP       "0810"

#define CISO_MSGTYPE_ADJUSTEMENT        "5400" // Miscelaneous: Adjustement
#define CISO_MSGTYPE_FINAN_RQST_ERR     "9200" // Financial Transactions Error
#define CISO_MSGTYPE_FINAN_RESP_ERR     "9210" 
#define CISO_MSGTYPE_MULT_ACCNT_SEL     "9906" // Multiple Account Selection
#define CISO_MSGTYPE_DEVICE_CONTROL     "9980" // Device Control



//////////////////////////////////////////////////////////////////////////////////////////
// ISO Test prototype
EXPORT int ISO_Test(BYTE *bBuffer, int iBuffer, bool bFields, char* szOutFile, bool bTruncate);
// Configuration file
#define _ISO8583_CFG_FILE_	"ISO8583.CF"
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////										   
// Clase Mensaje ISO-8583
//////////////////////////////////////////////////////////////////////////////////////////
class ISO8583MSG
    {
    public:
        
        // Metodos publicos de la Clase        
		EXPORT ISO8583MSG(void);						// Constructor default
        EXPORT ISO8583MSG(PBYTE,WORD,PSTR=NULL);		// Constructor opcional
        EXPORT ~ISO8583MSG(void);                       // Destructor

		EXPORT BOOL InitMessage(void);					// Iniciar mensaje en cero campos
		EXPORT BOOL DeleteField(WORD);                  // Borrar Campo
		EXPORT BOOL Export(PBYTE,PWORD);                // Exportador
		EXPORT BOOL GetField(WORD,PWORD,PBYTE,WORD);    // Valor de Campo
		EXPORT BOOL GetMsgTypeId(PBYTE,WORD);           // Id de Msg
		EXPORT BOOL Import(const PBYTE,WORD);           // Importador
		EXPORT BOOL InsertField(WORD,PWORD,PBYTE,WORD); // Insercion de Campo
		EXPORT BOOL IsNetworkMgmtRequest(void);			// NetRqst?
		EXPORT BOOL IsNetworkMgmtResponse(void);		// NetRsp?
		EXPORT BOOL IsValidMessage(void);               // Es Mensaje valido?
		EXPORT BOOL IsValidField(WORD);                 // Es Campo valido?		
		EXPORT WORD MessageLength(void);			    // Longitud mensaje		
		EXPORT BOOL SetField(WORD,PWORD,PBYTE,WORD);    // Valor a Campo		
		EXPORT BOOL SetMsgTypeId(PBYTE,WORD);           // Id de Msg		
		EXPORT BOOL SetMsgTypeIdResponse(void);         // Id Respuesta de Msg		
		EXPORT void GetBitmaps(char chBistmap1[],char chBitmap2[]);		
		
		//////////////////////////////////////////////////////////////////////////////////////
		EXPORT BOOL GetAllBitmapsDescriptions(PSTR);     // Descriptor de todos los Bitmaps
		EXPORT BOOL SaveAllBitmapsDescriptions(PSTR);    // Descriptor de todos los Bitmaps
		EXPORT BITMAPLIST * BitmapListReference(PCSTR);  // Recupera lista de BITMAPS determinada
		EXPORT BOOL GetHeader(char chHeader[], short nLen);	// Recuperar Header
		//////////////////////////////////////////////////////////////////////////////////////
        
    protected:
    
        // Metodos protegidos de la Clase
		EXPORT BOOL DefaultBitmapsDescriptions(void);// Defaults bitmaps
		EXPORT BOOL DefaultFieldsDescriptions(void); // Defaults fields
		EXPORT WORD FieldOffset(WORD);              // Desplazamiento Campo
		EXPORT WORD FieldLength(WORD);              // Longitud Campo
		EXPORT WORD FieldType(WORD);                // Tipo Campo				
		EXPORT BOOL GetFieldsDescription(PSTR);     // Descriptor de Campos
		EXPORT BOOL GetImplicitLength(WORD,PWORD);  // Longitud implicita		
		EXPORT BOOL IsValidHeader(void);            // Header?
		EXPORT BOOL IsValidTransactionType(void);   // Transaction Type?
		EXPORT BOOL IsValidMsgTypeId(PBYTE=NULL);   // Msg Type Id?
		
		///////////////////////////////////////////////////////////////////
		EXPORT BOOL GetBitmapDescription(PSTR,PSTR);// Descriptor de Bitmaps
		EXPORT BOOL SaveBitmapDescription(PSTR,PSTR);// Descriptor de Bitmaps
		///////////////////////////////////////////////////////////////////

		
		virtual EXPORT BOOL IsValidUserData(void); // User Data?
	    
		virtual EXPORT BOOL IsValid200(void);  // Financial Trx Request?
		virtual EXPORT BOOL IsValid210(void);  // Financial Trx Response?
		virtual EXPORT BOOL IsValid220(void);  // Financial Trx Completion?/SAF?
		virtual EXPORT BOOL IsValid221(void);  // Financial Trx Dup Completion?/SAF repeat?
		virtual EXPORT BOOL IsValid230(void);  // Financial Trx Completion Ack?/SAF Ack?

		virtual EXPORT BOOL IsValid205(void);  // Statement Print Request?
		virtual EXPORT BOOL IsValid215(void);  // Statement Print Response?
		
		virtual EXPORT BOOL IsValid420(void);  // Reversal?
		virtual EXPORT BOOL IsValid421(void);  // Reversal Repeat?
		virtual EXPORT BOOL IsValid430(void);  // Reversal Ack?

		virtual EXPORT BOOL IsValid800(void);  // Network Management?
		virtual EXPORT BOOL IsValid810(void);  // Network Management Response?

		EXPORT BOOL MakeValidField(BYTE [],WORD);  // Hacer Campo valido
		EXPORT BOOL MakeInvalidField(BYTE [],WORD);// Hacer Campo invalido
		EXPORT BOOL RecalculateOffsets(void);      // Recalcular Offsets  

		// Variables protegidas de la Instancia (para acceso x/subclases)
        BYTE		chMsg[CISOMESSAGESIZE*10]; // Buffer de Mensaje ISO
		WORD		cbMsgLen;                  // Longitud del Buffer		
		// Buffer auxiliar de copia 
		BYTE		chAuxMsg[CISOMESSAGESIZE*10];
		WORD		cbAuxMsgLen;
		BOOL		bCheckMessage;             // Verificacion habilitada?
		
	private:
        
		// Variables privadas de la Instancia
		FIELD       fdFields[CISOFIELDS],    // Descripcion de Campos en Mensaje
		     		fdFieldsBkp[CISOFIELDS]; // Backup original de Campos
		// Lista privada de Bitmaps para cada tipo de transaccion {200,205,210,...}
		BITMAPLIST	bml200,					 // Bit Map List 200
					bml205,					 // Bit Map List 205
					bml210,					 // Bit Map List 210
					bml215,					 // Bit Map List 215
					bml220,					 // Bit Map List 220
					bml230,					 // Bit Map List 230
					bml420,					 // Bit Map List 420
					bml430,					 // Bit Map List 430
					bml800,					 // Bit Map List 800
					bml810;					 // Bit Map List 810
    };

//////////////////////////////////////////////////////////////////////////////////////////

#endif // end-ifdef-ISO8583_H

