// TestHedCSP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HED_CryptoAPI.h"

#define DEBUG_MODE

int main(int argc, char* argv[])
{
	HCRYPTPROV hProv;
    HCRYPTKEY hKey;
    DWORD dwParam;
//    BYTE *pbData;
    DWORD dwFlags;

	PVTableProvStruc pVTable;

		LPDWORD dwret = (DWORD*)malloc(sizeof(DWORD));
		LPBYTE pbDate;
		
		pbDate = (BYTE*)malloc(sizeof(BYTE) * 255);
	hProv = (HCRYPTPROV)malloc(sizeof(HCRYPTPROV));
	hKey = (HCRYPTKEY)malloc(sizeof(HCRYPTKEY));
	dwParam = KP_KEYLEN;
//	pbData = (BYTE*)malloc(sizeof(BYTE));
	dwFlags = 0x31;
	pVTable = (PVTableProvStruc)malloc(sizeof(PVTableProvStruc));

	

	
//	CPGetKeyParam1(hProv, hKey, dwParam, pbDate, dwret, dwFlags);
//	CPSetKeyParam1(hProv, hKey, KP_SALT, pbDate, dwFlags);
	if(!CPAcquireContext1(
		&hProv, 
		NULL, 
		0,
		NULL
		))
	{
		CPAcquireContext1(
		&hProv, 
		NULL, 
		CRYPT_NEWKEYSET,
		NULL
		);
		printf("Hello World!\n");
	}
	
	return 0;
}

