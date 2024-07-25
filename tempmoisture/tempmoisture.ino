#include <SPI.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN on Blue Pill
const uint64_t address = 0xF0F0F0F0E1LL;

#define ONE_WIRE_BUS 5

const int AirValue = 645;   // you need to replace this value with Value_1
const int WaterValue = 254; // you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent = 0;

float temperature;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

struct MyVariable
{
  byte soilmoisturepercent;
  byte temperature;
};
MyVariable variable;

void setup()
{
  Serial.begin(9600);
  radio.begin();                 // Starting the Wireless communication
  radio.openWritingPipe(address); // Setting the address where we will send the data
  radio.setPALevel(RF24_PA_MIN); // You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.stopListening();         // This sets the module as transmitter
  sensors.begin();
}

void loop()
{
  // Read soil moisture value
  soilMoistureValue = analogRead(A0); // put Sensor into soil
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  if (soilmoisturepercent >= 100)
  {
    soilmoisturepercent = 100;
  }
  else if (soilmoisturepercent <= 0)
  {
    soilmoisturepercent = 0;
  }

  // Read temperature
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  // Store values in the variable structure
  variable.soilmoisturepercent = soilmoisturepercent;
  variable.temperature = temperature;

  // Print values to the serial monitor
  Serial.print("Soil Moisture: ");
  Serial.print(variable.soilmoisturepercent);
  Serial.println("%");

  Serial.print("Temperature: ");
  Serial.print(variable.temperature);
  Serial.println("*C");

  Serial.println();
  radio.write(&variable, sizeof(MyVariable));

  Serial.println("Data Packet Sent");
  Serial.println("");
  delay(10000);
}
