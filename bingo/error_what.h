/*
 * error_what.h
 *
 *  Created on: 2016-9-16
 *      Author: root
 */

#ifndef BINGO_ERROR_WHAT_H_
#define BINGO_ERROR_WHAT_H_

#include <string.h>
using namespace std;

namespace bingo {
class error_what {
public:
	error_what(){
		err_no_ = 0;
	};
	virtual ~error_what(){};

	string& err_message(){
		return err_;
	}

	void err_message(const char* err){
		err_ = err;
	}

	int err_no(){
		return err_no_;
	}

	void err_no(int n){
		err_no_ = n;
	}

private:
	string err_;
	int err_no_;
};
}


#endif /* BINGO_ERROR_WHAT_H_ */
