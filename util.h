#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

template <typename T, size_t size>
static size_t ArraySize(T (&arr)[size])
{
	return size;
}

static void str_replace(char* str, const char* src, const char* dest)
{
	size_t srclen = strlen(src);
	size_t destlen = strlen(dest);
	size_t len = strlen(str);
	char* end = str + len;
	
	char* p = str;
	while ((p = strstr(p, src)))
	{
		int offset = destlen - srclen;
		
		if (offset > 0)
		{
			memmove(p + offset, p, end - p);
			len += offset;
		}
		
		for (int i = 0; i < destlen; i++)
			p[i] = dest[i];
		
		p += destlen;
		
		if (offset < 0)
		{
			memmove(p, p - offset, end - (p - offset));
			len -= offset;
		}
	}
}

std::string PrintBuffer;

static void Print(const char* Format, ...)
{
#ifdef EMSCRIPTEN
	char buf[512] = "var element = document.getElementById('output'); \
	element.innerHTML += '";
	
	static const auto len = strlen("var element = document.getElementById('output'); \
	element.innerHTML += '");
	
	char* p = buf + len;
	
	va_list args;
	va_start(args, Format);
	vsprintf(p, Format, args);
	va_end(args);
	
	str_replace(buf, "\n", "<br>");
	
	strcat(buf, "';");
	
	printf("%s\n", buf);
	
	emscripten_run_script(buf);
#else
	va_list args;
	va_start(args, Format);
	vprintf(p, Format, args);
	va_end(args);
#endif
}

#ifdef DEBUG
static auto dbgprint = Print;
#else
static void dbgprint(...)
{
}
#endif


static void EndPrint()
{
	//emscripten_run_script(PrintBuffer.c_str());
}