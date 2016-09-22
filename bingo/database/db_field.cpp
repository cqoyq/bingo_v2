/*
 * dbfield.cpp
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#include "db_field.h"

//using namespace bingo;
using namespace bingo::database;

db_field::db_field() {
	// TODO Auto-generated constructor stub
	data_ = 0;
	data_size_ = 0;
}

db_field::db_field(const char* data, u64_t data_size){
	// Set data value.
	if(data == 0)
	{
		data_=new char[1];
		memset(data_, 0x00, 1);
	}
	else
	{
		data_=new char[data_size + 1];
		memset(data_, 0x00, data_size + 1);
		memcpy(data_, data, data_size);
	};

	data_size_ = data_size;
}

db_field::~db_field() {
	// TODO Auto-generated destructor stub
	if(data_ != 0) delete[] data_;
}

const char* db_field::data(){
	return data_;
}

bingo::u64_t db_field::data_size(){
	return data_size_;
}

