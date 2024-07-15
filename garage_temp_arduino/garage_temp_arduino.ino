#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

char replybuffer[32];


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  dht.begin();

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
  Serial.println(temp);

}
