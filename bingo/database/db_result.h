/*
 * db_result.h
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#ifndef BINGO_DATABASE_DB_RESULT_H_
#define BINGO_DATABASE_DB_RESULT_H_

#include <boost/ptr_container/ptr_vector.hpp>

#include <string.h>
using namespace std;

#include "bingo/type.h"
#include "db_row.h"

namespace bingo { namespace database {

class db_result {
public:
	db_result();
	virtual ~db_result();

	void add_header(db_field_header* header);

	void add_row(db_row* row);

	// Get db_row pointer, return o if fail.
	db_row* get_row(int index);

	// Get db_field_header pointer, return 0 if fail.
	db_field_header* get_header(int index);

	// Get index of db_field_header, return -1 if fail.
	int get_header_index(const char* header_name);

	int row_amount;
	int column_amount;

protected:
	boost::ptr_vector<db_row> rows_;
	boost::ptr_vector<db_field_header> header_;


};

} }

#endif /* BINGO_DATABASE_DB_RESULT_H_ */
