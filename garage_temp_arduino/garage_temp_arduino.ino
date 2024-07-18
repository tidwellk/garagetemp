#include <DHT.h>
#include <DHT_U.h>

#include <WiFiS3.h>
#include "arduino_secrets.h"

int status = WL_IDLE_STATUS;

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

unsigned int serverPort = 2390;
unsigned int listenPort = 2391;
const char* serverIPstring = "192.168.69.164\0";

WiFiUDP myudp;


#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

char sendbuffer[32];

IPAddress serverIP(serverIPstring);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

    // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();


  dht.begin();
  myudp.begin(listenPort);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(5000);

  float hum = dht.readHumidity();

  float temp = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(temp) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float heatIndex = dht.computeHeatIndex(temp, hum);

  Serial.print("temp: ");
  Serial.print(temp);

  Serial.print("\t");
  Serial.print("humidity: ");
  Serial.print(hum);
  
  Serial.print("\t");
  Serial.print("heat index: ");
  Serial.println(heatIndex);

  if (myudp.beginPacket(serverIP, serverPort) == 0) {
    Serial.println("error in beginPacket()");
  }
  prepTemp(temp);
  myudp.write(sendbuffer, 5);
  myudp.write(' ');
  prepHumidity(hum);
  myudp.write(sendbuffer, 5);
  myudp.write(' ');
  prepHeatIndex(heatIndex);
  myudp.write(sendbuffer, 5);

  int sendstatus = myudp.endPacket();

  if (sendstatus == 0) {
    Serial.println("error in endPacket()");
  }
  // sendTemp(temp);

  // delay(1000);

  // sendHumidity(hum);
  
  // delay(1000);

  // sendHeatIndex(heatIndex);


  // Serial.print("serverIP: ");
  // Serial.println(serverIP);
  // Serial.print("port: ");
  // Serial.println(serverPort);
}

void prepTemp(double temp) {

  dtostrf(temp, 5, 2, sendbuffer);
  // sendbuffer[5] = 'f';
  sendbuffer[5] = 0;
  // sendbuffer[6] = 0;
  //Serial.println(sendbuffer);

}

void prepHumidity(double hum) {

  dtostrf(hum, 5, 2, sendbuffer);
  // sendbuffer[5] = 'h';
  // sendbuffer[6] = 0;
  sendbuffer[5] = 0;

}

void prepHeatIndex(double heatIndex) {

  dtostrf(heatIndex, 5, 2, sendbuffer);
  // sendbuffer[5] = 'i';
  // sendbuffer[6] = 0;
  sendbuffer[5] = 0;
  // Serial.println(sendbuffer);

}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

