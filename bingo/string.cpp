/*
 * string.cpp
 *
 *  Created on: 2016-6-29
 *      Author: root
 */

#include "string.h"
using namespace bingo;

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace boost;


void string_ex::clear(){

	// Clear stringstream.
	oss_.clear();
	oss_.str("");

	str_ .clear();
	memset(&chr_[0], 0x00, MAX_SIZE_OF_CHR);
}


int string_ex::char_to_int(string chr){

	// Change to lower.
	to_lower(chr);

	if(chr.compare("1") == 0){
		return 1;
	}else if (chr.compare("2") == 0){
		return 2;
	}else if (chr.compare("3") == 0){
		return 3;
	}else if (chr.compare("4") == 0){
		return 4;
	}else if (chr.compare("5") == 0){
		return 5;
	}else if (chr.compare("6") == 0){
		return 6;
	}else if (chr.compare("7") == 0){
		return 7;
	}else if (chr.compare("8") == 0){
		return 8;
	}else if (chr.compare("9") == 0){
		return 9;
	}else if (chr.compare("a") == 0){
		return 10;
	}else if (chr.compare("b") == 0){
		return 11;
	}else if (chr.compare("c") == 0){
		return 12;
	}else if (chr.compare("d") == 0){
		return 13;
	}else if (chr.compare("e") == 0){
		return 14;
	}else if (chr.compare("f") == 0){
		return 15;
	}else{
		return 0;
	}
}

const char* string_ex::stream_to_string(const char* p, size_t p_size){

	clear();

	char tmp[4] = {0x00};
	for (size_t i = 0; i < p_size; ++i) {
		u8_t ch = (u8_t)(*(p + i));
		memset(tmp, 0x00, 4);
		sprintf(tmp, "%02x ", ch);

		str_.append(&tmp[0]);
	}

	trim(str_);

	return str_.c_str();
}

void string_ex::string_to_stream(string& in, char*& out){
	vector<string> v;
	split(v, in, is_any_of(" "),token_compress_on);

	size_t max = v.size();
	for (size_t i = 0; i < max; ++i) {
		string vt = v[i];

		string height_bit = vt.substr(0, 1);
		string low_bit 	 = vt.substr(1, 1);
		int hbit =char_to_int(height_bit);
		int lbit  =char_to_int(low_bit);

		int value = lbit + hbit * 16;

		out[i] = value;
	}
}

const char* string_ex::stream_to_char(const char* p, size_t p_size){

	clear();

	char str[p_size + 1];
	memset(str, 0x00, p_size+1);
	memcpy(str, p, p_size);

	str_.append(str);

	return str_.c_str();
}

const char* string_ex::short_to_stream(u16_t value){

	clear();

	chr_[0] = value % 256;
	chr_[1] = value / 256;

	return chr_;
}

u16_t string_ex::stream_to_short(const char* p){
	u16_t v;
	memcpy(&v, p, 2);

	return v;
}

u64_t string_ex::pointer_to_long(const void* p){
	u64_t address;
	memcpy(&address, &p, 8);

	return address;
}

u32_t string_ex::ip_to_int(string& s){
	vector<string> v;
	split(v, s, is_any_of("."), token_compress_on);

	if(v.size() != 4)
		return 0;
	else{

		u8_t data[4] = {0x00, 0x00, 0x00, 0x00};
		data[0] = lexical_cast<u32_t>(v[0]);
		data[1] = lexical_cast<u32_t>(v[1]);
		data[2] = lexical_cast<u32_t>(v[2]);
		data[3] = lexical_cast<u32_t>(v[3]);

		return data[0] + data[1]*256 + data[2]*256*256 + data[3]*256*256*256;
	}
}

const char* string_ex::int_to_ip(u32_t n){

	clear();

	u32_t n_part1 = n % 256;
	u32_t n_part2 = n / 256 % 256;
	u32_t n_part3 = n / 256 / 256 % 256;
	u32_t n_part4 = n / 256 / 256 / 256;

	char tmp[16] = {0x00};
	memset(tmp, 0x00, 16);
	sprintf(&tmp[0], "%d.%d.%d.%d", n_part1, n_part2, n_part3, n_part4);

	str_.append(&tmp[0]);

	return str_.c_str();
}



const char* string_ex::convert(const char*& t){
	clear();
	str_.append(t);
	return str_.c_str();
}

const char* string_ex::convert(const char& t){
	clear();
	u8_t c = (u8_t)t;
	oss_ << (u16_t)c;
	oss_ >> str_;
	return str_.c_str();
}

const char* string_ex::convert(const u8_t& t){
	clear();
	oss_ << (u16_t)t;
	oss_ >> str_;
	return str_.c_str();
}

