CC = gcc
CFLAGS = -O2 -fPIC -Wall
#LUADIR = /usr/local/include
LUADIR = ../../../LuaJIT/src/
TARGET = protobuf.so

.PHONY : all clean

all : $(TARGET)

$(TARGET) : pbc-lua.c
	$(CC) $(CFLAGS) -shared -o $@ -I../.. -I$(LUADIR) -L$(LUADIR) -L../../build $^ -lluajit -lpbc

clean :
	rm -f $(TARGET)
