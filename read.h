#include <vector>
#include <cstdint>

extern std::vector<unsigned char> InflatedFile;

extern int g_iBytesRead;
extern int g_iInflatedSize;

class EOFException : public std::runtime_error
{
public:
	EOFException(int a) : std::runtime_error("End of file was reached"), Position(a) { }
	int GetPosition() const {
		return Position;
	}
private:
	int Position;
};

template <typename T>
void unaligned_copy(T& dest, const T& src)
{
#ifdef EMSCRIPTEN
	for (int i = 0; i < sizeof(T); i++)
		*(uint8_t*)(((uint8_t*)&dest) + i) = *(uint8_t*)(((uint8_t*)&src) + i);
#else
	dest = src;
#endif
}

template <typename T>
T unaligned_read(T& arg)
{
#ifdef EMSCRIPTEN
	T temp;
	unaligned_copy(temp, arg);
	return temp;
#else
	return arg;
#endif
}

//#define BOUNDS_CHECK

template <typename T>
void peekat(T& buf, int pos)
{
#ifdef BOUNDS_CHECK
	if (pos + sizeof(buf) > InflatedFile.size())
		throw EOFException(pos);
#endif

	void Print(const char*, ...);
	if (pos + sizeof(buf) > InflatedFile.size())
		Print("Exceeded bounds %d, %d\n", pos, sizeof(buf));

	if (true)//(g_iBytesRead & 3) != 0))
	{
		unaligned_copy(buf, *(T*)&InflatedFile[pos]);
	}
	else
	{
		buf = *(T *)(InflatedFile.data() + pos);
	}
}

template<typename T>
void peek(T &buf)
{
	peekat(buf, g_iBytesRead);
}

template<typename T>
void read(T &buf)
{
	peek(buf);

	g_iBytesRead += sizeof(T);
}

template<typename T>
void read(T &buf, int pos)
{
	peekat(buf, pos);
}

template<typename T, int _size>
void read(T(&buf)[_size])
{
	for (int read = 0; read < _size; read++)
		peekat(buf[read], g_iBytesRead + read * sizeof(T));

	g_iBytesRead += sizeof(T) * _size;
}

template<typename T, int _size>
void read(T(&buf)[_size], int pos)
{
	for (int read = 0; read < _size; read++)
		peekat(buf[read], pos + read * sizeof(T));
}

template<typename T>
T &ref()
{
	if (g_iBytesRead + sizeof(T) > InflatedFile.size())
		throw EOFException(g_iBytesRead);
	
	return *(T *)(InflatedFile.data() + g_iBytesRead);
}

template<typename T>
void write(const T &buf, int pos)
{
#ifdef BOUNDS_CHECK
	if (g_iBytesRead + sizeof(buf) > InflatedFile.size())
		throw EOFException(g_iBytesRead);
#endif
	
	unaligned_copy(*(T*)&InflatedFile[pos], buf);
}

template<typename T, int _size>
void write(const T(&buf)[_size], int pos)
{
	for (int read = 0; read < _size; read++)
		write(buf[read], pos + read * sizeof(T));
}

template <typename T>
T& RefAt(int Pos)
{
	return *(T*)(InflatedFile.data() + Pos);
}

void nullexpand(int pos, int size)
{
	InflatedFile.resize(InflatedFile.size() + size);
	memmove((void *)((uint32_t)InflatedFile.data() + pos + size), InflatedFile.data() + pos, g_iInflatedSize - pos);
	memset(InflatedFile.data() + pos, 0, size);

	g_iInflatedSize += size;
	g_iBytesRead += size;
}