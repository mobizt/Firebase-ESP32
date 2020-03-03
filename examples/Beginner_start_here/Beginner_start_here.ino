
/*
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2019 mobizt
 * 
 * This example is for the beginner
 *
*/


#include <WiFi.h>
#include <FirebaseESP32.h>

//1. Change the following info
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"


//2. Define FirebaseESP8266 data object for data sending and receiving
FirebaseData firebaseData;


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


  //3. Set your Firebase info

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //4. Enable auto reconnect the WiFi when connection lost
  Firebase.reconnectWiFi(true);

  //5. Try to set int data to Firebase
  //The set function returns bool for the status of operation
  //firebaseData requires for sending the data
  if(Firebase.setInt(firebaseData, "/LED_Status", 1))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from firebaseData

    Serial.print("Error in setInt, ");
    Serial.println(firebaseData.errorReason());
  }


  //6. Try to get int data from Firebase
  //The get function returns bool for the status of operation
  //firebaseData requires for receiving the data
  if(Firebase.getInt(firebaseData, "/LED_Status"))
  {
    //Success
    Serial.print("Get int data success, int = ");
    Serial.println(firebaseData.intData());

  }else{
    //Failed?, get the error reason from firebaseData

    Serial.print("Error in getInt, ");
    Serial.println(firebaseData.errorReason());
  }

  /*

  In case where you want to set other data types i.e. bool, float, double and String, you can use setBool, setFloat, setDouble and setString.
  If you want to get data which you known its type at specific node, use getInt, getBool, getFloat, getDouble, getString.
  If you don't know the data type at specific node, use get and check its type.

  The following shows how to get the variant data

  */

 if(Firebase.get(firebaseData, "/LED_Status"))
  {
    //Success
    Serial.print("Get variant data success, type = ");
    Serial.println(firebaseData.dataType());

    if(firebaseData.dataType() == "int"){
      Serial.print("data = ");
      Serial.println(firebaseData.intData());
    }else if(firebaseData.dataType() == "bool"){
      if(firebaseData.boolData())
        Serial.println("data = true");
      else
        Serial.println("data = false");
    }

  }else{
    //Failed?, get the error reason from firebaseData

    Serial.print("Error in get, ");
    Serial.println(firebaseData.errorReason());
  }

  /*

  If you want to get the data in realtime instead of using get, see the stream examples.
  If you want to work with JSON, see the FirebaseJson, jsonObject and jsonArray examples.

  If you have questions or found the bugs, feel free to open the issue here https://github.com/mobizt/Firebase-ESP32

  */




}

void loop()
{
}

