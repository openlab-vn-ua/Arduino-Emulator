#include <chrono> 
#include <ctime>
#include <stdio.h>
#include <thread>
#include <chrono>

#include "Arduino.h"
#include "Common.h"
#include "PluggableUSB.h"
#include "RemoteSerial.h"
#include "Hardware.h"
#include "HardwareSetup.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "PluggableUSB.h"
#include "deprecated-avr-comp/avr/dtostrf.h"
#include "ArduinoLogger.h"
#include "ArdStdio.h"

// sleep ms milliseconds
void delay(unsigned long ms){
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));    
}

// sleep us milliseconds
void delayMicroseconds(unsigned int us){
    std::this_thread::sleep_for(std::chrono::microseconds(us));        
}

// double to string conversion -> we can use sprintf which is complete in linux
char *dtostrf(double val, signed char width, unsigned char prec, char *sout){
    sprintf(sout,"%*.*lf",width,prec, val);
    return sout;
}

// Returns the number of milliseconds passed since epich
unsigned long millis() {
    using namespace std::chrono;
    // Get current time with precision of milliseconds
    auto now = time_point_cast<milliseconds>(system_clock::now());
    // sys_milliseconds is type time_point<system_clock, milliseconds>
    using sys_milliseconds = decltype(now);
    // Convert time_point to signed integral type
    return now.time_since_epoch().count();

}

// Returns the micros of milliseconds passed since epich
unsigned long micros(void){
    using namespace std::chrono;
    // Get current time with precision of milliseconds
    auto now = time_point_cast<microseconds>(system_clock::now());
    // sys_milliseconds is type time_point<system_clock, milliseconds>
    using sys_milliseconds = decltype(now);
    // Convert time_point to signed integral type
    return now.time_since_epoch().count();
}


namespace arduino {

// Support for logging
ArduinoLogger Logger;

// Global Instances
StdioDevice Serial;
WiFiClient wifi;
HardwareImpl Hardware;
HardwareSetupImpl HardwareSetup;

WifiMock WiFi;

RemoteSerialImpl RemoteSerial(wifi,0);
RemoteSerialImpl RemoteSerial1(wifi,1);
RemoteSerialImpl RemoteSerial2(wifi,2);

//static PluggableUSB_ obj;
PluggableUSB_::PluggableUSB_(){}

    
}
