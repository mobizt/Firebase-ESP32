
/*
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2020 mobizt
 * 
 *
*/

//This example shows how to dynamic allocated the Firebase Data object and deallocated it for release the memory when no further use.

#include <WiFi.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Without http:// or https:// schemes
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

//Define FirebaseESP8266 data object
FirebaseData *firebaseData1 = new FirebaseData();
FirebaseData *firebaseData2 = new FirebaseData();

unsigned long sendDataPrevMillis = 0;

String path = "/Test/Stream";

int count = 0;

bool deallocated = false;

void printResult(FirebaseData &data);
void printResult(StreamData &data);

void streamCallback(StreamData data)
{

    Serial.println("Stream Data1 available...");
    Serial.println("STREAM PATH: " + data.streamPath());
    Serial.println("EVENT PATH: " + data.dataPath());
    Serial.println("DATA TYPE: " + data.dataType());
    Serial.println("EVENT TYPE: " + data.eventType());
    Serial.print("VALUE: ");
    printResult(data);
    Serial.println();
}

void streamTimeoutCallback(bool timeout)
{
    if (timeout)
    {
        Serial.println();
        Serial.println("Stream timeout, resume streaming...");
        Serial.println();
    }
}

void setup()
{

    Serial.begin(115200);

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

    if (!Firebase.beginStream(*firebaseData1, path))
    {
        Serial.println("------------------------------------");
        Serial.println("Can't begin stream connection...");
        Serial.println("REASON: " + firebaseData1->errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }

    Firebase.setStreamCallback(*firebaseData1, streamCallback, streamTimeoutCallback);
}

void loop()
{

    if (millis() - sendDataPrevMillis > 5000)
    {
        count++;
        sendDataPrevMillis = millis();

        if (count == 10)
        {
            deallocated = false;

            Serial.println("Reallocate the Firebase Data objects again after 10 times Firebasse call");

            firebaseData1 = new FirebaseData();
            firebaseData2 = new FirebaseData();

            if (!Firebase.beginStream(*firebaseData1, path))
            {
                Serial.println("------------------------------------");
                Serial.println("Can't begin stream connection...");
                Serial.println("REASON: " + firebaseData1->errorReason());
                Serial.println("------------------------------------");
                Serial.println();
            }

            Firebase.setStreamCallback(*firebaseData1, streamCallback, streamTimeoutCallback);

            count = 0;
        }

        if (!deallocated)
            Serial.print("Free Heap after allocated the Firebase Data objects: ");
        else
            Serial.print("Free Heap after deallocated the Firebase Data objects: ");

        Serial.println(ESP.getFreeHeap());

        delay(0);

        if (deallocated)
            return;

        Serial.println("------------------------------------");
        Serial.println("Set Int...");

        if (Firebase.set(*firebaseData2, path + "/Int", count))
        {
            Serial.println("PASSED");
            Serial.println("PATH: " + firebaseData2->dataPath());
            Serial.println("TYPE: " + firebaseData2->dataType());
            Serial.print("VALUE: ");
            printResult(*firebaseData2);
            Serial.println("------------------------------------");
            Serial.println();
        }
        else
        {
            Serial.println("FAILED");
            Serial.println("REASON: " + firebaseData2->errorReason());
            Serial.println("------------------------------------");
            Serial.println();
        }

        if (count == 5)
        {
            Serial.println("Deallocate the Firebase Data objects after 5 times Firebasse call");

            deallocated = true;

            //Need to stop the stream to prevent the readStream and callback to be called after objects deallocation
            Firebase.endStream(*firebaseData1);
            Firebase.removeStreamCallback(*firebaseData1);

            //Deallocate
            delete firebaseData1;
            delete firebaseData2;

            firebaseData1 = nullptr;
            firebaseData2 = nullptr;
        }
    }
}

//Helper function for printing the response/received data
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

//Helper function for printing the response/received stream data
void printResult(StreamData &data)
{

    if (data.dataType() == "int")
        Serial.println(data.intData());
    else if (data.dataType() == "float")
        Serial.println(data.floatData(), 5);
    else if (data.dataType() == "double")
        printf("%.9lf\n", data.doubleData());
    else if (data.dataType() == "boolean")
        Serial.println(data.boolData() == 1 ? "true" : "false");
    else if (data.dataType() == "string" || data.dataType() == "null")
        Serial.println(data.stringData());
    else if (data.dataType() == "json")
    {
        Serial.println();
        FirebaseJson *json = data.jsonObjectPtr();
        //Print all object data
        Serial.println("Pretty printed JSON data:");
        String jsonStr;
        json->toString(jsonStr, true);
        Serial.println(jsonStr);
        Serial.println();
        Serial.println("Iterate JSON data:");
        Serial.println();
        size_t len = json->iteratorBegin();
        String key, value = "";
        int type = 0;
        for (size_t i = 0; i < len; i++)
        {
            json->iteratorGet(i, type, key, value);
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
        json->iteratorEnd();
    }
    else if (data.dataType() == "array")
    {
        Serial.println();
        //get array data from FirebaseData using FirebaseJsonArray object
        FirebaseJsonArray *arr = data.jsonArrayPtr();
        //Print all array values
        Serial.println("Pretty printed Array:");
        String arrStr;
        arr->toString(arrStr, true);
        Serial.println(arrStr);
        Serial.println();
        Serial.println("Iterate array values:");
        Serial.println();

        for (size_t i = 0; i < arr->size(); i++)
        {
            Serial.print(i);
            Serial.print(", Value: ");

            FirebaseJsonData *jsonData = data.jsonDataPtr();
            //Get the result data from FirebaseJsonArray object
            arr->get(*jsonData, i);
            if (jsonData->typeNum == FirebaseJson::JSON_BOOL)
                Serial.println(jsonData->boolValue ? "true" : "false");
            else if (jsonData->typeNum == FirebaseJson::JSON_INT)
                Serial.println(jsonData->intValue);
            else if (jsonData->typeNum == FirebaseJson::JSON_FLOAT)
                Serial.println(jsonData->floatValue);
            else if (jsonData->typeNum == FirebaseJson::JSON_DOUBLE)
                printf("%.9lf\n", jsonData->doubleValue);
            else if (jsonData->typeNum == FirebaseJson::JSON_STRING ||
                     jsonData->typeNum == FirebaseJson::JSON_NULL ||
                     jsonData->typeNum == FirebaseJson::JSON_OBJECT ||
                     jsonData->typeNum == FirebaseJson::JSON_ARRAY)
                Serial.println(jsonData->stringValue);
        }
    }
}