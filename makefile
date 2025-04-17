# Compiler and flags
CXX := g++
TARGET := ida-rpc64.so
CXXFLAGS := -fPIC -shared -w \
            -I./idasdk90/include \
            -I./discord-rpc/linux-dynamic/include \
            -D__LINUX__ \
            -D__EA64__ \
            -DNDEBUG \
            -D_Release64 \
            -DUSE_DANGEROUS_FUNCTIONS \
            -D__IDP__ \
            -D__PLUGIN__ \
            -std=c++17 \

# Linker configuration
LDFLAGS := -shared \
           -L./idasdk90/lib/x64_linux_gcc_64 \
           -L./discord-rpc/linux-dynamic/lib \
           -Wl,--version-script=./idasdk90/plugins/exports.def \
           -l:libida64.so \
           -l:libidalib64.so \
           -l:libdiscord-rpc.so \
           -lstdc++ \
           -lpthread \
           -ldl \
           -Wl,-rpath,'$$ORIGIN/..:$$ORIGIN'

# Source files
SRC := main.cpp options.cpp
OBJ := $(SRC:.cpp=.o)

# Targets
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

install: $(TARGET)
	cp $(TARGET) ../bin/

.PHONY: all clean install