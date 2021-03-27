#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Wire.h>
#include <TinyGPS++.h>

static constexpr uint32_t GPSBaud = 115200;

TinyGPSPlus gps;

#define sck 18
#define mosi 23
#define dc 27
#define cs 15
#define rst 4
//Adafruit_SSD1351 tft(cs, dc, mosi, sck, rst);
Adafruit_ST7735 tft(cs, dc, rst);

// #define PRINT(...) Serial.print(__VA_ARGS__)
// #define PRINTLN(...) Serial.println(__VA_ARGS__)

#define PRINT(...) tft.print(__VA_ARGS__)
#define PRINTLN(...) tft.println(__VA_ARGS__)

inline static constexpr uint16_t to565(uint8_t r, uint8_t g, uint8_t b) {
	return ((static_cast<uint16_t>(b & 0b11111000) << 8) | (static_cast<uint16_t>(g & 0b11111100) << 3) | (static_cast<uint16_t>(r) >> 3));
}

void setup()
{
	Serial.begin(115200);
	Serial2.begin(GPSBaud);

	tft.initR(INITR_MINI160x80);
	tft.setRotation(3);
	tft.fillScreen(0x0000);
	tft.setTextSize(2);
	tft.setTextColor(to565(0, 0, 0), to565(255, 0, 0));
	tft.setTextWrap(true);
	tft.print("No GPS info yet, waiting...");

	//pinMode(13, OUTPUT);
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
	static uint32_t nSatsPrevious = ~0u;

	tft.setTextSize(1);
	tft.setCursor(0, 0);

	const auto nSats = gps.satellites.value();
	if (nSats != nSatsPrevious)
	{
		if (nSats == 0)
		{
			tft.setTextColor(0xFFE0, 0x0000);
		}
		else if (nSats >= 4)
		{
			tft.setTextColor(0x07E0, 0x0000);
		}
		else
		{
			tft.setTextColor(0xFFFF, 0x0000);
		}

		nSatsPrevious = nSats;
	}

	PRINT(F("# of sats: "));
	PRINTLN(nSats);

	PRINT(F("Location: "));
	if (gps.location.isValid())
	{
		PRINT(gps.location.lat(), 6);
		PRINT(F(","));
		PRINTLN(gps.location.lng(), 6);
	}
	else
	{
		PRINTLN(F("INVALID   "));
	}

	if (gps.time.isValid())
	{
		PRINT(F(" Time: "));
		if (gps.time.hour() < 10)
			PRINT(F("0"));
		PRINT(gps.time.hour());
		PRINT(F(":"));
		if (gps.time.minute() < 10)
			PRINT(F("0"));
		PRINT(gps.time.minute());
		PRINT(F(":"));
		if (gps.time.second() < 10)
			PRINT(F("0"));
		PRINT(gps.time.second());
		PRINT(F("."));
		if (gps.time.centisecond() < 10)
			PRINT(F("0"));
		PRINTLN(gps.time.centisecond());
	}
	else
		PRINTLN(F("INVALID"));

	if (gps.speed.isValid())
	{
		PRINT(F(" "));
		PRINT(gps.speed.kmph());
		PRINTLN(F(" kph"));
	}
}
