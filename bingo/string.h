/*
 * string.h
 *
 *  Created on: 2016-6-29
 *      Author: root
 */

#ifndef BINGO_STRING_HEADER_H_
#define BINGO_STRING_HEADER_H_

#include <string.h>
#include <stdio.h>
#include <iostream>
 #include <sstream>
using namespace std;

#include "type.h"

namespace bingo {

class string_ex {
private:
	stringstream 		oss_;
	string 				str_;
	enum { MAX_SIZE_OF_CHR = 8, };
	char 				chr_[MAX_SIZE_OF_CHR];
public:
	string_ex(){
		memset(&chr_[0], 0x00, MAX_SIZE_OF_CHR);
	}
	virtual ~string_ex(){}

private:
	void clear();

	// char -> int
	// for example: 'd' -> 11
	int char_to_int(string chr);

public:
	// byte's stream -> string.
	// for example: {0x00, 0x01, 0x02} -> '00 01 02'
	const char* stream_to_string(const char* p, size_t p_size);

	// string -> byte's stream.
	// for example: '01 02 03' -> {0x01, 0x02, 0x03}
	// @out must be initialized and allocate memory.
	void string_to_stream(string& in, char*& out);

	// byte's stream -> string.
	// for example: {0x66, 0x59, 0x48, 0x61} -> 'fYHa'
	const char* stream_to_char(const char* p, size_t p_size);

	// short -> byte of stream.
	// for example: 260 -> {0x04, 0x01}
	const char* short_to_stream(u16_t value);

	// byte of stream -> short
	// for example: {0x04, 0x01} -> 260
	u16_t stream_to_short(const char* p);

	// memory's address -> long.
	// for example: char* p = new char[10];
	//              p -> '12548741244422'
	u64_t pointer_to_long(const void* p);

	// Ip string -> int.
	u32_t ip_to_int(string& s);

	// Int -> ip string
	const char* int_to_ip(u32_t n);


	// Convert T to string.
	template<typename T>
	const char* convert(const T& t){
		clear();
		oss_ << t;
		oss_ >> str_;
		return str_.c_str();
	}

	const char* convert(const char*& t);

	const char* convert(const char& t);

	const char* convert(const u8_t& t);
};

}

#endif /* BINGO_STRING_HEADER_H_ */
