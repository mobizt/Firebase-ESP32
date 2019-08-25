
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

//This example shows how to set complex json data through FirebaseJson object then read the data back and parse them.

#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"


//Define Firebase Data object
FirebaseData firebaseData;

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

    String path = "/ESP8266_Test/Json";

    String jsonData = "";
    

    FirebaseJson json;
    FirebaseJson json2;
    FirebaseJson json3;
    FirebaseJson json4;
    FirebaseJsonArray jsonArr;
    FirebaseJsonArray jsonArr2;

    json2.addString("test1","hello world").addString("test2","nice to see you");

    jsonArr2.addInt(99).addBool(true).addDouble(190.24555673).addString("Firebase");


    jsonArr.addJson(&json2).addString("welcome").addArray(&jsonArr2);

    json3.addDouble("myVal",212.224).addString("myStr","good");

    json4.setJsonData("{\"simpleData\":\"this is string\"}");//Manual set raw json string.
   

    json.clear().addInt("data1",100).addArray("myArray",&jsonArr).addJson("anotherData",&json3).addJson("data2",&json4);



    Serial.println("------------------------------------");
    Serial.println("JSON Data");
    Serial.println(json.toString());
    Serial.println("------------------------------------");
    


   
    Serial.println("------------------------------------");
    Serial.println("Set JSON test...");

    if (Firebase.setJSON(firebaseData, path, json))
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
        Serial.println("------------------------------------");
        Serial.println();
    }



    Serial.println("------------------------------------");
    Serial.println("Get JSON test...");

    if (Firebase.getJSON(firebaseData, path))
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
        else if (firebaseData.dataType() == "json"){
           
           jsonData = firebaseData.jsonData(); //store for next test
           Serial.println(firebaseData.jsonData());

        }
            
        Serial.println("------------------------------------");
        Serial.println();
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }

    FirebaseJsonObject jsonParseResult;


     Serial.println("------------------------------------");
     Serial.println("Iterate all get back json data...");

      json2.clear().setJsonData(jsonData);
      json2.parse();
      size_t count =json2.getJsonObjectIteratorCount();
      String key;
      String value;

      for (size_t i = 0; i < count; i++)
      {
        
        json2.jsonObjectiterator(i,key,value);
        jsonParseResult = json2.parseResult();

        Serial.print("KEY: ");
        Serial.print(key);
        Serial.print(", ");
        Serial.print("VALUE: ");
        Serial.print(value); 
        Serial.print(", ");
        Serial.print("TYPE: ");
        Serial.println(jsonParseResult.type);        

      }

     Serial.println("------------------------------------");
     Serial.println();


    Serial.println("------------------------------------");
    Serial.println("Parse for some child node...");

    
    json2.parse().get("anotherData").get("myVal");    

    jsonParseResult = json2.parseResult();

    if(jsonParseResult.success){
     
      Serial.println("1. Parse json data result for node /anotherData/myVal");
      Serial.print("TYPE: ");
      Serial.println(jsonParseResult.type);
      Serial.print("VALUE: ");
      Serial.println(jsonParseResult.stringValue);
      //Serial.println(jsonParseResult.doubleValue);

    }else{
      Serial.println("1. Parse json data for node /anotherData/myVal was failed!");
    } 

    //Set parse with false to get data through all children nodes, 
    //otherwise this would be failed to get child node data because of it's under data2 node.
    json2.parse(false).get("simpleData"); 


    jsonParseResult = json2.parseResult();

    Serial.println();

    if(jsonParseResult.success){
     
      Serial.println("2. Parse json data result for node /data2/simpleData with skipChild set to false");
      Serial.print("TYPE: ");
      Serial.println(jsonParseResult.type);
      Serial.print("VALUE: ");
      Serial.println(jsonParseResult.stringValue);
      //Serial.println(jsonParseResult.doubleValue);

    }else{
      Serial.println("2. Parse json data for node /data2/simpleData was failed!");
    }    



    //Any json object inside array also can be accessed by set parse(false),
    //The next test will ignore it and the result was failed as expected.
    json2.parse().get("test2"); 


    jsonParseResult = json2.parseResult();

    Serial.println();

    if(jsonParseResult.success){
     
      Serial.println("3. Parse json data result for node /myArray/test2 with skipChild set to true, or ignored");
      Serial.print("TYPE: ");
      Serial.println(jsonParseResult.type);
      Serial.print("VALUE: ");
      Serial.println(jsonParseResult.stringValue);
      //Serial.println(jsonParseResult.doubleValue);

    }else{
      Serial.println("3. Parse json data for node /myArray/test2 was failed as expected!");
    }     
        
     Serial.println("------------------------------------");
     Serial.println();


}

void loop()
{
}
