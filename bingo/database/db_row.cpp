/*
 * dbrow.cpp
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#include "db_result.h"

#include "db_row.h"

using namespace bingo;
using namespace bingo::database;

// ----------------------------------  db_row ------------------------------------  //

db_row::db_row(void* res){
	res_ = res;
}

db_row::~db_row(){

}

void db_row::add_field(db_field* field){
	fields_.push_back(field);
}

db_field* db_row::get_field(int index){
	if(index >=0 && fields_.size()>0 && index <= (fields_.size() - 1)){
		return &(fields_[index]);
	}else{
		return 0;
	}
}

db_field* db_row::get_field(const char* field_name){
	db_result* p = static_cast<db_result*>(res_);

	int index = p->get_header_index(field_name);
	if(index == -1)
		return 0;
	else{
		return &(fields_[index]);
	}
}

int db_row::size(){
	return fields_.size();
}



