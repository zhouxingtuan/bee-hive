
JEMALLOC_DIR := ./../../third/jemalloc
#JEMALLOC_LIB := $(JEMALLOC_DIR)/lib/libjemalloc_pic.a $(JEMALLOC_DIR)/lib/libjemalloc.a
JEMALLOC_INC := $(JEMALLOC_DIR)/include/jemalloc/
JEMALLOC_DLL_PATH := $(JEMALLOC_DIR)/lib

#CURL_LIB := /usr/local/lib/libcurl.a
CURL_INC := /usr/local/include/curl/

#CORE_LIB := ./../libcore/libcore.a
CORE_INC := ./../libcore/
CORE_DLL_PATH := ./../libcore/

# 静态库的引用需要注意顺序，最底层的静态库放在后面
STATIC_LIB := $(CORE_LIB) $(CURL_LIB) $(JEMALLOC_LIB)

# static的依赖库要放在static的后面，比如luasocket依赖openssl
LIBPATH = -L/usr/local/lib -L$(CORE_DLL_PATH) -L$(JEMALLOC_DLL_PATH)
LIBS = -lcore -ljemalloc -lcurl -lssl -lcrypto -lz -lrt -lstdc++ -ldl

INCLUDES = -I$(JEMALLOC_INC) -I$(CORE_INC) -I$(CURL_INC)

# 注意，这里的 -ldl 需要放在flag的最后才能生效
CFLAGS = -O2 -Wall -fPIC -pthread -std=c++11 $(INCLUDES)

CC = gcc
DEBUG= -g -ggdb
RM = rm -f
BIN = ../../bin
TARGET = hive

OBJS = configloader.o commandfunction.o hivehandler.o

$(TARGET): main.o $(OBJS)
	$(MAKE) -C ./../libcore
	$(CC) $(DEBUG) main.o $(OBJS) $(STATIC_LIB) $(LIBPATH) $(LIBS) -o $(TARGET) $(CFLAGS)
	cp $(TARGET) $(BIN)
#	cp $(JEMALLOC_DLL_PATH)/libjemalloc.so.2 $(BIN)/lib

main.o: main.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

$(OBJS): %.o:%.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

configloader.o:configloader.cpp configloader.h

commandfunction.o:commandfunction.cpp commandfunction.h

hivehandler.o:hivehandler.cpp hivehandler.h


clean:
	-$(RM) $(TARGET)
	-$(RM) *.o





