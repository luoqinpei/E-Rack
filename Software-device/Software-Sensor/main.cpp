/**
 ******************************************************************************
 * @file    main.cpp
 * @author  CLab
 * @version V1.0.0
 * @date    2-December-2016
 * @brief   Simple Example application for using the X_NUCLEO_IKS01A1 
 *          MEMS Inertial & Environmental Sensor Nucleo expansion board.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
*/ 

/* Includes */
#include "mbed.h"
#include "XNucleoIKS01A2.h"
#include "stdio.h"
#include "wifi.h"
#include "string.h"
/* Instantiate the expansion board */
static XNucleoIKS01A2 *mems_expansion_board = XNucleoIKS01A2::instance(D14, D15, D4, D5);

/* Retrieve the composing elements of the expansion board */
static HTS221Sensor *hum_temp = mems_expansion_board->ht_sensor;
static LPS22HBSensor *press_temp = mems_expansion_board->pt_sensor;
char dataA[20]="10";
char dataB[20]="50";
char dataC[20]="80";
char access_token[500];
char str[100];
float TEMPERATURE_C = 20;
float HUMIDITY = 50;
float PRESSURE = 1000;
int RAIN = 0;
//DigitalIn mypin(PA_2);
/* Simple main function */
int main() {
  uint8_t id;
  float value1, value2;
  
  /* Enable all sensors */
  hum_temp->enable();
  press_temp->enable();
 
  /*wifi_init*/
    pc1.baud(115200);  // set what you want here depending on your terminal program speed
    esp.baud(115200);   // change this to the new ESP8266 baudrate if it is changed at any time.    
    
    //ESPsetbaudrate();   //******************  include this routine to set a different ESP8266 baudrate  ******************
    ESPconfig();        //******************  include Config to set the ESP8266 configuration  ***********************
    
	  strcpy(snd, "AT+CIPSSLSIZE=4096\r\n");//ESP建立TCP/UDP连接:AT+CIPSTART
	  SendCMD();
    timeout=5;
    getreply();
    pc1.printf(buf);
    wait_ms(500);
    pc1.printf("\n---------- Build connection ----------\r\n");
	  strcpy(snd, "AT+CIPSTART=\"SSL\",\"api.weixin.qq.com\",443\r\n");//ESP建立TCP/UDP连接:AT+CIPSTART
	  SendCMD();
    timeout=5;
    getreply();
    pc1.printf(buf);
    wait_ms(500);
    pc1.printf("\n---------- Setting Mode ----------\r\n");
    strcpy(snd, "AT+CIPMODE=1\r\n");//设置AP+STATION模式
    SendCMD();
    timeout=2;
    getreply();
    pc1.printf(buf);
    wait(1);
    pc1.printf("\n----------AT+CIPSEND ----------\r\n");
    strcpy(snd, "AT+CIPSEND\r\n");//
    SendCMD();
    timeout=1;
    getreply();
    pc1.printf(buf);
    wait_ms(500);
    
  while(1) {;
	 // RAIN = mypin;
		printf("RAIN: %d \r\n ", RAIN);
		
    hum_temp->get_temperature(&value1);
    hum_temp->get_humidity(&value2);
    TEMPERATURE_C = value1;
    HUMIDITY = value2;
    printf("HTS221: [temp] %.2f C,   [hum] %.2f%%\r\n", value1,value2);
		
    press_temp->get_temperature(&value1);
    press_temp->get_pressure(&value2);
    PRESSURE = value2;
    printf("LPS22HB: [temp] %.2f C, [press] %.2f mbar\r\n", value1,value2);
	
    sprintf(dataA,"%f",TEMPERATURE_C);
    sprintf(dataB,"%f",HUMIDITY);
    sprintf(dataC,"%d",RAIN);
    //sprintf(str,"GET https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=wx00a3ccbefbfb3f58&secret=b9ec2d3bd59fce3fcb73e7c28b3b05ac HTTP/1.1\r\n\r\n\r\n");
		//strcpy(snd,str);
    //SendCMD();
		esp.printf("GET https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=wx00a3ccbefbfb3f58&secret=b9ec2d3bd59fce3fcb73e7c28b3b05ac HTTP/1.1\r\n\r\n\r\n");
		timeout=5;
    getreply();
    pc1.printf(buf);
		pc1.printf("\r\n");
		int s=0,e=0;
		for(int i=0;i<(sizeof(buf)-2);i++){
			if(buf[i]=='"'&&buf[i+1]==':'&&buf[i+2]=='"') s=i+3;
		}
		for(int i=0;i<(sizeof(buf)-2);i++){
			if(buf[i]=='"'&&buf[i+1]==','&&buf[i+2]=='"') e=i-1;
		}
		int cnt=0;
		for(int i=s;i<=e;i++){
		  access_token[cnt++]=buf[i];
		}
		//pc1.printf(access_token);
		pc1.printf("\r\n");
		
		char update[100]="{data:{";
		//update = "{{data:{{temp:{},humid:{},rain:{}}}}}".format(temp, humid, rain)
		strcat(update,"temp:");
		strcat(update,dataA);
		strcat(update,",humid:");
		strcat(update,dataB);
		strcat(update,",rain:");
		strcat(update,dataC);
		strcat(update,"}}");
		//pc1.printf(update);
		pc1.printf("\r\n\r\n");
		
		char query2[1000] = "{\"env\": \"erackpku-2gfdx8lja5f1d4eb\",\"query\": \"db.collection('current_sensor').doc('2b6383996458e95f001249976b12df2c').update(";
		strcat(query2,update);
		strcat(query2,")\"}");
		int cou=0;
		for(int i=0;i<1000;i++){
		   if(query2[i]!='\0') cou++;
		}
		sprintf(str,"POST https://api.weixin.qq.com/tcb/databaseupdate?access_token=%s HTTP/1.0\r\nAccept:*/*\r\nContent-Type: application/json\r\nContent-Length:%d\r\ncharset=UTF-8\r\n\r\n%s",access_token,cou,query2);
		//"POST https://api.weixin.qq.com/tcb/databaseupdate?access_token=%s HTTP/1.0\r\nAccept:*/*\r\nContent-Type: application/json\r\nContent-Length:%d\r\ncharset=UTF-8\r\n\r\n%s"
		pc1.printf(str);
	  pc1.printf("\r\n");
		strcpy(snd,str);
    SendCMD();
		timeout=5;
    getreply();
    pc1.printf(buf);
		pc1.printf("\r\n");
		//POST https://api.weixin.qq.com/tcb/databasequery?access_token={}
		//POST https://api.weixin.qq.com/tcb/databaseupdate?access_token={}
		//command_response2 = requests.post("https://api.weixin.qq.com/tcb/databaseupdate?access_token={}"
		//.format(access_token, query2),json=query2)
    //wait_ms(20);
    //strcpy(snd,"+++");
    //SendCMD();
    //timeout=2;
    //getreply();
    //pc1.printf(buf);
    //wait_ms(1000);
    wait(2);
  }
    pc1.printf("\n---------- Setting Mode ----------\r\n");
    strcpy(snd, "AT+CIPMODE=0\r\n");//设置AP+STATION模式
    SendCMD();
    timeout=1;
    getreply();
    pc1.printf(buf);
    wait(1);
    pc1.printf("\n---------- CLOSE TCP ----------\r\n");
    strcpy(snd, "AT+CIPCLOSE\r\n");//设置AP+STATION模式
    SendCMD();
    timeout=1;
    getreply();
    pc1.printf(buf);
}

