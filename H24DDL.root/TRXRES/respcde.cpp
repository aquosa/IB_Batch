//
// IT24 Sistemas S.A.
// Proyecto Sistema OnLine de Atencion de Tarjetas
// Texto de Respuesta de Codigos de Errores 
//
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.08.07      mdc     Base
//


// Headers propios
#include <qusrinc/alias.h>
#include <qusrinc/typedefs.h>
#include <qusrinc/respcde.h>

// Standard I/O memory functions
#include <memory.h>
#include <string.h>
#include <stdlib.h>


// Clase Lista de BitMapStr
// Constructor por default
RESPCDELIST::RESPCDELIST() 
	{
	// Contador a cero
	iCounter = 0;	
	}

// Constructor opcional
RESPCDELIST::RESPCDELIST(basenum nRespCde, basenum nConv) 
	{
	// Contador a cero
	iCounter = 0;
	// Insercion de string de texto y codigo
	Insert(nRespCde, nConv);	
	}

// Destructor
RESPCDELIST::~RESPCDELIST() 
	{	
	// Contador a cero
	iCounter = 0;	
	}

// Insercion de string como int.
BOOL RESPCDELIST::Insert(basenum nRespCde, char *szConv) 
	{
	return this->Insert( nRespCde, atoi(szConv) );
	}

// Insercion
BOOL RESPCDELIST::Insert(basenum nRespCde, basenum nConv) 
	{
	// Precondicion
	if(iCounter >= MAX_RESPCDES || iCounter < 0)
		return (FALSE);
	// Precondicion
	if((iCounter+1) >= MAX_RESPCDES)
		return (FALSE);
	// Copia interna al final de la lista
	strlTexts[iCounter].nRespCde = nRespCde; 
	strlTexts[iCounter].nConv    = nConv; 
	// e incremento a posteriori del contador
	iCounter++;
	// Ok
	return (TRUE);
	}

// Buscador
BOOL RESPCDELIST::Find(basenum nRespCde) 
	{
	// Precondicion
	if(iCounter == 0)
		return (FALSE);	

	// Iteracion para la busqueda
	for (INT i = 0; i < iCounter; i++)
			// Es el codigo buscado?						
			if ( strlTexts[i].nRespCde == nRespCde )
				// Hallado
				return ( TRUE );
	// No hallado
	return (FALSE);
	}

// Cargador
basenum RESPCDELIST::Get(basenum nRespCde) 
	{
	// Precondicion
	if(iCounter == 0)
		return (FALSE);	

	// Iteracion para la busqueda
	for (INT i = 0; i < iCounter; i++)
			// Es el codigo buscado?						
			if ( strlTexts[i].nRespCde == nRespCde )
				// Hallado, retornar la conversion
				return strlTexts[i].nConv;
	// No hallado
	return (-1);
	}
