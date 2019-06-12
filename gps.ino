#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <DueTimer.h>

static constexpr uint32_t GPSBaud = 115200;

TinyGPSPlus gps;

#define sck 3
#define mosi 2
#define dc 4
#define cs 5
#define rst 6
Adafruit_SSD1351 tft(cs, dc, mosi, sck, rst);
//Adafruit_ST7735 tft(cs, dc, mosi, sck, rst);

// #define PRINT(...) Serial.print(__VA_ARGS__)
// #define PRINTLN(...) Serial.println(__VA_ARGS__)

#define PRINT(...) tft.print(__VA_ARGS__)
#define PRINTLN(...) tft.println(__VA_ARGS__)

static void blinkLED()
{
	static bool ledOn = false;
	ledOn = !ledOn;
	if (ledOn)
		PIOB->PIO_SODR = 1 << 27; //lights the LED
	else
		PIOB->PIO_CODR = 1 << 27; //clears it
}

void setup()
{
	Serial.begin(115200);
	Serial2.begin(GPSBaud);

	// tft.initR(INITR_144GREENTAB);
	tft.begin();
	tft.fillScreen(0x0000);
	tft.setTextSize(1);
	tft.setTextColor(0xFFFF, 0x0000);
	tft.setTextWrap(true);
	tft.print("Screen test");

	pinMode(13, OUTPUT);

	Timer3.attachInterrupt(&blinkLED);
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

	tft.setCursor(0, 0);

	const auto nSats = gps.satellites.value();
	if (nSats != nSatsPrevious)
	{
		if (nSats == 0)
		{
			tft.setTextColor(0xFFE0, 0x0000);
			Timer3.stop();
			Timer3.setFrequency(20.0);
			Timer3.start();
		}
		else if (nSats >= 4)
		{
			tft.setTextColor(0x07E0, 0x0000);
			Timer3.stop();
			Timer3.setFrequency(7.0);
			Timer3.start();
		}
		else
		{
			tft.setTextColor(0xFFFF, 0x0000);
			Timer3.stop();
			Timer3.setFrequency(2.0);
			Timer3.start();
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
