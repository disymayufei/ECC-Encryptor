#pragma once
#include <gmp.h>
#include <string>
#include <vector>

class BigInt {
public:
	BigInt();
	BigInt(const char* str);
	BigInt(std::string str, bool isBinary);
	BigInt(const signed long num);
	BigInt(const unsigned long num, const bool isUnsigned);

	BigInt operator+ (const BigInt& GB);
	BigInt operator- (const BigInt& GB);
	BigInt operator* (const BigInt& GB);
	BigInt operator/ (const BigInt& GB);
	BigInt operator% (const BigInt& GB);
	BigInt operator= (const char* str);

	BigInt operator+ (const unsigned long num);
	BigInt operator- (const unsigned long num);
	BigInt operator* (const unsigned long num);
	BigInt operator/ (const unsigned long num);
	BigInt operator% (const unsigned long num);
	BigInt operator= (const signed long);

	BigInt operator& (const BigInt& GB);

	BigInt operator++ ();
	BigInt operator-- ();

	bool operator< (const BigInt& GB);
	bool operator> (const BigInt& GB);
	bool operator<= (const BigInt& GB);
	bool operator>= (const BigInt& GB);
	bool operator== (const BigInt& GB);
	bool operator!= (const BigInt& GB);


	BigInt operator+= (const BigInt& GB);
	BigInt operator-= (const BigInt& GB);
	BigInt operator*= (const BigInt& GB);
	BigInt operator/= (const BigInt& GB);
	BigInt operator%= (const BigInt& GB);

	BigInt operator+= (const unsigned long num);
	BigInt operator-= (const unsigned long num);
	BigInt operator*= (const unsigned long num);
	BigInt operator/= (const unsigned long num);
	BigInt operator%= (const unsigned long num);

	bool operator< (const signed long num);
	bool operator> (const signed long num);
	bool operator<= (const signed long num);
	bool operator>= (const signed long num);
	bool operator== (const signed long num);
	bool operator!= (const signed long num);

	static BigInt abs(const BigInt& GB);

	friend std::ostream& operator<<(std::ostream& out, const BigInt& GB);

	std::string ToString();
	std::vector<unsigned char> ToCharList();
	void Free();

	mpz_t num;
	
};