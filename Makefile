CXX = g++
CXXFLAGS = -Wall -std=c++17

MYSQL_CFLAGS := $(shell mysql_config --cflags)
MYSQL_LIBS := $(shell mysql_config --libs)

TARGET = budget_tax_system

SRCS = main.cpp AuthManager.cpp User.cpp TaxPayer.cpp MoneyFormat.cpp Budget.cpp BarGraph.cpp History.cpp Database.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(MYSQL_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(MYSQL_CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
