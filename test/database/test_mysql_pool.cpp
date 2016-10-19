/*
 * test_mysql_pool.cpp
 *
 *  Created on: 2016-10-18
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include "bingo/singleton.h"
#include "bingo/define.h"
#include "bingo/error_what.h"
#include "bingo/database/mysql/all.h"
using namespace bingo::database;
using namespace bingo;

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

BOOST_AUTO_TEST_SUITE(test_database_mysql_pool)

typedef bingo::singleton_v0<mysql::mysql_pool> MYSQL_POOL_TYPE;

void run(){
	db_connector conn_info("192.168.1.104", "mysql", "root", "1234", 3306);
	if(MYSQL_POOL_TYPE::instance()->make_connector_pool(4, &conn_info) == -1){
		cout << "create database fail! err_msg:" << MYSQL_POOL_TYPE::instance()->err().err_message() << endl;
	}else{
		cout << "create database success!" << endl;
	}
}

// Test usecase:
// 1. Create mysql pool fail because mysql server close.
// 2.  Create mysql pool success, after 1 minute,  mysql server close, each item in pool will reconnect.
BOOST_AUTO_TEST_CASE(t_pool){
	MYSQL_POOL_TYPE::construct();

	boost::thread t(run);
	t.join();

	MYSQL_POOL_TYPE::release();
}

void exe_sql(){

	int max = 10, i=0;
	while(i <= max){
		this_thread::sleep(seconds(5));

		mysql::mysql_connector* conn = MYSQL_POOL_TYPE::instance()->get_connector();

		string sql = "select version();";

		db_result* res = new db_result();
		bingo::error_what er;
		message_out_with_time("mysql hdr:" << conn->get_mysql() << ",query result:" << conn->query_result(sql.c_str(), res, er))
		delete res;

		MYSQL_POOL_TYPE::instance()->free_connector(conn);

		i++;
	}

}

// Test usecase:
// 1. execute sql in pool.
BOOST_AUTO_TEST_CASE(t_exe_sql){

	MYSQL_POOL_TYPE::construct();

	boost::thread t(run);
	boost::thread t1(exe_sql);
	t.join();
	t1.join();

	MYSQL_POOL_TYPE::release();
}

BOOST_AUTO_TEST_SUITE_END()

