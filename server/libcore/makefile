
THIRD_CURL_INC := /usr/local/include/curl/

LIB_NAME := libcore.a
DLL_NAME := libcore.so

INC_PATH := -I$(THIRD_CURL_INC)
LIB_PATH :=
LIBS = -lssl -lcrypto -lz -lrt -lstdc++ -ldl

# 注意，这里的 -ldl 需要放在flag的最后才能生效
CXXFLAGS = -O2 -Wall -fPIC -pthread -std=c++11 $(INC_PATH) $(LIB_PATH) $(LIBS)
CFLAGS = -O2 -Wall -fPIC -pthread $(INC_PATH) $(LIB_PATH) $(LIBS)
CC = gcc
AR = ar
ARFLAGS = cr
DEBUG= -g -ggdb
BIN = ../../bin/lib

OBJS = common.o util.o systemsignal.o token.o log.o buffer.o packet.o object.o objectpool.o timer.o \
		epoll.o listener.o accept.o client.o http.o https.o multicurl.o handler.o node.o mainworker.o


COBJS = http_parser.o

all: $(COBJS) $(OBJS) $(LIB_NAME) $(DLL_NAME)

$(OBJS): %.o:%.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CXXFLAGS)

$(COBJS): %.o:%.c
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

$(LIB_NAME):$(OBJS) $(COBJS)
	$(AR) $(ARFLAGS) $(LIB_NAME) $(OBJS) $(COBJS)

$(DLL_NAME):$(OBJS) $(COBJS)
	$(CC) $(DEBUG) $(OBJS) $(COBJS) $(CXXFLAGS) -shared -o $(DLL_NAME)
#	$(RM) $(BIN)/$(DLL_NAME)
	cp $(DLL_NAME) $(BIN)/$(DLL_NAME)

clean:
	-$(RM) $(LIB_NAME)
	-$(RM) $(DLL_NAME)
	-$(RM) *.o

common.o:common.cpp common.h

util.o:common.o util.cpp util.h

systemsignal.o:systemsignal.cpp systemsignal.h

token.o:common.o token.cpp token.h

log.o:common.o log.cpp log.h

buffer.o:common.o log.o buffer.cpp buffer.h

packet.o:buffer.o packet.cpp packet.h

object.o:common.o object.cpp object.h

objectpool.o:object.o objectpool.cpp objectpool.h

timer.o:common.o objectpool.o timer.cpp timer.h

epoll.o:common.o epoll.cpp epoll.h

listener.o:epoll.o objectpool.o listener.h listener.cpp

accept.o:epoll.o objectpool.o packet.o util.o timer.o accept.cpp accept.h

client.o:accept.o client.h client.cpp

http_parser.o: http_parser.h http_parser.c

http.o:http_parser.o token.o epoll.o objectpool.o packet.o util.o timer.o http.h http.cpp

https.o:http.o https.h https.cpp

multicurl.o:epoll.o objectpool.o packet.o util.o timer.o multicurl.h multicurl.cpp

handler.o:packet.o util.o client.o https.o listener.o handler.h handler.cpp

node.o:packet.o util.o client.o https.o listener.o node.h node.cpp

mainworker.o:accept.o client.o http.o https.o handler.o multicurl.o mainworker.cpp mainworker.h










