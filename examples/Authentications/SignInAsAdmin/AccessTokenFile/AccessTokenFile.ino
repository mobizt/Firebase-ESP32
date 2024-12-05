
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP8266
 *
 * Copyright (c) 2023 mobizt
 *
 */

/** This example will show how to authenticate as admin using
 * the Service Account file to create the access token to sign in internally.
 *
 * The library will connect to NTP server to get the time (in case your device
 * time was not set)
 * and waiting for the time to be ready.
 *
 * If the waiting is timed out and the system time is not ready, the custom and
 * OAuth2.0 acces tokens generation will fail
 * because of invalid expiration time in JWT token that used in the id/access
 * token request.
 *
 * This library used RFC 7523, JWT Bearer Token Grant Type Profile for OAuth 2.0
 * which no refresh token is available for access token exchanging.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

#include <FirebaseESP32.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

/* 2. If work with RTDB, define the RTDB URL */
#define DATABASE_URL "URL" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 3. Define the Firebase Data object */
FirebaseData fbdo;

/* 4. Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* 5. Define the FirebaseConfig data for config data */
FirebaseConfig config;

unsigned long dataMillis = 0;
int count = 0;

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

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the certificate file (optional) */
  /** From the test as of July 2021, GlobalSign Root CA was missing from Google server
   * when checking with https://www.sslchecker.com/sslchecker.
   * The certificate chain, GTS Root R1 can be used instead.
   *
   * ESP32 Arduino SDK supports PEM format only even mBedTLS supports DER format too.
   * ESP8266 SDK supports both PEM and DER format certificates.
   */
  config.cert.file = "/gtsr1.pem";
  config.cert.file_storage = StorageType::FLASH; // or StorageType::SD

  /* The file systems for flash and SD/SDMMC can be changed in FirebaseFS.h. */

  /* Assign the sevice account JSON file and the file storage type (required) */
  config.service_account.json.path = "/service_account_file.json"; // change this for your json file
  config.service_account.json.storage_type = StorageType::FLASH;   // or StorageType::SD

  /** The user UID set to empty to sign in as admin */
  // auth.token.uid.clear();

  /* Assign the RTDB URL */
  config.database_url = DATABASE_URL;

  /** The scope of the OAuth2.0 authentication
   * If you wan't this access token for others Google Cloud Services.
   */
  // config.signer.tokens.scope = "Google Scope 1 Url, Google Scope 2 Url,..";

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  /** To set system time with the timestamp from RTC
   * The internal NTP server time acquisition
   * of token generation process will be skipped,
   * if the system time is already set.
   *
   * sec is the seconds from midnight Jan 1, 1970
   */
  // Firebase.setSystemTime(sec);

  /* Now we start to signin using access token */

  /** Initialize the library with the Firebase authen and config.
   *
   * The device time will be set by sending request to the NTP server
   * befor token generation and exchanging.
   *
   * The signed RSA256 jwt token will be created and used for access token exchanging.
   *
   * Theses process may take time to complete.
   */
  Firebase.begin(&config, &auth);

  /* The access token can be accessed from Firebase.getToken(). */
}

void loop()
{
  // Firebase.ready() should be called repeatedly to handle authentication tasks.

  if (Firebase.ready() && millis() - dataMillis > 5000)
  {
    dataMillis = millis();
    Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, "/test/int", count++) ? "ok" : fbdo.errorReason().c_str());
  }
}