#ifndef _DEFINE_H
#define _DEFINE_H
#include <wincrypt.h>
#include "OPENSSL\SHA.h"
#include "OPENSSL\md5.h"

const unsigned short OPERATION_SUCCESS = 0x9000;
const unsigned short VERIFY_FAILED = 0x63C;

const int LANGUAGE_CHINESE = 1;
const int LANGUAGE_ENGLISH = 2;


#define		PUBLIC_KEY_FLAG		0x00
#define		PRIVATE_KEY_FLAG		0x01

//typedef unsigned int ALG_ID;
struct CardContainer{
	BYTE	pbContName[64];//密钥容器名称
	ALG_ID	Algid;	       //密钥算法标识
	BYTE	dwParam;       //密钥参数  是否可以倒出等
	WORD	wKeyLen;       //密钥长度
	WORD	wKeySSfi;      //私钥地址
	WORD	wKeyPSfi;      //公钥地址
	BYTE	pbCertName[32];//证书名称
	WORD	wCertAddr;     //证书体地址
	WORD	wCertSize;     //证书体大小
};

struct AlgEnumalgs
{
	BYTE bAlgIndex;
	BYTE bAlgMax;
	PROV_ENUMALGS  strAlg[10];
};
struct AlgEnumalgsEx
{
	BYTE bAlgIndex;
	BYTE bAlgMax;
	PROV_ENUMALGS_EX  strAlgEx[10];
}; 
struct HashObject
{
	ALG_ID Algid;
	BOOL	bIsFinished;	
	//bool is hash state
	DWORD	HashLen;
	HCRYPTKEY hKey;
	BYTE	pbHashData[40];//最多为SHA120个字节
	DWORD	dwOIDLen;
	BYTE	OID[40];
	//可以多次进行摘要
	SHA_CTX SHA1_context;
	MD5_CTX MD5_context;
};

struct KeyObject
{
	ALG_ID	Algid;               //KP_ALGID
	DWORD	dwEffectiveKeyLen;   //KP_EFFECTIVE_KEYLEN
	DWORD	dwIVLen;             //KP_IV
	BYTE	bIV[8];              //KP_IV
	BYTE	bIVFact[8];          //KP_IV
	DWORD	dwMode;              //KP_MODE
	DWORD	dwModeBits;          //KP_MODE_BITS
	DWORD	dwPadding;           //KP_PADDING
	DWORD	dwPermissions;       //KP_PERMISSIONS
	DWORD	dwSaltLen;           //KP_SALT
	BYTE	bSalt[20];           //KP_SALT
	DWORD	dwBlockLen;          //KP_BLOCKLEN以bit为单位的长度
	//对称密钥
	DWORD	dwKeyLen;            //KP_KEYLEN 以bit为单位的长度
	BYTE	bKey[128];           //
	//公私钥对
	BOOL	bIsKeyPairs;
	WORD	wPsfi;               //公钥文件标识符
	WORD	wSsfi;               //私钥文件标识符
};



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//用途：在调用CpReleaseContext()的时候，释放所有与hProv有关的资源
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
struct CspContainer{	//参照：CPGetProvParam()
	HANDLE hUKey;
	DWORD	dwFlagsAQ;//请求密钥容器时指定的dwFlag,
						//如果dwFlag=CRYPT_VERIFYCONTEX则为交换密钥
						//公私密钥存入bPkey,bSkey中
	char	cProvider[256];
	DWORD	dwImptype;
	DWORD	dwVersion;
	WORD	wSigKeySizeInc;//->PP_SIG_KEYSIZE_INC
	WORD	wKeyxKeySizeInc;//->PP_KEYX_KEYSIZE_INC
	LPCSTR	szKeySetSecDescr;//->PP_KEYSET_SEC_DESCR
	DWORD	dwProvType;		//->PP_PROVTYPE
//以下为满足调用CPGetUserKey，CPExportKey
	DWORD	bHaveDefaultKey;
	DWORD	dwPermissions;//权限
	struct	CardContainer strCardContainer;
};

#endif