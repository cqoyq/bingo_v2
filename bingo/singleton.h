/*
 * singleton.h
 *
 *  Created on: 2016-1-18
 *      Author: root
 */

#ifndef BINGO_SINGLETON_HEADER_H_
#define BINGO_SINGLETON_HEADER_H_

#include <boost/assert.hpp>

namespace bingo {

// Singleton class which construct without argument.
template<typename T>
class singleton_v0 {
private:
	static T* self_;

public:
	singleton_v0(){};
	~singleton_v0(){};

public:
	static void construct(){
		if(self_ == 0){
			self_ = new T();
		}
	}

	static T*& instance(){
		BOOST_ASSERT(self_ != 0);
		return self_;
	}

	static void release(){
		if(self_ != 0) {
			delete self_;
			self_ = 0;
		}
	}
};
template<typename T>
T* singleton_v0<T>::self_ = 0;

// Singleton class which construct with one argument.
template<typename T, typename P1>
class singleton_v1 {
public:
	singleton_v1(){};
	~singleton_v1(){};

public:
	static void construct(P1 v1){
		if(self_ == 0){
			self_ = new T(v1);
		}
	}

	static T*& instance(){
		BOOST_ASSERT(self_ != 0);
		return self_;
	}

	static void release(){
		if(self_ != 0) {
			delete self_;
			self_ = 0;
		}
	}

protected:
	static T* self_;
};
template<typename T, typename P1>
T* singleton_v1<T, P1>::self_ = 0;

// Singleton class which construct with two argument.
template<typename T, typename P1, typename P2>
class singleton_v2 {
public:
	singleton_v2(){};
	~singleton_v2(){};

public:
	static void construct(P1 v1, P2 v2){
		if(self_ == 0){
			self_ = new T(v1, v2);
		}
	}

	static T*& instance(){
		BOOST_ASSERT(self_ != 0);
		return self_;
	}

	static void release(){
		if(self_ != 0) {
			delete self_;
			self_ = 0;
		}
	}

protected:
	static T* self_;
};
template<typename T, typename P1, typename P2>
T* singleton_v2<T, P1, P2>::self_ = 0;

// Singleton class which construct with three argument.
template<typename T, typename P1, typename P2, typename P3>
class singleton_v3 {
public:
	singleton_v3(){};
	~singleton_v3(){};

public:
	static void construct(P1 v1, P2 v2, P3 v3){
		if(self_ == 0){
			self_ = new T(v1, v2, v3);
		}
	}

	static T*& instance(){
		BOOST_ASSERT(self_ != 0);
		return self_;
	}

	static void release(){
		if(self_ != 0) {
			delete self_;
			self_ = 0;
		}
	}

protected:
	static T* self_;
};
template<typename T, typename P1, typename P2, typename P3>
T* singleton_v3<T, P1, P2, P3>::self_ = 0;

}




#endif /* BINGO_SINGLETON_HEADER_H_ */
