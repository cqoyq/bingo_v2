/*
 * mysql_pool.cpp
 *
 *  Created on: 2016-10-18
 *      Author: root
 */

#include "mysql_pool.h"
#include "mysql_timer.h"
using namespace bingo::database::mysql;
#include "bingo/define.h"
using namespace bingo;

mysql_timer* heartjump_;

mysql_pool::mysql_pool() {
	// TODO Auto-generated constructor stub
	heartjump_ = 0;
}

mysql_pool::~mysql_pool() {
	// TODO Auto-generated destructor stub
	while(conns_.size()>0){

		connector* conn = conns_.front();
		conns_.pop();

		delete conn;
	}

	if(heartjump_ != 0) delete heartjump_;

	mysql_library_end();
}

bingo::error_what& mysql_pool::err(){
	return err_;
}

int mysql_pool::make_connector_pool(size_t n,
		db_connector* conn_info){


	for (size_t i = 0; i < n; i++) {

		connector* conn = new connector(conn_info);

		// Connect to database.
		if(conn->connect() == -1){

			err_.err_message(conn->err().err_message().c_str());
			err_.err_no(conn->err().err_no());

			delete conn;

			return -1;
		}

		conns_.push(conn);
	}

	// Make heart jump.
	heartjump_ = new mysql_timer(ios_, this);
	heartjump_->schedule_timer();

	ios_.run();

	return 0;
}

mysql_pool::connector* mysql_pool::get_connector(){

	// lock part field.
	mutex::scoped_lock lock(mu_);

	if(conns_.size() > 0){
		connector* conn = conns_.front();
		conns_.pop();

		return conn;
	}else
		return 0;
}

void mysql_pool::free_connector(connector*& conn){
	// lock part field.
	mutex::scoped_lock lock(mu_);

	conns_.push(conn);
}

size_t mysql_pool::size(){
	return conns_.size();
}

void mysql_pool::check_heartjump(){
	// lock part field.
	mutex::scoped_lock lock(mu_);

	size_t max = conns_.size();

	for (size_t i = 0; i < max; i++) {
		// Pop item from queue.
		connector* conn = conns_.front();
		conns_.pop();

		// Check whether connect success.
		if(!conn->is_connect_success){
			if(conn->connect() == -1){
#ifdef BINGO_MYSQL_DEBUG
				message_out_with_time(
					"reconnect fail!mysql hdr:" << conn <<
					",error info:" << conn->err().err_message())
#endif
			}

			// Push item to queue.
			conns_.push(conn);

			continue;
		}

		// Send heartjump sql.
		{
			string sql("select version();");

			// Execute sql statement.
			mysql_query(conn->get_mysql(), sql.c_str());
			const char* er = mysql_error(conn->get_mysql());
			if((*er) != 0x00){

#ifdef BINGO_MYSQL_DEBUG
				message_out_with_time(
					"heart jump index:" <<i <<
					",mysql hdr:" << conn->get_mysql() <<
					",query fail, error info:" << er)
#endif

				// Set is_connect_success is false.
				conn->is_connect_success = false;

			}else{

				// Starting to handle return of result.
				MYSQL_RES *res;

				// Get MYSQL_RES.
				res = mysql_store_result(conn->get_mysql());

				mysql_free_result(res);

	#ifdef BINGO_MYSQL_DEBUG
				message_out_with_time(
					"heart jump index:" << i <<
					",mysql hdr:" << conn->get_mysql() <<
					",query success");
	#endif
			}

		}

		// Push item to queue.
		conns_.push(conn);
	}
}
