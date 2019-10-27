/*
 * FirebaseJson, version 2.2.4
 * 
 * The Easiest ESP8266/ESP32 Arduino library for parse, create and edit JSON object using relative path.
 * 
 * October 27, 2019
 * 
 * Features
 * - None recursive operations
 * - Parse and edit JSON object directly with specified relative path. 
 * - Prettify JSON string 
 * 
 * 
 * This library was implement the zserge's JSON object parser library, jasmine JSMN which available from
 * https://zserge.com/jsmn.html
 * 
 * The MIT License (MIT)
 * Copyright (c) 2019 K. Suwatchai (Mobizt)
 * Copyright (c) 2012–2018, Serge Zaitsev, zaitsev.serge@gmail.com
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

FirebaseJson::FirebaseJson()
{
    _init();
}

FirebaseJson::FirebaseJson(std::string &data)
{
    _init();
    _setJsonData(data);
}

FirebaseJson::~FirebaseJson()
{
    clear();
    _parser.reset();
    _parser.release();
    _parser = nullptr;
    _finalize();
}

void FirebaseJson::_init()
{
    _qt = new char[3];
    _tab = new char[6];
    _brk1 = new char[3];
    _brk2 = new char[3];
    _brk3 = new char[3];
    _brk4 = new char[3];
    _cm = new char[3];
    _pr2 = new char[3];
    _nl = new char[3];
    _nll = new char[6];
    _pr = new char[4];
    _pd = new char[4];
    _pf = new char[4];
    _fls = new char[7];
    _tr = new char[6];
    _string = new char[7];
    _int = new char[5];
    _dbl = new char[8];
    _bl = new char[6];
    _obj = new char[8];
    _arry = new char[7];
    _undef = new char[11];
    _dot = new char[3];
    memset(_qt, 0, 3);
    memset(_tab, 0, 6);
    memset(_brk1, 0, 3);
    memset(_brk2, 0, 3);
    memset(_brk3, 0, 3);
    memset(_brk4, 0, 3);
    memset(_cm, 0, 3);
    memset(_pr2, 0, 3);
    memset(_nl, 0, 3);
    memset(_nll, 0, 6);
    memset(_pr, 0, 4);
    memset(_pd, 0, 4);
    memset(_pf, 0, 4);
    memset(_fls, 0, 7);
    memset(_tr, 0, 6);
    memset(_string, 0, 7);
    memset(_int, 0, 5);
    memset(_dbl, 0, 8);
    memset(_bl, 0, 6);
    memset(_obj, 0, 8);
    memset(_arry, 0, 7);
    memset(_undef, 0, 11);
    memset(_dot, 0, 3);
    strcpy_P(_qt, FirebaseJson_STR_2);
    strcpy_P(_tab, FirebaseJson_STR_22);
    strcpy_P(_brk1, FirebaseJson_STR_8);
    strcpy_P(_brk2, FirebaseJson_STR_9);
    strcpy_P(_brk3, FirebaseJson_STR_10);
    strcpy_P(_brk4, FirebaseJson_STR_11);
    strcpy_P(_cm, FirebaseJson_STR_1);
    strcpy_P(_pr2, FirebaseJson_STR_3);
    strcpy_P(_nl, FirebaseJson_STR_24);
    strcpy_P(_nll, FirebaseJson_STR_18);
    strcpy_P(_pr, FirebaseJson_STR_25);
    strcpy_P(_pd, FirebaseJson_STR_4);
    strcpy_P(_pf, FirebaseJson_STR_5);
    strcpy_P(_fls, FirebaseJson_STR_6);
    strcpy_P(_tr, FirebaseJson_STR_7);
    strcpy_P(_string, FirebaseJson_STR_12);
    strcpy_P(_int, FirebaseJson_STR_13);
    strcpy_P(_dbl, FirebaseJson_STR_14);
    strcpy_P(_bl, FirebaseJson_STR_15);
    strcpy_P(_obj, FirebaseJson_STR_16);
    strcpy_P(_arry, FirebaseJson_STR_17);
    strcpy_P(_undef, FirebaseJson_STR_19);
    strcpy_P(_dot, FirebaseJson_STR_20);
}

void FirebaseJson::_finalize()
{
    delete[] _qt;
    delete[] _tab;
    delete[] _brk1;
    delete[] _brk2;
    delete[] _brk3;
    delete[] _brk4;
    delete[] _cm;
    delete[] _pr2;
    delete[] _nl;
    delete[] _nll;
    delete[] _pr;
    delete[] _pd;
    delete[] _pf;
    delete[] _fls;
    delete[] _tr;
    delete[] _string;
    delete[] _int;
    delete[] _dbl;
    delete[] _bl;
    delete[] _obj;
    delete[] _arry;
    delete[] _undef;
    delete[] _dot;
}

FirebaseJson &FirebaseJson::_setJsonData(std::string &data)
{
    return setJsonData(data.c_str());
}

FirebaseJson &FirebaseJson::setJsonData(const String &data)
{
    if (data.length() > 0)
    {
        int p1 = _strpos(data.c_str(), _brk1, 0);
        int p2 = _rstrpos(data.c_str(), _brk2, data.length() - 1);
        if (p1 != -1)
            p1 += 1;
        if (p1 != -1 && p2 != -1)
            _rawbuf = data.substring(p1, p2).c_str();
    }
    else
        _rawbuf.clear();

    return *this;
}

FirebaseJson &FirebaseJson::clear()
{
    std::string().swap(_rawbuf);
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    clearPathTk();
    _tokens.reset();
    _tokens = nullptr;
    return *this;
}

FirebaseJson &FirebaseJson::add(const String &key)
{
    _addNull(key.c_str());
    return *this;
}

FirebaseJson &FirebaseJson::add(const String &key, const String &value)
{
    _addString(key.c_str(), value.c_str());
    return *this;
}
FirebaseJson &FirebaseJson::add(const String &key, const char *value)
{
    _addString(key.c_str(), value);
    return *this;
}
FirebaseJson &FirebaseJson::add(const String &key, int value)
{
    _addInt(key.c_str(), value);
    return *this;
}

FirebaseJson &FirebaseJson::add(const String &key, unsigned short value)
{
    _addInt(key.c_str(), value);
    return *this;
}

FirebaseJson &FirebaseJson::add(const String &key, double value)
{
    _addDouble(key.c_str(), value);
    return *this;
}

FirebaseJson &FirebaseJson::add(const String &key, bool value)
{
    _addBool(key.c_str(), value);
    return *this;
}

FirebaseJson &FirebaseJson::add(const String &key, FirebaseJson &json)
{
    _addJson(key.c_str(), &json);
    return *this;
}

FirebaseJson &FirebaseJson::add(const String &key, FirebaseJsonArray &arr)
{
    _addArray(key.c_str(), &arr);
    return *this;
}

template <typename T>
FirebaseJson &FirebaseJson::add(const String &key, T value)
{
    if (std::is_same<T, int>::value)
        _addInt(key, value);
    else if (std::is_same<T, double>::value)
        _addDouble(key, value);
    else if (std::is_same<T, bool>::value)
        _addBool(key, value);
    else if (std::is_same<T, const char *>::value)
        _addString(key, value);
    else if (std::is_same<T, FirebaseJson &>::value)
        _addJson(key, &value);
    else if (std::is_same<T, FirebaseJsonArray &>::value)
        _addArray(key, &value);
    return *this;
}

void FirebaseJson::_addString(const std::string &key, const std::string &value)
{
    _add(key.c_str(), value.c_str(), key.length(), value.length(), true, true);
}

void FirebaseJson::_addInt(const std::string &key, int value)
{
    size_t bufSize = 50;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    sprintf(buf, _pd, value);
    _add(key.c_str(), buf, key.length(), 60, false, true);
    delete[] buf;
}

void FirebaseJson::_addDouble(const std::string &key, double value)
{
    size_t bufSize = 50;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    sprintf(buf, _pf, value);
    _add(key.c_str(), buf, key.length(), 60, false, true);
    delete[] buf;
}

void FirebaseJson::_addBool(const std::string &key, bool value)
{
    if (value)
        _add(key.c_str(), _tr, key.length(), 6, false, true);
    else
        _add(key.c_str(), _fls, key.length(), 7, false, true);

}

void FirebaseJson::_addNull(const std::string &key)
{
    _add(key.c_str(), _nll, key.length(), 6, false, true);
}

void FirebaseJson::_addJson(const std::string &key, FirebaseJson *json)
{
    std::string s;
    json->_toStdString(s);
    _add(key.c_str(), s.c_str(), key.length(), s.length(), false, true);
    std::string().swap(s);
}

void FirebaseJson::_addArray(const std::string &key, FirebaseJsonArray *arr)
{
    String arrStr;
    arr->toString(arrStr);
    _add(key.c_str(), arrStr.c_str(), key.length(), arrStr.length(), false, true);
}

void FirebaseJson::toString(String &buf, bool prettify)
{
    char *nbuf = new char[2];
    memset(nbuf, 0, 2);
    if (prettify)
        _parse(nbuf, PRINT_MODE_PRETTY);
    else
        _parse(nbuf, PRINT_MODE_PLAIN);
    buf = _jsonData._dbuf.c_str();
    std::string().swap(_jsonData._dbuf);
    delete[] nbuf;
}

void FirebaseJson::_toStdString(std::string &s, bool isJson)
{
    s.clear();
    size_t bufSize = 10;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    if (isJson)
        strcat(buf, _brk1);
    else
        strcat(buf, _brk3);
    s += buf;
    s += _rawbuf;
    memset(buf, 0, bufSize);
    if (isJson)
        strcat(buf, _brk2);
    else
        strcat(buf, _brk4);
    s += buf;
    delete[] buf;
}

FirebaseJson &FirebaseJson::_add(const char *key, const char *value, size_t klen, size_t vlen, bool isString, bool isJson)
{
    size_t bufSize = klen + vlen + 20;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    if (_rawbuf.length() > 0)
        strcpy_P(buf, FirebaseJson_STR_1);
    if (isJson)
    {
        strcat(buf, _qt);
        strcat(buf, key);
        strcat(buf, _qt);
        strcat_P(buf, _pr2);
    }
    if (isString)
        strcat(buf, _qt);
    strcat(buf, value);
    if (isString)
        strcat(buf, _qt);
    _rawbuf += buf;
    delete[] buf;
    return *this;
}

FirebaseJson &FirebaseJson::_addArrayStr(const char *value, size_t len, bool isString)
{
    char *nbuf = new char[2];
    memset(nbuf, 0, 2);
    _add(nbuf, value, 0, len, isString, false);
    delete[] nbuf;
    return *this;
}

bool FirebaseJson::get(FirebaseJsonData &jsonData, const String &path, bool prettify)
{
    clearPathTk();
    _strToTk(path.c_str(), _pathTk, '/');
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    if (prettify)
        _parse(path.c_str(), PRINT_MODE_PRETTY);
    else
        _parse(path.c_str(), PRINT_MODE_PLAIN);
    if (_jsonData.success)
    {
        if (_jsonData._type == JSMN_STRING && _jsonData._dbuf.c_str()[0] == '"' && _jsonData._dbuf.c_str()[_jsonData._dbuf.length() - 1] == '"')
            _jsonData.stringValue = _jsonData._dbuf.substr(1, _jsonData._dbuf.length() - 2).c_str();
        else
            _jsonData.stringValue = _jsonData._dbuf.c_str();
        jsonData = _jsonData;
    }
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    clearPathTk();
    _tokens.reset();
    _tokens = nullptr;
    return _jsonData.success;
}

size_t FirebaseJson::iteratorBegin(const char *data)
{
    if (data)
        setJsonData(data);
    _jsmn_parse(true);
    std::string s;
    _toStdString(s);
    int bufLen = s.length() + 20;
    char *buf = new char[bufLen];
    char *nbuf = new char[2];
    memset(nbuf, 0, 2);
    memset(buf, 0, bufLen);
    strcpy(buf, s.c_str());
    std::string().swap(s);
    int depth = -1;
    _collectTk = true;
    _eltk.clear();
    for (uint16_t i = 0; i < _tokenCount; i++)
        _parseToken(i, buf, depth, nbuf, -2, PRINT_MODE_NONE);
    _el.clear();
    delete[] buf;
    delete[] nbuf;
    return _eltk.size();
}

void FirebaseJson::iteratorEnd()
{
    _eltk.clear();
    clearPathTk();
    _jsonData.stringValue = "";
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    clearPathTk();
    _tokens.reset();
    _tokens = nullptr;
}

void FirebaseJson::iteratorGet(size_t index, int &type, String &key, String &value)
{
    if (_eltk.size() < index + 1)
        return;
    std::string s;
    _toStdString(s);
    int bufLen = s.length() + 20;
    char *buf = new char[bufLen];
    strcpy(buf, s.c_str());
    std::string().swap(s);
    if (_eltk[index].type == 0)
    {
        jsmntok_t *h = &_tokens.get()[_eltk[index].index];
        size_t len = h->end - h->start + 3;
        char *k = new char[len];
        memset(k, 0, len);
        strncpy(k, buf + h->start, h->end - h->start);
        jsmntok_t *g = &_tokens.get()[_eltk[index].index + 1];
        size_t len2 = g->end - g->start + 3;
        char *v = new char[len2];
        memset(v, 0, len2);
        strncpy(v, buf + g->start, g->end - g->start);
        key = k;
        value = v;
        type = JSON_OBJECT;
        delete[] k;
        delete[] v;
    }
    else if (_eltk[index].type == 1)
    {
        jsmntok_t *g = &_tokens.get()[_eltk[index].index];
        size_t len2 = g->end - g->start + 3;
        char *v = new char[len2];
        memset(v, 0, len2);
        strncpy(v, buf + g->start, g->end - g->start);
        value = v;
        memset(v, 0, len2);
        key = v;
        type = JSON_ARRAY;
        delete[] v;
    }
    delete[] buf;
}

void FirebaseJson::_jsmn_parse(bool collectTk)
{
    std::string s;
    _toStdString(s);
    int bufLen = s.length() + 20;
    char *buf = new char[bufLen];
    memset(buf, 0, bufLen);
    strcpy(buf, s.c_str());
    std::string().swap(s);
    _tokens.reset();
    _collectTk = collectTk;
    _eltk.clear();
    int cnt = jsmn_parse(_parser.get(), buf, bufLen, (jsmntok_t *)NULL, 0);
    if (cnt < 0)
    {
        int a = 0;
        int b = 0;
        for (int i = 0; i < bufLen; i++)
        {
            if (buf[i] == ',')
                a++;
            else if (buf[i] == '[' || buf[i] == '{')
                b++;
        }
        cnt = 10 + (2 * (a + 1)) + b;
    }
    _tokens = std::unique_ptr<jsmntok_t>(new jsmntok_t[cnt + 10]);
    jsmn_init(_parser.get());
    _tokenCount = jsmn_parse(_parser.get(), buf, bufLen, _tokens.get(), cnt + 10);
    _paresRes = true;
    if (_tokenCount < 0)
        _paresRes = false;
    if (_tokenCount < 1 || _tokens.get()[0].type != JSMN_OBJECT)
        _paresRes = false;
    _jsonData.success = _paresRes;
    _nextToken = 0;
    _nextDepth = 0;
    _tokenMatch = false;
    _refToken = -1;
    _resetParseResult();
    _setElementType();
    delete[] buf;
}

void FirebaseJson::_setMark(int depth, bool mark)
{
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            _el[i].omark = mark;
            break;
        }
    }
}

void FirebaseJson::_setSkip(int depth, bool skip)
{
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            _el[i].skip = skip;
            break;
        }
    }
}

void FirebaseJson::_setRef(int depth, bool ref)
{
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (ref)
        {
            if (_el[i].depth == depth - 1)
            {
                _el[i].ref = ref;
                break;
            }
        }
        else
            _el[i].ref = false;
    }
}

void FirebaseJson::_getTkIndex(int depth, tk_index_t &tk)
{
    tk.oindex = 0;
    tk.olen = 0;
    tk.omark = false;
    tk.type = JSMN_UNDEFINED;
    tk.depth = -1;
    tk.skip = false;
    tk.ref = false;
    tk.index = -1;
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            tk.index = _el[i].index;
            tk.omark = _el[i].omark;
            tk.ref = _el[i].ref;
            tk.type = _el[i].type;
            tk.depth = _el[i].depth;
            tk.oindex = _el[i].oindex;
            tk.olen = _el[i].olen;
            tk.skip = _el[i].skip;
            break;
        }
    }
}

bool FirebaseJson::_updateTkIndex(uint16_t index, int &depth, char *searchKey, int searchIndex, char *replace, PRINT_MODE printMode, bool advanceCount)
{
    int len = -1;
    bool skip = false;
    bool ref = false;
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            if (_el[i].type == JSMN_OBJECT || _el[i].type == JSMN_ARRAY)
            {
                _el[i].oindex++;
                if (_el[i].oindex >= _el[i].olen)
                {
                    depth = _el[i].depth;
                    len = _el[i].olen;
                    skip = _el[i].skip;
                    if (!_TkRefOk && _el[i].type == JSMN_OBJECT)
                        ref = _el[i].ref;
                    else if (!_TkRefOk && _el[i].type == JSMN_ARRAY && searchIndex > -1)
                        ref = _el[i].ref;
                    if (i > 0)
                        _el.erase(_el.begin() + i);
                    else
                        _el.erase(_el.begin());
                    if (printMode != PRINT_MODE_NONE && !skip)
                    {
                        if (len > 0)
                        {
                            if (ref)
                                _jsonData._dbuf += _cm;
                            if (_el[i].type == JSMN_OBJECT)
                            {
                                if (printMode == PRINT_MODE_PRETTY)
                                    _jsonData._dbuf += _nl;
                                if (printMode == PRINT_MODE_PRETTY && !ref)
                                {
                                    for (int j = 0; j < depth + 1; j++)
                                        _jsonData._dbuf += _tab;
                                }
                            }
                        }
                        if (ref)
                        {
                            if (!advanceCount)
                                _parseCompleted++;
                            if (_el[i].type == JSMN_OBJECT)
                            {
                                if (printMode == PRINT_MODE_PRETTY)
                                {
                                    for (int j = 0; j < depth + 2; j++)
                                        _jsonData._dbuf += _tab;
                                }
                                _jsonData._dbuf += _qt;
                                _jsonData._dbuf += searchKey;
                                _jsonData._dbuf += _qt;
                                if (printMode == PRINT_MODE_PRETTY)
                                    _jsonData._dbuf += _pr;
                                else
                                    _jsonData._dbuf += _pr2;
                                if (_parseCompleted == (int)_pathTk.size())
                                    _jsonData._dbuf += replace;
                                else
                                    _insertChilds(replace, printMode);
                                if (printMode == PRINT_MODE_PRETTY)
                                {
                                    _jsonData._dbuf += _nl;
                                    for (int j = 0; j < depth + 1; j++)
                                        _jsonData._dbuf += _tab;
                                }
                            }
                            else
                            {
                                for (int k = _el[i].oindex - 1; k < searchIndex; k++)
                                {
                                    if (printMode == PRINT_MODE_PRETTY)
                                    {
                                        _jsonData._dbuf += _nl;
                                        for (int j = 0; j < depth + 2; j++)
                                            _jsonData._dbuf += _tab;
                                    }
                                    if (k == searchIndex - 1)
                                    {
                                        if (_parseCompleted == (int)_pathTk.size())
                                            _jsonData._dbuf += replace;
                                        else
                                            _insertChilds(replace, printMode);
                                    }
                                    else
                                    {
                                        _jsonData._dbuf += _nll;
                                        _jsonData._dbuf += _cm;
                                    }
                                }
                            }
                            _setRef(depth, false);
                            if (!advanceCount)
                                _parseCompleted = _pathTk.size();
                        }
                        if (_el[i].type == JSMN_OBJECT)
                            _jsonData._dbuf += _brk2;
                        else
                        {
                            if (len > 0)
                            {
                                if (printMode == PRINT_MODE_PRETTY)
                                {
                                    _jsonData._dbuf += _nl;
                                    for (int j = 0; j < depth + 1; j++)
                                        _jsonData._dbuf += _tab;
                                }
                            }
                            _jsonData._dbuf += _brk4;
                        }
                    }
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

bool FirebaseJson::_updateTkIndex2(std::string &str, uint16_t index, int &depth, char *searchKey, int searchIndex, char *replace, PRINT_MODE printMode)
{
    int len = -1;
    bool skip = false;
    bool ref = false;
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            if (_el[i].type == JSMN_OBJECT || _el[i].type == JSMN_ARRAY)
            {
                _el[i].oindex++;
                if (_el[i].oindex >= _el[i].olen)
                {
                    depth = _el[i].depth;
                    len = _el[i].olen;
                    skip = _el[i].skip;
                    if (!_TkRefOk && _el[i].type == JSMN_OBJECT)
                        ref = _el[i].ref;
                    else if (!_TkRefOk && _el[i].type == JSMN_ARRAY && searchIndex > -1)
                        ref = _el[i].ref;
                    if (i > 0)
                        _el.erase(_el.begin() + i);
                    else
                        _el.erase(_el.begin());
                    if (printMode != PRINT_MODE_NONE && !skip)
                    {
                        if (len > 0)
                        {
                            if (printMode == PRINT_MODE_PRETTY)
                                str += _nl;
                            if (_el[i].type == JSMN_OBJECT)
                            {
                                if (printMode == PRINT_MODE_PRETTY && !ref)
                                {
                                    for (int j = 0; j < depth + 1; j++)
                                        str += _tab;
                                }
                            }
                            else
                            {
                                if (printMode == PRINT_MODE_PRETTY)
                                {
                                    for (int j = 0; j < depth + 1; j++)
                                        str += _tab;
                                }
                            }
                        }
                        if (ref)
                            _setRef(depth, false);
                        if (_el[i].type == JSMN_OBJECT)
                            str += _brk2;
                        else
                            str += _brk4;
                    }
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

bool FirebaseJson::_updateTkIndex3(uint16_t index, int &depth, char *searchKey, int searchIndex, PRINT_MODE printMode)
{
    int len = -1;
    bool skip = false;
    bool ref = false;
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            if (_el[i].type == JSMN_OBJECT || _el[i].type == JSMN_ARRAY)
            {
                _el[i].oindex++;
                if (_el[i].oindex >= _el[i].olen)
                {
                    depth = _el[i].depth;
                    len = _el[i].olen;
                    skip = _el[i].skip;
                    if (!_TkRefOk && _el[i].type == JSMN_OBJECT)
                        ref = _el[i].ref;
                    else if (!_TkRefOk && _el[i].type == JSMN_ARRAY && searchIndex > -1)
                        ref = _el[i].ref;
                    if (i > 0)
                        _el.erase(_el.begin() + i);
                    else
                        _el.erase(_el.begin());
                    if (depth < _skipDepth)
                        return false;
                    if (printMode != PRINT_MODE_NONE && skip)
                    {
                        if (len > 0)
                        {
                            if (printMode == PRINT_MODE_PRETTY)
                                _jsonData._dbuf += _nl;
                            if (_el[i].type == JSMN_OBJECT)
                            {
                                if (printMode == PRINT_MODE_PRETTY && !ref)
                                {
                                    for (int j = 0; j < depth + 1 - (_skipDepth + 1); j++)
                                        _jsonData._dbuf += _tab;
                                }
                            }
                            else
                            {
                                if (printMode == PRINT_MODE_PRETTY)
                                {
                                    for (int j = 0; j < depth + 1 - (_skipDepth + 1); j++)
                                        _jsonData._dbuf += _tab;
                                }
                            }
                        }
                        if (ref)
                            _setRef(depth, false);

                        if (_el[i].type == JSMN_OBJECT)
                            _jsonData._dbuf += _brk2;
                        else
                            _jsonData._dbuf += _brk4;
                    }
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

void FirebaseJson::_insertChilds(char *data, PRINT_MODE printMode)
{
    std::string str = "";
    for (int i = _pathTk.size() - 1; i > _parseCompleted - 1; i--)
    {
        if (_isArrTk(i))
        {
            std::string _str;
            _addArrNodes(_str, str, i, data, printMode);
            str = _str;
            std::string().swap(_str);
        }
        else
        {
            std::string _str;
            _addObjNodes(_str, str, i, data, printMode);
            str = _str;
            std::string().swap(_str);
        }
    }
    if ((int)_pathTk.size() == _parseCompleted)
        str = data;
    _jsonData._dbuf += str;
    std::string().swap(str);
}

void FirebaseJson::_addArrNodes(std::string &str, std::string &str2, int index, char *data, PRINT_MODE printMode)
{
   
    int i = _getArrIndex(index);
    str += _brk3;
    if (printMode == PRINT_MODE_PRETTY)
        str += _nl;
    for (int k = 0; k <= i; k++)
    {
        if (printMode == PRINT_MODE_PRETTY)
        {
            for (int j = 0; j < index + 1; j++)
                str += _tab;
        }
        if (k == i)
        {
            if (index == (int)_pathTk.size() - 1)
                str += data;
            else
                str += str2;
        }
        else
        {
            str += _nll;
            str += _cm;
        }

        if (printMode == PRINT_MODE_PRETTY)
            str += _nl;
    }

    if (printMode == PRINT_MODE_PRETTY)
    {
        for (int j = 0; j < index; j++)
            str += _tab;
    }
    str += _brk4;
}

void FirebaseJson::_addObjNodes(std::string &str, std::string &str2, int index, char *data, PRINT_MODE printMode)
{
    str += _brk1;
    if (printMode == PRINT_MODE_PRETTY)
    {
        str += _nl;
        for (int j = 0; j < index + 1; j++)
            str += _tab;
    }
    str += _qt;
    str += _pathTk[index].c_str();
    str += _qt;
    if (printMode == PRINT_MODE_PRETTY)
        str += _pr;
    else
        str += _pr2;
    if (index == (int)_pathTk.size() - 1)
        str += data;
    else
        str += str2;
    if (printMode == PRINT_MODE_PRETTY)
    {
        str += _nl;
        for (int j = 0; j < index; j++)
            str += _tab;
    }
    str += _brk2;
}

void FirebaseJson::_parseToken(uint16_t &i, char *buf, int &depth, char *searchKey, int searchIndex, PRINT_MODE printMode)
{
    tk_index_t tk;
    _getTkIndex(depth, tk);
    jsmntok_t *h = &_tokens.get()[i];
    bool oskip = false;
    bool ex = false;
    size_t resLen = _jsonData._dbuf.length();
    if (searchIndex == -2)
        tk.skip = true;
    delay(0);
    if (searchIndex > -1)
    {
        tk_index_t tk2;
        int depth2 = depth - 1;
        _getTkIndex(depth2, tk2);
        if (tk.type == JSMN_ARRAY && _parseDepth == depth && tk2.oindex == _parentIndex)
        {
            if (tk.oindex == searchIndex)
            {
                _nextToken = i;
                _nextDepth = depth;
                _parentIndex = tk.oindex;

                if ((int)_pathTk.size() != _parseDepth + 1)
                {
                    _tokenMatch = true;
                    _parseCompleted++;
                }
                else
                {
                    if (!_TkRefOk)
                    {
                        _parseCompleted++;
                        _refTkIndex = i + 1;
                        _refToken = i + 1;
                        _TkRefOk = true;
                        char *dat1 = new char[h->end - h->start + 10];
                        memset(dat1, 0, h->end - h->start + 10);
                        strncpy(dat1, buf + h->start, h->end - h->start);
                        _jsonData.stringValue = dat1;
                        delete[] dat1;
                        _jsonData._type = h->type;
                        _jsonData._k_start = h->start;
                        _jsonData._start = h->start;
                        _jsonData._end = h->end;
                        _jsonData._tokenIndex = i;
                        _jsonData._depth = depth;
                        _jsonData._len = h->size;
                        _jsonData.success = true;
                        _setElementType();
                        if (printMode != PRINT_MODE_NONE)
                            _jsonData.stringValue = "";
                        else
                        {
                            std::string().swap(_jsonData._dbuf);
                            std::string().swap(_tbuf);
                            _tokenMatch = true;
                            ex = true;
                        }
                    }
                }
            }
            else
            {
                if (tk.oindex + 1 == tk.olen)
                {
                    _setRef(depth - 1, false);
                    _setRef(depth, true);
                }
            }
        }
    }
    else
    {
        char *key = new char[h->end - h->start + 10];
        memset(key, 0, h->end - h->start + 10);
        strncpy(key, buf + h->start, h->end - h->start);
        if (tk.type != JSMN_UNDEFINED && _parseDepth == depth)
        {
            if (strcmp(searchKey, key) == 0)
            {
                _nextToken = i + 1;
                _nextDepth = depth;
                _parentIndex = tk.oindex;
                if ((int)_pathTk.size() != _parseDepth + 1)
                {
                    _tokenMatch = true;
                    _parseCompleted++;
                }
                else
                {
                    if (!_TkRefOk)
                    {
                        _parseCompleted++;
                        _refTkIndex = i + 1;
                        _refToken = i + 1;
                        _TkRefOk = true;
                        h = &_tokens.get()[i + 1];
                        char *dat2 = new char[h->end - h->start + 10];
                        memset(dat2, 0, h->end - h->start + 10);
                        strncpy(dat2, buf + h->start, h->end - h->start);
                        _jsonData.stringValue = dat2;
                        delete[] dat2;
                        _jsonData._type = h->type;
                        _jsonData._k_start = h->start;
                        _jsonData._start = h->start;
                        _jsonData._end = h->end;
                        _jsonData._tokenIndex = i;
                        _jsonData._depth = depth;
                        _jsonData._len = h->size;
                        _jsonData.success = true;
                        _setElementType();
                        if (printMode != PRINT_MODE_NONE)
                            _jsonData.stringValue = "";
                        else
                        {
                            std::string().swap(_jsonData._dbuf);
                            std::string().swap(_tbuf);
                            _tokenMatch = true;
                            ex = true;
                        }
                    }
                }
            }
            else
            {
                if (tk.oindex + 1 == tk.olen)
                {
                    _setRef(depth - 1, false);
                    _setRef(depth, true);
                }
            }
        }
        delete[] key;
    }
    if (ex)
        return;
    if (_refTkIndex == i + 1)
    {
        if (tk.type == JSMN_OBJECT)
            oskip = true;
        tk.skip = true;
        _skipDepth = depth;
    }
    h = &_tokens.get()[i];
    if (h->type == JSMN_OBJECT || h->type == JSMN_ARRAY)
    {
        if (printMode != PRINT_MODE_NONE && (tk.skip || _refTkIndex == i + 1))
        {
            if (!tk.omark && i > 0 && resLen > 0)
            {
                if (tk.oindex > 0)
                    _jsonData._dbuf += _cm;
                if (printMode == PRINT_MODE_PRETTY && h->size >= 0)
                    _jsonData._dbuf += _nl;
                if (printMode == PRINT_MODE_PRETTY && h->size >= 0)
                {
                    for (int j = 0; j < depth - (_skipDepth + 1); j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }
            }
            if (h->type == JSMN_OBJECT)
                _jsonData._dbuf += _brk1;
            else
                _jsonData._dbuf += _brk3;
        }
        el_t e;
        e.index = i;
        e.olen = h->size;
        e.type = h->type;
        e.oindex = 0;
        e.depth = depth;
        e.omark = false;
        e.ref = false;
        if (_refToken != -1)
            e.skip = true;
        else
            e.skip = tk.skip;
        _el.push_back(e);
        depth++;
        if (h->size == 0)
        {
            while (_updateTkIndex3(i, depth, searchKey, searchIndex, printMode))
            {
                delay(0);
            }
        }
    }
    else
    {
        char *tmp = new char[h->end - h->start + 10];
        memset(tmp, 0, h->end - h->start + 10);
        if (buf[h->start - 1] != '"')
            strncpy(tmp, buf + h->start, h->end - h->start);
        else
            strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
        if (h->size > 0)
        {
            if (printMode != PRINT_MODE_NONE && tk.skip && !oskip)
            {
                if (tk.oindex > 0)
                    _jsonData._dbuf += _cm;
                if (printMode == PRINT_MODE_PRETTY)
                    _jsonData._dbuf += _nl;
                if (printMode == PRINT_MODE_PRETTY && h->size > 0)
                {
                    for (int j = 0; j < depth - (_skipDepth + 1); j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }
                _jsonData._dbuf += tmp;
                if (printMode == PRINT_MODE_PRETTY)
                    _jsonData._dbuf += _pr;
                else
                    _jsonData._dbuf += _pr2;
            }
            if (_collectTk)
            {
                eltk_t el;
                el.index = i;
                el.type = 0;
                _eltk.push_back(el);
            }
            memset(tmp, 0, h->end - h->start + 10);
            strncpy(tmp, buf + h->start, h->end - h->start);
            h = &_tokens.get()[i + 1];
            if (h->type != JSMN_OBJECT && h->type != JSMN_ARRAY)
            {
                delete[] tmp;
                tmp = new char[h->end - h->start + 10];
                memset(tmp, 0, h->end - h->start + 10);
                strncpy(tmp, buf + h->start, h->end - h->start);
                if (printMode != PRINT_MODE_NONE && tk.skip)
                {
                    if (buf[h->start - 1] != '"')
                        strncpy(tmp, buf + h->start, h->end - h->start);
                    else
                        strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
                    _jsonData._dbuf += tmp;
                }
                i++;
                while (_updateTkIndex3(i, depth, searchKey, searchIndex, printMode))
                {
                    delay(0);
                }
            }
            else
            {
                if (_refToken == i + 1)
                {
                    _setSkip(depth, true);
                }
                _setMark(depth, true);
            }
        }
        else
        {
            if (printMode != PRINT_MODE_NONE && tk.skip)
            {
                if (tk.oindex > 0 && resLen > 0)
                {
                    _jsonData._dbuf += _cm;
                }
                if (printMode == PRINT_MODE_PRETTY && resLen > 0)
                    _jsonData._dbuf += _nl;

                if (printMode == PRINT_MODE_PRETTY && tk.olen > 0 && resLen > 0)
                {
                    for (int j = 0; j < depth - (_skipDepth + 1); j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }
                _jsonData._dbuf += tmp;
            }
            while (_updateTkIndex3(i, depth, searchKey, searchIndex, printMode))
            {
                delay(0);
            }
            if (_collectTk)
            {
                eltk_t el;
                el.index = i;
                el.type = 1;
                _eltk.push_back(el);
            }
        }
        delete[] tmp;

        if (_refToken == -1 && _skipDepth == depth)
            _setSkip(depth, false);
    }
    _nextToken = i + 1;
    _refToken = -1;
}

void FirebaseJson::_compileToken(uint16_t &i, char *buf, int &depth, char *searchKey, int searchIndex, PRINT_MODE printMode, char *replace, int refTokenIndex, bool removeTk)
{
    if (_tokenMatch)
        return;
    tk_index_t tk;
    _getTkIndex(depth, tk);
    jsmntok_t *h = &_tokens.get()[i];
    bool insertFlag = false;
    bool ex = false;
    delay(0);
    if (searchIndex > -1)
    {
        tk_index_t tk2;
        int depth2 = depth - 1;
        _getTkIndex(depth2, tk2);
        if (tk.type == JSMN_ARRAY && _parseDepth == depth && tk2.oindex == _parentIndex)
        {
            if (tk.oindex == searchIndex)
            {
                _nextToken = i;
                _nextDepth = depth;
                _parentIndex = tk.oindex;
                if ((int)_pathTk.size() != _parseDepth + 1)
                {
                    _tokenMatch = true;
                    _parseCompleted++;
                    _refTkIndex = i + 1;
                }
                else
                {
                    if (!_TkRefOk)
                    {
                        _parseCompleted++;
                        _refTkIndex = i + 1;
                        _refToken = i + 1;
                        _TkRefOk = true;
                        single_child_parent_t p = _findSCParent(depth);
                        if (p.success)
                        {
                            _remTkIndex = p.index + 1;
                            _remFirstTk = p.firstTk;
                            _remLastTk = p.lastTk;
                        }
                        else
                        {
                            _remTkIndex = i + 1;
                            _remFirstTk = tk.oindex == 0;
                            _remLastTk = tk.oindex + 1 == tk.olen;
                        }
                    }
                }
            }
            else
            {
                if (tk.oindex + 1 == tk.olen)
                {
                    _setRef(depth - 1, false);
                    _setRef(depth, true);
                }
            }
        }
    }
    else
    {
        char *key = new char[h->end - h->start + 10];
        memset(key, 0, h->end - h->start + 10);
        strncpy(key, buf + h->start, h->end - h->start);
        if (tk.type != JSMN_UNDEFINED && _parseDepth == depth)
        {
            if (strcmp(searchKey, key) == 0)
            {
                _nextToken = i + 1;
                _nextDepth = depth;
                _parentIndex = tk.oindex;
                if ((int)_pathTk.size() != _parseDepth + 1)
                {
                    _tokenMatch = true;
                    _parseCompleted++;
                    _refTkIndex = i + 1;
                }
                else
                {
                    if (!_TkRefOk)
                    {
                        _parseCompleted++;
                        _refTkIndex = i + 1;
                        _refToken = i + 1;
                        _TkRefOk = true;
                        single_child_parent_t p = _findSCParent(depth);
                        if (p.success)
                        {
                            _remTkIndex = p.index + 1;
                            _remFirstTk = p.firstTk;
                            _remLastTk = p.lastTk;
                        }
                        else
                        {
                            _remTkIndex = i + 1;
                            _remFirstTk = tk.oindex == 0;
                            _remLastTk = tk.oindex + 1 == tk.olen;
                        }
                    }
                }
            }
            else
            {
                if (tk.oindex + 1 == tk.olen)
                {
                    _setRef(depth - 1, false);
                    _setRef(depth, true);
                }
            }
        }
        else
        {
            if (_tokenCount == 1 && h->size == 0 && !removeTk)
            {
                _insertChilds(replace, printMode);
                _nextToken = i + 1;
                _nextDepth = 0;
                _parseCompleted = _pathTk.size();
                _tokenMatch = true;
                ex = true;
            }
        }
        delete[] key;
    }
    if (ex)
        return;

    h = &_tokens.get()[i];
    if (h->type == JSMN_OBJECT || h->type == JSMN_ARRAY)
    {
        if (printMode != PRINT_MODE_NONE && !tk.skip)
        {
            if (!tk.omark && i > 0)
            {
                if (tk.oindex > 0)
                    _jsonData._dbuf += _cm;
                if (printMode == PRINT_MODE_PRETTY && h->size >= 0)
                    _jsonData._dbuf += _nl;
                if (printMode == PRINT_MODE_PRETTY && h->size >= 0)
                {
                    for (int j = 0; j < depth; j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }
            }
            if (_refToken == -1)
            {
                if (h->type == JSMN_OBJECT)
                    _jsonData._dbuf += _brk1;
                else
                    _jsonData._dbuf += _brk3;
            }
            else if (_refToken != -1 && searchIndex > -1)
                _jsonData._dbuf += replace;
        }
        el_t e;
        e.index = i;
        e.olen = h->size;
        e.type = h->type;
        e.oindex = 0;
        e.depth = depth;
        e.omark = false;
        e.ref = false;
        if (_refToken != -1)
            e.skip = true;
        else
            e.skip = tk.skip;
        _el.push_back(e);
        depth++;
        if (h->size == 0)
        {
            while (_updateTkIndex(i, depth, searchKey, searchIndex, replace, printMode, removeTk))
            {
                delay(0);
            }
        }
    }
    else
    {
        if (_refTkIndex == refTokenIndex && refTokenIndex > -1)
        {
            _refToken = refTokenIndex;
            _refTkIndex = -1;
            insertFlag = true;
        }
        char *tmp = new char[h->end - h->start + 10];
        memset(tmp, 0, h->end - h->start + 10);
        if (buf[h->start - 1] != '"')
            strncpy(tmp, buf + h->start, h->end - h->start);
        else
            strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
        if (h->size > 0)
        {
            if (printMode != PRINT_MODE_NONE && !tk.skip)
            {
                if (tk.oindex > 0)
                    _jsonData._dbuf += _cm;
                if (printMode == PRINT_MODE_PRETTY)
                    _jsonData._dbuf += _nl;
                if (printMode == PRINT_MODE_PRETTY && h->size > 0)
                {
                    for (int j = 0; j < depth; j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }
                _jsonData._dbuf += tmp;
                if (printMode == PRINT_MODE_PRETTY)
                    _jsonData._dbuf += _pr;
                else
                    _jsonData._dbuf += _pr2;
            }
            memset(tmp, 0, h->end - h->start + 10);
            strncpy(tmp, buf + h->start, h->end - h->start);
            h = &_tokens.get()[i + 1];
            if (h->type != JSMN_OBJECT && h->type != JSMN_ARRAY)
            {
                delete[] tmp;
                tmp = new char[h->end - h->start + 10];
                memset(tmp, 0, h->end - h->start + 10);
                strncpy(tmp, buf + h->start, h->end - h->start);

                if (printMode != PRINT_MODE_NONE && !tk.skip)
                {
                    if (buf[h->start - 1] != '"')
                        strncpy(tmp, buf + h->start, h->end - h->start);
                    else
                        strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
                    if (_refToken == i + 1)
                    {
                        if (!insertFlag)
                            _jsonData._dbuf += replace;
                        else
                            _insertChilds(replace, printMode);
                    }
                    else
                        _jsonData._dbuf += tmp;
                }
                i++;
                while (_updateTkIndex(i, depth, searchKey, searchIndex, replace, printMode, removeTk))
                {
                    delay(0);
                }
            }
            else
            {
                if (_refToken == i + 1)
                {
                    _setSkip(depth, true);
                    _skipDepth = depth;
                    if (!insertFlag)
                        _jsonData._dbuf += replace;
                    else
                        _insertChilds(replace, printMode);
                    if (printMode != PRINT_MODE_NONE && (depth > 0 || tk.oindex == tk.olen - 1))
                    {
                        if (printMode == PRINT_MODE_PRETTY)
                            _jsonData._dbuf += _nl;
                        if (printMode == PRINT_MODE_PRETTY)
                        {
                            for (int j = 0; j < depth; j++)
                                _jsonData._dbuf += _tab;
                        }
                        _jsonData._dbuf += _brk2;
                    }
                }
                _setMark(depth, true);
            }
        }
        else
        {
            if (printMode != PRINT_MODE_NONE && !tk.skip)
            {
                if (tk.oindex > 0)
                    _jsonData._dbuf += _cm;
                if (printMode == PRINT_MODE_PRETTY)
                    _jsonData._dbuf += _nl;
                if (printMode == PRINT_MODE_PRETTY && tk.olen > 0)
                {
                    for (int j = 0; j < depth; j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }

                if (_refToken == i + 1)
                {
                    if (!insertFlag)
                        _jsonData._dbuf += replace;
                    else
                        _insertChilds(replace, printMode);
                }
                else
                    _jsonData._dbuf += tmp;
            }
            while (_updateTkIndex(i, depth, searchKey, searchIndex, replace, printMode, removeTk))
            {
                delay(0);
            }
        }
        delete[] tmp;

        if (_refToken == -1 && _skipDepth == depth)
            _setSkip(depth, false);
    }
    _nextToken = i + 1;
    _refToken = -1;
}

void FirebaseJson::_removeToken(uint16_t &i, char *buf, int &depth,  char *searchKey, int searchIndex, PRINT_MODE printMode, char *replace, int refTokenIndex, bool removeTk)
{
    bool ncm = false;
    tk_index_t tk;
    _getTkIndex(depth, tk);
    jsmntok_t *h = &_tokens.get()[i];
    delay(0);
    if (refTokenIndex == i && refTokenIndex > -1)
        ncm = _remFirstTk;
    if (refTokenIndex != i || (refTokenIndex == i && _remLastTk))
        _jsonData._dbuf += _tbuf;
    _tbuf.clear();
    bool flag = tk.oindex > 0 && !ncm && _jsonData._dbuf.c_str()[_jsonData._dbuf.length() - 1] != '{' && _jsonData._dbuf.c_str()[_jsonData._dbuf.length() - 1] != '[';
    if (refTokenIndex == i + 1 && refTokenIndex > -1)
    {
        _refToken = refTokenIndex;
        _refTkIndex = -1;
        tk.skip = true;
    }
    h = &_tokens.get()[i];
    if (h->type == JSMN_OBJECT || h->type == JSMN_ARRAY)
    {
        if (printMode != PRINT_MODE_NONE && !tk.skip)
        {
            if (!tk.omark && i > 0)
            {
                if (flag)
                    _tbuf += _cm;
                if (printMode == PRINT_MODE_PRETTY && h->size >= 0)
                    _tbuf += _nl;
                if (printMode == PRINT_MODE_PRETTY && h->size >= 0)
                {
                    for (int j = 0; j < depth; j++)
                        _tbuf += _tab;
                    _tbuf += _tab;
                }
            }
            if (_refToken == -1)
            {
                if (h->type == JSMN_OBJECT)
                    _tbuf += _brk1;
                else
                    _tbuf += _brk3;
            }
            else if (_refToken != -1 && searchIndex > -1)
                _tbuf += replace;
        }
        el_t e;
        e.index = i;
        e.olen = h->size;
        e.type = h->type;
        e.oindex = 0;
        e.depth = depth;
        e.omark = false;
        e.ref = false;
        if (_refToken != -1)
            e.skip = true;
        else
            e.skip = tk.skip;
        _el.push_back(e);
        depth++;
        if (h->size == 0)
        {
            while (_updateTkIndex2(_tbuf, i, depth, searchKey, searchIndex, replace, printMode))
            {
                delay(0);
            }
        }
    }
    else
    {
        char *tmp = new char[h->end - h->start + 10];
        memset(tmp, 0, h->end - h->start + 10);
        if (buf[h->start - 1] != '"')
            strncpy(tmp, buf + h->start, h->end - h->start);
        else
            strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
        if (h->size > 0)
        {
            if (printMode != PRINT_MODE_NONE && !tk.skip)
            {
                if (flag)
                    _tbuf += _cm;
                if (printMode == PRINT_MODE_PRETTY)
                    _tbuf += _nl;
                if (printMode == PRINT_MODE_PRETTY && h->size > 0)
                {
                    for (int j = 0; j < depth; j++)
                        _tbuf += _tab;
                    _tbuf += _tab;
                }
                _tbuf += tmp;
                if (printMode == PRINT_MODE_PRETTY)
                    _tbuf += _pr;
                else
                    _tbuf += _pr2;
            }
            memset(tmp, 0, h->end - h->start + 10);
            strncpy(tmp, buf + h->start, h->end - h->start);
            h = &_tokens.get()[i + 1];
            if (h->type != JSMN_OBJECT && h->type != JSMN_ARRAY)
            {
                delete[] tmp;
                tmp = new char[h->end - h->start + 10];
                memset(tmp, 0, h->end - h->start + 10);
                strncpy(tmp, buf + h->start, h->end - h->start);
                if (printMode != PRINT_MODE_NONE && !tk.skip)
                {
                    if (buf[h->start - 1] != '"')
                        strncpy(tmp, buf + h->start, h->end - h->start);
                    else
                        strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
                    _tbuf += tmp;
                }
                i++;
                while (_updateTkIndex2(_tbuf, i, depth, searchKey, searchIndex, replace, printMode))
                {
                    delay(0);
                }
            }
            else
            {
                if (_refToken == i + 1)
                {
                    _setSkip(depth, true);
                    _skipDepth = depth;
                    _tbuf += replace;
                    if (printMode != PRINT_MODE_NONE && (depth > 0 || tk.oindex == tk.olen - 1))
                    {
                        if (printMode == PRINT_MODE_PRETTY)
                            _tbuf += _nl;
                        if (printMode == PRINT_MODE_PRETTY)
                        {
                            for (int j = 0; j < depth; j++)
                                _tbuf += _tab;
                        }
                        _tbuf += _brk2;
                    }
                }
                _setMark(depth, true);
            }
        }
        else
        {
            if (printMode != PRINT_MODE_NONE && !tk.skip)
            {
                if (flag)
                    _tbuf += _cm;
                if (printMode == PRINT_MODE_PRETTY)
                    _tbuf += _nl;
                if (printMode == PRINT_MODE_PRETTY && tk.olen > 0)
                {
                    for (int j = 0; j < depth; j++)
                        _tbuf += _tab;
                    _tbuf += _tab;
                }
                _tbuf += tmp;
            }
            while (_updateTkIndex2(_tbuf, i, depth, searchKey, searchIndex, replace, printMode))
            {
                delay(0);
            }
        }
        delete[] tmp;

        if (_refToken == -1 && _skipDepth == depth)
            _setSkip(depth, false);
    }
    _nextToken = i + 1;
    _refToken = -1;
    _lastTk.olen = tk.olen;
    _lastTk.oindex = tk.oindex;
    _lastTk.type = tk.type;
    _lastTk.depth = tk.depth;
    _lastTk.index = tk.index;
    _lastTk.skip = tk.skip;
}

single_child_parent_t FirebaseJson::_findSCParent(int depth)
{
    single_child_parent_t res;
    res.index = -1;
    res.firstTk = false;
    res.lastTk = false;
    res.success = false;
    for (int i = depth; i >= 0; i--)
    {
        bool match = false;
        for (size_t j = 0; j < _el.size(); j++)
        {
            if (_el[j].depth == i - 1 && _el[i].olen == 1)
            {
                match = true;
                res.index = _el[i].index;
                res.firstTk = _el[j].oindex == 0;
                res.lastTk = _el[j].oindex + 1 == _el[j].olen;
                res.success = true;
            }
        }
        if (!match)
            break;
    }
    return res;
}

void FirebaseJson::_get(const char *key, int depth, int index)
{
    _tokenMatch = false;
    if (_paresRes)
    {
        std::string s;
        _toStdString(s);
        int bufLen = s.length() + 20;
        char *buf = new char[bufLen];
        memset(buf, 0, bufLen);
        strcpy(buf, s.c_str());
        std::string().swap(s);

        if (_jsonData.success)
        {
            _jsonData._dbuf.clear();
            _parseDepth = depth;
            if (_nextToken < 0)
                _nextToken = 0;
            for (uint16_t i = _nextToken; i < _tokenCount; i++)
            {
                _parseToken(i, buf, _nextDepth, (char *)key, index, PRINT_MODE_NONE);
                if (_tokenMatch)
                    break;
            }
        }
        delete[] buf;
        if (!_tokenMatch)
        {
            _paresRes = false;
            _jsonData.success = false;
            _resetParseResult();
        }
    }
}

void FirebaseJson::_strToTk(const std::string &str, std::vector<std::string> &cont, char delim)
{
    std::size_t current, previous = 0;
    current = str.find(delim);
    std::string s;
    while (current != std::string::npos)
    {
        s = str.substr(previous, current - previous);
        _trim(s);
        if (s.length() > 0)
            cont.push_back(s);
        previous = current + 1;
        current = str.find(delim, previous);
        delay(0);
    }
    s = str.substr(previous, current - previous);
    _trim(s);
    if (s.length() > 0)
        cont.push_back(s);
    std::string().swap(s);
}

void FirebaseJson::_ltrim(std::string &str, const std::string &chars)
{
    str.erase(0, str.find_first_not_of(chars));
}

void FirebaseJson::_rtrim(std::string &str, const std::string &chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
}

void FirebaseJson::_trim(std::string &str, const std::string &chars)
{
    _ltrim(str, chars);
    _rtrim(str, chars);
}

void FirebaseJson::_parse(const char *path, PRINT_MODE printMode)
{
    clearPathTk();
    _strToTk(path, _pathTk, '/');
    _jsmn_parse();
    if (!_jsonData.success)
        return;
    _jsonData.success = false;
    char *nbuf = new char[2];
    memset(nbuf, 0, 2);
    int len = _pathTk.size();
    _nextDepth = -1;
    _nextToken = 0;
    _skipDepth = -1;
    _parentIndex = -1;
    _TkRefOk = false;
    _parseCompleted = 0;
    _refTkIndex = -1;
    _remTkIndex = -1;
    _remFirstTk = false;
    _remLastTk = false;
    _el.clear();
    _eltk.clear();
    if (len == 0)
    {
        _parse(nbuf, 0, -2, printMode);
        _jsonData.success = true;
    }
    else
    {
        for (int i = 0; i < len; i++)
        {
            if (_isStrTk(i))
                _parse(_pathTk[i].c_str(), i, -1, printMode);
            else if (_isArrTk(i))
                _parse(nbuf, i, _getArrIndex(i), printMode);
            else
                _parse(_pathTk[i].c_str(), i, -1, printMode);
        }
        _jsonData.success = _parseCompleted == len;
    }
    _el.clear();
    _eltk.clear();
    delete[] nbuf;
    clearPathTk();
    std::string().swap(_tbuf);
    _tokens.reset();
    _tokens = nullptr;
}

void FirebaseJson::clearPathTk()
{
    size_t len = _pathTk.size();
    for (size_t i = 0; i < len; i++)
        std::string().swap(_pathTk[i]);
    for (size_t i = 0; i < len; i++)
        _pathTk.erase(_pathTk.end());
    _pathTk.clear();
    std::vector<std::string>().swap(_pathTk);
}

void FirebaseJson::_parse(const char *key, int depth, int index, PRINT_MODE printMode)
{
    _tokenMatch = false;
    if (_paresRes)
    {
        std::string s;
        _toStdString(s);
        int bufLen = s.length() + 20;
        char *buf = new char[bufLen];
        memset(buf, 0, bufLen);
        strcpy(buf, s.c_str());
        std::string().swap(s);
        _parseDepth = depth;
        if (_nextToken < 0)
            _nextToken = 0;
        for (uint16_t i = _nextToken; i < _tokenCount; i++)
        {
            _parseToken(i, buf, _nextDepth, (char *)key, index, printMode);
            if (_tokenMatch)
                break;
        }

        delete[] buf;
        if (!_tokenMatch)
        {
            _paresRes = false;
            _jsonData.success = false;
        }
    }
}

void FirebaseJson::_compile(const char *key, int depth, int index, const char *replace, PRINT_MODE printMode, int refTokenIndex, bool removeTk)
{
    _tokenMatch = false;
    if (_paresRes)
    {
        std::string s;
        _toStdString(s);
        int bufLen = s.length() + 20;
        char *buf = new char[bufLen];
        memset(buf, 0, bufLen);
        strcpy(buf, s.c_str());
        std::string().swap(s);
        _parseDepth = depth;
        if (_nextToken < 0)
            _nextToken = 0;
        for (uint16_t i = _nextToken; i < _tokenCount; i++)
        {
            _compileToken(i, buf, _nextDepth, (char *)key, index, printMode, (char *)replace, refTokenIndex, removeTk);
            if (_tokenMatch)
                break;
        }
        delete[] buf;
        if (!_tokenMatch)
        {
            _paresRes = false;
            _jsonData.success = false;
        }
    }
}

void FirebaseJson::_remove(const char *key, int depth, int index, const char *replace, int refTokenIndex, bool removeTk)
{
    if (_paresRes)
    {
        std::string s;
        _toStdString(s);
        int bufLen = s.length() + 20;
        char *buf = new char[bufLen];
        memset(buf, 0, bufLen);
        strcpy(buf, s.c_str());
        std::string().swap(s);
        _parseDepth = depth;
        if (_nextToken < 0)
            _nextToken = 0;
        for (uint16_t i = _nextToken; i < _tokenCount; i++)
        {
            _removeToken(i, buf, _nextDepth, (char *)key, index, PRINT_MODE_PLAIN, (char *)replace, refTokenIndex, removeTk);
        }
        delete[] buf;
    }
}

bool FirebaseJson::_isArrTk(int index)
{
    if (index < (int)_pathTk.size())
        return _pathTk[index].c_str()[0] == '[' && _pathTk[index].c_str()[_pathTk[index].length() - 1] == ']';
    else
        return false;
}
bool FirebaseJson::_isStrTk(int index)
{
    if (index < (int)_pathTk.size())
        return _pathTk[index].c_str()[0] == '"' && _pathTk[index].c_str()[_pathTk[index].length() - 1] == '"';
    else
        return false;
}

int FirebaseJson::_getArrIndex(int index)
{
    int res = -1;
    if (index < (int)_pathTk.size())
    {
        res = atoi(_pathTk[index].substr(1, _pathTk[index].length() - 2).c_str());
        if (res < 0)
            res = 0;
    }
    return res;
}

void FirebaseJson::set(const String &path)
{
    _setNull(path.c_str());
}

void FirebaseJson::set(const String &path, const String &value)
{
    _setString(path.c_str(), value.c_str());
}

void FirebaseJson::set(const String &path, const char *value)
{
    _setString(path.c_str(), value);
}

void FirebaseJson::set(const String &path, int value)
{
    _setInt(path.c_str(), value);
}

void FirebaseJson::set(const String &path, unsigned short value)
{
    _setInt(path.c_str(), value);
}

void FirebaseJson::set(const String &path, double value)
{
    _setDouble(path.c_str(), value);
}

void FirebaseJson::set(const String &path, bool value)
{
    _setBool(path.c_str(), value);
}

void FirebaseJson::set(const String &path, FirebaseJson &json)
{
    _setJson(path.c_str(), &json);
}

void FirebaseJson::set(const String &path, FirebaseJsonArray &arr)
{
    _setArray(path.c_str(), &arr);
}

template <typename T>
bool FirebaseJson::set(const String &path, T value)
{
    if (std::is_same<T, int>::value)
        return _setInt(path, value);
    else if (std::is_same<T, double>::value)
        return _setDouble(path, value);
    else if (std::is_same<T, bool>::value)
        return _setBool(path, value);
    else if (std::is_same<T, const char *>::value)
        return _setString(path, value);
    else if (std::is_same<T, FirebaseJson &>::value)
        return _setJson(path, &value);
    else if (std::is_same<T, FirebaseJsonArray &>::value)
        return _setArray(path, &value);
}

void FirebaseJson::_setString(const std::string &path, const std::string &value)
{
    char *tmp = new char[value.length() + 20];
    memset(tmp, 0, value.length() + 20);
    strcpy(tmp, _qt);
    strcat(tmp, value.c_str());
    strcat(tmp, _qt);
    _set(path.c_str(), tmp);
    delete[] tmp;
    std::string().swap(_jsonData._dbuf);
}

void FirebaseJson::_setInt(const std::string &path, int value)
{
    char *tmp = new char[50];
    memset(tmp, 0, 50);
    sprintf(tmp, _pd, value);
    _set(path.c_str(), tmp);
    delete[] tmp;
    std::string().swap(_jsonData._dbuf);
}

void FirebaseJson::_setDouble(const std::string &path, double value)
{
    char *tmp = new char[50];
    memset(tmp, 0, 50);
    sprintf(tmp, _pf, value);
    int i = strlen(tmp) - 1;
    while (tmp[i] == '0' && i > 0)
        i--;
    if (i < (int)strlen(tmp) - 1)
        tmp[i + 1] = '\0';
    _set(path.c_str(), tmp);
    delete[] tmp;
    std::string().swap(_jsonData._dbuf);
}

void FirebaseJson::_setBool(const std::string &path, bool value)
{
    if (value)
        _set(path.c_str(), _tr);
    else
        _set(path.c_str(), _fls);
    std::string().swap(_jsonData._dbuf);
}

void FirebaseJson::_setNull(const std::string &path)
{
    _set(path.c_str(), _nll);
    std::string().swap(_jsonData._dbuf);
}

void FirebaseJson::_setJson(const std::string &path, FirebaseJson *json)
{
    std::string s;
    json->_toStdString(s);
    _set(path.c_str(), s.c_str());
    std::string().swap(s);
}

void FirebaseJson::_setArray(const std::string &path, FirebaseJsonArray *arr)
{
    std::string s;
    arr->_toStdString(s);
    _set(path.c_str(), s.c_str());
    std::string().swap(s);
}

void FirebaseJson::_set(const char *path, const char *data)
{
    clearPathTk();
    _strToTk(path, _pathTk, '/');
    _jsmn_parse();
    if (!_jsonData.success)
        return;
    _jsonData.success = false;
    char *nbuf = new char[2];
    memset(nbuf, 0, 2);
    int len = _pathTk.size();
    _nextDepth = -1;
    _nextToken = 0;
    _skipDepth = -1;
    _parentIndex = -1;
    _TkRefOk = false;
    _parseCompleted = 0;
    _refTkIndex = -1;
    _remTkIndex = -1;
    _remFirstTk = false;
    _remLastTk = false;
    _el.clear();
    _eltk.clear();
    for (int i = 0; i < len; i++)
    {
        if (_isStrTk(i))
            _compile(_pathTk[i].c_str(), i, -1, data, PRINT_MODE_PLAIN);
        else if (_isArrTk(i))
            _compile(nbuf, i, _getArrIndex(i), data, PRINT_MODE_PLAIN);
        else
            _compile(_pathTk[i].c_str(), i, -1, data, PRINT_MODE_PLAIN);
    }
    _el.clear();
    _eltk.clear();
    if (_parseCompleted != len)
    {
        std::string().swap(_jsonData._dbuf);
        std::string().swap(_tbuf);
        int refTokenIndex = _refTkIndex;
        _nextDepth = -1;
        _nextToken = 0;
        _skipDepth = -1;
        _parentIndex = -1;
        _TkRefOk = false;
        _parseCompleted = 0;
        _refTkIndex = -1;
        _tokenMatch = false;
        _paresRes = true;
        for (int i = 0; i < len; i++)
        {
            if (_isStrTk(i))
                _compile(_pathTk[i].c_str(), i, -1, data, PRINT_MODE_PLAIN, refTokenIndex);
            else if (_isArrTk(i))
                _compile(nbuf, i, _getArrIndex(i), data, PRINT_MODE_PLAIN, refTokenIndex);
            else
                _compile(_pathTk[i].c_str(), i, -1, data, PRINT_MODE_PLAIN, refTokenIndex);
        }
        _el.clear();
        _eltk.clear();
    }
    delete[] nbuf;
    if (_jsonData._dbuf.length() >= 2)
    {
        _jsonData.success = true;
        _rawbuf = _jsonData._dbuf.substr(1, _jsonData._dbuf.length() - 2);
    }
    else
        _rawbuf.clear();
    clearPathTk();
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    _tokens.reset();
    _tokens = nullptr;
}

bool FirebaseJson::remove(const String &path)
{
    clearPathTk();
    _strToTk(path.c_str(), _pathTk, '/');
    _jsmn_parse();
    if (!_jsonData.success)
        return false;
    _jsonData.success = false;
    char *nbuf = new char[2];
    memset(nbuf, 0, 2);
    int len = _pathTk.size();
    _nextDepth = -1;
    _nextToken = 0;
    _skipDepth = -1;
    _parentIndex = -1;
    _TkRefOk = false;
    _parseCompleted = 0;
    _refTkIndex = -1;
    _remTkIndex = -1;
    _remFirstTk = false;
    _remLastTk = false;
    _el.clear();
    _eltk.clear();
    for (int i = 0; i < len; i++)
    {
        if (_isStrTk(i))
            _compile(_pathTk[i].c_str(), i, -1, nbuf, PRINT_MODE_NONE, -1, true);
        else if (_isArrTk(i))
            _compile(nbuf, i, _getArrIndex(i), nbuf, PRINT_MODE_NONE, -1, true);
        else
            _compile(_pathTk[i].c_str(), i, -1, nbuf, PRINT_MODE_NONE, -1, true);
    }
    _el.clear();
    _eltk.clear();
    std::string().swap(_jsonData._dbuf);
    int refTokenIndex = _remTkIndex;
    if (_parseCompleted == len)
    {
        _nextDepth = -1;
        _nextToken = 0;
        _skipDepth = -1;
        _parentIndex = -1;
        _TkRefOk = false;
        _parseCompleted = 0;
        _refTkIndex = -1;
        _tokenMatch = false;
        _paresRes = true;
        _jsonData.success = true;
        _lastTk.skip = false;
        _lastTk.olen = 0;
        _lastTk.oindex = 0;
        if (_isStrTk(len - 1))
            _remove(_pathTk[len - 1].c_str(), -1, -1, nbuf, refTokenIndex, true);
        else
            _remove(nbuf, -1, _getArrIndex(len - 1), nbuf, refTokenIndex, true);
        _jsonData._dbuf += _tbuf;
        _el.clear();
        _eltk.clear();
    }

    delete[] nbuf;
    if (_jsonData._dbuf.length() >= 2)
        _rawbuf = _jsonData._dbuf.substr(1, _jsonData._dbuf.length() - 2);
    else
        _rawbuf.clear();
    clearPathTk();
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    _tokens.reset();
    _tokens = nullptr;
    return _jsonData.success;
}

void FirebaseJson::_resetParseResult()
{
    _jsonData._type = 0;
    _jsonData.type = "";
    _jsonData.typeNum = 0;
    _jsonData.stringValue = "";
    _jsonData._dbuf = "";
    _jsonData.intValue = 0;
    _jsonData.doubleValue = 0;
    _jsonData.boolValue = false;
}

void FirebaseJson::_setElementType()
{
    bool typeSet = false;
    char *buf = new char[20];
    memset(buf, 0, 20);
    char *tmp = new char[20];
    memset(tmp, 0, 20);
    char *tmp2 = nullptr;
    if (_jsonData._type == JSMN_PRIMITIVE)
    {
        tmp2 = new char[_jsonData.stringValue.length() + 1];
        memset(tmp2, 0, _jsonData.stringValue.length() + 1);
        strcpy(tmp2, _jsonData.stringValue.c_str());
    }
    switch (_jsonData._type)
    {
    case JSMN_UNDEFINED:
        strcpy(buf, _undef);
        _jsonData.typeNum = JSON_UNDEFINED;
        break;
    case JSMN_OBJECT:
        strcpy(buf, _obj);
        _jsonData.typeNum = JSON_OBJECT;
        break;
    case JSMN_ARRAY:
        strcpy(buf, _arry);
        _jsonData.typeNum = JSON_ARRAY;
        break;
    case JSMN_STRING:
        strcpy(buf, _string);
        _jsonData.typeNum = JSON_STRING;
        break;
    case JSMN_PRIMITIVE:
        if (!typeSet && strcmp(tmp2, _tr) == 0)
        {
            typeSet = true;
            strcpy(buf, _bl);
            _jsonData.typeNum = JSON_BOOL;
            _jsonData.boolValue = true;
            _jsonData.doubleValue = 1.0;
            _jsonData.intValue = 1;
        }
        else
        {
            if (!typeSet && strcmp(tmp2, _fls) == 0)
            {
                typeSet = true;
                strcpy(buf, _bl);
                _jsonData.typeNum = JSON_BOOL;
                _jsonData.boolValue = false;
                _jsonData.doubleValue = 0.0;
                _jsonData.intValue = 0;
            }
        }
        strcpy(tmp, _dot);
        if (!typeSet && _strpos(tmp2, tmp, 0) > -1)
        {
            typeSet = true;
            strcpy(buf, _dbl);
            _jsonData.typeNum = JSON_DOUBLE;
            _jsonData.doubleValue = atof(tmp2);
            _jsonData.intValue = atoi(tmp2);
            _jsonData.boolValue = atof(tmp2) > 0 ? true : false;
        }
        if (!typeSet && strcmp(tmp2, _nll) == 0)
        {
            typeSet = true;
            strcpy(buf, _nll);
            _jsonData.typeNum = JSON_NULL;
        }
        if (!typeSet)
        {
            typeSet = true;
            double d = atof(tmp2);
            if (d > 0x7fffffff)
            {
                strcpy(buf, _dbl);
                _jsonData.doubleValue = d;
                _jsonData.intValue = atoi(tmp2);
                _jsonData.boolValue = atof(tmp2) > 0 ? true : false;
                _jsonData.typeNum = JSON_DOUBLE;
            }
            else
            {
                _jsonData.intValue = atoi(tmp2);
                _jsonData.doubleValue = atof(tmp2);
                _jsonData.boolValue = atof(tmp2) > 0 ? true : false;
                strcpy(buf, _int);
                _jsonData.typeNum = JSON_INT;
            }
        }
        break;
    default:
        break;
    }
    _jsonData.type = buf;
    delete[] buf;
    delete[] tmp;
    if (tmp2)
        delete[] tmp2;
}

int FirebaseJson::_strpos(const char *haystack, const char *needle, int offset)
{
    size_t len = strlen(haystack);
    char *_haystack = new char[len];
    memset(_haystack, 0, len);
    strncpy(_haystack, haystack + offset, strlen(haystack) - offset);
    char *p = strstr(_haystack, needle);
    int r = -1;
    if (p)
        r = p - _haystack + offset;
    delete[] _haystack;
    return r;
}

int FirebaseJson::_rstrpos(const char *haystack, const char *needle, int offset)
{
    size_t len = strlen(haystack);
    char *_haystack = new char[len];
    memset(_haystack, 0, len);
    strncpy(_haystack, haystack + offset, len - offset);
    char *p = _rstrstr(_haystack, needle);
    int r = -1;
    if (p)
        r = p - _haystack + offset;
    delete[] _haystack;
    return r;
}

char *FirebaseJson::_rstrstr(const char *haystack, const char *needle)
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

FirebaseJsonArray::FirebaseJsonArray()
{
    _init();
}
FirebaseJsonArray::~FirebaseJsonArray()
{
    _finalize();
    std::string().swap(_jbuf);
};

void FirebaseJsonArray::_init()
{
    _pd = new char[4];
    _pf = new char[4];
    _fls = new char[7];
    _tr = new char[6];
    _brk3 = new char[3];
    _brk4 = new char[3];
    _nll = new char[6];
    _root = new char[9];
    _root2 = new char[6];
    _qt = new char[3];
    _slash = new char[3];
    memset(_pd, 0, 4);
    memset(_pf, 0, 4);
    memset(_fls, 0, 7);
    memset(_tr, 0, 6);
    memset(_brk3, 0, 3);
    memset(_brk4, 0, 3);
    memset(_nll, 0, 6);
    memset(_root, 0, 9);
    memset(_root2, 0, 6);
    memset(_qt, 0, 3);
    memset(_slash, 0, 3);

    strcpy_P(_pd, FirebaseJson_STR_4);
    strcpy_P(_pf, FirebaseJson_STR_5);
    strcpy_P(_fls, FirebaseJson_STR_6);
    strcpy_P(_tr, FirebaseJson_STR_7);
    strcpy_P(_brk3, FirebaseJson_STR_10);
    strcpy_P(_brk4, FirebaseJson_STR_11);
    strcpy_P(_nll, FirebaseJson_STR_18);
    strcpy_P(_root, FirebaseJson_STR_21);
    strcpy_P(_root2, FirebaseJson_STR_26);
    strcpy_P(_qt, FirebaseJson_STR_2);
    strcpy_P(_slash, FirebaseJson_STR_27);
}

void FirebaseJsonArray::_finalize()
{
    delete[] _pd;
    delete[] _pf;
    delete[] _fls;
    delete[] _tr;
    delete[] _brk3;
    delete[] _brk4;
    delete[] _nll;
    delete[] _root;
    delete[] _root2;
    delete[] _qt;
    delete[] _slash;
}

FirebaseJsonArray &FirebaseJsonArray::add()
{
    _addNull();
    return *this;
}

FirebaseJsonArray &FirebaseJsonArray::add(const String &value)
{
    _addString(value.c_str());
    return *this;
}
FirebaseJsonArray &FirebaseJsonArray::add(const char *value)
{
    _addString(value);
    return *this;
}
FirebaseJsonArray &FirebaseJsonArray::add(int value)
{
    _addInt(value);
    return *this;
}

FirebaseJsonArray &FirebaseJsonArray::add(unsigned short value)
{
    _addInt(value);
    return *this;
}

FirebaseJsonArray &FirebaseJsonArray::add(double value)
{
    _addDouble(value);
    return *this;
}

FirebaseJsonArray &FirebaseJsonArray::add(bool value)
{
    _addBool(value);
    return *this;
}

FirebaseJsonArray &FirebaseJsonArray::add(FirebaseJson &json)
{
    _addJson(&json);
    return *this;
}

FirebaseJsonArray &FirebaseJsonArray::add(FirebaseJsonArray &arr)
{
    _addArray(&arr);
    return *this;
}

template <typename T>
FirebaseJsonArray &FirebaseJsonArray::add(T value)
{
    if (std::is_same<T, int>::value)
        _addInt(value);
    else if (std::is_same<T, double>::value)
        _addDouble(value);
    else if (std::is_same<T, bool>::value)
        _addBool(value);
    else if (std::is_same<T, const char *>::value)
        _addString(value);
    else if (std::is_same<T, FirebaseJson &>::value)
        _addJson(&value);
    else if (std::is_same<T, FirebaseJsonArray &>::value)
        _addArray(&value);
    return *this;
}

void FirebaseJsonArray::_addString(const std::string &value)
{
    _arrLen++;
    _json._addArrayStr(value.c_str(), value.length(), true);
}

void FirebaseJsonArray::_addInt(int value)
{
    _arrLen++;
    size_t bufSize = 50;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    sprintf(buf, _pd, value);
    _json._addArrayStr(buf, 60, false);
    delete[] buf;
}

void FirebaseJsonArray::_addDouble(double value)
{
    _arrLen++;
    size_t bufSize = 50;
    char *buf = new char[bufSize];
    memset(buf, 0, bufSize);
    sprintf(buf, _pf, value);
    _json._addArrayStr(buf, 60, false);
    delete[] buf;
}

void FirebaseJsonArray::_addBool(bool value)
{
    _arrLen++;
    if (value)
        _json._addArrayStr(_tr, 6, false);
    else
        _json._addArrayStr(_fls, 7, false);
}

void FirebaseJsonArray::_addNull()
{
    _arrLen++;
    _json._addArrayStr(_nll, 6, false);
}

void FirebaseJsonArray::_addJson(FirebaseJson *json)
{
    _arrLen++;
    std::string s;
    json->_toStdString(s);
    _json._addArrayStr(s.c_str(), s.length(), false);
    std::string().swap(s);
}

void FirebaseJsonArray::_addArray(FirebaseJsonArray *arr)
{
    _arrLen++;
    String arrStr;
    arr->toString(arrStr);
    _json._addArrayStr(arrStr.c_str(), arrStr.length(), false);
}

bool FirebaseJsonArray::get(FirebaseJsonData &jsonData, const String &path)
{
    return _get(jsonData, path.c_str());
}

bool FirebaseJsonArray::get(FirebaseJsonData &jsonData, int index)
{
    char *tmp = new char[50];
    std::string path = "";
    path += _brk3;
    memset(tmp, 0, 50);
    itoa(index, tmp, 10);
    path += tmp;
    path += _brk4;
    bool ret = _get(jsonData, path.c_str());
    std::string().swap(path);
    delete[] tmp;
    return ret;
}

bool FirebaseJsonArray::_get(FirebaseJsonData &jsonData, const char *path)
{
    _json._toStdString(_jbuf, false);
    _json._rawbuf = _root;
    _json._rawbuf += _jbuf;
    std::string path2 = _root2;
    path2 += _slash;
    path2 += path;
    _json.clearPathTk();
    _json._strToTk(path2.c_str(), _json._pathTk, '/');
    if (!_json._isArrTk(1))
    {
        _json._jsonData.success = false;
        goto ex_;
    }
    if (_json._getArrIndex(1) < 0)
    {
        _json._jsonData.success = false;
        goto ex_;
    }
    _json._parse(path2.c_str(), PRINT_MODE_NONE);
    if (_json._jsonData.success)
    {
        _json._rawbuf = _jbuf.substr(1, _jbuf.length() - 2).c_str();
        if (_json._jsonData._type == JSMN_STRING && _json._jsonData.stringValue.c_str()[0] == '"' && _json._jsonData.stringValue.c_str()[_json._jsonData.stringValue.length() - 1] == '"')
            _json._jsonData.stringValue = _json._jsonData.stringValue.substring(1, _json._jsonData.stringValue.length() - 1).c_str();
        jsonData = _json._jsonData;
    }
ex_:
    _json.clearPathTk();
    _json._tokens.reset();
    _json._tokens = nullptr;
    return _json._jsonData.success;
}

size_t FirebaseJsonArray::size()
{
    return _arrLen;
}

void FirebaseJsonArray::toString(String &buf, bool prettify)
{
    char *tmp = new char[20];
    memset(tmp, 0, 20);
    std::string().swap(_json._jsonData._dbuf);
    std::string().swap(_json._tbuf);
    _json._toStdString(_jbuf, false);
    _json._rawbuf = _root;
    _json._rawbuf += _jbuf;
    if (prettify)
        _json._parse(_root2, PRINT_MODE_PRETTY);
    else
        _json._parse(_root2, PRINT_MODE_PLAIN);
    std::string().swap(_json._tbuf);
    std::string().swap(_jbuf);
    _json.clearPathTk();
    _json._tokens.reset();
    _json._tokens = nullptr;
    delete[] tmp;
    _json._rawbuf = _json._jsonData._dbuf.substr(1, _json._jsonData._dbuf.length() - 2);
    buf = _json._jsonData._dbuf.c_str();
    std::string().swap(_json._jsonData._dbuf);
}

FirebaseJsonArray &FirebaseJsonArray::clear()
{
    _json.clear();
    std::string().swap(_jbuf);
    return *this;
}
void FirebaseJsonArray::_set2(int index, const char *value, bool isStr)
{
    char *tmp = new char[50];
    std::string path = _brk3;
    memset(tmp, 0, 50);
    itoa(index, tmp, 10);
    path += tmp;
    path += _brk4;
    _set(path.c_str(), value, isStr);
    std::string().swap(path);
    delete[] tmp;
}

void FirebaseJsonArray::_set(const char *path, const char *value, bool isStr)
{
    _json._jsonData.success = false;
    _json._toStdString(_jbuf, false);
    _json._rawbuf = _root;
    _json._rawbuf += _jbuf;
    char *tmp2 = new char[strlen(value) + 10];
    memset(tmp2, 0, strlen(value) + 10);
    if (isStr)
        strcpy_P(tmp2, _qt);
    strcat(tmp2, value);
    if (isStr)
        strcat_P(tmp2, _qt);
    std::string path2 = _root2;
    path2 += _slash;
    path2 += path;
    _json.clearPathTk();
    _json._strToTk(path2, _json._pathTk, '/');
    if (!_json._isArrTk(1))
        goto ex_2;
    if (_json._getArrIndex(1) < 0)
        goto ex_2;
    _json._set(path2.c_str(), tmp2);
    delete[] tmp2;
    std::string().swap(path2);
    if (_json._jsonData.success)
    {
        std::string().swap(_json._jsonData._dbuf);
        std::string().swap(_json._tbuf);
        _json._parse(_root2, PRINT_MODE_PLAIN);
        if (_json._jsonData.success)
        {
            _arrLen = _json._jsonData._len;
            _json._rawbuf = _json._jsonData._dbuf.substr(1, _json._jsonData._dbuf.length() - 2);
        }
    }
    else
        _json._rawbuf = _jbuf.substr(1, _jbuf.length() - 2);
ex_2:
    std::string().swap(_json._jsonData._dbuf);
    std::string().swap(_json._tbuf);
    std::string().swap(_jbuf);
    _json.clearPathTk();
    _json._tokens.reset();
    _json._tokens = nullptr;
}

void FirebaseJsonArray::set(int index)
{
    return _setNull(index);
}

void FirebaseJsonArray::set(const String &path)
{
    _setNull(path);
}

void FirebaseJsonArray::set(int index, const String &value)
{
    _setString(index, value.c_str());
}

void FirebaseJsonArray::set(const String &path, const String &value)
{
    _setString(path, value.c_str());
}

void FirebaseJsonArray::set(int index, const char *value)
{
    _setString(index, value);
}

void FirebaseJsonArray::set(const String &path, const char *value)
{
    _setString(path, value);
}

void FirebaseJsonArray::set(int index, int value)
{
    _setInt(index, value);
}

void FirebaseJsonArray::set(int index, unsigned short value)
{
    _setInt(index, value);
}

void FirebaseJsonArray::set(const String &path, int value)
{
    _setInt(path, value);
}

void FirebaseJsonArray::set(const String &path, unsigned short value)
{
    _setInt(path, value);
}

void FirebaseJsonArray::set(int index, double value)
{
    _setDouble(index, value);
}

void FirebaseJsonArray::set(const String &path, double value)
{
    _setDouble(path, value);
}

void FirebaseJsonArray::set(int index, bool value)
{
    _setBool(index, value);
}

void FirebaseJsonArray::set(const String &path, bool value)
{
    _setBool(path, value);
}

void FirebaseJsonArray::set(int index, FirebaseJson &json)
{
    _setJson(index, &json);
}

void FirebaseJsonArray::set(const String &path, FirebaseJson &json)
{
    _setJson(path, &json);
}

void FirebaseJsonArray::set(int index, FirebaseJsonArray &arr)
{
    _setArray(index, &arr);
}

void FirebaseJsonArray::set(const String &path, FirebaseJsonArray &arr)
{
    _setArray(path, &arr);
}

template <typename T>
void FirebaseJsonArray::set(int index, T value)
{
    if (std::is_same<T, int>::value)
        _setInt(index, value);
    else if (std::is_same<T, double>::value)
        _setDouble(index, value);
    else if (std::is_same<T, bool>::value)
        _setBool(index, value);
    else if (std::is_same<T, const char *>::value)
        _setString(index, value);
    else if (std::is_same<T, FirebaseJson &>::value)
        _setJson(index, &value);
    else if (std::is_same<T, FirebaseJsonArray &>::value)
        _setArray(index, &value);
}

template <typename T>
void FirebaseJsonArray::set(const String &path, T value)
{
    if (std::is_same<T, int>::value)
        _setInt(path, value);
    else if (std::is_same<T, double>::value)
        _setDouble(path, value);
    else if (std::is_same<T, bool>::value)
        _setBool(path, value);
    else if (std::is_same<T, const char *>::value)
        _setString(path, value);
    else if (std::is_same<T, FirebaseJson &>::value)
        _setJson(path, &value);
    else if (std::is_same<T, FirebaseJsonArray &>::value)
        _setArray(path, &value);
}

void FirebaseJsonArray::_setString(int index, const std::string &value)
{
    _set2(index, value.c_str(), true);
}

void FirebaseJsonArray::_setString(const String &path, const std::string &value)
{
    _set(path.c_str(), value.c_str(), true);
}

void FirebaseJsonArray::_setInt(int index, int value)
{
    char *tmp = new char[50];
    memset(tmp, 0, 50);
    sprintf(tmp, _pd, value);
    _set2(index, tmp, false);
    delete[] tmp;
}

void FirebaseJsonArray::_setInt(const String &path, int value)
{
    char *tmp = new char[50];
    memset(tmp, 0, 50);
    sprintf(tmp, _pd, value);
    _set(path.c_str(), tmp, false);
    delete[] tmp;
}

void FirebaseJsonArray::_setDouble(int index, double value)
{
    char *tmp = new char[50];
    memset(tmp, 0, 50);
    sprintf(tmp, _pf, value);
    int i = strlen(tmp) - 1;
    while (tmp[i] == '0' && i > 0)
        i--;
    if (i < (int)strlen(tmp) - 1)
        tmp[i + 1] = '\0';
    _set2(index, tmp, false);
    delete[] tmp;
}

void FirebaseJsonArray::_setDouble(const String &path, double value)
{
    char *tmp = new char[50];
    memset(tmp, 0, 50);
    sprintf(tmp, _pf, value);
    int i = strlen(tmp) - 1;
    while (tmp[i] == '0' && i > 0)
        i--;
    if (i < (int)strlen(tmp) - 1)
        tmp[i + 1] = '\0';
    _set(path.c_str(), tmp, false);
    delete[] tmp;
}

void FirebaseJsonArray::_setBool(int index, bool value)
{
    if (value)
        _set2(index, _tr, false);
    else
        _set2(index, _fls, false);
}

void FirebaseJsonArray::_setBool(const String &path, bool value)
{
    if (value)
        _set(path.c_str(), _tr, false);
    else
        _set(path.c_str(), _fls, false);
}

void FirebaseJsonArray::_setNull(int index)
{
    _set2(index, _nll, false);
}

void FirebaseJsonArray::_setNull(const String &path)
{
    _set(path.c_str(), _nll, false);
}

void FirebaseJsonArray::_setJson(int index, FirebaseJson *json)
{
    std::string s;
    json->_toStdString(s);
    _set2(index, s.c_str(), false);
    std::string().swap(s);
}

void FirebaseJsonArray::_setJson(const String &path, FirebaseJson *json)
{
    std::string s;
    json->_toStdString(s);
    _set(path.c_str(), s.c_str(), false);
    std::string().swap(s);
}

void FirebaseJsonArray::_setArray(int index, FirebaseJsonArray *arr)
{
    std::string s;
    arr->_toStdString(s);
    _set2(index, s.c_str(), false);
    std::string().swap(s);
}

void FirebaseJsonArray::_setArray(const String &path, FirebaseJsonArray *arr)
{
    std::string s;
    arr->_toStdString(s);
    _set(path.c_str(), s.c_str(), false);
    std::string().swap(s);
}

bool FirebaseJsonArray::remove(int index)
{
    char *tmp = new char[50];
    std::string path = "";
    path += _brk3;
    memset(tmp, 0, 50);
    itoa(index, tmp, 10);
    path += tmp;
    path += _brk4;
    bool ret = _remove(path.c_str());
    std::string().swap(path);
    delete[] tmp;
    return ret;
}

bool FirebaseJsonArray::remove(const String &path)
{
    return _remove(path.c_str());
}

bool FirebaseJsonArray::_remove(const char *path)
{
    _json._toStdString(_jbuf, false);
    _json._rawbuf = _root;
    _json._rawbuf += _jbuf;
    char *tmp2 = new char[2];
    memset(tmp2, 0, 2);
    std::string path2 = _root2;
    path2 += _slash;
    path2 += path;
    _json._jsonData.success = _json.remove(path2.c_str());
    delete[] tmp2;
    std::string().swap(path2);
    if (_json._jsonData.success)
    {
        std::string().swap(_json._jsonData._dbuf);
        std::string().swap(_json._tbuf);
        _json._parse(_root2, PRINT_MODE_PLAIN);
        if (_json._jsonData.success)
        {
            _arrLen = _json._jsonData._len;
            _json._rawbuf = _json._jsonData._dbuf.substr(1, _json._jsonData._dbuf.length() - 2);
        }
    }
    else
        _json._rawbuf = _jbuf.substr(1, _jbuf.length() - 2);
    return _json._jsonData.success;
}

void FirebaseJsonArray::_toStdString(std::string &s)
{
    _json._toStdString(s, false);
}

FirebaseJsonData::FirebaseJsonData()
{
}

FirebaseJsonData::~FirebaseJsonData()
{
    std::string().swap(_dbuf);
}

bool FirebaseJsonData::getArray(FirebaseJsonArray &jsonArray)
{
    if (typeNum != JSON_ARRAY || !success)
        return false;
    char *tmp = new char[20];
    memset(tmp, 0, 20);
    char *nbuf = new char[2];
    memset(nbuf, 0, 2);
    strcpy_P(tmp, FirebaseJson_STR_21);
    jsonArray._json._toStdString(jsonArray._jbuf, false);
    jsonArray._json._rawbuf = tmp;
    jsonArray._json._rawbuf += stringValue.c_str();
    memset(tmp, 0, 20);
    strcpy_P(tmp, FirebaseJson_STR_26);
    std::string().swap(jsonArray._json._jsonData._dbuf);
    std::string().swap(jsonArray._json._tbuf);
    jsonArray._json._parse(tmp, PRINT_MODE_PLAIN);
    jsonArray._json._rawbuf = jsonArray._json._jsonData._dbuf.substr(1, jsonArray._json._jsonData._dbuf.length() - 2).c_str();
    jsonArray._arrLen = jsonArray._json._jsonData._len;
    delete[] tmp;
    delete[] nbuf;
    return jsonArray._json._jsonData.success;
}

bool FirebaseJsonData::getJSON(FirebaseJson &json)
{
    if (typeNum != JSON_OBJECT || !success)
        return false;
    json.setJsonData(stringValue);
    json._jsmn_parse();
    return json._jsonData.success;
}