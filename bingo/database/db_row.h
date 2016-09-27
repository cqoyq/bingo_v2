/*
 * db_row.h
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#ifndef BINGO_DATABASE_DB_ROW_H_
#define BINGO_DATABASE_DB_ROW_H_

#include <boost/ptr_container/ptr_vector.hpp>

#include <string>
using namespace std;

#include "bingo/type.h"
#include "db_field.h"

namespace bingo { namespace database {

class db_row {
public:
	db_row(void* res);
	virtual ~db_row();

	void add_field(db_field* field);

	db_field* get_field(int index);
	db_field* get_field(const char* field_name);

	int size();

protected:
	boost::ptr_vector<db_field> fields_;

	// db_result pointer.
	void* res_;
};

} }

#endif /* BINGO_DATABASE_DB_ROW_H_ */
