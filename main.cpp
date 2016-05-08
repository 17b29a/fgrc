#include <cstdarg>
#include <vector>
#define ZLIB_CONST
#include "zlib.h"
#include "zfile.h"
#include "gunzdefs.h"
#include "read.h"
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>

#pragma comment(lib, "zdll.lib")

//#define DEBUG

std::vector<unsigned char> InflatedFile;

int g_iBytesRead;
int g_iInflatedSize;
int add;

bool g_bRename;

void str_replace(char* str, const char* src, const char* dest)
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

void Print(const char* Format, ...)
{
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
}

void Inflate(std::vector<unsigned char>& Out, const std::vector<unsigned char>& In)
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
	
		err = inflate(&Stream, Z_SYNC_FLUSH);
	
		if (err != Z_OK && err != Z_STREAM_END)
		{
			Print("Error: %d: %s\n", err, Stream.msg);
			break;
		}
		
		Pos += 1024;
	} while(Stream.avail_out == 0 && Stream.avail_in > 0);

	// Out.resize(Out.size() - Stream.avail_out);
	
	// uLong crc = crc32(0L, Z_NULL, 0);
	
	// crc = crc32(crc, Out.data(), Out.size());
	
	// Print("CRC32: %08X\n", crc);
}

void Deflate(std::vector<unsigned char>& Out, const std::vector<unsigned char>& In)
{
	z_stream Stream;
	
	Stream.zalloc = (alloc_func)0;
	Stream.zfree = (free_func)0;
	Stream.opaque = (voidpf)0;

	Stream.next_out = Out.data();
	Stream.avail_out = 1024;

	Stream.next_in = In.data();
	Stream.avail_in = In.size();

	auto err = deflateInit(&Stream, Z_DEFAULT_COMPRESSION);
	
	if (err != Z_OK)
	{
		Print("Error: %d\n", err);
		return;
	}
	
	size_t Pos = 0;
	do
	{
		Out.resize(Out.size() + 1024);
		
		Stream.next_out = Out.data() + Pos;
		Stream.avail_out = 1024;
	
		err = deflate(&Stream, Z_SYNC_FLUSH);
	
		if (err != Z_OK)
		{
			Print("Error: %d: %s\n", err, Stream.msg);
			Print("%d, %d\n", Stream.avail_out, Stream.avail_in);
			break;
		}
		
		Pos += 1024;
	} while(Stream.avail_out == 0 && Stream.avail_in > 0);

	Out.resize(Out.size() - Stream.avail_out);
}

std::vector<unsigned char> RepairFile(std::vector<unsigned char> File)
{
	InflatedFile.resize(0);
	
	Inflate(InflatedFile, File);
	
	Print("Inflated %d -> %d\n", File.size(), InflatedFile.size());
	
	g_iInflatedSize = InflatedFile.size();
	g_iBytesRead = 0;
	add = 0;

#ifdef DEBUG
	if (!bDirectory)
	{
		FILE *file;
		fopen_s(&file, szInflatedOrigFilename, "wb");
		fwrite(InflatedFile.data(), g_iInflatedSize, 1, file);
		fclose(file);
	}
#endif

	uint32_t header, version;

	read(header);
	read(version);

	write<uint32_t>(9, 4); // Write version 9

	REPLAY_STAGE_SETTING_NODE rssn;

	read(rssn);

	if (rssn.nGameType == MMATCH_GAMETYPE_DUEL)
	{
		MTD_DuelQueueInfo dqi;
		read(dqi);
	}

	// Position 527 holds the first char of the first ZCharacter's name in the proper V7 format, but holds an item count value in the interim V7 format.
	int offset = 527;
	if (rssn.nGameType == MMATCH_GAMETYPE_DUEL)
		offset += sizeof(MTD_DuelQueueInfo);
	bool bInterimV7 = version == 7 && (InflatedFile[offset] == 0x00 || InflatedFile[offset] == 0x01);

	Print("Header: %08X\nVersion: V%d.0%s\n", header, version, bInterimV7 ? " (interim)" : "");

	Print("Map name: %s\nStage name: %s\n", rssn.szMapName, rssn.szStageName);

	// Print("Read count: %d\n", g_iBytesRead);
	// for (int i = 0; i < 1000; i++)
	// {
		// Print("%02X ", InflatedFile[i]);
		// if (i % 64 == 0)
			// Print("\n");
	// }
	
	// for (int i = 0; i < 10; i++)
	// {
		// Print("%08X\n", *(uint32_t *)(InflatedFile.data() + g_iBytesRead + i));
		// Print("%d\n", g_iBytesRead + i);
	// }
	
	int32_t CharCount = 0;
	read(CharCount);

	Print("Character count: %d\n\n", CharCount);

	int CharStart = g_iBytesRead;

	if (version == 9)
	{
		for (int i = 0; i < CharCount; i++)
		{
			Print("Read count: %d\n", g_iBytesRead);
			MTD_CharInfo ci;
			peek(ci);
			Print("Character %d\n", i + 1);
			Print("Name: %s\n", ci.szName);
			Print("Clan name: %s\n", ci.szClanName);
			Print("Sex: %d\n", ci.nSex);
			Print("Hair: %d\n", ci.nHair);
			Print("Face: %d\n", ci.nFace);
			Print("Level: %d\n", ci.nLevel);

			for (int j = 0; j < MMCIP_END; j++)
			{
				if (!ci.nEquipedItemDesc[j])
					continue;

				Print("Equipped item ID %d: %d\n", j, ci.nEquipedItemDesc[j]);
#ifdef DEBUG
				uint64_t UID = (uint64_t(ci.uidEquipedItem[j].High) << 32) | ci.uidEquipedItem[j].Low;
				Print("Equipped item UID %d: %I64d\n", j, UID);
				Print("Equipped item count %d: %d\n", j, ci.nEquipedItemCount[j]);
				Print("Equipped item rarity %d: %d\n", j, ci.nEquipedItemRarity[j]);
				Print("Equipped item level %d: %d\n", j, ci.nEquipedItemLevel[j]);
#endif
			}

			Print("Read count: %d\n", g_iBytesRead);
			Print("\n");

			g_iBytesRead += 1053;
		}

		Print("Already V9!\n");

		if (!g_bRename)
		{
			//return true;
		}
	}
	else if (version == 7 && !bInterimV7)
	{
		for (int i = 0; i < CharCount; i++)
		{
			const int blocksize = 511 + 182;

			int mtdcharinfo = 376;

			Print("Read count: %d\n", g_iBytesRead);
			MTD_CharInfo ci;
			peek(ci);
			Print("Character %d\n", i + 1);
			Print("Name: %s\n", ci.szName);
			Print("Clan name: %s\n", ci.szClanName);
			Print("Sex: %d\n", ci.nSex);
			Print("Hair: %d\n", ci.nHair);
			Print("Face: %d\n", ci.nFace);
			Print("Level: %d\n", ci.nLevel);

			for (int j = 0; j < 17; j++)
			{
				Print("Equipped item ID %d: %d\n", j, ci.nEquipedItemDesc[j]);
			}

			Print("Read count: %d\n", g_iBytesRead);
			Print("\n");

			g_iBytesRead += blocksize;

			ZCharacterProperty &zcp = *(ZCharacterProperty *)(InflatedFile.data() + CharStart + blocksize * i + mtdcharinfo + 8 + add - 1);

			Print("ZCharacterProperty: %d\n", (uint32_t)&zcp - (uint32_t)InflatedFile.data());
			Print("Name: %s\n", zcp.szName);
			Print("Clan name: %s\n", zcp.szClanName);
			Print("Sex: %d\n", zcp.nSex);
			Print("Hair: %d\n", zcp.nHair);
			Print("Face: %d\n", zcp.nFace);
			Print("Level: %d\n", zcp.nLevel);
			Print("\n");

			int pos = CharStart + i * blocksize + 306 + add;
			Print("Pos1: %d\n", pos);
			int size = 84;
			nullexpand(pos, size);
			add += size;

			pos = CharStart + i * blocksize + mtdcharinfo + add;
			Print("Pos2: %d\n", pos);
			size = 220;
			nullexpand(pos, size);
			add += size;

			pos = CharStart + i * blocksize + mtdcharinfo + 279 + add;
			Print("Pos3: %d\n", pos);
			size = 56;
			nullexpand(pos, size);
			add += size;

			Print("\n");
		}
	}
	else if (version == 7 && bInterimV7)
	{
		for (int i = 0; i < CharCount; i++)
		{
			const int blocksize = 989;

			int mtdcharinfo = 631;

			Print("Read count: %d\n", g_iBytesRead);
			MTD_CharInfo ci;
			peek(ci);
			Print("Character %d\n", i + 1);
			Print("Name: %s\n", ci.szName);
			Print("Clan name: %s\n", ci.szClanName);
			Print("Sex: %d\n", ci.nSex);
			Print("Hair: %d\n", ci.nHair);
			Print("Face: %d\n", ci.nFace);
			Print("Level: %d\n", ci.nLevel);

			for (int j = 0; j < 17; j++)
			{
				Print("Equipped item ID %d: %d\n", j, ci.nEquipedItemDesc[j]);
			}

			Print("Read count: %d\n", g_iBytesRead);
			Print("\n");

			g_iBytesRead += blocksize;

			ZCharacterProperty &zcp = *(ZCharacterProperty *)(InflatedFile.data() + CharStart + blocksize * i + mtdcharinfo + 8 + add);

			Print("ZCharacterProperty: %d\n", (uint32_t)&zcp - (uint32_t)InflatedFile.data());
			Print("Name: %s\n", zcp.szName);
			Print("Clan name: %s\n", zcp.szClanName);
			Print("Sex: %d\n", zcp.nSex);
			Print("Hair: %d\n", zcp.nHair);
			Print("Face: %d\n", zcp.nFace);
			Print("Level: %d\n", zcp.nLevel);
			Print("HP/AP: %f/%f\n", zcp.fMaxHP, zcp.fMaxAP);
			Print("\n");

			int pos = CharStart + i * blocksize + 186 + add;
			Print("Pos1: %d\n", pos);
			int size = 8;
			nullexpand(pos, size);
			add += size;

			pos = CharStart + i * blocksize + 366 + add;
			Print("Pos1: %d\n", pos);
			size = 16;
			nullexpand(pos, size);
			add += size;

			pos = CharStart + i * blocksize + 630 + add;
			Print("Pos1: %d\n", pos);
			size = 24;
			nullexpand(pos, size);
			add += size;

			pos = CharStart + i * blocksize + mtdcharinfo + 327 + add;
			Print("Pos2: %d\n", pos);
			size = 16;
			nullexpand(pos, size);
			add += size;

			Print("\n");
		}
	}
	else
	{
		Print("Unsupported version %d\n", version);
		return InflatedFile;
	}

	Print("Read count: %d\n", g_iBytesRead);

	int CommandStream = g_iBytesRead;

	if (!strcmp(rssn.szMapName, "Dojo") && version != 9)
	{
		auto Transform = [](float pos[3])
		{
			pos[0] += 600;
			pos[1] -= 2800;
			pos[2] -= 400;
		};

		auto block = [&]()
		{
			int Count = 0;

			for (int i = CommandStream; i < g_iInflatedSize; i++)
			{
				if (InflatedFile[i] != 0x1C)
					continue;

				if (InflatedFile[i + 1] != 0x27)
					continue;

				if (InflatedFile[i + 2] != 0)
					continue;

				if (*(uint32_t *)(InflatedFile.data() + i + 3) != 0x19)
					continue;

				short shortpos[3];

				read(shortpos, i + 2 + 1 + 4 + 4);

				float pos[3] = { static_cast<float>(shortpos[0]), static_cast<float>(shortpos[1]), static_cast<float>(shortpos[2]) };

				if (pos[2] < 0)
				{
					Print("Replay is on new dojo map; not translating positions\n");
					return;
				}

				//Print("Original position: %f, %f, %f\n", pos[0], pos[1], pos[2]);

				Transform(pos);

				for (int j = 0; j < 3; j++)
					shortpos[j] = short(pos[j]);

				write(shortpos, i + 2 + 1 + 4 + 4);

				Count++;

				i += 25;
			}

			Print("Converted %d basicinfo commands\n", Count);

			Count = 0;

			for (int i = CommandStream; i < g_iInflatedSize; i++)
			{
				if (InflatedFile[i] != 0x35)
					continue;

				if (InflatedFile[i + 1] != 0x27)
					continue;

				if (InflatedFile[i + 2] != 0)
					continue;

				float pos[3];

				read(pos, i + 7);

				Transform(pos);

				write(pos, i + 7);

				Count++;

				i += 23;
			}

			Print("Converted %d slash commands\n", Count);
		};

		block();
	}

	int kills[2] = { 0, 0 };
	if (CharCount == 2)
	{
		MUID uid[2];
		uid[0] = *(MUID *)(InflatedFile.data() + CharStart + 679);
		uid[1] = *(MUID *)(InflatedFile.data() + CharStart + 1053 + 679);
		for (int i = CommandStream; i < g_iInflatedSize; i++)
		{
			if (*(uint32_t *)(InflatedFile.data() + i) != 0x0000000D)
				continue;

			if (*(uint32_t *)(InflatedFile.data() + i + 4) != 0x2739000D)
				continue;

			MUID sender = *(MUID *)(InflatedFile.data() + i - 8);

			for (int j = 0; j < 2; j++)
			{
				if (sender.High == uid[j].High && sender.Low == uid[j].Low)
				{
					kills[!j]++;
				}
			}
		}

		Print("Score: %d-%d\n", kills[0], kills[1]);
	}

	int nCommandsRemoved = 0;

	for (int i = CommandStream; i < g_iInflatedSize; i++)
	{
		if (*(uint32_t *)(InflatedFile.data() + i) != 0x00000011)
			continue;

		if (*(uint32_t *)(InflatedFile.data() + i + 4) != 0x275F0011)
			continue;

		int offset = i - 12;

		int size = g_iInflatedSize - offset;

		memcpy(InflatedFile.data() + offset, InflatedFile.data() + offset + 0x21, size);

		g_iInflatedSize -= 0x21;

		nCommandsRemoved++;

		i -= 13;
	}

	for (int i = CommandStream; i < g_iInflatedSize; i++)
	{
		if (*(uint32_t *)(InflatedFile.data() + i) != 0x00000021)
			continue;

		if (*(uint32_t *)(InflatedFile.data() + i + 4) != 0x276F0021)
			continue;

		int offset = i - 12;

		int size = g_iInflatedSize - offset;

		memcpy(InflatedFile.data() + offset, InflatedFile.data() + offset + 0x31, size);

		g_iInflatedSize -= 0x31;

		nCommandsRemoved++;

		i -= 13;
	}

	Print("Removed %d commands\n", nCommandsRemoved);

#ifdef DEBUG
	if (!bDirectory)
	{
		FILE *file;
		fopen_s(&file, szInflatedFilename, "wb");
		fwrite(InflatedFile.data(), g_iInflatedSize, 1, file);
		fclose(file);
	}
#endif

	// if (g_bRename && CharCount == 2)
	// {
		// char *name[2];
		// name[0] = (char*)InflatedFile.data() + CharStart + uint32_t(((MTD_CharInfo *)0)->szName);
		// name[1] = (char*)InflatedFile.data() + CharStart + 1053 + uint32_t(((MTD_CharInfo *)0)->szName);

		// char *path = strrchr(szOutputFilename, '\\');
		// if (!path) path = strrchr(szOutputFilename, '/');

		// if (path)
		// {
			// *(path + 1) = 0;
		// }

		// char buf[128];
		// if(kills[0] > kills[1])
			// sPrint(buf, "%s_vs_%s_(%d-%d).gzr", name[0], name[1], kills[0], kills[1]);
		// else
			// sPrint(buf, "%s_vs_%s_(%d-%d).gzr", name[1], name[0], kills[1], kills[0]);
		// strcat_s(szOutputFilename, buf);
	// }

	// {
		// ZFile File;
		// File.Open(szOutputFilename, true);
		// File.Write(InflatedFile.data(), g_iInflatedSize);
		// File.Close();
	// }

	//if (version == 9)
		//return true;

	g_iBytesRead = CharStart;

	Print("\n");
	
#ifdef DEBUG
	Print("V9 read\n");

	Print("Character count: %d\n\n", CharCount);

	for (int i = 0; i < CharCount; i++)
	{
		Print("Read count: %d\n", g_iBytesRead);
		MTD_CharInfo ci;
		peek(ci);
		Print("Character %d\n", i + 1);
		Print("Name: %s\n", ci.szName);
		Print("Clan name: %s\n", ci.szClanName);
		Print("Sex: %d\n", ci.nSex);
		Print("Hair: %d\n", ci.nHair);
		Print("Face: %d\n", ci.nFace);
		Print("Level: %d\n", ci.nLevel);

		for (int j = 0; j < MMCIP_END; j++)
		{
			if (!ci.nEquipedItemDesc[j])
				continue;

			Print("Equipped item ID %d: %ul\n", j, ci.nEquipedItemDesc[j]);
			uint64_t UID = (uint64_t(ci.uidEquipedItem[j].High) << 32) | ci.uidEquipedItem[j].Low;
			Print("Equipped item UID %d: %I64d\n", j, UID);
			Print("Equipped item count %d: %d\n", j, ci.nEquipedItemCount[j]);
			Print("Equipped item rarity %d: %d\n", j, ci.nEquipedItemRarity[j]);
			Print("Equipped item level %d: %d\n", j, ci.nEquipedItemLevel[j]);
		}

		Print("Read count: %d\n", g_iBytesRead);
		Print("\n");

		g_iBytesRead += 1053;
	}
#endif
	
	std::vector<unsigned char> DeflatedFile;
	
	Deflate(DeflatedFile, InflatedFile);
	
	Print("Deflated %d -> %d\n", InflatedFile.size(), DeflatedFile.size());
	
	return DeflatedFile;
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::register_vector<unsigned char>("FileVector");
    emscripten::function("RepairFile", &RepairFile);
}