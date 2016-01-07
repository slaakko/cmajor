include ../Makefile.common

OBJECTS = SocketApi.o SocketLib.o SocketUtil.o TcpSocket.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libsockets.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libsockets.a

