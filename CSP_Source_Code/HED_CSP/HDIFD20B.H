#ifndef HDIFD20B_H
#define HDIFD20B_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HDIFD20B_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HDIFD20B_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef HDIFD20B_EXPORTS
#define HDIFD20B_API __declspec(dllexport)
#else
#define HDIFD20B_API __declspec(dllimport)
#endif

#define CCONV _stdcall

typedef struct _HARD_WARE_INF
{
	//chip_type
	//0x00: double chip
	//0x01: single chip
	unsigned char chip_type;

	//ram_size
	unsigned char ram_size_h;
	unsigned char ram_size_l;

	//trans_mode
	//0x00: control
	//0x01: interrupt
	unsigned char trans_mode;

	//card_trans_speed
	//0x00:	Unknown
	//0x01:	16K
	//0x02:	93K
	unsigned char card_trans_speed;
	
	_HARD_WARE_INF()
	{
		card_trans_speed = 0;
	}

} HARD_WARE_INF, *PHARD_WARE_INF;

enum READER_TYPE
{
	TYPE_USB_CONTROL = 0,//����
	TYPE_USB_INTERRUPT,	 //�ж�
	TYPE_USB_NO_DRIVER,	 //����
	TYPE_COM			 //COM
};

typedef struct _READER_OPEN_STRUCT
{
	LPTSTR szPathName;
	READER_TYPE readerType;
} READER_OPEN_STRUCT, *PREADER_OPEN_STRUCT;

//��������
/**********************************************************************************************
1.HD_OpenPort
�����豸���������ͨ/USB KEY ͨѶǰ�����ȴ��豸��
unsigned short HD_OpenPort(
	short  			ivPortNo,		// ����/USB �˿ڱ��
	unsigned long 	ivBaud,			// ͨѶ������	//USB�ӿڲ�ʹ��
	short  			ivParity,		// У�鷽ʽ		//USB�ӿڲ�ʹ��
	HANDLE 			*devNo				
)

����˵��
ivPortNo 		���ڱ��: Windows�п�����1��2��3��4��	
                USB �˿ڱ��: 21, 22, 23, 24
ivBaud			ͨѶ������, ����״̬����Ч;����9600, 19200 , 38400, 115200 ������ͨѶ��
ivParity	    У�鷽ʽ, ����״̬����Ч;0��У��λ(ȱʡ);1��У��;2żУ��;
devNo			���ص��豸��

����ֵ
��������0x9000��ʾ��ȷ����ʱdevNo��Ч������ֵ��ʾ������������Ϣ������ġ�������Ϣ������
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_OpenPort(short ivPortNo,unsigned long ivBaud, short ivParity, HANDLE *devNo);

/**********************************************************************************************
2  HD_ClosePort
�ر��豸���ú����ر���HD_OpenPort�����򿪵��豸��
unsigned short HD_ClosePort(
	HANDLE devNo
)

����˵��
devNo				��HD_OpenPort���ص��豸��

����ֵ
��������0x9000��ʾ��ȷ�� ����ֵ��ʾ������������Ϣ������ġ�������Ϣ������
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_ClosePort(HANDLE devNo);

/**********************************************************************************************
3  HD_SetTimeout
���ó�ʱʱ�䡣���ڷ�ʽ����Ч��

unsigned short HD_SetTimeout(
    HANDLE 		devNo, 
	int 		ivTimeOut,	 // ʱ�䣬��msΪ��λ
	short 		type  		 // TimeOut����
)

����˵��
devNo		 	��HD_OpenPort���ص��豸��
ivTimeOut       ʱ��, ��msΪ��λ
type			TimeOut����(���궨��)��
				type = 0 ����PC�˿ڳ�ʱʱ��(�ȴ������������ݣ�
				type = 1 ���ö��������ڳ�ʱʱ�䣨�ȴ�PC�����ݣ�
				type = 2 ���ÿ���λӦ��ʱʱ�� (�������ȴ�3B��ʱ�䣩
				type = 3 ���ÿ���Ӧ��ʱʱ�� ���ȴ�����һ��״̬�ֽڵ�ʱ�䣩
				type = 4 ���ÿ��ּ�೬ʱʱ�� ���ȴ������������ֽڵ�ʱ�䣩
				��ivTimeOut = 0, PC�˿ڵȴ�ʱ�����޳���ϵͳȱʡֵΪ1s��

����ֵ
��������0x9000��ʾ��ȷ�� ����ֵ��ʾ������������Ϣ������ġ�������Ϣ������
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_SetTimeout( HANDLE devNo,	int ivTimeOut, short type );

/**********************************************************************************************
4 HD_ProbeCard
̽�⿨�����Ƿ��п����ڡ����ڷ�ʽ����Ч��

unsigned short HD_ProbeCard(
HANDLE 		devNo,
short 		ivCardSeat	//USB�ӿڲ�ʹ��
)		

����˵��
devNo			 ��HD_OpenPort���ص��豸��
ivCardSeat	 	 �����š�1����������2:������

����ֵ
��������0x0000ֵ��ʾ�޿����룬0x9000��ʾ�п�������ֵ��ʾ������������Ϣ������ġ�������Ϣ������
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_ProbeCard(HANDLE	devNo, short ivCardSeat);		

/**********************************************************************************************
5  HD_PowerOn
�����ϵ硣�ϵ�����󣬿���Vcc�ܽ�Ϊ5V,CLK�ܽ��ṩһ��3.684MHz���ȶ�ʱ�ӣ�RST�ܽ�Ϊ�͵�ƽ��I/O�ܽ�Ϊ�ߵ�ƽ�����ڷ�ʽ����Ч��

unsigned short  HD_PowerOn(
HANDLE 		devNo,
short  		ivCardSeat	//USB�ӿڲ�ʹ��
)

����˵��
devNo			    ��HD_OpenPort���ص��豸��
ivCardSeat		    �����š�1����������2:������

����ֵ
��������0x9000��ʾ��ȷ�� ����ֵ��ʾ������������Ϣ������ġ�������Ϣ������

ע������
�ú��������󣬿����ڸ�λ״̬����RST�ܽ�Ϊ�͵�ƽ��
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_PowerOn(HANDLE devNo, short ivCardSeat);

/**********************************************************************************************
6  HD_PowerOff
�����ϵ硣�ú����ɹ���ɺ󣬿��ĸ��ܽ������������롣���ڷ�ʽ����Ч��

unsigned short HD_PowerOff(
	HANDLE 		devNo,
	short 		ivCardSeat	//USB�ӿڲ�ʹ��
)

����˵��
devNo			��HD_OpenPort���ص��豸��
ivCardSeat		�����š�1����������2:������

����ֵ
��������0x9000��ʾ��ȷ�� ����ֵ��ʾ������������Ϣ������ġ�������Ϣ������
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_PowerOff(	HANDLE 	devNo,short	ivCardSeat);

/**********************************************************************************************
7  HD_ResetCard 
�����ܿ������ϵ縴λ��ȡ�ø�λӦ����Ϣ��

unsigned short HD_ResetCard(		
	HANDLE 			devNo,
	char			*srATR,			
    short			*srATRLen,
	short 			ivCardSeat	//USB�ӿڲ�ʹ��
)

����˵��
devNo			    ��HD_OpenPort���ص��豸��
srATR            	����յ�������
srATRLen			��Ž��յ������ݵĳ���
ivCardSeat			�����š�1����������2:������

����ֵ
��������0x9000��ʾ��ȷ����ʱsrATR������Ч������ֵ��ʾ������������Ϣ������ġ�������Ϣ��������ʱsrATR������Ч��

ע������
�ú��������󣬿���RST�ܽ�Ϊ�ߵ�ƽ�������빤��״̬�������صĸ�λӦ���ֽڴ���srATR�����С�
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_ResetCard( HANDLE devNo, unsigned char	*srATR, short	*srATRLen, short ivCardSeat);
HDIFD20B_API unsigned short CCONV HD_ResetCardVB( HANDLE devNo, char	*srATR, short ivCardSeat);

/**********************************************************************************************
8  HD_ApduT0
�����������,���������APDU Case1 Case2 Case3 Case4��ʽ��ָ�

unsigned short HD_ApduT0(				
    HANDLE 				devNo,
	char 				*apduData,
	short				srAPDULen,
	char 				*respData,
	short				*respDataLen,
	short 				ivCardSeat	//USB�ӿڲ�ʹ��
)

����˵��
devNo				��HD_OpenPort���ص��豸��
apduData			APDU����(�ֽڴ�)��CLA��INS��P1��P2�Լ�һ���ɱ䳤�ȵ�����������ʾ��������Ķ���ΪLc��Data��Lc���ֽڣ���Le��
					����Lc��Leֵ�Ĳ�ͬ����������������������±���ʾ��
					���	�ṹ
					1	CLA INS P1 P2
					2	CLA INS P1 P2 Le
					3	CLA INS P1 P2 Lc Data
					4	CLA INS P1 P2 Lc Data Le
srAPDULen			��ŷ��͵�APDU���ݵĳ���
respData			����Ӧ����
respDataLen			����Ӧ���ݵĳ���
ivCardSeat	 		������

����ֵ
��������0xffXX��ʾ����δ�ɹ�ִ�У�������Ϣ������ġ�������Ϣ����������ֵΪ������Ӧ��
������Case1�� Case2ʱ��respData�����塣������Case3�� Case4ʱ����������0x9000��respData�����壻������Ӧ��respData�����塣
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_ApduT0( HANDLE devNo,  unsigned char *apduData, short srAPDULen, unsigned char *respData, short *respDataLen, short ivCardSeat );
HDIFD20B_API unsigned short CCONV HD_ApduVB( HANDLE devNo, char *apduData, char *respData, short ivCardSeat );

/**********************************************************************************************
9 HD_GetDescriptor
��ȡ�����豸����
   
unsigned short HD_GetDescriptor(		
	HANDLE 		devNo,		
	char 		*descriptor
)

����˵��
devNo			��HD_OpenPort���ص��豸��
descriptor		�����豸��Ϣ,����CI-4��˫����������������Ϣ����Ϊ��CIDC CI-4-1.0-009600��(ʮ�����ƴ�)
				����USBKey������Ϣ����Ϊ1-15�ֽ�ATR��Ϣ.

����ֵ
��������0x9000��ʾ��ȷ����ʱdescriptor��Ч������ֵ��ʾ������������Ϣ������ġ�������Ϣ��������ʱdescriptor������Ч��
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_GetDescriptor( HANDLE devNo, char *descriptor);

/**********************************************************************************************
10 HD_ErReset
��������,��λ�źŸ�����100��ʱ�Ӻ�Ž�IO̧�ߣ��Ӷ���ȡ������
   
unsigned short HD_ErReset(		
	HANDLE      devNo,
	Char        *srHex,
	Short       Time,  (�������趨�ӳ�ʱ�䣬���ٸ�clock)
	Short       ivCardSet//USB�ӿڲ�ʹ��
)

����˵��
devNo				��icc_OpenPort���ص��豸��
srHex               �����ص����ݣ�16�ֽڡ�   
Time				�������趨�ӳ�ʱ�䣬���ٸ�clock
IvCardSet           �����š�1 - ����һ��2 - ��������

����ֵ
��������0x9000��ʾ��ȷ������ֵ��ʾ������������Ϣ������ġ�������Ϣ��������ʱdescriptor������Ч��
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_ErReset(HANDLE devNo,char *srHex, short Time, short ivCardSet);

/**********************************************************************************************
11 HD_GetDiskVer
��ȡ�����豸�汾��
   
unsigned short HD_GetDiskVer(		
	HANDLE 		devNo,		
	char 		*descriptor
)

����˵��
devNo			��HD_OpenPort���ص��豸��
descriptor		�豸�汾��.

����ֵ
��������0x9000��ʾ��ȷ����ʱdescriptor��Ч������ֵ��ʾ������������Ϣ������ġ�������Ϣ��������ʱdescriptor������Ч��
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_GetDiskVer( HANDLE devNo, char *descriptor);

/**********************************************************************************************
12 HD_DownDisk
��������Ӳ������
   
unsigned short HD_DownDisk(		
	HANDLE 		devNo,		
	unsigned char 		*descriptor
	short 
)

����˵��
devNo			��HD_OpenPort���ص��豸��
descriptor		��������Ӳ����������.

����ֵ
��������0x9000��ʾ��ȷ����ʱdescriptor��Ч������ֵ��ʾ������������Ϣ������ġ�������Ϣ��������ʱdescriptor������Ч��
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_DownDisk( HANDLE devNo, unsigned char *descriptor, short scriptlen);

/**********************************************************************************************
12 HD_GetHardwareInf
��Ӳ����Ϣ
   
HDIFD20B_API unsigned short CCONV HD_GetHardwareInf(HANDLE devNo, PHARD_WARE_INF phard_ware_inf, short ivCardSeat);
����˵��
devNo			��HD_OpenPort���ص��豸��
phard_ware_inf	Ӳ����Ϣָ��.

����ֵ
��������0x9000��ʾ��ȷ����ʱphard_ware_inf��Ч������ֵ��ʾ������������Ϣ������ġ�������Ϣ��������ʱphard_ware_inf������Ч��
***********************************************************************************************/

HDIFD20B_API unsigned short CCONV HD_GetHardwareInf(HANDLE devNo, PHARD_WARE_INF phard_ware_inf, short ivCardSeat);


HDIFD20B_API unsigned short CCONV HD_GetConfigDes(HANDLE devNo, unsigned char *descriptor, short *descriptorLen,short ivCardSeat);

HDIFD20B_API unsigned short CCONV HD_OpenPath(LPCTSTR DPath,HANDLE *devNo);

HDIFD20B_API unsigned short CCONV HD_GetDeviceName( HANDLE devNo, LPTSTR lpDeviceNameBuffer, DWORD nBufferLength);

#ifdef __cplusplus
extern "C" {
#endif
HDIFD20B_API BOOL CCONV HD_RegisterCardNotification(
	HANDLE hRecipient,
	DWORD Flags
);

HDIFD20B_API void CCONV HD_UnregisterCardNotification();

HDIFD20B_API void CCONV HD_CardChange( 
	UINT nEventType, 
	DWORD dwData,
	void pCardAdd( UINT nEventType, DWORD dwData),
	void pCardRemove( UINT nEventType, DWORD dwData)
);

HDIFD20B_API BOOL CCONV HD_IsHDDevice( LPCTSTR szDeviceName);

#ifdef __cplusplus
}
#endif

HDIFD20B_API PREADER_OPEN_STRUCT * CCONV HD_GetReaderList();

HDIFD20B_API void CCONV HD_DestroyReaderList( PREADER_OPEN_STRUCT * ppros);

HDIFD20B_API unsigned short CCONV HD_OpenReader(PREADER_OPEN_STRUCT pros, HANDLE *devNo, int nBaud = 9600, int nParity = 0);

#endif