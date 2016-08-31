/*
 * file_logger_without_postfix.h
 *
 *  Created on: 2016-8-31
 *      Author: root
 */

#ifndef BINGO_LOG_FILE_LOGGER_WITHOUT_POSTFIX_H_
#define BINGO_LOG_FILE_LOGGER_WITHOUT_POSTFIX_H_

#include "file_logger_every_day.h"

namespace bingo { namespace log {

template<typename PARSER>
class file_logger_without_postfix : public file_logger_every_day<PARSER> {
public:
	file_logger_without_postfix() : file_logger_every_day<PARSER>(){}
	virtual ~file_logger_without_postfix(){};

	void get_filename_postfix(string& filename){
	}
};

} }

#endif /* BINGO_LOG_FILE_LOGGER_WITHOUT_POSTFIX_H_ */
