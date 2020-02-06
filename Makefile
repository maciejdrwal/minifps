SHELL = sh
TARGET = minifps
DEBUG = 0

INCLUDES =
LIBS =

CXX = g++
CXXFLAGS = -std=c++17
LDFLAGS = -v -MD -lncurses
RM = rm -f

ifeq ($(DEBUG), 1)
CXXFLAGS += -g -DDEBUG
else
CXXFLAGS += -O3
endif

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

-include $(DEPS)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -MM -MP -MT $(@:.d=.o) >$@

clean:
	$(RM) $(SRCS:.cpp=.o) $(SRCS:.cpp=.d)
	$(RM) $(TARGET)
