#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

template <typename T, size_t size>
static size_t ArraySize(T (&arr)[size])
{
	return size;
}

static char* str_replace(char* str, const char* src, const char* dest)
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
			end += offset;
		}
		
		for (int i = 0; i < destlen; i++)
			p[i] = dest[i];
		
		p += destlen;
		
		if (offset < 0)
		{
			memmove(p, p - offset, end - (p - offset));
			len -= offset;
			end -= offset;
		}
	}

	return end;
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
	
	auto end = str_replace(buf, "\n", "<br>");
	
	strcat(end, "';");
	
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

static void Inflate(std::vector<unsigned char>& Out, const std::vector<unsigned char>& In)
{
	z_stream Stream;
	
	Stream.zalloc = (alloc_func)0;
	Stream.zfree = (free_func)0;
	Stream.opaque = (voidpf)0;

	Stream.next_out = Out.data();
	Stream.avail_out = 1024;

	Stream.next_in = In.data();
	Stream.avail_in = In.size();

	auto err = inflateInit(&Stream);
	
	if (err != Z_OK)
	{
		Print("Error: %d: %s\n", err, Stream.msg);
		return;
	}
	
	size_t Pos = 0;
	do
	{
		Out.resize(Out.size() + 1024);
		
		Stream.next_out = Out.data() + Pos;
		Stream.avail_out = 1024;
	
		err = inflate(&Stream, Z_NO_FLUSH);
	
		if (err != Z_OK)
		{
			if (err != Z_STREAM_END)
				Print("Error: %d: %s\n", err, Stream.msg);

			break;
		}
		
		Pos += 1024;
	} while(Stream.avail_out == 0 && Stream.avail_in > 0);

	Out.resize(Out.size() - Stream.avail_out);

	inflateEnd(&Stream);
	
	// uLong crc = crc32(0L, Z_NULL, 0);
	
	// crc = crc32(crc, Out.data(), Out.size());
	
	// Print("CRC32: %08X\n", crc);
}

static void Deflate(std::vector<unsigned char>& Out, const std::vector<unsigned char>& In)
{
	z_stream Stream;
	
	Stream.zalloc = (alloc_func)0;
	Stream.zfree = (free_func)0;
	Stream.opaque = (voidpf)0;

	auto err = deflateInit(&Stream, Z_DEFAULT_COMPRESSION);
	
	if (err != Z_OK)
	{
		Print("Error: %d\n", err);
		return;
	}
	
	size_t Pos = 0;

	Out.resize(1024);
	Stream.next_out = Out.data();
	Stream.avail_out = 1024;

	Stream.next_in = In.data();
	Stream.avail_in = In.size();

	while (true)
	{
		if (Stream.avail_out == 0)
		{
			Out.resize(Out.size() + 1024);
			Stream.avail_out = 1024;
		}
		
		Stream.next_out = Out.data() + Pos;

		int AvailOut = Stream.avail_out;

		err = deflate(&Stream, Stream.avail_in ? Z_NO_FLUSH : Z_FINISH);
	
		if (err != Z_OK)
		{
			if (err != Z_STREAM_END)
				Print("Error: %d: %s. avail_in: %d, avail_out: %d\n", err, Stream.msg, Stream.avail_in, Stream.avail_out);

			break;
		}
		
		Pos += AvailOut - Stream.avail_out;
	}

	Out.resize(Out.size() - Stream.avail_out);

	deflateEnd(&Stream);
}