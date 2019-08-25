/*
 * FirebaseJSON, version 1.0.3
 * 
 * The library for create and pares JSON object.
 * 
 * August 25, 2019
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


#include "FirebaseJson.h"

FirebaseJson::FirebaseJson(){};

FirebaseJson::FirebaseJson(std::string &data){
    setJsonData(data);
}

FirebaseJson::~FirebaseJson()
{
    clear();
    _parser.reset();
    _parser.release();
    _parser = nullptr;
};

FirebaseJson &FirebaseJson::setJsonData(std::string &data)
{
   return setJsonData(data.c_str());
}

FirebaseJson &FirebaseJson::setJsonData(const String &data)
{

    char *tmp= new char[10];
    memset(tmp, 0, 10);
    strcpy_P(tmp,FirebaseJson_STR_8);
    int p1 = strpos(data.c_str(), tmp, 0);
    memset(tmp, 0, 10);
    strcpy_P(tmp,FirebaseJson_STR_9);
    int p2 = rstrpos(data.c_str(), tmp, data.length()-1);

    if(p1!=-1) p1+=1;  
    
    if (p1 != -1 && p2 != -1)
    {
        _jstr = data.substring(p1, p2).c_str();
        _cCnt = 0;
    }

    delete[] tmp;
    return *this;
}

FirebaseJson &FirebaseJson::clear()
{
    std::string().swap(_jstr);
    _tokens.reset();
    _tokens.release();
    _tokens = nullptr;
    return *this;
}

FirebaseJson &FirebaseJson::addString(const String &key, const String &value)
{
    if (_cCnt == 0)
        _jstr.clear();

    _cCnt++;
    _add(key.c_str(), value.c_str(), key.length(), value.length(), true, true);
    return *this;
}

FirebaseJson &FirebaseJson::addArray(const String &key, FirebaseJsonArray *arr)
{
    if (_cCnt == 0)
        _jstr.clear();

    _cCnt += arr->getCount();

    _add(key.c_str(), arr->toString().c_str(), key.length(), arr->toString().length(), false, true);
    return *this;
}

FirebaseJson &FirebaseJson::addInt(const String &key, int value)
{
    size_t bufSize = 50;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    char *b = new char[10];
    memset(b, 0, 10);
    strcat_P(b, FirebaseJson_STR_4);
    sprintf(buf, b, value);
    if (_cCnt == 0)
        _jstr.clear();

    _cCnt++;
    _add(key.c_str(), buf, key.length(), 60, false, true);
    delete[] b;
    delete[] buf;
    return *this;
}

FirebaseJson &FirebaseJson::addDouble(const String &key, double value)
{
    size_t bufSize = 50;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    char *b = new char[10];
    memset(b, 0, 10);
    strcat_P(b, FirebaseJson_STR_5);
    sprintf(buf, b, value);

    if (_cCnt == 0)
        _jstr.clear();

    _cCnt++;
    _add(key.c_str(), buf, key.length(), 60, false, true);
    delete[] b;
    delete[] buf;
    return *this;
}

FirebaseJson &FirebaseJson::addBool(const String &key, bool value)
{
    size_t bufSize = 20;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    if (value)
        strcat_P(buf, FirebaseJson_STR_7);
    else
        strcat_P(buf, FirebaseJson_STR_6);

    if (_cCnt == 0)
        _jstr.clear();

    _cCnt++;
    _add(key.c_str(), buf, key.length(), bufSize, false, true);
    delete[] buf;
    return *this;
}

FirebaseJson &FirebaseJson::addJson(const String &key, FirebaseJson *json)
{
    if (_cCnt == 0)
        _jstr.clear();

    _cCnt += json->getCount();
    _add(key.c_str(), json->toStdString().c_str(), key.length(), json->toStdString().length(), false, true);
    return *this;
}

String FirebaseJson::toString()
{
    return toStdString().c_str();
}

std::string FirebaseJson::toStdString(bool isJson)
{

    std::string ret = "";
    size_t bufSize = 10;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);

    if (isJson)
        strcat_P(buf, FirebaseJson_STR_8);
    else
        strcat_P(buf, FirebaseJson_STR_10);

    ret += buf;
    ret += _jstr;

    memset(buf, 0, bufSize);
    if (isJson)
        strcat_P(buf, FirebaseJson_STR_9);
    else
        strcat_P(buf, FirebaseJson_STR_11);
    ret += buf;

    delete[] buf;
    return ret;
}

FirebaseJson &FirebaseJson::_add(const char *key, const char *value, size_t klen, size_t vlen, bool isString, bool isJson)
{
    size_t bufSize = klen + vlen + 10;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);

    if (_jstr.length() > 0)
        strcpy_P(buf, FirebaseJson_STR_1);

    if (isJson)
    {
        strcat_P(buf, FirebaseJson_STR_2);
        strcat(buf, key);
        strcat_P(buf, FirebaseJson_STR_2);
        strcat_P(buf, FirebaseJson_STR_3);
    }

    if (isString)
        strcat_P(buf, FirebaseJson_STR_2);
    strcat(buf, value);
    if (isString)
        strcat_P(buf, FirebaseJson_STR_2);
    _jstr += buf;
    delete[] buf;
    return *this;
}

FirebaseJson &FirebaseJson::addArrayStr(const char *value, size_t len, bool isString)
{
    _add("", value, 0, len, isString, false);
    return *this;
}

uint32_t FirebaseJson::getCount()
{
    return _cCnt;
}

FirebaseJson &FirebaseJson::parse(bool skipChild, size_t len)
{
   

    int bufLen = toStdString().length() + 1;

    char *buf = new char[bufLen];
    memset(buf, 0, bufLen);
    strcpy(buf, toStdString().c_str());

    _tokens.reset();
    _tokens.release();

    if (len == 0 && _cCnt > 0)
        len = _cCnt;
    else if (len == 0)
        len = 50;

    _tokens = std::unique_ptr<jsmntok_t>(new jsmntok_t[len * 2 + 20]);

    jsmn_init(_parser.get());
    _parseResNum = jsmn_parse(_parser.get(), buf, bufLen, _tokens.get(), len * 2 + 20);
    _paresRes = true;

    if (_parseResNum < 0)
        _paresRes = false;

    if (_parseResNum < 1 || _tokens.get()[0].type != JSMN_OBJECT)
        _paresRes = false;

    _skipChild = skipChild;
    _jsonObj.success = _paresRes;
    resetParseResult();
    setParseResultType();
    delete[] buf;
    return *this;
}

FirebaseJson &FirebaseJson::get(const String &key)
{
    if (_paresRes)
    {
        int bufLen = toStdString().length() + 1;
        char *k = new char[500];
        char *v = new char[bufLen];
        char *buf = new char[bufLen];
        memset(buf, 0, bufLen);
        strcpy(buf, toStdString().c_str());
        int type = _jsonObj._type;
        _jsonObj.success = false;
        resetParseResult();

        if (type == JSMN_OBJECT)
        {

            int index = _jsonObj._index;

            for (int j = 0; j < _tokens.get()[index + 1].size * 2; j++)
            {
                jsmntok_t *h = &_tokens.get()[index + j + 2];
                jsmntok_t *g = &_tokens.get()[index + j + 3];

                memset(k, 0, 100);
                strncpy(k, buf + h->start, h->end - h->start);

                if (strcmp(key.c_str(), k) == 0)
                {
                    memset(v, 0, bufLen);
                    strncpy(v, buf + g->start, g->end - g->start);
                    _jsonObj.stringValue = v;
                    _jsonObj._type = g->type;
                    _jsonObj._start = g->start;
                    _jsonObj._end = g->end;
                    _jsonObj._index = index + j + 2;
                    _jsonObj.success = true;
                    setParseResultType();
                    break;
                }

                if (g->type == JSMN_ARRAY || g->type == JSMN_OBJECT)
                    j += g->size + 1;
                else
                    j++;
            }

            delete[] buf;
            delete[] k;
            delete[] v;

            return *this;
        }

        _jsonObj._start = 0;
        _jsonObj._end = 0;
        _jsonObj._index = -1;

        int op2 = 0;
        int depth = 0;

        for (int j = 1; j < _parseResNum; j++)
        {

            jsmntok_t *h = &_tokens.get()[j];
            jsmntok_t *g = &_tokens.get()[j + 1];

            if (h->start >= bufLen || h->end >= bufLen)
                break;
            if (h->end <= h->start)
                break;

            memset(k, 0, 500);
            strncpy(k, buf + h->start, h->end - h->start);

            if (g->start < op2)
                depth++;
            else
                depth--;

            if (depth < 0)
                depth = 0;

            if ((g->type == JSMN_OBJECT || g->type == JSMN_ARRAY) && g->end < bufLen && g->start < bufLen)
                op2 = g->end;

            if (strcmp(key.c_str(), k) == 0)
            {
                if ((g->start > op2 && g->start < bufLen) || depth == 0 || !_skipChild)
                {
                    memset(v, 0, bufLen);
                    strncpy(v, buf + g->start, g->end - g->start);
                    _jsonObj.stringValue = v;
                    _jsonObj._type = g->type;
                    _jsonObj._start = g->start;
                    _jsonObj._end = g->end;
                    _jsonObj._index = j;
                    _jsonObj.success = true;  
                    setParseResultType();                 
                }

                break;
            }

            if (g->type == JSMN_ARRAY)
                j += g->size + 1;
            else
                j++;
        }

        delete[] buf;
        delete[] k;
        delete[] v;
    }
    else
        resetParseResult();

    return *this;
}

size_t FirebaseJson::getJsonObjectIteratorCount(){
  if(_parseResNum>=0) return _parseResNum;
  return 0;
}

FirebaseJson &FirebaseJson::jsonObjectiterator(size_t &index,String &key, String &value)
{
    if (_paresRes)
    {
        int bufLen = toStdString().length() + 1;
        char *k = new char[200];
        char *v = new char[bufLen];
        char *buf = new char[bufLen];
        memset(buf, 0, bufLen);
        strcpy(buf, toStdString().c_str());
        if(index == 0) index = 1;

        if(index >= 1){

            _jsonObj.success = false;
            resetParseResult();
            jsmntok_t *h = &_tokens.get()[index];
            jsmntok_t *g = &_tokens.get()[index + 1];


            memset(k, 0, 200);
            strncpy(k, buf + h->start, h->end - h->start);
            key = k;

           
            memset(v, 0, bufLen);
            strncpy(v, buf + g->start, g->end - g->start);
            value = v;

            _jsonObj.stringValue = v;
            _jsonObj._type = g->type;
            _jsonObj._start = g->start;
            _jsonObj._end = g->end;
            _jsonObj._index = index;
            _jsonObj.success = true;
           
            if (g->type == JSMN_ARRAY)
                index += g->size + 1;
            else
                index++;

             setParseResultType();        
        }
        
        delete[] buf;
        delete[] k;
        delete[] v;
    }
    
   
    return *this;
}

void FirebaseJson::resetParseResult()
{
    _jsonObj._type = 0;
    _jsonObj.type = "";
    _jsonObj.stringValue = "";
    _jsonObj.intValue = 0;
    _jsonObj.doubleValue = 0;
}

void FirebaseJson::setParseResultType()
{
    bool typeSet = false;

    char *buf = new char[20];
    memset(buf, 0, 20);
    char *tmp = new char[20];
    memset(tmp, 0, 20);

    switch (_jsonObj._type)
    {
    case JSMN_UNDEFINED:
        strcpy_P(buf, FirebaseJson_STR_19);
        break;
    case JSMN_OBJECT:
        strcpy_P(buf, FirebaseJson_STR_16);
        break;

    case JSMN_ARRAY:
        strcpy_P(buf, FirebaseJson_STR_17);
        break;

    case JSMN_STRING:
        strcpy_P(buf, FirebaseJson_STR_12);
        break;

    case JSMN_PRIMITIVE:

        strcpy_P(tmp, FirebaseJson_STR_7);
        if (strcmp(_jsonObj.stringValue.c_str(), tmp) == 0)
        {
            typeSet = true;
            strcpy_P(buf, FirebaseJson_STR_15);
        }
        else
        {
            memset(tmp, 0, 20);
            strcpy_P(tmp, FirebaseJson_STR_6);
            if (strcmp(_jsonObj.stringValue.c_str(), tmp) == 0)
            {
                typeSet = true;
                strcpy_P(buf, FirebaseJson_STR_15);
            }
        }
        
        memset(tmp, 0, 20);
        strcpy_P(tmp, FirebaseJson_STR_20);
        

        if (!typeSet && strpos(_jsonObj.stringValue.c_str(), tmp, 0) != -1)
        {
            typeSet = true;
            strcpy_P(buf, FirebaseJson_STR_14);
            _jsonObj.doubleValue = atof(_jsonObj.stringValue.c_str());
        }

        
        memset(tmp, 0, 20);
        strcpy_P(tmp, FirebaseJson_STR_18);
        
        if (!typeSet && strcmp(_jsonObj.stringValue.c_str(), tmp) == 0)
        {
            typeSet = true;
            strcpy_P(buf, FirebaseJson_STR_18);
        }

        if (!typeSet)
        {
            typeSet = true;
            double d = atof(_jsonObj.stringValue.c_str());
            if (d > 0x7fffffff)
            {
                _jsonObj.doubleValue = d;
                strcpy_P(buf, FirebaseJson_STR_14);
            }
            else
            {
                _jsonObj.intValue = atoi(_jsonObj.stringValue.c_str());
                strcpy_P(buf, FirebaseJson_STR_13);
            }
        }

        break;

    default:
        break;
    }

    _jsonObj.type = buf;

    delete[] buf;
    delete[] tmp;
}

int FirebaseJson::strpos(const char *haystack, const char *needle, int offset)
{
    size_t len =strlen(haystack);
    char *_haystack = new char[len];
    memset(_haystack,0, len);
    strncpy(_haystack, haystack + offset, strlen(haystack) - offset);
    char *p = strstr(_haystack, needle);
    int r =-1;    
    if (p) r = p - _haystack + offset;
    delete[] _haystack;
    return r;
}

int FirebaseJson::rstrpos(const char *haystack, const char *needle, int offset)
{
    size_t len =strlen(haystack);
    char *_haystack = new char[len];
    memset(_haystack,0, len);
    strncpy(_haystack, haystack + offset, len - offset);
    char *p = rstrstr(_haystack, needle);
    int r =-1;    
    if (p) r = p - _haystack + offset;
    delete[] _haystack;
    return r;
}

char *FirebaseJson::rstrstr(const char *haystack, const char *needle)
{
    size_t needle_length = strlen(needle);
    const char *haystack_end = haystack + strlen(haystack) - needle_length;
    const char *p;
    size_t i;
    for (p = haystack_end; p >= haystack; --p)
    {
        for (i = 0; i < needle_length; ++i)
        {
            if (p[i] != needle[i])
                goto next;
        }
        return (char *)p;
    next:;
    }
    return 0;
}

FirebaseJsonObject FirebaseJson::parseResult()
{
    return _jsonObj;
}

FirebaseJsonArray::FirebaseJsonArray(){};
FirebaseJsonArray::~FirebaseJsonArray()
{
    std::string().swap(_jstr);
};

FirebaseJsonArray &FirebaseJsonArray::addString(const String &value)
{
    _cCnt++;
    _json.addArrayStr(value.c_str(), value.length(), true);
    return *this;
}

FirebaseJsonArray &FirebaseJsonArray::addInt(int value)
{
    size_t bufSize = 50;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    char *b = new char[10];
    memset(b, 0, 10);
    strcat_P(b, FirebaseJson_STR_4);
    sprintf(buf, b, value);
    _cCnt++;
    _json.addArrayStr(buf, 60, false);
    delete[] b;
    delete[] buf;
    return *this;
}

FirebaseJsonArray &FirebaseJsonArray::addDouble(double value)
{
    size_t bufSize = 50;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    char *b = new char[10];
    memset(b, 0, 10);
    strcat_P(b, FirebaseJson_STR_5);
    sprintf(buf, b, value);
    _cCnt++;
    _json.addArrayStr(buf, 60, false);
    delete[] b;
    delete[] buf;
    return *this;
}

FirebaseJsonArray &FirebaseJsonArray::addBool(bool value)
{
    size_t bufSize = 20;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    if (value)
        strcat_P(buf, FirebaseJson_STR_7);
    else
        strcat_P(buf, FirebaseJson_STR_6);
    _cCnt++;
    _json.addArrayStr(buf, bufSize, false);
    delete[] buf;
    return *this;
}

FirebaseJsonArray &FirebaseJsonArray::addJson(FirebaseJson *json)
{
    _cCnt += json->getCount();
    _json.addArrayStr(json->toStdString().c_str(), json->toStdString().length(), false);
    return *this;
}

FirebaseJsonArray &FirebaseJsonArray::addArray(FirebaseJsonArray *arr)
{
    _cCnt += arr->getCount();
    _json.addArrayStr(arr->toString().c_str(), arr->toString().length(), false);
    return *this;
}

String FirebaseJsonArray::toString()
{
    return toStdString().c_str();
}

std::string FirebaseJsonArray::toStdString()
{
    return _json.toStdString(false);
}

uint32_t FirebaseJsonArray::getCount()
{
    return _cCnt;
}
