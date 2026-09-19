# macOS build. Requires GLFW from Homebrew: brew install glfw
# The Windows build command lives in .vscode/tasks.json.

GLFW_PREFIX ?= $(shell brew --prefix glfw 2>/dev/null)

ifneq ($(MAKECMDGOALS),clean)
ifeq ($(GLFW_PREFIX),)
$(error GLFW not found. Install it with: brew install glfw)
endif
endif

TARGET := build/almondbread
OBJDIR := build/obj
SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:src/%.cpp=$(OBJDIR)/%.o) $(OBJDIR)/glad.o
DEPS := $(OBJS:.o=.d)

CPPFLAGS := -I$(GLFW_PREFIX)/include -Ilib/glad/include -Ilib/stb -DGL_SILENCE_DEPRECATION

# Shader precision override for comparisons, e.g. `make clean && make PRECISION=FLOAT`
# (FLOAT, DOUBLE_FLOAT or DOUBLE). Objects are not rebuilt automatically when this changes.
ifdef PRECISION
CPPFLAGS += -DALMONDBREAD_PRECISION=PRECISION_$(PRECISION)
endif
CXXFLAGS := -std=c++11 -Wall -O3 -ffast-math -g -MMD -MP
CFLAGS := -O3 -g -MMD -MP
LDLIBS := $(GLFW_PREFIX)/lib/libglfw3.a \
	-framework Cocoa -framework IOKit -framework QuartzCore -framework CoreFoundation -framework OpenGL

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDLIBS) -o $@

$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/glad.o: lib/glad/src/glad.c | $(OBJDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $@

# Shaders and the palette are loaded relative to the working directory, so run from the repo root.
run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(TARGET)

-include $(DEPS)

.PHONY: all run clean
