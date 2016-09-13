/*
 * string_ex.cpp
 *
 *  Created on: 2016-9-13
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include "../bingo/string.h"

BOOST_AUTO_TEST_SUITE(test_string_ex)

// Test string_ex::string_to_stream() method
BOOST_AUTO_TEST_CASE(t1){

	string str = "68 20 0d 00 01 77 77 77 2e 73 6f 68 75 2e 63 6f 6d 01 00 00 00 00 00 00 00 d2 f8 21 5a 16";

	char* p = new char[200];
	memset(p, 0x00, 200);
	bingo::string_ex t;
	t.string_to_stream(str, p);

	string s_out = t.stream_to_string(p, 30);
	BOOST_CHECK(s_out.compare(str.c_str()) == 0);

	delete[] p;
}

BOOST_AUTO_TEST_SUITE_END()
