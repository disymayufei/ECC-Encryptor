#include <bitset>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <malloc.h>

#include "Base64.h"
#include"ECC_Calc.h"
#include "gmp.h"

Base64 base64;

std::ostream& operator<<(std::ostream& out, const G_point& G) {  // 重载运算符<<以正常显示点坐标
	out << "(" << G.x << ", " << G.y << ")";
	return out;
};


std::ostream& operator<<(std::ostream& out, const Encrypt_Point& EG) {  // 重载运算符<<以正常显示加密点组坐标
	out << "(" << EG.p1.x << ", " << EG.p1.y << ")" << "|" << "(" << EG.p2.x << ", " << EG.p2.y << ")";
	return out;
};



ECC_Calc::ECC_Calc() {
	/* 参数与BTC所用的参数一致
	 * a = 0
	 * b = 7
	 * p = 115792089237316195423570985008687907853269984665640564039457584007908834671663 (取模数)
	 * n = 115792089237316195423570985008687907852837564279074904382605163141518161494337 (基点G的阶)
	 * G = (55066263022277343669578718895168534326250603453777594175500187360389116729240, 32670510020758816978083085130507043184471273380659243275938904335757337482424) (基点G的坐标)
	*/
	mpz_inits(a, b, n, p, NULL);
	mpz_set_si(a, 0);
	mpz_set_si(b, 7);
	mpz_set_str(n, "115792089237316195423570985008687907852837564279074904382605163141518161494337", 10);
	mpz_set_str(p, "115792089237316195423570985008687907853269984665640564039457584007908834671663", 10);
	G = { "55066263022277343669578718895168534326250603453777594175500187360389116729240", "32670510020758816978083085130507043184471273380659243275938904335757337482424" };
};


std::string ECC_Calc::to_binary(mpz_t val) {  // 自己实现的十进制转二进制(倒序排列)，用于double速乘法
	std::string ans;

	ans = mpz_get_str(nullptr, 2, val);

	std::reverse(ans.begin(), ans.end());  // 需要正序的时候去掉这句
	return ans;
}


std::string ECC_Calc::to_binary_not_reverse(mpz_t val) {  // 自己实现的十进制转二进制
	std::string ans;

	ans = mpz_get_str(nullptr, 2, val);

	return ans;
}


std::string ECC_Calc::to_binary(char value) {
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


void ECC_Calc::ext_euclid(mpz_t result, mpz_t x, mpz_t a, mpz_t b) {  //扩展欧几里得算法

	mpz_t r, q, y, m, n, temp, s1, s2;  // temp为中间变量
	mpz_inits(r, q, y, m, n, temp, s1, s2, NULL);  // 初始化初始变量
	mpz_set_si(s1, 1);  // s1 = 1
	mpz_set_si(s2, 0);  // s2 = 0

	if (mpz_cmp_si(b, 0) == 0) {
		mpz_set_si(x, 1);
		mpz_set(result, a);  // return a
		mpz_clears(r, q, y, m, n, temp, s1, s2, a, b, NULL);  // 清除栈内临时数据
		return;
	}

	/* 对初值初始化 */
	mpz_set_si(x, 0);  // x = 0
	mpz_set_si(y, 1);  // y = 1
	mpz_mod(r, a, b);  // r = a % b
	mpz_tdiv_q(q, a, b);  // q = a / b

	while (mpz_cmp_si(r, 0) != 0 /* r != 0 */) {
		mpz_set(m, x); mpz_set(n, y);  // m = x, n = y

		mpz_mul(temp, x, q);  // temp = x * q
		mpz_sub(x, s1, temp);  // x = s1 - x*q

		mpz_mul(temp, y, q);  // temp = y * q
		mpz_sub(y, s2, temp);  // y = s2 - y*q

		mpz_set(s1, m); mpz_set(s2, n);  // s1 = m, s2 = n
		mpz_set(a, b); mpz_set(b, r);  // a = b, b = r

		mpz_mod(r, a, b);  // r = a % b
		mpz_tdiv_q(q, a, b);  // q = a / b
	}

	mpz_set(result, b);
	mpz_clears(r, q, y, m, n, temp, s1, s2, a, b, NULL);  // 回收中间变量
}


void ECC_Calc::calc_inverse(mpz_t result, mpz_t a, mpz_t m) {  //求乘法逆元的值
	mpz_t gcd, x, clone_a, clone_m, clone_m_abs, ans;
	mpz_inits(gcd, x, clone_a, clone_m, clone_m_abs, ans, NULL);

	mpz_set(clone_a, a); mpz_set(clone_m, m);  // a.clone(), m.clone()
	mpz_abs(clone_m_abs, m);  // abs(m.clone())
	
	this->ext_euclid(gcd, x, clone_a, clone_m);

	if (mpz_cmp_si(gcd, 1) != 0) {
		mpz_set_si(result, -1);  // return -1;
	}

	mpz_mod(ans, x, clone_m_abs);  // ans = x % abs(m);

	mpz_set(result, ans);  // return ans;
	mpz_clears(gcd, clone_m_abs, ans, a, NULL);  // 回收中间变量
}


G_point ECC_Calc::Double(G_point& G, mpz_t a, mpz_t p){
	mpz_t x, y, double_y, double_x, Inverse, clone_p, k, x1, y1;
	mpz_inits(x, y, double_y, double_x, Inverse, clone_p, k, x1, y1, NULL);

	mpz_set_str(x, G.x, 10);  // x = G.x
	mpz_set_str(y, G.y, 10);  // y = G.y
	mpz_set(clone_p, p);  // p.clone()

	mpz_mul_si(double_y, y, 2);
	mpz_mul_si(double_x, x, 2);

	this->calc_inverse(Inverse, double_y, clone_p);

	mpz_pow_ui(k, x, 2);
	mpz_mul_si(k, k, 3);
	mpz_add(k, k, a);
	mpz_mul(k, k, Inverse);  // k = (3 * x * x + a) * Inverse;

	mpz_pow_ui(x1, k, 2);
	mpz_sub(x1, x1, double_x);
	mpz_mod(x1, x1, p);  // x1 = (k * k - (2 * x)) % p

	mpz_sub(y1, x, x1);
	mpz_mul(y1, y1, k);
	mpz_sub(y1, y1, y);
	mpz_mod(y1, y1, p);  // y1 = (k * (x - x1) - y) % p

	G_point* Result = (G_point*)malloc(sizeof(G_point));
	if (Result) {
		Result->x = mpz_get_str(nullptr, 10, x1);
		Result->y = mpz_get_str(nullptr, 10, y1);
		mpz_clears(x, y, Inverse, k, x1, y1, NULL);
		return *Result;
	}
	else {
		std::cout << "Error: Out of Memory" << std::endl;
		exit(255);
	}
	
}


G_point ECC_Calc::Add(G_point &p1, G_point &p2, mpz_t a, mpz_t p) {
	mpz_t x1, x2, x3, y1, y2, y3;

	mpz_init_set_str(x1, p1.x, 10);
	mpz_init_set_str(x2, p2.x, 10);
	mpz_init_set_str(y1, p1.y, 10);
	mpz_init_set_str(y2, p2.y, 10);

	mpz_t k, Inverse, clone_p, double_y1, temp;
	mpz_inits(k, Inverse, clone_p, double_y1, temp, NULL);

	mpz_set(clone_p, p);
	mpz_mul_si(double_y1, y1, 2);

	if (p1 == p2) {
		// k = (3 * x1 * x1 + a) * (this->calc_inverse(2 * y1), p)) % p

		this->calc_inverse(Inverse, double_y1, clone_p);
		mpz_pow_ui(k, x1, 2);
		mpz_mul_si(k, k, 3);
		mpz_add(k, k, a);
		mpz_mul(k, k, Inverse);
		mpz_mod(k, k, p);
	}
	else {
		// k = ((y2 - y1) * this->calc_inverse((x2 - x1), p) % p;
		mpz_sub(temp, x2, x1);
		this->calc_inverse(Inverse, temp, p);
		mpz_sub(k, y2, y1);
		mpz_mul(k, k, Inverse);
		mpz_mod(k, k, p);
	}

	mpz_inits(x3, y3, NULL);

	/* 依离散椭圆曲线规则计算x坐标 */
	mpz_pow_ui(x3, k, 2);
	mpz_sub(x3, x3, x1);
	mpz_sub(x3, x3, x2);
	mpz_mod(x3, x3, p);

	/* 依离散椭圆曲线规则计算y坐标 */
	mpz_sub(y3, x1, x3);
	mpz_mul(y3, y3, k);
	mpz_sub(y3, y3, y1);
	mpz_mod(y3, y3, p);

	G_point* Result = (G_point*)malloc(sizeof(G_point));
	if (Result) {
		Result->x = mpz_get_str(nullptr, 10, x3);
		Result->y = mpz_get_str(nullptr, 10, y3);
		mpz_clears(x1, x2, x3, y1, y2, y3, nullptr);
		return *Result;
	}
	else {
		std::cout << "Error: Out of Memory!" << std::endl;
		exit(255);
	}
	
}


G_point ECC_Calc::Multiply(G_point& G, mpz_t& n, mpz_t& a, mpz_t& p) {
	mpz_t x, y;
	mpz_inits(x, y, NULL);

	mpz_set_str(x, G.x, 10);  // x = G.x
	mpz_set_str(y, G.y, 10);  // y = G.y

	G_point* Result = nullptr;

	std::string bin = to_binary(n);

	unsigned long len_bin = bin.size();
	for (int i = 0; i < len_bin; i++) {
		if (bin[i] == '1') {
			if (!Result) {
				Result = (G_point*)malloc(sizeof(G_point));
				if (!Result) {
					std::cout << "Error: Out of Memory!" << std::endl;
					exit(255);
				}
				else {
					char r_x[256];
					char r_y[256];

					strcpy_s(r_x, G.x);
					strcpy_s(r_y, G.y);

					Result->x = (char*)&r_x;
					Result->y = (char*)&r_y;
				}
			}
			else {
				*Result = ECC_Calc::Add(*Result, G, a, p);
			}
		}
		G = ECC_Calc::Double(G, a, p);
	}
	mpz_clears(x, y, nullptr);
	if(Result){
		return *Result;
	}
	else {
		std::cout << "Error: Out of Memory!" << std::endl;
		exit(255);
	}
}


void ECC_Calc::Pow_FAST(mpz_t result, mpz_t a, mpz_t n){
	mpz_t temp, one;
	mpz_inits(temp, NULL);

	mpz_init_set_si(result, 1);  // var ans = 1
	mpz_init_set_si(one, 1);  // var one = 1

	
	while (mpz_cmp_si(n, 0) != 0) {  // while (!n)
		mpz_and(temp, n, one);  // temp = n & 1
		if (mpz_cmp_si(temp, 0) != 0) {
			mpz_mul(result, result, a);  // ans *= a
			mpz_mul(a, a, a);  // a *= a
			mpz_tdiv_q_ui(n, n, 2);  // n /= 2
		}
	}

	mpz_clears(a, n, NULL);
	// return result;
}


void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c){    //字符串的分割
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}


std::vector<unsigned char> ECC_Calc::ToCharList(mpz_t num) {  // 大数编码为字符数组
	std::string Binary = this->to_binary_not_reverse(num);
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


std::vector<unsigned char> ECC_Calc::ToCharList(std::string num) {  // 字符串形式的大数编码为字符数组
	mpz_t temp_num;
	mpz_init_set_str(temp_num, num.c_str(), 10);

	std::string Binary = this->to_binary_not_reverse(temp_num);
	mpz_clear(temp_num);

	std::string temp = "";

	std::vector<unsigned char> result;

	while ((Binary.size() % 8) != 0) {
		Binary = "0" + Binary;
	}

	volatile long long BinarySize = Binary.size();

	for (int i = 0; i < BinarySize; ++i) {
		if (Binary[i] != '-') {
			temp += Binary[i];
		}
		else {
			temp += '1';
		}

		if (!((i + 1) % 8)) {
			result.push_back((unsigned char)std::bitset<8>(temp).to_ulong());
			temp = "";
		}
	}

	return result;
}


std::string ECC_Calc::str_to_binary(std::string in_str) {
	std::string temp;

	int str_Size = in_str.size();

	for (long long i = 0; i < str_Size; ++i) {
		temp += this->to_binary(in_str[i]);
	}

	return temp;
}


/*以下为加解密与生成密钥的主要算法*/

KeyPackage ECC_Calc::Gen_Keys() {
	KeyPackage result;
	mpz_t random_num, random_key, temp_big_num;

	mpz_inits(random_num, random_key, NULL);
	mpz_init_set_str(temp_big_num, "100000000000000000000000000000000000000", 10);
	mpz_set_ui(random_num, random.randUnsignedLong());

	for (int i = 0; i < 4; i++) {
		mpz_mul_ui(random_num, random_num, random.randUnsignedLong());
	}

	mpz_mul(random_num, random_num, temp_big_num);

	while (mpz_cmp(random_num, p) >= 0) {  // while(random_num >= p)
		mpz_set_str(temp_big_num, "10000000000000000000000000000000000000", 10);
		mpz_set_ui(random_num, random.randUnsignedLong());
		for (int i = 0; i < 4; i++) {
			mpz_mul_ui(random_num, random_num, random.randUnsignedLong());
		}
		mpz_mul(random_num, random_num, temp_big_num);
	}

	mpz_sub(random_key, p, random_num);
	mpz_abs(random_key, random_key);

	std::vector<unsigned char> Private = ToCharList(random_key);
	result.Private_Key = base64.Encode(&Private[0], Private.size());

	G_point Public = this->Multiply(this->G, random_key, this->a, this->p);
	std::vector<unsigned char> p_x = ToCharList(Public.x);
	std::vector<unsigned char> p_y = ToCharList(Public.y);
	result.Public_Key = (base64.Encode(&p_x[0], p_x.size()) + "|" + base64.Encode(&p_y[0], p_y.size()));

	mpz_clears(random_num, random_key, temp_big_num, NULL);

	return result;
}


/*以下针对明文的加解密*/
std::vector<G_point> ECC_Calc::Plain_Encode(std::string x) {
	WinRandom r;
	std::vector<G_point> G;
	mpz_t result_x, result_y;
	mpz_inits(result_x, result_y, NULL);

	unsigned int* temp = (unsigned int*)x.c_str();

	int Size = strlen(x.c_str());

	if (Size % 4 == 0) {
		Size /= 4;
	}
	else {
		Size /= 4;
		Size += 1;
	}

	for (int i = 0; i < Size; i++) {
		mpz_set_ui(result_y, r.randUnsignedLong());
		mpz_set_ui(result_x, (const unsigned int)*temp);

		G.push_back(G_point{ mpz_get_str(nullptr, 10, result_x), mpz_get_str(nullptr, 10, result_y) });

		temp++;
	}

	mpz_clears(result_x, result_y, NULL);
	return G;
}


std::string ECC_Calc::Encrypt(std::string K, std::vector<G_point> M) {
	std::string Result = "";
	WinRandom random;
	Encrypt_Point E_Mes;

	mpz_t r, temp_x, temp_y;
	mpz_init_set_ui(r, random.randUnsignedLong());

	std::vector<std::string> temp;
	SplitString(K, temp, "|");

	mpz_init_set_str(temp_x, str_to_binary(base64.Decode(temp[0])).c_str(), 2);
	mpz_init_set_str(temp_y, str_to_binary(base64.Decode(temp[1])).c_str(), 2);

	G_point PK = { mpz_get_str(nullptr, 10, temp_x), mpz_get_str(nullptr, 10, temp_y) };

	long long VSize = M.size();

	for (long long i = 0; i < VSize; ++i) {
		E_Mes = {};
		G_point temp_point = (this->Multiply(PK, r, this->a, this->p));

		E_Mes.p1 = this->Add(M[i], temp_point, this->a, this->p);
		E_Mes.p2 = this->Multiply(this->G, r, this->a, this->p);

		std::vector<unsigned char> p1_x = ToCharList(E_Mes.p1.x);
		std::vector<unsigned char> p1_y = ToCharList(E_Mes.p1.y);
		std::vector<unsigned char> p2_x = ToCharList(E_Mes.p2.x);
		std::vector<unsigned char> p2_y = ToCharList(E_Mes.p2.y);

		Result += base64.Encode(&p1_x[0],p1_x.size()) + "|" + base64.Encode(&p1_y[0], p1_y.size()) + "|" + base64.Encode(&p2_x[0], p2_x.size()) + "|" + base64.Encode(&p2_y[0], p2_y.size());
		if (i < VSize - 1) {
			Result += "||";
		}

		mpz_set_ui(r, random.randUnsignedLong());
	}

	mpz_clears(r, temp_x, temp_y, NULL);

	return Result;
}


std::string ECC_Calc::Decrypt(std::string k, std::string EM) {

	std::vector<Encrypt_Point> EM_List;

	std::vector<std::string> temp_encrypt_point;
	std::vector<std::string> temp_coordinate;

	SplitString(EM, temp_encrypt_point, "||");


	mpz_t x1, x2, y1, y2;
	mpz_inits(x1, x2, y1, y2, NULL);

	for (long long i = 0; i < temp_encrypt_point.size(); ++i) {
		SplitString(temp_encrypt_point[i], temp_coordinate, "|");

		Encrypt_Point* temp_point = (Encrypt_Point*)malloc(sizeof(Encrypt_Point));
		
		mpz_set_str(x1, str_to_binary(base64.Decode(temp_coordinate[0])).c_str(), 2);
		mpz_set_str(y1, str_to_binary(base64.Decode(temp_coordinate[1])).c_str(), 2);
		mpz_set_str(x2, str_to_binary(base64.Decode(temp_coordinate[2])).c_str(), 2);
		mpz_set_str(y2, str_to_binary(base64.Decode(temp_coordinate[3])).c_str(), 2);

		if (temp_point) {
			temp_point->p1.x = mpz_get_str(nullptr, 10, x1);
			temp_point->p1.y = mpz_get_str(nullptr, 10, y1);
			temp_point->p2.x = mpz_get_str(nullptr, 10, x2);
			temp_point->p2.y = mpz_get_str(nullptr, 10, y2);
		}
		else {
			std::cout << "Error: Out of Memory!" << std::endl;
			exit(255);
		}

		EM_List.push_back(*temp_point);

		std::vector<std::string>().swap(temp_coordinate);  // 释放temp_coordinate
	}
	mpz_clears(x1, x2, y1, y2, NULL);

	std::vector<unsigned char> temp_result;

	std::string Result;

	int VSize = EM_List.size();

	mpz_t temp_num;
	mpz_init(temp_num);

	mpz_t pk;
	mpz_init_set_str(pk, str_to_binary(base64.Decode(k)).c_str(), 2);

	for (int i = 0; i < VSize; ++i) {
		mpz_set_str(temp_num, EM_List[i].p2.y, 10);
		mpz_mul_si(temp_num, temp_num, -1);
		mpz_mod(temp_num, temp_num, this->p);

		EM_List[i].p2.y = mpz_get_str(nullptr, 10, temp_num);

		G_point temp_g_point = this->Multiply(EM_List[i].p2, pk, this->a, this->p);
		std::string result_x = this->Add(EM_List[i].p1, temp_g_point, this->a, this->p).x;


		unsigned int value = (unsigned int)atoi(result_x.c_str());  // 临时储存解析出的8字节数据
		
		unsigned char* char_value = (unsigned char*)(&value);  // 转化为char*指针逐字节读取

		for (int i = 0; i < sizeof(unsigned int); i++) {
			temp_result.push_back(*char_value);
			char_value++;
		}
	}

	mpz_clear(temp_num);

	mpz_clear(pk);

	long long RSize = temp_result.size();

	for (long long i = 0; i < RSize; ++i) {
		Result += temp_result[i];
	}

	return Result;
};