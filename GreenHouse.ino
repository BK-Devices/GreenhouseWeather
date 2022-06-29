#include <DHT.h>
#include <DHT_U.h>
#include <OneWire.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

#define ONE_WIRE_BUS 7
#define DHTPIN 8
#define DHTTYPE DHT11
// #define DHTTYPE DHT21
// #define DHTTYPE DHT22
#define RAINPIN 9

const double R2 = 10000;
const double m = -1.4; 

const int AirValue = 620;    // you need to replace this value with Value_1
const int WaterValue = 260;  // you need to replace this value with Value_2

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
double Temp = 0, Hum = 0, STemp = 0, Moi = 0, Lux = 0, Rain = 1;

LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(ONE_WIRE_BUS);	
DallasTemperature sensors(&oneWire);

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  sensors.begin();

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

  lcd.backlight();
  lcd.clear();

  pinMode(RAINPIN, INPUT_PULLUP);
}

void loop()
{
  lcd.setCursor(0, 0);
  lcd.print("   Greenhouse   ");
  lcd.setCursor(0, 1);
  lcd.print("Weather  Station");
  delay(2000);

  // Read Sensors
  soilMoi();
  TempHum();
  soilTemp();
  light_intensity();
  rainDetection();

  // Send and Display Data
  sendData();
  displayData();
  delay(1000);
}

double light_intensity()
{
  double rvolt = 5.0 / 1023 * analogRead(A2);
  double ldrresi = (5.0 - rvolt) / rvolt * 10000.0;
  Lux = 12518931 * pow(ldrresi, -1.405);
}
 
void TempHum()
{
  delay(delayMS);
  sensors_event_t event;
 
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) Temp = 0;
  else Temp = event.temperature;
 
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) Hum = 0;
  else Hum = event.relative_humidity;
}

void soilTemp()
{
  sensors.requestTemperatures();
  STemp = sensors.getTempCByIndex(0);
}

void soilMoi()
{
  int MoiVal = analogRead(A3);  // put Sensor insert into soil
  int MoiPer = map(MoiVal, AirValue, WaterValue, 0, 100);
  if(MoiPer >= 100) Moi = 100;
  else if(MoiPer <= 0) Moi = 0;
  else if(MoiPer >0 && MoiPer < 100) Moi = MoiPer;
}

void rainDetection()
{
  // Rain analog pin - A1
  // Digital pin D9
  Rain = digitalRead(RAINPIN);
}

void sendData()
{
  // send data to mobile application using bluetooth
  Serial.print(Temp);
  Serial.print("° C|");
  Serial.print(Hum);
  Serial.print(" %|");
  Serial.print(STemp);
  Serial.print("° C|");
  Serial.print(Moi);
  Serial.print(" %|");
  Serial.print(Lux);
  Serial.print(" Lux|");
  if(Rain == 0) Serial.print("Yes");
  else Serial.print("No");
}

void displayData()
{
  // Temperature
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Temperature   ");
  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.print(Temp);
  lcd.print((char)223);
  lcd.print(" C        ");
  delay(3000);

  // Humidity
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Humidity    ");
  lcd.setCursor(0, 1);
  lcd.print("     ");
  lcd.print(Hum);
  lcd.print(" %        ");
  delay(3000);

  // Soil Temperature
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Soil Temperature");
  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.print(STemp);
  lcd.print((char)223);
  lcd.print(" C         ");
  delay(3000);

  // Soil Moisture
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Soil Moisture  ");
  lcd.setCursor(0, 1);
  lcd.print("     ");
  lcd.print(Moi);
  lcd.print(" %         ");
  delay(3000);

  // Lux Level
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Light Intensity ");
  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.print(Lux);
  lcd.print(" lux      ");
  delay(3000);

  // Rain Detection
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Rain Detect   ");
  lcd.setCursor(0, 1);
  if(Rain == 0) lcd.print("      Yes       ");
  else lcd.print("       No       ");
  delay(3000);
}