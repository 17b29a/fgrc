#include <cstdarg>
#include <vector>
#include <unordered_map>
#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#endif
#define ZLIB_CONST
#include "zlib.h"
#include "zfile.h"
#include "gunzdefs.h"
#include "read.h"
#include "util.h"

#pragma comment(lib, "zdll.lib")

//#define DEBUG

std::vector<unsigned char> InflatedFile;

int g_iBytesRead;
int g_iInflatedSize;
int add;

bool g_bRename;

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

	Out.resize(Out.size() - Stream.avail_out);

	inflateEnd(&Stream);
	
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
	
		err = deflate(&Stream, Z_NO_FLUSH);
	
		if (err != Z_OK)
		{
			Print("Error: %d: %s\n", err, Stream.msg);
			Print("%d, %d\n", Stream.avail_out, Stream.avail_in);
			break;
		}
		
		Pos += 1024;
	} while(Stream.avail_out == 0 && Stream.avail_in > 0);

	Out.resize(Out.size() - Stream.avail_out);

	while (true)
	{
		Out.resize(Out.size() + 1024);

		Stream.next_out = Out.data() + Pos;
		Stream.avail_out = 1024;

		err = deflate(&Stream, Z_FINISH);
	
		if (err != Z_OK)
		{
			if (err == Z_STREAM_END)
				break;

			Print("Error: %d: %s\n", err, Stream.msg);
			Print("%d, %d\n", Stream.avail_out, Stream.avail_in);
			break;
		}

		Pos += 1024 - Stream.avail_out;
	}

	deflateEnd(&Stream);

	Out.resize(Out.size() - Stream.avail_out);
}

template <typename T>
void PrintCharacterInfo(const T& rpi)
{
	auto& ci = rpi.Info;
	Print("UID: %08X%08X\n", rpi.State.UID.High, rpi.State.UID.Low);
	Print("Name: %s\n", ci.szName);
	if (ci.szClanName[0])
		Print("Clan name: %s\n", ci.szClanName);

#ifdef DEBUG
	Print("Sex: %d\n", ci.nSex);
	Print("Hair: %d\n", ci.nHair);
	Print("Face: %d\n", ci.nFace);
	Print("Level: %d\n", ci.nLevel);

	for (int j = 0; j < ArraySize(ci.nEquipedItemDesc); j++)
	{
		if (!ci.nEquipedItemDesc[j])
			continue;

		Print("Equipped item ID %d: %d\n", j, ci.nEquipedItemDesc[j]);
		uint64_t UID = (uint64_t(ci.uidEquipedItem[j].High) << 32) | ci.uidEquipedItem[j].Low;
		Print("Equipped item UID %d: %I64d\n", j, UID);
		Print("Equipped item count %d: %d\n", j, ci.nEquipedItemCount[j]);
		Print("Equipped item rarity %d: %d\n", j, ci.nEquipedItemRarity[j]);
		Print("Equipped item level %d: %d\n", j, ci.nEquipedItemLevel[j]);
	}

	Print("Read count: %d\n", g_iBytesRead);
#endif

	Print("\n");
}

std::vector<unsigned char> RepairFile(std::vector<unsigned char> File)
{
	InflatedFile.resize(0);
	
	Inflate(InflatedFile, File);
	
	Print("Inflated %d -> %d\n\n", File.size(), InflatedFile.size());
	
	g_iInflatedSize = InflatedFile.size();
	g_iBytesRead = 0;
	add = 0;

#if defined DEBUG && !defined EMSCRIPTEN
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

	if (version != 9 && version != 7)
	{
		Print("Unsupported version %d\n", version);
		return std::vector<unsigned char>();
	}

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

	struct PlayerInfo
	{
		std::string Name;
		uint32_t Kills;
		uint32_t Deaths;

		PlayerInfo(const std::string& a, uint32_t b, uint32_t c) : Name(a), Kills(b), Deaths(c)
		{
		}
	};
	std::unordered_map<MUID, PlayerInfo> PlayerMap;

	for (int i = 0; i < CharCount; i++)
	{
		if (version == 9)
		{
			ReplayPlayerInfo_FG_V9 rpi;
			peek(rpi);
			MTD_CharInfo_FG_V9& ci = rpi.Info;
			PrintCharacterInfo(rpi);

			PlayerMap.emplace(unaligned_read(rpi.State.UID), PlayerInfo(ci.szName, 0, 0));

			g_iBytesRead += 1053;
		}
		else if (version == 7 && !bInterimV7)
		{
			const int blocksize = 511 + 182;

			int mtdcharinfo = 376;

			dbgprint("Read count: %d\n", g_iBytesRead);
			ReplayPlayerInfo_FG_V7_0 rpi;
			peek(rpi);
			MTD_CharInfo_FG_V7_0& ci = rpi.Info;
			PrintCharacterInfo(rpi);

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
			dbgprint("Pos1: %d\n", pos);
			int size = 84;
			nullexpand(pos, size);
			add += size;

			pos = CharStart + i * blocksize + mtdcharinfo + add;
			dbgprint("Pos2: %d\n", pos);
			size = 220;
			nullexpand(pos, size);
			add += size;

			pos = CharStart + i * blocksize + mtdcharinfo + 279 + add;
			dbgprint("Pos3: %d\n", pos);
			size = 56;
			nullexpand(pos, size);
			add += size;

			PlayerMap.emplace(unaligned_read(rpi.State.UID), PlayerInfo(ci.szName, 0, 0));

			Print("\n");
		}
		else if (version == 7 && bInterimV7)
		{
			const int blocksize = 989;

			int mtdcharinfo = 631;

			dbgprint("Read count: %d\n", g_iBytesRead);
			ReplayPlayerInfo_FG_V7_1 rpi;
			peek(rpi);
			MTD_CharInfo_FG_V7_1& ci = rpi.Info;
			PrintCharacterInfo(rpi);

			g_iBytesRead += blocksize;

			ZCharacterProperty &zcp = *(ZCharacterProperty *)(InflatedFile.data() + CharStart + blocksize * i + mtdcharinfo + 8 + add);

			Print("ZCharacterProperty: %d\n", (uint32_t)&zcp - (uint32_t)InflatedFile.data());
			Print("UID: %08X%08X\n", rpi.State.UID.High, rpi.State.UID.Low);
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

			PlayerMap.emplace(unaligned_read(rpi.State.UID), PlayerInfo(ci.szName, 0, 0));

			Print("\n");
		}
	}

	dbgprint("Read count: %d\n", g_iBytesRead);

	int CommandStream = g_iBytesRead;

	auto FilePos = static_cast<size_t>(CommandStream);
	[&]()
	{
#define Read(type) [&]() -> type { type ret; read(ret, FilePos); FilePos += sizeof(type); return ret; }()
#define ReadAt(type, pos) [&]() -> type { type ret; read(ret, pos); return ret; }()

		auto StartTime = Read(float);

		int NumCommands = 0;

		int CommandsRemoved = 0;

		bool IsOldDojo = !strcmp(rssn.szMapName, "Dojo") && version == 9;
		size_t BasicInfoCount = 0;

		bool PrintNextCommand = false;

		while (FilePos < InflatedFile.size())
		{
			auto Time = Read(float);
			auto SenderUID = Read(MUID);
			auto Size = Read(uint32_t);
			auto CommandSize = ReadAt(uint16_t, FilePos);
			auto CommandID = ReadAt(MCommandID, FilePos + 2);
			if (Size == 0)
				Print("Zero size: %04X, %d\n", uint16_t(CommandID), CommandSize);

			//Print("i: %d, CommandID: %04X, size: %d\n", FilePos, uint16_t(CommandID), Size);

			bool DeletedCommand = false;

			auto DeleteCommand = [&]()
			{
				InflatedFile.erase(InflatedFile.begin() + FilePos - 0x10, InflatedFile.begin() + FilePos + Size);
				g_iInflatedSize = InflatedFile.size();

				DeletedCommand = true;
			};

			[&]
			{
				switch (CommandID)
				{
				case MCommandID::MC_PEER_BASICINFO:
				{
					if (!IsOldDojo)
						return;

					auto Transform = [](float pos[3])
					{
						pos[0] += 600;
						pos[1] -= 2800;
						pos[2] -= 400;
					};

					short shortpos[3];

					read(shortpos, FilePos + 2 + 1 + 4 + 4 + 2);

					float pos[3] = { static_cast<float>(shortpos[0]), static_cast<float>(shortpos[1]), static_cast<float>(shortpos[2]) };

					if (pos[2] < 0)
					{
						Print("Replay is on new dojo map; not translating positions\n");
						IsOldDojo = false;
						return;
					}

					Transform(pos);

					for (int j = 0; j < 3; j++)
						shortpos[j] = short(pos[j]);

					write(shortpos, FilePos + 2 + 1 + 4 + 4 + 2);

					BasicInfoCount++;
				}
				break;

				case MCommandID::MC_PEER_DIE:
				{
					auto VictimIt = PlayerMap.find(SenderUID);
					if (VictimIt == PlayerMap.end())
					{
						Print("Couldnt find victim %08X%08X\n", SenderUID.High, SenderUID.Low);
						return;
					}

					MUID AttackerUID = ReadAt(MUID, FilePos + 5);
					auto AttackerIt = PlayerMap.find(AttackerUID);
					if (AttackerIt == PlayerMap.end())
					{
						Print("Couldnt find attacker %08X%08X\n", AttackerUID.High, AttackerUID.Low);
						return;
					}

					AttackerIt->second.Kills++;
					VictimIt->second.Deaths++;
				}
				break;

				case MCommandID::Broken1:
				case MCommandID::Broken2:
				{
					DeleteCommand();
					CommandsRemoved++;
				}
				break;
				};
			}();

			if (!DeletedCommand)
				FilePos += Size;
			else
				FilePos -= 0x10;

			NumCommands++;
		}

		Print("Parsed %d commands\n", NumCommands);

		if (IsOldDojo)
			Print("Fixed %d BasicInfo commands\n", BasicInfoCount);

		Print("Removed %d commands\n", CommandsRemoved);
#undef Read
#undef ReadAt
	}();

	Print("\n");

	for (auto& Item : PlayerMap)
	{
		auto& Player = Item.second;

		Print("%s - %d/%d\n", Player.Name.c_str(), Player.Kills, Player.Deaths);
	}

#if defined DEBUG && !defined EMSCRIPTEN
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

	//EndPrint();
	
	return DeflatedFile;
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::register_vector<unsigned char>("FileVector");
    emscripten::function("RepairFile", &RepairFile);
}