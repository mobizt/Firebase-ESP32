
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

/** 
 * This example shows the basic example for automatic plant watering system.
 * The sketch will set GPIO16 for Pump1, and GPIO12 for Pump2
 * The status of pumps showed at /PlantWatering/status
 * 
 * Two pumps will be set to turn on in the moring and evening for 120 second everyday
 * To manually turn on and off both pumps, change the value under /PlantWatering/control
 * 
 * To control the device, send command at /PlantWatering/control/cmd and the result from process
 * showed at /PlantWatering/status/terminal
 * 
 * The command and its description.
 * 
 * idle: nothing to do
 * get-time: get time from NTP server
 * boot: restart the device
 * load-pump: load pump configuration
 * load-schedule: load schedule configuration
 * pump-count: show the number of pumps at /PlantWatering/status/terminal
 * schedule-count: show the number of schedules at /PlantWatering/status/terminal
*/

//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <time.h>


#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_HOST "PROJECT_ID.firebaseio.com" 

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "DATABASE_SECRET"

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
//Change to match your time zone
float time_zone = 3;
float daylight_offset_in_sec = 0;

//For 10 presets
preset_time_t presetTime[10];

//For 5 pumps
pump_info_t pumpInfo[5];

void setPumpState(int pumpIndex, int state);
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);
void setDeviceTime(float time_zone, float daylight_offset_in_sec);
void setDailySchedule(int presetIndex, int pumpIndex, int activeState, int inactiveState, int hour, int min, int sec, int duration_in_sec, FirebaseJsonArray *scheduleConfig);
void runSchedule();
void loadSchedule(FirebaseData &data);
void setPump(int pumpIndex, String id, int gpio, int state, FirebaseJsonArray *pumpConfig);
void loadPump(FirebaseData &data);

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

    //get time from NTP server and set up
    Serial.println("Get time from NTP server...");
    setDeviceTime(time_zone, daylight_offset_in_sec);

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    if (!Firebase.beginStream(firebaseData1, path + "/control"))
        Serial.println(firebaseData1.errorReason());

    Firebase.setStreamCallback(firebaseData1, streamCallback, streamTimeoutCallback);

    Firebase.set(firebaseData2, path + "/control/cmd", "idle");

    if (!Firebase.pathExist(firebaseData2, path + "/config/pump"))
    {
        //Setup initial pump data

        FirebaseJsonArray pumpConfig;

        //pump index, id, gpio, state, pump config
        setPump(0, "Pump1", 16, 0, &pumpConfig);
        setPump(1, "Pump2", 17, 0, &pumpConfig);

        Firebase.set(firebaseData2, path + "/config/pump", pumpConfig);
    }
    else
    {
        if (Firebase.get(firebaseData2, path + "/config/pump"))
            loadPump(firebaseData2);
    }

    //Set up node for pump if not exist.
    int pumpNum = sizeof(pumpInfo) / sizeof(pumpInfo[0]);
    for (int i = 0; i < pumpNum; i++)
    {
        if (pumpInfo[i].id != "")
        {
            if (!Firebase.pathExist(firebaseData2, path + "/control/" + pumpInfo[i].id))
            {
                Firebase.set(firebaseData2, path + "/control/" + pumpInfo[i].id, pumpInfo[i].state);
                Firebase.set(firebaseData2, path + "/status/" + pumpInfo[i].id, pumpInfo[i].state);
            }
        }
    }

    if (timeReady)
    {

        if (!Firebase.pathExist(firebaseData2, path + "/config/schedule"))
        {

            Serial.println("Setup timer...");

            FirebaseJsonArray scheduleConfig;

            //preset index, pump index, activeState, inactiveState, hour, minute, second, duration in second, new setting

            //Set for Pump1 (index 0) to turn on (1) 120 seconds from 7:30:00 then turn off (0)
            setDailySchedule(0, 0, 1, 0, 7, 30, 00, 120, &scheduleConfig);
            //Set for Pump1 (index 0) to turn on (1) 120 seconds from 17:30:00 then turn off (0)
            setDailySchedule(1, 0, 1, 0, 17, 30, 00, 120, &scheduleConfig);

            //Set for Pump2 (index 1) to turn on (1) 120 seconds from 7:30:00 then turn off (0)
            setDailySchedule(2, 1, 1, 0, 7, 30, 00, 120, &scheduleConfig);
            //Set for Pump2 (index 1) to turn on (1) 120 seconds from 17:30:00 then turn off (0)
            setDailySchedule(3, 1, 1, 0, 17, 30, 00, 120, &scheduleConfig);

            Firebase.set(firebaseData2, path + "/config/schedule", scheduleConfig);
        }
        else
        {
            if (Firebase.get(firebaseData2, path + "/config/schedule"))
                loadSchedule(firebaseData2);
        }
    }

    Serial.println("Ready!");
}

void loop()
{
    runSchedule();
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
        Serial.println(pumpInfo[pumpIndex].id + " OFF");
    else if (state == 1)
        Serial.println(pumpInfo[pumpIndex].id + " ON");
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
            {
                setPumpState(i, data.intData());
                String status = pumpInfo[i].id;
                if (data.intData() == 0)
                    status += " OFF";
                else if (data.intData() == 1)
                    status += " ON";
                Firebase.set(firebaseData2, path + "/status/terminal", status);
            }
        }
    }
    else if (data.dataPath() == "/cmd")
    {
        if (data.stringData() == "get-time")
        {
            Serial.println("cmd: get time from NTP server");
            Firebase.set(firebaseData2, path + "/status/terminal", "get time");
            setDeviceTime(time_zone, daylight_offset_in_sec);
        }
        else if (data.stringData() == "load-pump")
        {
            Serial.println("cmd: load pump");
            Firebase.set(firebaseData2, path + "/status/terminal", "load pump");
            if (Firebase.get(firebaseData2, path + "/config/pump"))
                loadPump(firebaseData2);
        }
        else if (data.stringData() == "load-schedule")
        {
            Serial.println("cmd: load schedule");
            Firebase.set(firebaseData2, path + "/status/terminal", "load schedule");
            if (Firebase.get(firebaseData2, path + "/config/schedule"))
                loadSchedule(firebaseData2);
        }
        if (data.stringData() == "schedule-count")
        {
            int presetNum = sizeof(presetTime) / sizeof(presetTime[0]);
            Serial.println("cmd: schedule-count");
            int count = 0;
            for (int i = 0; i < presetNum; i++)
                if (presetTime[i].state > 0)
                    count++;
            Firebase.set(firebaseData2, path + "/status/terminal", String(count));
        }
        if (data.stringData() == "pump-count")
        {
            int pumpNum = sizeof(pumpInfo) / sizeof(pumpInfo[0]);
            Serial.println("cmd: pump-count");
            int count = 0;
            for (int i = 0; i < pumpNum; i++)
                if (pumpInfo[i].id != "")
                    count++;
            Firebase.set(firebaseData2, path + "/status/terminal", String(count));
        }
        else if (data.stringData() == "boot")
        {
            Serial.println("cmd: reboot device");
            Firebase.set(firebaseData2, path + "/status/terminal", "restart device");
            ESP.restart();
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
    if (timeReady)
        Firebase.set(firebaseData2, path + "/status/terminal", "idle");
    else
        Firebase.set(firebaseData2, path + "/status/terminal", "cannot get time");
}

void setPump(int pumpIndex, String id, int gpio, int state, FirebaseJsonArray *pumpConfig)
{
    int pumpNum = sizeof(pumpInfo) / sizeof(pumpInfo[0]);

    if (pumpIndex < pumpNum)
    {
        pumpInfo[pumpIndex].id = id;
        pumpInfo[pumpIndex].gpio = gpio;
        pumpInfo[pumpIndex].state = state;
        pinMode(gpio, OUTPUT);

        if (pumpConfig)
        {
            FirebaseJson json;
            json.add("id", id);
            json.add("gpio", gpio);
            json.add("state", state);
            json.add("idx", pumpIndex);
            pumpConfig->add(json);
        }
    }
}

void setDailySchedule(int presetIndex, int pumpIndex, int activeState, int inactiveState, int hour, int min, int sec, int duration_in_sec, FirebaseJsonArray *scheduleConfig)
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

        if (scheduleConfig)
        {
            FirebaseJson json;
            json.add("hour", hour);
            json.add("min", min);
            json.add("sec", sec);
            json.add("dur", duration_in_sec);
            json.add("idx", pumpIndex);
            json.add("act", activeState);
            json.add("ina", inactiveState);
            scheduleConfig->add(json);
        }
    }
}

void runSchedule()
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
                        String status = pumpInfo[presetTime[i].pump_index].id + " ON - " + String(presetTime[i].duration) + " sec from ";

                        if (target_timeinfo.tm_hour < 10)
                            status += "0";
                        status + String(target_timeinfo.tm_hour);
                        status += ":";
                        if (target_timeinfo.tm_min < 10)
                            status + "0";
                        status + String(target_timeinfo.tm_min);
                        status += ":";
                        if (target_timeinfo.tm_sec < 10)
                            status += "0";
                        status + String(target_timeinfo.tm_sec);

                        Serial.println(status);

                        presetTime[i].state = 2;
                        Firebase.set(firebaseData2, path + "/control/" + pumpInfo[presetTime[i].pump_index].id, presetTime[i].active);
                        setPumpState(presetTime[i].pump_index, presetTime[i].active);
                        Firebase.set(firebaseData2, path + "/status/terminal", status);
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
                        String status = pumpInfo[presetTime[i].pump_index].id + " OFF";
                        Firebase.set(firebaseData2, path + "/status/terminal", status);
                    }
                }
            }
        }
    }
}

void loadSchedule(FirebaseData &data)
{
    int presetNum = sizeof(presetTime) / sizeof(presetTime[0]);
    for (int i = 0; i < presetNum; i++)
        presetTime[i].state = 0;

    FirebaseJsonArray *scheduleConfig = data.jsonArrayPtr();
    FirebaseJson json;

    Serial.println("Schedule config:");

    for (size_t i = 0; i < scheduleConfig->size(); i++)
    {
        FirebaseJsonData &jsonData = data.jsonData();
        scheduleConfig->get(jsonData, i);
        Serial.println(jsonData.stringValue);
        json.setJsonData(jsonData.stringValue);
        json.get(jsonData, "hour");
        int hour = jsonData.intValue;
        json.get(jsonData, "min");
        int min = jsonData.intValue;
        json.get(jsonData, "sec");
        int sec = jsonData.intValue;
        json.get(jsonData, "dur");
        int dur = jsonData.intValue;
        json.get(jsonData, "idx");
        int idx = jsonData.intValue;
        json.get(jsonData, "act");
        int act = jsonData.intValue;
        json.get(jsonData, "ina");
        int ina = jsonData.intValue;
        setDailySchedule(i, idx, act, ina, hour, min, sec, dur, nullptr);
    }
}

void loadPump(FirebaseData &data)
{
    int pumpNum = sizeof(pumpInfo) / sizeof(pumpInfo[0]);
    for (int i = 0; i < pumpNum; i++)
        pumpInfo[i].id = "";

    FirebaseJsonArray *pumpConfig = data.jsonArrayPtr();
    FirebaseJson json;

    Serial.println("Pump config:");

    for (size_t i = 0; i < pumpConfig->size(); i++)
    {
        FirebaseJsonData &jsonData = data.jsonData();
        pumpConfig->get(jsonData, i);
        Serial.println(jsonData.stringValue);
        json.setJsonData(jsonData.stringValue);
        json.get(jsonData, "id");
        String id = jsonData.stringValue;
        json.get(jsonData, "state");
        int state = jsonData.intValue;
        json.get(jsonData, "gpio");
        int gpio = jsonData.intValue;
        json.get(jsonData, "idx");
        int index = jsonData.intValue;
        setPump(index, id, gpio, state, nullptr);
    }
}
