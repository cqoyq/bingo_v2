/*
 * logger.cpp
 *
 *  Created on: 2016-7-3
 *      Author: root
 */

#include "logger.h"

using namespace bingo::log;

#define get_now(now) { \
		ptime p1 = boost::posix_time::microsec_clock::local_time(); \
		int year = p1.date().year(); \
		int month = p1.date().month(); \
		int day = p1.date().day(); \
		\
		int hour = p1.time_of_day().hours(); \
		int minute = p1.time_of_day().minutes(); \
		int second = p1.time_of_day().seconds(); \
		\
		int millsecond = p1.time_of_day().total_milliseconds() - p1.time_of_day().total_seconds()*1000; \
		\
		char ctime[20]; \
		memset(&ctime[0], 0x00, 20); \
		\
		sprintf(&ctime[0], "%4d-%02d-%02d %02d:%02d:%02d.%03d", year, month, day, hour, minute, second, millsecond); \
		now.append(ctime); \
}

logger::logger(){
	cur_level_ = LOG_LEVEL_ALL;
}

logger::~logger(){

}

void logger::log_f(const char* tag, log_data& data){
	if(cur_level_ < LOG_LEVEL_FATAL){
		return;
	}else{
		string msg;

		string now;
		get_now(now);
		msg.append(now);
		msg.append("@");
		msg.append("fatal");
		msg.append("@");
		msg.append(tag);
		msg.append("@");
		msg.append(data.str());

		log_out(msg);
	}
}

void logger::log_e(const char* tag, log_data& data){

	if(cur_level_ < LOG_LEVEL_ERROR){
		return;
	}else{
		string msg;

		string now;
		get_now(now);
		msg.append(now);
		msg.append("@");
		msg.append("error");
		msg.append("@");
		msg.append(tag);
		msg.append("@");
		msg.append(data.str());

		log_out(msg);
	}
}

void logger::log_i(const char* tag, log_data& data){
	if(cur_level_ < LOG_LEVEL_INFO){
		return;
	}else{
		string msg;

		string now;
		get_now(now);
		msg.append(now);
		msg.append("@");
		msg.append("info");
		msg.append("@");
		msg.append(tag);
		msg.append("@");
		msg.append(data.str());

		log_out(msg);
	}
}

void logger::log_d(const char* tag, log_data& data){
	if(cur_level_ < LOG_LEVEL_DEBUG){
		return;
	}else{
		string msg;

		string now;
		get_now(now);
		msg.append(now);
		msg.append("@");
		msg.append("debug");
		msg.append("@");
		msg.append(tag);
		msg.append("@");
		msg.append(data.str());

		log_out(msg);
	}
}

int logger::level(){
	// lock part field.
	mutex::scoped_lock lock(mu_);

	return cur_level_;
}

void logger::level(int v){
	// lock part field.
	mutex::scoped_lock lock(mu_);

	cur_level_ = v;
}



