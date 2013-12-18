// TestContainer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

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

	ReaderNo = (int*)malloc(sizeof(int));
	ReaderCount = (int*)malloc(sizeof(int));

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
	apduData[0] = 0x00; //cls
	apduData[1] = 0x84; //ins
	apduData[2] = 0x00; // p1 根据sfi更新
	apduData[3] = 0x00; // p2 从文件头开始
	apduData[4] = 0x08; // wCertAddr
	apduData[5] = 0x00;
	
	retvalue = HD_ContextInitialize(&ContextNo);
	if(0x9000 == retvalue){
		retvalue = HD_GetReaderList(ContextNo, ReaderNo, ReaderCount);
		if(0x9000 == retvalue){
			retvalue = HD_OpenPort(21, 9600, 8, &hUKey);
			if(0x9000 == retvalue){
				retvalue = HD_ResetCard(hUKey, srATR, srATRLen, 1);
				if(0x9000 == retvalue){
					retvalue = HD_ApduT0(hUKey, apduData, 5, respData, &respDataLen, 1);
					printf("Hello World!\n");
					getchar();
				}
				
			}
			
		}
		
	}

	
	return 0;
}

