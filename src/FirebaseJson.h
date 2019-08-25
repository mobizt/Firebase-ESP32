
/*
 * FirebaseJSON, version 1.0.3
 * 
 * The library for create and pares JSON object.
 * 
 * August 25, 2019
 * 
 * 
 * This library was implement the zserge's JSON object parser library, jasmine JSMN which available here https://zserge.com/jsmn.html
 * 
 * The MIT License (MIT)
 * Copyright (c) 2019 K. Suwatchai (Mobizt)
 * 
 * 
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#ifndef FirebaseJson_H
#define FirebaseJson_H

#include <Arduino.h>
#include "jsmn.h"
#include <memory>

static const char FirebaseJson_STR_1[] PROGMEM = ",";
static const char FirebaseJson_STR_2[] PROGMEM = "\"";
static const char FirebaseJson_STR_3[] PROGMEM = ":";
static const char FirebaseJson_STR_4[] PROGMEM = "%d";
static const char FirebaseJson_STR_5[] PROGMEM = "%f";
static const char FirebaseJson_STR_6[] PROGMEM = "false";
static const char FirebaseJson_STR_7[] PROGMEM = "true";
static const char FirebaseJson_STR_8[] PROGMEM = "{";
static const char FirebaseJson_STR_9[] PROGMEM = "}";
static const char FirebaseJson_STR_10[] PROGMEM = "[";
static const char FirebaseJson_STR_11[] PROGMEM = "]";
static const char FirebaseJson_STR_12[] PROGMEM = "string";
static const char FirebaseJson_STR_13[] PROGMEM = "int";
static const char FirebaseJson_STR_14[] PROGMEM = "double";
static const char FirebaseJson_STR_15[] PROGMEM = "bool";
static const char FirebaseJson_STR_16[] PROGMEM = "object";
static const char FirebaseJson_STR_17[] PROGMEM = "array";
static const char FirebaseJson_STR_18[] PROGMEM = "null";
static const char FirebaseJson_STR_19[] PROGMEM = "undefined";
static const char FirebaseJson_STR_20[] PROGMEM = ".";

class FirebaseJson;
class FirebaseJsonArray;

class FirebaseJsonObject
{
    friend class FirebaseJson;

public:
    FirebaseJsonObject(){};
    ~FirebaseJsonObject(){};

    String stringValue;
    int intValue;
    double doubleValue;
    bool boolValue;
    String type;
    bool success;

private:
    int _type = 0;
    int _index = 0;
    int _start = 0;
    int _end = 0;
};



class FirebaseJson
{

    friend class FirebaseJsonArray;
    friend class FirebaseJsonObject;
    friend class FCMObject;
    friend class FirebaseData;
    friend class StreamData;

public:
    FirebaseJson();
    FirebaseJson(std::string &data);
    ~FirebaseJson();

    /*
    Clear buffer data and tokens pointer.
    
    @return instance of an object.

   */
    FirebaseJson &clear();

    /*
    Set JSON data to buffer.
    
    @param data - The JSON object string.

    @return instance of an object.

    The internal buffer will be overwritten by add functions.

   */
    FirebaseJson &setJsonData(const String &data);

    /*
    Add the string to JSON object.
    
    @param key - Key string of data.

    @param value - String data.    

    @return instance of an object.

   */
    FirebaseJson &addString(const String &key, const String &value);

    /*
    Add the array of objects to JSON object.
    
    @param key - Key string of data.

    @param arr - The FirebaseJsonArray object.    

    @return instance of an object.

   */
    FirebaseJson &addArray(const String &key, FirebaseJsonArray *arr);

    /*
    Add the integer value to JSON object.
    
    @param key - Key string of data.

    @param value - Integer data.    

    @return instance of an object.

   */
    FirebaseJson &addInt(const String &key, int value);

    /*
    Add the double value to JSON object.
    
    @param key - Key string of data.

    @param value - Double data.    

    @return instance of an object.

   */
    FirebaseJson &addDouble(const String &key, double value);

    /*
    Add the boolean value to JSON object.
    
    @param key - Key string of data.

    @param value - Boolean data.    

    @return instance of an object.

   */
    FirebaseJson &addBool(const String &key, bool value);

    /*
    Add the JSON data to JSON object.
    
    @param key - Key string of data.

    @param json - FirebaseJson object that hold the data to be added to this JSON object.    

    @return instance of an object.

   */
    FirebaseJson &addJson(const String &key, FirebaseJson *json);

    /*
    Get the JSON object's string. 

    @return serialized string of JSON object.

   */
    String toString();

    /*
    Parse the internal buffer JSON object.
    
    @param skipChild - Flag to skip any children nodes under root or array.

    @param len - Optional. The expected number of JSON tokens (key and value). Should be number of keys and values + 1.
                 Default value is 50. If object parse failed, increase this value.     

    @return instance of an object.

   */
    FirebaseJson &parse(bool skipChild = true, size_t len = 0);

    /*
    Get the value from key of JSON object.
    
    @param key - Key string of data to get.

    @return instance of an object.

    This should call after parse().

   */
    FirebaseJson &get(const String &key);

    /*
    Set the numbers of JSON tokens (keys + values + 1) to be available for read from jsonObjectIterator.  

    @return numbers of tokens.

   */
    size_t getJsonObjectIteratorCount();

    /*
    Read data from JSON objects by providing tokens ID.
    
    @param index - The referenced token index. This will auto increase to the next token index after read.

    @param key - The referenced key data string. This provided the key data output.

    @param value - The referenced value string. This provided the value of current key output.   

    @return instance of an object.

   */
    FirebaseJson &jsonObjectiterator(size_t &index,String &key, String &value);

    /*
    Get the parse result of JSON object.   

    @return FirebaseJsonObject.

    The properties available are: stringValue, intValue, doubleValue, boolValue, type, success.

    The type property provides the types of value string e.g. string, int, double, boolean, array, object, null and undefined.

    The success property provides the success status of get or parse functions.

   */
    FirebaseJsonObject parseResult();

    std::string toStdString(bool isJson = true);

private:
    std::string _jstr = "";
    uint32_t _cCnt = 0;
    int _parseResNum = 0;
    bool _skipChild = false;
    bool _paresRes = false;

    FirebaseJsonObject _jsonObj;
    std::unique_ptr<jsmn_parser> _parser = std::unique_ptr<jsmn_parser>(new jsmn_parser());
    std::unique_ptr<jsmntok_t> _tokens = nullptr;
    FirebaseJson &setJsonData(std::string &data);
    FirebaseJson &_add(const char *key, const char *value, size_t klen, size_t vlen, bool isString = true, bool isJson = true);
    FirebaseJson &addArrayStr(const char *value, size_t len, bool isString);
    uint32_t getCount();
    void resetParseResult();
    void setParseResultType();
    int strpos(const char *haystack, const char *needle, int offset);
    int rstrpos(const char *haystack, const char *needle, int offset);
    char *rstrstr(const char *haystack, const char *needle);
};

class FirebaseJsonArray
{

public:
    FirebaseJsonArray();
    ~FirebaseJsonArray();

    /*
    Add the string value to JSON Array object.

    @param value - String data.    

    @return instance of an object.

   */
    FirebaseJsonArray &addString(const String &value);

    /*
    Add the integer value to JSON Array object.

    @param value - Integer data.    

    @return instance of an object.

   */
    FirebaseJsonArray &addInt(int value);

    /*
    Add the double value to JSON Array object.

    @param value - double data.    

    @return instance of an object.

   */
    FirebaseJsonArray &addDouble(double value);

    /*
    Add the boolean value to JSON Array object.

    @param value - Boolean data.    

    @return instance of an object.

   */
    FirebaseJsonArray &addBool(bool value);

    /*
    Add the JSON object to JSON Array object.

    @param json - FirebaseJson object that holds data to be added to array.    

    @return instance of an object.

   */
    FirebaseJsonArray &addJson(FirebaseJson *json);

    /*
    Add the JSON arrat object to JSON Array object.

    @param arr - JSON array object that holds data to be added to this JSON array.    

    @return instance of an object.

   */
    FirebaseJsonArray &addArray(FirebaseJsonArray *arr);

    /*
    Get the JSON Array object's string.  

    @return string of an object.

   */
    String toString();

private:
    std::string _jstr = "";
    uint32_t _cCnt = 0;
    FirebaseJson _json;

    std::string toStdString();
    uint32_t getCount();

    friend class FirebaseJson;
};

#endif