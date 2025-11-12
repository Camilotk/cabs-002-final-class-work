CXX ?= g++

EXEC = visualizador

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

PKGCONFIG_BIN := $(shell command -v pkg-config 2>/dev/null)
ifeq ($(PKGCONFIG_BIN),)
$(info [INFO] pkg-config nao encontrado; usando caminhos padrao para SFML.)
SFML_CFLAGS :=
SFML_LIBS := -lsfml-graphics -lsfml-window -lsfml-system
else
	SFML_CFLAGS := $(shell pkg-config --cflags sfml-all 2>/dev/null)
	SFML_LIBS := $(shell pkg-config --libs sfml-all 2>/dev/null)
endif

NIX_SFML_INCLUDE := $(shell find /nix/store -maxdepth 3 -type f -name Graphics.hpp 2>/dev/null | head -1 | xargs dirname | xargs dirname)
ifneq ($(NIX_SFML_INCLUDE),)
SFML_CFLAGS += -I$(NIX_SFML_INCLUDE)
endif

NIX_SFML_LIBDIR := $(shell find /nix/store -maxdepth 3 -type f -name libsfml-graphics.* 2>/dev/null | head -1 | xargs dirname)
ifneq ($(NIX_SFML_LIBDIR),)
SFML_LIBS += -L$(NIX_SFML_LIBDIR)
endif

CXXFLAGS = $(STD_FLAG) -Wall -Wextra -g $(SFML_CFLAGS)

LDLIBS = $(SFML_LIBS)

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
