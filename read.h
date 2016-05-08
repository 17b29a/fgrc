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

template<typename T>
void read(T &buf)
{
	if (g_iBytesRead + sizeof(buf) > InflatedFile.size())
		throw EOFException(g_iBytesRead);
	
	buf = *(T *)(InflatedFile.data() + g_iBytesRead);

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
	if (g_iBytesRead + sizeof(buf) > InflatedFile.size())
		throw EOFException(g_iBytesRead);
	
	for (int read = 0; read < _size; read++)
		*(T *)((uint32_t)&buf + read * sizeof(T)) = *(T *)(InflatedFile.data() + g_iBytesRead + read * sizeof(T));

	g_iBytesRead += sizeof(T)* _size;
}

template<typename T, int _size>
void read(T(&buf)[_size], int pos)
{
	if (g_iBytesRead + sizeof(buf) > InflatedFile.size())
		throw EOFException(g_iBytesRead);
	
	for (int read = 0; read < _size; read++)
		*(T *)((uint32_t)&buf + read * sizeof(T)) = *(T *)(InflatedFile.data() + pos + read * sizeof(T));
}

template<typename T>
T &ref()
{
	if (g_iBytesRead + sizeof(T) > InflatedFile.size())
		throw EOFException(g_iBytesRead);
	
	return *(T *)(InflatedFile.data() + g_iBytesRead);
}

template<typename T>
void peek(T &buf)
{
	if (g_iBytesRead + sizeof(buf) > InflatedFile.size())
		throw EOFException(g_iBytesRead);
	
	buf = *(T *)(InflatedFile.data() + g_iBytesRead);
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