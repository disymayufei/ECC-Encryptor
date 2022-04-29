#pragma once
/*
 * 用于AES的加密/解密
 */

#include <stdint.h>
#include <stdlib.h>
#include <string>

 /*
  * Multiplication in GF(2^8)
  * http://en.wikipedia.org/wiki/Finite_field_arithmetic
  * Irreducible polynomial m(x) = x8 + x4 + x3 + x + 1
  *
  * NOTE: we are using the look up table instead of the (slower) gmult function
  */
#define gmult(a,b) gmult_aes[256*a + b]

uint8_t* aes_init(size_t key_size);

void aes_key_expansion(uint8_t* key, uint8_t* w);

void aes_inv_cipher(uint8_t* in, uint8_t* out, uint8_t* w);

void aes_cipher(uint8_t* in, uint8_t* out, uint8_t* w);


std::string Gen_AES_Key();

std::string AES_Encrypt(std::string Mes, std::string Key);

std::string AES_Decrypt(std::string E_Mes, std::string Key);