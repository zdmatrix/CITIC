
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HDIFD20B_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HDIFD20B_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef HDIFD20B_EXPORTS
#define HDIFD20B_API __declspec(dllexport)//extern "C"
#else
#define HDIFD20B_API __declspec(dllimport)//extern "C"
#endif

#define CCONV _stdcall//WINAPI

//导出函数
/**********************************************************************************************
1.HD_ContextInitialize
连接设备。与读卡器通/USB KEY 通讯前必须先打开设备。
unsigned short HD_ContextInitialize(
	HANDLE *ContextNo			//返回的容器号

)

参数说明
ContextNo	返回的容器号

返回值
函数返回0x9000表示正确，此时devNo有效；其它值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_ContextInitialize(HANDLE *ContextNo);

/**********************************************************************************************
2.HD_ContextRelease
连接设备。与读卡器通/USB KEY 通讯前必须先打开设备。
unsigned short HD_ContextRelease(
	HANDLE ContextNo				//容器号
)

参数说明
ContextNo			容器号

返回值
函数返回0x9000表示正确，此时devNo有效；其它值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_ContextRelease(HANDLE ContextNo);

/**********************************************************************************************
3.HD_GetReaderList
连接设备。与读卡器通/USB KEY 通讯前必须先打开设备。
unsigned short HD_GetReaderList(
	HANDLE ContextNo,				//容器号
	unsigned long * ReaderNo,		//返回值，设备标识列表
	unsigned long * ReaderCount		//返回值，设备数量
)

参数说明
ContextNo		容器号
ReaderNo 		返回值，设备标识列表。	
ReaderCount		返回值，设备数量。

返回值
函数返回0x9000表示正确，此时devNo有效；其它值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_GetReaderList(HANDLE ContextNo, int *ResponseBuffer , int *ResponseLength);

/**********************************************************************************************
4.HD_OpenPort
连接设备。与读卡器通/USB KEY 通讯前必须先打开设备。
unsigned short HD_OpenPort(
	HANDLE			ContextNo,		//容器号
	short  			ivPortNo,		// 串口/USB 端口编号/PCSC设备编号
	unsigned long 	ivBaud,			// 通讯波特率	//USB接口不使用
	short  			ivParity,		// 校验方式		//USB接口不使用
	HANDLE 			*devNo				
)

参数说明
ContextNo		容器号
ivPortNo 		串口编号: Windows中可以是1，2，3或4。	
                USB 端口编号: 21, 22, 23, 24
				PCSC设备编号: 根据ReaderName对应选择
ivBaud			通讯波特率, 串口状态下有效;采用9600, 19200 , 38400, 115200 波特率通讯。
ivParity	    校验方式, 串口状态下有效;0无校验位(缺省);1奇校验;2偶校验;
devNo			返回的设备号

返回值
函数返回0x9000表示正确，此时devNo有效；其它值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/
//HDIFD20B_API unsigned short CCONV HD_OpenPort(HANDLE ContextNo,short ivPortNo,unsigned long ivBaud, short ivParity, HANDLE *devNo);
HDIFD20B_API unsigned short CCONV HD_OpenPort(short ivPortNo,unsigned long ivBaud, short ivParity, HANDLE *devNo);
HDIFD20B_API unsigned short CCONV HD_OpenPath(unsigned char* DPath,HANDLE *devNo);

/**********************************************************************************************
5  HD_ResetCard 
对智能卡进行上电复位，取得复位应答信息。

unsigned short HD_ResetCard(		
	HANDLE 			devNo,
	char			*srATR,			
    short			*srATRLen,
	short 			ivCardSeat	//USB接口不使用
)

参数说明
devNo			    由HD_OpenPort返回的设备号
srATR            	存放收到的数据
srATRLen			存放接收到的数据的长度
ivCardSeat			卡座号。1：主卡座；2:副卡座

返回值
函数返回0x9000表示正确，此时srATR数据有效；其余值表示出错，错误信息见附后的“错误信息表”，此时srATR数据无效。

注意事项
该函数结束后，卡的RST管脚为高电平，卡进入工作状态。卡返回的复位应答字节存入srATR变量中。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_ResetCard( HANDLE devNo, unsigned char	*srATR, short	*srATRLen, short ivCardSeat);
HDIFD20B_API unsigned short CCONV HD_ResetCardVB( HANDLE devNo, char	*srATR, short ivCardSeat);

/**********************************************************************************************
6  HD_ClosePort
关闭设备。该函数关闭由HD_OpenPort函数打开的设备。
unsigned short HD_ClosePort(
	HANDLE devNo
)

参数说明
devNo				由HD_OpenPort返回的设备号

返回值
函数返回0x9000表示正确； 其余值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/
//HDIFD20B_API unsigned short CCONV HD_ClosePort(short ivPort,HANDLE devNo);
HDIFD20B_API unsigned short CCONV HD_ClosePort(HANDLE devNo);

/**********************************************************************************************
7  HD_PowerOn
给卡上电。上电结束后，卡的Vcc管脚为5V,CLK管脚提供一个3.684MHz的稳定时钟，RST管脚为低电平，I/O管脚为高电平。

unsigned short  HD_PowerOn(
HANDLE 		devNo,
short  		ivCardSeat	//USB接口不使用
)

参数说明
devNo			    由HD_OpenPort返回的设备号
ivCardSeat		    卡座号。1：主卡座；2:副卡座

返回值
函数返回0x9000表示正确； 其余值表示出错，错误信息见附后的“错误信息表”。

注意事项
该函数结束后，卡处在复位状态，即RST管脚为低电平。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_PowerOn(HANDLE devNo, short ivCardSeat);

/**********************************************************************************************
8  HD_PowerOff
给卡断电。该函数成功完成后，卡的各管脚与读卡器电隔离。

unsigned short HD_PowerOff(
	HANDLE 		devNo,
	short 		ivCardSeat	//USB接口不使用
)

参数说明
devNo			由HD_OpenPort返回的设备号
ivCardSeat		卡座号。1：主卡座；2:副卡座

返回值
函数返回0x9000表示正确； 其余值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_PowerOff(	HANDLE 	devNo,short	ivCardSeat);

/**********************************************************************************************
3  HD_SetTimeout
设置超时时间。串口方式下有效。

unsigned short HD_SetTimeout(
    HANDLE 		devNo, 
	int 		ivTimeOut,	 // 时间，以ms为单位
	short 		type  		 // TimeOut类型
)

参数说明
devNo		 	由HD_OpenPort返回的设备号
ivTimeOut       时间, 以ms为单位
type			TimeOut类型(见宏定义)。
				type = 0 设置PC端口超时时间(等待读卡器的数据）
				type = 1 设置读卡器串口超时时间（等待PC的数据）
				type = 2 设置卡复位应答超时时间 (读卡器等待3B的时间）
				type = 3 设置卡响应超时时间 （等待卡第一个状态字节的时间）
				type = 4 设置卡字间距超时时间 （等待卡返回两个字节的时间）
				若ivTimeOut = 0, PC端口等待时间无限长。系统缺省值为1s。

返回值
函数返回0x9000表示正确； 其余值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_SetTimeout( HANDLE devNo,	int ivTimeOut, short type );

/**********************************************************************************************
9 HD_ProbeCard
探测卡座中是否有卡存在。

unsigned short HD_ProbeCard(
HANDLE 		devNo,
short 		ivCardSeat	//USB接口不使用
)		

参数说明
devNo			 由HD_OpenPort返回的设备号
ivCardSeat	 	 卡座号。1：主卡座；2:副卡座

返回值
函数返回0x0000值表示无卡插入，0x9000表示有卡；其余值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/
//HDIFD20B_API unsigned short CCONV HD_ProbeCard(HANDLE ContextNo, HANDLE devNo,short ivPortNo, short ivCardSeat);		
//HDIFD20B_API unsigned short CCONV HD_ProbeCard(HANDLE ContextNo, short ivPortNo, short ivCardSeat);		
HDIFD20B_API unsigned short CCONV HD_ProbeCard(HANDLE devNo,short	ivCardSeat);		

/**********************************************************************************************
10  HD_ApduT0
卡命令处理函数,该命令接收APDU Case1 Case2 Case3 Case4格式的指令。

unsigned short HD_ApduT0(				
    HANDLE 				devNo,
	char 				*apduData,
	short				srAPDULen,
	char 				*respData,
	short				*respDataLen,
	short 				ivCardSeat	//USB接口不使用
)

参数说明
devNo				由HD_OpenPort返回的设备号
apduData			APDU命令(字节串)用CLA、INS、P1、P2以及一个可变长度的条件体来表示，条件体的定义为Lc、Data（Lc个字节）和Le。
					根据Lc和Le值的不同，共有四种命令情况，如下表所示：
					情况	结构
					1	CLA INS P1 P2
					2	CLA INS P1 P2 Le
					3	CLA INS P1 P2 Lc Data
					4	CLA INS P1 P2 Lc Data Le
srAPDULen			存放发送的APDU数据的长度
respData			卡响应数据
respDataLen			卡响应数据的长度
ivCardSeat	 		卡座号

返回值
函数返回0xffXX表示命令未成功执行，错误信息见附后的“错误信息表”；其余值为卡的响应。
当操作Case1和 Case2时，respData无意义。当操作Case3和 Case4时，函数返回0x9000，respData有意义；其余响应，respData无意义。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_ApduT0( HANDLE devNo,  unsigned char *apduData, short srAPDULen, unsigned char *respData, short *respDataLen, short ivCardSeat );
HDIFD20B_API unsigned short CCONV HD_ApduVB( HANDLE devNo, char *apduData, char *respData, short ivCardSeat );

/**********************************************************************************************
11 HD_GetDescriptor
获取读卡设备描述
   
unsigned short HD_GetDescriptor(		
	HANDLE 		devNo,		
	char 		*descriptor
)

参数说明
devNo			由HD_OpenPort返回的设备号
descriptor		读卡设备信息,对于CI-4型双卡座读卡器，该信息意义为“CIDC CI-4-1.0-009600“(十六进制串)
				对于USBKey，硬件序列号是对CPU IC卡做FORMAT时写入卡内的历史字节

返回值
函数返回0x9000表示正确，此时descriptor有效。其余值表示出错，错误信息见附后的“错误信息表”，此时descriptor数据无效。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_GetDescriptor( HANDLE devNo, char *descriptor);

HDIFD20B_API unsigned short CCONV HD_GetProtocolType(short *ProtocolType);
/**********************************************************************************************
3.HD_GetReaderNames
连接设备。与读卡器通/USB KEY 通讯前必须先打开设备。
unsigned short HD_GetReaderNames(
	HANDLE ContextNo,				//容器号
	unsigned long * ReaderNo,		//返回值，设备标识列表
	ReaderList * readerinfo,		//返回值，设备名称
	unsigned long * ReaderCount		//返回值，设备数量
)
参数类型说明
struct ReaderList
{
	char ReaderName[50];		//读卡器名称
	unsigned long ReaderID;		//读卡器标识
} readerinfo;。

参数说明
ContextNo		容器号
ReaderNo 		返回值，设备标识列表。
readerinfo		返回值，设备信息。	
ReaderCount		返回值，设备数量。


返回值
函数返回0x9000表示正确，此时devNo有效；其它值表示出错，错误信息见附后的“错误信息表”。
***********************************************************************************************/
HDIFD20B_API unsigned short CCONV HD_GetReaderNames(char *ReaderNames, int *Count);

HDIFD20B_API unsigned short CCONV HD_OpenPort2(short ReaderID, unsigned long ivBaud, short ivParity, HANDLE *devNo);
void DebugMessage(const char * message);