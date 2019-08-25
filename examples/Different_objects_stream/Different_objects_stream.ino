/*
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2019 mobizt
 *
*/

//This example shows how to use different Firebase Data objects to handle to same streaming path, one for stream connection (database data changes monitoring) and
//other for store, read, update database operation.


#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"

//Define Firebase Data objects
FirebaseData firebaseData1;
FirebaseData firebaseData2;

unsigned long sendDataPrevMillis1;

void printJsonObjectContent(FirebaseData &data);
void printJsonObjectContent2(StreamData &data);

uint16_t count1;

String json;

String path = "";

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println();
    Serial.println("Stream Data 1 timeout, resume streaming...");
    Serial.println();
  }
}

void streamCallback(StreamData data)
{
  Serial.println("Stream Data 1 available...");
  Serial.println("STREAM PATH: " + data.streamPath());
  Serial.println("EVENT PATH: " + data.dataPath());
  Serial.println("DATA TYPE: " + data.dataType());
  Serial.println("EVENT TYPE: " + data.eventType());
  Serial.print("VALUE: ");
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
    printJsonObjectContent2(data);
  else if (data.dataType() == "blob")
  {
    //See blob examples
  }
  Serial.println();
}

/*

void errorQueueCallback (QueueInfo queueinfo){

  if (queueinfo.isQueueFull())
  {
    Serial.println("Queue is full");
  }

  Serial.print("Remaining queues: ");
  Serial.println(queueinfo.totalQueues());

  Serial.print("Being processed queue ID: ");
  Serial.println(queueinfo.currentQueueID());  

  Serial.print("Data type:");
  Serial.println(queueinfo.dataType()); 

  Serial.print("Method: ");
  Serial.println(queueinfo.firebaseMethod());

  Serial.print("Path: ");
  Serial.println(queueinfo.dataPath());

  Serial.println();
}

*/

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

  //Firebase.setMaxRetry(firebaseData1, 3);
  //Firebase.setMaxErrorQueue(firebaseData1, 10);
  //Firebase.beginAutoRunErrorQueue(firebaseData1, errorQueueCallback);

  Serial.println("------------------------------------");
  Serial.println("Begin stream 1...");
  if (!Firebase.beginStream(firebaseData2, path + "/Stream/data1"))
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData2.errorReason());
    Serial.println();
  }
  else
  {
    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }

  //Firebase.setStreamCallback(firebaseData2, streamCallback);
  Firebase.setStreamCallback(firebaseData2, streamCallback, streamTimeoutCallback);
}

void loop()
{

  if (millis() - sendDataPrevMillis1 > 15000)
  {
    sendDataPrevMillis1 = millis();

    FirebaseJson json;
    json.addInt("data1-1",count1).addInt("data1-2",count1 + 1).addInt("data1-3",count1 + 2);

    Serial.println("------------------------------------");
    Serial.println("Update Data 1...");
    if (Firebase.updateNode(firebaseData1, path + "/Stream/data1", json))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData1.dataPath());
      Serial.println("TYPE: " + firebaseData1.dataType());
      Serial.print("VALUE: ");
      if (firebaseData1.dataType() == "int")
        Serial.println(firebaseData1.intData());
      else if (firebaseData1.dataType() == "float")
        Serial.println(firebaseData1.floatData(), 5);
      else if (firebaseData1.dataType() == "double")
        printf("%.9lf\n", firebaseData1.doubleData());
      else if (firebaseData1.dataType() == "boolean")
        Serial.println(firebaseData1.boolData() == 1 ? "true" : "false");
      else if (firebaseData1.dataType() == "string")
        Serial.println(firebaseData1.stringData());
      else if (firebaseData1.dataType() == "json")
        printJsonObjectContent(firebaseData1);
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData1.errorReason());
      /*
      if (Firebase.getErrorQueueID(firebaseData1) > 0)
      {
        Serial.println("Error Queue ID: " + String(Firebase.getErrorQueueID(firebaseData1)));
      }
      */
      Serial.println("------------------------------------");
      Serial.println();
    }

    count1+=3;
  }
}

void printJsonObjectContent(FirebaseData &data){
  size_t tokenCount = data.jsonObject().parse(false).getJsonObjectIteratorCount();
  String key;
  String value;
  FirebaseJsonObject jsonParseResult;
  Serial.println();
  for (size_t i = 0; i < tokenCount; i++)
  {
    data.jsonObject().jsonObjectiterator(i,key,value);
    jsonParseResult = data.jsonObject().parseResult();
    Serial.print("KEY: ");
    Serial.print(key);
    Serial.print(", ");
    Serial.print("VALUE: ");
    Serial.print(value); 
    Serial.print(", ");
    Serial.print("TYPE: ");
    Serial.println(jsonParseResult.type);        

  }
}

void printJsonObjectContent2(StreamData &data){
  size_t tokenCount = data.jsonObject().parse(false).getJsonObjectIteratorCount();
  String key;
  String value;
  FirebaseJsonObject jsonParseResult;
  Serial.println();
  for (size_t i = 0; i < tokenCount; i++)
  {
    data.jsonObject().jsonObjectiterator(i,key,value);
    jsonParseResult = data.jsonObject().parseResult();
    Serial.print("KEY: ");
    Serial.print(key);
    Serial.print(", ");
    Serial.print("VALUE: ");
    Serial.print(value); 
    Serial.print(", ");
    Serial.print("TYPE: ");
    Serial.println(jsonParseResult.type);        

  }
}
