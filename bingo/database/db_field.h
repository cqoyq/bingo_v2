/*
 * db_field.h
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#ifndef BINGO_DATABASE_DB_FIELD_H_
#define BINGO_DATABASE_DB_FIELD_H_

#include <memory.h>
#include <string>
using namespace std;

#include "bingo/type.h"

namespace bingo { namespace database {

struct db_field_header {
public:
	db_field_header(const char* s){
		name = s;
	}
	string name;
};

class db_field {
public:
	db_field();
	db_field(const char* data, u64_t data_size);
	virtual ~db_field();

	const char* data();

	u64_t data_size();

protected:
	char* data_;
	u64_t data_size_;
};

} }

#endif /* BINGO_DATABASE_DB_FIELD_H_ */
