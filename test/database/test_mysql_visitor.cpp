/*
 * test_mysql_visitor.cpp
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include "../../bingo/database/mysql/all.h"
using namespace bingo::database;

#include <iostream>
using namespace std;

BOOST_AUTO_TEST_SUITE(test_database_mysql_visitor)

// 测试查询获得单结果集
BOOST_AUTO_TEST_CASE(t_single_result){
	db_connector conn;
	conn.ip("192.168.1.107");
	conn.port(3306);
	conn.user("root");
	conn.pwd("1234");
	conn.dbname("project");

	db_connector* p = &conn;
	mysql::mysql_visitor v(p);

	bingo::error_what err;
	{
		// Check error sql.
		db_result* res = new db_result();
		BOOST_CHECK(v.query_result("select * from user2;", res, err) == -1);

		cout << err.err_message() << endl;
		// output:
		//	database query fail! error:Table 'project.user2' doesn't exist


		delete res;
	}

	{
		// Correct query.
		db_result* res = new db_result();
		BOOST_CHECK(v.query_result("select * from user;", res, err) == 0);

		BOOST_CHECK(res->row_amount == 8);
		BOOST_CHECK(res->column_amount == 4);

		BOOST_CHECK(res->get_header(0)->name.compare("userId") == 0);
		BOOST_CHECK(res->get_header(1)->name.compare("userName") == 0);
		BOOST_CHECK(res->get_header(2)->name.compare("password") == 0);
		BOOST_CHECK(res->get_header(3)->name.compare("gender") == 0);
		BOOST_CHECK(res->get_header_index("userName") == 1);

		BOOST_CHECK(strcmp(res->get_row(0)->get_field("userName")->data(), "aaa") ==0);
		BOOST_CHECK(strcmp(res->get_row(0)->get_field("password")->data(), "vvvv") ==0);

		delete res;
	}

	// Call free_database() when progress is destroyed.
	mysql::free_mysql_database();

}

// 测试查询获得多结果集
BOOST_AUTO_TEST_CASE(t_multi_result){
	db_connector conn;
	conn.ip("192.168.1.107");
	conn.port(3306);
	conn.user("root");
	conn.pwd("1234");
	conn.dbname("test");

	db_connector* p = &conn;
	mysql::mysql_visitor v(p);

	bingo::error_what err;
	boost::ptr_vector<db_result> ress;
	std::vector<string> sqls;
	{
		// Check error sql.
		sqls.push_back("select * from user2;");
		sqls.push_back("select * from user_roles;");
		BOOST_CHECK(v.query_result(sqls, ress, err) == -1);

		cout << err.err_message() << endl;
		// output:
		//	database query fail! error:Table 'project.user2' doesn't exist

	}

	{
		// Correct query.
		sqls.clear();
		ress.clear();

		sqls.push_back("select * from user;");
		sqls.push_back("select * from user_roles;");
		BOOST_CHECK(v.query_result(sqls, ress, err) == 0);

		BOOST_CHECK(ress.size() == 2);

		{
			db_result* res = &(ress[0]);
			BOOST_CHECK(res->row_amount == 1);
			BOOST_CHECK(res->column_amount == 4);

			BOOST_CHECK(res->get_header(0)->name.compare("username") == 0);
			BOOST_CHECK(res->get_header(1)->name.compare("password") == 0);
			BOOST_CHECK(res->get_header(2)->name.compare("password_salt") == 0);
			BOOST_CHECK(res->get_header(3)->name.compare("autoid") == 0);

			BOOST_CHECK(strcmp(res->get_row(0)->get_field("username")->data(), "tim") ==0);

		}

		{
			db_result* res = &(ress[1]);
			BOOST_CHECK(res->row_amount == 2);
			BOOST_CHECK(res->column_amount == 2);

			BOOST_CHECK(res->get_header(0)->name.compare("username") == 0);
			BOOST_CHECK(res->get_header(1)->name.compare("role") == 0);

			BOOST_CHECK(strcmp(res->get_row(0)->get_field("role")->data(), "dev") ==0);
			BOOST_CHECK(strcmp(res->get_row(1)->get_field("role")->data(), "admin") ==0);

		}

	}

	// Call free_database() when progress is destroyed.
	mysql::free_mysql_database();
}

// 测试插入操作事物处理
BOOST_AUTO_TEST_CASE(t_transaction){
	db_connector conn;
	conn.ip("192.168.1.107");
	conn.port(3306);
	conn.user("root");
	conn.pwd("1234");
	conn.dbname("project");

	db_connector* p = &conn;
	mysql::mysql_visitor v(p);

	bingo::error_what err;
	std::vector<string> sqls;
	{
		// Check error.
		sqls.push_back("insert into user (userName, password) values ('wolai','1111');");
		sqls.push_back("insert into user (userName, password, ab) values ('wolai99','1111','33');");
		BOOST_CHECK(v.query_result(sqls, err) == -1);

		cout << err.err_message() << endl;
	}

	{
		// Check correct.
		sqls.clear();

		sqls.push_back("insert into user (userName, password, gender) values ('wolai','1111', 0);");
		sqls.push_back("insert into user (userName, password, gender) values ('wolai99','222', 1);");
		BOOST_CHECK(v.query_result(sqls, err) == 0);
	}

	// Call free_database() when progress is destroyed.
	mysql::free_mysql_database();
}

BOOST_AUTO_TEST_SUITE_END()
