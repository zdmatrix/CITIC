// TestUKey.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "UKeyAPI.h"
#include "HDIFD20B.h"

int main(int argc, char* argv[])
{
	HANDLE ContextNo;
	HANDLE hUKey;
//	HANDLE devNo;

	unsigned char* srATR;
	short* srATRLen;
//	short ivCardSeat;
	int* ReaderNo;
	int* ReaderCount;


	unsigned short retvalue;
	unsigned char respData[255];
	unsigned char apduData[255];
	short respDataLen = 0;

	unsigned short count;

	ReaderNo = (int*)malloc(sizeof(int));
	ReaderCount = (int*)malloc(sizeof(int));

	*ReaderNo = 0x100;

	srATR = (unsigned char*)malloc(255 * sizeof(unsigned char));
	srATRLen = (short*)malloc(sizeof(short));
/*	
	apduData[0] = 0x80; //cls
	apduData[1] = 0xBF; //ins
	apduData[2] = 0x01; // p1 根据sfi更新
	apduData[3] = 0x00; // p2 从文件头开始
	apduData[4] = 0x02; // wCertAddr
	apduData[5] = 0x38;
	apduData[6] = 0x32;
	apduData[7] = 0x00;
*/
	
	unsigned char indata[128] = {0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x30,0x21,0x30,0x09,0x06,0x05,0x2B,0x0E,0x03,0x02,0x1A,0x05,0x00,0x04,0x14,0xE3,0xFF,0xB9,0x84,0x0F,0x36,0x7E,0xB8,0xF5,0xD8,0xC5,0x79,0xB8,0x13,0xEA,0x35,0xD0,0x68,0x9C,0x04};
	
	int length = sizeof(indata)/sizeof(unsigned char);
	apduData[0] = 0x00; //cls
	apduData[1] = 0x84; //ins
	apduData[2] = 0x00; // p1 根据sfi更新
	apduData[3] = 0x00; // p2 从文件头开始
	apduData[4] = 0x08; // wCertAddr

	memcpy(&apduData[5], indata, length);
	apduData[5 + length] = 0x00;
	
	BYTE pstrCardCt[0x80] = {0};
	
//	BYTE pContainerName[] = "SCM Microsystems Inc. SCR33x USB Smart Card Reader 0";
	BYTE pContainerName[0x80] = {0};	
		//	UKeyTestDisplayNum(hUKey, 0x34);

//	retvalue = HD_ContextInitialize(&ContextNo);
//	if(0x9000 == retvalue){
//		retvalue = HD_GetReaderList(ContextNo, ReaderNo, ReaderCount);
//		if(0x9000 == retvalue){
	if(UKeyOpen(&hUKey))
	{
//		if(UKeyGetCert(hUKey, NULL, pbdate, 0x7fc))
//		if(UKeyRSASignature(hUKey, 0x00, indata, length, respData, ReaderNo))
//		if(UKeyCheckPIN(hUKey, 1))
		if(UKeyGetContainer(hUKey, pContainerName, pstrCardCt))
		{
			printf("Hello World!\n");
			getchar();
		}
		else
		{
			printf("UKeyGetCspContainerCount Failed!\n");
			getchar();
		}
		
	}
/*
	retvalue = HD_OpenPort(21, 9600, 8, &hUKey);
			if(0x9000 == retvalue){
				retvalue = HD_ResetCard(hUKey, srATR, srATRLen, 1);
				if(0x9000 == retvalue){
//					retvalue = UKeyGetCspContainerCount(hUKey, &count, NULL);
					retvalue = UKeyGetCert(hUKey, NULL, pbdate, 0x7fc);
//					retvalue = HD_ApduT0(hUKey, apduData, 5, respData, &respDataLen, 1);
					if(retvalue){
						printf("Hello World!\n");
						getchar();
					}else{
						printf("UKeyGetCspContainerCount Failed!\n");
						getchar();
					}
					
				}
				
			}
			
//		}
		
//	}
*/
  return 0;
}

