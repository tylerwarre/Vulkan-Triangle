
CFLAGS_DEV = -std=c++17 -O2
CFLAGS_RELEASE = -std=c++17 -O2 -DNDEBUG
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
.PHONY: test clean release

VulkanTest: main.cpp
	g++ $(CFLAGS_DEV) -o VulkanTest main.cpp $(LDFLAGS)

release: main.cpp
	g++ $(CFLAGS_RELEASE) -o VulkanTest main.cpp $(LDFLAGS)

test: VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest
