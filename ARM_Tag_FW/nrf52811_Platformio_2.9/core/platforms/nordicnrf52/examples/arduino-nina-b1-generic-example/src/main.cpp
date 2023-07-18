/******************************************************************************

u-blox NINA-B1 Arduino example
Leonardo Bispo
March - 2019
https://github.com/ldab/Arduino-u-blox-NINA-B1

Distributed as-is; no warranty is given.

This example tries to implement some key functions and key PIN atributes in order
to get you started with using Arduino and the NRF52832 board u-blox NINA-B1

Timer functionas are implemented intead of delay() and the PINs have been re-mapped
on the #define section

******************************************************************************/

#include <Arduino.h>
#include "BLEPeripheral.h"
#include "Ticker.h"
#include "Adafruit_SHT31.h"

// Since the u-blox NINA-B1 board uses the generic Arduino variant, we need to reassing the pins;
// pins below are accordingly to the EVK-NINA-B1 board https://www.u-blox.com/en/product/evk-nina-b1

#define NINA_RX       5   // P0.05 - GPIO23
#define NINA_TX       6   // P0.06 - GPIO22
#define NINA_SDA      2   // P0.02 - GPIO24
#define NINA_SCL      3   // P0.03 - GPIO25
#define LED_DS1       7   // P0.07 - GPIO21
#define LED_DS2       31  // P0.31 - GPIO31
#define RGB_R         8   // P0.08 - GPIO1 SPI_CS1
#define RGB_G         16  // P0.16 - GPIO7 BOOT_I - *THIS PIN IS SHARED WITH SWITCH 1*
#define RGB_B         18  // P0.18 - GPIO8 SWO_I
#define B_SW1         16  // P.016 - GPIO7 - SWITCH 1 - *THIS PIN IS SHARED WITH RGB GREEN*
#define B_SW2         30  // P.030 - GPIO18 - SWITCH 2

// ********************************** BLE Stuff ******************************************************

const char * localName = "NINA-B1";
BLEPeripheral           blePeriph   = BLEPeripheral();

BLEService              tempSer     = BLEService("FFE0");
BLEIntCharacteristic    tempCharac  = BLEIntCharacteristic("FFE1", BLERead | BLENotify);
BLEDescriptor           tempDesc    = BLEDescriptor("2901", "Temperature");

BLEService              humSer      = BLEService("DDD0");
BLEFloatCharacteristic  humCharac   = BLEFloatCharacteristic("DDD1", BLERead | BLENotify);
BLEDescriptor           humDes      = BLEDescriptor("2901", "Humidity Percent");

BLEService              RGBSer      = BLEService("FFD0");
BLECharCharacteristic   redCharac   = BLECharCharacteristic("FFD1", BLERead | BLEWrite );
BLECharCharacteristic   greenCharac = BLECharCharacteristic("FFD2", BLERead | BLEWrite );

//****************************************************************************************************

// Initialize the Temperature and Humidity Sensor SHT31
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Create functions prior to calling them as .cpp files are differnt from Arduino .ino
void setupBLE   ( void );
void blinky     ( void );
void colorLED   ( void );
void readSensor ( void );
void GreenCharacWritten(BLECentral& central, BLECharacteristic& characteristic);
void RedCharacWritten(BLECentral& central, BLECharacteristic& characteristic);

// Create timers using Ticker library in oder to avoid delay()
Ticker blinkLED ( blinky, 600 );
Ticker readIT   ( readSensor, 5000 );

// Declare Global variables
float t = NAN;
float h = NAN;
bool  LED = false;
bool  LED_ACTIVE = false;

// Double clich switch 2 to turn RGB off variables
uint32_t  turnRGBoff  = 1000;
uint32_t  startLED    = 0;

// Put your code here to run once
void setup()
{
  // initialize LEDs pin as Outputs, Active LOW
  pinMode(LED_DS1, OUTPUT);
  pinMode(LED_DS2, OUTPUT);
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);

  digitalWrite(RGB_B, HIGH);
  digitalWrite(RGB_G, HIGH);
  digitalWrite(RGB_R, HIGH);

  // initialize Switch 2 as input, double click to turn RGB off
  pinMode(B_SW2, INPUT_PULLUP);
  //attachInterrupt(B_SW2, colorLED, CHANGE); // this conflicts with BLE lib https://github.com/sandeepmistry/arduino-BLEPeripheral/issues/205

  // Assign UART TX and RX pins accordingly
  Serial.setPins(NINA_RX, NINA_TX);
  Serial.begin(115200);

  // Assign I2C pins accordingly and start sensor
  Wire.setPins(NINA_SDA, NINA_SCL);
  if (! sht31.begin(0x44) )    // Set to 0x45 for alternate i2c addr
  {
    Serial.println("Couldn't find SHT31");
  }

  // Set-up BLE Service
  setupBLE();

  // Start the Timer function to avoid using delay()
  blinkLED.start();
  readIT.start();

}

void loop()
{
  // Keep feeding timers
  blinkLED.update();
  readIT.update();

  // Constantly polls BLE
  blePeriph.poll();

}

void setupBLE()
{
  // Advertise name
  blePeriph.setDeviceName(localName);
  blePeriph.setLocalName(localName);

  // Service for Temperature
  blePeriph.setAdvertisedServiceUuid(tempSer.uuid());

  blePeriph.addAttribute(tempSer);
  blePeriph.addAttribute(tempCharac);
  blePeriph.addAttribute(tempDesc);

  // Service for Humidity
  blePeriph.setAdvertisedServiceUuid(humSer.uuid());

  blePeriph.addAttribute(humSer);
  blePeriph.addAttribute(humCharac);
  blePeriph.addAttribute(humDes);

  // Service for RGB LED
  blePeriph.setAdvertisedServiceUuid(RGBSer.uuid());
  
  blePeriph.addAttribute(RGBSer);
  blePeriph.addAttribute(greenCharac);
  blePeriph.addAttribute(redCharac);

  // Assign event handlers for characteristic
  greenCharac.setEventHandler(BLEWritten, GreenCharacWritten);
  redCharac.setEventHandler(BLEWritten, RedCharacWritten);

  // One can also assign events for connection, etc...
  //blePeriph.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  //blePeriph.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  blePeriph.begin();

  Serial.println(F("BLE Temperature Sensor Peripheral"));
}

void colorLED( void )
{
    // Check Switch 2 state and light randonly;
  bool button2 = digitalRead(B_SW2);
  if( !button2 )
  {
    analogWrite( RGB_R, random(255) );
    analogWrite( RGB_G, random(255) );
    analogWrite( RGB_B, random(255) );
    
    if( millis() - startLED < turnRGBoff )
      startLED = 0;
    else
      startLED = millis();
  }
  else if( startLED == 0 )
  {
    analogWrite( RGB_R, 255 );
    analogWrite( RGB_G, 255 );
    analogWrite( RGB_B, 255 );
  }

}

void readSensor( void )
{
  t = sht31.readTemperature();
  h = sht31.readHumidity();

  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Temp ºC = "); Serial.println(t);
  } else { 
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(h);
  } else { 
    Serial.println("Failed to read humidity");
  }
  Serial.println();

  tempCharac.setValue(t);
  humCharac.setValue(h);
  
}

void blinky( void )
{
  // Toggle LED_DS1
  digitalWrite(LED_DS1, LED);

  // Toggle LED_DS2
  digitalWrite(LED_DS2, !LED);

  LED = !LED;
}

void GreenCharacWritten(BLECentral& central, BLECharacteristic& characteristic)
{
  if ( greenCharac.value() )
    analogWrite( RGB_G, 000 );
  else
    analogWrite( RGB_G, 255 );
}

void RedCharacWritten(BLECentral& central, BLECharacteristic& characteristic)
{
  if ( redCharac.value() )
    analogWrite( RGB_R, 000 ); 
  else
    analogWrite( RGB_R, 255 ); 
}
