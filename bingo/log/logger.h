/*
 * logger.h
 *
 *  Created on: 2016-2-19
 *      Author: root
 */

#ifndef BINGO_LOG_LOGGER_HEADER_H_
#define BINGO_LOG_LOGGER_HEADER_H_

#include "log_data.h"

#include <vector>
#include <iostream>
using namespace std;

#include <boost/thread/thread.hpp>
using namespace boost;

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

namespace bingo { namespace log {

// Log's level is translate to string.
string level_to_string(int level);

// Log class.
class logger {
public:
	logger();
	virtual ~logger();

	void log_f(const char* tag, log_data& data);

	void log_e(const char* tag, log_data& data);

	void log_i(const char* tag, log_data& data);

	void log_d(const char* tag, log_data& data);

	// Log Level, LOG_LEVEL_OFF is default.
	enum {
		LOG_LEVEL_OFF 	= 0x00,
		LOG_LEVEL_FATAL = 0x01,
		LOG_LEVEL_ERROR = 0x02,
		LOG_LEVEL_INFO 	= 0x03,
		LOG_LEVEL_DEBUG = 0x04,
		LOG_LEVEL_ALL 	= 0x05,
	};

	// Current log's level, Default is LOG_LEVEL_ALL.
	int level();

	void level(int v);

public:
	virtual void log_out(string& msg){

	}

protected:

	mutex mu_;

	int cur_level_;
};


} }

#endif /* BINGO_LOG_LOGGER_HEADER_H_ */
