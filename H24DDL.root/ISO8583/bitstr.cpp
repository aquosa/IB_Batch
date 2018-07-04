//////////////////////////////////////////////////////////////////////////////////////
// IT24 Sitemas S.A.
// BitMap String Library
//
//   Libreria de Cadenas BitMaps, implementadas como un arreglo de estructuras de tamano
//   fijo de 16 bytes, para optimizar las busquedas e inserciones, mas, mejorar las 
//   referencias a memoria manteniendo la localidad de los datos, relegando otras
//   implementaciones con listas-pilas-colas variables.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.07.31      mdc     Base
// (Inicial)    1998.08.05      mdc     bitmapstring_t
// (Alfa)       1998.08.06      mdc     Inserta solo si no existe previamente
// (Alfa)       1999.01.06      mdc     Header <bitmapl.h> en vez de <bitstr.h>
// (Beta)       2003.01.21      mdc     BOOL ::GetAt(PSTR,INT=BITMAP_STR_LEN,INT);                                      
//
//////////////////////////////////////////////////////////////////////////////////////

// Headers propios
#include <qusrinc/alias.h>
#include <qusrinc/typedefs.h>
#include <qusrinc/bitmapl.h>

// Standard I/O memory functions
#include <memory.h>
#include <string.h>


// Clase Lista de BitMapStr
// Constructor por default
BITMAPLIST::BITMAPLIST() 
	{
	// Contador a cero
	iCounter = 0;	
	}

// Constructor opcional
BITMAPLIST::BITMAPLIST(PSTR pszExtBitmap, INT iLen) 
	{
	// Contador a cero
	iCounter = 0;
	// Chequear longitudes...
	if(iLen >= BITMAP_STR_LEN)
		// Copia interna al final de la lista
		memcpy(strlPrimary[iCounter++].byte, pszExtBitmap, BITMAP_STR_LEN); 
	// e incremento a posteriori del contador
	}

// Destructor
BITMAPLIST::~BITMAPLIST() 
	{	
	// Contador a cero
	iCounter = 0;	
	}


// Insercion
BOOL BITMAPLIST::Insert(PSTR pszExtBitmap, INT iLen) 
	{
	// Chequear longitudes...
	if( (iLen >= BITMAP_STR_LEN) 
		&&
		// y maximo de contador
		(iCounter < MAX_BITMAPS)
		&&
		// y no existe previamente
		(Find(pszExtBitmap, BITMAP_STR_LEN) == FALSE)
		)
		{
		// Copia interna al final de la lista
		memmove( &strlPrimary[iCounter].byte[0], pszExtBitmap, BITMAP_STR_LEN); 
		// e incremento a posteriori del contador
		iCounter++;
		// Ok
		return (TRUE);
		}
	else
		return (FALSE);
	}

// Buscador
BOOL BITMAPLIST::Find(PSTR pszExtBitmap, INT iLen) 
	{
	// Precondicion
	if( (iCounter == 0) || (iLen < BITMAP_STR_LEN))
		return (FALSE);	

	// Iteracion para la busqueda
	for (INT i = 0; i < iCounter; i++)
			// Es el BitMapString buscado?						
			if ( memcmp(strlPrimary[i].byte, pszExtBitmap, BITMAP_STR_LEN) == 0 )
				// Hallado
				return (TRUE);
	// No hallado
	return (FALSE);
	}

///////////////////////////////////////////////////////////////////////
// Retornar contador
INT BITMAPLIST::GetCount() 
	{	
	// Contador a cero
	return iCounter;	
	}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Recuperar cierto BITMAP
BOOL BITMAPLIST::GetAt(PSTR pszExtBitmap, INT iLen, INT iPosition) 
	{
	// Precondicion
	if( (iCounter == 0) || (iLen < BITMAP_STR_LEN) || 
		iPosition > iCounter || iPosition < 0 )
		return (FALSE);	

	// BitMapString buscado
	memcpy(pszExtBitmap, strlPrimary[iPosition].byte, BITMAP_STR_LEN);

	// OK
	return (TRUE);
	}

///////////////////////////////////////////////////////////////////////
// Hacer valido/presente el Campo#
BOOL BITMAPLIST::MakeValidField(INT iPosition, WORD wField)
	{
	// Precondicion
	if( iCounter == 0 || iPosition > iCounter || iPosition < 0 )
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
		pchBitMap = &strlPrimary[iPosition].byte[0]; // Primary Bitmap
		}
	else
		{
		wBitMapPos -= BITMAP_STR_LEN;
		pchBitMap = &strlSecondary[iPosition].byte[0]; // Secondary Bitmap
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

	////////////////////////////////////////////////////////
	// Si de hecho YA ERA VALIDO, no hacer nada....
	switch(bBit)
		{
		case 0:  if ( 0x08 & bPosBitMap ) return TRUE ; break;
		case 1:  if ( 0x04 & bPosBitMap ) return TRUE ; break;
		case 2:  if ( 0x02 & bPosBitMap ) return TRUE ; break; 
		case 3:  if ( 0x01 & bPosBitMap ) return TRUE ; break; 
		default: return (FALSE);
		};
	/////////////////////////////////////////////////////////
	
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
//////////////////////////////////////////////////////////////////////////////////////
// Hacer invalido/no-presente el Campo#
BOOL BITMAPLIST::MakeInvalidField(INT iPosition, WORD wField)
	{
	// Precondicion
	if( iCounter == 0 || iPosition > iCounter || iPosition < 0 )
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
		pchBitMap = &strlPrimary[iPosition].byte[0]; // Primary Bitmap
		}
	else
		{
		wBitMapPos -= BITMAP_STR_LEN;
		pchBitMap = &strlSecondary[iPosition].byte[0]; // Secondary Bitmap
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

	////////////////////////////////////////////////////////
	// Si de hecho YA NO ERA VALIDO, no hacer nada....
	switch(bBit)
		{
		case 0:  if (!( 0x08 & bPosBitMap )) return TRUE ; break; 
		case 1:  if (!( 0x04 & bPosBitMap )) return TRUE ; break;
		case 2:  if (!( 0x02 & bPosBitMap )) return TRUE ; break;
		case 3:  if (!( 0x01 & bPosBitMap )) return TRUE ; break;
		default: return (FALSE);
		};
	/////////////////////////////////////////////////////////
	
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
		};

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

//////////////////////////////////////////////////////////////////////////////////////
// Es valido el Campo#?
BOOL BITMAPLIST::IsValidField(INT iPosition, WORD wField)
	{
	// Precondicion
	if( iCounter == 0 || iPosition > iCounter || iPosition < 0 )
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
		pchBitMap = &strlPrimary[iPosition].byte[0]; // Primary Bitmap
		}
	else
		{
		wBitMapPos -= BITMAP_STR_LEN;
		pchBitMap = &strlSecondary[iPosition].byte[0]; // Secondary Bitmap
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

//////////////////////////////////////////////////////////////////////////////////////
