INCLUDE = -I$(BOOST_INCLUDE_ROOT) \
          -I$(MYSQL_INCLUDE_ROOT) \
          -I./

LIBS = -lpthread \
	   -lmysqlclient_r \
	   -lsqlite3 \
	   -lrt \
	   -lboost_random \
	   -lboost_regex \
       -lboost_chrono \
	   -lboost_timer \
	   -lboost_system \
	   -lboost_thread \
	   -lboost_context \
	   -lboost_coroutine \
       -lboost_unit_test_framework
	  
LIBPATH = -L$(BOOST_LIB_ROOT) \
		   -L$(MYSQL_LIB_ROOT)

OBJS =  main.o \
		bingo/config/json/parse_handler.o \
		bingo/config/xml/parse_handler.o \
		bingo/log/logger.o \
		bingo/database/db_field.o \
		bingo/database/db_row.o \
		bingo/database/db_result.o \
		bingo/database/db_factory.o \
		bingo/database/db_connector.o \
		bingo/database/mysql/mysql_visitor.o \
		bingo/string.o \
		bingo/error_what.o \
		test/string_ex.o \
		test/singleton.o \
		test/mem_guard.o \
		test/tcp/server_t1.o \
		test/tcp/server_t2.o \
		test/tcp/server_t3.o \
		test/tcp/server_t4.o \
		test/tcp/server_t5.o \
		test/tcp/server_t6.o \
		test/tcp/server_t7.o \
		test/tcp/server_t8.o \
		test/tcp/server_t9.o \
		test/tcp/server_t10.o \
		test/tcp/client_t1.o \
		test/tcp/client_t2.o \
		test/tcp/client_t3.o \
		test/tcp/client_t4.o \
		test/tcp/client_t5.o \
		test/log/file_log.o \
		test/config/xml.o \
		test/config/json.o \
		test/thread/many_to_one_t1.o \
		test/thread/many_to_one_t2.o \
		test/thread/many_to_one_t3.o \
		test/thread/many_to_many_t1.o \
		test/thread/many_to_many_t2.o \
		test/thread/many_to_many_t3.o \
		test/thread/many_to_many_t4.o \
		test/process/shm_t.o \
		test/database/test_mysql_visitor.o
		
CPPS =  bingo/string.cpp \
		bingo/error_what.cpp \
		bingo/log/logger.cpp \
		bingo/database/db_field.cpp \
		bingo/database/db_row.cpp \
		bingo/database/db_result.cpp \
		bingo/database/db_factory.cpp \
		bingo/database/db_connector.cpp \
		bingo/database/mysql/mysql_visitor.cpp \
		bingo/config/xml/parse_handler.cpp \
		bingo/config/json/parse_handler.cpp
		
ifeq ($(ShowDebug),y)
DEBUGS = -DBINGO_TCP_SERVER_DEBUG \
		 -DBINGO_TCP_CLIENT_DEBUG \
		 -DBINGO_THREAD_TASK_DEBUG \
		 -DBINGO_PROCESS_SHARED_MEMORY_DEBUG
else
DEBUGS =
endif

ifeq ($(findstring Test_Debug,$(ConfigName)),Test_Debug)
	CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11 $(DEBUGS) $(INCLUDE) 
	TARGET = mytest
else ifeq ($(findstring Lib_Debug,$(ConfigName)),Lib_Debug)
	CXXFLAGS =	-O2 -g -fPIC  -shared -std=c++11 $(INCLUDE)
	TARGET = libbingo.so
else ifeq ($(findstring Lib_Release,$(ConfigName)),Lib_Release)
	CXXFLAGS =	-O2 -fPIC  -shared -std=c++11 $(INCLUDE)
	TARGET = libbingo.so
endif

ifeq ($(findstring Test_,$(ConfigName)),Test_)
$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS) $(LIBPATH);

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
else ifeq ($(findstring Lib_,$(ConfigName)),Lib_)
all:
	$(CXX)  $(CXXFLAGS) -o $(TARGET) $(CPPS) $(LIBS) $(LIBPATH);
	rm -fR $(MY_LIB_ROOT)/bingo
	cp $(TARGET) $(MY_LIB_ROOT);
	cp -fR bingo $(MY_LIB_ROOT);
	find $(MY_LIB_ROOT)/bingo/ -regextype "posix-egrep" -regex '.*\.(o|cpp)' -exec rm -rf {} \;

clean:
	rm -f $(TARGET)
endif


