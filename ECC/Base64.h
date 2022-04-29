#pragma once
#ifndef BASE_64_H
#define BASE_64_H

/*
 * 用于Base64的编码/解码
 */

class Base64 {

private:
    std::string _base64_table;

static const char base64_pad = '='; public:
    Base64()
    {
        _base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; // Base64标准字典
    }

    /*
    这里必须是unsigned类型，否则编码中文的时候出错
    */

    std::string Encode(const unsigned char* str, int bytes);
    std::string Decode(const std::string str);
};
#endif