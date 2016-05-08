#include <cstdint>

struct MUID{
	uint32_t	High;	///< High 4 Byte
	uint32_t	Low;	///< High 4 Byte

	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator > (const MUID& a, const MUID& b){
		if(a.High>b.High) return true;
		if(a.High==b.High){
			if(a.Low>b.Low) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator >= (const MUID& a, const MUID& b){
		if(a.High>b.High) return true;
		if(a.High==b.High){
			if(a.Low>=b.Low) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator < (const MUID& a, const MUID& b){
		if(a.High<b.High) return true;
		if(a.High==b.High){
			if(a.Low<b.Low) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator <= (const MUID& a, const MUID& b){
		if(a.High<b.High) return true;
		if(a.High==b.High){
			if(a.Low<=b.Low) return true;
		}
		return false;
	}

	/// ���� ���۷����� �����ε�
	inline MUID& operator=(int v){
		High = 0;
		Low = v;
		return *this;
	}
	/// ���� ���۷����� �����ε�
	inline MUID& operator=(const MUID& a){
		High = a.High;
		Low = a.Low;
		return *this;
	}
	/// �� ���۷����� �����ε�
	inline friend bool operator==(const MUID& a, const MUID& b){
		if(a.High==b.High){
			if(a.Low==b.Low) return true;
		}
		return false;
	}
	/// �� ���۷����� �����ε�
	inline friend bool operator!=(const MUID& a, const MUID& b){
		if(a.Low!=b.Low) return true;
		if(a.High!=b.High) return true;
		return false;
	}
};

enum MMATCH_GAMETYPE {
	MMATCH_GAMETYPE_DEATHMATCH_SOLO = 0,			///< ���� ������ġ
	MMATCH_GAMETYPE_DEATHMATCH_TEAM = 1,			///< �� ������ġ
	MMATCH_GAMETYPE_GLADIATOR_SOLO = 2,			///< ���� �۷�������
	MMATCH_GAMETYPE_GLADIATOR_TEAM = 3,			///< �� �۷�������
	MMATCH_GAMETYPE_ASSASSINATE = 4,			///< ������(�ϻ���)
	MMATCH_GAMETYPE_TRAINING = 5,			///< ����

	MMATCH_GAMETYPE_SURVIVAL = 6,			///< �����̹�
	MMATCH_GAMETYPE_QUEST = 7,			///< ����Ʈ

	MMATCH_GAMETYPE_BERSERKER = 8,			///< ������ġ ����Ŀ
	MMATCH_GAMETYPE_DEATHMATCH_TEAM2 = 9,			///< ��������ġ �ͽ�Ʈ��
	MMATCH_GAMETYPE_DUEL = 10,		///< ��� ��ġ
	MMATCH_GAMETYPE_DUELTOURNAMENT = 11,		///< ��� ��ʸ�Ʈ
	MMATCH_GAMETYPE_QUEST_CHALLENGE = 12,		///< ç��������Ʈ

	// Custom: Game modes
	MMATCH_GAMETYPE_TEAM_TRAINING = 13,
	MMATCH_GAMETYPE_CTF = 14,
	MMATCH_GAMETYPE_INFECTED = 15,
	MMATCH_GAMETYPE_GUNGAME = 16,
	/*
	#ifndef _CLASSIC
	MMATCH_GAMETYPE_CLASSIC_SOLO,
	MMATCH_GAMETYPE_CLASSIC_TEAM,
	#endif
	*/
	MMATCH_GAMETYPE_MAX,
};

struct REPLAY_STAGE_SETTING_NODE
{
	MUID				uidStage;
	char				szMapName[32];	// ���̸�
	char				nMapIndex;					// ���ε���
	MMATCH_GAMETYPE		nGameType;					// ����Ÿ��
	int					nRoundMax;					// ����
	int					nLimitTime;					// ���ѽð�(1 - 1��)
	int					nLimitLevel;				// ���ѷ���
	int					nMaxPlayers;				// �ִ��ο�
	bool				bTeamKillEnabled;			// ��ų����
	bool				bTeamWinThePoint;			// ������ ����
	bool				bForcedEntryEnabled;		// ������ ���� ����
	char				szStageName[64]; // Custom: Added stage name to replays
};

enum MMatchClanGrade
{
	MCG_NONE = 0,		// Ŭ������ �ƴ�
	MCG_MASTER = 1,		// Ŭ�� ������
	MCG_ADMIN = 2,		// Ŭ�� ���

	MCG_MEMBER = 9,		// �Ϲ� Ŭ����
	MCG_END
};

enum MMatchUserGradeID
{
	MMUG_FREE = 0,	// ���� ����
	MMUG_REGULAR = 1,	// ���� ����
	MMUG_STAR = 2,	// ��Ÿ����(����¯)

	MMUG_CRIMINAL = 100,	// ������
	MMUG_WARNING_1 = 101,	// 1�� ���
	MMUG_WARNING_2 = 102,	// 2�� ���
	MMUG_WARNING_3 = 103,	// 3�� ���
	MMUG_CHAT_LIMITED = 104,  // ä�� ����
	MMUG_PENALTY = 105,	// �Ⱓ ����

	MMUG_EVENTTEAM = 251,	// Custom: Event Team
	MMUG_EVENTMASTER = 252,	// �̺�Ʈ ������
	MMUG_BLOCKED = 253,	// ��� ����
	MMUG_DEVELOPER = 254,	// ������
	MMUG_ADMIN = 255	// ������
};

enum MMatchCharItemParts
{
	MMCIP_HEAD = 0,
	MMCIP_CHEST = 1,
	MMCIP_HANDS = 2,
	MMCIP_LEGS = 3,
	MMCIP_FEET = 4,
	MMCIP_FINGERL = 5,
	MMCIP_FINGERR = 6,
	MMCIP_MELEE = 7,
	MMCIP_PRIMARY = 8,
	MMCIP_SECONDARY = 9,
	MMCIP_CUSTOM1 = 10,
	MMCIP_CUSTOM2 = 11,
	MMCIP_AVATAR = 12,
	MMCIP_COMMUNITY1 = 13,
	MMCIP_COMMUNITY2 = 14,
	MMCIP_LONGBUFF1 = 15,
	MMCIP_LONGBUFF2 = 16,
	MMCIP_AVATAR_HEAD = 17,
	MMCIP_AVATAR_CHEST = 18,
	MMCIP_AVATAR_HANDS = 19,
	MMCIP_AVATAR_LEGS = 20,
	MMCIP_AVATAR_FEET = 21,
	MMCIP_END
};

#pragma pack(push)
#pragma pack(1)

struct MTD_CharInfo_V6
{
	char				szName[32];
	char				szClanName[16];
	MMatchClanGrade		nClanGrade;
	unsigned short		nClanContPoint;
	char				nCharNum;
	unsigned short		nLevel;
	char				nSex;
	char				nHair;
	char				nFace;
	unsigned long int	nXP;
	int					nBP;
	float				fBonusRate;
	unsigned short		nPrize;
	unsigned short		nHP;
	unsigned short		nAP;
	unsigned short		nMaxWeight;
	unsigned short		nSafeFalls;
	unsigned short		nFR;
	unsigned short		nCR;
	unsigned short		nER;
	unsigned short		nWR;

	// ������ ����
	unsigned long int	nEquipedItemDesc[17];

	// account �� ����
	MMatchUserGradeID	nUGradeID;

	// ClanCLID
	unsigned int		nClanCLID;

	// ������ �����ʸ�Ʈ ���
	int					nDTLastWeekGrade;

	MUID				uidEquipedItem[17];
	unsigned long int	nEquipedItemCount[17];
};

using MTD_CharInfo_FG_V7_0 = MTD_CharInfo_V6;

struct MTD_CharInfo_FG_V7_1
{
	char				szName[32];
	char				szClanName[16];
	MMatchClanGrade		nClanGrade;
	unsigned short		nClanContPoint;
	char				nCharNum;
	unsigned short		nLevel;
	char				nSex;
	char				nHair;
	char				nFace;
	unsigned long int	nXP;
	int					nBP;
	float				fBonusRate;
	unsigned short		nPrize;
	unsigned short		nHP;
	unsigned short		nAP;
	unsigned short		nMaxWeight;
	unsigned short		nSafeFalls;
	unsigned short		nFR;
	unsigned short		nCR;
	unsigned short		nER;
	unsigned short		nWR;

	unsigned long int	nEquipedItemDesc[22];

	MMatchUserGradeID	nUGradeID;

	unsigned int		nClanCLID;

	int					nDTLastWeekGrade;

	int64_t				uidEquipedItem[22];
	unsigned long int	nEquipedItemCount[22];
	unsigned long int	nEquipedItemRarity[22];
	unsigned long int	nEquipedItemLevel[22];
};

struct MTD_CharInfo_FG_V9
{
	char				szName[32];
	char				szClanName[16];
	MMatchClanGrade		nClanGrade;
	unsigned short		nClanContPoint;
	char				nCharNum;
	unsigned short		nLevel;
	char				nSex;
	char				nHair;
	char				nFace;
	unsigned long int	nXP;
	int					nBP;
	float				fBonusRate;
	unsigned short		nPrize;
	unsigned short		nHP;
	unsigned short		nAP;
	unsigned short		nMaxWeight;
	unsigned short		nSafeFalls;
	unsigned short		nFR;
	unsigned short		nCR;
	unsigned short		nER;
	unsigned short		nWR;

	// ������ ����
	unsigned long int	nEquipedItemDesc[22];

	// account �� ����
	MMatchUserGradeID	nUGradeID;

	// ClanCLID
	unsigned int		nClanCLID;

	// ������ �����ʸ�Ʈ ���
	int					nDTLastWeekGrade;

	uint32_t unk[6];

	// ������ ���� �߰�
	int64_t				uidEquipedItem[22];
	unsigned long int	nEquipedItemCount[22];
	unsigned long int	nEquipedItemRarity[22];
	unsigned long int	nEquipedItemLevel[22];

	char unk2[24];
};

enum MMatchSex
{
	MMS_MALE = 0,		// ����
	MMS_FEMALE = 1		// ����
};

struct ZCharacterProperty
{
	char		szName[32];
	char		szClanName[16];
	MMatchSex	nSex;
	int			nHair;
	int			nFace;
	int			nLevel;
	float		fMaxHP;
	float		fMaxAP;
	int			nMoveSpeed;
	int			nWeight;
	int			nMaxWeight;
	int			nSafeFall;
};

struct ZCharacterStatus
{
	int			nLife;
	int			nKills;
	int			nDeaths;
	int			nLoadingPercent;
	int			nCombo;
	int			nMaxCombo;
	int			nAllKill;
	int			nExcellent;
	int			nFantastic;
	int			nHeadShot;
	int			nUnbelievable;
	int			nExp;};

struct MTD_DuelQueueInfo
{
	MUID			m_uidChampion;
	MUID			m_uidChallenger;
	MUID			m_WaitQueue[14];				// ��
	char			m_nQueueLength;
	char			m_nVictory;						// ���¼�
	bool			m_bIsRoundEnd;					// ���� �������ΰ�
};

struct BulletInfo
{
	int Clip;
	int Magazine;
};

enum MMatchTeam
{
	MMT_ALL			= 0,
	MMT_SPECTATOR	= 1,
	MMT_RED			= 2,
	MMT_BLUE		= 3,
	MMT_END
};

using v3 = float[3];

template <size_t NumItems>
struct ZCharacterReplayStateImpl
{
	MUID UID;
	ZCharacterProperty Property;
	float HP;
	float AP;
	ZCharacterStatus Status;

	BulletInfo BulletInfos[NumItems];

	v3 Position;
	v3 Direction;

	MMatchTeam Team;

	bool Dead;

	bool HidingAdmin;
};

using ZCharacterReplayState_FG_V7_0 = ZCharacterReplayStateImpl<17>;
using ZCharacterReplayState_FG_V7_1 = ZCharacterReplayStateImpl<22>;
using ZCharacterReplayState_FG_V9 =  ZCharacterReplayStateImpl<24>;

template <typename CharInfo, typename ReplayState>
struct ReplayPlayerInfo
{
	bool IsHero;
	CharInfo Info;
	ReplayState State;
};

using ReplayPlayerInfo_FG_V7_0 = ReplayPlayerInfo<MTD_CharInfo_FG_V7_0, ZCharacterReplayState_FG_V7_0>;
using ReplayPlayerInfo_FG_V7_1 = ReplayPlayerInfo<MTD_CharInfo_FG_V7_1, ZCharacterReplayState_FG_V7_1>;
using ReplayPlayerInfo_FG_V9 = ReplayPlayerInfo<MTD_CharInfo_FG_V9, ZCharacterReplayState_FG_V9>;

#pragma pack(pop)

namespace std
{
	template <>
	class hash<MUID> : private hash<uint64_t>
	{
	public:
		size_t operator()(const MUID &UID) const
		{
			uint64_t val = (uint64_t(UID.High) << 32) | UID.Low;
			return hash<uint64_t>::operator()(val);
		}
	};
}

enum class MCommandID : uint16_t
{
	MC_PEER_BASICINFO = 0x271C,
	MC_PEER_DIE = 0x2739,
	Broken1 = 0x275F,
	Broken2 = 0x276F,
};