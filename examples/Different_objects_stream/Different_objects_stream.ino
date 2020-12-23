/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2020 mobizt
 *
*/

//This example shows how to use different Firebase Data objects to handle to same streaming path, one for stream connection (database data changes monitoring) and
//other for store, read, update database operation.


#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_HOST "PROJECT_ID.firebaseio.com"

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "DATABASE_SECRET"

//Define Firebase Data objects
FirebaseData fbdo1;
FirebaseData fbdo2;

unsigned long sendDataPrevMillis1;

uint16_t count1;

String path = "";

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
  Serial.println("Begin stream 1...");
  if (!Firebase.beginStream(fbdo2, path + "/Stream/data1"))
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo2.errorReason());
    Serial.println();
  }
  else
  {
    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void loop()
{

  if (!Firebase.readStream(fbdo2))
  {
    Serial.println("Can't read stream data");
    Serial.println("REASON: " + fbdo2.errorReason());
    Serial.println();
  }

  if (fbdo2.streamTimeout())
  {
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }

  if (fbdo2.streamAvailable())
  {
    Serial.println("------------------------------------");
    Serial.println("Stream Data Available...");
    Serial.println("STREAM PATH: " + fbdo2.streamPath());
    Serial.println("EVENT PATH: " + fbdo2.dataPath());
    Serial.println("DATA TYPE: " + fbdo2.dataType());
    Serial.println("EVENT TYPE: " + fbdo2.eventType());
    Serial.print("VALUE: ");
    printResult(fbdo2);
    if (fbdo2.dataType() == "blob")
    {
      std::vector<uint8_t> blob = fbdo2.blobData();

      Serial.println();

      for (int i = 0; i < blob.size(); i++)
      {
        if (i > 0 && i % 16 == 0)
          Serial.println();

        if (i < 16)
          Serial.print("0");

        Serial.print(blob[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (millis() - sendDataPrevMillis1 > 15000)
  {
    sendDataPrevMillis1 = millis();

    //Create demo data
    uint8_t data[256];
    for (int i = 0; i < 256; i++)
      data[i] = i;
    data[255] = rand();

    Serial.println("------------------------------------");
    Serial.println("Set Blob Data 1...");
    if (Firebase.setBlob(fbdo1, path + "/Stream/data1", data, sizeof(data)))
    {
      Serial.println("PASSED");
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo1.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }

    FirebaseJson json;
    json.add("data1-1", count1).add("data1-2", count1 + 1).add("data1-3", count1 + 2);
    Serial.println("------------------------------------");
    Serial.println("Update Data 1...");
    if (Firebase.updateNode(fbdo1, path + "/Stream/data1", json))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo1.dataPath());
      Serial.println("TYPE: " + fbdo1.dataType());
      Serial.print("VALUE: ");
      printResult(fbdo1);
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo1.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }

    //Stop WiFi client will gain the free memory
    //This requires more time for the SSL handshake process in the next connection
    // due to the previous connection was completely stopped.
    fbdo1.stopWiFiClient();

    Serial.print("Free Heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.println();

    count1 += 3;
  }
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
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
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
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
        Serial.println(jsonData.floatValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
  else
  {
    Serial.println(data.payload());
  }
}
