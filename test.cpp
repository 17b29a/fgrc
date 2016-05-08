#include <emscripten/bind.h>

int square(int x)
{
	return x * x;
}

int main()
{
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("square", &square);
}