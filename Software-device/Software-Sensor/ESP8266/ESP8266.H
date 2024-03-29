/*
//#include "includes.h"
//-------------------------
//AT+RST 重新启动
//-------------------------
uint8_t RST[8]={0x41,0x54,0x2B,0x52,0x53,0x54,0x0D,0x0A};

//-------------
//启动多路连接
//AT+CIPMUX=1
//-------------
uint8_t CIPMUX[13]={0x41,0x54,0x2B,0x43,0x49,0x50,0x4D,0x55,0x58,0x3D,0x31,'\0'};

//------------------------------------
//AT+ CIPSERVER=<mode>,<port>
//开启服务模式<1开启/0 停止>,<端口号>
//------------------------------------
char const CIPSERVER[21]={0x41,0x54,0x2B,0x43,0x49,0x50,0x53,0x45,
0x52,0x56,0x45,0x52,0x3D,0x31,0x2C,0x35,0x30,0x30,0x30,0x0D,0x0A};

//--------------------------------------------
//AT+CIPSEND=<id>,<length>（多路连接时）
//发送数据<需要用于连接的id号><数据长度<2049>
//--------------------------------------------
char const CIPSEND[11]={0x41,0x54,0x2B,0x43,0x49,0x50,0x53,0x45,0x4E,0x44,0x3D};
//-----------------------------------
//AP模式设置，在AP开启后有效
//AT+ CWSAP=<ssid>,<pwd>,<chl>,<ecn>
//<接入点名称><密码><通道号><密码允许>
//------------------------------------
char const CWSAP[40]={0x41,0x54,0x2B,0x43,0x57,
              0x53,0x41,0x50,0x3D,0x22,0x4E,0x55,0x43,
0x4C,0x45,0x4F,0x30,0X37,0X32,0x22,0x2C,0x22,0x30,0x31,0x32,0x33,0x34,0x35,
0x36,0x37,0x38,0x39,0x22,0x2C,0x31,0x31,0x2C,0x30,0x0D,0x0A};
//---------------------------
//配置WIFI模式
//AT+CWMODE=X;
//1.Station 无线网卡作为接受点模式		（接收）
//2.AP  基站模式，无线发射、路由模式	（发送）
//3.AP+Station  兼容模式
//---------------------------
char const CWMODE[13]={0x41,0x54,0x2B,0x43,0x57,0x4D,0x4F,0x44,0x45,0x3D,0x32,0x0D,0x0A};
*/
