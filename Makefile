
CXX = /usr/gcc-4.9/bin/g++
CFLAGS = -g -Wall -fPIC -std=c++11 -march=x86-64 -Wno-deprecated -Wno-unused-function -fdiagnostics-color=auto 
LIB_DIR = -L/usr/lib64/ -L/lib64/

LIB_NAME = -lgtest -lboost_unit_test_framework -lpthread

.PHONY: clean

target: test_lock test_ThreadSafe


test_lock: test_Lock.o Lock.o 
	$(CXX) $(CFLAGS) -o $@  $(LIB_DIR) $(LIB_NAME) $^ 

test_ThreadSafe: test_ThreadSafe.o Lock.o
	$(CXX) $(CFLAGS) -o $@  $(LIB_DIR) $(LIB_NAME) $^ 

test_ThreadPool: test_ThreadPool.o ThreadPool.o Lock.o
	$(CXX) $(CFLAGS) -o $@  $(LIB_DIR) $(LIB_NAME) $^ 

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f test_lock
	rm -f test_ThreadSafe
