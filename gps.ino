#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <TinyGPS++.h>

static constexpr uint32_t GPSBaud = 9600;

TinyGPSPlus gps;

#define sclk 2
#define mosi 3
#define dc   4
#define cs   5
#define rst  6
Adafruit_SSD1351 tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst);

void setup()
{
//  Serial.begin(115200);
  Serial2.begin(GPSBaud);
  tft.begin();
  tft.fillScreen(0x0000);
  tft.setTextSize(1);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setTextWrap(true);
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (Serial2.available() > 0)
  {
    if (gps.encode(Serial2.read()))
      displayInfo();
  }
}

void displayInfo()
{
  tft.setCursor(0, 0);

  const auto nSats = gps.satellites.value();
  if (nSats == 0)
    tft.setTextColor(0xFFE0, 0x0000);
  else if (nSats >= 4)
    tft.setTextColor(0x07E0, 0x0000);
  else
    tft.setTextColor(0xFFFF, 0x0000);

  tft.print(F("# of sats: "));
  tft.println(nSats, 2);

  tft.print(F(" Location: ")); 
  if (gps.location.isValid())
  {
    tft.print(gps.location.lat(), 6);
    tft.print(F(","));
    tft.println(gps.location.lng(), 6);
  }
  else
  {
     tft.println(F("INVALID"));
  }

  if (gps.time.isValid())
  {
    tft.print(F(" Time: "));
    if (gps.time.hour() < 10) tft.print(F("0"));
    tft.print(gps.time.hour());
    tft.print(F(":"));
    if (gps.time.minute() < 10) tft.print(F("0"));
    tft.print(gps.time.minute());
    tft.print(F(":"));
    if (gps.time.second() < 10) tft.print(F("0"));
    tft.print(gps.time.second());
    tft.print(F("."));
    if (gps.time.centisecond() < 10) tft.print(F("0"));
    tft.println(gps.time.centisecond());
  }
  else
  {
    tft.println(F("INVALID"));
  }

  if (gps.speed.isValid())
  {
    tft.print(F(" "));
    tft.print(gps.speed.kmph());
    tft.println(F(" kph"));
  }
}
