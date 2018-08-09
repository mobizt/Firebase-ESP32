
#include <WiFi.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST "YOUR_DATABASE.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_AUTHEN"
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOU_WIFI_PASSWORD"

void setup() {

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.stream("/temp/ch01/sensorValue", streamCallback);

}

void loop() {
  double temp = randomDouble(25.0, 40.0);
  Serial.println("Set float value: " + String(temp) + " to /temp/ch01/sensorValue");
  Firebase.setFloat("/temp/ch01/sensorValue", temp );
  delay(1000);
}

void streamCallback(streamResult event) {
  String eventType = event.eventType();
  eventType.toLowerCase();
  if (eventType == "put") {
    Serial.println("The stream event path: " + event.path() + ", value: " + String(event.getFloat()));
    Serial.println();
  }

}

double randomDouble(double minf, double maxf)
{
  return minf + random(1UL << 31) * (maxf - minf) / (1UL << 31);  // use 1ULL<<63 for max double values)
}
