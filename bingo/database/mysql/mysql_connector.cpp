/*
 * mysql_connector.cpp
 *
 *  Created on: 2016-10-18
 *      Author: root
 */

#include "mysql_connector.h"
#include "bingo/define.h"
#include "bingo/string.h"
using namespace bingo;

using namespace bingo::database::mysql;

mysql_connector::mysql_connector(db_connector* conn_info) {
	// TODO Auto-generated constructor stub
	conn_info_ = conn_info;
	mysql_conn_ = 0;
	is_connect_success = false;
}

mysql_connector::~mysql_connector() {
	// TODO Auto-generated destructor stub
	if(mysql_conn_!=0){

		mysql_close(mysql_conn_);

		mysql_thread_end();
	}
}

int mysql_connector::connect(){

	// Init mysql connector.
	mysql_conn_ = mysql_init(NULL);

	my_bool reconnect = 0;
	mysql_options(mysql_conn_, MYSQL_SECURE_AUTH, &reconnect);

	// Connect mysql's server.
	if (!mysql_real_connect(mysql_conn_,
			conn_info_->ip(), conn_info_->user(), conn_info_->pwd(), conn_info_->dbname(), conn_info_->port(),
			NULL, 0)) {

		err_.err_no(error_database_connect_fail);
		err_.err_message("database connect fail");

		// free Mysql.
		mysql_close(mysql_conn_);

		mysql_thread_end();

		mysql_conn_ = 0;

		return -1;
	}

	mysql_set_character_set(mysql_conn_, "utf8");

#ifdef BINGO_MYSQL_DEBUG
	message_out_with_time("database connect success, mysql hdr:" << mysql_conn_)
#endif

	is_connect_success = true;

	return 0;
}

bingo::error_what& mysql_connector::err(){
	return err_;
}

MYSQL*& mysql_connector::get_mysql(){
	return mysql_conn_;
}

int mysql_connector::query_result(const char* sql, db_result*& result, error_what& e_what){
	// Execute sql statement.
	mysql_query(mysql_conn_, sql);
	const char* er = mysql_error(mysql_conn_);
	if((*er) != 0x00){

		string_append err_msg;
		err_msg.add("database query fail! error info:")->add(er);

		err_.err_no(error_database_query_fail);
		err_.err_message(err_msg.to_string().c_str());

		return -1;
	}


	// Starting to handle return of result.
	MYSQL_RES *res;
	MYSQL_ROW row;

	// Get MYSQL_RES.
	res = mysql_store_result(mysql_conn_);

	// Get row number and column number in MYSQL_RES.
	result->row_amount 	= mysql_num_rows(res);
	result->column_amount = mysql_num_fields(res);


	// Get column data in MYSQL_RES.
	MYSQL_FIELD *columns;
	columns = mysql_fetch_fields(res);
	for(int i = 0; i < result->column_amount; i++)
	{
		result->add_header(new db_field_header(columns[i].name));
	}

	// Get row data in MYSQL_RES.
	while((row = mysql_fetch_row(res))!= NULL) {

		u64_t *FieldLength = mysql_fetch_lengths(res);

		db_row* d_row = new db_row(result);

		// Save field data.
		for(int i = 0; i<result->column_amount; i++ ){
			// Get field length.
			u64_t field_size  = (u64_t)FieldLength[i];

			// Get field value.
			char* field_value = row[i];

			d_row->add_field(new db_field(field_value, field_size));
		}

		// Save row data.
		result->add_row(d_row);
	}

	mysql_free_result(res);

	return 0;
}

int mysql_connector::query_result(vector<string>& sqls, boost::ptr_vector<db_result>& results, error_what& e_what){
	for (size_t n = 0;  n< sqls.size(); n++){

		if(strcmp(sqls[n].c_str(),"") == 0) continue;

		// Execute sql statement.
		mysql_query(mysql_conn_, sqls[n].c_str());
		const char* er = mysql_error(mysql_conn_);
		if((*er) != 0x00){
			string_append err_msg;
			err_msg.add("database query fail! error info:")->add(er);

			err_.err_no(error_database_query_fail);
			err_.err_message(err_msg.to_string().c_str());

			return -1;
		}


		// Starting to handle return of result.
		MYSQL_RES *res;
		MYSQL_ROW row;

		db_result* result = new db_result();

		// Add result to vector.
		results.push_back(result);


		// Get MYSQL_RES.
		res = mysql_store_result(mysql_conn_);

		if(res){

			// Get row number and column number in MYSQL_RES.
			result->row_amount 	= mysql_num_rows(res);
			result->column_amount= mysql_num_fields(res);


			// Get column data in MYSQL_RES.
			MYSQL_FIELD *columns;
			columns = mysql_fetch_fields(res);
			for(int i = 0; i < result->column_amount; i++)
			{
				result->add_header(new db_field_header(columns[i].name));
			}

			// Get row data in MYSQL_RES.
			while((row = mysql_fetch_row(res))!= NULL) {

				u64_t *FieldLength = mysql_fetch_lengths(res);

				db_row* d_row = new db_row(result);

				// Save field data.
				for(int i = 0; i<result->column_amount; i++ ){
					// Get field length.
					u64_t field_size  = (u64_t)FieldLength[i];

					// Get field value.
					char* field_value = row[i];

					d_row->add_field(new db_field(field_value, field_size));
				}

				// Save row data.
				result->add_row(d_row);
			}

		}

		mysql_free_result(res);

	}

	return 0;
}

int mysql_connector::query_result(vector<string>& sqls, error_what& e_what){
	// Start trasaction.
	mysql_query(mysql_conn_, "start transaction;");

	for(size_t i=0;i< sqls.size(); i++){

		if(sqls[i] != ""){

			mysql_query(mysql_conn_, sqls[i].c_str());
			const char* er = mysql_error(mysql_conn_);
			if((*er) != 0x00){

				string_append err_msg;
				err_msg.add("database query fail! error info:")->add(er);

				err_.err_no(error_database_query_fail);
				err_.err_message(err_msg.to_string().c_str());

				// Roll back transaction.
				mysql_query(mysql_conn_, "rollback;");

				return -1;
			}
		}
	}

	// Commit transaction.
	mysql_query(mysql_conn_, "commit;");

	return 0;
}

