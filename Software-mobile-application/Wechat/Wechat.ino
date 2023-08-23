#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Arduino_JSON.h>

const char* ssid = "#define";
const char* password = "sunflower";

//Constants
#define LED D4

//Variables
const int enablePin = 0;                            // ESP8266 digital pin 0
const int stepPin = 4;                              // ESP8266 digital pin 4
const int dirPin = 5;                               // ESP8266 digital pin 5
bool tuneStatus = false;

int runOrStop=0;

String tokenNow;
String command_motor;

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  //Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

// Declare pins as Outputs
  pinMode(stepPin, OUTPUT);                           // step pin
  pinMode(dirPin, OUTPUT);                            // direction pin
  pinMode(enablePin, OUTPUT);                         // enable pin
  pinMode(LED, OUTPUT);                               // onboard LED indicator
  digitalWrite(enablePin, HIGH);                      // start with driver disable
  int old_state = 0;
  int count = 4;
  while(1)
  {
    Serial.print(old_state);
      // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    // Ignore SSL certificate validation
    client->setInsecure();
    
    //create an HTTPClient instance
    HTTPClient https;
    
    //Initializing an HTTPS communication using the secure client
    if (https.begin(*client, "https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=wx00a3ccbefbfb3f58&secret=b9ec2d3bd59fce3fcb73e7c28b3b05ac")) {  // HTTPS
      // start connection and send HTTP header
      int httpCode = https.GET();


      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          JSONVar myObject = JSON.parse(payload);


          JSONVar keys = myObject.keys();
    
          for (int i = 0; i < keys.length(); i++) {
            JSONVar value = myObject[keys[i]];

          }
          tokenNow=JSON.stringify(myObject[keys[0]]);
          
        }
      } else {
        //Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
    ///////////////////////////////////////////////////////////////////
    HTTPClient https1;
    //after getting token
    String websitePre="https://api.weixin.qq.com/tcb/databasequery?access_token=";
    char*website_char=(char*)tokenNow.c_str();

    int len,i,j;
    char website__[100];
    len=strlen(website_char);
    j=0;
    for(i=0;i<len;i++)
    {
    if(website_char[i]!='\"')
    {
    website__[j]=website_char[i];
    j++;
    }
    }
    website__[j]='\0';

        
    String website=websitePre+website__;


    //Serial.printf(website);
    if (https1.begin(*client, website)) {  // HTTPS
      // start connection and send HTTP header
      // httpCode will be negative on error
      https1.addHeader("Content-Type", "application/json");

      int httpCode1 = https1.POST("{\"env\": \"erackpku-2gfdx8lja5f1d4eb\",\"query\": \"db.collection('command').where({_id: 'rack_command',_openid: 'ogrCU5E-OscTqofOhvGATKvcmZSI'}).get()\"}");

      if (httpCode1 > 0) {
        // HTTP header has been send and Server response header has been handled
        // file found at server
        if (httpCode1 == HTTP_CODE_OK || httpCode1 == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload1 = https1.getString();

          JSONVar myObject1 = JSON.parse(payload1);


          JSONVar keys1 = myObject1.keys();
    
          for (int i = 0; i < keys1.length(); i++) {
            JSONVar value1 = myObject1[keys1[i]];

          }
          //String command_motor_old=command_motor;
          command_motor=JSON.stringify(myObject1[keys1[keys1.length()-1]]);
          char*command_char=(char*)command_motor.c_str();
          int length_command_char=strlen(command_char);

          Serial.print(int(command_char[length_command_char-6]-'0'));
          Serial.print(old_state);
          if(old_state != command_char[length_command_char-6]-'0')
          {
               tuneStatus=1;
               count = 0;
          }
          else
          {
           tuneStatus=0;
          }
          old_state = command_char[length_command_char-6]-'0';
          Serial.print(old_state);
          if(command_char[length_command_char-6]=='1')
          {
            digitalWrite(dirPin, HIGH);
          }
          else
          {
            digitalWrite(dirPin, LOW);
            
          }
        }
      } else {
   //     Serial.printf("[HTTPS1] GET... failed, error: %s\n", https1.errorToString(httpCode1).c_str());
      }

      https1.end();
    } else {
//      Serial.printf("[HTTPS1] Unable to connect\n");
    }
    ///////////////////////////////////////////////////////////////////
      https.end();
    } else {
  //    Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
 // Serial.println("Waiting 1min before the next round...");
  if (count <= 2) {
    for(int i=0;i<300;i++)
    {
    digitalWrite(LED, LOW);                           // turn LED indicator OFF
    digitalWrite(enablePin, LOW);
    digitalWrite(stepPin, HIGH);                           // perform a step
    delayMicroseconds(2000);    // wait for the steps operation
    digitalWrite(stepPin, LOW);                            // stop the step 
    delayMicroseconds(2000);    // wait between steps 
  }
  count = count + 1;
  }
   else {
    digitalWrite(LED, HIGH);
    digitalWrite(enablePin, HIGH);
  }
  }
}

////////////////////////////////////////////////////////
char* strip(char *s,char deleteis)
{
int len,i,j;
char a[100];
len=strlen(s);
j=0;
for(i=0;i<len;i++)
{
if(s[i]!=deleteis)
{
a[j]=s[i];
j++;
}
}
a[j]='\0';
return a;
}


void loop() {

}