#include <cstdint>

struct MUID{
	uint32_t	High;	///< High 4 Byte
	uint32_t	Low;	///< High 4 Byte

	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator > (const MUID& a, const MUID& b){
		if(a.High>b.High) return true;
		if(a.High==b.High){
			if(a.Low>b.Low) return true;
		}
		return false;
	}
	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator >= (const MUID& a, const MUID& b){
		if(a.High>b.High) return true;
		if(a.High==b.High){
			if(a.Low>=b.Low) return true;
		}
		return false;
	}
	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator < (const MUID& a, const MUID& b){
		if(a.High<b.High) return true;
		if(a.High==b.High){
			if(a.Low<b.Low) return true;
		}
		return false;
	}
	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator <= (const MUID& a, const MUID& b){
		if(a.High<b.High) return true;
		if(a.High==b.High){
			if(a.Low<=b.Low) return true;
		}
		return false;
	}

	/// 대입 오퍼레이터 오버로딩
	inline MUID& operator=(int v){
		High = 0;
		Low = v;
		return *this;
	}
	/// 대입 오퍼레이터 오버로딩
	inline MUID& operator=(const MUID& a){
		High = a.High;
		Low = a.Low;
		return *this;
	}
	/// 비교 오퍼레이터 오버로딩
	inline friend bool operator==(const MUID& a, const MUID& b){
		if(a.High==b.High){
			if(a.Low==b.Low) return true;
		}
		return false;
	}
	/// 비교 오퍼레이터 오버로딩
	inline friend bool operator!=(const MUID& a, const MUID& b){
		if(a.Low!=b.Low) return true;
		if(a.High!=b.High) return true;
		return false;
	}
};

enum MMATCH_GAMETYPE {
	MMATCH_GAMETYPE_DEATHMATCH_SOLO = 0,			///< 개인 데쓰매치
	MMATCH_GAMETYPE_DEATHMATCH_TEAM = 1,			///< 팀 데쓰매치
	MMATCH_GAMETYPE_GLADIATOR_SOLO = 2,			///< 개인 글래디에이터
	MMATCH_GAMETYPE_GLADIATOR_TEAM = 3,			///< 팀 글래디에이터
	MMATCH_GAMETYPE_ASSASSINATE = 4,			///< 보스전(암살전)
	MMATCH_GAMETYPE_TRAINING = 5,			///< 연습

	MMATCH_GAMETYPE_SURVIVAL = 6,			///< 서바이벌
	MMATCH_GAMETYPE_QUEST = 7,			///< 퀘스트

	MMATCH_GAMETYPE_BERSERKER = 8,			///< 데쓰매치 버서커
	MMATCH_GAMETYPE_DEATHMATCH_TEAM2 = 9,			///< 팀데쓰매치 익스트림
	MMATCH_GAMETYPE_DUEL = 10,		///< 듀얼 매치
	MMATCH_GAMETYPE_DUELTOURNAMENT = 11,		///< 듀얼 토너먼트
	MMATCH_GAMETYPE_QUEST_CHALLENGE = 12,		///< 챌린지퀘스트

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
	char				szMapName[32];	// 맵이름
	char				nMapIndex;					// 맵인덱스
	MMATCH_GAMETYPE		nGameType;					// 게임타입
	int					nRoundMax;					// 라운드
	int					nLimitTime;					// 제한시간(1 - 1분)
	int					nLimitLevel;				// 제한레벨
	int					nMaxPlayers;				// 최대인원
	bool				bTeamKillEnabled;			// 팀킬여부
	bool				bTeamWinThePoint;			// 선승제 여부
	bool				bForcedEntryEnabled;		// 게임중 참가 여부
	char				szStageName[64]; // Custom: Added stage name to replays
};

enum MMatchClanGrade
{
	MCG_NONE = 0,		// 클랜원이 아님
	MCG_MASTER = 1,		// 클랜 마스터
	MCG_ADMIN = 2,		// 클랜 운영자

	MCG_MEMBER = 9,		// 일반 클랜원
	MCG_END
};

enum MMatchUserGradeID
{
	MMUG_FREE = 0,	// 무료 계정
	MMUG_REGULAR = 1,	// 정액 유저
	MMUG_STAR = 2,	// 스타유저(게임짱)

	MMUG_CRIMINAL = 100,	// 전과자
	MMUG_WARNING_1 = 101,	// 1차 경고
	MMUG_WARNING_2 = 102,	// 2차 경고
	MMUG_WARNING_3 = 103,	// 3차 경고
	MMUG_CHAT_LIMITED = 104,  // 채팅 금지
	MMUG_PENALTY = 105,	// 기간 정지

	MMUG_EVENTTEAM = 251,	// Custom: Event Team
	MMUG_EVENTMASTER = 252,	// 이벤트 진행자
	MMUG_BLOCKED = 253,	// 사용 정지
	MMUG_DEVELOPER = 254,	// 개발자
	MMUG_ADMIN = 255	// 관리자
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

	// 아이템 정보
	unsigned long int	nEquipedItemDesc[17];

	// account 의 정보
	MMatchUserGradeID	nUGradeID;

	// ClanCLID
	unsigned int		nClanCLID;

	// 지난주 듀얼토너먼트 등급
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

	// 아이템 정보
	unsigned long int	nEquipedItemDesc[22];

	// account 의 정보
	MMatchUserGradeID	nUGradeID;

	// ClanCLID
	unsigned int		nClanCLID;

	// 지난주 듀얼토너먼트 등급
	int					nDTLastWeekGrade;

	uint32_t unk[6];

	// 아이템 정보 추가
	int64_t				uidEquipedItem[22];
	unsigned long int	nEquipedItemCount[22];
	unsigned long int	nEquipedItemRarity[22];
	unsigned long int	nEquipedItemLevel[22];

	char unk2[24];
};

enum MMatchSex
{
	MMS_MALE = 0,		// 남자
	MMS_FEMALE = 1		// 여자
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
	MUID			m_WaitQueue[14];				// 팀
	char			m_nQueueLength;
	char			m_nVictory;						// 연승수
	bool			m_bIsRoundEnd;					// 라운드 끝날때인가
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