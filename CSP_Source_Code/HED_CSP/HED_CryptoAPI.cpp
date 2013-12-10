/////////////////////////////////////////////////////////////////////////////
//  FILE          : csp.c                                                  //
//  DESCRIPTION   : Crypto API interface                                   //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//                                                                         //
//  Copyright (C) 1993 Microsoft Corporation   All Rights Reserved         //
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define DEBUG_MODE
//#define DEBUG_FILE_OPP
#include <wincrypt.h>
#include "HED_CryptoAPI.h"
#include "HDIFD20B.H"
#include "define.h"
#include "UKey.h"
#include "rsaref.h"
#include "rc2.h"
#include "des.h"

#define  DEBUG_MODE

#define ENCRYPT_MAX_DATALEN     128
#define CRYPT_SILENT            0x00000040
#define CRYPT_IMPL_REMOVABLE    8
#define	CRYPT_NOHASHOID         0x00000001


#define PP_ENUMMANDROOTS		25
#define PP_ENUMELECTROOTS		26
#define	PP_KEYSET_TYPE			27
#define PP_ADMIN_PIN            31
#define PP_KEYEXCHANGE_PIN      32
#define PP_SIGNATURE_PIN        33
#define PP_SIG_KEYSIZE_INC      34
#define PP_KEYX_KEYSIZE_INC     35
#define PP_UNIQUE_CONTAINER     36

struct AlgEnumalgs gAlgEnumalgs={0,6, 
						{ {CALG_MD5,128,4,"MD5"},
						{CALG_MAC,32,4,"MAC"},
						{CALG_RSA_SIGN,1024,9,"RSA_SIGN"},
						{CALG_SHA1,160,6,"SHA_1"},
						{CALG_DES,56,4,"DES"},
						{CALG_3DES,112,5,"3DES"},
						{CALG_RSA_KEYX,1024,9,"RSA_KEYX"}}};

struct AlgEnumalgsEx gAlgEnumalgsEx={0,6,
						{{CALG_MD5,128,128,128,0,4,"MD5",23,"Message Digest 5 (MD5)"},
						{CALG_MAC,32,32,32,0,4,"MAC",28,"Message Authentication Code"},
						{CALG_RSA_SIGN,1024,1024,1024,0,9,"RSA_SIGN",9,"RSA_SIGN"},
						{CALG_SHA1,160,160,160,0,6,"SHA_1",6,"SHA_1"},				
						{CALG_DES,56,56,56,0,4,"DES",4,"DES"},											
						{CALG_3DES,112,112,112,0,5,"3DES",5,"3DES"},
						{CALG_RSA_KEYX,1024,1024,1024,0,9,"RSA_KEYX",9,"RSA_KEYX"}}};

void _stdcall DEBUG_MSG(const char  *szCaption,const char  * szHint)
{
#ifdef DEBUG_MODE
	MessageBox(NULL,szCaption,szHint,MB_OK);
#endif
}


HINSTANCE g_hModule = NULL;

BOOL KeyImportFlag = FALSE;
int HaveGetContainer = 0;

/*
 -  CPAcquireContext
 -
 *  Purpose:
 *               The CPAcquireContext function is used to acquire a context
 *               handle to a cryptographic service provider (CSP).
 *
 *
 *  Parameters:
 *               OUT phProv         -  Handle to a CSP
 *               IN  szContainer    -  Pointer to a string which is the
 *                                     identity of the logged on user
 *               IN  dwFlags        -  Flags values
 *               IN  pVTable        -  Pointer to table of function pointers
 *
 *  Returns:
 */
BOOL WINAPI
CPAcquireContext1(
    OUT HCRYPTPROV *phProv,
    IN  LPCSTR szContainer,
    IN  DWORD dwFlags,
    IN  PVTableProvStruc pVTable)
{
	KeyImportFlag = FALSE;
	BOOL retVal=FALSE;
	//WORD wContextCount;
	struct CspContainer *pCspContainer;   //CSP容器指针
	struct CardContainer strCardContainer;//卡容器
	HaveGetContainer = 0;
	BYTE ContainerNameEx[100];
	int ContainerNameLenEx = 0;
//DEBUG信息
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
//	fprintf(fp,"CPAcquireContext1 in \t%x \t%s \t%x \t%x\n",*phProv,szContainer,dwFlags,pVTable);
	
//		fprintf(fp,"pVTable list is:cbContextInfo=%x, dwProvType=%x,Version=%x,pbContextInfo=",
//			pVTable->cbContextInfo,pVTable->dwProvType,pVTable->Version);
//		for (int j=0;j<pVTable->cbContextInfo;j++)
//			fprintf(fp, "%02x", pVTable->pbContextInfo[j]);	
	
	
	fprintf(fp,";;;\n");
	fclose(fp);
#endif
	*phProv = 0;

	//初始化CSP容器，申请内存空间
	pCspContainer=(struct CspContainer *)malloc(sizeof(struct CspContainer));
	if(pCspContainer==NULL)
	{
		SetLastError(NTE_NO_MEMORY);
		DEBUG_MSG("Create pCspContainer Err:NTE_NO_MEMORY !","CPAcquireContext !");
		return FALSE;
	}
	//初始化卡容器
	memset(&strCardContainer,0,sizeof(struct CardContainer));
		
	strCardContainer.Algid = CALG_RSA_SIGN;
	strCardContainer.dwParam = 1;
	
	strcpy((char *)strCardContainer.pbCertName, "test");
	strCardContainer.wCertAddr = 0x1B;
	strCardContainer.wCertSize = 0x00;
	strCardContainer.wKeyLen = 0x00;
	strCardContainer.wKeyPSfi = 0x0F;
	strCardContainer.wKeySSfi = 0x0E;

	//如果传入的容器名称为空，设置默认名称
	if(szContainer==NULL)
		strcpy((char *)strCardContainer.pbContName , "www.hed.com.cn");
 	else
 	{
 		//windows验证的时候采用读卡器+USB的名称带入作为container的名称，做一下转换
//		if (memcmp((char *)szContainer, "\\\\.\\VIRTUAL_CARD_READER 0\\", 30) == 0)
		
 		if (memcmp((char *)szContainer, "\\\\.\\SCM Microsystems Inc. SCR33x USB Smart Card Reader 0\\", 0x34) == 0)
 			memcpy((char *)strCardContainer.pbContName , szContainer, 0x34);
		else
			strcpy((char *)strCardContainer.pbContName , szContainer);

 	}

	//windows登录必须支持这个
	if (((dwFlags & CRYPT_SILENT) == CRYPT_SILENT) || ((dwFlags & CRYPT_VERIFYCONTEXT) == CRYPT_VERIFYCONTEXT))
	{
		//UKeyOpen(&pCspContainer->hUKey);
#ifdef DEBUG_MODE
		FILE *fp;
		fp=fopen("c:\\HED_CSP_LOG.txt","a+");
		fprintf(fp,"I come In \n");
		fclose(fp);
#endif
		if (UKeyOpen(&pCspContainer->hUKey))
		{
//			if ((szContainer != NULL) && (memcmp((char *)szContainer, "\\\\.\\VIRTUAL_CARD_READER 0\\", 30) == 0))
			
			if ((szContainer != NULL) && (memcmp((char *)szContainer, "\\\\.\\SCM Microsystems Inc. SCR33x USB Smart Card Reader 0\\", 0x34) == 0))
			{
				if (UKeyGetContainerName(pCspContainer->hUKey, ContainerNameEx, &ContainerNameLenEx))
				{
					
					if (ContainerNameLenEx == 0)
						strcpy((char *)strCardContainer.pbContName , "www.hed.com.cn");
					else
					{
						memcpy(strCardContainer.pbContName, ContainerNameEx, ContainerNameLenEx);
						strCardContainer.pbContName[ContainerNameLenEx] = 0;
					}

					
				}
			}
			UKeyGetContainer(pCspContainer->hUKey, strCardContainer.pbContName, (BYTE *)&strCardContainer);
#ifdef DEBUG_MODE
			//fp=fopen("c:\\HED_CSP_LOG.txt","a+");
			
		//	fprintf(fp,"CPAcquireContext wCertSize=%d, pbContName=%s\n",strCardContainer.wCertSize, strCardContainer.pbContName);
			//fclose(fp);
#endif
// 			if(!UKeyGetContainer(pCspContainer->hUKey, strCardContainer.pbContName, (BYTE *)&strCardContainer))
// 			{
// 				if (UKeyAddContainer(pCspContainer->hUKey, (BYTE *)&strCardContainer))
// 				{
// #ifdef DEBUG_MODE
// 		FILE *fp;
// 		fp=fopen("c:\\HED_CSP_LOG.txt","a+");
// 		fprintf(fp,"UKeyAddContainer Success\n");
// 		fclose(fp);
// #endif
// 				}
// 			}
		}
		retVal = TRUE;
		goto CPAcquireContextOut;
	}
	
	//打开UKey
	while(1)
	{
		if(!UKeyOpen(&pCspContainer->hUKey))
		{
			if(MessageBox(NULL, "请插入华大电子的UKEY !", NULL, MB_OKCANCEL)==IDCANCEL)
			{
				retVal=FALSE;
				goto CPAcquireContextOut;
			}
		}
		else
		{
			break;
		}
	}

    //
	if(!UKeyCheckPIN(pCspContainer->hUKey))
	{
		retVal=FALSE;
		goto CPAcquireContextOut;
	}

	retVal = true;
//	if(!UKeyGetCspContainerCount(pCspContainer->hUKey,&wContextCount))
//	{
//		DEBUG_MSG("UKeyGetCspContainerCount Err !","CPAcquireContext !");
//		goto CPAcquireContextOut;
//	}
//	if(memcmp((char *)szContainer, "\\\\.\\VIRTUAL_CARD_READER 0\\", 30) == 0))
	if (memcmp((char *)szContainer, "\\\\.\\SCM Microsystems Inc. SCR33x USB Smart Card Reader 0\\", 0x34) == 0)
	{
		if (UKeyGetContainerName(pCspContainer->hUKey, ContainerNameEx, &ContainerNameLenEx))
		{
#ifdef DEBUG_MODE
			fp=fopen("c:\\HED_CSP_LOG.txt","a+");
			
			fprintf(fp,"UKeyGetContainerName ContainerNameLenEx=%d, ContainerNameEx=%d\n",ContainerNameLenEx, ContainerNameEx[0]);
			fclose(fp);
#endif
			if (ContainerNameLenEx == 0)
				strcpy((char *)strCardContainer.pbContName , "www.hed.com.cn");
			else
			{
				memcpy(strCardContainer.pbContName, ContainerNameEx, ContainerNameLenEx);
				strCardContainer.pbContName[ContainerNameLenEx] = 0;
			}
		}
	}
	if(dwFlags==CRYPT_DELETEKEYSET) //删除容器
	{
		if (szContainer)
			retVal = UKeyDelContainer(pCspContainer->hUKey, strCardContainer.pbContName);
		else
			retVal = FALSE;
		goto CPAcquireContextOut;
	}
	else if(dwFlags==CRYPT_NEWKEYSET) //新建容器
	{	
		if(!UKeyAddContainer(pCspContainer->hUKey, (BYTE *)&strCardContainer))
		{
			//if(GetUkeyLastErr() == KEY_NO_SPACE)
			//	SetLastError(NTE_NO_MEMORY);
			//else
			//	SetLastError(SCARD_E_READER_UNAVAILABLE);

			retVal = FALSE;
			goto CPAcquireContextOut;
		}
		retVal = TRUE;
	}			
	else if(dwFlags==CRYPT_MACHINE_KEYSET || dwFlags==0)//暂时定为检索其中一个密钥容器
	{
		if(!UKeyGetContainer(pCspContainer->hUKey, strCardContainer.pbContName, (BYTE *)&strCardContainer))
		{
			//if(GetUkeyLastErr() == KEY_NOT_FIND_CONTEXT)
			//	SetLastError(NTE_KEYSET_NOT_DEF);
			//else
			//	SetLastError(SCARD_E_READER_UNAVAILABLE);
			retVal = FALSE;
			goto CPAcquireContextOut;
		}
		else
		{
			retVal = TRUE;
		}
	}
	else if (dwFlags == CRYPT_VERIFYCONTEXT)
	{
		//私有key不允许使用。
		//strCardContainer.wKeySSfi = 0x00;
		retVal = TRUE;
	}
	else
	{
		SetLastError(NTE_BAD_FLAGS);
		retVal=FALSE;
		goto CPAcquireContextOut;
	}

CPAcquireContextOut:
		if(!retVal)
		{
			//CPReleaseContext(*phProv,0);
			if(pCspContainer->hUKey)
				UKeyClose(pCspContainer->hUKey);
			if(pCspContainer != NULL)
			{
				free((void *)pCspContainer);
				pCspContainer = NULL;
				
			}
			//DEBUG_MSG("Return Err !","CPAcquireContext ");
			return FALSE;
		}
		//把CSP容器指针赋值给输出参数
		*phProv=(HCRYPTPROV) pCspContainer;
		pCspContainer->dwFlagsAQ=dwFlags;
		memcpy(&pCspContainer->strCardContainer,&strCardContainer,sizeof(struct CardContainer));
		strcpy(pCspContainer->cProvider,"HED_RAS_Cryptographic_Service_Provider_V1.0");
		pCspContainer->dwImptype=CRYPT_IMPL_REMOVABLE|CRYPT_IMPL_MIXED;  //如果要被CA服务器识别必须是这两种模式的
		pCspContainer->dwVersion=0x100;	
		pCspContainer->wSigKeySizeInc=0;
		pCspContainer->wKeyxKeySizeInc=0;
		//pCspContainer->szKeySetSecDescr=0;
		pCspContainer->dwProvType=PROV_RSA_FULL;
		pCspContainer->bHaveDefaultKey=FALSE;
		if(dwFlags==CRYPT_VERIFYCONTEXT)
			pCspContainer->dwPermissions=CRYPT_EXPORTABLE;
		else
			pCspContainer->dwPermissions=CRYPT_USER_PROTECTED;//私钥不可导出	

		
#ifdef DEBUG_MODE
		fp=fopen("c:\\HED_CSP_LOG.txt","a+");
		fprintf(fp,"CPAcquireContext out \t%x \t%s \t%x \t%x\n",*phProv,szContainer,dwFlags,pVTable);
		fclose(fp);
#endif
		return retVal;

    return TRUE;
}


/*
 -      CPReleaseContext
 -
 *      Purpose:
 *               The CPReleaseContext function is used to release a
 *               context created by CryptAcquireContext.
 *
 *     Parameters:
 *               IN  phProv        -  Handle to a CSP
 *               IN  dwFlags       -  Flags values
 *
 *  Returns:
 */

BOOL WINAPI
CPReleaseContext1(
    IN  HCRYPTPROV hProv,
    IN  DWORD dwFlags)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPReleaseContext \t%x \t%x\n",hProv,dwFlags);
	fclose(fp);
#endif
	HaveGetContainer = 0;
	struct CspContainer *pstrCspCt;
	if(hProv!=NULL)
	{
		pstrCspCt=(struct CspContainer *)hProv;
		UKeyClose(pstrCspCt->hUKey);
		free((void *)hProv);
	}
    return TRUE;
}


/*
 -  CPGenKey
 -
 *  Purpose:
 *                Generate cryptographic keys
 *
 *
 *  Parameters:
 *               IN      hProv   -  Handle to a CSP
 *               IN      Algid   -  Algorithm identifier
 *               IN      dwFlags -  Flags values
 *               OUT     phKey   -  Handle to a generated key
 *
 *  Returns:
 */
//还未完成.dwFlags还没考虑
BOOL WINAPI
CPGenKey1(
    IN  HCRYPTPROV hProv,
    IN  ALG_ID Algid,
    IN  DWORD dwFlags,
    OUT HCRYPTKEY *phKey)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPGenKey \t%x \t%x \t%x \t%x\n",hProv,Algid,dwFlags,phKey);
	fclose(fp);
#endif
	struct CspContainer *pstrCspCt;
	struct KeyObject *pKeyObject;
	WORD wSSfi = 0, wPsfi = 0;
	WORD wPosition = 0;
	BOOL bRetVal = 0;
	ALG_ID AlgIdVal;
	pstrCspCt=(struct CspContainer *)hProv;
	pKeyObject=(struct KeyObject *)malloc(sizeof(struct KeyObject));
	if(pKeyObject==NULL)
	{
		SetLastError(NTE_NO_MEMORY);
		return FALSE;
	}
	// 产生sessionkey
	if(Algid == CALG_RC2 || Algid == CALG_RC4 || Algid == CALG_DES)
	{
		DEBUG_MSG("Algid Is Not Surport !","CPGenKey !");
		SetLastError(NTE_BAD_ALGID);
		goto CPGenKeyOut;
	}
	//产生公私钥对
	if(Algid == AT_KEYEXCHANGE || Algid == AT_SIGNATURE)
	{
		if(Algid == AT_SIGNATURE)
		{
			wSSfi = pstrCspCt->strCardContainer.wKeySSfi;
			wPsfi = pstrCspCt->strCardContainer.wKeyPSfi;
			AlgIdVal = CALG_RSA_SIGN ;
		}
		else
		{
			AlgIdVal = CALG_RSA_KEYX ;
			wPsfi = 0x1F;
			wSSfi = 0x1E;

		}
			
		

		if (dwFlags != 0)
		{
			if(!UKeyGenKey(pstrCspCt->hUKey, wPsfi, wSSfi))
			{
				goto CPGenKeyOut;
			}
		}
		/*if(wSSfi == 0 || wPsfi == 0)//add key pairs
		{
			if(!UKeyGetNextKeyAddr(pstrCspCt->hUKey,&wSSfi,&wPsfi))
			{
				if(GetUkeyLastErr() == KEY_NO_CONTEXT)
					SetLastError(NTE_BAD_UID);
				else if(GetUkeyLastErr() == KEY_NO_SPACE )
				{
					SetLastError(NTE_NO_MEMORY);
					MessageBox(NULL,"The Size of Key Is Not Enough !","CPGenKey !",MB_OK);
				}
				else
					SetLastError(SCARD_E_READER_UNAVAILABLE);
				bRetVal = FALSE;
				goto CPGenKeyOut;
			}
		}
		if(!UKeyGenKey(pstrCspCt->hUKey,pstrCspCt->strCardContainer.pbContName,AlgIdVal,dwFlags,wSSfi,wPsfi))
		{
			if(GetUkeyLastErr() == KEY_NO_CONTEXT)
				SetLastError(NTE_BAD_UID);
			else if(GetUkeyLastErr() == KEY_GENERATE_PEM_ERROR)
				SetLastError(NTE_FAIL);
			else
				SetLastError(SCARD_E_READER_UNAVAILABLE);
			bRetVal = FALSE;
			goto CPGenKeyOut;
		}
		if(!UKeyGetContainer(pstrCspCt->hUKey,pstrCspCt->strCardContainer.pbContName,&pstrCspCt->strCardContainer))
		{
			if(GetUkeyLastErr() == KEY_NO_CONTEXT)
				SetLastError(NTE_BAD_UID);
			else
				SetLastError(SCARD_E_READER_UNAVAILABLE);
			bRetVal = FALSE;
			goto CPGenKeyOut;
		}*/
		//set the pKeyObject attributes
		memset(pKeyObject,0,sizeof(struct KeyObject));
		pKeyObject->wSsfi = wSSfi;	
		pKeyObject->wPsfi = wPsfi;
		pKeyObject->Algid=AlgIdVal;
		pKeyObject->dwBlockLen=1024;//HIWORD(dwFlags);
		pKeyObject->dwKeyLen=1024;//HIWORD(dwFlags);
		if(dwFlags & CRYPT_EXPORTABLE)
			pKeyObject->dwPermissions = CRYPT_ENCRYPT|CRYPT_DECRYPT|CRYPT_EXPORT|CRYPT_READ|CRYPT_WRITE;
		else
			pKeyObject->dwPermissions = CRYPT_ENCRYPT|CRYPT_DECRYPT|CRYPT_WRITE;
		pKeyObject->dwPadding=PKCS5_PADDING;
		pKeyObject->dwMode=CRYPT_MODE_ECB;
		pKeyObject->dwModeBits=0;//只有OFB方式有此属性
		pKeyObject->dwEffectiveKeyLen=0;//????
		pKeyObject->bIsKeyPairs=TRUE;
		bRetVal=TRUE;
	}//if Algid
	else
	{
		DEBUG_MSG("Algid is Unknow !","CPGenKey !");
		SetLastError(NTE_BAD_ALGID);
		goto CPGenKeyOut;
	}

CPGenKeyOut:
	if(!bRetVal)
	{
		if(pKeyObject!=NULL)
			free(pKeyObject);
		DEBUG_MSG("CPGenKey return is FALSE!","CPGenKey !");
		return FALSE;
	}
	else
	{	
		*phKey = (HCRYPTKEY)pKeyObject;
		return TRUE;
	}
	return TRUE;
}


/*
 -  CPDeriveKey
 -
 *  Purpose:
 *                Derive cryptographic keys from base data
 *
 *
 *  Parameters:
 *               IN      hProv      -  Handle to a CSP
 *               IN      Algid      -  Algorithm identifier
 *               IN      hBaseData -   Handle to base data
 *               IN      dwFlags    -  Flags values
 *               OUT     phKey      -  Handle to a generated key
 *
 *  Returns:
 */

BOOL WINAPI
CPDeriveKey1(
    IN  HCRYPTPROV hProv,
    IN  ALG_ID Algid,
    IN  HCRYPTHASH hHash,
    IN  DWORD dwFlags,
    OUT HCRYPTKEY *phKey)
{
#ifdef DEBUG_MODE
		FILE *fp;
		fp=fopen("c:\\HED_CSP_LOG.txt","a+");
		fprintf(fp,"CPDeriveKey \t%x \t%x \t%x \t%x\t%x\n",hProv,Algid,hHash,phKey,dwFlags,phKey);
		fclose(fp);
#endif
    *phKey = (HCRYPTKEY)NULL;    // Replace NULL with your own structure.
    return TRUE;
}


/*
 -  CPDestroyKey
 -
 *  Purpose:
 *                Destroys the cryptographic key that is being referenced
 *                with the hKey parameter
 *
 *
 *  Parameters:
 *               IN      hProv  -  Handle to a CSP
 *               IN      hKey   -  Handle to a key
 *
 *  Returns:
 */

BOOL WINAPI
CPDestroyKey1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPDestroyKey \t%x \t%x\n",hProv,hKey);
	fclose(fp);
#endif
	if(hKey != NULL)
		free((void *)hKey);

   return TRUE;
}


/*
 -  CPSetKeyParam
 -
 *  Purpose:
 *                Allows applications to customize various aspects of the
 *                operations of a key
 *
 *  Parameters:
 *               IN      hProv   -  Handle to a CSP
 *               IN      hKey    -  Handle to a key
 *               IN      dwParam -  Parameter number
 *               IN      pbData  -  Pointer to data
 *               IN      dwFlags -  Flags values
 *
 *  Returns:
 */

BOOL WINAPI
CPSetKeyParam1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  DWORD dwParam,
    IN  CONST BYTE *pbData,
    IN  DWORD dwFlags)
{
#ifdef DEBUG_MODE
	FILE *fp;
	long i;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPSetKeyParam \t%x \t%x \t%x \t%x\t%x\t%x\n",hProv,hKey,dwParam,pbData,dwFlags);
	if(dwParam != KP_CERTIFICATE)
	{
		for(i=0;i<(long)strlen((char *)pbData);i++)
			fprintf(fp,"%02x",pbData[i]);
	}
	else
	{
		for(i=0;i<pbData[2]*256+pbData[3]+4;i++)
		{
			fprintf(fp,"%02x ",pbData[i]);
			if(i%63==0 && i!=0)
				fprintf(fp,"\n");
		}
	}
	fprintf(fp,"\n");

	fclose(fp);
#endif
    
	struct CspContainer *pstrCspCt;
	struct KeyObject *pKeyObject;
	pstrCspCt=(struct CspContainer *)hProv;
	pKeyObject=(struct KeyObject *)hKey;
	DWORD dwLen;
	
	

	if(pbData==NULL)
	{
		SetLastError(ERROR_MORE_DATA);
		return FALSE;
	}
	switch(dwParam)
	{
	case KP_SALT:
		UKeyTestDisplayNum(pstrCspCt->hUKey, (BYTE)dwFlags);
		break;
	case KP_SALT_EX:
		if(pKeyObject->bIsKeyPairs)
		{
			SetLastError(NTE_BAD_TYPE);
			return FALSE;
		}
		memcpy(pKeyObject->bSalt,pbData,pKeyObject->dwSaltLen/8);
		break;
	case KP_PERMISSIONS:
		memcpy(&pKeyObject->dwPermissions,pbData,sizeof(DWORD));
		break;
	case KP_IV:
		memcpy(pKeyObject->bIV,pbData,8);
		memcpy(pKeyObject->bIVFact,pbData,8);
		pKeyObject->dwIVLen=8 * 8;
		break;
	case KP_PADDING:
		memcpy(&pKeyObject->dwPadding,pbData,sizeof(DWORD));
		break;
	case KP_MODE:
		memcpy(&pKeyObject->dwMode,pbData,sizeof(DWORD));
		break;
	case KP_MODE_BITS:
		break;
	case KP_EFFECTIVE_KEYLEN:
		memcpy(&pKeyObject->dwEffectiveKeyLen,pbData,sizeof(DWORD));
		break;
	case KP_CERTIFICATE:
		//证书二进制采用TLV（TAG-LENGTH-VALUE）格式。证书二进制数据总是以 0x30,0x82,SizeHighByte,SizeLowByte,….这些开头。
		//证书大小不包括开始4个字节，因此应该加上4字节去计算证书的大小。证书为DER编码
		dwLen = pbData[2]*256+pbData[3]+4 ;
		if(!UKeyAppendCert(pstrCspCt->hUKey, (BYTE *)&pstrCspCt->strCardContainer, (BYTE *)pbData, dwLen))
		{
			SetLastError(NTE_FAIL);
			DEBUG_MSG("Update CERTIFICATE Err !","CPSetKeyParam !");
			return FALSE;
		}
		BYTE ContainerName[64];
		strcpy((char *)ContainerName, (char *) pstrCspCt->strCardContainer.pbContName);
		strcpy((char *) pstrCspCt->strCardContainer.pbContName, (char *) pstrCspCt->strCardContainer.pbCertName);
		if(!UKeyUpdateContainer(pstrCspCt->hUKey, ContainerName, (BYTE *)&pstrCspCt->strCardContainer))
		{
			SetLastError(NTE_FAIL);
			DEBUG_MSG("Update CardContainer Err !","CPSetKeyParam !");
			return FALSE;
		}

		break;

//	case 0x30:
//		UKeyTestDisplayNum(pstrCspCt->hUKey, (BYTE)dwFlags);
//		break;
	default:
		SetLastError(NTE_BAD_TYPE);
		return FALSE;	
	}
    return TRUE;
}


/*
 -  CPGetKeyParam
 -
 *  Purpose:
 *                Allows applications to get various aspects of the
 *                operations of a key
 *
 *  Parameters:
 *               IN      hProv      -  Handle to a CSP
 *               IN      hKey       -  Handle to a key
 *               IN      dwParam    -  Parameter number
 *               OUT     pbData     -  Pointer to data
 *               IN      pdwDataLen -  Length of parameter data
 *               IN      dwFlags    -  Flags values
 *
 *  Returns:
 */

BOOL WINAPI
CPGetKeyParam1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  DWORD dwParam,
    OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen,
    IN  DWORD dwFlags)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	DWORD len = *pcbDataLen;
	fprintf(fp,"CPGetKeyParam \t%x \t%x \t%x \t%x\t%x\t%x\n",hProv,hKey,dwParam,pbData,len,dwFlags);
	fclose(fp);
#endif
    
	struct CspContainer *pstrCspCt;
	struct KeyObject *pKeyObject;
	pstrCspCt=(struct CspContainer *)hProv;
	pKeyObject=(struct KeyObject *)hKey;
		
#ifdef DEBUG_MODE
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	
	fprintf(fp,"CPGetKeyParam wCertSize=%d, pbContName=%s, hProv=%x\n",pstrCspCt->strCardContainer.wCertSize, pstrCspCt->strCardContainer.pbContName,hProv);
	fclose(fp);
#endif

	if(pbData==NULL)
	{
		if(dwParam != KP_CERTIFICATE)
			*pcbDataLen = 4;
		else
		{
			//	if(!UKeyGetCertLen(pstrCspCt->hUKey,pstrCspCt->strCardContainer.pbContName,pcbDataLen))
			//		return FALSE;
			*pcbDataLen = pstrCspCt->strCardContainer.wCertSize;
		}
		return TRUE;
	}
	switch(dwParam)
	{
	case KP_ALGID:
		memcpy(pbData,&pKeyObject->Algid,sizeof(DWORD));
		*pcbDataLen=sizeof(DWORD);
		break;
	case KP_BLOCKLEN:
		memcpy(pbData,&pKeyObject->dwBlockLen,sizeof(DWORD));
		*pcbDataLen=sizeof(DWORD);
		break;
	case KP_KEYLEN:
		memcpy(pbData,&pKeyObject->dwKeyLen,sizeof(DWORD));
		*pcbDataLen=sizeof(DWORD);
		break;
	case KP_SALT:
		break;
	case KP_PERMISSIONS:
		memcpy(pbData,&pKeyObject->dwPermissions,sizeof(DWORD));
		*pcbDataLen=sizeof(DWORD);
		break;
	case KP_IV:
		break;
	case KP_PADDING:
		memcpy(pbData,&pKeyObject->dwPadding,sizeof(DWORD));
		*pcbDataLen=sizeof(DWORD);
		break;
	case KP_MODE:
		memcpy(pbData,&pKeyObject->dwMode,sizeof(DWORD));
		*pcbDataLen=sizeof(DWORD);
		break;
	case KP_CERTIFICATE:
		if(!UKeyGetCert(pstrCspCt->hUKey, (BYTE *)&pstrCspCt->strCardContainer, pbData, *pcbDataLen))
			return FALSE;
		break;

	case KP_MODE_BITS:
		break;
	case KP_EFFECTIVE_KEYLEN:
		break;
	default:
		SetLastError(NTE_BAD_TYPE);
		DEBUG_MSG("dwParam Not Surport !","CPGetKeyParam");
		return FALSE;	
	}

    return TRUE;
}


/*
 -  CPSetProvParam
 -
 *  Purpose:
 *                Allows applications to customize various aspects of the
 *                operations of a provider
 *
 *  Parameters:
 *               IN      hProv   -  Handle to a CSP
 *               IN      dwParam -  Parameter number
 *               IN      pbData  -  Pointer to data
 *               IN      dwFlags -  Flags values
 *
 *  Returns:
 */

BOOL WINAPI
CPSetProvParam1(
    IN  HCRYPTPROV hProv,
    IN  DWORD dwParam,
    IN  CONST BYTE *pbData,
    IN  DWORD dwFlags)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPSetProvParam \t%x\t%x\t%s\t%x\n",hProv,dwParam,pbData,dwFlags);
	fclose(fp);
#endif
	struct CspContainer *pstrCspCt;
	pstrCspCt=(struct CspContainer *)hProv;
    if (dwParam == PP_KEYEXCHANGE_PIN)
 	{
		 if(!UKeyVerifyPIN(pstrCspCt->hUKey, (char *)pbData, strlen((char *)pbData)))
		 {
			 SetLastError(NTE_FAIL);
			 return FALSE;
		 }

 	}
// 	else
// 	{
// 		SetLastError(NTE_BAD_TYPE);
// #ifdef DEBUG_MODE
// 		FILE *fp;
// 		fp=fopen("c:\\HED_CSP_LOG.txt","a+");
// 		fprintf(fp,"CPSetProvParam Not Supported Type %d \n",dwParam);
// 		fclose(fp);
// #endif
// 		return FALSE;
// 	}
    return TRUE;
}


/*
 -  CPGetProvParam
 -
 *  Purpose:
 *                Allows applications to get various aspects of the
 *                operations of a provider
 *
 *  Parameters:
 *               IN      hProv      -  Handle to a CSP
 *               IN      dwParam    -  Parameter number
 *               OUT     pbData     -  Pointer to data
 *               IN OUT  pdwDataLen -  Length of parameter data
 *               IN      dwFlags    -  Flags values
 *
 *  Returns:
 */

BOOL WINAPI
CPGetProvParam1(
    IN  HCRYPTPROV hProv,
    IN  DWORD dwParam,
    OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen,
    IN  DWORD dwFlags)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPGetProvParam \t%x \tdwParam:%x \t%s \t%x \t%x\n",hProv,dwParam,pbData,*pcbDataLen,dwFlags);
	fclose(fp);
#endif

	BOOL retVal=FALSE;
	struct CspContainer *pstrCspCt;
	pstrCspCt=(struct CspContainer *)hProv;
    *pcbDataLen = 0;

	if(dwFlags!=CRYPT_FIRST && dwFlags!=CRYPT_MACHINE_KEYSET && dwFlags!=0)
	{
		SetLastError(NTE_BAD_FLAGS);
#ifdef	DEBUG_MODE
		MessageBox(NULL,"dwParam Not Surport !","CPGetProvParam !",MB_OK);
#endif
		return FALSE;
	}

	switch (dwParam)
	{
	case PP_CONTAINER:
	case PP_UNIQUE_CONTAINER:
		*pcbDataLen=strlen((char *)pstrCspCt->strCardContainer.pbContName)+1;
		if(pbData!=NULL)
			strcpy((char *)pbData,(char *)pstrCspCt->strCardContainer.pbContName);
		retVal = TRUE ;
		break;
	case PP_ENUMALGS:
		//gAlgEnumalgs
		*pcbDataLen=sizeof(PROV_ENUMALGS);		
		if(dwFlags==CRYPT_FIRST)
			gAlgEnumalgs.bAlgIndex=0;
		if(gAlgEnumalgs.bAlgIndex > gAlgEnumalgs.bAlgMax)
		{
			SetLastError(ERROR_NO_MORE_ITEMS);
			return FALSE;
		}
		if(pbData!=NULL)
		{
			memcpy((char *)pbData,gAlgEnumalgs.strAlg+gAlgEnumalgs.bAlgIndex,sizeof(PROV_ENUMALGS));			
			gAlgEnumalgs.bAlgIndex++;
		}
		retVal = TRUE ;
		break;

	case PP_ENUMALGS_EX:
		//gAlgEnumalgsEx
		*pcbDataLen=sizeof(PROV_ENUMALGS_EX);		
		if(dwFlags==CRYPT_FIRST)
			gAlgEnumalgsEx.bAlgIndex=0;
		if(gAlgEnumalgsEx.bAlgIndex>gAlgEnumalgsEx.bAlgMax)
		{
			SetLastError(ERROR_NO_MORE_ITEMS);
			return FALSE;
		}
		if(pbData!=NULL)
		{
			memcpy((char *)pbData,&gAlgEnumalgsEx.strAlgEx[gAlgEnumalgsEx.bAlgIndex],sizeof(PROV_ENUMALGS_EX));								
			gAlgEnumalgsEx.bAlgIndex++;
		}

		retVal = TRUE ;
		break;
	case PP_ENUMCONTAINERS://列举密钥容器

		if (HaveGetContainer > 0)
		{
			SetLastError(ERROR_NO_MORE_ITEMS);		
		}
		else
		{
			*pcbDataLen=strlen((char *)pstrCspCt->strCardContainer.pbContName)+1;
			if(pbData!=NULL)
				strcpy((char *)pbData,(char *)pstrCspCt->strCardContainer.pbContName);
			retVal = TRUE ;
			HaveGetContainer ++;
		}
		break;
	case PP_IMPTYPE:
		*pcbDataLen=sizeof(pstrCspCt->dwImptype);
		if(pbData!=NULL)
			memcpy(pbData,&pstrCspCt->dwImptype,4);//可能要用memcpy()更合适，低位在前
		retVal = TRUE;
		break;

	case PP_NAME:
		*pcbDataLen=strlen(pstrCspCt->cProvider)+1;
		if(pbData!=NULL)
			strcpy((char *)pbData,pstrCspCt->cProvider);
		retVal =TRUE;
		break;
	case PP_VERSION:
		*pcbDataLen=sizeof(pstrCspCt->dwVersion);
		if(pbData!=NULL)
			memcpy(pbData,&pstrCspCt->dwVersion,4);
		retVal =TRUE;
		break;
	case PP_SIG_KEYSIZE_INC:
		*pcbDataLen=sizeof(pstrCspCt->wSigKeySizeInc);
		if(pbData!=NULL)
			memcpy(pbData,&pstrCspCt->wSigKeySizeInc,4);
		retVal=TRUE;
		break;
	case PP_KEYX_KEYSIZE_INC:
		*pcbDataLen=sizeof(pstrCspCt->wKeyxKeySizeInc);
		if(pbData!=NULL)
			memcpy(pbData,&pstrCspCt->wKeyxKeySizeInc,4);
		retVal=TRUE;
		break;
	//case PP_KEYSET_SEC_DESCR:

	case PP_PROVTYPE:
		*pcbDataLen=sizeof(pstrCspCt->dwProvType);
		if(pbData!=NULL)
			memcpy(pbData,&pstrCspCt->dwProvType,4);
		retVal = TRUE;
		break;

	default:
		retVal=FALSE;
		*pcbDataLen = 0;
		SetLastError(NTE_BAD_TYPE);
		break;
	}

    return retVal;
}


/*
 -  CPSetHashParam
 -
 *  Purpose:
 *                Allows applications to customize various aspects of the
 *                operations of a hash
 *
 *  Parameters:
 *               IN      hProv   -  Handle to a CSP
 *               IN      hHash   -  Handle to a hash
 *               IN      dwParam -  Parameter number
 *               IN      pbData  -  Pointer to data
 *               IN      dwFlags -  Flags values
 *
 *  Returns:
 */

BOOL WINAPI
CPSetHashParam1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  DWORD dwParam,
    IN  CONST BYTE *pbData,
    IN  DWORD dwFlags)
{
#ifdef DEBUG_MODE
   	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPSetHashParam \t%x \t%x \t%x\t%x\t%x\n",hProv,hHash,dwParam,pbData,dwFlags);
	fclose(fp);
#endif
    
	struct CspContainer *pstrCspCt;
	struct HashObject *pHashObject;
	pstrCspCt=(struct CspContainer *)hProv;
	pHashObject=(struct HashObject *)hHash;

	if(pbData==NULL)
	{
		SetLastError(ERROR_MORE_DATA);
		return FALSE;
	}	
	if(dwParam!=HP_HASHVAL)
	{
		SetLastError(NTE_BAD_TYPE);
		DEBUG_MSG("dwParam!=HP_HASHVAL !","CPSetHashParam !");
		return FALSE;
	}
	//需不需要加Hash值是否为空判断?
	memcpy(&pHashObject->pbHashData,pbData,pHashObject->HashLen);
	pHashObject->bIsFinished = TRUE ;
	
#ifdef	DEBUG_MODE
   	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"The Set HASH PARAM is \n");
	for(DWORD i = 0; i<pHashObject->HashLen; i++)
		fprintf(fp,"%02x",pbData[i]);
	fprintf(fp,"\n");
	fclose(fp);
#endif
	
	return TRUE;
}


/*
 -  CPGetHashParam
 -
 *  Purpose:
 *                Allows applications to get various aspects of the
 *                operations of a hash
 *
 *  Parameters:
 *               IN      hProv      -  Handle to a CSP
 *               IN      hHash      -  Handle to a hash
 *               IN      dwParam    -  Parameter number
 *               OUT     pbData     -  Pointer to data
 *               IN      pdwDataLen -  Length of parameter data
 *               IN      dwFlags    -  Flags values
 *
 *  Returns:
 */

BOOL WINAPI
CPGetHashParam1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  DWORD dwParam,
    OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen,
    IN  DWORD dwFlags)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPGetHashParam \t%x \t%x \t%x \t%x\t%x\t%x\n",hProv,hHash,dwParam,pbData,pcbDataLen,dwFlags);
	fclose(fp);
#endif
    
	struct CspContainer *pstrCspCt;
	struct HashObject *pHashObject;
	pstrCspCt=(struct CspContainer *)hProv;
	pHashObject=(struct HashObject *)hHash;
	
	switch(dwParam)
	{
	case HP_ALGID:
		*pcbDataLen=4;
		if(pbData==NULL)		
			return TRUE;
		memcpy(pbData,&pHashObject->Algid,4);
		break;	
	case HP_HASHSIZE:
		*pcbDataLen=4;
		if(pbData==NULL)
			return TRUE;
		memcpy(pbData,&pHashObject->HashLen,4);
		break;
	case HP_HASHVAL:
		*pcbDataLen=pHashObject->HashLen;
		if(pbData==NULL)
			return TRUE;
//为支持多次摘要
		if( !pHashObject->bIsFinished )
		{
			if(pHashObject->Algid==CALG_MD5)
				MD5_Final(pHashObject->pbHashData,&pHashObject->MD5_context);
			else if(pHashObject->Algid==CALG_SHA1)
				SHA1_Final (pHashObject->pbHashData, &pHashObject->SHA1_context);
			else if(pHashObject->Algid == CALG_SSL3_SHAMD5)
			{
				MD5_Final(pHashObject->pbHashData,&pHashObject->MD5_context);
				SHA1_Final (pHashObject->pbHashData + 16, &pHashObject->SHA1_context);
			}
			else
			{
				SetLastError(NTE_BAD_ALGID);
				DEBUG_MSG("Hash Algid Not Surport !","CPGetHashParam !");
				return FALSE;
			}
		}
		memcpy(pbData,&pHashObject->pbHashData,pHashObject->HashLen);
		break;
	default:
		SetLastError(NTE_BAD_TYPE);
		DEBUG_MSG("dwParam TYPE Unknow !","CPGetHashParam !");
		return FALSE;
	}
#ifdef DEBUG_MODE
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"GetHash返回：-> ");
	for(DWORD i=0;i<*pcbDataLen;i++)
		fprintf(fp,"%02x",pbData[i]);
	fprintf(fp,"\n");
	fclose(fp);
#endif

	pHashObject->bIsFinished=TRUE;
	return TRUE;
}


/*
 -  CPExportKey
 -
 *  Purpose:
 *                Export cryptographic keys out of a CSP in a secure manner
 *
 *
 *  Parameters:
 *               IN  hProv         - Handle to the CSP user
 *               IN  hKey          - Handle to the key to export
 *               IN  hPubKey       - Handle to exchange public key value of
 *                                   the destination user
 *               IN  dwBlobType    - Type of key blob to be exported
 *               IN  dwFlags       - Flags values
 *               OUT pbData        -     Key blob data
 *               IN OUT pdwDataLen - Length of key blob in bytes
 *
 *  Returns:
 */

BOOL WINAPI
CPExportKey1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  HCRYPTKEY hPubKey,
    IN  DWORD dwBlobType,
    IN  DWORD dwFlags,
    OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen)
{
//	AfxMessageBox("进入函数： CPExportKey");

#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+"); 	                                              
	fprintf(fp,"CPExportKey \thProv:%x \thKey:%x \t%x \t%x \t%x\t%x\t%x\n",hProv,hKey,hPubKey,dwBlobType,dwFlags,pbData,pcbDataLen);
	fclose(fp);
#endif

	DWORD i, k;
	BLOBHEADER blobheader;
	RSAPUBKEY rsapubkey;
	R_RSA_PUBLIC_KEY prblickey;
	R_RSA_PRIVATE_KEY privatekey; 
	struct CspContainer *pstrCspCt;
	struct KeyObject *pKeyObject;
	pstrCspCt=(struct CspContainer *)hProv;
	pKeyObject=(struct KeyObject *)hKey;	
	

	if(dwBlobType==PUBLICKEYBLOB)
	{
		*pcbDataLen=sizeof(BLOBHEADER)+sizeof(RSAPUBKEY)+sizeof(prblickey.modulus);
		if(pbData==NULL)
			return TRUE;
	
		if(!UKeyGetKey(pstrCspCt->hUKey, pKeyObject->wPsfi, PUBLIC_KEY_FLAG, (BYTE *)&prblickey))
		{
#ifdef DEBUG_MODE
			FILE *fp;
			fp=fopen("c:\\HED_CSP_LOG.txt","a+"); 	                                             
			fprintf(fp,"NO KEY");
			
			fclose(fp);
#endif
			SetLastError(NTE_NO_KEY);
			return FALSE;
		}
		
		blobheader.bType    = PUBLICKEYBLOB;      // 0x06
		blobheader.bVersion = CUR_BLOB_VERSION;   // 0x02
		blobheader.reserved = 0;                  // 0x0000
		blobheader.aiKeyAlg = pKeyObject->Algid;//CALG_RSA_KEYX;      // CALG_RSA_SIGN
		rsapubkey.magic     = 0x31415352;         //RSA1 RSA的标志，必须是这个值,说明是公钥
		rsapubkey.bitlen    = 1024;                // 公钥的长度
		//rsapubkey.pubexp    = 0x01000100;          //
		memcpy(&rsapubkey.pubexp, prblickey.exponent, sizeof(DWORD));			// 0x00010001注意：以卡为准

		memcpy(pbData,&blobheader,sizeof(BLOBHEADER));
		memcpy(pbData+sizeof(BLOBHEADER),&rsapubkey,sizeof(RSAPUBKEY));
		//memcpy(pbData+sizeof(BLOBHEADER)+sizeof(RSAPUBKEY),modulus,sizeof(modulus));
		int mlen = sizeof(prblickey.modulus);
		for(i=0; i<mlen; i++)
			pbData[sizeof(BLOBHEADER)+sizeof(RSAPUBKEY)+i]=prblickey.modulus[i];
		    //pbData[sizeof(BLOBHEADER)+sizeof(RSAPUBKEY)+i]=prblickey.modulus[sizeof(prblickey.modulus)-1-i];
			//pbData[sizeof(BLOBHEADER)+sizeof(RSAPUBKEY)+i]=modulus[sizeof(modulus)-1-i];
	}
/*	else if(dwBlobType == PRIVATEKEYBLOB)
	{
		if(!(pKeyObject->dwPermissions & CRYPT_EXPORT))
		{
			SetLastError(NTE_BAD_KEY_STATE);
			return FALSE;
		}
		*pcbDataLen=sizeof(BLOBHEADER)+sizeof(RSAPUBKEY)+sizeof(privatekey.modulus)+sizeof(privatekey.prime[0])
					+sizeof(privatekey.prime[1])+sizeof(privatekey.primeExponent[0])+sizeof(privatekey.primeExponent[1])
					+ sizeof(privatekey.coefficient)+sizeof(privatekey.exponent);
		if(pbData==NULL)
			return TRUE;
		if(!UKeyGetKey(pstrCspCt->hUKey, pKeyObject->wPsfi, PRIVATE_KEY_FLAG, (BYTE *)&privatekey))
		{
			SetLastError(SCARD_E_READER_UNAVAILABLE);
			return FALSE;
		}
		blobheader.bType    = PRIVATEKEYBLOB;     // 0x07
		blobheader.bVersion = CUR_BLOB_VERSION;   // 0x02
		blobheader.reserved = 0;                  // 0x0000
		blobheader.aiKeyAlg = CALG_RSA_KEYX;      // 0x0000a400
		rsapubkey.magic     = 0x32415352;         // "RSA2"
		rsapubkey.bitlen    = 1024;                // 0x00000200
		rsapubkey.pubexp    = 65537;              // 0x00010001
		memcpy(pbData,&blobheader,sizeof(BLOBHEADER));
		memcpy(pbData+sizeof(BLOBHEADER),&rsapubkey,sizeof(RSAPUBKEY));

		i = sizeof(BLOBHEADER)+sizeof(RSAPUBKEY);
		for(k=0;k<sizeof(privatekey.modulus);k++)
			pbData[i + k] = privatekey.modulus[sizeof(privatekey.modulus)- k - 1];
		i += sizeof(privatekey.modulus);
		for(k=0;k<sizeof(privatekey.prime[0]);k++)
			pbData[i + k] = privatekey.prime[0][sizeof(privatekey.prime[0])- k - 1];
		i += sizeof(privatekey.prime[0]);
		for(k=0;k<sizeof(privatekey.prime[1]);k++)
			pbData[i + k] = privatekey.prime[1][sizeof(privatekey.prime[1])- k - 1];
		i += sizeof(privatekey.prime[1]);
		for(k=0;k<sizeof(privatekey.primeExponent[0]);k++)
			pbData[i + k] = privatekey.primeExponent[0][sizeof(privatekey.primeExponent[0])- k - 1];
		i += sizeof(privatekey.primeExponent[0]);
		for(k=0;k<sizeof(privatekey.primeExponent[1]);k++)
			pbData[i + k] = privatekey.primeExponent[1][sizeof(privatekey.primeExponent[1])- k - 1];
		i += sizeof(privatekey.primeExponent[1]);
		for(k=0;k<sizeof(privatekey.coefficient);k++)
			pbData[i + k] = privatekey.coefficient[sizeof(privatekey.coefficient)- k - 1];
		i += sizeof(privatekey.coefficient);
		for(k=0;k<sizeof(privatekey.exponent);k++)
			pbData[i + k] = privatekey.exponent[sizeof(privatekey.exponent)- k - 1];
	}
	*/
	else
	{
		SetLastError(NTE_BAD_TYPE);
		DEBUG_MSG("dwBlobType=NTE_BAD_TYPE","CPExportKey");
		return FALSE;
	}

#ifdef DEBUG_MODE
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp, "pubexp=%02x,exponent->%02x,%02x,%02x,%02x", rsapubkey.pubexp, prblickey.exponent[0],prblickey.exponent[1],prblickey.exponent[2],prblickey.exponent[3]);
	fprintf(fp,"密钥为：\n");
	for(i=0;i<*pcbDataLen;i++)
		fprintf(fp,"%02x",pbData[i]);
	fprintf(fp,"\n");
	for(i=0;i<*pcbDataLen;i++)
		fprintf(fp,"%02x",pbData[*pcbDataLen-1-i]);
	fprintf(fp,"\n");
	fclose(fp);
#endif

    return TRUE;
}



/////////------------------------------------------------//////////////////
BOOL SetAlgDefaltKey(HCRYPTPROV hProv, struct KeyObject * pKeyObject,BLOBHEADER *pBlobHeader, DWORD dwFlags, BYTE *pbKey)
{
	struct CspContainer *pstrCspCt;
	WORD wSsfi = 0, wPsfi = 0;
	R_RSA_PUBLIC_KEY publickey;
	R_RSA_PRIVATE_KEY privatekey;

	WORD wPosition = 0;
	BOOL bRetVal = 0;
	#ifdef DEBUG_MODE
		FILE *fp;
	#endif

	pstrCspCt=(struct CspContainer *)hProv;

	if(pBlobHeader->bType == SIMPLEBLOB)
	{
		if(pBlobHeader->aiKeyAlg == CALG_RC2)
		{
			pKeyObject->Algid=pBlobHeader->aiKeyAlg;
			pKeyObject->dwBlockLen=8*8;
			pKeyObject->dwKeyLen=5*8;
			memset(pKeyObject->bKey,0,sizeof(pKeyObject->bKey));
			memcpy(pKeyObject->bKey,pbKey,pKeyObject->dwKeyLen/8);
			pKeyObject->dwSaltLen=11*8;
			memset(pKeyObject->bSalt,0,sizeof(pKeyObject->bSalt));
			pKeyObject->dwPermissions = 0xffffffff;
			pKeyObject->dwIVLen=8*8;
			memset(pKeyObject->bIV,0,sizeof(pKeyObject->bIV));
			pKeyObject->dwPadding=PKCS5_PADDING;
			pKeyObject->dwMode=CRYPT_MODE_CBC;//??
			pKeyObject->dwModeBits=0;//只有OFB方式有此属性
			pKeyObject->dwEffectiveKeyLen=5*8;//????
			pKeyObject->bIsKeyPairs=FALSE;
		}
		else if(pBlobHeader->aiKeyAlg == CALG_3DES)
		{
			pKeyObject->Algid = pBlobHeader->aiKeyAlg;
			pKeyObject->dwBlockLen = 8 * 8;
			pKeyObject->dwKeyLen = 24 * 8;
			memset(pKeyObject->bKey, 0, sizeof(pKeyObject->bKey));
			memcpy(pKeyObject->bKey, pbKey, pKeyObject->dwKeyLen / 8);
			pKeyObject->dwSaltLen = 0;
			memset(pKeyObject->bSalt,0,sizeof(pKeyObject->bSalt));
			pKeyObject->dwPermissions = 0xffffffff;
			pKeyObject->dwIVLen = 8 * 8;
			memset(pKeyObject->bIV, 0, sizeof(pKeyObject->bIV));
			pKeyObject->dwPadding = PKCS5_PADDING;
			pKeyObject->dwMode = CRYPT_MODE_CBC;//??
			pKeyObject->dwModeBits = 0;//只有OFB方式有此属性
			pKeyObject->dwEffectiveKeyLen = 24 * 8;//????
			pKeyObject->bIsKeyPairs = FALSE;	
		}
		else//CALG_DES....
		{
			SetLastError(NTE_BAD_ALGID);			
			DEBUG_MSG("SIMPLEBLOB : NTE_BAD_ALGID","SetAlgDefaltKey");
			return FALSE;
		}	
		//debug only
		#ifdef DEBUG_MODE
			fp=fopen("c:\\HED_CSP_LOG.txt","a+");
			fprintf(fp,"密钥材料为：\n");
			for(DWORD i=0;i<pKeyObject->dwKeyLen / 8;i++)
				fprintf(fp,"%02x",pKeyObject->bKey[i]);
			fprintf(fp,"\n");
			fclose(fp);
		#endif
	}
/*	else if(pBlobHeader->bType == PRIVATEKEYBLOB)
	{
		wSsfi = pstrCspCt->strCardContainer.wKeySSfi;
		wPsfi = pstrCspCt->strCardContainer.wKeyPSfi;
		if(wSsfi == 0 || wPsfi == 0)//add key pairs
		{//得到当前公私钥文件封需装成函数
			if(!UKeyGetNextKeyAddr(pstrCspCt->hUKey,&wSsfi,&wPsfi))
			{
				if(GetUkeyLastErr() == KEY_NO_CONTEXT)
					SetLastError(NTE_BAD_UID);
				else if(GetUkeyLastErr() == KEY_NO_SPACE )
				{
					SetLastError(NTE_NO_MEMORY);
					MessageBox(NULL,"The Size of Key Is Not Enough !","SetAlgDefaltKey !",MB_OK);
				}
				else
					SetLastError(SCARD_E_READER_UNAVAILABLE);
				return FALSE;
			}
		}
		memcpy(&privatekey,pbKey,sizeof(R_RSA_PRIVATE_KEY));
		memcpy(publickey.modulus,privatekey.modulus,sizeof(publickey.modulus));
		memcpy(publickey.exponent,privatekey.publicExponent,sizeof(publickey.exponent));
		publickey.bits = privatekey.bits;
		if(!UKeyUpdateKey(pstrCspCt->hUKey,pstrCspCt->strCardContainer.pbContName,
			pBlobHeader->aiKeyAlg, dwFlags, wSsfi,wPsfi,&privatekey,&publickey))

		if(!UKeyGetContainer(pstrCspCt->hUKey,pstrCspCt->strCardContainer.pbContName,&pstrCspCt->strCardContainer))
		{
			if(GetUkeyLastErr() == KEY_NO_CONTEXT)
				SetLastError(NTE_BAD_UID);
			else
				SetLastError(SCARD_E_READER_UNAVAILABLE);
			return FALSE;
		}
		//set the pKeyObject attributes
		memset(pKeyObject,0,sizeof(struct KeyObject));
		pKeyObject->wSsfi = wSsfi;	
		pKeyObject->wPsfi = wPsfi;
		pKeyObject->Algid = pBlobHeader->bType;
		pKeyObject->dwBlockLen = 1024;//HIWORD(dwFlags);
		pKeyObject->dwKeyLen = 1024;//HIWORD(dwFlags);
		if(dwFlags & CRYPT_EXPORTABLE)
			pKeyObject->dwPermissions = CRYPT_ENCRYPT|CRYPT_DECRYPT|CRYPT_EXPORT|CRYPT_READ|CRYPT_WRITE;
		else
			pKeyObject->dwPermissions = CRYPT_ENCRYPT|CRYPT_DECRYPT|CRYPT_WRITE;
		pKeyObject->dwPadding=PKCS5_PADDING;
		pKeyObject->dwMode=CRYPT_MODE_ECB;
		pKeyObject->dwModeBits=0;//只有OFB方式有此属性
		pKeyObject->dwEffectiveKeyLen=0;//????
		pKeyObject->bIsKeyPairs=TRUE;
	}
	else if(pBlobHeader->bType == PUBLICKEYBLOB)
	{
		DEBUG_MSG("pBlobHeader->bType == PUBLICKEYBLOB !","SetAlgDefaltKey !");
		return FALSE;
	}
	else
	{
		SetLastError(NTE_BAD_TYPE);
		DEBUG_MSG("BAD pBlobHeader->bType!","SetAlgDefaltKey !");
		return FALSE;
	}*/
	return TRUE;
}
/////////------------------------------------------------//////////////////
/*
 -  CPImportKey
 -
 *  Purpose:
 *                Import cryptographic keys
 *
 *
 *  Parameters:
 *               IN  hProv     -  Handle to the CSP user
 *               IN  pbData    -  Key blob data
 *               IN  dwDataLen -  Length of the key blob data
 *               IN  hPubKey   -  Handle to the exchange public key value of
 *                                the destination user
 *               IN  dwFlags   -  Flags values
 *               OUT phKey     -  Pointer to the handle to the key which was
 *                                Imported
 *
 *  Returns:
 */

BOOL WINAPI
CPImportKey1(
    IN  HCRYPTPROV hProv,
    IN  CONST BYTE *pbData,
    IN  DWORD cbDataLen,
    IN  HCRYPTKEY hPubKey,
    IN  DWORD dwFlags,
    OUT HCRYPTKEY *phKey)
{
	DWORD i,k;
	BOOL bRetVal=0;
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPImportKey \t%x \t%x \t%x \t%x\t%x\t%x \n",hProv,pbData,cbDataLen,hPubKey,dwFlags,phKey);
	fprintf(fp,"倒入密钥数据为：\n");
	for(i=0;i<cbDataLen;i++)
		fprintf(fp,"%02x",*(pbData+i));
	fprintf(fp,"\n");
	fprintf(fp,"倒入密钥转换为：\n");
	for(i=0;i<cbDataLen;i++)
		fprintf(fp,"%02x",*(pbData+cbDataLen-i-1));
	fprintf(fp,"\n");
	fclose(fp);
#endif

	BYTE pbInData[1024],pbOutData[1024], pIn[1024];
	BLOBHEADER blobheader;
	RSAPUBKEY rsapubkey;
	R_RSA_PUBLIC_KEY publickey;
	R_RSA_PRIVATE_KEY privatekey; 
	int pcbOutLen = 0;
	int InLen = 128;
	int xLen = 0;
//	ALG_ID algid;
	struct CspContainer *pstrCspCt;
	struct KeyObject *pPubKeyObject,*pKeyObject;
	pstrCspCt=(struct CspContainer *)hProv;
	pPubKeyObject=(struct KeyObject *)hPubKey;

	pKeyObject=(struct KeyObject *)malloc(sizeof(struct KeyObject));
	if(pKeyObject==NULL)
	{
		SetLastError(NTE_NO_MEMORY);
		DEBUG_MSG("malloc->NTE_NO_MEMORY !","CPImportKey !");
		return FALSE;
	}

	memcpy(&blobheader,pbData,sizeof(BLOBHEADER));
	if(blobheader.bVersion!=CUR_BLOB_VERSION)
	{
		SetLastError(NTE_BAD_VER);
		DEBUG_MSG("blobheader.bVersion!=CUR_BLOB_VERSION !","CPImportKey !");
		goto CPImportKeyOut;
	}	

	memset(pbInData,0,sizeof(pbInData));
	memset(pbOutData,0,sizeof(pbOutData));
	memset(&publickey,0,sizeof(R_RSA_PUBLIC_KEY));
	memset(&privatekey,0,sizeof(R_RSA_PRIVATE_KEY));
#ifdef DEBUG_MODE	
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"blobheader.bType：%x \n", blobheader.bType);
	fclose(fp);
#endif
	if(blobheader.bType == SIMPLEBLOB)
	{
		xLen = cbDataLen-sizeof(BLOBHEADER)-sizeof(ALG_ID);
		if(!hPubKey)//没有保护密钥
		{
			for(i=0;i< xLen;i++)
				pbOutData[i]=pbData[cbDataLen-i];
		}
		else if(pPubKeyObject->Algid == CALG_RSA_KEYX || pPubKeyObject->Algid == CALG_RSA_SIGN)
		{
			for(i=0;i<xLen;i++)
				pbInData[i]=pbData[cbDataLen-1-i];
// 			
// 			if(!UKeyPrivateKeyDecrypt(pstrCspCt->hUKey,pPubKeyObject->wSsfi, pbInData, pbOutData))
// 			{
// 				DEBUG_MSG("UKeyPrivateKeyDecrypt Error !","CPImportKey !");
// 				if(GetUkeyLastErr() == KEY_DECRYPT_ERROR)
// 					SetLastError(NTE_FAIL);
// 				else
// 					SetLastError(SCARD_E_READER_UNAVAILABLE);
// 				goto CPImportKeyOut;
// 			}
#ifdef DEBUG_MODE
			fp=fopen("c:\\HED_CSP_LOG.txt","a+");
			fprintf(fp,"pPubKeyObject->wPsfi=：%x,%x, xLen=%d, InLen=%d\n", pPubKeyObject->wSsfi,pPubKeyObject->wPsfi, xLen, InLen);
			fclose(fp);
#endif
			for (i=0; i<xLen; i++)
				pIn[i] = pbInData[xLen - 1 -i];
			//if (!UKeyRSASignature(pstrCspCt->hUKey, pPubKeyObject->wSsfi, pbInData, InLen, pbOutData, &pcbOutLen))
			if (!UKeyRSADecrypt(pstrCspCt->hUKey, pPubKeyObject->wSsfi, pIn, &InLen, pbOutData, &pcbOutLen))
			{
				SetLastError(NTE_FAIL);
				return FALSE;
			}
			for (i=0; i<pcbOutLen; i++)
				pIn[i] = pbOutData[pcbOutLen - 1 -i];
			for (i=0; i<pcbOutLen; i++)
				pbOutData[i] = pIn[i];
		}
		else//保护密钥为对称密钥
		{
			DEBUG_MSG("保护密钥可能为对称密钥","CPImportKey !");
			SetLastError(NTE_BAD_ALGID);
			goto CPImportKeyOut;
		}
		for(i = sizeof(BLOBHEADER) + sizeof(ALG_ID); i< cbDataLen; i++)	//only for SIMPLEBLOB
			if(!pbOutData[i]) break;  //find the 00 data
		if(i == cbDataLen)
			goto CPImportKeyOut;
		if(!SetAlgDefaltKey(hProv,pKeyObject,&blobheader, dwFlags, pbOutData + i + 1))
			goto CPImportKeyOut;
		else
			bRetVal = TRUE ;

		#ifdef DEBUG_MODE
			fp=fopen("c:\\HED_CSP_LOG.txt","a+");
			fprintf(fp,"倒入密钥解密后数据为：\n");
			for(i=0;i<128;i++)
				fprintf(fp,"%02x",pbOutData[i]);
			fprintf(fp,"\n");
			fclose(fp);
		#endif
	}
/*	else if(blobheader.bType == PRIVATEKEYBLOB)
	{
		memcpy(&rsapubkey,pbData+sizeof(BLOBHEADER),sizeof(RSAPUBKEY));
		privatekey.bits = rsapubkey.bitlen;
		//加入rsapubkey的合法性判断
		i = sizeof(BLOBHEADER)+sizeof(RSAPUBKEY);
		for(k=0;k<sizeof(privatekey.modulus);k++)
			privatekey.modulus[sizeof(privatekey.modulus)- k - 1] = pbData[i + k];
		i += sizeof(privatekey.modulus);
		for(k=0;k<sizeof(privatekey.prime[0]);k++)
			privatekey.prime[0][sizeof(privatekey.prime[0])- k - 1] = pbData[i + k];
		i += sizeof(privatekey.prime[0]);
		for(k=0;k<sizeof(privatekey.prime[1]);k++)
			privatekey.prime[1][sizeof(privatekey.prime[1])- k - 1] = pbData[i + k];
		i += sizeof(privatekey.prime[1]);
		for(k=0;k<sizeof(privatekey.primeExponent[0]);k++)
			privatekey.primeExponent[0][sizeof(privatekey.primeExponent[0])- k - 1] = pbData[i + k];
		i += sizeof(privatekey.primeExponent[0]);
		for(k=0;k<sizeof(privatekey.primeExponent[1]);k++)
			privatekey.primeExponent[1][sizeof(privatekey.primeExponent[1])- k - 1] = pbData[i + k];
		i += sizeof(privatekey.primeExponent[1]);
		for(k=0;k<sizeof(privatekey.coefficient);k++)
			privatekey.coefficient[sizeof(privatekey.coefficient)- k - 1] = pbData[i + k];
		i += sizeof(privatekey.coefficient);
		for(k=0;k<sizeof(privatekey.exponent);k++)
			privatekey.exponent[sizeof(privatekey.exponent)- k - 1] = pbData[i + k];
		i += sizeof(privatekey.exponent);
		memset(privatekey.publicExponent,0,sizeof(privatekey.publicExponent));
		i = sizeof(privatekey.publicExponent) - sizeof(rsapubkey.pubexp) ;
		memcpy(privatekey.publicExponent + i ,"\x00\x01\x00\x01",4);
		
		if(!SetAlgDefaltKey(hProv, pKeyObject, &blobheader, dwFlags, (BYTE *)&privatekey ))
			goto CPImportKeyOut;
		else
			bRetVal = TRUE ;

#ifdef DEBUG_MODE
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"\n++++++++++++++++++++++++++++++++++++++++++\n");
	fprintf(fp,"公钥数据为：\n");
	fprintf(fp,"publickey.bits:	\n\t%04x \n",publickey.bits);
	fprintf(fp,"publickey.exponent:\n\t");
	for(i=0;i<sizeof(publickey.exponent);i++)
		fprintf(fp,"%02x",publickey.exponent[i]);
	fprintf(fp,"\npublickey.modulus:\n\t");
	for(i=0;i<sizeof(publickey.modulus);i++)
		fprintf(fp,"%02x",publickey.modulus[i]);

	fprintf(fp,"\n私钥数据为：\n");
	fprintf(fp,"privatekey.bits:	\n\t%04x \n",privatekey.bits);
	fprintf(fp,"privatekey.modulus:\n\t");
	for(i=0;i<sizeof(privatekey.modulus);i++)
		fprintf(fp,"%02x",privatekey.modulus[i]);

	fprintf(fp,"\nprivatekey.publicExponent:\n\t");
	for(i=0;i<sizeof(privatekey.publicExponent);i++)
		fprintf(fp,"%02x",privatekey.publicExponent[i]);

	fprintf(fp,"\npublickey.exponent:\n\t");
	for(i=0;i<sizeof(privatekey.exponent);i++)
		fprintf(fp,"%02x",privatekey.exponent[i]);

	fprintf(fp,"\npublickey.prime[0]:\n\t");
	for(i=0;i<sizeof(privatekey.prime[0]);i++)
		fprintf(fp,"%02x",privatekey.prime[0][i]);

	fprintf(fp,"\npublickey.prime[1]:\n\t");
	for(i=0;i<sizeof(privatekey.prime[1]);i++)
		fprintf(fp,"%02x",privatekey.prime[1][i]);

	fprintf(fp,"\npublickey.primeExponent[0]:\n\t");
	for(i=0;i<sizeof(privatekey.primeExponent[0]);i++)
		fprintf(fp,"%02x",privatekey.primeExponent[0][i]);

	fprintf(fp,"\npublickey.primeExponent[1]:\n\t");
	for(i=0;i<sizeof(privatekey.primeExponent[1]);i++)
		fprintf(fp,"%02x",privatekey.primeExponent[1][i]);

	fprintf(fp,"\npublickey.coefficient:\n\t");
	for(i=0;i<sizeof(privatekey.coefficient);i++)
		fprintf(fp,"%02x",privatekey.coefficient[i]);

	fprintf(fp,"\n");
	fclose(fp);
#endif
		#ifdef DEBUG_MODE
			memcpy(pbOutData,&privatekey,sizeof(R_RSA_PRIVATE_KEY));
			fp=fopen("c:\\HED_CSP_LOG.txt","a+");
			fprintf(fp,"倒入私钥数据为：\n");
			for(i=0;i<sizeof(R_RSA_PRIVATE_KEY);i++)
				fprintf(fp,"%02x",pbOutData[i]);
			fprintf(fp,"\n");
			fclose(fp);
		#endif
			
	}
	else */

	else if(blobheader.bType == PUBLICKEYBLOB)
	{
		memset(pKeyObject,0,sizeof(struct KeyObject));
		pKeyObject->Algid = blobheader.aiKeyAlg;
		pKeyObject->bIsKeyPairs = FALSE;
		pKeyObject->wSsfi = 0x1C;	
		pKeyObject->wPsfi = 0x1D;

		pKeyObject->dwBlockLen=1024;//HIWORD(dwFlags);
		pKeyObject->dwKeyLen=1024;//HIWORD(dwFlags);
		if(dwFlags & CRYPT_EXPORTABLE)
			pKeyObject->dwPermissions = CRYPT_ENCRYPT|CRYPT_DECRYPT|CRYPT_EXPORT|CRYPT_READ|CRYPT_WRITE;
		else
			pKeyObject->dwPermissions = CRYPT_ENCRYPT|CRYPT_DECRYPT|CRYPT_WRITE;
		pKeyObject->dwPadding=PKCS5_PADDING;
		pKeyObject->dwMode=CRYPT_MODE_ECB;
		pKeyObject->dwModeBits=0;//只有OFB方式有此属性
		pKeyObject->dwEffectiveKeyLen=0;//????
		pKeyObject->bIsKeyPairs=TRUE;

		
		i = sizeof(BLOBHEADER);
		memcpy(&rsapubkey,pbData+i,sizeof(RSAPUBKEY));
		publickey.bits = rsapubkey.bitlen;

		i += sizeof(RSAPUBKEY);
		memcpy(publickey.modulus,pbData+i,sizeof(publickey.modulus));
		i = sizeof(publickey.exponent) - sizeof(rsapubkey.pubexp);
		memcpy(publickey.exponent,&rsapubkey.pubexp,sizeof(rsapubkey.pubexp));
		if(!UKeyImportKey(pstrCspCt->hUKey, pKeyObject->wPsfi, (BYTE *)&publickey))
			goto CPImportKeyOut;
		else
			bRetVal = TRUE ;

		#ifdef DEBUG_MODE
			memcpy(pbOutData,&publickey,sizeof(R_RSA_PUBLIC_KEY));
			fp=fopen("c:\\HED_CSP_LOG.txt","a+");
			fprintf(fp,"倒入公钥填充后数据为：\n");
			for(i=0;i<sizeof(R_RSA_PUBLIC_KEY);i++)
				fprintf(fp,"%02x",pbOutData[i]);
			fprintf(fp,"\n");
			fclose(fp);
		#endif
	}
	else
	{
		SetLastError(NTE_BAD_TYPE);
		DEBUG_MSG("BAD blobheader.bType !","CPImportKey !");
		goto CPImportKeyOut;
	}


CPImportKeyOut:
	if(!bRetVal)
	{
		if(pKeyObject!=NULL)
			free(pKeyObject);
		//debug only
		DEBUG_MSG("return FALSE !","CPImportKey  !");
		return FALSE;
	}
	
	*phKey = (HCRYPTKEY)pKeyObject;
	KeyImportFlag = TRUE;
    return TRUE;
}


/*
 -  CPEncrypt
 -
 *  Purpose:
 *                Encrypt data
 *
 *
 *  Parameters:
 *               IN  hProv         -  Handle to the CSP user
 *               IN  hKey          -  Handle to the key
 *               IN  hHash         -  Optional handle to a hash
 *               IN  Final         -  Boolean indicating if this is the final
 *                                    block of plaintext
 *               IN  dwFlags       -  Flags values
 *               IN OUT pbData     -  Data to be encrypted
 *               IN OUT pdwDataLen -  Pointer to the length of the data to be
 *                                    encrypted
 *               IN dwBufLen       -  Size of Data buffer
 *
 *  Returns:
 */
//需添加算法 3DES,DES等

BOOL WINAPI
CPEncrypt1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  HCRYPTHASH hHash,
    IN  BOOL fFinal,
    IN  DWORD dwFlags,
    IN OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen,
    IN  DWORD cbBufLen)
{
   	DWORD i,j;
	unsigned short xkey[64];
	BYTE key[128],iv[8],buf[128];
	
	//	AfxMessageBox("进入函数： CPEncrypt");
	
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPEncrypt \t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\n",hProv,hKey,hHash,fFinal,dwFlags,pbData,pcbDataLen,cbBufLen);
	for(i=0;i<*pcbDataLen;i++)
	{
		fprintf(fp,"0x%02x,",pbData[i]);
		if((i+1)%64==0) fprintf(fp,"\n");
	}
	fprintf(fp,"\n");
	fclose(fp);
#endif
	
	struct CspContainer *pstrCspCt;
	struct KeyObject *pKeyObject;
	
	pstrCspCt=(struct CspContainer *) hProv;
	pKeyObject=(struct KeyObject *) hKey;
	
	if(*pcbDataLen==0 || (*pcbDataLen % 8!=0 && !fFinal))
	{
		SetLastError(NTE_BAD_LEN);
		DEBUG_MSG("DEBUG:NTE_BAD_LEN","CPEncrypt");
		return FALSE;
	}
	memset(key,0,sizeof(key));
	memset(buf,0,sizeof(buf));
	memset(iv,0,sizeof(iv));
	
	if(fFinal)
	{
		i = 8 - *pcbDataLen % 8;
		*pcbDataLen = *pcbDataLen - i;
		if(*pcbDataLen > cbBufLen)
		{
			SetLastError(NTE_BAD_LEN);
			return FALSE;
		}
		memset(pbData + *pcbDataLen - i, i , i);//padding here
	}
	
	if(pKeyObject->Algid==CALG_RC2)
	{
		memcpy(key,pKeyObject->bKey,pKeyObject->dwKeyLen/8);
		memcpy(key+pKeyObject->dwKeyLen/8,pKeyObject->bSalt,pKeyObject->dwSaltLen/8);
		rc2_keyschedule(xkey,key,pKeyObject->dwEffectiveKeyLen/8,pKeyObject->dwEffectiveKeyLen);
		memcpy(iv,pKeyObject->bIVFact,pKeyObject->dwIVLen/8);
		for(j=0;j<*pcbDataLen;j+=8)
		{
			for(i=0;i<8;i++)
				buf[i]=buf[i]^iv[i];
			memcpy(iv,pbData+j,8);
			rc2_encrypt( xkey,pbData+j,buf);
			memcpy(pbData+j,buf,8);
		}
		memcpy(pKeyObject->bIVFact,iv,8);
#ifdef DEBUG_MODE
		fp=fopen("c:\\HED_CSP_LOG.txt","a+");
		fprintf(fp,"\n\n\n加密后数据为：\n");
		for(i=0;i<*pcbDataLen;i++)
		{
			fprintf(fp,"0x%02x,",pbData[i]);
			if((i+1)%64==0) fprintf(fp,"\n");
		}
		fprintf(fp,"\n");
		fclose(fp);
#endif
	}
	else if (pKeyObject->Algid == CALG_RSA_SIGN)
	{
		int nDataLen = *pcbDataLen;
		int nReqPos = 0;
		int nRespPos = 0;
		int nLen = ENCRYPT_MAX_DATALEN;
		BYTE RespData[ENCRYPT_MAX_DATALEN];
		BYTE ReqData[ENCRYPT_MAX_DATALEN];
		
		PBYTE pbDataTemp = new BYTE[cbBufLen];
		// 如果长度超过ENCRYPT_MAX_DATALEN，按照每次加密ENCRYPT_MAX_DATALEN逐段加密
		while (nDataLen >= ENCRYPT_MAX_DATALEN)
		{
			memcpy(ReqData, (LPBYTE)&pbData[nReqPos], ENCRYPT_MAX_DATALEN);
			if (!UKeyRSAEncrypt(pstrCspCt->hUKey, pKeyObject->wPsfi, ReqData, &nLen, RespData, &nLen))
			{
				delete [] pbDataTemp;
				SetLastError(NTE_FAIL);
				return FALSE;
			}
			memcpy((PBYTE)&pbDataTemp[nRespPos], RespData, nLen);
			nDataLen -= ENCRYPT_MAX_DATALEN;
			nReqPos += ENCRYPT_MAX_DATALEN;
			nRespPos += nLen;
		}
		//如果剩余的数据长度不足ENCRYPT_MAX_DATALEN，后面补0
		if (nDataLen > 0) 
		{
			memcpy(ReqData, (LPBYTE)&pbData[nReqPos], nDataLen);
			nLen = ENCRYPT_MAX_DATALEN;
			for (int k=nDataLen; k<ENCRYPT_MAX_DATALEN; k++)
				ReqData[k] = 0;
			if (!UKeyRSAEncrypt(pstrCspCt->hUKey, pKeyObject->wPsfi, ReqData, &nLen, RespData, &nLen))
			{
				delete [] pbDataTemp;
				SetLastError(NTE_FAIL);
				return FALSE;
			}
			memcpy((PBYTE)&pbDataTemp[nRespPos], RespData, nLen);
			nRespPos += nLen;
		}
		if (nRespPos > cbBufLen)
		{
			delete [] pbDataTemp;
			SetLastError(NTE_BAD_LEN);
			return FALSE;
		}
		*pcbDataLen = nRespPos;
		memcpy(pbData, pbDataTemp, nRespPos);
		memset(pbDataTemp, 0 , nDataLen);
		delete [] pbDataTemp;
		pbDataTemp = NULL;
		
	}
	else
	{
		SetLastError(NTE_BAD_ALGID);
		DEBUG_MSG("pKeyObject->Algid: NTE_BAD_ALGID !","CPEncrypt !");
		return FALSE;
	}
    return TRUE;
}
/*BOOL WINAPI
CPEncrypt1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  HCRYPTHASH hHash,
    IN  BOOL fFinal,
    IN  DWORD dwFlags,
    IN OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen,
    IN  DWORD cbBufLen)
{
   	DWORD i,j;
	unsigned short xkey[64];
	BYTE key[128],iv[8],buf[128];

//	AfxMessageBox("进入函数： CPEncrypt");

#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPEncrypt \t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\n",hProv,hKey,hHash,fFinal,dwFlags,pbData,pcbDataLen,cbBufLen);
	for(i=0;i<*pcbDataLen;i++)
	{
		fprintf(fp,"0x%02x,",pbData[i]);
		if((i+1)%64==0) fprintf(fp,"\n");
	}
	fprintf(fp,"\n");
	fclose(fp);
#endif

	struct CspContainer *pstrCspCt;
	struct KeyObject *pKeyObject;
	
	pstrCspCt=(struct CspContainer *) hProv;
	pKeyObject=(struct KeyObject *) hKey;

	//if(*pcbDataLen==0 || (*pcbDataLen % 8!=0 && !fFinal))
	//DWORD n = *pcbDataLen;
	//*pcbDataLen = 0x64;
	if(*pcbDataLen == 0)
	{
		SetLastError(NTE_BAD_LEN);
		DEBUG_MSG("DEBUG:NTE_BAD_LEN","CPEncrypt");
		return FALSE;
	}
	if (pbData == NULL)
	{
		SetLastError(NTE_BAD_DATA);
		DEBUG_MSG("DEBUG:NTE_BAD_DATA","CPEncrypt");
		return FALSE;
	}

	memset(key,0,sizeof(key));
	memset(buf,0,sizeof(buf));
	memset(iv,0,sizeof(iv));
 */
	/* 
	if(fFinal)
	{
		i = 8 - *pcbDataLen % 8;
		*pcbDataLen = *pcbDataLen - i;
		if(*pcbDataLen > cbBufLen)
		{
			SetLastError(NTE_BAD_LEN);
			return FALSE;
		}
		memset(pbData + *pcbDataLen - i, i , i);//padding here
	}*/
/*
	if (pKeyObject->Algid == CALG_RSA_SIGN)
	{
		int nDataLen = *pcbDataLen;
		int nReqPos = 0;
		int nRespPos = 0;
		int nLen = ENCRYPT_MAX_DATALEN;
		BYTE RespData[ENCRYPT_MAX_DATALEN];
		BYTE ReqData[ENCRYPT_MAX_DATALEN];

		PBYTE pbDataTemp = new BYTE[cbBufLen];
		// 如果长度超过ENCRYPT_MAX_DATALEN，按照每次加密ENCRYPT_MAX_DATALEN逐段加密
		while (nDataLen >= ENCRYPT_MAX_DATALEN)
		{
			memcpy(ReqData, (LPBYTE)&pbData[nReqPos], ENCRYPT_MAX_DATALEN);
			if (!UKeyRSAEncrypt(pstrCspCt->hUKey, pKeyObject->wPsfi, ReqData, &nLen, RespData, &nLen))
			{
				delete [] pbDataTemp;
				SetLastError(NTE_FAIL);
				return FALSE;
			}
			memcpy((PBYTE)&pbDataTemp[nRespPos], RespData, nLen);
			nDataLen -= ENCRYPT_MAX_DATALEN;
			nReqPos += ENCRYPT_MAX_DATALEN;
			nRespPos += nLen;
		}
		//如果剩余的数据长度不足ENCRYPT_MAX_DATALEN，后面补0
		if (nDataLen > 0) 
		{
			memcpy(ReqData, (LPBYTE)&pbData[nReqPos], nDataLen);
			nLen = ENCRYPT_MAX_DATALEN;
			for (int k=nDataLen; k<ENCRYPT_MAX_DATALEN; k++)
				ReqData[k] = 0;
			if (!UKeyRSAEncrypt(pstrCspCt->hUKey, pKeyObject->wPsfi, ReqData, &nLen, RespData, &nLen))
			{
				delete [] pbDataTemp;
				SetLastError(NTE_FAIL);
				return FALSE;
			}
			memcpy((PBYTE)&pbDataTemp[nRespPos], RespData, nLen);
			nRespPos += nLen;
		}
		if (nRespPos > cbBufLen)
		{
			delete [] pbDataTemp;
			SetLastError(NTE_BAD_LEN);
			return FALSE;
		}
		*pcbDataLen = nRespPos;
		memcpy(pbData, pbDataTemp, nRespPos);
		memset(pbDataTemp, 0 , nDataLen);
		delete [] pbDataTemp;
		pbDataTemp = NULL;
	
	}
	else
	{
		SetLastError(NTE_BAD_ALGID);
		DEBUG_MSG("pKeyObject->Algid: NTE_BAD_ALGID !","CPEncrypt !");
		return FALSE;
	}
	*/
/*	if(pKeyObject->Algid==CALG_RC2)
	{
		memcpy(key,pKeyObject->bKey,pKeyObject->dwKeyLen/8);
		memcpy(key+pKeyObject->dwKeyLen/8,pKeyObject->bSalt,pKeyObject->dwSaltLen/8);
		rc2_keyschedule(xkey,key,pKeyObject->dwEffectiveKeyLen/8,pKeyObject->dwEffectiveKeyLen);
		memcpy(iv,pKeyObject->bIVFact,pKeyObject->dwIVLen/8);
		for(j=0;j<*pcbDataLen;j+=8)
		{
			for(i=0;i<8;i++)
				buf[i]=buf[i]^iv[i];
			memcpy(iv,pbData+j,8);
			rc2_encrypt( xkey,pbData+j,buf);
			memcpy(pbData+j,buf,8);
		}
		memcpy(pKeyObject->bIVFact,iv,8);
	#ifdef DEBUG_MODE
		fp=fopen("c:\\HED_CSP_LOG.txt","a+");
		fprintf(fp,"\n\n\n加密后数据为：\n");
		for(i=0;i<*pcbDataLen;i++)
		{
			fprintf(fp,"0x%02x,",pbData[i]);
			if((i+1)%64==0) fprintf(fp,"\n");
		}
		fprintf(fp,"\n");
		fclose(fp);
	#endif
	}
	else
	{
		SetLastError(NTE_BAD_ALGID);
		DEBUG_MSG("pKeyObject->Algid: NTE_BAD_ALGID !","CPEncrypt !");
		return FALSE;
	}*/
/*    return TRUE;
}
*/

/*
 -  CPDecrypt
 -
 *  Purpose:
 *                Decrypt data
 *
 *
 *  Parameters:
 *               IN  hProv         -  Handle to the CSP user
 *               IN  hKey          -  Handle to the key
 *               IN  hHash         -  Optional handle to a hash
 *               IN  Final         -  Boolean indicating if this is the final
 *                                    block of ciphertext
 *               IN  dwFlags       -  Flags values
 *               IN OUT pbData     -  Data to be decrypted
 *               IN OUT pdwDataLen -  Pointer to the length of the data to be
 *                                    decrypted
 *
 *  Returns:
 */
//最好改称都用openssl中的函数
//
BOOL WINAPI
CPDecrypt1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  HCRYPTHASH hHash,
    IN  BOOL fFinal,
    IN  DWORD dwFlags,
    IN OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen)
{
	DWORD i,j;
	unsigned short xkey[64];
	BYTE key[128],iv[8],buf[128];

#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPDecrypt \t%x\t%x\t%x\t%x\t%x\t%x\t%x\n",hProv,hKey,hHash,fFinal,dwFlags,pbData,*pcbDataLen);
	for(i=0;i<*pcbDataLen;i++)
	{
		fprintf(fp,"0x%02x,",pbData[i]);
		if((i+1)%64==0) fprintf(fp,"\n");
	}
	fprintf(fp,"\n");
	fclose(fp);
#endif

	struct CspContainer *pstrCspCt;
	struct KeyObject *pKeyObject;
	
	pstrCspCt=(struct CspContainer *) hProv;
	pKeyObject=(struct KeyObject *) hKey;

	if(*pcbDataLen==0 || *pcbDataLen % 8!=0)
	{
		SetLastError(NTE_BAD_LEN);
		DEBUG_MSG("DEBUG:NTE_BAD_LEN","CPDecrypt");
		return FALSE;
	}
	#ifdef DEBUG_MODE
		fp=fopen("c:\\HED_CSP_LOG.txt","a+");
		fprintf(fp,"pKeyObject->dwKeyLen=%04x\n",pKeyObject->dwKeyLen);
		fprintf(fp,"pKeyObject->bIVFact 为:\n");
		for(i=0;i<pKeyObject->dwIVLen/8;i++)
			fprintf(fp,"%02x",pKeyObject->bIVFact[i]);
		fprintf(fp,"\npKeyObject->dwSaltLen=%04x\n",pKeyObject->dwSaltLen);
		fprintf(fp,"\npKeyObject->bSalt 为:\n");
		for(i=0;i<pKeyObject->dwSaltLen/8;i++)
			fprintf(fp,"%02x",pKeyObject->bSalt[i]);
		fprintf(fp,"\npKeyObject->dwIVLen=%04x\n",pKeyObject->dwIVLen);
		fprintf(fp,"\npKeyObject->IV 为:\n");
		for(i=0;i<pKeyObject->dwIVLen/8;i++)
			fprintf(fp,"%02x",pKeyObject->bIV[i]);
		fprintf(fp,"\npKeyObject->bKey:");
		for(i=0;i<pKeyObject->dwKeyLen/8;i++)
			fprintf(fp,"%02x",pKeyObject->bKey[i]);
		fprintf(fp,"\n");
		fclose(fp);
	#endif

	memset(key,0,sizeof(key));
	memset(buf,0,sizeof(buf));
	memset(iv,0,sizeof(iv));
	if(pKeyObject->Algid==CALG_RC2)
	{
		memcpy(key, pKeyObject->bKey, pKeyObject->dwKeyLen / 8);
		memcpy(key + pKeyObject->dwKeyLen/8, pKeyObject->bSalt, pKeyObject->dwSaltLen / 8);
		rc2_keyschedule(xkey ,key ,pKeyObject->dwEffectiveKeyLen/8 , pKeyObject->dwEffectiveKeyLen);
		memcpy(iv, pKeyObject->bIVFact, pKeyObject->dwIVLen/8);
		for(j=0;j<*pcbDataLen;j+=8)
		{
			rc2_decrypt( xkey,buf,pbData+j);
			for(i=0;i<8;i++)
				buf[i]=buf[i]^iv[i];
			memcpy(iv,pbData+j,8);
			memcpy(pbData+j,buf,8);
		}
		memcpy(pKeyObject->bIVFact,iv,8);
		//if(hHash){}
	}
	else if(pKeyObject->Algid == CALG_3DES)
	{
		memcpy(iv, pKeyObject->bIVFact, pKeyObject->dwIVLen/8);
		memcpy(key, pKeyObject->bKey,pKeyObject->dwKeyLen/8);
		for(j=0;j<*pcbDataLen;j+=8)
		{
			TripleDES_1(pbData + j, key, buf);
			for(i=0; i<8; i++)
				buf[i]=buf[i]^iv[i];
			memcpy(iv,pbData+j,8);
			memcpy(pbData+j,buf,8);
		}
		memcpy(pKeyObject->bIVFact,iv,8);			
	}
	else if (pKeyObject->Algid == CALG_RSA_SIGN)
	{
		int nDataLen = (int)*pcbDataLen;
		int nReqPos = 0;
		int nRespPos = 0;
		int nLen = ENCRYPT_MAX_DATALEN;
		BYTE RespData[ENCRYPT_MAX_DATALEN];
		BYTE ReqData[ENCRYPT_MAX_DATALEN];
		
		PBYTE pbDataTemp = (PBYTE)malloc(nDataLen);
		// 如果长度超过ENCRYPT_MAX_DATALEN，按照每次加密ENCRYPT_MAX_DATALEN逐段加密
		while (nDataLen >= ENCRYPT_MAX_DATALEN)
		{
			memcpy(ReqData, (LPBYTE)&pbData[nReqPos], ENCRYPT_MAX_DATALEN);
			if (!UKeyRSADecrypt(pstrCspCt->hUKey, pKeyObject->wSsfi, ReqData, &nLen, RespData, &nLen))
			{
				free(pbDataTemp);
				SetLastError(NTE_FAIL);
				return FALSE;
			}
			memcpy((PBYTE)&pbDataTemp[nRespPos], RespData, nLen);
			nDataLen -= ENCRYPT_MAX_DATALEN;
			nReqPos += ENCRYPT_MAX_DATALEN;
			nRespPos += nLen;
		}
		*pcbDataLen = nRespPos;
		memcpy(pbData, pbDataTemp, *pcbDataLen);
		memset(pbDataTemp, 0, nDataLen);
		free(pbDataTemp);
		pbDataTemp = NULL;
			
	}
	else
	{
		SetLastError(NTE_BAD_ALGID);
		DEBUG_MSG("pKeyObject->Algid: NTE_BAD_ALGID !","CPDecrypt !");
		return FALSE;
	}

	if(fFinal)
		*pcbDataLen=*pcbDataLen-pbData[*pcbDataLen-1];

	//debug only
	#ifdef DEBUG_MODE
		fp=fopen("c:\\HED_CSP_LOG.txt","a+");
		fprintf(fp,"\n\n\n解密后数据为：\n");
		for(i=0;i<*pcbDataLen;i++)
		{
			fprintf(fp,"0x%02x,",pbData[i]);
			if((i+1)%64==0) fprintf(fp,"\n");
		}
		fprintf(fp,"\n");
		fclose(fp);
	#endif

    return TRUE;
}

/*BOOL WINAPI
CPDecrypt1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  HCRYPTHASH hHash,
    IN  BOOL fFinal,
    IN  DWORD dwFlags,
    IN OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen)
{
	DWORD i,j;
	unsigned short xkey[64];
	BYTE key[128],iv[8],buf[128];

#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPDecrypt \t%x\t%x\t%x\t%x\t%x\t%x\t%x\n",hProv,hKey,hHash,fFinal,dwFlags,pbData,*pcbDataLen);
	for(i=0;i<*pcbDataLen;i++)
	{
		fprintf(fp,"0x%02x,",pbData[i]);
		if((i+1)%64==0) fprintf(fp,"\n");
	}
	fprintf(fp,"\n");
	fclose(fp);
#endif
 
	struct CspContainer *pstrCspCt;
	struct KeyObject *pKeyObject;
	
	pstrCspCt=(struct CspContainer *) hProv;
	pKeyObject=(struct KeyObject *) hKey;

	//*pcbDataLen = 0x80;
	if ((*pcbDataLen == 0) || ((*pcbDataLen % ENCRYPT_MAX_DATALEN) != 0))
	{
		SetLastError(NTE_BAD_LEN);
		DEBUG_MSG("DEBUG:NTE_BAD_LEN","CPDecrypt");
		return FALSE;
	}
	if (pbData == NULL)
	{
		SetLastError(NTE_BAD_DATA);
		DEBUG_MSG("DEBUG:NTE_BAD_DATA","CPDecrypt");
		return FALSE;
	}
	memset(key,0,sizeof(key));
	memset(buf,0,sizeof(buf));
	memset(iv,0,sizeof(iv));
	if (pKeyObject->Algid == CALG_RSA_SIGN)
	{
		int nDataLen = (int)*pcbDataLen;
		int nReqPos = 0;
		int nRespPos = 0;
		int nLen = ENCRYPT_MAX_DATALEN;
		BYTE RespData[ENCRYPT_MAX_DATALEN];
		BYTE ReqData[ENCRYPT_MAX_DATALEN];

		PBYTE pbDataTemp = (PBYTE)malloc(nDataLen);
		// 如果长度超过ENCRYPT_MAX_DATALEN，按照每次加密ENCRYPT_MAX_DATALEN逐段加密
		while (nDataLen >= ENCRYPT_MAX_DATALEN)
		{
			memcpy(ReqData, (LPBYTE)&pbData[nReqPos], ENCRYPT_MAX_DATALEN);
			if (!UKeyRSADecrypt(pstrCspCt->hUKey, pKeyObject->wSsfi, ReqData, &nLen, RespData, &nLen))
			{
				free(pbDataTemp);
				SetLastError(NTE_FAIL);
				return FALSE;
			}
			memcpy((PBYTE)&pbDataTemp[nRespPos], RespData, nLen);
			nDataLen -= ENCRYPT_MAX_DATALEN;
			nReqPos += ENCRYPT_MAX_DATALEN;
			nRespPos += nLen;
		}
		*pcbDataLen = nRespPos;
		memcpy(pbData, pbDataTemp, *pcbDataLen);
		memset(pbDataTemp, 0, nDataLen);
		free(pbDataTemp);
		pbDataTemp = NULL;
		
	}
	*/
/*	if(*pcbDataLen==0 || *pcbDataLen % 8!=0)
	{
		SetLastError(NTE_BAD_LEN);
		DEBUG_MSG("DEBUG:NTE_BAD_LEN","CPDecrypt");
		return FALSE;
	}
	#ifdef DEBUG_MODE
		fp=fopen("c:\\HED_CSP_LOG.txt","a+");
		fprintf(fp,"pKeyObject->dwKeyLen=%04x\n",pKeyObject->dwKeyLen);
		fprintf(fp,"pKeyObject->bIVFact 为:\n");
		for(i=0;i<pKeyObject->dwIVLen/8;i++)
			fprintf(fp,"%02x",pKeyObject->bIVFact[i]);
		fprintf(fp,"\npKeyObject->dwSaltLen=%04x\n",pKeyObject->dwSaltLen);
		fprintf(fp,"\npKeyObject->bSalt 为:\n");
		for(i=0;i<pKeyObject->dwSaltLen/8;i++)
			fprintf(fp,"%02x",pKeyObject->bSalt[i]);
		fprintf(fp,"\npKeyObject->dwIVLen=%04x\n",pKeyObject->dwIVLen);
		fprintf(fp,"\npKeyObject->IV 为:\n");
		for(i=0;i<pKeyObject->dwIVLen/8;i++)
			fprintf(fp,"%02x",pKeyObject->bIV[i]);
		fclose(fp);
	#endif
*/
	
/*	else if(pKeyObject->Algid==CALG_RC2)
	{
		memcpy(key, pKeyObject->bKey, pKeyObject->dwKeyLen / 8);
		memcpy(key + pKeyObject->dwKeyLen/8, pKeyObject->bSalt, pKeyObject->dwSaltLen / 8);
		rc2_keyschedule(xkey ,key ,pKeyObject->dwEffectiveKeyLen/8 , pKeyObject->dwEffectiveKeyLen);
		memcpy(iv, pKeyObject->bIVFact, pKeyObject->dwIVLen/8);
		for(j=0;j<*pcbDataLen;j+=8)
		{
			rc2_decrypt( xkey,buf,pbData+j);
			for(i=0;i<8;i++)
				buf[i]=buf[i]^iv[i];
			memcpy(iv,pbData+j,8);
			memcpy(pbData+j,buf,8);
		}
		memcpy(pKeyObject->bIVFact,iv,8);
		//if(hHash){}
	}
	else if(pKeyObject->Algid == CALG_3DES)
	{
		memcpy(iv, pKeyObject->bIVFact, pKeyObject->dwIVLen/8);
		memcpy(key, pKeyObject->bKey,pKeyObject->dwKeyLen/8);
		for(j=0;j<*pcbDataLen;j+=8)
		{
			TripleDES_1(pbData + j, key, buf);
			for(i=0; i<8; i++)
				buf[i]=buf[i]^iv[i];
			memcpy(iv,pbData+j,8);
			memcpy(pbData+j,buf,8);
		}
		memcpy(pKeyObject->bIVFact,iv,8);			
	}
	else
	{
		SetLastError(NTE_BAD_ALGID);
		DEBUG_MSG("pKeyObject->Algid: NTE_BAD_ALGID !","CPDecrypt !");
		return FALSE;
	}

	if(fFinal)
		*pcbDataLen=*pcbDataLen-pbData[*pcbDataLen-1];

	//debug only
	#ifdef DEBUG_MODE
		fp=fopen("c:\\HED_CSP_LOG.txt","a+");
		fprintf(fp,"\n\n\n解密后数据为：\n");
		for(i=0;i<*pcbDataLen;i++)
		{
			fprintf(fp,"0x%02x,",pbData[i]);
			if((i+1)%64==0) fprintf(fp,"\n");
		}
		fprintf(fp,"\n");
		fclose(fp);
	#endif

    return TRUE;
}
*/

/*
 -  CPCreateHash
 -
 *  Purpose:
 *                initate the hashing of a stream of data
 *
 *
 *  Parameters:
 *               IN  hUID    -  Handle to the user identifcation
 *               IN  Algid   -  Algorithm identifier of the hash algorithm
 *                              to be used
 *               IN  hKey   -   Optional handle to a key
 *               IN  dwFlags -  Flags values
 *               OUT pHash   -  Handle to hash object
 *
 *  Returns:
 */

BOOL WINAPI
CPCreateHash1(
    IN  HCRYPTPROV hProv,
    IN  ALG_ID Algid,
    IN  HCRYPTKEY hKey,
    IN  DWORD dwFlags,
    OUT HCRYPTHASH *phHash)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPCreateHash \t%x \t%x \t%x \t%x \t%x\n",hProv,Algid,hKey,dwFlags,phHash);
	fclose(fp);
#endif
	struct CspContainer *pstrCspCt;
	struct HashObject *pHashObject;
	
	pstrCspCt=(struct CspContainer *)hProv;
	pHashObject=(struct HashObject *) malloc(sizeof(struct HashObject));
	if(pHashObject==NULL)
	{
		SetLastError(NTE_NO_MEMORY);
		return FALSE;
	}
	pHashObject->Algid=Algid;
	pHashObject->hKey=hKey;
	memset(pHashObject->pbHashData,0,sizeof(pHashObject->pbHashData));
	pHashObject->bIsFinished=FALSE;

	switch (Algid)
	{
	case CALG_MD5:
		pHashObject->HashLen=16;
		pHashObject->dwOIDLen=18;
		memcpy(pHashObject->OID,"\x030\x020\x030\x00c\x006\x008\x02a\x086\x048\x086\x0f7\x00d\x002\x005\x005\x000\x004\x010",18);
		MD5_Init (&pHashObject->MD5_context);
//		AfxMessageBox("Algid is CALG_MD5");
		break;
	case CALG_MAC:
		pHashObject->HashLen=4;
		DEBUG_MSG("Algid is CALG_MAC,NOT Complete","CPCreateHash");
		break;
	case CALG_SHA1:
		pHashObject->HashLen=20;
		pHashObject->dwOIDLen=15;
		memcpy(pHashObject->OID,"\x030\x021\x030\x009\x006\x005\x02b\x00e\x003\x002\x01a\x005\x000\x004\x014",15);
		SHA1_Init (&pHashObject->SHA1_context);
		break;
	case CALG_SSL3_SHAMD5:
		pHashObject->HashLen=36;
		pHashObject->dwOIDLen =0;
		MD5_Init (&pHashObject->MD5_context);
		SHA1_Init (&pHashObject->SHA1_context);
		break;
	default:
		SetLastError(NTE_BAD_ALGID);
		DEBUG_MSG("Algid is Unknow !","CPCreateHash !");
		return TRUE;
	}
#ifdef DEBUG_MODE
	fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"pHashObject->HashLen is :%02x\n",pHashObject->HashLen);
	fclose(fp);
#endif

	*phHash = (HCRYPTHASH)pHashObject;  // Replace NULL with your own structure.
    return TRUE;
}


/*
 -  CPHashData
 -
 *  Purpose:
 *                Compute the cryptograghic hash on a stream of data
 *
 *
 *  Parameters:
 *               IN  hProv     -  Handle to the user identifcation
 *               IN  hHash     -  Handle to hash object
 *               IN  pbData    -  Pointer to data to be hashed
 *               IN  dwDataLen -  Length of the data to be hashed
 *               IN  dwFlags   -  Flags values
 *
 *  Returns:
 */

BOOL WINAPI
CPHashData1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  CONST BYTE *pbData,
    IN  DWORD dwDataLen,
    IN  DWORD dwFlags)
{

#ifdef DEBUG_MODE
	DWORD i;
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPHashData \t%x \t%x \t%x \t%x \t%x\n",hProv,hHash,pbData,dwDataLen,dwFlags);
	fclose(fp);

	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPHashData->pbData is:\n");
	for(i=0;i<dwDataLen;i++)
		fprintf(fp,"0x%02x,",pbData[i]);
	fprintf(fp,"\n");
	fclose(fp);
#endif

	struct CspContainer *pstrCspCt;
	struct HashObject *pHashObject;
	
	pstrCspCt=(struct CspContainer *) hProv;
	pHashObject=(struct HashObject *) hHash;
	if(dwFlags)//暂不支持CRYPT_USERDATA
	{
		SetLastError(NTE_BAD_FLAGS);
		DEBUG_MSG("dwFlags == CRYPT_USERDATA Not Surport !", "CPHashData !");
		return FALSE;
	}
	if(!dwDataLen)
	{
		SetLastError(NTE_BAD_LEN);
		return FALSE;
	}
	if(pbData==NULL)
	{
		SetLastError(NTE_NO_MEMORY);
		return FALSE;
	}
	if(pHashObject->bIsFinished)
	{
		SetLastError(NTE_BAD_HASH_STATE);
		return FALSE;
	}

	switch (pHashObject->Algid)
	{
	case CALG_MD5:
		MD5_Update (&pHashObject->MD5_context, (BYTE *)pbData,dwDataLen);

		break;
	case CALG_MAC:
		SetLastError(NTE_BAD_ALGID);
		DEBUG_MSG("CALG_MAC Not Surport Present !","CPHashData");
		return FALSE;
	case CALG_SHA1:
		SHA1_Update(&pHashObject->SHA1_context,(BYTE *)pbData,dwDataLen);
		break;
//	case CALG_SSL3_SHAMD5:
//		MD5Update0 (&pHashObject->MD5_context, (BYTE *)pbData,dwDataLen);
//		SHA1Update(&pHashObject->SHA1_context,(BYTE *)pbData,dwDataLen);
//		break;
	default:
		SetLastError(NTE_BAD_ALGID);
		DEBUG_MSG("Algid is Unknow !","CPHashData !");
		return FALSE;
	}
    return TRUE;
}


/*
 -  CPHashSessionKey
 -
 *  Purpose:
 *                Compute the cryptograghic hash on a key object.
 *
 *
 *  Parameters:
 *               IN  hProv     -  Handle to the user identifcation
 *               IN  hHash     -  Handle to hash object
 *               IN  hKey      -  Handle to a key object
 *               IN  dwFlags   -  Flags values
 *
 *  Returns:
 *               CRYPT_FAILED
 *               CRYPT_SUCCEED
 */

BOOL WINAPI
CPHashSessionKey1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  HCRYPTKEY hKey,
    IN  DWORD dwFlags)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPHashSessionKey \t%x \t%x \t%x \t%x \n",hProv,hHash,hKey,dwFlags);
	fclose(fp);
#endif

    return TRUE;
}


/*
 -  CPSignHash
 -
 *  Purpose:
 *                Create a digital signature from a hash
 *
 *
 *  Parameters:
 *               IN  hProv        -  Handle to the user identifcation
 *               IN  hHash        -  Handle to hash object
 *               IN  dwKeySpec    -  Key pair to that is used to sign with
 *               IN  sDescription -  Description of data to be signed
 *               IN  dwFlags      -  Flags values
 *               OUT pbSignature  -  Pointer to signature data
 *               IN OUT dwHashLen -  Pointer to the len of the signature data
 *
 *  Returns:
 */

BOOL WINAPI
CPSignHash1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  DWORD dwKeySpec,
    IN  LPCWSTR szDescription,
    IN  DWORD dwFlags,
    OUT LPBYTE pbSignature,
    IN OUT LPDWORD pcbSigLen)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPSignHash \t%x \t%x\t%x \t%x\t%x \t%x\t%x\n",hProv,hHash,dwKeySpec,szDescription,dwFlags,pbSignature,*pcbSigLen);
	fclose(fp);
#endif
	//BYTE InData[256],OutData[256];
	DWORD i;
	WORD wSfi;

	struct CspContainer *pstrCspCt;
	struct HashObject *pHashObject;
	struct KeyObject *pKeyObject;
	pstrCspCt=(struct CspContainer *)hProv;
	pHashObject=(struct HashObject *)hHash;
	pKeyObject=(struct KeyObject*)(pHashObject->hKey);

	if (hHash == 0 || pHashObject->HashLen == 0 || pHashObject->pbHashData == NULL)
	{
		SetLastError(NTE_BAD_HASH);
		return FALSE;
	}
	*pcbSigLen=128;
	if(pbSignature == NULL)
		return TRUE;
// 	if (dwKeySpec != AT_SIGNATURE)
// 	{
// 		SetLastError(NTE_NO_KEY);
// 		return FALSE;
// 	}
	if(!pHashObject->bIsFinished)
	{
		if(pHashObject->Algid==CALG_MD5)
			MD5_Final(pHashObject->pbHashData,&pHashObject->MD5_context);
		else if(pHashObject->Algid==CALG_SHA1)
			SHA1_Final(pHashObject->pbHashData, &pHashObject->SHA1_context);
		else if(pHashObject->Algid == CALG_SSL3_SHAMD5)
		{
			//左边20字节MD5的，右边16字节SHA1的
			MD5_Final(pHashObject->pbHashData,&pHashObject->MD5_context);
			SHA1_Final(pHashObject->pbHashData + 16, &pHashObject->SHA1_context);
		}
		else
		{		
			SetLastError(NTE_BAD_ALGID);
			DEBUG_MSG("pHashObject->Algid Unknow !","CPSignHash !");
			return FALSE;
		}
	}
	if (dwKeySpec == AT_SIGNATURE)
	{
		wSfi = pstrCspCt->strCardContainer.wKeySSfi;//;pKeyObject->bSsfi
	}
	else if (dwKeySpec == AT_KEYEXCHANGE)
	{
		wSfi = 0x1E;
	}
	else
	{
	 	SetLastError(NTE_NO_KEY);
	 	return FALSE;
 	}
	
	BYTE InData[128];
	if (pHashObject->HashLen > 128)
	{
		SetLastError(NTE_BAD_HASH);
		return FALSE;
	}

	if(dwFlags & CRYPT_NOHASHOID)
	{
		InData[0]=0x00;
		InData[1]=0x01;
		for(i=0;i<128-pHashObject->HashLen-3;i++)
			InData[i+2]=0xff;
		InData[128-pHashObject->HashLen-1]=0x00;
		for(i=0;i<pHashObject->HashLen;i++)
			InData[i+128-pHashObject->HashLen]=pHashObject->pbHashData[i];
	}
	else
	{
		InData[0]=0x00;  //起始字节
		InData[1]=0x01;  //数据块类型(01h：签名)
		//补位
		for(i=0;i<128-pHashObject->HashLen - pHashObject->dwOIDLen-3;i++)
			InData[i+2]=0xff;
		//分割符 00
		InData[128-pHashObject->HashLen - pHashObject->dwOIDLen - 1]=0x00;
		//oid
		memcpy(&InData[128-pHashObject->HashLen - pHashObject->dwOIDLen],pHashObject->OID,pHashObject->dwOIDLen);
		//HASH 值
		for(i=0;i<pHashObject->HashLen;i++)
			InData[i+128-pHashObject->HashLen]=pHashObject->pbHashData[i];
		//debug::???
	}
	//memcpy(SigData, pHashObject->pbHashData, pHashObject->HashLen);
	//if (pHashObject->HashLen < 128)
	//{		
	//	for (i=pHashObject->HashLen; i<128; i++)
	//		SigData[i] = 0x00;
	//}
	BYTE SignData[128];
	//UKey支持低字节在前，所以需要倒一下顺序
	for (i=0; i<128; i++)
		SignData[i] = InData[127 - i];
	if (!UKeyRSASignature(pstrCspCt->hUKey, wSfi, SignData, 128, pbSignature, (int *)pcbSigLen))
	{
		SetLastError(NTE_FAIL);
		return FALSE;
	}

#ifdef DEBUG_MODE		
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"\n签名填充数据:\n");
	for(i=0;i<128;i++)
		fprintf(fp,"%02x",SignData[i]);
	fprintf(fp,"\n");
	fclose(fp);
#endif
/*	//CALG_MD5 CALG_MAC CALG_SHA1
	
	
//debug only
#ifdef DEBUG_MODE
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"SignHash GetHash返回：-> ");
	for(i=0;i<pHashObject->HashLen;i++)
		fprintf(fp,"%02x",pHashObject->pbHashData[i]);
	fprintf(fp,"\n");
	fclose(fp);
#endif	

	if(dwFlags & CRYPT_NOHASHOID)
	{
		InData[0]=0x00;
		InData[1]=0x01;
		for(i=0;i<128-pHashObject->HashLen-3;i++)
			InData[i+2]=0xff;
		InData[128-pHashObject->HashLen-1]=0x00;
		for(i=0;i<pHashObject->HashLen;i++)
			InData[i+128-pHashObject->HashLen]=pHashObject->pbHashData[i];
	}
	else
	{
		InData[0]=0x00;
		InData[1]=0x01;
		for(i=0;i<128-pHashObject->HashLen - pHashObject->dwOIDLen-3;i++)
			InData[i+2]=0xff;
		InData[128-pHashObject->HashLen - pHashObject->dwOIDLen - 1]=0x00;
		memcpy(&InData[128-pHashObject->HashLen - pHashObject->dwOIDLen ],pHashObject->OID,pHashObject->dwOIDLen);
		for(i=0;i<pHashObject->HashLen;i++)
			InData[i+128-pHashObject->HashLen]=pHashObject->pbHashData[i];
		//debug::???
	}

	memset(OutData,0,sizeof(OutData));
	if(!UKeyPrivateKeyDecrypt(pstrCspCt->hUKey ,wSfi,InData,OutData))
		return FALSE;
	for(i=0;i<*pcbSigLen;i++)
		pbSignature[*pcbSigLen-i-1]=OutData[i];

#ifdef DEBUG_MODE		
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"\n签名填充数据:\n");
	for(i=0;i<128;i++)
		fprintf(fp,"%02x",InData[i]);
	fprintf(fp,"\n");
	fclose(fp);
#endif*/

	return TRUE;

}


/*
 -  CPDestroyHash
 -
 *  Purpose:
 *                Destroy the hash object
 *
 *
 *  Parameters:
 *               IN  hProv     -  Handle to the user identifcation
 *               IN  hHash     -  Handle to hash object
 *
 *  Returns:
 */

BOOL WINAPI
CPDestroyHash1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPDestroyHash \t%x \t%x\n",hProv,hHash);
	fclose(fp);
#endif
	if(hHash != NULL)
		free((void *)hHash);

    return TRUE;
}


/*
 -  CPVerifySignature
 -
 *  Purpose:
 *                Used to verify a signature against a hash object
 *
 *
 *  Parameters:
 *               IN  hProv        -  Handle to the user identifcation
 *               IN  hHash        -  Handle to hash object
 *               IN  pbSignture   -  Pointer to signature data
 *               IN  dwSigLen     -  Length of the signature data
 *               IN  hPubKey      -  Handle to the public key for verifying
 *                                   the signature
 *               IN  sDescription -  String describing the signed data
 *               IN  dwFlags      -  Flags values
 *
 *  Returns:
 */

BOOL WINAPI
CPVerifySignature1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  CONST BYTE *pbSignature,
    IN  DWORD cbSigLen,
    IN  HCRYPTKEY hPubKey,
    IN  LPCWSTR szDescription,
    IN  DWORD dwFlags)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPVerifySignature \t%x\t%x\t%x\t%x\t%x\t%x\t%x\n",hProv,hHash,pbSignature,cbSigLen,hPubKey,szDescription,dwFlags);
	fclose(fp);
#endif
	BYTE InData[256],OutData[256];
	int OutLen = 0;
	DWORD i;
	WORD wSfi;

	struct CspContainer *pstrCspCt;
	struct HashObject *pHashObject;
	struct KeyObject *pKeyObject;
	pstrCspCt=(struct CspContainer *)hProv;
	pHashObject=(struct HashObject *)hHash;
	pKeyObject=(struct KeyObject*)(hPubKey);

	if(pbSignature == NULL)
	{
		SetLastError(NTE_BAD_SIGNATURE);
		return FALSE;
	}

	if(!pHashObject->bIsFinished)
	{
		if(pHashObject->Algid==CALG_MD5)
			MD5_Final(pHashObject->pbHashData,&pHashObject->MD5_context);
		else if(pHashObject->Algid==CALG_SHA1)
			SHA1_Final(pHashObject->pbHashData, &pHashObject->SHA1_context);
		else if(pHashObject->Algid == CALG_SSL3_SHAMD5)
		{
			MD5_Final(pHashObject->pbHashData,&pHashObject->MD5_context);
			SHA1_Final(pHashObject->pbHashData + 16, &pHashObject->SHA1_context);
		}
		else
		{		
			SetLastError(NTE_BAD_ALGID);
			DEBUG_MSG("pHashObject->Algid Unknow !","CPSignHash !");
			return FALSE;
		}
	}
	wSfi = pKeyObject->wPsfi;//pstrCspCt->strCardContainer.wKeyPSfi;//
	if (!UKeyRSASignatureVerify(pstrCspCt->hUKey, wSfi, pbSignature, cbSigLen, OutData, &OutLen))
	{
		//SetLastError();
		return FALSE;
	}
	
	BOOL bRet = memcmp(pHashObject->pbHashData, OutData, pHashObject->HashLen) == 0;
	
    return bRet;
}


/*
 -  CPGenRandom
 -
 *  Purpose:
 *                Used to fill a buffer with random bytes
 *
 *
 *  Parameters:
 *               IN  hProv         -  Handle to the user identifcation
 *               IN  dwLen         -  Number of bytes of random data requested
 *               IN OUT pbBuffer   -  Pointer to the buffer where the random
 *                                    bytes are to be placed
 *
 *  Returns:
 */

BOOL WINAPI
CPGenRandom1(
    IN  HCRYPTPROV hProv,
    IN  DWORD cbLen,
    OUT LPBYTE pbBuffer)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPGenRandom \t%x \t%x \t%x\n",hProv,cbLen,pbBuffer);
	fclose(fp);
#endif
	struct CspContainer *pstrCspCt;
	pstrCspCt=(struct CspContainer *)hProv;

	if(!UKeyGetChallange(pstrCspCt->hUKey, pbBuffer, cbLen))
	{
		return FALSE;
	}
    return TRUE;
}


/*
 -  CPGetUserKey
 -
 *  Purpose:
 *                Gets a handle to a permanent user key
 *
 *
 *  Parameters:
 *               IN  hProv      -  Handle to the user identifcation
 *               IN  dwKeySpec  -  Specification of the key to retrieve
 *               OUT phUserKey  -  Pointer to key handle of retrieved key
 *
 *  Returns:
 */

BOOL WINAPI
CPGetUserKey1(
    IN  HCRYPTPROV hProv,
    IN  DWORD dwKeySpec,
    OUT HCRYPTKEY *phUserKey)
{

#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPGetUserKey \t%x \t%x \t%x\n",hProv,dwKeySpec,*phUserKey);
	fclose(fp);
#endif
//	AfxMessageBox("进入函数： CPGetUserKey");
	struct CspContainer *pstrCspCt;
	struct KeyObject *pKeyObject;
	WORD wSsfi = 0, wPsfi = 0;

	unsigned int AlgID = 0;
	pstrCspCt=(struct CspContainer *)hProv;
	if(phUserKey==NULL)
	{
		SetLastError(NTE_BAD_KEY);
		return FALSE;
	}

	if(dwKeySpec != AT_KEYEXCHANGE && dwKeySpec != AT_SIGNATURE)
	{
		SetLastError(NTE_NO_KEY);
		return FALSE;
	}
	if (dwKeySpec == AT_KEYEXCHANGE)
	{
		wSsfi = 0x1E;
		wPsfi = 0x1F;
		AlgID = CALG_RSA_KEYX;
	}
	else
	{
		if(!UKeyGetContainer(pstrCspCt->hUKey,pstrCspCt->strCardContainer.pbContName, (BYTE *)&pstrCspCt->strCardContainer))
		{
			//if(GetUkeyLastErr() == KEY_NO_CONTEXT)
			//	SetLastError(NTE_BAD_UID);
			//else
			//	SetLastError(SCARD_E_READER_UNAVAILABLE);
			return FALSE;
		}
		AlgID = pstrCspCt->strCardContainer.Algid;
		wSsfi = pstrCspCt->strCardContainer.wKeySSfi;
		wPsfi = pstrCspCt->strCardContainer.wKeyPSfi;
	}
	if(wSsfi == 0 || wPsfi == 0)
	{
		SetLastError(NTE_NO_KEY);
		return FALSE;
	}

	pKeyObject=(struct KeyObject *)malloc(sizeof(struct KeyObject));
	if(pKeyObject==NULL)
	{
		SetLastError(NTE_NO_MEMORY);
		DEBUG_MSG("NTE_NO_MEMORY !","CPGetUserKey !");
		return FALSE;
	}
	memset(pKeyObject,0,sizeof(struct KeyObject));

	pKeyObject->wSsfi = wSsfi;	
	pKeyObject->wPsfi = wPsfi;
	pKeyObject->Algid= AlgID;
	pKeyObject->dwBlockLen=HIWORD(pstrCspCt->strCardContainer.dwParam);
	pKeyObject->dwKeyLen=HIWORD(pstrCspCt->strCardContainer.dwParam);
	if(pstrCspCt->strCardContainer.dwParam & CRYPT_EXPORTABLE)
		pKeyObject->dwPermissions = CRYPT_ENCRYPT|CRYPT_DECRYPT|CRYPT_EXPORT|CRYPT_READ|CRYPT_WRITE;
	else
		pKeyObject->dwPermissions = CRYPT_ENCRYPT|CRYPT_DECRYPT|CRYPT_WRITE;
	pKeyObject->dwPadding=PKCS5_PADDING;
	pKeyObject->dwMode=CRYPT_MODE_ECB;
	pKeyObject->dwModeBits=0;//只有OFB方式有此属性
	pKeyObject->dwEffectiveKeyLen=0;//????
	pKeyObject->bIsKeyPairs=TRUE;

    *phUserKey = (HCRYPTKEY)pKeyObject;    // Replace NULL with your own structure.
#ifdef DEBUG_MODE
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPGetUserKey out \t%x \t%x \t%x\n",hProv,dwKeySpec,*phUserKey);
	fclose(fp);
#endif
	return TRUE;
}


/*
 -  CPDuplicateHash
 -
 *  Purpose:
 *                Duplicates the state of a hash and returns a handle to it.
 *                This is an optional entry.  Typically it only occurs in
 *                SChannel related CSPs.
 *
 *  Parameters:
 *               IN      hUID           -  Handle to a CSP
 *               IN      hHash          -  Handle to a hash
 *               IN      pdwReserved    -  Reserved
 *               IN      dwFlags        -  Flags
 *               IN      phHash         -  Handle to the new hash
 *
 *  Returns:
 */

BOOL WINAPI
CPDuplicateHash1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  LPDWORD pdwReserved,
    IN  DWORD dwFlags,
    OUT HCRYPTHASH *phHash)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPDuplicateHash1 \t%x \t%x \t%x\n",hProv,hHash,dwFlags);
	fclose(fp);
#endif
	DEBUG_MSG("Not Surport This Function !","CPDuplicateHash !");
    *phHash = (HCRYPTHASH)NULL;  // Replace NULL with your own structure.
    return TRUE;
}


/*
 -  CPDuplicateKey
 -
 *  Purpose:
 *                Duplicates the state of a key and returns a handle to it.
 *                This is an optional entry.  Typically it only occurs in
 *                SChannel related CSPs.
 *
 *  Parameters:
 *               IN      hUID           -  Handle to a CSP
 *               IN      hKey           -  Handle to a key
 *               IN      pdwReserved    -  Reserved
 *               IN      dwFlags        -  Flags
 *               IN      phKey          -  Handle to the new key
 *
 *  Returns:
 */

BOOL WINAPI
CPDuplicateKey1(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  LPDWORD pdwReserved,
    IN  DWORD dwFlags,
    OUT HCRYPTKEY *phKey)
{
#ifdef DEBUG_MODE
	FILE *fp;
	fp=fopen("c:\\HED_CSP_LOG.txt","a+");
	fprintf(fp,"CPDuplicateKey1 \t%x \t%x \t%x\n",hProv,hKey,dwFlags);
	fclose(fp);
#endif
	DEBUG_MSG("Not Surport This Function !","CPDuplicateKey !");
    *phKey = (HCRYPTKEY)NULL;    // Replace NULL with your own structure.
    return TRUE;
}

