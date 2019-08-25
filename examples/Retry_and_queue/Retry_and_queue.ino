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


//This example shows how error retry and queues work.


#include <WiFi.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

//Define Firebase Data object
FirebaseData firebaseData;

String path = "/ESP32_Test";

std::vector<uint8_t> myblob;
double mydouble = 0;

uint32_t queueID[20];
uint8_t qIdx = 0;



void callback (QueueInfo queueinfo){

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

  //Open and retore Firebase Error Queues from file.
  if (Firebase.errorQueueCount(firebaseData, "/test.txt", StorageType::SPIFFS) > 0)
  {
    Firebase.restoreErrorQueue(firebaseData, "/test.txt", StorageType::SPIFFS);
    Firebase.deleteStorageFile("/test.txt", StorageType::SPIFFS);
  }

  //Set maximum Firebase read/store retry operation (0 - 255) in case of network problems and buffer overflow
  Firebase.setMaxRetry(firebaseData, 3);

  //Set the maximum Firebase Error Queues in collection (0 - 255).
  //Firebase read/store operation causes by network problems and buffer overflow will be added to Firebase Error Queues collection.
  Firebase.setMaxErrorQueue(firebaseData, 10);

  
  Firebase.beginAutoRunErrorQueue(firebaseData, callback);

  //Firebase.beginAutoRunErrorQueue(firebaseData);


  Serial.println("------------------------------------");
  Serial.println("Set BLOB data test...");

  //Create demo data
  uint8_t data[256];
  for (int i = 0; i < 256; i++)
    data[i] = i;

  //Set binary data to database
  if (Firebase.setBlob(firebaseData, path + "/Binary/Blob/data", data, sizeof(data)))
  {
    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    if (Firebase.getErrorQueueID(firebaseData) > 0)
    {
      Serial.println("Error Queue ID: " + String(Firebase.getErrorQueueID(firebaseData)));
      queueID[qIdx] = Firebase.getErrorQueueID(firebaseData);
      qIdx++;
    }
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("--------------------------------------------------------------------------");
    Serial.println("To test error queue, turn off WiFi AP to make error in the next operation");
    Serial.println("--------------------------------------------------------------------------");
    Serial.println();

    delay(10000);
  }

  Serial.println("------------------------------------");
  Serial.println("Get BLOB data test...");

  //Get binary data from database
  //Assign myblob as the target variable
  if (Firebase.getBlob(firebaseData, path + "/Binary/Blob/data", myblob))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.print("VALUE: ");
    if (firebaseData.dataType() == "int")
      Serial.println(firebaseData.intData());
    else if (firebaseData.dataType() == "float")
      Serial.println(firebaseData.floatData(), 5);
    else if (firebaseData.dataType() == "double")
      printf("%.9lf\n", firebaseData.doubleData());
    else if (firebaseData.dataType() == "boolean")
      Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
    else if (firebaseData.dataType() == "string")
      Serial.println(firebaseData.stringData());
    else if (firebaseData.dataType() == "json")
      Serial.println(firebaseData.jsonData());
    else if (firebaseData.dataType() == "blob")
    {

      Serial.println();

      for (int i = 0; i < myblob.size(); i++)
      {
        if (i > 0 && i % 16 == 0)
          Serial.println();

        if (i < 16)
          Serial.print("0");

        Serial.print(myblob[i], HEX);
        Serial.print(" ");
      }
      myblob.clear();
      Serial.println();
    }
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    if (Firebase.getErrorQueueID(firebaseData) > 0)
    {
      Serial.println("Error Queue ID: " + String(Firebase.getErrorQueueID(firebaseData)));
      queueID[qIdx] = Firebase.getErrorQueueID(firebaseData);
      qIdx++;
    }
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Set double test...");

  if (Firebase.setDouble(firebaseData, path + "/Double/Data", 340.123456789))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.print("VALUE: ");
    if (firebaseData.dataType() == "int")
      Serial.println(firebaseData.intData());
    else if (firebaseData.dataType() == "float")
      Serial.println(firebaseData.floatData(), 5);
    else if (firebaseData.dataType() == "double")
      printf("%.9lf\n", firebaseData.doubleData());
    else if (firebaseData.dataType() == "boolean")
      Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
    else if (firebaseData.dataType() == "string")
      Serial.println(firebaseData.stringData());
    else if (firebaseData.dataType() == "json")
      Serial.println(firebaseData.jsonData());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    if (Firebase.getErrorQueueID(firebaseData) > 0)
    {
      Serial.println("Error Queue ID: " + String(Firebase.getErrorQueueID(firebaseData)));
      queueID[qIdx] = Firebase.getErrorQueueID(firebaseData);
      qIdx++;
    }
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Get double test...");

  if (Firebase.getDouble(firebaseData, path + "/Double/Data", mydouble))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.print("VALUE: ");
    if (firebaseData.dataType() == "int")
      Serial.println(firebaseData.intData());
    else if (firebaseData.dataType() == "float")
      Serial.println(firebaseData.floatData(), 5);
    else if (firebaseData.dataType() == "double")
      printf("%.9lf\n", firebaseData.doubleData());
    else if (firebaseData.dataType() == "boolean")
      Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
    else if (firebaseData.dataType() == "string")
      Serial.println(firebaseData.stringData());
    else if (firebaseData.dataType() == "json")
      Serial.println(firebaseData.jsonData());
    Serial.println("------------------------------------");
    Serial.println();
    mydouble = 0;
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    if (Firebase.getErrorQueueID(firebaseData) > 0)
    {
      Serial.println("Error Queue ID: " + String(Firebase.getErrorQueueID(firebaseData)));
      queueID[qIdx] = Firebase.getErrorQueueID(firebaseData);
      qIdx++;
    }
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (Firebase.errorQueueCount(firebaseData) > 0)
  {
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("Now turn on WiFi hotspot or router to process these queues");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println();

    //Save Error Queues to file
    Firebase.saveErrorQueue(firebaseData, "/test.txt", StorageType::SPIFFS);
  }

  //Stop error queue auto run process
  //Firebase.endAutoRunErrorQueue(firebaseData);
}

void loop()
{
  if (Firebase.errorQueueCount(firebaseData) > 0)
  {

   

    /*

    if Firebase.beginAutoRunErrorQueue was not call,
    to manaul run the Firebase Error Queues, just call Firebase.processErrorQueue in loop
    
    
    Firebase.processErrorQueue(firebaseData);

    delay(1000);

    if (Firebase.isErrorQueueFull(firebaseData))
    {
      Serial.println("Queue is full");
    }

    Serial.print("Remaining queues: ");
    Serial.println(Firebase.errorQueueCount(firebaseData));

    for (uint8_t i = 0; i < qIdx; i++)
    {
      Serial.print("Error Queue ");
      Serial.print(queueID[i]);
      if (Firebase.isErrorQueueExisted(firebaseData, queueID[i]))
        Serial.println(" is queuing");
      else
        Serial.println(" is done");
    }
    Serial.println();

    */

    if (mydouble > 0)
    {
      Serial.println("------------------------------------");
      Serial.println("Double Data get from Queue");
      Serial.println(mydouble, 9);
      Serial.println();
      mydouble = 0;
    }

    if (myblob.size() > 0)
    {
      Serial.println("------------------------------------");
      Serial.println("Blob Data get from Queue");
      Serial.println();
      for (int i = 0; i < myblob.size(); i++)
      {
        if (i > 0 && i % 16 == 0)
          Serial.println();
        if (myblob[i] < 16)
          Serial.print("0");
        Serial.print(myblob[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      Serial.println();
      myblob.clear();
    }
  }
}
