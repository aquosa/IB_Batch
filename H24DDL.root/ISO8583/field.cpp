//
// IT24 ASAP S.L.
// Proyecto Pago Automatico de Servicios
// Transaction Control Server
// ISO-8583-Field Library
//
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.07.31      mdc     Base
//


// Headers propios
// Header Type Definitions
#include <qusrinc/typedefs.h>
// Aliases
#include <qusrinc/alias.h>
// Campos ISO-8583
#include <qusrinc/field.h>

// Clase FIELD
// Constructor por default
FIELD::FIELD(void) 
	{ 
	wOffset = wLength = wType = 0; 
	} 
// Constructor opcional
FIELD::FIELD(WORD wX_Type,WORD wX_Offset,WORD wX_Length) 
	{ 	
	wType   = wX_Type; 
	wOffset = wX_Offset; 
	wLength = wX_Length; 
	} 
// Reset a NULL
void FIELD::Reset(void) 
	{ 
	wOffset = wLength = wType = 0; 
	} 
// Set a Externos
void FIELD::Set(WORD wX_Type,WORD wX_Offset,WORD wX_Length) 
	{ 
	wType   = wX_Type; 
	wOffset = wX_Offset; 
	wLength = wX_Length; 
	}  
