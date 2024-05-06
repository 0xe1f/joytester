INCLUDE=-I/usr/include/SDL
LIB=-lSDL -ludev
DEFINE=
CFLAGS=-Wall -O2
CXXFLAGS=$(CFLAGS)
LDFLAGS=$(LIB)
EXE=joytester

all: $(EXE)

$(EXE): $(EXE).o
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(INCLUDE) $(DEFINE) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(INCLUDE) $(DEFINE) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf *.o $(EXE)
