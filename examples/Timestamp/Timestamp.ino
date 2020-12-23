
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

//This example shows how to set and push timestamp (server time) which is the server variable that suopported by Firebase

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

  /*
  This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
  Firewall that allows only GET and POST requests.
  
  Firebase.enableClassicRequest(fbdo, true);
  */

  String path = "/Test";

  Serial.println("------------------------------------");
  Serial.println("Set Timestamp test...");

  if (Firebase.setTimestamp(fbdo, path + "/Set/Timestamp"))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());

    //Timestamp saved in millisecond, get its seconds from intData()
    Serial.print("TIMESTAMP (Seconds): ");
    Serial.println(fbdo.intData());

    //Or print the total milliseconds from doubleData()
    //Due to bugs in Serial.print in Arduino library, use printf to print double instead.
    printf("TIMESTAMP (milliSeconds): %.0lf\n", fbdo.doubleData());

    //Or print it from payload directly
    Serial.print("TIMESTAMP (milliSeconds): ");
    Serial.println(fbdo.payload());

    //Due to some internal server error, ETag cannot get from setTimestamp
    //Try to get ETag manually
    Serial.println("ETag: " + Firebase.getETag(fbdo, path + "/Set/Timestamp"));
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

  Serial.println("------------------------------------");
  Serial.println("Get Timestamp (double of milliseconds) test...");

  if (Firebase.getDouble(fbdo, path + "/Set/Timestamp"))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());

    printf("TIMESTAMP: %.0lf\n", fbdo.doubleData());
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

  Serial.println("------------------------------------");
  Serial.println("Push Timestamp test...");

  if (Firebase.pushTimestamp(fbdo, path + "/Push/Timestamp"))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.print("PUSH NAME: ");
    Serial.println(fbdo.pushName());

    //Due to some internal server error, ETag cannot get from pushTimestamp
    //Try to get ETag manually
    Serial.println("ETag: " + Firebase.getETag(fbdo, path + "/Push/Timestamp/" + fbdo.pushName()));
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

void loop()
{
}