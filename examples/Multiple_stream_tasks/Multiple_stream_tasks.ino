/*
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2019 mobizt
 * 
 * This example is for FirebaseESP32 Arduino library v 3.5.0 and later
 *
*/

//This example shows how to read stream on other task function.

/*

THIS EXAMPLE IS OBSOLETED AND NOT RECOMMENDED TO USE SINCE FIREBASE-ESP32 VERSION 3.0.0
DEU TO UNHANDLED TASKS AND NOT COMPATTIBLE WITH FIREBASE ERROR QUEUES.

*/

#include <WiFi.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#if CONFIG_FREERTOS_UNICORE
#define CPU_CORE 0
#else
#define CPU_CORE 1
#endif

#define STACK_SIZE_IN_WORD 16384

unsigned long sendPrevMillis = 0;
int sendCount = 0;

//Define Firebase Data objects
FirebaseData firebaseData1;
FirebaseData firebaseData2;

String path1 = "/Test/Data1";
String path2 = "/Test/Data2";

void runTask();
double randomDouble(double minf, double maxf);
void printResult(FirebaseData &data);

void setup()
{

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  Serial.println("------------------------------------");
  Serial.println("> Begin stream 1...");
  if (!Firebase.beginStream(firebaseData1, path1))
  {
    Serial.println("> FAILED");
    Serial.println("> REASON: " + firebaseData1.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("> PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println(">> Begin stream 2...");
  if (!Firebase.beginStream(firebaseData2, path2))
  {
    Serial.println(">> FAILED");
    Serial.println(">> REASON: " + firebaseData2.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println(">> PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }

  runTask();
}

void loop()
{

  //This example uses the same Firebase Data object to read/store data (get, set, update, push and delete) and stream.
  //This causes some delay (for start new SSL connection) for swiching between read/store and stream operation.
  //For no delay, see Different_objects_stream.ino example which uses different Firebase Data object for read/store and stream data.

  if (millis() - sendPrevMillis > 30000)
  {
    sendPrevMillis = millis();

    for (int i = 0; i < 3; i++)
    {

      FirebaseJson json1;
      FirebaseJson json2;
      double r1 = randomDouble(25.0, 40.0);
      double r2 = randomDouble(25.0, 40.0);

      json1.set("ID", "Node 1");
      json1.set("Count", sendCount + 1);
      json1.set("Temp", r1);
      json2.set("ID", "Node 2");
      json2.set("Count", sendCount + 1);
      json2.set("Temp", r2);

      sendCount++;

      Serial.println("------------------------------------");
      Serial.println("> Set Data 1...");

      if (Firebase.setJSON(firebaseData1, path1, json1))
      {
        Serial.println("> PASSED");
        Serial.println("> PATH: " + firebaseData1.dataPath());
        Serial.println("> TYPE: " + firebaseData1.dataType());
        Serial.print("> VALUE: ");
        printResult(firebaseData1);
        Serial.println("------------------------------------");
        Serial.println();
      }
      else
      {
        Serial.println("> FAILED");
        Serial.println("> REASON: " + firebaseData1.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }

      Serial.println("------------------------------------");
      Serial.println(">> Set Data 2...");

      if (Firebase.setJSON(firebaseData2, path2, json2))
      {
        Serial.println(">> PASSED");
        Serial.println(">> PATH: " + firebaseData2.dataPath());
        Serial.println(">> TYPE: " + firebaseData2.dataType());
        Serial.print(">> VALUE: ");
        printResult(firebaseData2);
        Serial.println("------------------------------------");
        Serial.println();
      }
      else
      {
        Serial.println(">> FAILED");
        Serial.println(">>REASON: " + firebaseData2.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
    }
  }
}

void runTask()
{

  xTaskCreatePinnedToCore([](void *param) {
    for (;;)
    {

      if (!Firebase.readStream(firebaseData1))
      {
        Serial.println("------------------------------------");
        Serial.println("> Read stream 1...");
        Serial.println("> FAILED");
        Serial.println("> REASON: " + firebaseData1.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
      if (firebaseData1.streamTimeout())
        Serial.println("> Stream timeout, resume streaming...");

      if (firebaseData1.streamAvailable())
      {
        Serial.println("> Stream Data1 available...");
        Serial.println("> STREAM PATH: " + firebaseData1.streamPath());
        Serial.println("> EVENT PATH: " + firebaseData1.dataPath());
        Serial.println("> DATA TYPE: " + firebaseData1.dataType());
        Serial.println("> EVENT TYPE: " + firebaseData1.eventType());
        Serial.print("> VALUE: ");
        printResult(firebaseData1);
        Serial.println();
      }
    }

    vTaskDelete(NULL);
  },
                          "streamTask1", STACK_SIZE_IN_WORD, NULL, 3, NULL, CPU_CORE);

  xTaskCreatePinnedToCore([](void *param) {
    for (;;)
    {

      if (!Firebase.readStream(firebaseData2))
      {
        Serial.println("------------------------------------");
        Serial.println(">> Read stream 2...");
        Serial.println(">> FAILED");
        Serial.println(">> REASON: " + firebaseData2.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }

      if (firebaseData2.streamTimeout())
        Serial.println(">> Stream timeout, resume streaming...");

      if (firebaseData2.streamAvailable())
      {
        Serial.println(">> Stream Data2 available...");
        Serial.println(">> STREAM PATH: " + firebaseData2.streamPath());
        Serial.println(">> EVENT PATH: " + firebaseData2.dataPath());
        Serial.println(">> DATA TYPE: " + firebaseData2.dataType());
        Serial.println(">> EVENT TYPE: " + firebaseData2.eventType());
        Serial.print(">> VALUE: ");
        printResult(firebaseData2);

        Serial.println();
      }
    }
    vTaskDelete(NULL);
  },
                          "streamTask2", STACK_SIZE_IN_WORD, NULL, 3, NULL, CPU_CORE);
}

double randomDouble(double minf, double maxf)
{
  return minf + random(1UL << 31) * (maxf - minf) / (1UL << 31);
}

void printResult(FirebaseData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == JSON_OBJECT ? "object" : "array");
      if (type == JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray &arr = data.jsonArray();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr.toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();
    for (size_t i = 0; i < arr.size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData &jsonData = data.jsonData();
      //Get the result data from FirebaseJsonArray object
      arr.get(jsonData, i);
      if (jsonData.typeNum == JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == JSON_STRING ||
               jsonData.typeNum == JSON_NULL ||
               jsonData.typeNum == JSON_OBJECT ||
               jsonData.typeNum == JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
}
