CXX = g++
CXXFLAGS = -Wall -std=c++17

TARGET = budget_tax_system

SRCS = main.cpp AuthManager.cpp User.cpp TaxPayer.cpp MoneyFormat.cpp Budget.cpp BarGraph.cpp History.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
