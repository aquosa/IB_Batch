/***************************************************************************************
 * H24 Data Definition Language, Definitions, and Methods
 * v2.0 2000.12.22 Martin D Cernadas. Etapa 2.
 * v3.0 2001.05.18 Martin D Cernadas. Etapa 3. DES no lleva mas conversion hexadecimal.
 ***************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "h24ddl.h"
#include "..\md5lib\md5lib.h"
#include "..\deslib\deslib.h"
#include "..\tcpsend\tcpsend.h"

/* encripcion DES del mensaje */
int encrypt_msg(HXXX_MESSAGES_UN *pClear, char *szKey, HXXX_MESSAGES_UN *pCiphered, 
				short tfHexaChars)
{	
	int iLen     = sizeof(pClear->stH200) - sizeof(struct HXXX_HEADER_NO_CRYPT);
	int iModulo8 = 8 - (iLen % 8);	
	int iOffset  = 0;		
	int iBlocks  = 0;
	HXXX_MESSAGES_UN	msgTemp;
	
	/* trabajar sobre el buffer de salida */
	(*pCiphered) = (*pClear);

	/* multiplo de 8 bytes? completar con blancos */
	while(iModulo8 > 0)
	{		
		if(pCiphered->chBuffer[sizeof(struct HXXX_HEADER_NO_CRYPT)+iLen+iModulo8] != ' ')
			pCiphered->chBuffer[sizeof(struct HXXX_HEADER_NO_CRYPT)+iLen+iModulo8] = ' ';
		iModulo8--;
	}

	/* encriptar cada 8 bytes con DES */
	msgTemp = *pCiphered;
	iLen += 8 - (iLen % 8);
	while(iLen > 0)
	{
		DES_encrypt( szKey ,pCiphered->chBuffer + (sizeof(struct HXXX_HEADER_NO_CRYPT) + iOffset) );
		/* convierta a hexadecimal? */
		if(tfHexaChars)
			if(binaryhexchar( msgTemp.chBuffer + sizeof(struct HXXX_HEADER_NO_CRYPT) + (iOffset*2), 
				16, (short *)(pCiphered->chBuffer + (sizeof(struct HXXX_HEADER_NO_CRYPT) + iOffset))) < 0)
				return -1;
		iLen    -= 8;
		iOffset += 8;
		iBlocks++;
		if(iLen == 8)
			;
	}	

	/* copiar al buffer de salida, dependiendo de la conversion hexa */
	if (tfHexaChars)
		(*pCiphered) = msgTemp;

	/* retornar longitud segun haya convertido a hexa o no */
	if (tfHexaChars)
		return (iBlocks * 8)*2;
	else
		return (iBlocks * 8);
}

/* des-encripcion DES del mensaje */
int decrypt_msg(HXXX_MESSAGES_UN *pClear, char *szKey, HXXX_MESSAGES_UN *pCiphered, short shLen,
				short tfHexaChars)
{	
	int iLen       =  shLen - sizeof(struct HXXX_HEADER_NO_CRYPT);
	int iOffset    = 0;	
	HXXX_MESSAGES_UN	msgTemp;
	int nCycleLen  = 16; /* 16 bytes block length as default */
	int nAddOffset = 0;
	
	/* trabajar sobre el buffer de salida */
	(*pClear) = (*pCiphered) ;
	iLen = min( (int)antoi(pCiphered->stHeader.long_msg, sizeof pCiphered->stHeader.long_msg) 
				- (int)(sizeof(struct HXXX_HEADER_NO_CRYPT)-sizeof(struct HXXX_HEADER_CRC)),
				iLen );	
	if(iLen <= 0)
		return -1;

	/* convierte a hexa si o no ? */
	if(tfHexaChars)
		nCycleLen = 16; /* por si, son 16 caracteres por bloque */
	else
		nCycleLen = 8; /* por no, son 8 caracteres por bloque */


	/* desencriptar cada 8 bytes con DES (en hexadecimal son 16 bytes) */
	msgTemp = (*pCiphered);
	while(iLen > 0)
	{
		/* convierte a hexa si o no ? */
		if(tfHexaChars)
		{
			if(hexcharbinary( (short *)(pClear->chBuffer + (sizeof(struct HXXX_HEADER_NO_CRYPT) + (iOffset/2))),
				msgTemp.chBuffer + sizeof(struct HXXX_HEADER_NO_CRYPT) + iOffset, 16) < 0)
				return -1;
			nAddOffset = sizeof(struct HXXX_HEADER_NO_CRYPT) + (iOffset/2) ;
		}
		else		
			nAddOffset = sizeof(struct HXXX_HEADER_NO_CRYPT) + iOffset ;
		/* desencripcion DES */
		DES_decrypt( szKey ,pClear->chBuffer + nAddOffset );		
		iLen    -= nCycleLen;
		iOffset += nCycleLen;
	}	

	/* Ok ! */
	return 0;
}

/* conversion de numerico string a numerico binario */
int antoi(char *pchAsciiNum, int iLen)
{
	char szNumber[32];
	/* Precondicion */
	if(iLen >= (sizeof(szNumber)-1))
		return -1;
	/* Copia interna */
	strncpy(szNumber, pchAsciiNum, iLen);
	szNumber[iLen] = 0x00;
	/* Retorno estandard */
	return atoi(szNumber);
};
