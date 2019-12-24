
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

/*

  This example shows the basic example for automatic plant watering system.
  The sketch will set GPIO16 for Pump1, and GPIO17 for Pump2
  The status of pumps showed at /PlantWatering/status
  
  Two pumps will be set to turn on in the moring and evening for 120 second everyday
  To manually turn on and off both pumps, change the value under /PlantWatering/control

*/

//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <time.h>


#define WIFI_SSID "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"

struct preset_time_t
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int duration;
    int state;
    int pump_index;
    int active;
    int inactive;
};

struct pump_info_t
{
    String id;
    int state;
    int gpio;
};

FirebaseData firebaseData1;
FirebaseData firebaseData2;

String path = "/PlantWatering";
bool timeReady = false;

//For 4 presets
preset_time_t presetTime[4];

//For 2 pumps
pump_info_t pumpInfo[2];


void setPumpState(int pumpIndex, int state);
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);
void setDeviceTime(float time_zone, float daylight_offset_in_sec);
void setDailyTimer(int presetIndex, int pumpIndex, int activeState, int inactiveState, int hour, int min, int sec, int duration_in_sec);
void runTimer();

void setup()
{

    Serial.begin(115200);

    //Setup initial pump data
    //state = 0 for Off, state = 1 for On
    //id should not contains space or special chars

    pumpInfo[0].gpio = 16;
    pumpInfo[0].id = "Pump1";
    pumpInfo[0].state = 0;

    pumpInfo[1].gpio = 17;
    pumpInfo[1].id = "Pump2";
    pumpInfo[1].state = 0;

    //Set output

    pinMode(pumpInfo[0].gpio, OUTPUT);
    pinMode(pumpInfo[1].gpio, OUTPUT);

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

    //get time from NTP server and set up
    Serial.println("Get time from NTP server...");
    
    //Change to match your time zone
    float time_zone = 3;
    float daylight_offset_in_sec = 0;
    setDeviceTime(time_zone, daylight_offset_in_sec);

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    if (!Firebase.beginStream(firebaseData1, path + "/control"))
        Serial.println(firebaseData1.errorReason());

    Firebase.setStreamCallback(firebaseData1, streamCallback, streamTimeoutCallback);

    //Set up node for pump if not exist.
    int pumpNum = sizeof(pumpInfo) / sizeof(pumpInfo[0]);
    for (int i = 0; i < pumpNum; i++)
    {
        if (!Firebase.pathExist(firebaseData2, path + "/control/" + pumpInfo[i].id))
        {
            Firebase.set(firebaseData2, path + "/control/" + pumpInfo[i].id, pumpInfo[i].state);
            Firebase.set(firebaseData2, path + "/status/" + pumpInfo[i].id, pumpInfo[i].state);
        }
    }

    if (timeReady)
    {
        Serial.println("Setup timer...");

        //preset index, pump index, activeState, inactiveState, hour, minute, second, duration in second

        //Set for Pump1 (index 0) to turn on (1) 120 seconds from 7:30:00 then turn off (0)
        setDailyTimer(0, 0, 1, 0, 7, 30, 00, 120);
        //Set for Pump1 (index 0) to turn on (1) 120 seconds from 17:30:00 then turn off (0)
        setDailyTimer(1, 0, 1, 0, 17, 30, 00, 120);

        //Set for Pump2 (index 1) to turn on (1) 120 seconds from 7:30:00 then turn off (0)
        setDailyTimer(2, 1, 1, 0, 7, 30, 00, 120);
        //Set for Pump2 (index 1) to turn on (1) 120 seconds from 17:30:00 then turn off (0)
        setDailyTimer(3, 1, 1, 0, 17, 30, 00, 120);
    }

    Serial.println("Ready!");
}

void loop()
{
    runTimer();
}

void setPumpState(int pumpIndex, int state)
{
    int pumpNum = sizeof(pumpInfo) / sizeof(pumpInfo[0]);
    if (pumpIndex < 0 || pumpIndex >= pumpNum)
        return;

    if (pumpInfo[pumpIndex].state == state)
        return;

    digitalWrite(pumpInfo[pumpIndex].gpio, state);
    pumpInfo[pumpIndex].state = state;

    Firebase.set(firebaseData2, path + "/status/" + pumpInfo[pumpIndex].id, state);
    if (state == 0)
        Serial.println(pumpInfo[pumpIndex].id + " Off");
    else if (state == 1)
        Serial.println(pumpInfo[pumpIndex].id + " On");
}

void streamCallback(StreamData data)
{

    int pumpNum = sizeof(pumpInfo) / sizeof(pumpInfo[0]);

    if (data.dataType() == "json")
    {
        FirebaseJson *json = data.jsonObjectPtr();
        FirebaseJsonData jsonData;

        for (int i = 0; i < pumpNum; i++)
        {
            //Parse for each pump state
            json->get(jsonData, pumpInfo[i].id);
            setPumpState(i, jsonData.intValue);
        }
    }
    else if (data.dataType() == "int")
    {
        for (int i = 0; i < pumpNum; i++)
        {
            if (data.dataPath() == "/" + pumpInfo[i].id)
                setPumpState(i, data.intData());
        }
    }
}

void streamTimeoutCallback(bool timeout)
{
    if (timeout)
    {
        //Timeout occurred
    }
}

void setDeviceTime(float time_zone, float daylight_offset_in_sec)
{
    configTime(time_zone * 3600, daylight_offset_in_sec, "pool.ntp.org", "time.nist.gov", NULL);
    time_t now = time(nullptr);
    int cnt = 0;

    while (now < 8 * 3600 * 2 && cnt < 20)
    {
        delay(50);
        now = time(nullptr);
        cnt++;
    }

    timeReady = now > 8 * 3600 * 2;
}

void setDailyTimer(int presetIndex, int pumpIndex, int activeState, int inactiveState, int hour, int min, int sec, int duration_in_sec)
{
    int presetNum = sizeof(presetTime) / sizeof(presetTime[0]);

    if (presetIndex < presetNum)
    {
        presetTime[presetIndex].tm_hour = hour;
        presetTime[presetIndex].tm_min = min;
        presetTime[presetIndex].tm_sec = sec;
        presetTime[presetIndex].state = 1;
        presetTime[presetIndex].duration = duration_in_sec;
        presetTime[presetIndex].pump_index = pumpIndex;
        presetTime[presetIndex].active = activeState;
        presetTime[presetIndex].inactive = inactiveState;
    }
}

void runTimer()
{
    struct tm current_timeinfo;
    struct tm target_timeinfo;
    time_t current_ts = time(nullptr);
    time_t target_ts;
    getLocalTime(&current_timeinfo);
    target_timeinfo.tm_year = current_timeinfo.tm_year;
    target_timeinfo.tm_mon = current_timeinfo.tm_mon;
    target_timeinfo.tm_mday = current_timeinfo.tm_mday;

    int presetNum = sizeof(presetTime) / sizeof(presetTime[0]);
    int pumpNum = sizeof(pumpInfo) / sizeof(pumpInfo[0]);

    for (int i = 0; i < presetNum; i++)
    {
        if (presetTime[i].state > 0)
        {
            target_timeinfo.tm_hour = presetTime[i].tm_hour;
            target_timeinfo.tm_min = presetTime[i].tm_min;
            target_timeinfo.tm_sec = presetTime[i].tm_sec;

            target_ts = mktime(&target_timeinfo);

            if (current_ts >= target_ts && current_ts <= target_ts + presetTime[i].duration && presetTime[i].state == 1)
            {

                if (presetTime[i].pump_index > -1 && presetTime[i].pump_index < pumpNum)
                {
                    if (presetTime[i].active != pumpInfo[presetTime[i].pump_index].state)
                    {
                        Serial.print(pumpInfo[presetTime[i].pump_index].id + " will be activated for ");
                        Serial.print(presetTime[i].duration);
                        Serial.print(" second");
                        if (presetTime[i].duration > 1)
                            Serial.print("s");
                        Serial.print(" from ");
                        if (target_timeinfo.tm_hour < 10)
                            Serial.print("0");
                        Serial.print(target_timeinfo.tm_hour);
                        Serial.print(":");
                        if (target_timeinfo.tm_min < 10)
                            Serial.print("0");
                        Serial.print(target_timeinfo.tm_min);
                        Serial.print(":");
                        if (target_timeinfo.tm_sec < 10)
                            Serial.print("0");
                        Serial.println(target_timeinfo.tm_sec);

                        presetTime[i].state = 2;
                        Firebase.set(firebaseData2, path + "/control/" + pumpInfo[presetTime[i].pump_index].id, presetTime[i].active);
                        setPumpState(presetTime[i].pump_index, presetTime[i].active);
                    }
                }
            }
            else if (current_ts > target_ts + presetTime[i].duration && presetTime[i].state == 2)
            {

                if (presetTime[i].pump_index > -1 && presetTime[i].pump_index < pumpNum)
                {
                    if (presetTime[i].inactive != pumpInfo[presetTime[i].pump_index].state)
                    {
                        presetTime[i].state = 1;
                        Firebase.set(firebaseData2, path + "/control/" + pumpInfo[presetTime[i].pump_index].id, presetTime[i].inactive);
                        setPumpState(presetTime[i].pump_index, presetTime[i].inactive);
                    }
                }
            }
        }
    }
}
