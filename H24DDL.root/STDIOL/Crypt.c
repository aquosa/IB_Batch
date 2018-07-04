#include <windows.h>
#include <wincrypt.h>
#include <tchar.h>

// Header propio
#include <qusrinc/stdiol.h>

#pragma comment(lib, "crypt32.lib")

// Proteger una cadena de datos 
DWORD ProtectStringValue(const char *value, char *result, unsigned long *len)
{
    DATA_BLOB unencryptedData, encryptedData;
	// Proteger una cadena de datos a nivel MAQUINA, no solo usaurio y credenciales
	unsigned long dwFlags = CRYPTPROTECT_LOCAL_MACHINE ;
	
	if (!value)
        return ERROR_INVALID_DATA;

    unencryptedData.pbData = (BYTE *)value;
	unencryptedData.cbData = (_tcslen(value) + 1)  * sizeof(*value);
	
	if (!CryptProtectData(
        &unencryptedData, 
        L"DATANET ONLINE",
        NULL,
        NULL,
        NULL,
        dwFlags,
        &encryptedData))
    {
        return GetLastError();
    }

	memcpy(result, encryptedData.pbData, encryptedData.cbData);

	*len = encryptedData.cbData;

    return 1;
}

LPTSTR UnprotectStringValue(char* value, unsigned long cryptLen)
{
    LONG result = ERROR_SUCCESS;
    DWORD valueType = REG_BINARY;
    DWORD requiredLen = 0;
    DATA_BLOB encryptedData, unencryptedData;
	LPTSTR unencryptedValue;

	if (!value)
        return NULL;

    encryptedData.pbData = (BYTE *)value;
    encryptedData.cbData = (cryptLen);

    if (!CryptUnprotectData(
        &encryptedData,
        NULL,
        NULL,
        NULL,
        NULL,
        0,
        &unencryptedData))
    {
		result = GetLastError();
        LocalFree(encryptedData.pbData);
        return NULL;
    }
    
    // NOTE: Contains NULL terminator
    unencryptedValue = (LPTSTR)malloc(unencryptedData.cbData);
    
	_tcscpy_s(unencryptedValue, unencryptedData.cbData, (LPTSTR)unencryptedData.pbData);
    
    // Cleanup
    LocalFree(unencryptedData.pbData);

    return unencryptedValue;
}

// afp: defino función para liberar memoria alocada dinamicamente en UnprotectStringValue desde fuera del dll. No está asegurado que se comparta heap entre dll y exe
void EXPORT_ATTR liberarMemoriaHeap(char *ptr)
{
	free(ptr);
}