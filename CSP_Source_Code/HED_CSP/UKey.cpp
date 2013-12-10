#include "StdAfx.h"
#include "UKey.h"
#include "HDIFD20B.H"
#include "Define.h"
#include "PinDlg1.h"
#include "ChangePin.h"
#include "RSAREF.h"
#include <X509.h>

DWORD g_dwUkeyError = 0;

void SetUkeyLastErr(DWORD dwErr)
{
	g_dwUkeyError = dwErr;
	return;
}

DWORD GetUkeyLastErr(void)
{
	return g_dwUkeyError;
}

//打开UKey
BOOL UKeyOpen(HANDLE *hUKey)
{
	unsigned short ret = HD_OpenPort(21, 9600, 8, hUKey);
	return ret == OPERATION_SUCCESS;
}
	
//关闭UKey
BOOL UKeyClose(HANDLE hUKey)
{
	unsigned short ret = HD_ClosePort(hUKey) ;
	return ret == OPERATION_SUCCESS;
}

// 验证Pin
BOOL UKeyVerifyPIN(HANDLE hUKey, const char *Pin, const short PinLen)
{
	if ((PinLen % 2) != 0)
		return false;

	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x00; //cls
	apduData[1] = 0x20; //ins
	apduData[2] = 0x00; //p1
	apduData[3] = 0x3f; //p2
	apduData[4] = PinLen / 2; //lc
	int n = 0;
	int i = 0;
	int j = 5;
	char c;
	while (i < PinLen)
	{
		c = Pin[i];
		n = atoi(&c);
		i++;
		c = Pin[i];
		n = n * 16 + atoi(&c);
		apduData[j] = n;
		j++;
		i++;
	}
	short srAPDULen = (short)5+ (PinLen / 2);
	short respDataLen = (short)0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if ((ret / 0x10) == VERIFY_FAILED)
		AfxMessageBox("密码错误，请重新输入");
	return ret == OPERATION_SUCCESS;
}

BOOL UKeyCheckPIN(HANDLE hUKey)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CPinDlg pwd;
	CString szMsg;
	WORD wPinCount=0;
	char bBuf[256];

	while(1)
	{
		memset(bBuf,0,sizeof(bBuf));
		if(pwd.DoModal()==IDCANCEL)
		{
			return FALSE;
		}
		memcpy(bBuf,pwd.m_sPassword,strlen(pwd.m_sPassword));
		if(UKeyVerifyPIN(hUKey, bBuf, strlen(pwd.m_sPassword)))
			return TRUE;
	}
}

// 验证Pin
BOOL UKeyChangePINToUkey(HANDLE hUKey, const char *Pin, const short PinLen)
{
	if ((PinLen % 2) != 0)
		return false;

	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0x5E; //ins
	apduData[2] = 0x01; //p1
	apduData[3] = 0x3f; //p2
	apduData[4] = PinLen / 2; //lc
	int n = 0;
	int i = 0;
	int j = 5;
	char c;
	while (i < PinLen)
	{
		c = Pin[i];
		n = atoi(&c);
		i++;
		c = Pin[i];
		n = n * 16 + atoi(&c);
		apduData[j] = n;
		j++;
		i++;
	}
	short srAPDULen = (short)5+ (PinLen / 2);
	short respDataLen = (short)0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if ((ret / 0x10) == VERIFY_FAILED)
		AfxMessageBox("旧密码错误，请重新输入");
	return ret == OPERATION_SUCCESS;
}
BOOL UKeyChangePIN(HANDLE hUKey)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CChangePin pwd;
	CString szMsg;
	WORD wPinCount=0;
	char bBuf[256];
	char OldPin[32];
	char NewPin1[32];
	char NewPin2[32];
    int OldPinLen, NewPinLen, BufLen;
	while(1)
	{
		memset(OldPin,0,sizeof(bBuf));
		memset(NewPin1,0,sizeof(bBuf));
		memset(NewPin2,0,sizeof(bBuf));
		if(pwd.DoModal()==IDCANCEL)
		{
			return FALSE;
		}
		OldPinLen = strlen(pwd.m_OldPin);
		NewPinLen = strlen(pwd.m_NewPin1);
		memcpy(OldPin,pwd.m_OldPin, OldPinLen);
		memcpy(NewPin1,pwd.m_NewPin1, NewPinLen);
		memcpy(NewPin2,pwd.m_NewPin2,strlen(pwd.m_NewPin2));
		if (strcmp(NewPin1, NewPin2) != 0)
		{
			AfxMessageBox("两次输入密码不一致");
			continue;
		}
        memcpy(bBuf, OldPin, OldPinLen);
		memcpy(&bBuf[OldPinLen], "FF", 2);
		memcpy(&bBuf[OldPinLen + 2], NewPin1, NewPinLen);
		
		BufLen = OldPinLen + 2 + NewPinLen;
		if(UKeyChangePINToUkey(hUKey, bBuf, BufLen))
			return TRUE;
	}
}

BOOL UKeyGetChallange(HANDLE hUKey, unsigned char *Challange, const short Len)
{
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x00; //cls
	apduData[1] = 0x84; //ins
	apduData[2] = 0x00; //p1
	apduData[3] = 0x00; //p2
	apduData[4] = Len; //le

	short srAPDULen = 5;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return false;

	memcpy(Challange, respData, Len);
	return true;
}

//读取卡内容器数据
BOOL UKeyGetContainerData(HANDLE hUKey, unsigned char *Data, short *DataLength, short *ContainerCount)
{
	short srAPDULen = 5;
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x00; //cls
	apduData[1] = 0xB2; //ins
	apduData[2] = 0x01; //p1
	apduData[3] = 0xD5; //p2 从头读到尾
	apduData[4] = 00; //le

	
	short respDataLen = 0;
	unsigned char respData[1000] = {0};

	short respDataLenSub = 0;
	unsigned char respDataSub[256] = {0};

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if ((ret != OPERATION_SUCCESS) && ((ret/0x100) != 0x61))
		return false;
    if (respDataLen == 0)
		return false;
    short LeftLen = 0;
	while (ret != OPERATION_SUCCESS)
	{
		// 读取后续未读出的字节
		apduData[0] = 0x00; //cls
		apduData[1] = 0xC0; //ins
		apduData[2] = 0x00; //p1
		apduData[3] = 0x00; //p2 
		LeftLen = ret % 0x100;
		apduData[4] = LeftLen; //le

		ret = HD_ApduT0(hUKey, apduData, srAPDULen, &respData[respDataLen], &LeftLen, 0);
		if ((ret != OPERATION_SUCCESS) && ((ret/0x100) != 0x61))
			return false;
		respDataLen += LeftLen;
	}
	if ((respDataLen % sizeof(CardContainer)) != 0) 
		return false;

	memcpy(Data, respData, respDataLen);
	*DataLength = respDataLen;
	*ContainerCount = respDataLen / sizeof(CardContainer);
	return true;
}

BOOL UKeyGetContainer(HANDLE hUKey,BYTE *pbContainer,BYTE *pstrCardCt)
{
	short respDataLen = 0;
	unsigned char respData[1000] = {0};
	short count = 0;
	bool ret = UKeyGetContainerData(hUKey, respData, &respDataLen, &count);
	if (!ret)
		return false;
  
	CardContainer *pContainer = (CardContainer *)respData;
	int i = 0;
	while (i < count) 
	{
		if (strcmp((char *)pContainer->pbContName, (char *)pbContainer) == 0)
		{
			memcpy(pstrCardCt, pContainer, sizeof(CardContainer));
			return true;
		}
		pContainer++;
		i++;
	}
	return false;
}

BOOL UKeyGetCspContainerCount(HANDLE hUKey,WORD *wCount)
{
	short respDataLen = 0;
	unsigned char respData[1000] = {0};
	short count = 0;
	char EmptyData[64];
	for (int k=0; k<64; k++)
		EmptyData[k] = 'F';
	bool ret = UKeyGetContainerData(hUKey, respData, &respDataLen, &count);
	if (!ret)
		return false;
  
	CardContainer *pContainer = (CardContainer *)respData;
	int i = 0;
	int j = 0;
	while (i < count) 
	{
		if ((strlen((char *)pContainer->pbContName) != 0) &&
			(strcmp((char *)pContainer->pbContName, EmptyData) != 0))
		{
			j++;
		}
		pContainer++;
		i++;
	}
	*wCount = j;
	return true;
}
BOOL UKeyFindContainer(HANDLE hUKey,BYTE *pbContainer)
{
	short respDataLen = 0;
	unsigned char respData[1000] = {0};
	short count = 0;
	bool ret = UKeyGetContainerData(hUKey, respData, &respDataLen, &count);
	if (!ret)
		return false;
  
	CardContainer *pContainer = (CardContainer *)respData;
	int i = 0;
	while (i < count) 
	{
		if (strcmp((char *)pContainer->pbContName, (char *)pbContainer) == 0)
		{
			return true;
		}
		pContainer++;
		i++;
	}
	return false;
}

BOOL UKeyDelContainer(HANDLE hUKey,BYTE *pbContainer)
{
	short respDataLen = 0;
	unsigned char respData[1000] = {0};
	short count = 0;
	short srAPDULen = 5;
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));

	bool ret = UKeyGetContainerData(hUKey, respData, &respDataLen, &count);
	if (!ret)
		return false;
  
	CardContainer *pContainer = (CardContainer *)respData;

	unsigned short i = 0;
	while (i < count) 
	{
		if (strcmp((char *)pContainer->pbContName, (char *)pbContainer) == 0)
		{
			memset(pContainer->pbContName, 0, sizeof(pContainer->pbContName));
			apduData[0] = 0x00; //cls
			apduData[1] = 0xDC; //ins
			apduData[2] = i + 1; //p1
			apduData[3] = 0xD4; //p2 标识符加P1
			apduData[4] = sizeof(CardContainer); //lc
			memcpy(&apduData[5], pContainer, sizeof(CardContainer));
			srAPDULen = 5 + sizeof(CardContainer);
			apduData[srAPDULen] = 0;
			unsigned short nRet = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
			if (nRet == OPERATION_SUCCESS) 
				return true;
			break;
		}
		i++;
		pContainer++;
	}
	return false;
}

BOOL UKeyAddContainer(HANDLE hUKey,BYTE *strCardCt)
{
	short respDataLen = 0;
	unsigned char respData[1000] = {0};
	short count = 0;
	short srAPDULen = 5;
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));

	bool ret = UKeyGetContainerData(hUKey, respData, &respDataLen, &count);
	if (!ret)
		return false;
	char EmptyData[64];
    for (int k=0; k<64; k++)
		EmptyData[k] = 'F';
	CardContainer *pContainer = (CardContainer *)respData;
	CardContainer *p = pContainer;
	CardContainer *pCardCt = (CardContainer *)strCardCt;
	int EmptyPos = 0;
	int i = 0;
	while (i < count) 
	{
		// 找到第一个为空的位置
		if ((strlen((char *)pContainer->pbContName) == 0) ||
			(strcmp((char *)pContainer->pbContName, EmptyData) == 0))
		{
			if (EmptyPos == 0)
				EmptyPos = i + 1;
		}
		//如果有同名的，直接返回错误
		if (strcmp((char *)pContainer->pbContName, (char *)pCardCt->pbContName) == 0)
		{
			return false;
		}

		i++;
		pContainer++;
	}

	apduData[0] = 0x00;     //cls
	apduData[1] = 0xDC;     //ins
	apduData[2] = EmptyPos + 1; //p1
	apduData[3] = 0xD4;     //p2 标识符加P1
	apduData[4] = sizeof(CardContainer); //lc
	memcpy(&apduData[5], strCardCt, sizeof(CardContainer));
	srAPDULen = 5 + sizeof(CardContainer);
	apduData[srAPDULen] = 0;
	unsigned short nRet = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (nRet == OPERATION_SUCCESS) 
		return true;

	return false;
}
BOOL UKeyGenKey(HANDLE hUKey, WORD wKeyPSfi, WORD wKeySSfi)
{
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xD0; //ins
	apduData[2] = 0x00 | wKeyPSfi; //p1 保存公钥文件 文件名 0F 
	apduData[3] = 0x00 | wKeySSfi; //p2 保存私钥文件 文件名 0E
	apduData[4] = 0x00; //le

	short srAPDULen = 5;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return false;

	return true;	
}
BOOL UKeyGetKey(HANDLE hUKey, DWORD sfi, unsigned char KeyFlag, unsigned char *Key)
{
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xD8; //ins
	apduData[2] = 0x40 | sfi; //p1 根据sfi读取公钥 
	apduData[3] = 0x02; //p2 读取模数
	apduData[4] = 0x80; //le

	short srAPDULen = 5;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return false;

	if (KeyFlag == PUBLIC_KEY_FLAG)
	{
		R_RSA_PUBLIC_KEY *p = (R_RSA_PUBLIC_KEY *)Key;
		p->bits = 1024;                           /* length in bits of modulus */
		memcpy(p->modulus, respData, respDataLen);                    /* modulus */
		apduData[3] = 0x01; //p2 读取指数
		apduData[4] = 0x04; //le
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
		if (ret != OPERATION_SUCCESS) 
			return false;
		memcpy(p->exponent, respData, respDataLen);   
	}
	return true;	
}

BOOL UKeyRSAEncrypt(HANDLE hUKey, DWORD sfi, unsigned char *InData, int *InDataLen, 
					unsigned char *OutData, int *OutDataLen)
{
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xF6; //ins
	apduData[2] = 0x80 | sfi; // p1 根据sfi读取私钥 
	apduData[3] = 0x00; //p2
	apduData[4] = 0x80; //le

	memcpy(&apduData[5], InData, *InDataLen);
	apduData[*InDataLen + 5] = 0x80;
	apduData[*InDataLen + 6] = 0x00;
	short srAPDULen = *InDataLen + 6;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return false;
	memset(OutData, 0, *OutDataLen);
	memcpy(OutData, respData, respDataLen);
	*OutDataLen = respDataLen;

	return true;	
}

BOOL UKeyRSADecrypt(HANDLE hUKey, DWORD sfi, unsigned char *InData, int *InDataLen, 
					unsigned char *OutData, int *OutDataLen)
{
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xF8; //ins
	apduData[2] = 0x80 | sfi; //p1 根据sfi读取私钥 
	apduData[3] = 0x00; //p2 读取莫属
	apduData[4] = 0x80; //le

	memcpy(&apduData[5], InData, *InDataLen);
	apduData[*InDataLen + 5] = 0x80;
	apduData[*InDataLen + 6] = 0x00;
	short srAPDULen = *InDataLen + 6;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return false;
	memset(OutData, 0, *OutDataLen);
	memcpy(OutData, respData, respDataLen);
	*OutDataLen = respDataLen;

	return true;	
}
BOOL UKeyRSASignature(HANDLE hUKey, DWORD sfi, unsigned char *InData, int InDataLen, 
					unsigned char *OutData, int *OutDataLen)
{
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xF2; //ins
	apduData[2] = 0x80 | sfi; // p1 根据sfi读取公钥 
	apduData[3] = 0x00; // p2 
	apduData[4] = 0x80; //le

	memcpy(&apduData[5], InData, InDataLen);
	apduData[InDataLen + 5] = 0x80;
	apduData[InDataLen + 6] = 0x00;
	short srAPDULen = InDataLen + 6;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return false;
	memset(OutData, 0, *OutDataLen);
	memcpy(OutData, respData, respDataLen);
	*OutDataLen = respDataLen;

	return true;	
}

BOOL UKeyRSASignatureVerify(HANDLE hUKey, DWORD sfi, const unsigned char *InData, int InDataLen, 
					unsigned char *OutData, int *OutDataLen)
{
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xF4; //ins
	apduData[2] = 0x80 | sfi; // p1 根据sfi读取公钥 
	apduData[3] = 0x00; // p2 
	apduData[4] = 0x80; //le

	memcpy(&apduData[5], InData, InDataLen);
	apduData[InDataLen + 5] = 0x00;
	//apduData[*InDataLen + 6] = 0x00;
	short srAPDULen = InDataLen + 5;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret == 0x6180)
	{
		memset(apduData, 0, sizeof(apduData));
		apduData[0] = 0x00; //cls
		apduData[1] = 0xC0; //ins
		apduData[2] = 0x00; // p1 
		apduData[3] = 0x00; // p2 
		apduData[4] = 0x80; //le
		srAPDULen = 5;
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	}
	if (ret != OPERATION_SUCCESS) 
		return false;
	memset(OutData, 0, *OutDataLen);
	memcpy(OutData, respData, respDataLen);
	*OutDataLen = respDataLen;

	return true;	
}

BOOL UKeyGetNameFromCN(char *cn,char *name)
{
	WORD wNamePos,wEmailPos;
	for(wNamePos=0; wNamePos<strlen(cn)-strlen("/CN="); wNamePos++)
	{
		if(!memcmp(cn+wNamePos,"/CN=",strlen("/CN=")))
		{
			wNamePos += strlen("/CN=");
			break;
		}
	}

	if(wNamePos == strlen(cn))
		return FALSE;

	for(wEmailPos=wNamePos; wEmailPos<strlen(cn); wEmailPos++)
	{
		if(!memcmp(cn+wEmailPos,"/",strlen("/")))
			break;
	}
	memcpy(name, cn+wNamePos, (wEmailPos - wNamePos));
	return TRUE;
}

BOOL UKeyGetCertName(BYTE *pbData, DWORD dwLen, char *pcOutName)
{
	BYTE  *p;
	X509 *x=NULL;
	X509_NAME *name=NULL;
	char cn[512];
	p = pbData;
	x = d2i_X509(NULL, (const unsigned char **)&p, dwLen);
	name = X509_get_subject_name(x);
	memset(cn, 0, sizeof(cn));
	X509_NAME_oneline(name, cn, sizeof(cn));
	//此时cn中就是该证书持有人对应的CN;
	X509_free(x);

	if(!UKeyGetNameFromCN(cn,pcOutName))
		return FALSE;
	return TRUE;
}

BOOL UKeyAppendCert(HANDLE hUKey, BYTE *pstrCardCt, BYTE *pbData, DWORD dwLen)
{
	char cName[128];
	WORD wCertAddr;
	memset(cName,0,sizeof(cName));
	CardContainer *PContainer = (CardContainer *)pstrCardCt;
	memcpy(cName, PContainer->pbCertName, sizeof(PContainer->pbCertName));
	wCertAddr = PContainer->wCertAddr;
	PContainer->wCertSize = dwLen;
	if(!UKeyGetCertName(pbData, dwLen, cName))//证书格式不对
		return FALSE;
	unsigned char apduData[300];// = ;
	short srAPDULen = 0;
	short respDataLen = 0;
	unsigned char respData[255];
	unsigned short ret = 0;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x00; //cls
	apduData[1] = 0xD6; //ins
	apduData[2] = 0x80 | wCertAddr; // p1 根据sfi更新
	apduData[3] = 0x00; // p2 从文件头开始
	int count = dwLen / 0xFF;
	int i=0;
	//如果长度大于一次可写入的长度，分段写入
	for (i=0; i<count; i++)
	{
		apduData[3] = 0xFF * i;
		apduData[4] = 0xFF;  //lc
		memcpy(&apduData[5], (BYTE *)(pbData + i * 0xFF), 0xFF);
		apduData[0xFF + 5] = 0x00;
		srAPDULen = 5 + 0xFF;
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
		if (ret != OPERATION_SUCCESS) 
			return false;
	}
	count = dwLen % 0xFF;
	if (count > 0)
	{
		apduData[3] = 0xFF * i;
		apduData[4] = count; //lc
		memcpy(&apduData[5], (BYTE *)(pbData + i * 0xFF), count);
		apduData[count + 5] = 0x00;
		srAPDULen = 5 + count;
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
		if (ret != OPERATION_SUCCESS) 
			return false;
	}

	return true;
}
BOOL UKeyGetCert(HANDLE hUKey, BYTE *pstrCardCt, BYTE *pbData, DWORD dwLen)
{
	WORD wCertAddr;
	CardContainer *PContainer = (CardContainer *)pstrCardCt;
	wCertAddr = PContainer->wCertAddr;
	PContainer->wCertSize = dwLen;
	
	unsigned char apduData[255];// = ;
	short srAPDULen = 0;
	short respDataLen = 0;
	unsigned char respData[255];
	unsigned short ret = 0;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x00; //cls
	apduData[1] = 0xB0; //ins
	apduData[2] = 0x80 | wCertAddr; // p1 根据sfi更新
	apduData[3] = 0x00; // p2 从文件头开始
	int count = dwLen / 0xFF;
	if ((dwLen % 0xFF) != 0)
		count++;
	int i=0;
	//如果长度大于一次可写入的长度，分段写入
	int templen = 0xFF;
	for (i=0; i<count; i++)
	{
		apduData[4] = templen;  //le
		apduData[5] = 0x00;
		srAPDULen = 5;
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
		if (((ret / 0x100) != 61) || (ret != OPERATION_SUCCESS)) 
			return false;
		memcpy((BYTE *)(pbData + i * 0xFF), respData, respDataLen);
		if (ret == OPERATION_SUCCESS) 
			break;
		templen = ret % 0x100;
	}

	return true;
}

BOOL UKeyImportKey(HANDLE hUKey, DWORD sfi, unsigned char *Key)
{
	unsigned char apduData[255];// = ;
	short respDataLen = 0;
	unsigned char respData[255];

	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xD2; //ins
	apduData[2] = 0x40 | sfi; //p1 根据sfi读取公钥 
	apduData[3] = 0x01; //p2 指数
	apduData[4] = 0x04; //le

	R_RSA_PUBLIC_KEY *p = (R_RSA_PUBLIC_KEY *)Key;
	memcpy(&apduData[5], p->exponent, 4);
	short srAPDULen = 5 + 4;
	apduData[srAPDULen] = 0;

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return false;

	apduData[3] = 0x02; //p2 模
	apduData[4] = 0x80; //le
	memcpy(&apduData[5], p->modulus, p->bits / 8);
	srAPDULen = 5 + (p->bits / 8);
	apduData[srAPDULen] = 0;
	
	ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return false;

	return true;
}