#define THRESHOLD 200
#define THRESHOLD_TIME 5000 // Time in ms after threshold is broken, resets if threshold is broken again
#define MAG_QUEUE_SIZE 50 // Averages the amount of samples
#define MAG_SAMPLE_SIZE 50  // Find the max value out of samples
#define SLEEP false  // Sleep functionality
#define SERIAL_USE true  // Log to the serial port, screws up when the MCU sleeps
#define DEBUG false

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;

HTTPClient http;

double audioMax;
unsigned long startTime;
double threshold;
char ssid[] = "";        // your network SSID (name)

char server[] = "";
WiFiClient client;

double data;

void setup() {
  digitalWrite(LED_BUILTIN, HIGH);
  
  if (SERIAL_USE){
    //Initialize serial and wait for port to open:
    Serial.begin(115200);
    while (!Serial) {} // wait for serial port to connect. Needed for native USB port only
    //Serial.setDebugOutput(DEBUG);
    Serial.println("Serial Started");
  }


  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("", "");

  // allow reuse (if server supports it)
  http.setReuse(true);


}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    data = updateAverageMag();

    http.begin(String("") + "?project=mkrAudio&val1=" + data);
    http.end();

    if (SLEEP){
      digitalWrite(LED_BUILTIN, LOW);
      printSerial("Sleep Time");
      ESP.deepSleep(30e6);
    }
  }

  delay(500);

void printSerial(char message[]){
  if (SERIAL_USE){
    Serial.println(message);
  }
}

double updateAverageMag(){
  //float magQueue[MAG_QUEUE_SIZE];
  double averageMag;
  double i;
  double j;
  int val;
  
  averageMag = 0;
  for(i=0;i<MAG_QUEUE_SIZE;i++){

    //Get local maximum
    audioMax = 0;  
    for(j=0;j<MAG_SAMPLE_SIZE;j++){
      
        val = analogRead(0);
        if (audioMax < val)
        {
          audioMax = val;
        }
    }
    averageMag += audioMax;
  }

  averageMag /= MAG_QUEUE_SIZE;
  return averageMag;
}
