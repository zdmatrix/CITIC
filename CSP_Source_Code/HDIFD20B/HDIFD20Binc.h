#define MAX_BUF_LEN     512
#define MAX_APDU_RET_DATA 256
#define MAX_APDU_ISO_T0   262
#define MIN_RET_DATA_LEN  4

/********define by hp 070801 in function(GetReaderList)***********
#define NUMBER_OF_READERS	4
#define NAME_LENGTH			100
#define MAX_RESPONSE		2000
************************************************/

#define EXCUTE_SUC		0x9000

#define CER_RANGE       0xff0c  //The parameters range is invalid 
#define CER_NOCARD		0xff0f
#define CER_APDU	    0xff10   //The APDU data format error 
#define CER_HEXSTR		0xff30	//非法十六进制串，有‘0’～‘9’，‘a’～‘f’，‘A’～‘F’以外的字符，或字符个数不是偶数
#define CER_RESULT		0xff38	//结果溢出
#define CER_UNKNOWN     0xffff	//无错误信息给出	

#define CER_PCSC_SCardReleaseContext		0xff60
#define CER_PCSC_SCardEstablishContext		0xff61
#define CER_PCSC_SCardListReaders           0xff62
#define CER_PCSC_SCardListReaders_NONE      0xff63     //No pcsc reader found
#define CER_PCSC_SCardConnect               0xff64     //
#define CER_PCSC_ProtocolType               0xff65     //Not T0, T1
#define CER_PCSC_SCardStatus                0xff66     //Get ATR error
#define CER_PCSC_SCardReconnect             0xff67     //Reset Card Fail
#define CER_PCSC_SCardDisconnect            0xff68     //Power off fail

#define CER_PCSC_SCardTransmit              0xff69     //SCardTransmit failed.

#define CER_PCSC_SCardTransmit_Data_LOST    0xff6A     //case 2 data less than 2

#define CER_PCSC_SCardGetAttrib             0xff70     
#define CER_PCSC_SCardListCards             0xff71     
#define CER_PCSC_SCardCancel	            0xff72    
#define CER_PCSC_SCardBeginTransaction      0xff73    
#define CER_PCSC_SCardEndTransaction        0xff74    
#define CER_PCSC_SCardGetProviderId         0xff75    
#define CER_PCSC_SCardGetStatusChange         0xff76 

  
 

