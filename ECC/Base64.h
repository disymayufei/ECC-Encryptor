#pragma once
#ifndef BASE_64_H
#define BASE_64_H

/*
 * ����Base64�ı���/����
 */

class Base64 {

private:
    std::string _base64_table;

static const char base64_pad = '='; public:
    Base64()
    {
        _base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; // Base64��׼�ֵ�
    }

    /*
    ���������unsigned���ͣ�����������ĵ�ʱ�����
    */

    std::string Encode(const unsigned char* str, int bytes);
    std::string Decode(const std::string str);
};
#endif