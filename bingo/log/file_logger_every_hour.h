/*
 * file_logger_every_day.h
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#ifndef BINGO_LOG_FILE_LOGGER_EVERY_HOUR_HEADER_H_
#define BINGO_LOG_FILE_LOGGER_EVERY_HOUR_HEADER_H_

#include "file_logger_every_day.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace bingo { namespace log {

template<typename PARSER>
class file_logger_every_hour : public file_logger_every_day<PARSER> {
public:
	file_logger_every_hour() : file_logger_every_day<PARSER>(){}
	virtual ~file_logger_every_hour(){};

	void get_filename_postfix(string& filename){
		date now = day_clock::local_day();
		int year = now.year();
		int month = now.month();
		int day = now.day();

		ptime p1 = second_clock::local_time();
		int hour = p1.time_of_day().hours();

		char cnow[11];
		memset(&cnow[0], 0x00, 11);
		sprintf(&cnow[0], "%4d%02d%02d%02d", year, month, day, hour);
		filename.append(cnow);
	}
};

} }

#endif /* BINGO_LOG_FILE_LOGGER_EVERY_HOUR_HEADER_H_ */
