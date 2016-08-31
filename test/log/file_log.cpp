/*
 * file_log.cpp
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include "../../bingo/log/all.h"
#include "../../bingo/singleton.h"
using namespace bingo;

#include <boost/thread.hpp>
using namespace boost;

// Test that It is write log in multi-thread.
BOOST_AUTO_TEST_SUITE(test_file_log)

struct my_parse {
	static const char* directory;
	static const char* filename;
};
const char* my_parse::directory = ".";
const char* my_parse::filename = "log";

typedef bingo::singleton_v0<
//					log::file_logger_every_day<my_parse>
				    log::file_logger_every_hour<my_parse>
		> my_log;


void run_thread(int& n){

	for (int i = 0; i < 1000; ++i) {
		my_log::instance()->log_i("info", log::log_data() << "n:" << n);
	}

}

BOOST_AUTO_TEST_CASE(t){

	my_log::construct();
	my_log::instance()->level(log::file_logger_every_hour<my_parse>::LOG_LEVEL_ALL);
	this_thread::sleep(seconds(2));

	thread t1(run_thread, 1);
	thread t2(run_thread, 2);
	thread t3(run_thread, 3);
	thread t4(run_thread, 4);

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	my_log::release();
}

typedef bingo::singleton_v0<
				    log::file_logger_without_postfix<my_parse>
		> my_log_without_postfix;

BOOST_AUTO_TEST_CASE(t_without_postfix){
	my_log_without_postfix::construct();
	my_log_without_postfix::instance()->log_d("info", log::log_data() << "this is a test!");
	my_log_without_postfix::release();
}

BOOST_AUTO_TEST_SUITE_END()



