#include "BigInt.h"

#include <bitset>
#include <iostream>
#include <string>
#include <vector>

std::string to_binary(char value) {
	unsigned char val = (unsigned char)value;

	std::string ans;

	while (val)
	{
		ans += val & 1 ? '1' : '0';
		val = val >> 1;
	}

	std::reverse(ans.begin(), ans.end());

	while (ans.size() < 8) {
		ans = '0' + ans;
	}
	return ans;
}

std::string to_binary(BigInt val) {
	std::string ans;

	for (BigInt a = val; a != BigInt("0"); a = a / BigInt("2")) {
		ans = ans + ((a % BigInt("2")) != BigInt("0") ? '1' : '0');
	}

	std::reverse(ans.begin(), ans.end());
	return ans;
}


BigInt::BigInt() {
	mpz_init(this->num);
	mpz_set_str(this->num, "0", 10);
}

BigInt::BigInt(const char* str) {
	mpz_init(this->num);
	mpz_set_str(this->num, str, 10);
}

BigInt::BigInt(std::string in_str, bool isBinary) {

	std::string temp;

	int str_Size = in_str.size();

	for (long long i = 0; i < str_Size; ++i) {
		temp += to_binary(in_str[i]);
	}

	mpz_init(this->num);
	mpz_set_str(this->num, temp.c_str(), 2);
}

BigInt::BigInt(const signed long num) {
	mpz_init(this->num);
	mpz_set_si(this->num, num);
}

BigInt::BigInt(const unsigned long num, bool isUnsigned) {
	if (isUnsigned) {
		mpz_init(this->num);
		mpz_set_ui(this->num, num);
	}
	else {
		mpz_init(this->num);
		mpz_set_si(this->num, num);
	}
}

BigInt BigInt::operator+ (const BigInt& GB) {
	BigInt result = BigInt();
	mpz_add(result.num, this->num, GB.num);
	return result;
}

BigInt BigInt::operator- (const BigInt& GB) {
	BigInt result = BigInt();
	mpz_sub(result.num, this->num, GB.num);
	return result;
}

BigInt BigInt::operator* (const BigInt& GB) {
	BigInt result = BigInt();
	mpz_mul(result.num, this->num, GB.num);
	return result;
}

BigInt BigInt::operator/ (const BigInt& GB) {
	BigInt result = BigInt();
	mpz_tdiv_q(result.num, this->num, GB.num);
	return result;
}

BigInt BigInt::operator% (const BigInt& GB) {
	BigInt result = BigInt();
	mpz_tdiv_r(result.num, this->num, GB.num);

	while (result < 0) {
		mpz_add(result.num, result.num, GB.num);
	}

	return result;
}

BigInt BigInt::operator= (const char* str) {
	mpz_init(this->num);
	mpz_set_str(this->num, str, 10);
	return *this;
};

BigInt BigInt::operator& (const BigInt& GB) {
	BigInt result = BigInt();
	mpz_and(result.num, this->num, GB.num);
	return result;
};


BigInt BigInt::operator+ (const unsigned long num) {
	BigInt result = BigInt();
	mpz_add_ui(result.num, this->num, num);
	return result;
}

BigInt BigInt::operator- (const unsigned long num) {
	BigInt result = BigInt();
	mpz_sub_ui(result.num, this->num, num);
	return result;
}

BigInt BigInt::operator* (const unsigned long num) {
	BigInt result = BigInt();
	mpz_mul_ui(result.num, this->num, num);
	return result;
}

BigInt BigInt::operator/ (const unsigned long num) {
	BigInt result = BigInt();
	mpz_tdiv_q_ui(result.num, this->num, num);
	return result;
}

BigInt BigInt::operator% (const unsigned long num) {
	BigInt result = BigInt();
	mpz_tdiv_r_ui(result.num, this->num, num);

	while (result < 0) {
		result += num;
	}
	return result;
}

BigInt BigInt::operator= (const signed long num) {
	mpz_init(this->num);
	mpz_set_si(this->num, num);
	return *this;
};


BigInt BigInt::operator++ () {
	BigInt result = BigInt();
	mpz_add_ui(result.num, this->num, 1L);
	return result;
};

BigInt BigInt::operator-- () {
	BigInt result = BigInt();
	mpz_sub_ui(result.num, this->num, 1L);
	return result;
};

BigInt BigInt::operator+= (const BigInt& GB) {
	mpz_add(this->num, this->num, GB.num);
	return *this;
}

BigInt BigInt::operator-= (const BigInt& GB) {
	mpz_sub(this->num, this->num, GB.num);
	return *this;
}

BigInt BigInt::operator*= (const BigInt& GB) {
	mpz_mul(this->num, this->num, GB.num);
	return *this;
}

BigInt BigInt::operator/= (const BigInt& GB) {
	mpz_tdiv_q(this->num, this->num, GB.num);
	return *this;
}

BigInt BigInt::operator%= (const BigInt& GB) {
	mpz_tdiv_r(this->num, this->num, GB.num);

	while (*this < 0) {
		mpz_add(this->num, this->num, GB.num);
	}

	return *this;
}



BigInt BigInt::operator+= (const unsigned long num) {
	mpz_add_ui(this->num, this->num, num);
	return *this;
}

BigInt BigInt::operator-= (const unsigned long num) {
	mpz_sub_ui(this->num, this->num, num);
	return *this;
}

BigInt BigInt::operator*= (const unsigned long num) {
	mpz_mul_ui(this->num, this->num, num);
	return *this;
}

BigInt BigInt::operator/= (const unsigned long num) {
	mpz_tdiv_q_ui(this->num, this->num, num);
	return *this;
}

BigInt BigInt::operator%= (const unsigned long num) {
	mpz_tdiv_r_ui(this->num, this->num, num);

	while (*this < 0) {
		mpz_add_ui(this->num, this->num, num);
	}
	return *this;
}



bool BigInt::operator> (const BigInt& GB) {
	return mpz_cmp(this->num, GB.num) > 0;
}

bool BigInt::operator< (const BigInt& GB) {
	return mpz_cmp(this->num, GB.num) < 0;
}

bool BigInt::operator>= (const BigInt& GB) {
	return mpz_cmp(this->num, GB.num) >= 0;
}

bool BigInt::operator<= (const BigInt& GB) {
	return mpz_cmp(this->num, GB.num) <= 0;
}

bool BigInt::operator== (const BigInt& GB) {
	return mpz_cmp(this->num, GB.num) == 0;
}

bool BigInt::operator!= (const BigInt& GB) {
	return mpz_cmp(this->num, GB.num) != 0;
}


bool BigInt::operator> (const signed long num) {
	return mpz_cmp_si(this->num, num) > 0;
}

bool BigInt::operator< (const signed long num) {
	return mpz_cmp_si(this->num, num) < 0;
}

bool BigInt::operator>= (const signed long num) {
	return mpz_cmp_si(this->num, num) >= 0;
}

bool BigInt::operator<= (const signed long num) {
	return mpz_cmp_si(this->num, num) <= 0;
}

bool BigInt::operator== (const signed long num) {
	return mpz_cmp_si(this->num, num) == 0;
}

bool BigInt::operator!= (const signed long num) {
	return mpz_cmp_si(this->num, num) != 0;
}

BigInt BigInt::abs(const BigInt& GB) {
	BigInt result = BigInt();

	mpz_abs(result.num, GB.num);

	return result;
};

std::ostream& operator<<(std::ostream& out, const BigInt& GB) {
	gmp_printf("%Zd", GB.num);
	out << "";
	return out;
}

std::string BigInt::ToString() {
	char temp[256];
	gmp_sprintf(temp, "%Zd", this->num);
	std::string result = temp;
	return result;
}

std::vector<unsigned char> BigInt::ToCharList() {
	std::string Binary = to_binary(*this);
	std::string temp = "";

	std::vector<unsigned char> result;

	while ((Binary.size() % 8) != 0) {
		Binary = "0" + Binary;
	}

	volatile long long BinarySize = Binary.size();

	for (int i = 0; i < BinarySize; ++i) {
		temp += Binary[i];
		if (!((i + 1) % 8)) {
			result.push_back((unsigned char)std::bitset<8>(temp).to_ulong());
			temp = "";
		}
	}

	return result;
}

void BigInt::Free() {
	mpz_clear(this->num);
}
