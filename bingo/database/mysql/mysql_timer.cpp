/*
 * mysql_timer.cpp
 *
 *  Created on: 2016-10-18
 *      Author: root
 */

#include "mysql_timer.h"
#include "bingo/define.h"
using namespace bingo;

using namespace bingo::database::mysql;

mysql_timer::mysql_timer(io_service& io, mysql_pool* pool):
		timer_(io),
		pool_(pool){

}

mysql_timer::~mysql_timer(){

}

void mysql_timer::schedule_timer(){
	minutes s(MAX_TIME_INTERVAL_MINUTE);
	time_duration td = s;
	timer_.expires_from_now(td);
	timer_.async_wait(bind(&mysql_timer::time_out_handler, this, boost::asio::placeholders::error));
}

void mysql_timer::time_out_handler(const system::error_code& ec){
	if(ec){

	}else{
		// Send heartjump to mysql server.
		pool_->check_heartjump();

		// Reset timer.
		schedule_timer();
	}
}

