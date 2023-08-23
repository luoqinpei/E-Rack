#include <ESP8266WiFi.h> // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <DoubleResetDetect.h> // https://github.com/jenscski/DoubleResetDetect
#include <ESPAsyncTCP.h> // https://github.com/me-no-dev/ESPAsyncTCP
#define WEBSERVER_H
#include <ESPAsyncWebServer.h> // https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESP8266mDNS.h>
#include <Updater.h>
#define U_PART U_FS

//Constants
#define LED D4

// maximum number of seconds between resets that
// counts as a double reset 
#define DRD_TIMEOUT 2.0

// address to the block in the RTC user memory
// change it if it collides with another usage of the address block
#define DRD_ADDRESS 0x00

DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);

//Parameters
String AP_ID = "Temporary";
String AP_PW = "Temporary_";
const char* host = "ESPWiFi";

//Variables
const int enablePin = 0;                            // ESP8266 digital pin 0
const int stepPin = 4;                              // ESP8266 digital pin 4
const int dirPin = 5;                               // ESP8266 digital pin 5
bool tuneStatus = false;
String sliderValue = "2000";
int sliderRange = 4400;
const char* PARAM_INPUT = "value";

// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<body style='background-color:#77a6f7; color:white;'>
    <title>WiFi Stepper Motor Controller</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial;
            text-align: center;
            margin: 0px auto;
            padding-top: 30px;
        }
        .button {
            padding: 10px 20px;
            padding-left: 10%;
            padding-right: 10%;
            font-size: 24px;
            text-align: center;
            outline: none;
            color: #fff;
            background-color: #2f4468;
            border: none;
            border-radius: 5px;
            box-shadow: 0 6px #999;
            cursor: pointer;
            -webkit-touch-callout: none;
            -webkit-user-select: none;
            -khtml-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            user-select: none;
            -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
        }
        .button:hover {
            background-color: #1f2e45
        }
        .button:active {
            background-color: #1f2e45;
            box-shadow: 0 4px #666;
            transform: translateY(2px);
        }
        .slider {
            -webkit-appearance: none;
            margin: 0 auto;
            border-radius: 25px;
            width: 60%;
            height: 25px;
            background: #FFD65C;
            outline: none;
            -webkit-transition: .2s;
            transition: opacity .2s;
        }
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 65px;
            height: 35px;
            border-radius: 25%;
            background: #2f4468;
            cursor: pointer;
        }
        .slider::-webkit-slider-thumb:hover {
            background-color: #1f2e45
        }
        .slider::-moz-range-thumb {
            width: 65px;
            height: 35px;
            border-radius: 25%;
            background: #2f4468;
            cursor: pointer;
        }
        .slider::-moz-range-thumb:hover {
            background-color: #1f2e45
        }
    </style>
    </head>
    <body>
        <h2><center>WiFi Stepper Motor Controller</center></h2>
        <hr>
        <p id="tuneStatus">Connecting...</p>
        <p>slow<input type="range" direction="ltr" onchange="updateSpeedSlider(this)" id="speedSlider" min="0"
                max="4000" value="" step="1" class="slider">fast</p>
        <br>
        <button class="button" onmousedown="toggleCheckbox('scanToggle');" ontouchstart="toggleCheckbox('scanToggle');">
            SCAN </button>
        <br><br><br>
        <button class="button" onmousedown="toggleCheckbox('leftOn');" ontouchstart="toggleCheckbox('leftOn');"
            onmouseup="toggleCheckbox('leftOff');" ontouchend="toggleCheckbox('leftOff');"><<<<</button>      
        <button class="button" onmousedown="toggleCheckbox('rightOn');" ontouchstart="toggleCheckbox('rightOn');"
            onmouseup="toggleCheckbox('rightOff');" ontouchend="toggleCheckbox('rightOff');">>>>></button>
        <br><br><hr>
        <font size="1"><center>PKU EECS 2023</center></font>
        <script>
            function toggleCheckbox(x) {
                var xhr = new XMLHttpRequest();
                xhr.open("GET", "/" + x, true);
                xhr.send();
            }
            function updateSpeedSlider(element) {
                var sliderValue = document.getElementById("speedSlider").value;
                //document.getElementById("textSliderValue").innerHTML = sliderValue;
                console.log(sliderValue);
                var xhr = new XMLHttpRequest();
                xhr.open("GET", "/slider?value=" + sliderValue, true);
                xhr.send();
            }
            function checkAndUpdateStatus() {
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("tuneStatus").innerHTML =
                            this.responseText;
                    }
                };
                xhttp.open("GET", "/tuneStatus", true);
                xhttp.send();
            }
            setInterval(checkAndUpdateStatus, 75);
    
        </script>
    </body>
</html>)rawliteral";

AsyncWebServer server(80);//As input of the constructor, we will pass the port where the server will be listening. We will use port 80, which is the default HTTP port.
size_t content_len;

void handleRoot(AsyncWebServerRequest *request) {
  request->redirect("/update");
}

void handleUpdate(AsyncWebServerRequest *request) {
  const char* html = "<body style='background-color:#77a6f7; color:white; align=left; font-family: Arial;'><h3>Firmware Update:</h3><form method='POST' action='/fwUpdate' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form><font size='2'><a href='http://ESPWiFi.local'>Back</a></font></body><hr><font size='1'><center>Created by N6JJ - Community Version (v0.3.1-beta) 2023</center></font>";
  request->send(200, "text/html", html);
}

void handleFWUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index){
    Serial.println("Updating firmware...");
    content_len = request->contentLength();
    // if filename includes spiffs, update the spiffs partition
    int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;

    Update.runAsync(true);
    if (!Update.begin(content_len, cmd)) {
      Update.printError(Serial);
    }
    request->send(200, "text/html", "<head><meta http-equiv='refresh' content='10;URL=/'/></head><body style='background-color:#77a6f7; color:white; align=left; font-family: Arial;'>Uploaded! Rebooting, please wait...</body>");
  }

  if (Update.write(data, len) != len) {
    Update.printError(Serial);
    Serial.printf("Progress: %d%%\n", (Update.progress()*100)/Update.size());
  }

  if (final) {
    if (!Update.end(true)){
      Update.printError(Serial);
    } else {
      Serial.println("Firmware updated!");
      Serial.flush();
      ESP.restart();
    }
  }
}

void printProgress(size_t prg, size_t sz) {
  Serial.printf("Progress: %d%%\n", (prg*100)/content_len);
}

/*We specify this by calling the on method on the server object. As first input, this method receives a string with the path where it will be listening. 
We are going to set it to listen for requests on the “/” route.

As second parameter, it receives an enum of type WebRequestMethod, which allows to specify which type of HTTP request is allowed on that route. 
We will specify that we only want to receive HTTP GET requests, and thus we use the value HTTP_GET.

As third argument, it receives a function to which the signature is defined by the ArRequestHandlerFunction type.
So, this handling function we will specify has to return void and receives as parameter a pointer to an object of type AsyncWebServerRequest. 
Each incoming client will be wrapped in an object of this class and both live together until disconnection.
In order to keep the syntax compact, we will declare this handling function as a C++ lambda function(namely [captures](params){body}). Thus, we can specify a locally declared unnamed function. 
For servers with many routes, this is much cleaner and compact than having to declare a named function for each route. 
In our case, we will not use any captures, so we simply use empty square brackets []. For the params, we will need to respect the signature of the previously mentioned definition of the handling function,
which is specified by the ArRequestHandlerFunction type. Thus, our lambda will receive a parameter which is a pointer to an object of type AsyncWebServerRequest.*/
boolean fwPage() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {request->redirect("/update");});
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){handleUpdate(request);});
  server.on("/fwUpdate", HTTP_POST,
    [](AsyncWebServerRequest *request) {},
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
                  size_t len, bool final) {handleFWUpdate(request, filename, index, data, len, final);}
  );
  server.onNotFound([](AsyncWebServerRequest *request){request->send(404);});
  server.begin();
  return fwPage;
}

void setup() {
  //Init Serial USB
  Serial.begin(115200);
  Serial.println(F("Initialize System..."));

  // WiFiManager
  WiFiManager wm;
  wm.resetSettings();
  bool res;
  res = wm.autoConnect(AP_ID.c_str(),AP_PW.c_str()); // password protected access point
  
  if (drd.detect())
     {
         Serial.println("** Double reset boot **");
         WiFi.disconnect();
         wm.startConfigPortal(AP_ID.c_str(),AP_PW.c_str());
         delay(1000);
         ESP.restart();
     }
  
  if(!res) {
     Serial.println("Failed to connect");
  } 
  else {    
     Serial.println("Connected!");
  }
  
  // Declare pins as Outputs
  pinMode(stepPin, OUTPUT);                           // step pin
  pinMode(dirPin, OUTPUT);                            // direction pin
  pinMode(enablePin, OUTPUT);                         // enable pin
  pinMode(LED, OUTPUT);                               // onboard LED indicator
  digitalWrite(enablePin, HIGH);                      // start with driver disable
  
  // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html",index_html);
  });

/*
For our handling function implementation, we want to return to the client a text message. 
As said before, each client is associated with a AsyncWebServerRequest object, which has a method called send that allows us to specify the HTTP response to be returned.
This method receives as first input the HTTP response code, which will be 200 in our case. This is the HTTP response code for “OK”.
As second input, the send method receives the answer content-type of the response. We will use the value “text/plain“, since we simply want to return a text message.
Finally, as third argument, we will pass the actual content, which will be our text message, for example "ok".
Note that since we are working with a pointer to an object rather than the object itself we will need to use the arrow operator to call the send method on the AsyncWebServerRequest object.
Since our server is asynchronous, we will not need to call any client handling function on the main loop, as stated before. 
So, the route handling function we just defined will be asynchronously called and executed upon receiving requests from clients. The final code can be seen below.
*/

//Open http://serverIP/leftOff we can see "ok" printed. 
  // Button Left On
  server.on("/leftOn", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("Left On");
    digitalWrite(dirPin, HIGH);                         // set direction to one side
    tuneStatus = true;//motor on
    request->send(200, "text/plain", "ok");
  });

  // Button Left Off
  server.on("/leftOff", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("Left Off");
    tuneStatus = false;//motor off
    request->send(200, "text/plain", "ok");
  });

  // Button Right On
  server.on("/rightOn", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("Right On");
    digitalWrite(dirPin, LOW);                         // set direction to one side
    tuneStatus = true;
    request->send(200, "text/plain", "ok");
  });

  // Button Right Off
  server.on("/rightOff", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("Right Off");
    tuneStatus = false;
    request->send(200, "text/plain", "ok");
  });

  // Button Scan Toggle
  server.on("/scanToggle", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("Scan Toggle");
    if (tuneStatus == false) {
      tuneStatus = true;
    }
    else {
      tuneStatus = false;
    }
    Serial.println({tuneStatus});
    request->send(200, "text/plain", "ok");
  });

  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      sliderValue = inputMessage;
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  server.on("/tuneStatus", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String statusValue = "Idle";
    if(tuneStatus){
      statusValue = "Tuning...";
    }
    request->send(200, "text/plain", statusValue);
  });
  
  MDNS.begin(host);
  server.onNotFound(notFound);
  server.begin();
  fwPage();
  MDNS.addService("http", "tcp", 80);
  Serial.printf("The application is ready at http://%s.local\n", host);
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Hmm... something is wrong... :(");
}

void loop() {
  // -- Main Tuning Function --
  if (tuneStatus) {
    digitalWrite(LED, LOW);                           // turn LED indicator OFF
    digitalWrite(enablePin, LOW);
    digitalWrite(stepPin, HIGH);                           // perform a step
    delayMicroseconds(sliderRange-sliderValue.toInt());    // wait for the steps operation
    digitalWrite(stepPin, LOW);                            // stop the step 
    delayMicroseconds(sliderRange-sliderValue.toInt());    // wait between steps
  }
  else {
    digitalWrite(LED, HIGH);
    digitalWrite(enablePin, HIGH);
  }
  MDNS.update();
}