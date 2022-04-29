#pragma once
#ifndef _RDRAND_H
#define _RDRAND_H

#include <assert.h>
#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <sodium.h>

class WinRandom {
    HCRYPTPROV handle;
public:
    WinRandom() {
        handle = NULL;
        CryptAcquireContext(
            (HCRYPTPROV*)&handle, NULL, NULL,
            PROV_RSA_FULL, 0
        );
    }
    ~WinRandom() {
        if (handle != NULL) CryptReleaseContext(handle, 0);
    }
    bool randBuf(void* dest, int len) {
        if (handle == NULL) {
            randombytes_buf(dest, len);
            return false;
        }
        return CryptGenRandom(handle, len, (BYTE*)dest);
    }
#define _(func, typ) \
	typ func() { \
    	typ ret = 0; \
        randBuf((void *)&ret, sizeof(ret)); \
        return ret; \
    }
    _(randInt, volatile int)
        _(randLong, volatile long long)
        _(randUnsigned, volatile unsigned)
        _(randUnsignedLong, volatile unsigned long long)
        _(randShort, volatile short)
        _(randUnsignedShort, volatile unsigned short)
        _(randChar, volatile char)
        _(randUnsignedChar, volatile unsigned char)
        _(randSignedChar, volatile signed char)
};
#endif