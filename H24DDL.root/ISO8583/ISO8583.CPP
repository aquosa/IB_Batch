/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// ISO-8583-Message Propietary Library
// 
//
//   Libreria Propietaria de Mensajes ISO-8583.
//   Propietaria implica que, como es de esperar de un estandard, se 
// respeta el formato, pero... se ajustan los mensajes a las necesidades 
// de B24-HISO.
//   Entonces, esta es la base que verifica los mensajes ISO-8583-B24
// hasta el Secondary BitMap inclusive, en forma parametrica, a traves de un profile
// o bien con valores por default. Esta tecnica utiliza una lista de strings de bitmaps
// para cada tipo de transaccion y verifica contra los bitmaps informados en el mensaje.
//   El modulo se basa en B24-HISO Release 4, en cuanto a Type, Offset y Length 
// de campos ISO8583, y la forma parametrica de cargar los bitmaps externamente (EMF). 
//   (*) Adaptados los BITMAPS especialmente por omision , para conexion con RED LINK S.A.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1997.09.23      mdc     Base
// (Inicial)    1997.09.26      mdc     Base
// (Inicial)    1997.09.29      mdc     Extensiones Headers/TrxCode/Bitmap
// (Inicial)    1997.10.08      mdc     Extensiones Headers/TrxCode/Bitmap
// (Inicial)    1997.10.15      mdc     Recalculo de Offsets dinamicamente
// (Inicial)    1997.10.28      mdc     Tipo de Campo (0=Fijo,1,2,3=Variable)
// (Inicial)    1997.10.30      mdc     Extensiones Headers/TrxCode/Bitmap
// (Inicial)    1997.10.31      mdc     Longitud de mensaje
// (Inicial)    1997.11.06      mdc     Extensiones de Funcionalidad en Campos
// (Inicial)    1997.11.07      mdc     Extensiones de Tipos de Mensaje
// (Inicial)    1997.11.18      mdc     Establecer Codigo de Respuesta de Mensaje
// (Inicial)    1997.12.16      mdc     Chequeos fuertes de rangos de copia en InsertField()
// (Inicial)    1998.04.01      mdc     Aportes voluntarios AFJP=Bitmaps validos
// (Inicial)    1998.06.22      mdc     Reemplazo de CHAR[] por BYTE[], e inclusion de typedefs.h        
// (Inicial)    1998.06.23      mdc     Ultimos Movimientos=Bitmaps validos
// (Inicial)    1998.06.24      mdc     ISO-MsgTypes en defines
// (Alfa)       1998.06.25      mdc     Bitmaps ULTMOV corregidos
// (Beta)       1998.07.02      mdc     Bitmaps AFJP corregidos
// (Beta)       1998.07.31      mdc     BitMapList para cada TranTyp ahora parametrizable (SOAT)
// (Beta)       1998.08.06      mdc     Agrega BitMaps harcodeados del SOAT-NT
// (Beta)       1998.09.03      mdc     Buffer expandido a 4096 kbytes y agregado Tipo de Campo 4
// (Beta)       1998.10.15      mdc     Longitud campo #126 en 4 digitos
// (Beta)       1998.12.02      mdc     Exceso de copia en campo #126
// (Beta)       1998.12.23      mdc     Nulificacion de buffer antes de copiar datos externos
// (Beta)       1999.06.10      mdc     GetBitmapDescription() retorno dependiente de parametros.
// (Beta)       1999.07.07      mdc     Precondicion: Verificacion habilitada? : Parametrizable en .INI
// (Beta)       2000.02.15      mdc     Parametrizacion en archivo independinte de plataforma (.TXT)
// (Release)	2001.02.14		mdc		::InitMessage() con "Product-Id"="02" = POS
// (Release)    2001.06.27      mdc     Longitud campo #56 en 4 digitos
// (Release)    2005.08.24      mdc     _SYSTEM_ATM_ = _SYSTEM_BASE_
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
/*              Base24 4.0 ISO-8583 FIELD NUMBER DEFINITIONS         */
/*      position    type         offset and length                   */
///////////////////////////////////////////////////////////////////////
/*         1,        0,      primary^field( secndry^bit^map     ),
           2,        2,      primary^field( pan                 ),
           3,        0,      primary^field( proc^cde            ),
           4,        0,      primary^field( tran^amt            ),
           5,        0,      primary^field( setl^amt            ),
           6,        0,      primary^field( bill^amt            ),
           7,        0,      primary^field( xmit^dat^tim        ),
           8,        0,      primary^field( bill^fee            ),
           9,        0,      primary^field( setl^conv^rat       ),
          10,        0,      primary^field( bill^conv^rat       ),
          11,        0,      primary^field( trace^num           ),
          12,        0,      primary^field( tran^tim            ),
          13,        0,      primary^field( tran^dat            ),
          14,        0,      primary^field( exp^dat             ),
          15,        0,      primary^field( setl^dat            ),
          16,        0,      primary^field( conv^dat            ),
          17,        0,      primary^field( cap^dat             ),
          18,        0,      primary^field( mrcht^typ^cde       ),
          19,        0,      primary^field( acq^inst^cntry^cde  ),
          20,        0,      primary^field( pan^ext^cntry^cde   ),
          21,        0,      primary^field( frwd^inst^cntry^cde ),
          22,        0,      primary^field( entry^mde           ),
          23,        0,      primary^field( mbr^num             ),
          24,        0,      primary^field( netw^intl^id        ),
          25,        0,      primary^field( pt^tran^spcl^cde    ),
          26,        0,      primary^field( pos^pin^capture^cde ),
          27,        0,      primary^field( auth^id^resp^len    ),
          28,        0,      primary^field( tran^fee            ),
          29,        0,      primary^field( setl^fee            ),
          30,        0,      primary^field( tran^proc^fee       ),
          31,        0,      primary^field( setl^proc^fee       ),
          32,        2,      primary^field( acq^inst^id         ),
          33,        2,      primary^field( frwd^inst^id        ),
          34,        2,      primary^field( pan^extnd           ),
          35,        2,      primary^field( track2              ),
          36,        3,      primary^field( track3              ),
          37,        0,      primary^field( retrvl^ref^num      ),
          38,        0,      primary^field( auth^id^resp        ),
          39,        0,      primary^field( resp^cde            ),
          40,        0,      primary^field( service^cde         ),
          41,        0,      primary^field( term^id             ),
          42,        0,      primary^field( crd^accpt^id^cde    ),
          43,        0,      primary^field( crd^accpt^name^loc  ),
          44,        2,      primary^field( resp^data           ),
          45,        2,      primary^field( track1              ),
          46,        3,      primary^field( add^data^iso        ),
          47,        3,      primary^field( add^data^natl       ),
          48,        3,      primary^field( add^data^prvt       ),
          49,        0,      primary^field( crncy^cde           ),
          50,        0,      primary^field( setl^crncy          ),
          51,        0,      primary^field( bill^crncy          ),
          52,        0,      primary^field( pin                 ),
          53,        0,      primary^field( sec^cntrl^info      ),
          54,        3,      primary^field( add^amts            ),
          55,        3,      primary^field( pri^rsrvd1^iso      ),
          56,        3,      primary^field( pri^rsrvd2^iso      ),
          57,        3,      primary^field( pri^rsrvd1^natl     ),
          58,        3,      primary^field( pri^rsrvd2^natl     ),
          59,        3,      primary^field( pri^rsrvd3^natl     ),
          60,        3,      primary^field( pri^rsrvd1^prvt     ),
          61,        3,      primary^field( pri^rsrvd2^prvt     ),
          62,        3,      primary^field( pri^rsrvd3^prvt     ),
          63,        3,      primary^field( pri^rsrvd4^prvt     ),
          64,        0,      primary^field( pri^mac^cde         ),
          65,       -1,      0, 0, ! Not defined in the standard !
          66,        0,      secndry^field( setl^cde            ),
          67,        0,      secndry^field( extd^pay^cde        ),
          68,        0,      secndry^field( rcv^inst^cntry^cde  ),
          69,        0,      secndry^field( setl^inst^cntry^cde ),
          70,        0,      secndry^field( netw^mgmt^cde       ),
          71,        0,      secndry^field( msg^num             ),
          72,        0,      secndry^field( lst^msg^num         ),
          73,        0,      secndry^field( action^dat          ),
          74,        0,      secndry^field( num^cr              ),
          75,        0,      secndry^field( num^cr^rvsl         ),
          76,        0,      secndry^field( num^db              ),
          77,        0,      secndry^field( num^db^rvsl         ),
          78,        0,      secndry^field( num^xfer            ),
          79,        0,      secndry^field( num^xfer^rvsl       ),
          80,        0,      secndry^field( num^inq             ),
          81,        0,      secndry^field( num^auth            ),
          82,        0,      secndry^field( amt^cr^proc^fees    ),
          83,        0,      secndry^field( amt^cr^tran^fees    ),
          84,        0,      secndry^field( amt^db^proc^fees    ),
          85,        0,      secndry^field( amt^db^tran^fees    ),
          86,        0,      secndry^field( amt^cr              ),
          87,        0,      secndry^field( amt^cr^rvsl         ),
          88,        0,      secndry^field( amt^db              ),
          89,        0,      secndry^field( amt^db^rvsl         ),
          90,        0,      secndry^field( orig^info           ),
          91,        0,      secndry^field( file^updt^cde       ),
          92,        0,      secndry^field( file^sec^cde        ),
          93,        0,      secndry^field( resp^ind            ),
          94,        0,      secndry^field( srv^ind             ),
          95,        0,      secndry^field( replacement         ),
          96,        0,      secndry^field( msg^sec^cde         ),
          97,        0,      secndry^field( setl^amt^net        ),
          98,        0,      secndry^field( payee               ),
          99,        2,      secndry^field( setl^inst           ),
         100,        2,      secndry^field( rcv^inst            ),
         101,        2,      secndry^field( fname               ),
         102,        2,      secndry^field( acct1               ),
         103,        2,      secndry^field( acct2               ),
         104,        3,      secndry^field( tran^descr          ),
         105,        3,      secndry^field( secndry^rsrvd1^iso  ),
         106,        3,      secndry^field( secndry^rsrvd2^iso  ),
         107,        3,      secndry^field( secndry^rsrvd3^iso  ),
         108,        3,      secndry^field( secndry^rsrvd4^iso  ),
         109,        3,      secndry^field( secndry^rsrvd5^iso  ),
         110,        3,      secndry^field( secndry^rsrvd6^iso  ),
         111,        3,      secndry^field( secndry^rsrvd7^iso  ),
         112,        3,      secndry^field( secndry^rsrvd1^natl ),
         113,        3,      secndry^field( secndry^rsrvd2^natl ),
         114,        3,      secndry^field( secndry^rsrvd3^natl ),
         115,        3,      secndry^field( secndry^rsrvd4^natl ),
         116,        3,      secndry^field( secndry^rsrvd5^natl ),
         117,        3,      secndry^field( secndry^rsrvd6^natl ),
         118,        3,      secndry^field( secndry^rsrvd7^natl ),
         119,        3,      secndry^field( secndry^rsrvd8^natl ),
         120,        3,      secndry^field( secndry^rsrvd1^prvt ),
         121,        3,      secndry^field( secndry^rsrvd2^prvt ),
         122,        3,      secndry^field( secndry^rsrvd3^prvt ),
         123,        3,      secndry^field( secndry^rsrvd4^prvt ),
         124,        3,      secndry^field( secndry^rsrvd5^prvt ),
         125,        3,      secndry^field( secndry^rsrvd6^prvt ),
         126,        3,      secndry^field( secndry^rsrvd7^prvt ),
         127,        3,      secndry^field( secndry^rsrvd8^prvt ),
         128,        0,      secndry^field( secndry^mac^cde )    ;
*/
///////////////////////////////////////////////////////////////////////


// Headers ANSI
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

// Headers propios
// Header Type Definitions
#include <qusrinc/typedefs.h>
// Aliases
#include <qusrinc/alias.h>
// ISO-8583
#include <qusrinc/iso8583.h>
// ISO8583 Customer definitions
#include <qusrinc/dpcsys.h>

// Longitud de bitmap segun ISO
#ifndef BITMAP_STR_LEN	
#define BITMAP_STR_LEN  16
#endif


#ifndef min
#define min(x,y)	(x<y) ? x : y
#define max(x,y)	(x>y) ? x : y
#endif // min-max


// Clase ISO8583MSG
// Constructor
 ISO8583MSG::ISO8583MSG(void) 
	{
	// Inicializacion
    memset((PBYTE)chMsg,0x00,sizeof chMsg); // Buffer Mensaje
	cbMsgLen = 0;                           // Longitud Buffer		
	// Buffer auxiliar de copia 
	memset(chAuxMsg,0x00,sizeof chAuxMsg);	// Buffer Mensaje
	cbAuxMsgLen = 0;						// Longitud Buffer		
	bCheckMessage = FALSE;					// No checking by default

	// Descripcion de Bitmaps y Campos por default
	DefaultBitmapsDescriptions();
	DefaultFieldsDescriptions();
	}

// Constructor opcional
 ISO8583MSG::ISO8583MSG(PBYTE lpbExtMsg	, 
							  WORD  cbExtLen	,
							  PSTR  szFldDescFile) 
	{
	// Inicializacion
    memset((PBYTE)chMsg,0x00,sizeof chMsg);            // Buffer Mensaje
	cbMsgLen = 0;                                   // Longitud Buffer		
	// Buffer auxiliar de copia 
	memset(chAuxMsg,0x00,sizeof chAuxMsg);	// Buffer Mensaje
	cbAuxMsgLen = 0;						// Longitud Buffer		
	bCheckMessage = TRUE;							// Checking by default
	// Descripcion de Bitmaps y Campos por default
	DefaultBitmapsDescriptions();
	DefaultFieldsDescriptions();
	// ¿Descripcion de Bitmaps y Campos parametrica?
	if(szFldDescFile)
		{
		GetAllBitmapsDescriptions(szFldDescFile);
		GetFieldsDescription(szFldDescFile);
		}//end-if-szFldDescFile
	if((lpbExtMsg) && (cbExtLen))
		// Copia de buffer plano externo
		Import(lpbExtMsg, cbExtLen);
	}

// Destructor
 ISO8583MSG::~ISO8583MSG(void)
	{
	// ...nada especifico que desalojar dinamicamente...
	}

// Importador
 BOOL ISO8583MSG::Import(const PBYTE lpbExtMsg, WORD cbExtLen)
	{
	PBYTE lpbLocMsg = lpbExtMsg;
	PBYTE lpbMsgETX = NULL;

	// Copia de buffer plano, chequeando longitud
	if( (lpbExtMsg) &&
		(cbExtLen < sizeof(chMsg)) &&
	    (cbExtLen > 0) )	    
		{
		// Nulificacion
		memset((PBYTE)chMsg,0,sizeof chMsg);
		cbMsgLen = 0;
		///////////////////////////////////////////////////////////////
		memmove((PBYTE)chMsg, lpbLocMsg, cbExtLen);		
		chMsg[cbExtLen] = 0x00; // Fin de Mensaje es NULL
		cbMsgLen = cbExtLen;
		// Si hay caracteres ETX o longitudes invalidas
		lpbMsgETX = (PBYTE)strchr((char *)lpbLocMsg, 0x03);
		if(lpbMsgETX)
			{ 
			*lpbMsgETX = 0x00;
			cbMsgLen = strlen((char *)lpbLocMsg);
			}
		// Si hay caracteres ETX o longitudes invalidas
		lpbMsgETX = (PBYTE)strchr((char *)lpbLocMsg, 0x02);
		if(lpbMsgETX)
			{ 
			*lpbMsgETX = 0x00;
			cbMsgLen = strlen((char *)lpbLocMsg);
			}
		if(cbExtLen > CISOMESSAGESIZE-1)
			cbExtLen = CISOMESSAGESIZE-1;
		///////////////////////////////////////////////////////////////

		// Postcondicion: Recalculo dinamico de offsets de Campos presentes Ok?
		return RecalculateOffsets();
		}
	else
		// Error
		return (FALSE);
	}

// Exportador
 BOOL ISO8583MSG::Export(PBYTE lpbExtMsg, PWORD pcbExtMsgLen)
	{
	// Copia de buffer plano, chequeando longitud
	if( (lpbExtMsg)                   &&
		((*pcbExtMsgLen) >= cbMsgLen) &&		
	    ((*pcbExtMsgLen) > 0)         &&
		(cbMsgLen > 0))
		{
		// Copia
		memmove(lpbExtMsg, (PBYTE)chMsg, cbMsgLen);
		///////////////////////////////////////////////////////////////
#ifdef _USE_ETX_
		lpbExtMsg[cbMsgLen  ] = 0x03; // ETX
		lpbExtMsg[cbMsgLen+1] = 0x00; // NULL
		(*pcbExtMsgLen)       = cbMsgLen +1 ;
#else
		lpbExtMsg[cbMsgLen  ] = 0x00; // NUL		
		(*pcbExtMsgLen)       = cbMsgLen ;
#endif
		///////////////////////////////////////////////////////////////
		// Ok
		return (TRUE);
		}
	else
		// Error
		return (FALSE);
	}

// Valor de Campo
 BOOL ISO8583MSG::GetField(WORD wField,PWORD pwFieldLen,
	                             PBYTE szValue,WORD wValueLen)
	{	
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Precondicion: campo valido
	if(!IsValidField(wField)) 
		return (FALSE);

	// Precondicion: Destino no nulo
	if((pwFieldLen == NULL) || (szValue == NULL) || (wValueLen == 0))
		return (FALSE);

	// Implicit Length
	WORD wImpLength = 0; 
	// Propiedades del Campo especifico
	WORD wType      = FieldType(wField),
         wOffset    = FieldOffset(wField),
	     wLength    = FieldLength(wField);

	// Campo Fijo o Variable?
	if (wType == 0) 
		// Campo fijo, longitud explicita
	    wImpLength = wLength;
	else 
	    // Campo variable, extraer longitud implicita
		if(!GetImplicitLength(wField, &wImpLength))
			return (FALSE);

	// Chequeo de longitud de campo, y eventual correccion
	if(wImpLength == 0)
		return (FALSE);
	else if((INT)wImpLength > ((INT)cbMsgLen - 32)) // ISO Header=32 bytes
		{		
		// Correccion de longitud implicita al maximo posible
		wImpLength = cbMsgLen - 32;                  
		// Correccion de longitud en campo interno
		fdFields[wField].Set(wType,wOffset,wLength); 
		} // end-if

	// Chequeo de longitud implicita/explicita contra la maxima a copiar
	if(wValueLen < wImpLength)
		{		
		// Copia directa...
		memmove((PBYTE)szValue,               // Destiny
			   (PBYTE)&chMsg[wOffset+wType], // Source + Offset
			   wValueLen );                   // Length
		// ...terminada en NULL para tratarla como STRING de BYTES
		szValue[wValueLen] = 0;
		// Longitud de Valor de Campo informada
		(*pwFieldLen) = wValueLen;
		}
	else
		{
		// Copia directa...
		memmove((PBYTE)szValue,               // Destiny
			   (PBYTE)&chMsg[wOffset+wType], // Source + Offset
				wImpLength );                  // Implicit Length
		// ...terminada en NULL para tratarla como STRING de BYTES
		szValue[wImpLength] = 0;
		// Longitud de Valor de Campo informada
		(*pwFieldLen) = wImpLength;
		}

	// Ok
	return (TRUE);
	}

// Valor de Longitud Implicita
 BOOL ISO8583MSG::GetImplicitLength(WORD wField,
                                          PWORD pwLength)
	{		
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Precondicion: Field ISO 1..128
	// Es campo valido?
	if(!IsValidField(wField)) 
		return (FALSE);

	BYTE szLength[6]; // ASCII Length
	WORD wImpLength;  // Implicit Length
	WORD wDigit;      // Digit Index 
	
	// Propiedades del Campo especifico
	WORD wType   = FieldType(wField)  ,
		 wOffset = FieldOffset(wField),
		 wLength = FieldLength(wField);

	// Retorno de Longitud Implicita...
	switch (wType)
		{
		case 0 : // Campo fijo, sin longitud implicita
				 return (FALSE);
		         break;
		case 1 : 
		case 2 : 
		case 3 :
		case 4 : // Campo variable, longitud implicita de 1 a 3 digitos
				 memmove((PBYTE)szLength,(PBYTE)&chMsg[wOffset], wType);
				 // Cada caracter debe ser un digito numerico
				 for(wDigit = 0; wDigit < wType; wDigit++)
					if(!isdigit((int)szLength[wDigit])) 
						return (FALSE);
				 // STRING de BYTES terminada en NULL
				 szLength[wType] = 0x00;
				 // Conversion a entero binario
		         wImpLength = atoi((PSTR)szLength);				   
				 // Ok
				 (*pwLength) = wImpLength; 
				 return (TRUE);
				 break;
		default: // Error
		         return (FALSE);
		         break;
		}
	// Return x default	
	return (FALSE);
	}

// Valor a Campo
 BOOL ISO8583MSG::SetField(WORD wField,PWORD pwFieldLen,
	                             PBYTE szValue,WORD wValueLen)
	{	
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Precondicion: campo valido
	if( !IsValidField(wField) ) 
		return (FALSE);

	// Precondicion: Origen no nulo
	if((pwFieldLen == NULL) || (szValue == NULL) || (wValueLen == 0))
		return (FALSE);

	// Propiedades del Campo especifico
	WORD wOffset    = FieldOffset(wField),
	     wLength    = FieldLength(wField),
	     wType      = FieldType(wField);
	// Longitud implicita en Campos variables
	WORD wImpLength = 0;

	// Segun tipo de campo	
	switch(wType)
		{
		case 0: // Campos fijos
		        // Chequear la longitud exacta...
				if(wLength != wValueLen) 
					return (FALSE);
				// Copia plana del campo especifico
				memmove( (PBYTE)&chMsg[wOffset], 
				        (PBYTE)szValue, 
						wLength);
				// Longitud de Campo informada
				(*pwFieldLen) = wLength;
				// Ok
				return (TRUE);
				break;
		case 1: 
		case 2: 
		case 3:
		case 4: // Campos variables, pero solo de longitud equivalente
				if(!GetImplicitLength( wField, &wImpLength ))
					return (FALSE);
				// Chequear la longitud...
				if(wImpLength == wValueLen) 
					{					
					// Copia del campo especifico
					memmove( (PBYTE)&chMsg[wOffset+wType], 
						    (PBYTE)szValue, wValueLen);
					// Longitud de Campo informada
					(*pwFieldLen) = wValueLen;
					// Ok
					return (TRUE);
					}				
				else
					return (FALSE);
				break;
		default:
		        return (FALSE);
				break;
		}
	}

// Insercion de Campo
 BOOL ISO8583MSG::InsertField(WORD  wField   ,PWORD pwFieldLen,
	                                PBYTE lpszValue,WORD  cbValueLen)
	{	
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Precondicion: No debe ser campo valido... porque se va a insertar:
	if( IsValidField(wField) ) 
		return (FALSE);

	// Precondicion: Origen no nulo
	if((pwFieldLen == NULL) || (lpszValue == NULL) || (cbValueLen == 0))
		return (FALSE);

	// Buffer auxiliar de copia (NO estatico por threads multiples)
	memset(chAuxMsg,0x00,sizeof chAuxMsg);
	cbAuxMsgLen = 0;
	// Mascara de formateo de longitud implicita
	BYTE szPrintMask[16];
	// Propiedades del Campo Anterior/Siguiente
	WORD wAuxField = 0,
		 wOffset   = 0,
		 wLength   = 0,
		 wType     = 0;
	// Propiedades del Nuevo Campo
	WORD wNewType  = 0;


	// Buscar el campo anterior valido
	for(wAuxField = wField-1;
		wAuxField >= 1         &&
		wAuxField < CISOFIELDS &&
	    !IsValidField(wAuxField);
		wAuxField--);

	// Hallado?
	if(IsValidField(wAuxField))
	{
		// Propiedades del Campo especifico
		wOffset = FieldOffset(wAuxField),
		wLength = FieldLength(wAuxField),
		wType   = FieldType(wAuxField);
	}
	else
	{

		// Buscar el campo siguiente valido
		for(wAuxField = wField+1;
			wAuxField >  1          &&
			wAuxField <= CISOFIELDS &&
			!IsValidField(wAuxField);
			wAuxField++);
		
		// Hallado?		
		if(IsValidField(wAuxField))
		{
			// Propiedades del Campo especifico
			wOffset = FieldOffset(wAuxField),
			wLength = FieldLength(wAuxField),
			wType   = FieldType(wAuxField);
			// Por ser campo siguiente, el nuevo campo se
			// inserta antes, por lo tanto, no se tiene en cuenta
			// la longitud de aquel para la copia, sino solo su
			// desplazamiento u offset:
			wLength = 0;

		}
		else
		{
			// Es el primer campo a insertar
			// Es el Secondary Bitmap?
			if(wField==1)
			{
				wOffset = 32; // Secondary Bitmap
				wLength = 16;
				wType   = 0;
				wAuxField = 0;
			}
			else
			{
				wOffset = 48; // Other
				wLength = FieldLength(wField),
				wType   = FieldType(wField);
				wLength = 0;			
				wAuxField = 0;
			}
		}
	}

	// Set a nulo del buffer local
	memset( (PBYTE)chAuxMsg, 0, sizeof chAuxMsg);

	// Copia a area local de trabajo, 
	// hasta el campo anterior/siguiente especifico
	memmove( (PBYTE)chAuxMsg, 
		    (PBYTE)chMsg, 
			wOffset+wLength );	
	// Longitud copiada hasta el campo anterior/siguiente especifico
	cbAuxMsgLen = wOffset+wLength; 

	// Hacerlo valido al Campo a insertar
	if( !MakeValidField( chAuxMsg, wField) ) 
		return (FALSE);

	// Insercion en si misma del nuevo campo, segun sea variable o fijo	
	wNewType = FieldType(wField);
	switch ( wNewType )
		{
		case 0: // Copia plana hasta campo anterior/siguiente
				memmove( (PBYTE)&chAuxMsg[wOffset+wLength],
						(PBYTE)lpszValue, 
						cbValueLen );	
				// Longitud adicionada del campo insertado
				cbAuxMsgLen += cbValueLen;
				break;
		case 1: 
		case 2: 
		case 3:
		case 4: // Mascara de formateo de longitud implicita "%0#d"
				sprintf( (PSTR)szPrintMask, "%%0%1i%1s", wNewType, "d");				
				sprintf( (PSTR)&chAuxMsg[wOffset+wLength], 
						 (PSTR)szPrintMask, cbValueLen );	
				// Copia variable hasta campo anterior/siguiente
				if((wOffset+wLength+wNewType) >= sizeof(chAuxMsg))
					return (FALSE);
				if((wOffset+wLength+wNewType+cbValueLen) >= sizeof(chAuxMsg))
					return (FALSE);
				memmove( (PBYTE)&chAuxMsg[wOffset+wLength+wNewType],
						(PBYTE)lpszValue, 
						cbValueLen );	
				// Longitud adicionada del campo insertado
				cbAuxMsgLen += cbValueLen+wNewType;
				break;
		default:// Error
				return (FALSE);				
		}

	// Copia a area local de trabajo, desde el campo anterior/siguiente
	if((wOffset+wLength+cbValueLen+wNewType) > sizeof(chAuxMsg))
		return (FALSE);
	if( cbMsgLen >= (wOffset+wLength) )
	{
	/****************************************************************/
		memmove( (PBYTE)&chAuxMsg[wOffset+wLength+cbValueLen+wNewType],
				(PBYTE)&chMsg[wOffset+wLength], 
				cbMsgLen-(wOffset+wLength) );	
		cbAuxMsgLen += cbMsgLen-(wOffset+wLength);
	/****************************************************************/
	} 
	else 
	{
	/****************************************************************/
		memmove( (PBYTE)&chAuxMsg[wOffset+wLength+cbValueLen+wNewType],
				(PBYTE)&chMsg[wOffset+wLength], 
				(wOffset+wLength)-cbMsgLen );	
		cbAuxMsgLen += (wOffset+wLength)-cbMsgLen;
	/****************************************************************/
	}

	// Copia a area externa
	memmove( (PBYTE)chMsg, 
	        (PBYTE)chAuxMsg, 
			cbAuxMsgLen);	
	cbMsgLen = cbAuxMsgLen;

	// Postcondicion: Recalculo de Offsets Ok?
	return RecalculateOffsets();

	}

// Borrado de Campo
 BOOL ISO8583MSG::DeleteField(WORD wAuxField)
	{	
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Precondicion: debe ser campo valido
	if( !IsValidField(wAuxField) ) 
		return (FALSE);

	// Buffer auxiliar de copia, (NO estatico por threads multiples)
	memset(chAuxMsg,0x00,sizeof chAuxMsg);
	cbAuxMsgLen = 0;

	// Copia a area local de trabajo
	memmove( (PBYTE)chAuxMsg, (PBYTE)chMsg, sizeof chAuxMsg);
	cbAuxMsgLen = min(cbMsgLen,sizeof chAuxMsg);

	// Propiedades del Campo especifico
	WORD wOffset    = FieldOffset(wAuxField),
		 wLength    = FieldLength(wAuxField),
		 wType      = FieldType(wAuxField);
    // Longitud implicita en campos variables
	WORD wImpLength = 0;

	// Verificar longitudes, y corregirlas
	if(wLength >= cbMsgLen)
		{
		if(((INT)cbMsgLen-(INT)wOffset) < 0)
			// Error
			return FALSE;
		else
			wLength = cbMsgLen-wOffset;
		}//end-if-length

	// Borrado en si mismo del campo, segun sea variable o fijo	
	switch ( wType )
		{
		case 0: // Copia exceptuada del resto del mensaje sobre el campo
			    // memmove(...)
			    if(cbAuxMsgLen < (wOffset+wLength))
					return (FALSE);
				memmove(  (PBYTE)&chAuxMsg[wOffset],
			             (PBYTE)&chAuxMsg[wOffset+wLength], 
				         cbAuxMsgLen-(wOffset+wLength) );	
				// Longitud restada del campo borrado
				cbAuxMsgLen -= wLength;
				break;
		case 1: 
		case 2: 
		case 3:
		case 4: // Longitud implicita?
				if(!GetImplicitLength( wAuxField, &wImpLength ))
					return (FALSE);
				// Chequeo de longitudes
				if((INT)cbAuxMsgLen-(wOffset+wImpLength+wType) < 0)
					return (FALSE);
				// Copia exceptuada del resto del mensaje sobre el campo
				// memmove(...)
				memmove( (PBYTE)&chAuxMsg[wOffset],
			            (PBYTE)&chAuxMsg[wOffset+wImpLength+wType],
				        cbAuxMsgLen-(wOffset+wImpLength+wType) );	
				// Longitud implicita restada del campo borrado
				cbAuxMsgLen -= (wImpLength+wType);
				// Set a NULL del resto
				memset( (PBYTE)&chAuxMsg[cbAuxMsgLen], 0,
				        sizeof(chAuxMsg) - cbAuxMsgLen);
				break;
		default:// Error
				return (FALSE);				
		}

	// Invalidar lo borrado
	if(!MakeInvalidField( chAuxMsg, wAuxField ))
		return (FALSE);

	// Copia a area externa
	memmove( (PBYTE)chMsg, (PBYTE)chAuxMsg, sizeof chMsg);
	cbMsgLen = cbAuxMsgLen;

	// Postcondicion: Recalculos Ok?
	return RecalculateOffsets();
	}

// Network Management Request
 BOOL ISO8583MSG::IsNetworkMgmtRequest(void)
{
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_NETMGMT_RQST, 4) == 0));
}

// Network Management Response
 BOOL ISO8583MSG::IsNetworkMgmtResponse(void)
{
	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_NETMGMT_RESP, 4) == 0));
}

// Es valido el Campo#?
 BOOL ISO8583MSG::IsValidField(WORD wField)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Precondicion: Campos validos
	if(!(wField >= 1 && wField <= CISOFIELDS)) 
		return (FALSE);

	// El Bitmap es una representacion hexadecimal {'0'..'9','A'..'F'},
	// tambien conocida como Bytemap cuando es en forma ASCII, como esta.
	// Cada digito indica la presencia o ausencia de campos, 
	// 4 por cada digito, y hasta un maximo de 64 campos, o sea
	// 16 posiciones. Si son dos Bitmaps, ocuparan 32 digitos.
	WORD wBitMapPos = 0;     // Posicion en BitMap (4 campos x c/u) 		
	BYTE bPosBitMap = 0x00;  // BitMap Binario de Posicion (4 bits)
	BYTE bBit       = 0x00;  // Bit en BitMap de Posicion {0,1,2,3}
	BYTE *pchBitMap = NULL;  // BitMap respectivo (Primario/Secundario)

	// Tratamiento como indice, 0..127, no como ISO 1..128: resto 1.
	wField--;

	// Posicion en BitMap? (4 campos cada posicion)
	wBitMapPos = (wField / 4);
	// Bit dentro del BitMap de Posicion (4 bits cada digito, 0..9,A..F)
	bBit = (BYTE)(wField % 4);
	
	// En que BitMap es, si c/u tiene 16 posiciones?
	if(wBitMapPos >= 0 && wBitMapPos <= 15) 
		{
		pchBitMap = &chMsg[16]; // Primary Bitmap
		}
	else
		{
		wBitMapPos -= BITMAP_STR_LEN;
		pchBitMap = &chMsg[32]; // Secondary Bitmap
		}

	// Chequeo de Posicion en BitMap
	if(!(wBitMapPos >= 0 && wBitMapPos <= 15))
		return (FALSE);

	// Bitmap ASCII (Bytemap) a Bitmap Binario (SWITCH por claridad en vez de restar 48='0')
	switch(pchBitMap[wBitMapPos])
		{
		// ASCII             // Decimal
		case '0': bPosBitMap = 0;  break; 
		case '1': bPosBitMap = 1;  break; 
		case '2': bPosBitMap = 2;  break; 
		case '3': bPosBitMap = 3;  break; 
		case '4': bPosBitMap = 4;  break; 
		case '5': bPosBitMap = 5;  break; 
		case '6': bPosBitMap = 6;  break; 
		case '7': bPosBitMap = 7;  break; 
		case '8': bPosBitMap = 8;  break; 
		case '9': bPosBitMap = 9;  break; 
		case 'A': bPosBitMap = 10; break; 
		case 'B': bPosBitMap = 11; break; 
		case 'C': bPosBitMap = 12; break; 
		case 'D': bPosBitMap = 13; break; 
		case 'E': bPosBitMap = 14; break; 
		case 'F': bPosBitMap = 15; break; 
		default:  return (FALSE);
		}
	
	// Esta presente el Bit en el BitMap de la Posicion?	
	// Se lo verifica con mascaras de bits predefinidas,
	// y con el operador AND
	switch(bBit)
		{
		case 0:  return ( 0x08 & bPosBitMap ) ? TRUE : FALSE; 
		case 1:  return ( 0x04 & bPosBitMap ) ? TRUE : FALSE; 
		case 2:  return ( 0x02 & bPosBitMap ) ? TRUE : FALSE; 
		case 3:  return ( 0x01 & bPosBitMap ) ? TRUE : FALSE; 
		default: return (FALSE);
		}	
	
	}

// Hacer valido/presente el Campo#
 BOOL ISO8583MSG::MakeValidField(BYTE chXMsg[], WORD wField)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Campos validos?
	if(!(wField >= 1 && wField <= CISOFIELDS)) 
		return (FALSE);

	// Ver comentarios de IsValidField()
	WORD wBitMapPos = 0;     // Posicion en BitMap (4 campos x c/u) 		
	BYTE bPosBitMap = 0x00;  // BitMap Binario de Posicion (4 bits)
	BYTE bBit       = 0x00;  // Bit en BitMap de Posicion {0,1,2,3}
	BYTE *pchBitMap = NULL;  // BitMap respectivo (Primario/Secundario)

	// Tratamiento como indice, 0..127, no como ISO 1..128
	wField--;

	// Posicion en BitMap? (4 campos cada posicion)
	wBitMapPos = (wField / 4);
	// Bit dentro del BitMap de Posicion (4 bits cada digito, 0..9,A..F)
	bBit = (BYTE)(wField % 4);
	
	// En que BitMap es, si c/u tiene 16 posiciones?
	if(wBitMapPos >= 0 && wBitMapPos <= 15) 
		{
		pchBitMap = &chXMsg[16]; // Primary Bitmap
		}
	else
		{
		wBitMapPos -= BITMAP_STR_LEN;
		pchBitMap = &chXMsg[32]; // Secondary Bitmap
		}

	// Chequeo de Posicion en BitMap
	if(!(wBitMapPos >= 0 && wBitMapPos <= 15))
		return (FALSE);

	// Bitmap ASCII (Bytemap) a Bitmap Binario (SWITCH por claridad en vez de restar 48='0')
	switch(pchBitMap[wBitMapPos])
		{
		// ASCII            // Decimal
		case '0': bPosBitMap = 0;  break; 
		case '1': bPosBitMap = 1;  break; 
		case '2': bPosBitMap = 2;  break; 
		case '3': bPosBitMap = 3;  break; 
		case '4': bPosBitMap = 4;  break; 
		case '5': bPosBitMap = 5;  break; 
		case '6': bPosBitMap = 6;  break; 
		case '7': bPosBitMap = 7;  break; 
		case '8': bPosBitMap = 8;  break; 
		case '9': bPosBitMap = 9;  break; 
		case 'A': bPosBitMap = 10; break; 
		case 'B': bPosBitMap = 11; break; 
		case 'C': bPosBitMap = 12; break; 
		case 'D': bPosBitMap = 13; break; 
		case 'E': bPosBitMap = 14; break; 
		case 'F': bPosBitMap = 15; break; 
		default:  return (FALSE);
		}
	
	// Hacer presente el Bit en el BitMap de la Posicion
	// Se lo hace con mascaras de bits predefinidas,
	// y con el operador INCLUSIVE-OR
	switch(bBit)
		{
		case 0:  bPosBitMap |= 0x08; break; 
		case 1:  bPosBitMap |= 0x04; break;  
		case 2:  bPosBitMap |= 0x02; break;  
		case 3:  bPosBitMap |= 0x01; break;  
		default: return (FALSE);
		}

	// Bitmap Binario a ASCII (SWITCH por claridad en vez de sumar 48='0')
	switch(bPosBitMap) 
		{
		// Decimal                     // ASCII
		case 0 : pchBitMap[wBitMapPos] = '0'; break; 
		case 1 : pchBitMap[wBitMapPos] = '1'; break; 
		case 2 : pchBitMap[wBitMapPos] = '2'; break; 
		case 3 : pchBitMap[wBitMapPos] = '3'; break; 
		case 4 : pchBitMap[wBitMapPos] = '4'; break; 
		case 5 : pchBitMap[wBitMapPos] = '5'; break; 
		case 6 : pchBitMap[wBitMapPos] = '6'; break; 
		case 7 : pchBitMap[wBitMapPos] = '7'; break; 
		case 8 : pchBitMap[wBitMapPos] = '8'; break; 
		case 9 : pchBitMap[wBitMapPos] = '9'; break; 
		case 10: pchBitMap[wBitMapPos] = 'A'; break; 
		case 11: pchBitMap[wBitMapPos] = 'B'; break; 
		case 12: pchBitMap[wBitMapPos] = 'C'; break; 
		case 13: pchBitMap[wBitMapPos] = 'D'; break; 
		case 14: pchBitMap[wBitMapPos] = 'E'; break; 
		case 15: pchBitMap[wBitMapPos] = 'F'; break; 
		default:  return (FALSE);
		}

	// Ok
	return (TRUE);
	
	}

// Hacer invalido/no-presente el Campo#
 BOOL ISO8583MSG::MakeInvalidField(BYTE chXMsg[], WORD wField)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Campos validos?
	if(!(wField >= 1 && wField <= CISOFIELDS)) 
		return (FALSE);

	// Ver comentarios de IsValidField()
	WORD wBitMapPos = 0;     // Posicion en BitMap (4 campos x c/u) 		
	BYTE bPosBitMap = 0x00;  // BitMap Binario de Posicion (4 bits)
	BYTE bBit       = 0x00;  // Bit en BitMap de Posicion {0,1,2,3}
	BYTE *pchBitMap = NULL;  // BitMap respectivo (Primario/Secundario)

	// Tratamiento como indice, 0..127, no como ISO 1..128
	wField--;

	// Posicion en BitMap? (4 campos cada posicion)
	wBitMapPos = (wField / 4);
	// Bit dentro del BitMap de Posicion (4 bits cada digito, 0..9,A..F)
	bBit = (BYTE)(wField % 4);
	
	// En que BitMap es, si c/u tiene 16 posiciones?
	if(wBitMapPos >= 0 && wBitMapPos <= 15) 
		{
		pchBitMap = &chXMsg[16]; // Primary Bitmap
		}
	else
		{
		wBitMapPos -= BITMAP_STR_LEN;
		pchBitMap = &chXMsg[32]; // Secondary Bitmap
		}

	// Chequeo de Posicion en BitMap
	if(!(wBitMapPos >= 0 && wBitMapPos <= 15))
		return (FALSE);

	// Bitmap ASCII (Bytemap) a Bitmap Binario (SWITCH por claridad en vez de restar 48='0')
	switch(pchBitMap[wBitMapPos])
		{
		// ASCII            // Decimal
		case '0': bPosBitMap = 0;  break; 
		case '1': bPosBitMap = 1;  break; 
		case '2': bPosBitMap = 2;  break; 
		case '3': bPosBitMap = 3;  break; 
		case '4': bPosBitMap = 4;  break; 
		case '5': bPosBitMap = 5;  break; 
		case '6': bPosBitMap = 6;  break; 
		case '7': bPosBitMap = 7;  break; 
		case '8': bPosBitMap = 8;  break; 
		case '9': bPosBitMap = 9;  break; 
		case 'A': bPosBitMap = 10; break; 
		case 'B': bPosBitMap = 11; break; 
		case 'C': bPosBitMap = 12; break; 
		case 'D': bPosBitMap = 13; break; 
		case 'E': bPosBitMap = 14; break; 
		case 'F': bPosBitMap = 15; break; 
		default:  return (FALSE);
		}
	
	// Hacer no-presente el Bit en el BitMap de la Posicion
	// Se lo hace con mascaras de bits predefinidas,
	// y con el operador EXCLUSIVE-OR
	switch(bBit)
		{
		case 0:  bPosBitMap ^= 0x08; break; 
		case 1:  bPosBitMap ^= 0x04; break;  
		case 2:  bPosBitMap ^= 0x02; break;  
		case 3:  bPosBitMap ^= 0x01; break;  
		default: return (FALSE);
		}

	// Bitmap Binario a ASCII (SWITCH por claridad en vez de sumar 48='0')
	switch(bPosBitMap)
		{
		// Decimal                     // ASCII
		case 0 : pchBitMap[wBitMapPos] = '0'; break; 
		case 1 : pchBitMap[wBitMapPos] = '1'; break; 
		case 2 : pchBitMap[wBitMapPos] = '2'; break; 
		case 3 : pchBitMap[wBitMapPos] = '3'; break; 
		case 4 : pchBitMap[wBitMapPos] = '4'; break; 
		case 5 : pchBitMap[wBitMapPos] = '5'; break; 
		case 6 : pchBitMap[wBitMapPos] = '6'; break; 
		case 7 : pchBitMap[wBitMapPos] = '7'; break; 
		case 8 : pchBitMap[wBitMapPos] = '8'; break; 
		case 9 : pchBitMap[wBitMapPos] = '9'; break; 
		case 10: pchBitMap[wBitMapPos] = 'A'; break; 
		case 11: pchBitMap[wBitMapPos] = 'B'; break; 
		case 12: pchBitMap[wBitMapPos] = 'C'; break; 
		case 13: pchBitMap[wBitMapPos] = 'D'; break; 
		case 14: pchBitMap[wBitMapPos] = 'E'; break; 
		case 15: pchBitMap[wBitMapPos] = 'F'; break; 
		default:  return (FALSE);
		}

	// Ok
	return (TRUE);
	
	}

// Es valido el mensaje?
 BOOL ISO8583MSG::IsValidMessage(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	/////////////////////////////////////////
	// Precondicion: Verificacion habilitada?
	if(!bCheckMessage) 
		return (TRUE);
	/////////////////////////////////////////

	// Compara cada parte del Mensaje
	// El Header esta Ok?
	if( !IsValidHeader() )
		return (FALSE);
	
	// El Tipo de Transaccion esta Ok?
	if( !IsValidTransactionType() )
		return (FALSE);

	// Los Datos de Usuario estan Ok?
	if( !IsValidUserData() )
		return (FALSE);

	// Ok
	return (TRUE);
	}

// Es valido el tipo de transaccion?
 BOOL ISO8583MSG::IsValidTransactionType(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	/////////////////////////////////////////
	// Precondicion: Verificacion habilitada?
	if(!bCheckMessage) 
		return (TRUE);
	/////////////////////////////////////////

	// El Tipo de Mensaje y BitMaps esta alguno Ok?
	if( IsValid200()  || IsValid205()  || IsValid210() ||
	    IsValid215()  || IsValid220()  || IsValid221() ||
		IsValid230()  || IsValid420()  || IsValid421() ||
		IsValid430()  || IsValid800()  || IsValid810()		 
	  ) return (TRUE);

	// Tipo Trx invalido
    return (FALSE);
	}

// Es valido el Header?
 BOOL ISO8583MSG::IsValidHeader(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Precondicion : Mensaje OBLIGATORIAMENTE con ISO-Header-Indicator. [0..2=3]
	if(memcmp((PBYTE)&chMsg[0], "ISO", 3) != 0)
		return (FALSE);

	/////////////////////////////////////////
	// Precondicion: Verificacion habilitada?
	if(!bCheckMessage) 
		return (TRUE);
	/////////////////////////////////////////

	// ISO-Header-Data [3..11=9]
	// Product-Id [3..4=2]
	if((memcmp((PBYTE)&chMsg[3], "00", 2) != 0) && // Base/Propietary
	   (memcmp((PBYTE)&chMsg[3], "01", 2) != 0) && // B24 ATM
	   (memcmp((PBYTE)&chMsg[3], "02", 2) != 0) && // B24 POS
	   (memcmp((PBYTE)&chMsg[3], "03", 2) != 0) && // B24 Teller
	   (memcmp((PBYTE)&chMsg[3], "08", 2) != 0) && // From Host Maintenance
	   (memcmp((PBYTE)&chMsg[3], "11", 2) != 0) && // B24 EMS
	   (memcmp((PBYTE)&chMsg[3], "12", 2) != 0))   // B24 EFTPOS
		return (FALSE);

	// Release-Number [5..6=2] (B24 Release 4.0="40", Release 5.1="51",...)
	if(!isdigit(chMsg[5]) || !isdigit(chMsg[6]))
	   return (FALSE);

    // Status [7..9=3] {"000"=ok,..."127","199"=security violation}
	if(!isdigit(chMsg[7]) || !isdigit(chMsg[8]) || !isdigit(chMsg[9]))
	   return (FALSE);

	// Originator [10..10=1]
	if((memcmp((PBYTE)&chMsg[10], "0", 1) != 0) && // Base/Propietary
	   (memcmp((PBYTE)&chMsg[10], "1", 1) != 0) && // B24 Device
	   (memcmp((PBYTE)&chMsg[10], "2", 1) != 0) && // Device Handler
	   (memcmp((PBYTE)&chMsg[10], "3", 1) != 0) && // Authorization
	   (memcmp((PBYTE)&chMsg[10], "4", 1) != 0) && // Host Interface Process
	   (memcmp((PBYTE)&chMsg[10], "5", 1) != 0) && // Host
	   (memcmp((PBYTE)&chMsg[10], "6", 1) != 0) && // Switch Interface Process
	   (memcmp((PBYTE)&chMsg[10], "7", 1) != 0))   // Switch
		return (FALSE);

	// Responder [11..11=1]
	if((memcmp((PBYTE)&chMsg[11], "0", 1) != 0) && // Base/Propietary
	   (memcmp((PBYTE)&chMsg[11], "1", 1) != 0) && // B24 Device
	   (memcmp((PBYTE)&chMsg[11], "2", 1) != 0) && // Device Handler
	   (memcmp((PBYTE)&chMsg[11], "3", 1) != 0) && // Authorization
	   (memcmp((PBYTE)&chMsg[11], "4", 1) != 0) && // Host Interface Process
	   (memcmp((PBYTE)&chMsg[11], "5", 1) != 0) && // Host
	   (memcmp((PBYTE)&chMsg[11], "6", 1) != 0) && // Switch Interface Process
	   (memcmp((PBYTE)&chMsg[11], "7", 1) != 0) && // Switch
	   (memcmp((PBYTE)&chMsg[11], "8", 1) != 0))   // Host Maintenance
		return (FALSE);

	// Message-Type-Identifier [12..15=4]
	if(!IsValidMsgTypeId())
		return (FALSE);

	// Ok
	return (TRUE);
	}

// Es Financial Trx Request valida? 
 BOOL ISO8583MSG::IsValid200(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]		
	if(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) // Trx Requirement
		{
			// Precondicion: Verificacion habilitada?
			if(!bCheckMessage) 
				return (TRUE);
			return (
				// Primary-Bitmap [16..31=16]	
				(bml200.Find((PSTR)&chMsg[16], 16) == TRUE)
				&&
				// Optional-Secondary-Bitmap [32..47=16]
				(bml200.Find((PSTR)&chMsg[32], 16) == TRUE)			
				) ? TRUE : FALSE;
		}
	else
		return FALSE;
	}

// Es Statement Print Request valido?
 BOOL ISO8583MSG::IsValid205(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]    	
	if (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_STMT_PRINT_RQST, 4) == 0) // StmtPrnt Requirement
		{
			// Precondicion: Verificacion habilitada?
			if(!bCheckMessage) 
				return (TRUE);
			return (
				// Primary-Bitmap [16..31=16]	
				(bml205.Find((PSTR)&chMsg[16], 16) == TRUE)
				&&
				// Optional-Secondary-Bitmap [32..47=16]
				(bml205.Find((PSTR)&chMsg[32], 16) == TRUE)			
				) ? TRUE : FALSE;
		}
	else
		return FALSE;
	}

// Es Financial Trx Response valida?
 BOOL ISO8583MSG::IsValid210(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]    	
	if (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RESP, 4) == 0) // Trx Response
		{
			// Precondicion: Verificacion habilitada?
			if(!bCheckMessage) 
				return (TRUE);
			return (
				// Primary-Bitmap [16..31=16]	
				(bml210.Find((PSTR)&chMsg[16], 16) == TRUE)
				&&
				// Optional-Secondary-Bitmap [32..47=16]
				(bml210.Find((PSTR)&chMsg[32], 16) == TRUE)			
				) ? TRUE : FALSE;
		}
	else
		return FALSE;
	}

// Es Satement Print Response valido?
 BOOL ISO8583MSG::IsValid215(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]    	
	if (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_STMT_PRINT_RESP, 4) == 0) // Stmtprnt Response
		{
			// Precondicion: Verificacion habilitada?
			if(!bCheckMessage) 
				return (TRUE);
			return (
				// Primary-Bitmap [16..31=16]	
				(bml215.Find((PSTR)&chMsg[16], 16) == TRUE)
				&&
				// Optional-Secondary-Bitmap [32..47=16]
				(bml215.Find((PSTR)&chMsg[32], 16) == TRUE)			
				) ? TRUE : FALSE;
		}
	else
		return FALSE;
	}

// Es Financial Trx Completion/SAF Advice valida?
 BOOL ISO8583MSG::IsValid220(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]    	
	if (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_SAF_ADV, 4) == 0) // SAF Requirement-Advice
		{
			// Precondicion: Verificacion habilitada?
			if(!bCheckMessage) 
				return (TRUE);
			return (
				// Primary-Bitmap [16..31=16]	
				(bml220.Find((PSTR)&chMsg[16], 16) == TRUE)
				&&
				// Optional-Secondary-Bitmap [32..47=16]
				(bml220.Find((PSTR)&chMsg[32], 16) == TRUE)			
				) ? TRUE : FALSE;
		}
	else
		return FALSE;
	}

// Es Financial Trx Dup Completion?
 BOOL ISO8583MSG::IsValid221(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]    	
	if (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_SAF_ADV_REP, 4) == 0) // Dup Completion?
		{
			// Precondicion: Verificacion habilitada?
			if(!bCheckMessage) 
				return (TRUE);
			return (
				// Primary-Bitmap [16..31=16]	
				(bml220.Find((PSTR)&chMsg[16], 16) == TRUE)
				&&
				// Optional-Secondary-Bitmap [32..47=16]
				(bml220.Find((PSTR)&chMsg[32], 16) == TRUE)			
				) ? TRUE : FALSE;
		}
	else
		return FALSE;
	}

// Es Financial Trx Completion Ack?
 BOOL ISO8583MSG::IsValid230(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]    	
	if (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_SAF_ACK, 4) == 0) // Completion Ack?
		{
			// Precondicion: Verificacion habilitada?
			if(!bCheckMessage) 
				return (TRUE);
			return (
				// Primary-Bitmap [16..31=16]	
				(bml230.Find((PSTR)&chMsg[16], 16) == TRUE)
				&&
				// Optional-Secondary-Bitmap [32..47=16]
				(bml230.Find((PSTR)&chMsg[32], 16) == TRUE)			
				) ? TRUE : FALSE;
		}
	else
		return FALSE;
	}


// Es Reversal valido?
 BOOL ISO8583MSG::IsValid420(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]    
	if (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0) // Reverse Trx Requirement
		{
			// Precondicion: Verificacion habilitada?
			if(!bCheckMessage) 
				return (TRUE);
			return (
				// Primary-Bitmap [16..31=16]	
				(bml420.Find((PSTR)&chMsg[16], 16) == TRUE)
				&&
				// Optional-Secondary-Bitmap [32..47=16]
				(bml420.Find((PSTR)&chMsg[32], 16) == TRUE)			
				) ? TRUE : FALSE;
		}
	else
		return FALSE;
	}

// Es Reversal Repeat valido?
 BOOL ISO8583MSG::IsValid421(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]    
	if (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0) // Reverse Repeat Trx Req
		{
			// Precondicion: Verificacion habilitada?
			if(!bCheckMessage) 
				return (TRUE);
			return (
				// Primary-Bitmap [16..31=16]	
				(bml420.Find((PSTR)&chMsg[16], 16) == TRUE)
				&&
				// Optional-Secondary-Bitmap [32..47=16]
				(bml420.Find((PSTR)&chMsg[32], 16) == TRUE)			
				) ? TRUE : FALSE;
		}
	else
		return FALSE;
	}


// Es Reversal Ack valido?
 BOOL ISO8583MSG::IsValid430(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]    
	if (memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_ACK, 4) == 0) // Reverse Trx Response
		{
			// Precondicion: Verificacion habilitada?
			if(!bCheckMessage) 
				return (TRUE);
			return (
				// Primary-Bitmap [16..31=16]	
				(bml430.Find((PSTR)&chMsg[16], 16) == TRUE)
				&&
				// Optional-Secondary-Bitmap [32..47=16]
				(bml430.Find((PSTR)&chMsg[32], 16) == TRUE)			
				) ? TRUE : FALSE;
		}
	else
		return FALSE;
	}

// Es Network Management valido?
 BOOL ISO8583MSG::IsValid800(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]    	
	return((memcmp((PBYTE)&chMsg[12], 
		           CISO_MSGTYPE_NETMGMT_RQST, 4) == 0) // Network Mgm Requirement
		   &&
		   // Primary-Bitmap [16..31=16]	
	       (bml800.Find((PSTR)&chMsg[16], 16) == TRUE)
		   &&
		   // Optional-Secondary-Bitmap [32..47=16]
	       (bml800.Find((PSTR)&chMsg[32], 16) == TRUE)
		   &&
		   // Transmision-Date-Time [48..57=10]		   
		   (isdigit(chMsg[48]) && isdigit(chMsg[49]) &&
		    isdigit(chMsg[50]) && isdigit(chMsg[51]) &&
			isdigit(chMsg[52]) && isdigit(chMsg[53]) &&
			isdigit(chMsg[54]) && isdigit(chMsg[55]) &&
			isdigit(chMsg[56]) && isdigit(chMsg[57]))			
		   &&
		   // Trace-Number [58..63=6]
		   (isdigit(chMsg[58]) && isdigit(chMsg[59]) &&
		    isdigit(chMsg[60]) && isdigit(chMsg[61]) &&
			isdigit(chMsg[62]) && isdigit(chMsg[58]))
		   &&		   
		   // Network-Managment-Information-Code [64..66=3]
	       ((memcmp((PBYTE)&chMsg[64], "001", 3) == 0) || // Logon
		    (memcmp((PBYTE)&chMsg[64], "002", 3) == 0) || // Logoff
	        (memcmp((PBYTE)&chMsg[64], "301", 3) == 0))   // Echo
	      ) ? TRUE : FALSE;		
	}	   

// Es Network Management Response valido?
 BOOL ISO8583MSG::IsValid810(void)
	{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Message-Type-Identifier [12..15=4]    
	return((memcmp((PBYTE)&chMsg[12], 
		            CISO_MSGTYPE_NETMGMT_RESP, 4) == 0) // Network Mgm Response
		   &&
		   // Primary-Bitmap [16..31=16]	
	       (bml810.Find((PSTR)&chMsg[16], 16) == TRUE)
		   &&
		   // Optional-Secondary-Bitmap [32..47=16]
	       (bml810.Find((PSTR)&chMsg[32], 16) == TRUE)
		   &&
		   // Transmision-Date-Time [48..57=10]		   
		   (isdigit(chMsg[48]) && isdigit(chMsg[49]) &&
		    isdigit(chMsg[50]) && isdigit(chMsg[51]) &&
			isdigit(chMsg[52]) && isdigit(chMsg[53]) &&
			isdigit(chMsg[54]) && isdigit(chMsg[55]) &&
			isdigit(chMsg[56]) && isdigit(chMsg[57]))			
		   &&
		   // Trace-Number [58..63=6]
		   (isdigit(chMsg[58]) && isdigit(chMsg[59]) &&
		    isdigit(chMsg[60]) && isdigit(chMsg[61]) &&
			isdigit(chMsg[62]) && isdigit(chMsg[58]))
		   &&		   
		   // Response-Code [64..65=2]
	       ((memcmp((PBYTE)&chMsg[64], "00", 2) == 0) || // Approved
		    (memcmp((PBYTE)&chMsg[64], "01", 2) == 0) || // Denied
	        (memcmp((PBYTE)&chMsg[64], "90", 2) == 0))   // Host Down
		   &&		   
		   // Network-Managment-Information-Code [66..68=3]
	       ((memcmp((PBYTE)&chMsg[66], "001", 3) == 0) || // Logon
		    (memcmp((PBYTE)&chMsg[66], "002", 3) == 0) || // Logoff
	        (memcmp((PBYTE)&chMsg[66], "301", 3) == 0))   // Echo
	      ) ? TRUE : FALSE;		
	}

// Son validos los datos de usuario?
 BOOL ISO8583MSG::IsValidUserData(void)
	{
	/////////////////////////////////////////
	// Precondicion: Verificacion habilitada?
	if(!bCheckMessage) 
		return (TRUE);
	/////////////////////////////////////////
	// Ok por default
	return (TRUE);
	}

// Offset de Campo en el Mensaje
 WORD ISO8583MSG::FieldOffset(WORD wField)  // Offset 
	{
	// Offset de campo especifico
	return(wField >= 1 && wField <= CISOFIELDS)
		? fdFields[wField-1].wOffset
		: 0;
	}

// Longitud de Campo en el Mensaje
 WORD ISO8583MSG::FieldLength(WORD wField)  // Length
	{	
	// Length de campo especifico
	return(wField >= 1 && wField <= CISOFIELDS)
		? fdFields[wField-1].wLength
		: 0;
	}

// Tipo de Campo en el Mensaje
 WORD ISO8583MSG::FieldType(WORD wField)  // Type
	{	
	// Type de campo especifico
	return(wField >= 1 && wField <= CISOFIELDS)
		? fdFields[wField-1].wType
		: (WORD)(-1);
	}

// Longitud de Mensaje
 WORD ISO8583MSG::MessageLength(void) 
	{		
	return (cbMsgLen);
	}

// Descripcion de Campos, tomada desde un archivo de parametrizacion
// externo opcional. Por omision se asume la tabla de inicializacion
// descripta en ISO8583MSG::DefaultFieldDescriptions()
 BOOL ISO8583MSG::GetFieldsDescription(PSTR szFldDescFile)
	{
	// Campos temporales de conversion
	int  iFldType   = 0,
		 iFldOffset = 0,
		 iFldLength = 0,
		 iIndex     = 0,
		 iCount     = 0,
		 iFldCount	= 0;
	FILE *pfStream	= NULL;
	// Rango ISO de posicion de campo, 1..128 en vez de 0..127
	BOOL bIs_ISO_Range = FALSE;	
	// Campos de conversion ASCII-binario
	char	szIndex[128]     = {0},
			szFldType[128]   = {0},
			szFldOffset[128] = {0},
			szFldLength[128] = {0},
			szBuffer[128]    = {0};

	// Abrir flujo de entrada
	pfStream = fopen(szFldDescFile, "rt");
	if(NULL == pfStream)
		return (FALSE);

	// Reset de Descripcion de Campos
	for(iIndex=0; iIndex < CISOFIELDS; iIndex++)		
		fdFields[iIndex].Reset();	

	// Mientras haya datos... leerlos y cargarlos (siempre habra 4 por linea)
	while( fgets( szBuffer, sizeof szBuffer, pfStream ) ) 
		{
			// Escaneo de parametros (siempre habra 4 por linea)
			iCount = sscanf( szBuffer, "%s %s %s %s", szIndex, szFldType, szFldOffset, szFldLength);
			// Verificar parametros (siempre habra 4 por linea)
			if(iCount == EOF || iCount != 4)
				continue;
			// Suma contador
			iFldCount++;
			// Convierte asignaciones
			iIndex     = atoi(szIndex);
			iFldType   = atoi(szFldType);
			iFldOffset = atoi(szFldOffset);
			iFldLength = atoi(szFldLength);
			// De posicion indice 0..127, no ISO 1..128
			if(iIndex == 0 && bIs_ISO_Range == FALSE && iFldCount == 1)
				bIs_ISO_Range = FALSE; // Basado en campo 0..127, no en campo 1..128
			else if(iIndex == 1 && bIs_ISO_Range == FALSE && iFldCount == 1)
				{
				bIs_ISO_Range = TRUE; // Basado en campo 1..128, no en campo 0..127
				iIndex--; // Ajustar ISO 1..128 a formato interno de indice 0..127
				}
			// De posicion ISO 1..128 a indice 0..127
			else if( iIndex >= 1 && iIndex <= CISOFIELDS && bIs_ISO_Range == TRUE  )
				iIndex--; // Ajustar ISO 1..128 a formato interno de indice 0..127

			// Son valores validos?
			if(
			   ((iFldType == (WORD)(-1)) ||       // Reservado,
			    ((iFldType >= FIELD_TYPE_FIX &&   // De Longitud Fija o
		         iFldType <= FIELD_TYPE_VAR_MAX)) // Variable?
			   ) 
			   && 
			   (iFldLength >= 0 && iFldLength <= CISOMESSAGESIZE) // Longitud?
			   && 
			   (iFldOffset >= 0 && iFldOffset <= CISOMESSAGESIZE) ) // Desplazamiento?
			  {
				// Copia de valores (Type,Offset,Length)
				fdFields[iIndex].wType   = iFldType;
				fdFields[iIndex].wOffset = iFldOffset;
				fdFields[iIndex].wLength = iFldLength;
			  	// Backup de Descriptor de Campo
				fdFieldsBkp[iIndex] = fdFields[iIndex];
			  }			  
			else
			{
				// Cierre del flujo
				fclose(pfStream);
				pfStream = NULL;		
				// Error
				return (FALSE);
			};//end-if
		};//while
	// Cierre del flujo
	fclose(pfStream);
	pfStream = NULL;		
	// Retorno Ok
	return (TRUE);	
	}

// Recalculo de Offset segun BitMaps actuales
 BOOL ISO8583MSG::RecalculateOffsets(void)
	{
	// Precondicion: Mensaje presente
	if(!cbMsgLen)
		return (FALSE);

	// Precondicion: Header valido
	if( !IsValidHeader() ) 
		return (FALSE);

	// Desplazamiento de campos desde el inicio del mensaje
	int  iOffset  = 32; // ISO-Fix-Header-Size
	// Longitud de Campo temporal
	WORD wFldLen  = 0;

	// Segun la descripcion inicial de Campos evaluada,
	// recalcular los Offsets de los campos variables,
	// si los hubiere segun el BitMap... segun los descriptores
	// originales...
	for(WORD iIndex=0; iIndex < CISOFIELDS; iIndex++)
		{

		  // Restore de Descriptor original
		  fdFields[iIndex] = fdFieldsBkp[iIndex];

		  // Esta activo el campo? (ISO 1-128)
		  if(IsValidField(iIndex+1))
			{
			// Es campo de Longitud Fija?
			if((fdFields[iIndex].wType == FIELD_TYPE_FIX) &&
			   (fdFields[iIndex].wOffset != 0) &&
			   (fdFields[iIndex].wLength != 0))
				{
				// Se establece el Offset dinamico...
				fdFields[iIndex].wOffset = iOffset;
				// ...y se lo recalcula en base al Length
				iOffset += fdFields[iIndex].wLength;				
				}
			// Es campo de Longitud Variable?
			else if((fdFields[iIndex].wType >= FIELD_TYPE_VAR_MIN)   &&
					(fdFields[iIndex].wType <= FIELD_TYPE_VAR_MAX)   &&				    
			        (fdFields[iIndex].wLength != 0))
				{
				// Se establece el Offset dinamico...
				fdFields[iIndex].wOffset = iOffset;

				// ... y se lo recalcula segun la Longitud Implicita
				// ... mas el Tipo de Campo Variable {1,2,3} (digitos implicitos)
				// (Field ISO 1..128, Index 0..127)
				if(!GetImplicitLength(iIndex+1, &wFldLen)) 
				{
					/////////////////////////////////////////
					// Precondicion: Verificacion habilitada?
					if(!bCheckMessage) 
						return (TRUE);
					/////////////////////////////////////////
					return (FALSE);
				};

				// Longitud implicita
				fdFields[iIndex].wLength = wFldLen + fdFields[iIndex].wType;
				// ...y se recalcula el Offset, Longitud mas los digitos implicitos:
				iOffset += wFldLen + fdFields[iIndex].wType;
				
				}
			// (Type!={0,1,2,3})&&(Offset=Length=0) no considerados
			}//if
		}//for

	// 2001-06-23-mdc. Postcondicion: Offset valido?
	return (iOffset < CISOMESSAGESIZE) 
		? TRUE 
		: FALSE;	

	// Postcondicion: Offset valido?
	return (iOffset < CISOMESSAGESIZE && iOffset < cbMsgLen) 
		? TRUE 
		: FALSE;	
	}

// Offsets & Lengths por default.
// Esta tabla de inicializacion de campos ISO-8583 de B24-HISO Release 4
// fue obtenida haciendo el siguiente comando en el BIND, en Tandem,
// "DUMP DATA BIT^MAP^TABLE^G 0,1001 DECIMAL FROM ATHISOO"
//
// TYPE especifica Longitud de Campo: {0=Fijo, 1,2,3,4=Variable} con 1,2,3,4 digitos
//	 implicitos: esto es, tomando los bytes y convertiendolos a entero.
// OFFSET especifica el desplazamiento desde el inicio del buffer. 
//   Se acumulan los desplazamientos anteriores y se suma la longitud.
// LENGTH especifica la longitud maxima admitida. 
 BOOL ISO8583MSG::DefaultFieldsDescriptions(void)
	{	
	// Recordar que los equivalentes campos ISO-8583 son de 1 a 128
	// Originales:
	// Campo Numero  ( Type, Offset, Length )	
	fdFieldsBkp[0].Set( 0, 32, 16 );
	fdFieldsBkp[1].Set( 2, 48, 21 );
	fdFieldsBkp[2].Set( 0, 69, 6 ); 
	fdFieldsBkp[3].Set( 0, 75, 12 );
	fdFieldsBkp[4].Set( 0, 87, 12 ); 
	fdFieldsBkp[5].Set( 0, 99, 12 );
	fdFieldsBkp[6].Set( 0, 111, 10 ); 
	fdFieldsBkp[7].Set( 0, 121, 8 );
	fdFieldsBkp[8].Set( 0, 129, 8 ); 
	fdFieldsBkp[9].Set( 0, 137, 8 );
	fdFieldsBkp[10].Set( 0, 145, 6 ); 
	fdFieldsBkp[11].Set( 0, 151, 6 );
	fdFieldsBkp[12].Set( 0, 157, 4 ); 
	fdFieldsBkp[13].Set( 0, 161, 4 );
	fdFieldsBkp[14].Set( 0, 165, 4 ); 
	fdFieldsBkp[15].Set( 0, 169, 4 );
	fdFieldsBkp[16].Set( 0, 173, 4 ); 
	fdFieldsBkp[17].Set( 0, 177, 4 );
	fdFieldsBkp[18].Set( 0, 181, 3 ); 
	fdFieldsBkp[19].Set( 0, 184, 3 );
	fdFieldsBkp[20].Set( 0, 187, 3 ); 
	fdFieldsBkp[21].Set( 0, 190, 3 );
	fdFieldsBkp[22].Set( 0, 193, 3 ); 
	fdFieldsBkp[23].Set( 0, 196, 3 );
	fdFieldsBkp[24].Set( 0, 199, 2 ); 
	fdFieldsBkp[25].Set( 0, 201, 2 );
	fdFieldsBkp[26].Set( 0, 203, 1 ); 
	fdFieldsBkp[27].Set( 0, 204, 9 );
	fdFieldsBkp[28].Set( 0, 213, 9 ); 
	fdFieldsBkp[29].Set( 0, 222, 9 );
	fdFieldsBkp[30].Set( 0, 231, 9 ); 
	fdFieldsBkp[31].Set( 2, 240, 13 );
	fdFieldsBkp[32].Set( 2, 253, 13 ); 
	fdFieldsBkp[33].Set( 2, 266, 30 );
	fdFieldsBkp[34].Set( 2, 296, 39 ); 
	fdFieldsBkp[35].Set( 3, 335, 107 );
	fdFieldsBkp[36].Set( 0, 442, 12 ); 
	fdFieldsBkp[37].Set( 0, 454, 6 );
	fdFieldsBkp[38].Set( 0, 460, 2 ); 
	fdFieldsBkp[39].Set( 0, 462, 3 );
	fdFieldsBkp[40].Set( 0, 465, 16 ); 
	fdFieldsBkp[41].Set( 0, 481, 15 );
	fdFieldsBkp[42].Set( 0, 496, 40 ); 
	fdFieldsBkp[43].Set( 2, 536, 27 );
	fdFieldsBkp[44].Set( 2, 563, 78 ); 
	fdFieldsBkp[45].Set( 3, 641, 103 );
	fdFieldsBkp[46].Set( 3, 744, 103 ); 
	fdFieldsBkp[47].Set( 3, 847, 103 );
	fdFieldsBkp[48].Set( 0, 950, 3 ); 
	fdFieldsBkp[49].Set( 0, 953, 3 );
	fdFieldsBkp[50].Set( 0, 956, 3 ); 
	fdFieldsBkp[51].Set( 0, 959, 16 );
	fdFieldsBkp[52].Set( 0, 975, 16 ); 
	fdFieldsBkp[53].Set( 3, 991, 123 );
	fdFieldsBkp[54].Set( 3, 1114, 363 ); 
#if defined( _SYSTEM_H24_ ) 
	fdFieldsBkp[55].Set( 4, 1477, 103 ); // Campo especialmente ampliado para H24
#elif  defined( _SYSTEM_SOAT_ ) || defined( _SYSTEM_BASE_ )
	fdFieldsBkp[55].Set( 3, 1477, 103 );
#elif  defined( _SYSTEM_ATM_ )
	fdFieldsBkp[55].Set( 3, 1477, 103 );
#else
	fdFieldsBkp[55].Set( 3, 1477, 103 );
#endif
	fdFieldsBkp[56].Set( 3, 1580, 103 ); 
	fdFieldsBkp[57].Set( 3, 1683, 103 );
	fdFieldsBkp[58].Set( 3, 1786, 103 ); 
	fdFieldsBkp[59].Set( 3, 1889, 103 );
	fdFieldsBkp[60].Set( 3, 1992, 103 ); 
	fdFieldsBkp[61].Set( 3, 2095, 103 );
	fdFieldsBkp[62].Set( 3, 2198, 600 ); 
	fdFieldsBkp[63].Set( 0, 2798, 16 );
	fdFieldsBkp[64].Set( 65535, 0, 0 ); // Campo reservado en el estandard ISO
	fdFieldsBkp[65].Set( 0, 2814, 1 );
	fdFieldsBkp[66].Set( 0, 2815, 2 ); 
	fdFieldsBkp[67].Set( 0, 2817, 3 );
	fdFieldsBkp[68].Set( 0, 2820, 3 ); 
	fdFieldsBkp[69].Set( 0, 2823, 3 );
	fdFieldsBkp[70].Set( 0, 2826, 4 ); 
	fdFieldsBkp[71].Set( 0, 2830, 4 );
	fdFieldsBkp[72].Set( 0, 2834, 6 ); 
	fdFieldsBkp[73].Set( 0, 2840, 10 );
	fdFieldsBkp[74].Set( 0, 2850, 10 ); 
	fdFieldsBkp[75].Set( 0, 2860, 10 );
	fdFieldsBkp[76].Set( 0, 2870, 10 ); 
	fdFieldsBkp[77].Set( 0, 2880, 10 );
	fdFieldsBkp[78].Set( 0, 2890, 10 ); 
	fdFieldsBkp[79].Set( 0, 2900, 10 );
	fdFieldsBkp[80].Set( 0, 2910, 10 ); 
	fdFieldsBkp[81].Set( 0, 2920, 12 );
	fdFieldsBkp[82].Set( 0, 2932, 12 ); 
	fdFieldsBkp[83].Set( 0, 2944, 12 );
	fdFieldsBkp[84].Set( 0, 2956, 12 ); 
	fdFieldsBkp[85].Set( 0, 2968, 16 );
	fdFieldsBkp[86].Set( 0, 2984, 16 ); 
	fdFieldsBkp[87].Set( 0, 3000, 16 );
	fdFieldsBkp[88].Set( 0, 3016, 16 ); 
	fdFieldsBkp[89].Set( 0, 3032, 42 );
	fdFieldsBkp[90].Set( 0, 3074, 1 ); 
	fdFieldsBkp[91].Set( 0, 3075, 2 );
	fdFieldsBkp[92].Set( 0, 3077, 5 ); 
	fdFieldsBkp[93].Set( 0, 3082, 7 );
	fdFieldsBkp[94].Set( 0, 3089, 42 ); 
	fdFieldsBkp[95].Set( 0, 3131, 16 );
	fdFieldsBkp[96].Set( 0, 3147, 17 ); 
	fdFieldsBkp[97].Set( 0, 3164, 25 );
	fdFieldsBkp[98].Set( 2, 3189, 13 ); 
	fdFieldsBkp[99].Set( 2, 3202, 13 );
	fdFieldsBkp[100].Set( 2, 3215, 19 ); 
	fdFieldsBkp[101].Set( 2, 3234, 30 );
	fdFieldsBkp[102].Set( 2, 3264, 30 ); 
	fdFieldsBkp[103].Set( 3, 3294, 103 );
	fdFieldsBkp[104].Set( 3, 3397, 103 ); 
	fdFieldsBkp[105].Set( 3, 3500, 103 );
	fdFieldsBkp[106].Set( 3, 3603, 103 ); 
	fdFieldsBkp[107].Set( 3, 3706, 103 );
	fdFieldsBkp[108].Set( 3, 3809, 103 ); 
	fdFieldsBkp[109].Set( 3, 3912, 103 );
	fdFieldsBkp[110].Set( 3, 4015, 103 ); 
	fdFieldsBkp[111].Set( 3, 4118, 203 );
	fdFieldsBkp[112].Set( 3, 4321, 103 );
	fdFieldsBkp[113].Set( 3, 4424, 103 );
	fdFieldsBkp[114].Set( 3, 4527, 103 );
	fdFieldsBkp[115].Set( 3, 4630, 103 );
	fdFieldsBkp[116].Set( 3, 4733, 103 );
	fdFieldsBkp[117].Set( 3, 4836, 103 );
	fdFieldsBkp[118].Set( 3, 4939, 103 );
	fdFieldsBkp[119].Set( 3, 5042, 153 );
	fdFieldsBkp[120].Set( 3, 5195, 153 ); 
	fdFieldsBkp[121].Set( 3, 5348, 153 );
	fdFieldsBkp[122].Set( 3, 5501, 458 ); 
	fdFieldsBkp[123].Set( 3, 5959, 687 );
	fdFieldsBkp[124].Set( 3, 6646, 683 ); 
#if defined ( _SYSTEM_BASE_ ) || defined ( _SYSTEM_ATM_ )
	fdFieldsBkp[125].Set( 3, 7329, 683 );
	fdFieldsBkp[126].Set( 3, 8012, 203 ); 
	fdFieldsBkp[127].Set( 0, 8215, 16 );
#elif defined( _SYSTEM_SOAT_ )
	fdFieldsBkp[125].Set( 4, 7329,1500 ); // Campo especialmente ampliado para SOAT
	fdFieldsBkp[126].Set( 3, 8829, 203 ); 
	fdFieldsBkp[127].Set( 0, 9032, 16 );
#else
	fdFieldsBkp[125].Set( 3, 7329, 683 );
	fdFieldsBkp[126].Set( 3, 8012, 203 ); 
	fdFieldsBkp[127].Set( 0, 8215, 16 );
#endif // _SOAT_SYSTEM_
	// Siempre OK
	return (TRUE);
	}

// TypeId de Msg
 BOOL ISO8583MSG::GetMsgTypeId(PBYTE lpszValue,WORD wValueLen)
	{
	// Precondicion: Mensaje presente
	if(!cbMsgLen)
		return (FALSE);

	// Precondicion: ISO-Header OK
	if(!IsValidHeader())
		return (FALSE);

	// Precondicion: Longitud suficiente: Son 4 de codigo mas 1 de NULL
	if( wValueLen < 5 )
		return (FALSE);

	// Message-Type-Identifier [12..15=4]
	memmove((PBYTE)lpszValue, (PBYTE)&chMsg[12], 4);	
	// Null-terminated array of bytes
	memset((PBYTE)lpszValue+4,0,1);

	// Ok
	return (TRUE);
	}

// Establecer TypeId de Msg
 BOOL ISO8583MSG::SetMsgTypeId(PBYTE lpszValue,WORD wValueLen)
	{
	// Precondicion: Mensaje presente
	if(!cbMsgLen)
		return (FALSE);

	// Precondicion: Longitud correcta
	if( wValueLen != 4 )
		return (FALSE);

	// Message-Type-Identifier [12..15=4]
	if(!IsValidMsgTypeId(lpszValue))
		return (FALSE);

	// Message-Type-Identifier [12..15=4]
	memmove((PBYTE)&chMsg[12], (PBYTE)lpszValue, 4);		

	// Postcondicion: Header Ok?
	return IsValidHeader();
	}

// Es valida la Identificacion de Tipo de Mensaje?
 BOOL ISO8583MSG::IsValidMsgTypeId(PBYTE lpcszXValue)
	{
	// Precondicion: Mensaje presente
	if(!cbMsgLen)
		return (FALSE);

	/////////////////////////////////////////
	// Precondicion: Verificacion habilitada?
	if(!bCheckMessage) 
		return (TRUE);
	/////////////////////////////////////////

	// Precondicion: Puntero local a MsgTypeId nulo en la inicializacion
	PBYTE lpszValue = NULL;

	// Precondicion: Tomar alguna direccion valida, 
	// y por omision la del mensaje en si mismo:
	if(lpcszXValue == NULL)
		lpszValue = (PBYTE)&chMsg[12];  // Al mensaje en si mismo
	else
		lpszValue = (PBYTE)lpcszXValue; // sino externamente

	// Message-Type-Identifier [12..15=4]
	if((memcmp((PBYTE)lpszValue, CISO_MSGTYPE_FINANCIAL_RQST , 4) != 0) && // Trx Requirement
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_STMT_PRINT_RQST, 4) != 0) && // Trx Requirement
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_FINANCIAL_RESP , 4) != 0) && // Request Response
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_STMT_PRINT_RESP, 4) != 0) && // Request Response
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_SAF_ADV        , 4) != 0) && // SAF Requirement
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_SAF_ADV_REP    , 4) != 0) && // SAF Requirement Repeat
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_FINANCIAL_ACK  , 4) != 0) && // Trx/SAF Response
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_REVERSAL       , 4) != 0) && // Reverse Trx Requirement
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_REVERSAL_REP   , 4) != 0) && // Reverse Trx Requirement Repeat
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_REVERSAL_ACK   , 4) != 0) && // Reverse Response
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_NETMGMT_RQST   , 4) != 0) && // Control Requirement
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_NETMGMT_RESP   , 4) != 0) && // Control Response
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_ADJUSTEMENT    , 4) != 0) && // Adjustement
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_FINAN_RQST_ERR , 4) != 0) && // Trx Req Error
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_FINAN_RESP_ERR , 4) != 0) && // Trx Resp Error
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_MULT_ACCNT_SEL , 4) != 0) && // Multiple Count Select
	   (memcmp((PBYTE)lpszValue, CISO_MSGTYPE_DEVICE_CONTROL , 4) != 0))   // Device Control
		return (FALSE);
	else
		return (TRUE);
	}

// Establecer TypeId Response de Msg
 BOOL ISO8583MSG::SetMsgTypeIdResponse(void)
	{
	// Precondicion: Mensaje presente
	if(!cbMsgLen)
		return (FALSE);

	// Message-Type-Identifier [12..15=4]
	if(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0) 
		memmove((PBYTE)&chMsg[12],CISO_MSGTYPE_FINANCIAL_RESP, 4); 
	else if(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_STMT_PRINT_RQST, 4) == 0) 
		memmove((PBYTE)&chMsg[12],CISO_MSGTYPE_STMT_PRINT_RESP, 4); 
	else if(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_SAF_ADV, 4) == 0) 
		memmove((PBYTE)&chMsg[12],CISO_MSGTYPE_SAF_ACK, 4); 
	else if(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_SAF_ADV_REP, 4) == 0) 
		memmove((PBYTE)&chMsg[12],CISO_MSGTYPE_SAF_ACK, 4); 
	else if(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL, 4) == 0) 
		memmove((PBYTE)&chMsg[12],CISO_MSGTYPE_REVERSAL_ACK, 4); 
	else if(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_REVERSAL_REP, 4) == 0) 
		memmove((PBYTE)&chMsg[12],CISO_MSGTYPE_REVERSAL_ACK, 4); 
	else if(memcmp((PBYTE)&chMsg[12], CISO_MSGTYPE_NETMGMT_RQST, 4) == 0) 
		memmove((PBYTE)&chMsg[12],CISO_MSGTYPE_NETMGMT_RESP, 4); 
	else
		return (FALSE);

	// ISO-BASE24-Header-Data [3..11=9]
	// Product-Id [3..4=2]
	if(chMsg[3] < '0' && chMsg[4] < '0')
	{
		chMsg[3] = '0'; chMsg[4] = '0'; // BASE PROPIETARY
		chMsg[3] = '0'; chMsg[4] = '2'; // POS=02
		chMsg[3] = '0'; chMsg[4] = '1'; // ATM=01		  <------ CURRENT PRODUCT
	};
	// Release-Number [5..6=2] (B24 Release 4.0="40", Release 5.1="51",...)
	if(chMsg[5] < '4')
	{
		chMsg[5] = '5'; chMsg[6] = '3'; // Release 5.3="53"
		chMsg[5] = '5'; chMsg[6] = '1'; // Release 5.1="51"
		chMsg[5] = '4'; chMsg[6] = '0'; // Release 4.0="40" <------ CURRENT VERSION
	};
	// Originator [10..10=1]
	if('0' == chMsg[11] || ' ' == chMsg[11])
		// '1' : Propietary ATM 		
		// '5' : Host Interface Process
		// '7' : Interchange Network (CIRRUS,VISANET,...)
		chMsg[10] = '5'; // Host Interface Process			<------ CURRENT ORIGINATOR		

	// Responder [11..11=1]
	if('0' == chMsg[11] || '3' == chMsg[11] || '7' == chMsg[11] || ' ' == chMsg[11])
		// '1' : Propietary ATM 		
		// '3' : Authorizator
		// '5' : Host Interface Process
		// '7' : Interchange Network (CIRRUS,VISANET,...)
		chMsg[11] = '5'; // Host Interface Process			<------ CURRENT RESPONDER	

	// Postcondicion: Header Ok?
	return IsValidHeader();
	}

// Descripcion de Bitmaps
 BOOL ISO8583MSG::GetAllBitmapsDescriptions(PSTR szFldDescFile)
	{
	BOOL bGetOk = FALSE;
	// Para todas los tipos de transacciones
	bGetOk |= GetBitmapDescription(CISO_MSGTYPE_FINANCIAL_RQST , szFldDescFile);
	bGetOk |= GetBitmapDescription(CISO_MSGTYPE_STMT_PRINT_RQST, szFldDescFile);
	bGetOk |= GetBitmapDescription(CISO_MSGTYPE_FINANCIAL_RESP , szFldDescFile);
	bGetOk |= GetBitmapDescription(CISO_MSGTYPE_STMT_PRINT_RESP, szFldDescFile);
	bGetOk |= GetBitmapDescription(CISO_MSGTYPE_FINANCIAL_ADV  , szFldDescFile);
	bGetOk |= GetBitmapDescription(CISO_MSGTYPE_FINANCIAL_ACK  , szFldDescFile);
	bGetOk |= GetBitmapDescription(CISO_MSGTYPE_REVERSAL       , szFldDescFile);
	bGetOk |= GetBitmapDescription(CISO_MSGTYPE_REVERSAL_ACK   , szFldDescFile);
	bGetOk |= GetBitmapDescription(CISO_MSGTYPE_NETMGMT_RQST   , szFldDescFile);
	bGetOk |= GetBitmapDescription(CISO_MSGTYPE_NETMGMT_RESP   , szFldDescFile);
	// Retorno dependiente
	return (bGetOk);	
	}

// Descripcion de Bitmaps
 BOOL ISO8583MSG::GetBitmapDescription(PSTR szTranTyp, PSTR szFldDescFile)
	{
	// Valor de los Items cargados
	CHAR  szBitmapTranTyp[512]	= {0};
	CHAR  szValue[512]			= {0};
	WORD  wFieldLen				= 0;	
	FILE *pfStream				= NULL;
	int   iCount                = 0;

	// Abrir flujo de entrada
	pfStream = fopen(szFldDescFile, "rt");
	if(NULL == pfStream)
		return (FALSE);
	// Mientras haya datos... leerlos y cargarlos (siempre habra 2 por linea)
	while( (iCount = fscanf(pfStream, "%s %s", szBitmapTranTyp, szValue)) > 0 )
		{
			// Verificar cantidad de parametros
			if(iCount != 2)
				continue;
			// Longitud de item
			wFieldLen = strlen((PSTR)szValue);
			// Escaneo de valores de bitmaps
			// Verificar bitmap exacto, sino ignorarlo
			if(wFieldLen != BITMAP_STR_LEN)
				continue;
			// Carga de valores segun tipo de transaccion {200,205,210,...}
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_FINANCIAL_RQST, 4) == 0)
				bml200.Insert(szValue);
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_STMT_PRINT_RQST, 4) == 0) 
				bml205.Insert(szValue);
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_FINANCIAL_RESP, 4) == 0) 
				bml210.Insert(szValue);
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_STMT_PRINT_RESP, 4) == 0) 
				bml215.Insert(szValue);
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_SAF_ADV, 4) == 0) 
				bml220.Insert(szValue);
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_SAF_ADV_REP, 4) == 0) 
				bml220.Insert(szValue);
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_SAF_ACK, 4) == 0)
				bml230.Insert(szValue);
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_REVERSAL, 4) == 0) 
				bml420.Insert(szValue);
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_REVERSAL_REP, 4) == 0) 
				bml420.Insert(szValue);
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_REVERSAL_ACK, 4) == 0) 
				bml430.Insert(szValue);
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_NETMGMT_RQST, 4) == 0) 
				bml800.Insert(szValue);
			else if(memcmp((PBYTE)szTranTyp, CISO_MSGTYPE_NETMGMT_RESP, 4) == 0) 
				bml810.Insert(szValue);
			else
				continue;	
		};//end-while
	// Cerrar flujo
	fclose(pfStream);
	pfStream = NULL;		
	// Retorno
	return (TRUE);	
	}

// Bitmaps por default en cada tipo de transaccion {200,205,215,200,...}
 BOOL ISO8583MSG::DefaultBitmapsDescriptions(void)
	{
	// Adaptados los BITMAPS especialmente por omision , para conexion con RED LINK S.A.
	// 200		                              
	bml200.Insert("B23A800128E0941A"); 
	bml200.Insert("B23880012EA08418");
	bml200.Insert("B23A800128E09018");
	bml200.Insert("B238800128E09018");
	bml200.Insert("B23A80012AD08418");
	bml200.Insert("B23A800128E09018");
	bml200.Insert("B238800128E19410");
	bml200.Insert("000000001600011A");
	bml200.Insert("000000001600001A");
	bml200.Insert("0000004010000002");
	bml200.Insert("000000421600007A");
	bml200.Insert("0000000004000012");
	bml200.Insert("0000000010000118");
	bml200.Insert("0000000014000118");
	bml200.Insert("000000000000001A");
	bml200.Insert("000000001000011C");
	// 205
	bml205.Insert("B23A800128E0941A");
	bml205.Insert("B23A800128E09018");
	bml205.Insert("B238800128E19410");
	bml205.Insert("000000001600011A");
	bml205.Insert("0000000014000118");
	bml205.Insert("000000000000001A");
	// 210
	bml210.Insert("B23A80012AD08418");
	bml210.Insert("B23A80012AE0961A");
	bml210.Insert("B23A80012AE0941A");
	bml210.Insert("B23A80012AE09018");				   
	bml210.Insert("B23A80012AE08018");
	bml210.Insert("B23A80012AD08618");	
	bml210.Insert("000000001600007A");
	bml210.Insert("0000004010000002");
	bml210.Insert("0000000004000012");
	bml210.Insert("000000421600007A");
	bml210.Insert("000000000000001A");
	bml210.Insert("0000000016000112");
	bml210.Insert("0000000014000118");
	bml210.Insert("000000001600001A");
	bml210.Insert("000000001000011C");
	// 215
	bml215.Insert("B23A80012AD08418");
	bml215.Insert("B23A80012AE09018");
	bml215.Insert("B23A80012AE08018");
	bml215.Insert("000000001600007A"); 
	bml215.Insert("0000000014000118");
	bml215.Insert("0000000016000110");
	bml215.Insert("0000000016000112");
	bml215.Insert("000000001600011A");
	// 220-221
	bml220.Insert("B23A80012AD08418");
	bml220.Insert("B23880012EA08418");
	bml220.Insert("B238800128E09018");
	bml220.Insert("B23A800128E0941A");
	bml220.Insert("000000001600007A");
	bml220.Insert("000000001600001A");
	bml220.Insert("0000004010000002");
	bml220.Insert("000000421600007A");
	bml220.Insert("0000000004000012");
	// 230
	bml230.Insert("B23A80012AD08418");
	bml230.Insert("B23A80012AE0941A");
	bml230.Insert("000000001600007A");
	bml230.Insert("000000001600008A");
	bml230.Insert("000000001600001A");
	bml230.Insert("000000001600000A");
	// 420-421
	bml420.Insert("B23A80012AD08418");
	bml420.Insert("B23A80012AE0961A");
	bml420.Insert("B23A80012AE0941A");
	bml420.Insert("B23A80012AD08618");
	bml420.Insert("B23A80012AD08418");
	bml420.Insert("B23880012EA08418");
	bml420.Insert("B238800128E09018");
	bml420.Insert("B23A800128E0941A");		     
	bml420.Insert("000000001600007A");
	bml420.Insert("0000004010000002");
	bml420.Insert("0000000004000012");
	bml420.Insert("000000421600007A");
	bml420.Insert("000000000000001A");
	bml420.Insert("000000001600001A");
	bml420.Insert("000000001600007A");
	bml420.Insert("000000001600001A");
	bml420.Insert("0000004010000002");
	bml420.Insert("000000421600007A");
	bml420.Insert("0000000004000012");
	// 430
	bml430.Insert("B23A80012AE08418");	
	bml430.Insert("000000421600007A");
	// 800
	bml800.Insert("8220000000000000");	
	bml800.Insert("0400000000000000");	        
	// 810
	bml810.Insert("8220000002000000");	
	bml810.Insert("0400000000000000");

	// Ok
	return TRUE;
	}

// Iniciar mensasje con un Header valido
 BOOL ISO8583MSG::InitMessage(void)
	{
	WORD wLen = 0;

	// Precondicion: sin longitud y sin mensaje previo
	cbMsgLen = 0;		
	memset( chMsg, 0x00, sizeof chMsg);		

	// ISO-Header-Indicator. [0..2=3]
	memcpy((PBYTE)&chMsg[0], "ISO", 3);		
	// ISO-Header-Data [3..11=9]
	// Product-Id [3..4=2]
	chMsg[3] = '0'; chMsg[4] = '0'; // BASE PROPIETARY
	chMsg[3] = '0'; chMsg[4] = '2'; // POS=02 POS? optional
	chMsg[3] = '0'; chMsg[4] = '1'; // ATM=01 ATM is the default
	// Release-Number [5..6=2] (B24 Release 4.0="40", Release 5.1="51",...)
	chMsg[5] = '5'; chMsg[6] = '3'; // Release 5.3=53 ?	optional.
	chMsg[5] = '5'; chMsg[6] = '1'; // Release 5.1=51 ? optional.
	chMsg[5] = '4'; chMsg[6] = '0'; // Release 4.0=40 BY DEFAULT, override if necessary
    // Status [7..9=3] {"000"=ok,..."127","199"=security violation}
	memcpy((PBYTE)&chMsg[7], "000", 3); // Base/Propietary
	// Originator [10..10=1]
	chMsg[10] = '4'; // Host Interface Process
	// Responder [11..11=1]
	chMsg[11] = '4'; // Host Interface Process
	// Message-Type-Identifier [12..15=4]	
	memcpy((PBYTE)&chMsg[12], CISO_MSGTYPE_FINANCIAL_RQST , 4); // Trx Requirement
	// Primary Bitmap
	memset((PBYTE)&chMsg[16], '0', 16); // 16 bytes in zero
	// Secondary Bitmap (field 1 in Primary Bitmap)
	chMsg[16] = '8'; // (field 1 in Primary Bitmap, '8' in ASCII, '1000' in BINARY ASCII)
	memset((PBYTE)&chMsg[32], '0', 16); // 16 bytes in zero

	// Postcondicion: Longitud
	cbMsgLen = strlen((PCHAR)chMsg);


	// Ok?
	return (RecalculateOffsets());
	}


// Recuperar bitmap 1 y 2
 void ISO8583MSG::GetBitmaps(char chBitmap1[],char chBitmap2[])
{
	if(chBitmap1 && chBitmap2)
	{
		memmove(chBitmap1,&chMsg[16],BITMAP_STR_LEN); // Primary Bitmap
		memmove(chBitmap2,&chMsg[32],BITMAP_STR_LEN); // Secondary Bitmap	
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Guardar Descripcion de Bitmaps
 BOOL ISO8583MSG::SaveAllBitmapsDescriptions(PSTR szFldDescFile)
	{
	BOOL bSaveOk = FALSE;
	FILE *pfStream				= NULL;
	// Eliminar configuracion anterior, abriendo y truncando
	pfStream = fopen(szFldDescFile, "wt");
	if(pfStream)
		fclose(pfStream);
	else
		return FALSE;
	// Para todas los tipos de transacciones
	bSaveOk |= SaveBitmapDescription(CISO_MSGTYPE_FINANCIAL_RQST , szFldDescFile);
	bSaveOk |= SaveBitmapDescription(CISO_MSGTYPE_STMT_PRINT_RQST, szFldDescFile);
	bSaveOk |= SaveBitmapDescription(CISO_MSGTYPE_FINANCIAL_RESP , szFldDescFile);
	bSaveOk |= SaveBitmapDescription(CISO_MSGTYPE_STMT_PRINT_RESP, szFldDescFile);
	bSaveOk |= SaveBitmapDescription(CISO_MSGTYPE_FINANCIAL_ADV  , szFldDescFile);
	bSaveOk |= SaveBitmapDescription(CISO_MSGTYPE_FINANCIAL_ACK  , szFldDescFile);
	bSaveOk |= SaveBitmapDescription(CISO_MSGTYPE_REVERSAL       , szFldDescFile);
	bSaveOk |= SaveBitmapDescription(CISO_MSGTYPE_REVERSAL_ACK   , szFldDescFile);
	bSaveOk |= SaveBitmapDescription(CISO_MSGTYPE_NETMGMT_RQST   , szFldDescFile);
	bSaveOk |= SaveBitmapDescription(CISO_MSGTYPE_NETMGMT_RESP   , szFldDescFile);
	// Retorno dependiente
	return (bSaveOk);	
	}

/////////////////////////////////////////////////////////////////////////////////
// Descripcion de Bitmaps
 BOOL ISO8583MSG::SaveBitmapDescription(PSTR szTranTyp, PSTR szFldDescFile)
	{
	// Valor de los Items cargados
	CHAR  szValue[512]			= {0};
	FILE *pfStream				= NULL;
	int   iCount                = 0;
	BITMAPLIST	*ptrList		= NULL;

	// Precondicion
	if(!szTranTyp || !szFldDescFile)
		return (FALSE);

	// Abrir flujo de salida
	pfStream = fopen(szFldDescFile, "at");
	if(NULL == pfStream)
		return (FALSE);

	// Segun tipo de mensaje ISO...
	if(strcmp(szTranTyp, CISO_MSGTYPE_FINANCIAL_RQST)==0)
		ptrList = &bml200;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_FINANCIAL_RESP)==0)
		ptrList = &bml210;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_FINANCIAL_ADV)==0)
		ptrList = &bml220;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_FINANCIAL_ADV_REP)==0)
		ptrList = &bml220;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_FINANCIAL_ACK)==0)
		ptrList = &bml230;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_STMT_PRINT_RQST)==0)
		ptrList = &bml205;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_STMT_PRINT_RESP)==0)
		ptrList = &bml215;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_REVERSAL)==0)
		ptrList = &bml420;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_REVERSAL_REP)==0)
		ptrList = &bml420;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_REVERSAL_ACK)==0)
		ptrList = &bml430;
	else
		return FALSE;
	// Precondicion
	if(!ptrList)
		return FALSE;
	// Recorrer la lista seleccionada y guardar BITMAP
	for ( iCount = ptrList->GetCount(); iCount > 0 && ptrList != NULL; iCount-- )
	{
		ptrList->GetAt(szValue, BITMAP_STR_LEN, iCount-1 );
		fprintf(pfStream, "%s %s\r\n", szTranTyp, szValue);
	}
	// Cerrar flujo
	fclose(pfStream);
	pfStream = NULL;		
	// Retorno
	return (TRUE);	
	}

/////////////////////////////////////////////////////////////////////////////////
// Recupera lista de BITMAPS determinada
BITMAPLIST * ISO8583MSG::BitmapListReference(PCSTR szTranTyp)
{
	// Precondicion
	if(!szTranTyp)
		return (FALSE);
	// Segun tipo de mensaje ISO...
	if(strcmp(szTranTyp, CISO_MSGTYPE_FINANCIAL_RQST)==0)
		return  &bml200;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_FINANCIAL_RESP)==0)
		return  &bml210;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_FINANCIAL_ADV)==0)
		return  &bml220;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_FINANCIAL_ADV_REP)==0)
		return  &bml220;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_FINANCIAL_ACK)==0)
		return  &bml230;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_STMT_PRINT_RQST)==0)
		return  &bml205;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_STMT_PRINT_RESP)==0)
		return  &bml215;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_REVERSAL)==0)
		return  &bml420;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_REVERSAL_REP)==0)
		return  &bml420;
	else if(strcmp(szTranTyp, CISO_MSGTYPE_REVERSAL_ACK)==0)
		return  &bml430;
	else
		return NULL;
}

/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Recupera el HEADER ISO
BOOL ISO8583MSG::GetHeader(char chHeader[], short nLen)	// Header
{
	// Precondicion: Mensaje existente
	if(!cbMsgLen) 
		return (FALSE);

	// Precondicion : Header Valido
	if(!IsValidHeader())
		return (FALSE);

	// Copiar header al exterior, de posicion 0 a 15 inclusive
	memcpy(chHeader,chMsg,min(16,nLen));

	// Ok
	return (TRUE);
}


/////////////////////////////////////////////////////////////////////////////////
