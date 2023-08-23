#include "mbed.h"
#include "string.h"
#include "math.h"
#include "ESP8266.H"


Serial pc1(USBTX, USBRX);
Serial esp(PA_11, PA_12);
Timer t;
DigitalOut CH_PD(PC_3);

int  count,ended,timeout,countbyte,scountbyte;
char buf[4096];
char snd[255];
char AP[4096];
char pcstate[4096];
int a=80;

char ssid[32] = "sky";     // enter router ssid inside the quotes  ����������·������SSID
char pwd [32] = "12345678"; // enter router passwo h8rd inside the quotes  ����·����������

	
void SendCMD(),getreply(),ESPconfig(),change(),getstate();


//  +++++++change the data to send +++++++
void change() {
    for (int i = 0; i < countbyte ; i++) {
        }
    }
	
//  +++++++++++++++++++++++++++++++++ This is for ESP8266 config only, run this once to set up the ESP8266 +++++++++++++++
void ESPconfig()
{
    CH_PD=1;
    pc1.printf("---------- Starting ESP Config ----------\r\n\n");
    wait(2);
    pc1.printf("---------- Reset & get Firmware ----------\r\n");
    strcpy(snd,"AT+RST\r\n");
    SendCMD();//ESP8266���и�λ
    timeout=3;
    getreply();
    pc1.printf(buf);
    
    wait(1);   
   
    pc1.printf("\n---------- Get Version ----------\r\n");
    strcpy(snd,"AT+GMR\r\n");
    SendCMD();
    timeout=1;
    getreply();
    pc1.printf(buf);    
    
    wait(1);
        
    // set CWMODE to 1=Station,2=AP,3=BOTH, default mode 1 (Station)
    pc1.printf("\n---------- Setting Mode ----------\r\n");
    strcpy(snd, "AT+CWMODE=3\r\n");//����AP+STATIONģʽ
    SendCMD();
    timeout=1;
    getreply();
    pc1.printf(buf);

    
    wait(1);
    
    // set CIPMUX to 0=Single,1=Multi 
    pc1.printf("\n---------- Setting Connection Mode ----------\r\n");
    strcpy(snd, "AT+CIPMUX=0\r\n"); 
    SendCMD();
    timeout=1;
    getreply();
    pc1.printf(buf);
    
    wait(1);
    
    pc1.printf("\n---------- Listing Access Points ----------\r\n");
    strcpy(snd, "AT+CWLAP\r\n");
    SendCMD();
    timeout=3;
    getreply();
    pc1.printf(buf);
    
    wait(1);    
    
    pc1.printf("\n---------- Connecting to AP ----------\r\n");
    pc1.printf("ssid = %s   pwd = %s\r\n",ssid,pwd);
    strcpy(snd, "AT+CWJAP=\"");
    strcat(snd, ssid);
    strcat(snd, "\",\"");
    strcat(snd, pwd);
    strcat(snd, "\"\r\n");   
    SendCMD();
    timeout=5;
    getreply();    
    pc1.printf(buf);
    
    wait(5);
    
    pc1.printf("\n---------- Get IP's ----------\r\n"); 
    strcpy(snd, "AT+CIFSR\r\n");
    SendCMD();
    timeout=2;
    getreply(); 
    pc1.printf(buf);
    
    wait(1);
    
    pc1.printf("\n---------- Get Connection Status ----------\r\n"); 
    strcpy(snd, "AT+CIPSTATUS\r\n");
    SendCMD();
    timeout=3;
    getreply(); 
    pc1.printf(buf);   
    
    pc1.printf("\n\n\n  If you get a valid IP, ESP8266 has been set up.\r\n");
    pc1.printf("  Run this if you want to reconfig the ESP8266 at any time.\r\n");
		
		 pc1.printf("\n---------- Get IP's ----------\r\n"); 
        strcpy(snd, "AT+CIFSR\r\n");//snd<<AT+CIFSR\r\n
        SendCMD();
        timeout=2;
        getreply(); 
        pc1.printf(buf);
				

				
} 

void SendCMD()
{    
    esp.printf("%s", snd);    
} 

void getreply()
{    
    memset(buf, '\0', sizeof(buf));//buf��ʼ��
    t.start();
    ended=0;count=0;
    while(!ended) {
        if(esp.readable()) {
            buf[count] = esp.getc();count++;
            }
        if(t.read() > timeout) {
                ended = 1;t.stop();t.reset();
            }
        }   
}    