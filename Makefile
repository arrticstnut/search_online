INC_DIR:= \
	./include/ \
	./include/task/ \
	./include/tcpSer/ \
	./include/threadPool/ \
	./include/cacle/ \
	./include/timer/ \
	./include/configuration/ \
	./include/onlineServer/ \
	./include/wordSegmentation/ \
	./include/wordSegmentation/cppjieba/ \
	./include/wordSegmentation/cppjieba/limonp \
	./include/redis \
	./include/log \
	/usr/local/include \
	
#SRC_DIR:= 
#	./src/ \
#	./src/task/ \
#	./src/tcpSer/ \
#	./src/threadPool/ \
#	./src/cacle/ \
#	./src/timer/ \
#	./src/cppjieba/ \
#	./src/configuration/ \

SRCS:=\
	$(wildcard ./src/*.cc) \
	$(wildcard ./src/threadPool/*.cc) \
	$(wildcard ./src/tcpSer/*.cc) \
	$(wildcard ./src/timer/*.cc) \
	$(wildcard ./src/configuration/*.cc) \
	$(wildcard ./src/cacle/*.cc) \
	$(wildcard ./src/task/*.cc) \
	$(wildcard ./src/onlineServer/*.cc) \
	$(wildcard ./src/wordSegmentation/*.cc) \
	$(wildcard ./src/redis/*.cc) \
	$(wildcard ./src/log/*.cc) \

OBJS:= $(patsubst %.cc, %.o, $(SRCS))
LIBS:= -llog4cpp -lpthread -ljson -lhiredis -lmysqlclient

CXX:=g++

CXXFLAGS:= -Wall -std=c++11 $(addprefix -I, $(INC_DIR)) $(LIBS) -Wno-deprecated

EXE:=./bin/ser

$(EXE):$(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(CXXFLAGS)

clean:
#	rm -rf $(EXE)
	rm -rf $(OBJS)

