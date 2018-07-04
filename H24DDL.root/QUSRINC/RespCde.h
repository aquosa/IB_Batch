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
// (Inicial)    1998.09.02      mdc     Get
//

#ifndef RESPCDE_H
#define RESPCDE_H

#ifdef  MAX_RESPCDES
#undef  MAX_RESPCDES
#endif // MAX_RESPCDES
#define MAX_RESPCDES	(100)

// Estructura de Codigos y Textos asociados
typedef int basenum;
typedef	struct 
	{			
	basenum   nRespCde;		// Codigo de Respuesta
	basenum   nConv;		// Conversion
	} respcode_list_t[MAX_RESPCDES];

// Clase Lista de Textos de Codigos de Respuesta
class RESPCDELIST
{
public:
	
	// Metodos publicos de la clase
	EXPORT RESPCDELIST(void);			// Constructor por default
	EXPORT RESPCDELIST(basenum,basenum);// Constructor opcional	
	EXPORT ~RESPCDELIST();				// Destructor
	
	BOOL	EXPORT Insert(basenum,char *);  // Insercion de string de texto	
	BOOL	EXPORT Insert(basenum,basenum);  // Insercion de entero binario
	BOOL	EXPORT Find(basenum);         // Buscador de codigo de rta
	basenum EXPORT Get(basenum);			 // Cargador de codigo

private:

	// Lista privada para mantener los strings
	respcode_list_t	strlTexts;   // Lista de codigos binarios
	INT				iCounter;	 // Contador de carga

};

#endif // RESPCDE_H