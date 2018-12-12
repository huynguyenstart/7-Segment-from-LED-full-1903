#include "FastLED.h"
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

SoftwareSerial EEBlue(4, 5); // RX | TX ( D4 | D5 )
int marker = 0;
char command;
String datain = "";
String string;
String aCmd;
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/
const int led7type1[10][21] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1}
};
const int led7type2[10][21] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};
int temp = 0;
int temp1 = 0;
int temp2 = 0;
int i;
CRGB LED7Color[4] = { CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue };
boolean systemStatus = true;

#define LED_FULL_BUS 3
#define NUM_LEDS 100
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<UCS1903, LED_FULL_BUS>(leds, NUM_LEDS);
  EEBlue.begin(38400); //Default Baud for HC-05
//  Serial.begin(115200);
}

void writeLed7(int index, int type, int offset, int value){
  for( i = 0 ; i < 21; i++ ){
    if( type == 1 ){
      if( led7type1[value][i] > 0 ){
        leds[offset + i] = LED7Color[index];
      } else {
        leds[offset + i] = CRGB::Black;
      }
    } else {
      if( led7type2[value][i] > 0 ){
        leds[offset + i] = LED7Color[index];
      } else {
        leds[offset + i] = CRGB::Black;
      }
    }
  }
}

void writeSpecial(int index, int offset, int len, int value){
  for( i = offset; i < offset + len; i++ ){
    if( value > 0 ){
      leds[i] = LED7Color[index];
    } else {
      leds[i] = CRGB::Black;
    }
  }
}

int systemOff(){
  int count = 0;
  for( i = 0; i < NUM_LEDS; i++ ){
    if( leds[i].r != 0 || leds[i].g != 0 || leds[i].b != 0 ){
      count++;
      leds[i] = CRGB::Black;
    }
  }
  return count;
}

void sendBack(){
  String strout = "<";
  // system status
  if( systemStatus ){
    strout = strout + "on:";
  } else {
    strout = strout + "of:";
  }
  // leds 7
  for( int i = 0; i < 4; i++ ){
    String sred = String(LED7Color[i].r, HEX);
    String sgreen = String(LED7Color[i].g, HEX);
    String sblue = String(LED7Color[i].b, HEX);
    if( sred.length() < 2 ) sred = "0" + sred;
    if( sgreen.length() < 2 ) sgreen = "0" + sgreen;
    if( sblue.length() < 2 ) sblue = "0" + sblue;
    strout = strout + sred + sgreen + sblue;
    if( i < 4 - 1 ) {
      strout = strout + ":";
    }
  }
  strout = strout + ">";
  char output[ strout.length() + 1 ];
  strout.toCharArray(output, strout.length() + 1 );
  EEBlue.write(output);
}

int hexvalue(String hexv){
  if( hexv == "0" ) return 0;
  if( hexv == "1" ) return 1;
  if( hexv == "2" ) return 2;
  if( hexv == "3" ) return 3;
  if( hexv == "4" ) return 4;
  if( hexv == "5" ) return 5;
  if( hexv == "6" ) return 6;
  if( hexv == "7" ) return 7;
  if( hexv == "8" ) return 8;
  if( hexv == "9" ) return 9;
  if( hexv == "A" ) return 10;
  if( hexv == "B" ) return 11;
  if( hexv == "C" ) return 12;
  if( hexv == "D" ) return 13;
  if( hexv == "E" ) return 14;
  if( hexv == "F" ) return 15;
  return 0;
}

int hex2int(String hex){
    return hexvalue(hex.substring(0, 1)) * 16 + hexvalue(hex.substring(1, 1) );
}

void loop() {
  if( systemStatus ){
    sensors.requestTemperatures(); // Send the command to get temperature readings
    /********************************************************************/
    temp = (int)sensors.getTempCByIndex(0);
    // Convert
    if(temp > 9){
      temp1 = temp/10;
      temp2 = temp%10;
    } else {
      temp1 = 0;
      temp2 = temp;
    }
    // write out
    writeLed7(0, 1, 0,  temp1);
    writeLed7(1, 2, 22, temp2);
    writeSpecial(2, 44, 12, 1);
    FastLED.show();
  } else {
    if( systemOff() > 0 ){
      FastLED.show();
    }
  }

  // read Bluetooth
  while(EEBlue.available() > 0)
  {
    string = "";
    while(EEBlue.available() > 0)
    {
      command = ((byte)EEBlue.read());
      if(command == '<')
      {
        marker = 1;
      }
      else if ( command == '>' ){
        marker = 0;
        break;
      }
      else if( marker == 1 )
      {
        string += command;
      }
      delay(1);
    }
    
    // Process command
    if( string == "read" ){
      sendBack();
    } else if ( string == "pow:on" ){
      systemStatus = true;
      sendBack();
    } else if ( string == "pow:of" ){
      systemStatus = false;
      sendBack();
    } else if ( string.length() > 11 && string.length() < 14 ){
      int sepos = 0;
      int LEDpos = 1;
      sepos = string.indexOf(":");
      aCmd = string.substring(0, sepos);
      if( aCmd == "col" ){
        // get LED7 position
        string = string.substring(sepos + 1);
        sepos = string.indexOf(":");
        aCmd = string.substring(0, sepos);
        LEDpos = aCmd.toInt();
        // get color
        aCmd = string.substring(sepos + 1);
  
        int red = hex2int( aCmd.substring(0, 2) ) ;
        int green = hex2int( aCmd.substring(2, 4) ) ;
        int blue = hex2int( aCmd.substring(4) ) ;
  
        LED7Color[LEDpos] = CRGB( red, green, blue );
        // sent back status
        sendBack();
      }
    }
  }
  
  // delay 0.25s
  delay(250);
}

