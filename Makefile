CXX = g++

EXEC = visualizador

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

CXXFLAGS = -std=c++23 -Wall -Wextra -g $(shell pkg-config --cflags sfml-all)

LDLIBS = $(shell pkg-config --libs sfml-all)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC) $(LDLIBS)
	@echo "Executavel '$(EXEC)' criado com sucesso!"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(EXEC)

clean:
	@echo "Limpando arquivos gerados..."
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean run
