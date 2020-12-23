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

//This example shows how to construct queries to filter data.


#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_HOST "PROJECT_ID.firebaseio.com"

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "DATABASE_SECRET"

//Define the Firebase Data object
FirebaseData fbdo;

FirebaseJson json;

void printResult(FirebaseData &data);
void updateDatabaseRules(FirebaseData &dataObj);

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
  Serial.println("Push JSON test...");

  for (uint8_t i = 0; i < 30; i++)
  {
    json.set("Data1",i + 1);
    json.set("Data2",i + 100);

    //Also can use Firebase.push instead of Firebase.pushJSON
    //JSON string does not support in v 2.6.0 and later, only FirebaseJson object is supported.
    if (Firebase.pushJSON(fbdo, "/Test/Int", json))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.print("PUSH NAME: ");
      Serial.println(fbdo.pushName());
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }

  //Add an index to the node that being query.
  //Update the existing database rules by adding key "Test/Int/.indexOn" and value "Data2"
  //Check your database rules changes after running this function.
  updateDatabaseRules(fbdo);

  QueryFilter query;

  query.orderBy("Data2");
  query.startAt(105);
  query.endAt(120);
  query.limitToLast(8);

  
  //Begin the data filtering test
  Serial.println("------------------------------------");
  Serial.println("Data Filtering test...");

  if (Firebase.getJSON(fbdo, "/Test/Int", query))
  {

    Serial.println("PASSED");
    Serial.println("JSON DATA: ");
    printResult(fbdo);
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  //Clear all query parameters
  query.clear();
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
        json.toString(jsonStr,true);
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
        //get the array data from FirebaseData using FirebaseJsonArray object
        FirebaseJsonArray &arr = data.jsonArray();
        //Print all array values
        Serial.println("Pretty printed Array:");
        String arrStr;
        arr.toString(arrStr,true);
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

void loop()
{
}

void updateDatabaseRules(FirebaseData &dataObj)
{

  String path = "rules/Test/Int/.indexOn";
  //Read all database rules.
  if (Firebase.getRules(dataObj))
  {
    FirebaseJsonData jdata;
    FirebaseJson &json = dataObj.jsonObject();
    bool flag = false;
    bool update = false;
    
    //Check for the existent of the defined path 
    if (!json.get(jdata, path))
      flag = true;
    else if (jdata.stringValue != "Data2")
      flag = true;

    //If the defined path does not exist
    if (flag)
    {
      //Add new data to "rules/Test/Int/.indexOn"
      json.set(path, "Data2");
      update = true;
    }

    if (update)
    {
      //Update the database rules
      String rules = "";
      json.toString(rules, true);
      Firebase.setRules(dataObj, rules);
    }

    json.clear();
  }
 
}

