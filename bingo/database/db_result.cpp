/*
 * dbresult.cpp
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#include <boost/algorithm/string.hpp>
#include "../foreach_.h"

#include "db_result.h"

using namespace bingo::database;

db_result::db_result() {
	// TODO Auto-generated constructor stub
	row_amount = 0;
	column_amount = 0;
}

db_result::~db_result() {
	// TODO Auto-generated destructor stub
}

void db_result::add_header(db_field_header* header){
	header_.push_back(header);
}

void db_result::add_row(db_row* row){
	rows_.push_back(row);
}

db_row* db_result::get_row(int index){
	if(index >=0 && rows_.size()>0 && index <= (rows_.size() - 1)){
		return &(rows_[index]);
	}else{
		return 0;
	}
}

db_field_header* db_result::get_header(int index){
	if(index >=0 && header_.size()>0 && index <= (header_.size() - 1)){
		return &(header_[index]);
	}else{
		return 0;
	}
}

int db_result::get_header_index(const char* header_name){
	int result = -1,  index = 0;
	foreach_(db_field_header& n, header_){

		// @s_name1 and @s_name2 change to lower both.
		string s_name1 = boost::to_lower_copy(n.name);
		string s_name2 = header_name;
		boost::to_lower(s_name2);

		if(s_name1.compare(s_name2.c_str()) == 0){
			result = index;
			break;
		}
		index++;
	}

	return result;
}

