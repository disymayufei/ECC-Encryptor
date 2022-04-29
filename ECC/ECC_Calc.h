#pragma once
#ifndef _ECC_CALC_H
#define _ECC_CALC_H

#include <vector>
#include <string>

#include "gmp.h"
#include "RdRand.h"


typedef struct G_point {  // 储存椭圆曲线点坐标
	const char* x;
	const char* y;

	bool operator== (G_point& p2) {  // 判断两个点是否处于同一坐标
		if (this->x == x && this->y == p2.y) {
			return true;
		}

		return false;
	}

	friend std::ostream& operator<<(std::ostream& out, const G_point& G);

} G_point;

typedef struct Encrypt_Point {  // 储存加密后的两个点(M + rk, rG)
	G_point p1;
	G_point p2;
	friend std::ostream& operator<<(std::ostream& out, const Encrypt_Point& EG);
} Encrypt_Point;

typedef struct KeyPackage {  // 储存密钥对（公钥 + 私钥）
	std::string Private_Key;
	std::string Public_Key;
} KeyPackage;


class ECC_Calc
{
public:
	ECC_Calc();

	std::string Encrypt(std::string Public_Key, std::vector<G_point> M);  // 基于Base64公钥的加密
	std::string Decrypt(std::string Private_Key, std::string EM);         // 基于Base64私钥的解密

	std::vector<G_point> Plain_Encode(std::string x);

	KeyPackage Gen_Keys();

	std::string to_binary(mpz_t val);
	std::string to_binary(char value);

	G_point Add(G_point &p1, G_point &p2, mpz_t a, mpz_t p);  // 椭圆曲线点的加和运算
	G_point Double(G_point &G, mpz_t a, mpz_t p);			  // 椭圆曲线点的倍乘运算
	G_point Multiply(G_point& G, mpz_t& n, mpz_t& a, mpz_t& p);   // 椭圆曲线的乘法运算

private:
	
	void Pow_FAST(mpz_t result, mpz_t a, mpz_t n);                          // 快速幂算法（弃用，由于GMP本身对幂运算有优化）
	std::vector<unsigned char> ToCharList(mpz_t num);
	std::vector<unsigned char> ToCharList(std::string num);
	std::string str_to_binary(std::string in_str);
	std::string to_binary_not_reverse(mpz_t val);
	

	WinRandom random;

	mpz_t a, b, p, n;

	G_point G;

	void calc_inverse(mpz_t result, mpz_t a, mpz_t m);
	void ext_euclid(mpz_t result, mpz_t x, mpz_t a, mpz_t b);
	
};

#endif

