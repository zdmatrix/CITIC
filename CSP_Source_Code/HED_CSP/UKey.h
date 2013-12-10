#pragma once


void WINAPI SetUkeyLastErr(DWORD dwErr);

DWORD WINAPI GetUkeyLastErr(void);

//打开UKey
BOOL WINAPI UKeyOpen(HANDLE *hUKey);
	
//关闭UKey
BOOL WINAPI UKeyClose(HANDLE hUKey);

//验证UKey 的pin 含密码输入框,
BOOL WINAPI UKeyVerifyPIN(HANDLE hUKey, const char *Pin, const short PinLen);
BOOL WINAPI UKeyCheckPIN(HANDLE hUKey, int nLanguage = 1);
BOOL WINAPI UKeyChangePINToUkey(HANDLE hUKey, const char *Pin, const short PinLen);
extern "C" __declspec(dllexport) BOOL WINAPI UKeyChangePIN(HANDLE hUKey, int nLanguage = 1);
//去随机数
BOOL WINAPI UKeyGetChallange(HANDLE hUKey, unsigned char *Challange, const short Len);

BOOL WINAPI UKeyGetContainer(HANDLE hUKey,BYTE *pbContainer,BYTE *pstrCardCt);
BOOL WINAPI UKeyGetCspContainerCount(HANDLE hUKey, WORD *wCount, BYTE *pContainers);
BOOL WINAPI UKeyFindContainer(HANDLE hUKey,BYTE *pbContainer);
BOOL WINAPI UKeyDelContainer(HANDLE hUKey,BYTE *pbContainer);
BOOL WINAPI UKeyAddContainer(HANDLE hUKey,BYTE *strCardCt);
BOOL WINAPI UKeyUpdateContainer(HANDLE hUKey, BYTE *ContainerName, BYTE *strCardCt);
BOOL WINAPI UKeyGenKey(HANDLE hUKey, WORD wKeyPSfi, WORD wKeySSfi);
BOOL WINAPI UKeyGetKey(HANDLE hUKey, DWORD sfi, unsigned char KeyFlag, unsigned char *Key);
BOOL WINAPI UKeyRSAEncrypt(HANDLE hUKey, DWORD sfi, unsigned char *InData, int *InDataLen, 
					unsigned char *OutData, int *OutDataLen);
BOOL WINAPI UKeyRSADecrypt(HANDLE hUKey, DWORD sfi, unsigned char *InData, int *InDataLen, 
					unsigned char *OutData, int *OutDataLen);
BOOL WINAPI UKeyRSASignature(HANDLE hUKey, DWORD sfi, unsigned char *InData, int InDataLen, 
					unsigned char *OutData, int *OutDataLen);
BOOL WINAPI UKeyRSASignatureVerify(HANDLE hUKey, DWORD sfi, const unsigned char *InData, int InDataLen, 
					unsigned char *OutData, int *OutDataLen);
//证书相关
BOOL WINAPI UKeyGetNameFromCN(char *cn,char *name);
BOOL WINAPI UKeyGetCertName(BYTE *pbData, DWORD dwLen, char *pcOutName);
BOOL WINAPI UKeyAppendCert(HANDLE hUKey, BYTE *pstrCardCt, BYTE *pbData, DWORD dwLen);
BOOL WINAPI UKeyGetCert(HANDLE hUKey, BYTE *pstrCardCt, BYTE *pbData, DWORD dwLen);
BOOL WINAPI UKeyImportKey(HANDLE hUKey, DWORD sfi, unsigned char *Key);

BOOL WINAPI UKeyUnLock(HANDLE hUKey, BYTE *NewPin, int NewPinLen);
BOOL WINAPI UKeyGetInfo(HANDLE hUKey,BYTE *Info, int *InfoLen);
BOOL WINAPI UKeyGetContainerName(HANDLE hUKey,BYTE *CName, int *CNameLen);
BOOL WINAPI UKeySetContainerName(HANDLE hUKey,BYTE *CName, int CNameLen);
BOOL WINAPI UKeyClearPin(HANDLE hUKey);

BOOL WINAPI UKeyTestDisplayNum(HANDLE hUKey, int num);