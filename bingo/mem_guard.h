/*
 * mem_guard.h
 *
 *  Created on: 2016-1-30
 *      Author: root
 */

#ifndef BINGO_MEM_GUARD_HEADER_H_
#define BINGO_MEM_GUARD_HEADER_H_

#include "string.h"

namespace bingo {

template<typename T>
class mem_guard{
private:
	char data_[sizeof(T)];

	size_t size_;
	size_t length_;
public:
	enum {	error_copy_data_exceed_max_size 	= 0x01,
			error_append_data_exceed_max_size	= 0x02,
			error_change_length_exceed_max_size	= 0x03};
public:
	mem_guard()
		: size_(sizeof(T)),
		  length_(0)
	{
		memset(&data_[0], 0x00, size_);
	}

	void clear(){
		memset(&data_[0], 0x00, size_);
		length_ = 0;
	}

	// Get pointer to T.
	T* object(){
		return (T*)data_;
	}

	size_t& size(){
		return size_;
	}

	// Return header pointer of block.
	char* header(){
		return data_;
	}

	// Return length of data.
	size_t& length(){
		return length_;
	}

	// Return current pointer in block, value is 0 if arrive end of data_.
	char* current() const{
		return (length_ == size_)? 0: const_cast<char*>(data_ + length_);
	}

	// Copy data into block, success return 0, fail return -1.
	int copy(const char* data, size_t data_size, u8_t& err_code){

		if(data_size > size_){

			err_code = error_copy_data_exceed_max_size;
			return -1;
		}

		memcpy(data_, data, data_size);
		length_ = data_size;

		return 0;
	}

	int copy(const char* data, size_t data_size, int& err_code){

		if(data_size > size_){

			err_code = error_copy_data_exceed_max_size;
			return -1;
		}

		memcpy(data_, data, data_size);
		length_ = data_size;

		return 0;
	}

	// Copy data into block, success return 0, fail return -1.
	int copy(const unsigned char* data, size_t data_size, u8_t& err_code){

		if(data_size > size_){

			err_code = error_copy_data_exceed_max_size;
			return -1;
		}

		memcpy(data_, data, data_size);
		length_ = data_size;

		return 0;
	}

	// Append data into block, success return 0, fail return -1.
	int append(const char* data, size_t data_size, u8_t& err_code){

		if((length_ + data_size) > size_){

			err_code = error_append_data_exceed_max_size;
			return -1;
		}

		memcpy(data_ + length_, data, data_size);
		length_ += data_size;

		return 0;
	}

	// Check remain space in block, return true so pass.
	bool check_block_remain_space(size_t& from_data_size){

		if((length_ + from_data_size) > size_)
			return false;
		else
			return true;
	}



	// Change length_
	int change_length(size_t len, u8_t& err_code){
		if((length_ + len) > size_){
			err_code = error_change_length_exceed_max_size;
			return -1;
		}

		length_ += len;

		return 0;
	}

};

}


#endif /* BINGO_MEM_GUARD_HEADER_H_ */
