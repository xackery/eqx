.PHONY: build-docker-all
build-docker-all:
	@docker run -v ${PWD}:/src terrifried

CFLAGS = -g -Wall 

COMMON_FILES := $(shell find src/common -name '*.cpp')
LOG_FILES := $(shell find src/log -name '*.cpp')
all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c
 
clean:
	$(RM) $(TARGET)

%.o: %.cpp
	@$(CC) $(CFLAGS) $< -c

build-windows:
	@x86_64-w64-mingw32-g++ -lz $(CFLAGS) -std=c++17 -Idependencies/glm/glm/ -Isrc/log/ -Isrc/common/ -o bin/eqx.exe src/eqx/eqx.cpp  $(COMMON_FILES) $(LOG_FILES)

build-linux:
	@c++ $(CFLAGS) -std=c++17 -lz -Idependencies/glm/glm/ -Isrc/log/ -Isrc/common/ -o bin/eqx src/eqx/eqx.cpp $(COMMON_FILES) $(LOG_FILES)

build-osx:
	@c++ $(CFLAGS) -std=c++17 -lz -Idependencies/glm/glm/ -Isrc/log/ -Isrc/common/ -o bin/eqx src/eqx/eqx.cpp $(COMMON_FILES) $(LOG_FILES)