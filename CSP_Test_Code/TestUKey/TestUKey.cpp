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
	
	unsigned char signatureData[128] = {0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x30,0x21,0x30,0x09,0x06,0x05,0x2B,0x0E,0x03,0x02,0x1A,0x05,0x00,0x04,0x14,0xE3,0xFF,0xB9,0x84,0x0F,0x36,0x7E,0xB8,0xF5,0xD8,0xC5,0x79,0xB8,0x13,0xEA,0x35,0xD0,0x68,0x9C,0x04};
	unsigned char encodeData[] = {0x00,0x14,0x91,0xCF,0x29,0x4E,0xF8,0x9D,0xF3,0x91,0x86,0x27,0xF8,0xDD,0xD2,0xFB,0x9B,0xDA,0x6E,0xAE,0x2D,0x61,0x60,0x7E,0x05,0x3E,0xD6,0x36,0xD3,0x35,0x15,0x52,0x31,0x42,0xB3,0x64,0x7B,0x25,0x06,0xB6,0xE1,0x1E,0xDC,0x31,0xBA,0xF6,0x6C,0x7D,0x1B,0x3E,0x8D,0x6A,0xA9,0xDB,0x26,0x8F,0xAB,0xC3,0xE6,0x30,0x63,0x01,0x35,0x65,0x9F,0xFF,0x6A,0x79,0x8B,0x04,0x3E,0x91,0xC9,0xFC,0x80,0x8E,0xC4,0x78,0x0B,0x34,0x41,0xF5,0xF6,0x8B,0x36,0x61,0x0A,0x85,0xE0,0xDB,0x79,0xE2,0x10,0xBA,0xCC,0x51,0xC5,0xD2,0x1E,0xD8,0x00,0xF2,0x88,0x75,0xD4,0xB1,0xDC,0x06,0xBD,0x6B,0x96,0x66,0x32,0x86,0x0F,0xD9,0x7E,0xFA,0xF4,0xAA,0xCB,0x10,0xF7,0x12,0x80,0x69,0xC4,0x5C};
	unsigned char decodeData[] = {0x77,0x12,0xE8,0xA2,0x59,0x73,0x3D,0x04,0x83,0xB1,0x18,0x53,0xB6,0xD7,0xEC,0x3B,0x21,0x77,0x4A,0x0F,0x8D,0x7B,0x79,0x50,0x05,0x54,0x7F,0x65,0x33,0xFD,0x92,0x1B,0x02,0x7D,0x97,0x94,0x63,0x05,0x78,0x3F,0x5C,0x5D,0x07,0x4D,0x46,0x3F,0xAB,0x5F,0x10,0xEC,0x0F,0xF4,0x2A,0x04,0xD9,0xC9,0x28,0xB8,0xCE,0x24,0x7D,0xE3,0x5D,0xCB,0xED,0x45,0xA9,0x6A,0x10,0x89,0x62,0x2B,0x06,0x0A,0x2B,0xBC,0x32,0x24,0x27,0xA5,0x1C,0xC5,0x6F,0x67,0xC9,0x42,0x61,0xAC,0x57,0xA1,0xE6,0x25,0x0F,0x28,0x0E,0xDA,0xCE,0x91,0xD5,0x61,0xF3,0x22,0x77,0xBD,0x0C,0x41,0xF3,0x91,0x6E,0x5F,0x65,0xAA,0xBA,0xE9,0x6A,0x3D,0x12,0x9D,0x09,0xF0,0x7A,0x61,0xAC,0xCD,0xCA,0xA1,0x21,0xEE};
	
	const unsigned char decodedData[] = {0x83,0x6B,0xB7,0xAA,0x8F,0x8D,0x15,0x40,0x27,0xBB,0xFF,0xFB,0xD3,0xB2,0x2C,0xC5,0xA7,0x73,0x62,0x48,0x7B,0x8C,0x4C,0xC8,0x4E,0xBF,0x5D,0x00,0xD1,0xA8,0x49,0xFF,0x9F,0xDE,0x21,0xB4,0xAB,0x2B,0x4E,0x2A,0xEC,0x88,0x95,0x7E,0x9E,0xDD,0x00,0x95,0xC3,0x59,0xEE,0xBF,0x2A,0x81,0x08,0x72,0xAB,0x9B,0x65,0x4B,0xBB,0x2D,0xDE,0x8E,0xA2,0x45,0x0C,0xEA,0x7E,0xF7,0x91,0xD3,0xD4,0x2E,0x65,0x45,0xA3,0x11,0x45,0x2E,0x86,0xFE,0x69,0x22,0x7D,0x26,0x62,0xBB,0x85,0xE4,0x4D,0xE4,0xB3,0x1E,0x61,0x8A,0x0E,0xF4,0xB2,0x1A,0x91,0x67,0xB4,0x3D,0xC4,0x77,0x17,0x30,0x47,0xB0,0x6F,0x26,0x9F,0x69,0x20,0xF1,0x86,0x8B,0xCF,0x6B,0x42,0xAD,0x20,0xBC,0x5D,0xDE,0xDF,0x49};
	const unsigned char encodedData[] = {0xBA,0x3D,0x19,0xC0,0x9F,0x70,0xD9,0xB8,0x16,0x46,0xB3,0x15,0x80,0x08,0x3F,0x8F,0x50,0x40,0x3B,0x21,0x6F,0x9D,0xDB,0x87,0xA0,0x54,0xAC,0xAA,0x7C,0xBA,0x57,0xEB,0x7D,0x00,0xCD,0x51,0xFC,0x8F,0xAC,0x21,0xED,0xDE,0x11,0x5B,0x98,0xA4,0x5C,0x1B,0x80,0xC1,0x78,0xE0,0xC6,0xC5,0x17,0xD1,0xA4,0xEF,0xC3,0x70,0xA6,0x0E,0xA8,0x59,0x83,0x44,0x11,0x5C,0xE7,0xDB,0xCE,0x0D,0x34,0xAC,0x2E,0xC5,0x61,0xA7,0xF2,0xF7,0x09,0x5A,0x0A,0x2A,0x4F,0x0E,0x5A,0xCA,0x30,0xD7,0x8C,0xC2,0xCA,0x02,0x91,0x50,0x11,0x76,0x22,0xE0,0x31,0x4B,0x99,0x53,0xBE,0x3D,0x82,0xAF,0x3D,0x63,0x26,0xDD,0x40,0xD0,0x25,0x61,0x2E,0x59,0x68,0x36,0x56,0x45,0x8C,0xF0,0x2A,0x8A,0x1B,0x37};
	int length = sizeof(encodeData)/sizeof(unsigned char);
	apduData[0] = 0x00; //cls
	apduData[1] = 0x84; //ins
	apduData[2] = 0x00; // p1 根据sfi更新
	apduData[3] = 0x00; // p2 从文件头开始
	apduData[4] = 0x08; // wCertAddr

	memcpy(&apduData[5], encodeData, length);
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
//		if(UKeyRSASignature(hUKey, 0x00, signatureData, length, respData, ReaderNo))
//		if(UKeyCheckPIN(hUKey, 1))
//		if(UKeyGetContainer(hUKey, pContainerName, pstrCardCt))
		if(UKeyRSAEncrypt(hUKey, 0x00, encodeData, &length, respData, ReaderNo))
		{
			for(int i = 0; i < 128; i ++){
				if(respData[i] != encodedData[i])
				{
					break;
				}
			}
			if(i != 128)
			{
				printf("encode error!!!\n");
				getchar();
			}
			else
			{
				printf("Encode done!!!\n");
			}
/*
			if(UKeyRSASignatureVerify(hUKey, 0x00, respData, *ReaderNo, apduData, ReaderNo))
			{
				printf("Hello World!\n");
				getchar();
			}
*/
		}
		if(UKeyRSADecrypt(hUKey, 0x00, decodeData, &length, respData, ReaderNo))
		{
			for(int i = 0; i < 128; i ++){
				if(respData[i] != decodedData[i])
				{
					break;
				}
			}
			if(i != 128)
			{
				printf("encode error!!!\n");
				getchar();
			}
			else
			{
				printf("Decode done!!!\n");
			}
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
	getchar();
  return 0;
}

