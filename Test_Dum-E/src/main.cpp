#include <Arduino.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"


//#include "esp_camera.h"
//#include "servo.hpp"
#include "double_motor.hpp"

// Initialize WiFi parameters
const char *ssid = "Device-Northwestern";
const char *passphrase = NULL;

// Instantiate WebSocket client
WebSocketsClient webSocket;

//initialize motor class
Motors motors;

//task for parallel processing of moving the motors
void moveMotorsTask(void * parameters) {
    //ESP32Encoder::useInternalWeakPullResistors=UP;
    while(1) {
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;
        if (motors.new_command == "NORTH") {        //north
            while(motors.absDistanceToPosn(motors.base_enc, NORTH) > 7) {
                motors.goshortestWay(NORTH, motors.base_enc, motors.base);
            }
            motors.base.motor_speed_zero(); //stops motor
            motors.desired_base = NORTH;    //updates desired base to north
        }
        else if (motors.new_command == "EAST") {   //east
            while(motors.absDistanceToPosn(motors.base_enc, EAST) > 7) {
                motors.goshortestWay(EAST, motors.base_enc, motors.base);
            }
            motors.base.motor_speed_zero(); //stops motor
            motors.desired_base = EAST;     //updates desired base to east
        }    
        else if (motors.new_command == "SOUTH") {   //south
            while(motors.absDistanceToPosn(motors.base_enc, SOUTH) > 7) {
                motors.goshortestWay(SOUTH, motors.base_enc, motors.base);
            }
            motors.base.motor_speed_zero(); //stops motor
            motors.desired_base = SOUTH;    //updates desired base to east
        }    
        else if (motors.new_command == "WEST") {   //west
            while(motors.absDistanceToPosn(motors.base_enc, WEST) > 7) {
                motors.goshortestWay(WEST, motors.base_enc, motors.base);
            }
            motors.base.motor_speed_zero(); //stops motor
            motors.desired_base = WEST;     //updates desired base to west
        }
        else if (motors.new_command != motors.old_command) {    //if we received new command
            motors.decipherInput(motors.new_command);       //decipher input into two numbers
            motors.old_command = motors.new_command;    //stores new command into old
        }
        ///////base/////////
        if (motors.absDistanceToPosn(motors.base_enc, motors.desired_base) < 10) {   //if within a certain threshold
            motors.base.motor_speed_zero();   //stop the base
        }
        else {
            motors.goshortestWay(motors.desired_base, motors.base_enc, motors.base);    //goes shortest way for base
        }
        //Serial.print("Desired base: ");
        //Serial.print(motors.desired_base);
        //Serial.print(", Actual base: ");
        //Serial.println(motors.base_enc.getCount());
        ///////tilt///////////
        /*
        if (motors.absDistanceToPosn(motors.pan_enc, motors.desired_base) < 10) { //if within a certain threshold
            motors.pan.motor_speed_zero();    //stop the pan
        }
        else {
            motors.goshortestWay(motors.desired_pan, motors.pan_enc, motors.pan);   //goes shortest way for pan
        }
        */
    }
};

// Define WebSocket event handler
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            Serial.printf("[WSc] Connected to url: %s\n", payload);
            break;
        case WStype_TEXT:
            motors.new_command = (char*)payload;
            Serial.printf("[WSc] get text: %s\n", payload);
            break;
        case WStype_BIN:
        case WStype_ERROR:
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
            break;
    }
}


void setup() {
    // put your  setup code here, to run once:
    Serial.begin(115200);
    
    xTaskCreate(
        moveMotorsTask,  //function to imnplement the task
        "moveMotors",           //name of the task
        4096,                   //stack size
        NULL,                   //task input parameter
        0,                      //priority of the task
        NULL                   //task handle
    );    

    // Connect to WiFi
    WiFi.begin(ssid, passphrase);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // Connect WebSocket client to WebSocket server
    webSocket.begin("10.105.19.28", 8888, "/sentry");

    // Assign a callback function to the event handler
    webSocket.onEvent(webSocketEvent);

    // Set WebSocket reconnect interval
    webSocket.setReconnectInterval(3000);
}

void loop() {
    // put your main code here, to run repeatedly:

    webSocket.loop();

    // if (millis() - prevShot >= PERIOD)
    /*
    if (true) {
        // Take picture with camera
        
        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            esp_camera_fb_return(fb);
            return;
        }

        // Send picture through WebSocket
        webSocket.sendBIN((const uint8_t *)fb->buf, fb->len);
        esp_camera_fb_return(fb);
        

        // Print FPS as captured from the ESP32
        Serial.printf("%.2f\nlength: %d\n", 1000 / (double)(millis() - prevShot), fb->len);

        // Reset timer
        prevShot = millis();
        
    }
    */
}