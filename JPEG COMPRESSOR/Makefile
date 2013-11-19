RM = /bin/rm
TARGET = runTestJPEG

all : $(TARGET)

runTestJPEG : jpegCompress.o testJPEG.o
	g++ $< testJPEG.o -o $@ `pkg-config opencv --libs`

jpegCompress.o : jpegCompress.cpp
	g++ -c `pkg-config opencv --cflags` $< -o $@

testJPEG.o : testJPEG.cpp
	g++ -c `pkg-config opencv --cflags` $< -o $@

clean :
	$(RM) *.o $(TARGET)
