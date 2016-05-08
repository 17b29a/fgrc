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

//#define BOUNDS_CHECK

template <typename T>
void peekat(T& buf, int pos)
{
#ifdef BOUNDS_CHECK
	if (pos + sizeof(buf) > InflatedFile.size())
		throw EOFException(pos);
#endif

	if (sizeof(T) == 4 && (g_iBytesRead & 3) != 0)
	{
		// Unaligned read
		memcpy(&buf, InflatedFile.data() + pos, sizeof(T));
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
	if (g_iBytesRead + sizeof(buf) > InflatedFile.size())
		throw EOFException(g_iBytesRead);
	
	buf = *(T *)(InflatedFile.data() + pos);
}

template<typename T, int _size>
void read(T(&buf)[_size])
{
	for (int read = 0; read < _size; read++)
		peekat(buf[read], g_iBytesRead + read * sizeof(T));
		//*(T *)((uint32_t)&buf + read * sizeof(T)) = *(T *)(InflatedFile.data() + g_iBytesRead + read * sizeof(T));

	g_iBytesRead += sizeof(T) * _size;
}

template<typename T, int _size>
void read(T(&buf)[_size], int pos)
{
	if (g_iBytesRead + sizeof(buf) > InflatedFile.size())
		throw EOFException(g_iBytesRead);
	
	for (int read = 0; read < _size; read++)
		peekat(buf[read], g_iBytesRead + read * sizeof(T));
		//*(T *)((uint32_t)&buf + read * sizeof(T)) = *(T *)(InflatedFile.data() + pos + read * sizeof(T));
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
	if (g_iBytesRead + sizeof(buf) > InflatedFile.size())
		throw EOFException(g_iBytesRead);
	
	*(T *)(InflatedFile.data() + pos) = buf;
}

template<typename T, int _size>
void write(const T(&buf)[_size], int pos)
{
	if (g_iBytesRead + sizeof(buf) > InflatedFile.size())
		throw EOFException(g_iBytesRead);
	
	for (int read = 0; read < _size; read++)
		*(T *)(InflatedFile.data() + pos + read * sizeof(T)) = *(T *)((uint32_t)&buf + read * sizeof(T));
}

template <typename T>
T& RefAt(int Pos)
{
	return *(T*)(InflatedFile.data() + Pos);
}

void nullexpand(int pos, int size)
{
	memmove((void *)((uint32_t)InflatedFile.data() + pos + size), InflatedFile.data() + pos, g_iInflatedSize - pos);
	memset(InflatedFile.data() + pos, 0, size);

	g_iInflatedSize += size;
	g_iBytesRead += size;
}