//
// IT24 SISTEMAS S.A.
// Proyecto Sistema OnLine de Atencion de Tarjetas (SOAT)
// Transaction Control Server
// ResponseText Class
//
//   Libreria de Cadenas BitMaps
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.08.07      mdc     Base
// (Inicial)    1998.09.02      mdc     Get(PSTR,PSTR=NULL)
//

#ifndef RESPTXT_H
#define RESPTXT_H

// Longitud maxima del texto
#ifndef MAX_RESPTXT_LEN	
#define MAX_RESPTXT_LEN		255
#endif 
// Longitud maxima de textos soportados
#ifndef MAX_RESPTXTS
#define MAX_RESPTXTS		100
#endif 

// Estructura de Codigos y Textos asociados
typedef	struct 
	{			
	PSTR   pszRespCde;		// Codigo de Respuesta
	PSTR   pszTxt;			// Texto de longitud variable	
	} respcode_text_t[MAX_RESPTXTS];

// Clase Lista de Textos de Codigos de Respuesta
CLASS RESPTXTLIST
{
	PUBLIC:
	
	// Metodos publicos de la clase
	EXPORT RESPTXTLIST(void);		// Constructor por default
	EXPORT RESPTXTLIST(PSTR,PSTR);	// Constructor opcional	
	EXPORT ~RESPTXTLIST();			// Destructor
	
	BOOL  EXPORT Insert(PSTR,PSTR);  // Insercion de string de texto	
	BOOL  EXPORT Find(PSTR);         // Buscador de codigo de rta
	PSTR  EXPORT Get(PSTR);			 // Cargador de texto desde un codigo

	PRIVATE:

	// Lista privada para mantener los strings
	respcode_text_t	strlTexts;   // Lista de strings 
	INT				iCounter;	 // Contador de carga

};

#endif // RESPTXT_H