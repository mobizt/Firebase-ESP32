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

//This example shows how error retry and queues work.


#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_HOST "PROJECT_ID.firebaseio.com"

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "DATABASE_SECRET"

//Define Firebase Data object
FirebaseData fbdo;

String path = "/Test";

std::vector<uint8_t> myblob;
double mydouble = 0;

uint32_t queueID[20];
uint8_t qIdx = 0;

void printResult(FirebaseData &data);

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
  if (Firebase.errorQueueCount(fbdo, "/test.txt", StorageType::FLASH) > 0)
  {
    Firebase.restoreErrorQueue(fbdo, "/test.txt", StorageType::FLASH);
    Firebase.deleteStorageFile("/test.txt", StorageType::FLASH);
  }

  //Set maximum Firebase read/store retry operation (0 - 255) in case of network problems and buffer overflow
  Firebase.setMaxRetry(fbdo, 3);

  //Set the maximum Firebase Error Queues in collection (0 - 255).
  //Firebase read/store operation causes by network problems and buffer overflow will be added to Firebase Error Queues collection.
  Firebase.setMaxErrorQueue(fbdo, 10);

  
  Firebase.beginAutoRunErrorQueue(fbdo, callback);

  //Firebase.beginAutoRunErrorQueue(fbdo);


  Serial.println("------------------------------------");
  Serial.println("Set BLOB data test...");

  //Create demo data
  uint8_t data[256];
  for (int i = 0; i < 256; i++)
    data[i] = i;

  //Set binary data to database
  if (Firebase.setBlob(fbdo, path + "/Binary/Blob/data", data, sizeof(data)))
  {
    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    if (Firebase.getErrorQueueID(fbdo) > 0)
    {
      Serial.println("Error Queue ID: " + String(Firebase.getErrorQueueID(fbdo)));
      queueID[qIdx] = Firebase.getErrorQueueID(fbdo);
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
  if (Firebase.getBlob(fbdo, path + "/Binary/Blob/data", myblob))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    Serial.print("VALUE: ");
    if (fbdo.dataType() == "blob")
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
    Serial.println("REASON: " + fbdo.errorReason());
    if (Firebase.getErrorQueueID(fbdo) > 0)
    {
      Serial.println("Error Queue ID: " + String(Firebase.getErrorQueueID(fbdo)));
      queueID[qIdx] = Firebase.getErrorQueueID(fbdo);
      qIdx++;
    }
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Set double test...");

  if (Firebase.setDouble(fbdo, path + "/Double/Data", 340.123456789))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    Serial.print("VALUE: ");
    printResult(fbdo);
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    if (Firebase.getErrorQueueID(fbdo) > 0)
    {
      Serial.println("Error Queue ID: " + String(Firebase.getErrorQueueID(fbdo)));
      queueID[qIdx] = Firebase.getErrorQueueID(fbdo);
      qIdx++;
    }
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Get double test...");

  if (Firebase.getDouble(fbdo, path + "/Double/Data", mydouble))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    Serial.print("VALUE: ");
    printResult(fbdo);
    Serial.println("------------------------------------");
    Serial.println();
    mydouble = 0;
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    if (Firebase.getErrorQueueID(fbdo) > 0)
    {
      Serial.println("Error Queue ID: " + String(Firebase.getErrorQueueID(fbdo)));
      queueID[qIdx] = Firebase.getErrorQueueID(fbdo);
      qIdx++;
    }
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (Firebase.errorQueueCount(fbdo) > 0)
  {
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("Now turn on WiFi hotspot or router to process these queues");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println();

    //Save Error Queues to file
    Firebase.saveErrorQueue(fbdo, "/test.txt", StorageType::FLASH);
  }

  //Stop error queue auto run process
  //Firebase.endAutoRunErrorQueue(fbdo);
}

void loop()
{
  if (Firebase.errorQueueCount(fbdo) > 0)
  {

    /*

    if Firebase.beginAutoRunErrorQueue was not call,
    to manaul run the Firebase Error Queues, just call Firebase.processErrorQueue in loop
    
    
    Firebase.processErrorQueue(fbdo);

    delay(1000);

    if (Firebase.isErrorQueueFull(fbdo))
    {
      Serial.println("Queue is full");
    }

    Serial.print("Remaining queues: ");
    Serial.println(Firebase.errorQueueCount(fbdo));

    for (uint8_t i = 0; i < qIdx; i++)
    {
      Serial.print("Error Queue ");
      Serial.print(queueID[i]);
      if (Firebase.isErrorQueueExisted(fbdo, queueID[i]))
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
