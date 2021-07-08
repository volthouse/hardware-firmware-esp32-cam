#include <WiFi.h>
#include "file.h"
#include "camera.h"
#include "lapse.h"
#include "time.h"

const char *ssid = "...";
const char *password = "...";

void startCameraServer();

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void printLocalTime()
{
	struct tm timeinfo;
	if(!getLocalTime(&timeinfo)){
		Serial.println("Failed to obtain time");
		return;
	}

	Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
	Serial.begin(115200);
	Serial.setDebugOutput(true);
	Serial.println();
	initFileSystem();
	initCamera();

	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	startCameraServer();
	Serial.print("Camera Ready! Use 'http://");
	Serial.print(WiFi.localIP());
	Serial.println("' to connect");

	//init and get the time
  	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  	printLocalTime();	
}

void loop()
{
	static struct tm timeinfo;
	unsigned long t = millis();
	static unsigned long ot = 0;
	unsigned long dt = t - ot;
	ot = t;

	if( getLocalTime(&timeinfo) ) {		
		if( timeinfo.tm_hour >= 6 && timeinfo.tm_hour <= 18 ) {
			processLapse(dt);
			//Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
		}
	} else {
		Serial.println("Faild to optain time");
	}
}
