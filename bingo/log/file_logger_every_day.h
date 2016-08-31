/*
 * file_logger_every_day.h
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#ifndef BINGO_LOG_FILE_LOGGER_EVERY_DAY_HEADER_H_
#define BINGO_LOG_FILE_LOGGER_EVERY_DAY_HEADER_H_

#include "logger.h"

#include <sys/stat.h>
#include <fcntl.h>

#include <fstream>
using namespace std;

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace bingo { namespace log {

template<typename PARSER>
class file_logger_every_day : public logger {
public:
	file_logger_every_day(): logger(){
	}
	virtual ~file_logger_every_day(){};

	void log_out(string& msg){

		string d(PARSER::directory);
		string f(PARSER::filename);

		try {

			{
				// lock part field.
				mutex::scoped_lock lock(mu_);

				// whether LOG_DIRECTORY is exist.
				if(access(d.c_str(), 0) == -1){
					// the directory isn't exist.
					// make new directory.
					mkdir(d.c_str(), S_IRWXU + S_IRGRP + S_IXGRP + S_IXOTH);
				}
			}


			// get filename.
			string filename(d.c_str());
			filename.append("/");
			filename.append(f.c_str());

			filename.append(".");
			get_filename_postfix(filename);

			{
				// lock part field.
				mutex::scoped_lock lock(mu_);

				// whether file is exist.
				if(access(filename.c_str(), 0) == -1){
					// the file isn't exist.
					// make new file.

					ofstream fs(filename.c_str());
					fs << msg.c_str() << endl;
					fs.close();

				}else{

					ofstream fs(filename.c_str(), ios_base::app);
					fs << msg.c_str() << endl;
					fs.close();
				}
			}


		}catch(std::exception& err){
		}
	}

public:
	virtual void get_filename_postfix(string& filename){
		date now = day_clock::local_day();
		int year = now.year();
		int month = now.month();
		int day = now.day();
		char cnow[9];
		memset(&cnow[0], 0x00, 9);
		sprintf(&cnow[0], "%4d%02d%02d", year, month, day);
		filename.append(cnow);
	}
};

} }

#endif /* BINGO_LOG_FILE_LOGGER_EVERY_DAY_HEADER_H_ */
