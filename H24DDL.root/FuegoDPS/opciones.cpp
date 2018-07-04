///////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// Clase para leer el archivo de opciones en formato XML
//

#include "stdafx.h"
#include <QUSRINC/opciones.h>


/* CONSTRUCTOR CON LECTURA DE ARCHIVO*/

OPCIONES::OPCIONES( char *file ) 
{ 
	IsOpen = false;
	FILE *options = fopen(file, "rt");
	errornumber = errno;
	if (options == NULL)
		return;

	int read = fread(szXMLBuffer, sizeof(char), sizeof szXMLBuffer, options);

	szXMLBuffer[read] = '\0';
	IsOpen = strlen(szXMLBuffer) > 0;

	fclose(options);
	
};

/* DEFAULT DESTRUCTOR */
OPCIONES::~OPCIONES() 
{ 
};

/* GET TAG VALUE FROM INTERNAL MESSAGE */
char *OPCIONES::GetTagValue(  char *ptrTAG, char *ptrVALUE, size_t nLength )
{
	short shError = 0;
	return GetNextTagValue( ptrTAG, ptrVALUE, nLength, &shError, TRUE );
}

/* GET NEXT TAG VALUE FROM INTERNAL MESSAGE */
/* char   *ptrTAG = name of the TAG (input)
   char   *ptrVALUE = value of the TAG (output) 
   size_t nLength   = max-length of the output field  
   short  *pshError = error number if available (utput) 0=no error
   BOOL bFromBeginning = flag to start for the FIRST position or NEXT position
*/
char *OPCIONES::GetNextTagValue(  char *ptrTAG, char *ptrVALUE, size_t nLength, 
									   short *pshError, BOOL bFromBeginning  )
{
	char   *ptrBeginOpen    = NULL,
		   *ptrBeginClose   = NULL,
		   *ptrEnd			= NULL,
		   *ptr             = NULL,
		   *amp             = NULL;
	size_t nLenTAG			= 0,
		   nLenVALUE        = 0,
		   nLenAux          = 0;
	char   szLocalTAG[256]	= {"<"};
	char   *ptrLocalVALUE   = NULL;
	
	if(! ptrTAG || !ptrVALUE || 0 == nLength || !pshError)
		return (NULL);

	/* SEARCH FOR "<" - APPEND SYMBOLS */
	if( strchr( ptrTAG, '<') == 0)
	{
		/* NOT BEGINNING WITH "<", CONCATENATE TO LOCAL TAG */
		strcat(szLocalTAG, ptrTAG);
	}
	else
		/* ALREADY CONTAINS A "<", COPY TO LOCAL TAG */
		strcpy(szLocalTAG, ptrTAG);

	/* TAG LEN - MUST ADD DATA TYPE IF AVAILABLE */
	nLenTAG = strlen(szLocalTAG);

	/* TAKE LAST POSITION ? */
	if( bFromBeginning )
		ptr = szXMLBuffer;
	else if ( ptrLastTag && !bFromBeginning )
		ptr = ptrLastTag;
	else
		ptr = szXMLBuffer;
	
	/* SEARCH THE TAG */
	if( ptrBeginOpen = strstr( ptr, szLocalTAG ) )
	{
		/* SEARCH FOR "</" - APPEND BOTH SYMBOLS */
		sprintf(szLocalTAG, "</%s" , ptrTAG);
		if( ptrEnd = strstr( ptr, szLocalTAG ) )
		{
			/* SEARCH FOR ">" FROM BEGINNING OF TAG */
			if( ptrBeginClose = strchr( ptrBeginOpen + 1, '>' ) )
			{
				/* CHECK BUFFER SPACE UP TO X BYTES */
				if(ptrEnd > ptrBeginClose) nLenAux = (ptrEnd - ptrBeginClose);
				else					   nLenAux = (ptrBeginClose - ptrEnd);
				ptrLocalVALUE = new char[max(nLength,nLenAux)];
				if(!ptrLocalVALUE)
					return (NULL);
				if(ptrEnd > ptrBeginClose) nLenVALUE = (ptrEnd - ptrBeginClose) - 1;
				else                       nLenVALUE = (ptrBeginClose - ptrEnd) - 1;
				/* COPY VALUE */
				strncpy( ptrLocalVALUE, ptrBeginClose + 1, nLenVALUE );
				ptrLocalVALUE[nLenVALUE] = 0x00;

				/*********************************************/
				/* SPECIAL CHARS REPLACEMENT                 */
				/*********************************************/
				for( amp = strchr( ptrLocalVALUE, '&') ;
				     amp != NULL                  ;
					 amp = strchr( amp+1, '&'))
					 {
						 if(strncmp(amp,"&amp;",5)==0)
						 {
							strcpy(amp+1,amp+5);
							nLenVALUE = strlen( ptrLocalVALUE );
						 }
						 else
						 if(strncmp(amp,"&lt;",4)==0)
						 {
							strcpy(amp+1,amp+4);
							nLenVALUE = strlen( ptrLocalVALUE );
							(*amp) = '<';
						 }
						 else
						 if(strncmp(amp,"&gt;",4)==0)
						 {
							strcpy(amp+1,amp+4);
							nLenVALUE = strlen( ptrLocalVALUE );
							(*amp) = '>';
						 }
						 else
						 if(strncmp(amp,"&quot;",6)==0)
						 {
							strcpy(amp+1,amp+6);
							nLenVALUE = strlen( ptrLocalVALUE );
							(*amp) = '"'; /* ASCII-34 : DOUBLE QUOTE */ 
						 }
						 else
						 if(strncmp(amp,"&apos;",6)==0)
						 {
							strcpy(amp+1,amp+6);
							nLenVALUE = strlen( ptrLocalVALUE );
							(*amp) = 39; /* ASCII-39 : SINGLE QUOTE */
						 };
					 };
				/*********************************************/

				/*********************************************************/
				/* COMMENTS REPLACEMENT, STRING BETWEEN "!--" AND "--"   */
				/*********************************************************/
				 if(strncmp(ptrLocalVALUE,"<!--",4)==0)
				 {
					if(strncmp(ptrLocalVALUE+(nLenVALUE)-3,"-->",3)==0)
					{
						ptrLocalVALUE[nLenVALUE-3] = 0x00;
						strcpy(ptrLocalVALUE,ptrLocalVALUE+4);
						nLenVALUE = strlen( ptrLocalVALUE );
					}
				 };


				/*********************************************************/
				/* SPECIAL CHARS REPLACEMENT, STRING BETWEEN ' AND "     */
				/* ASCII-34 = " DOUBLE QUOTE / ASCII-39 = ' SINGLE QUOTE */
				/*********************************************************/
				if ((0x22 == ptrLocalVALUE[0] && 0x22 == ptrLocalVALUE[nLenVALUE-1])
					 ||
					(0x27 == ptrLocalVALUE[0] && 0x27 == ptrLocalVALUE[nLenVALUE-1]))
				{
					ptrLocalVALUE[nLenVALUE-1] = 0x00;
					strcpy(ptrLocalVALUE,ptrLocalVALUE+1);
					nLenVALUE = strlen( ptrLocalVALUE );
				}
				/*********************************************************/

				/* EXTERNAL COPY */
				strncpy( ptrVALUE, ptrLocalVALUE, min(nLenVALUE,nLength));
				ptrVALUE[min(nLenVALUE,nLength)] = 0x00;

				/* SAVE LAST POSITION */
				ptrLastTag = ptrEnd + nLenTAG;
				/* NO ERROR , OK */
				(*pshError) = 0;
				
				/* FREE THE ALLOCATED BUFFER */
				if(ptrLocalVALUE)
				{
					delete[] ptrLocalVALUE;
					ptrLocalVALUE = NULL;
				};

				/* RETURN PTR TO EXTERNAL STRING */				
				return ptrVALUE;
			}
			else
			{
				/* NO CLOSING-BRACKET OF END TAG FOUND */
				(*pshError) = -3;
				return NULL;
			}
		}
		else
		{
			/* NO END TAG FOUND */
			(*pshError) = -2;
			return NULL;
		}
	}
	/* NO BEGIN TAG FOUND */
	(*pshError) = -1;
	/*****************************/
	/* EMPTY EXTERN VALUE BUFFER */
	if(!ptrBeginOpen)
		(*ptrVALUE) = 0x00;
	/*****************************/
	return NULL;
}
