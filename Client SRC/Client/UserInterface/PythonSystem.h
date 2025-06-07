#pragma once

class CPythonSystem : public CSingleton<CPythonSystem>
{
public:
	enum EWindow
	{
		WINDOW_STATUS,
		WINDOW_INVENTORY,
		WINDOW_ABILITY,
		WINDOW_SOCIETY,
		WINDOW_JOURNAL,
		WINDOW_COMMAND,

		WINDOW_QUICK,
		WINDOW_GAUGE,
		WINDOW_MINIMAP,
		WINDOW_CHAT,

		WINDOW_MAX_NUM,
	};

	enum
	{
		FREQUENCY_MAX_NUM = 30,
		RESOLUTION_MAX_NUM = 100
	};

	typedef struct SResolution
	{
		DWORD width;
		DWORD height;
		DWORD bpp; // bits per pixel (high-color = 16bpp, true-color = 32bpp)

		DWORD frequency[20];
		BYTE frequency_count;
	} TResolution;

	typedef struct SWindowStatus
	{
		int isVisible;
		int isMinimized;

		int ixPosition;
		int iyPosition;
		int iHeight;
	} TWindowStatus;

	typedef struct SConfig
	{
		DWORD width;
		DWORD height;
		DWORD bpp;
		DWORD frequency;

		bool is_software_cursor;
		bool is_object_culling;
		int iDistance;
		int iShadowLevel;
#if defined(ENABLE_SHADOW_RENDER_QUALITY_OPTION)
		int iShadowQualityLevel;
		int iShadowTargetLevel;
#endif

		FLOAT music_volume;
		BYTE voice_volume;

		int gamma;

		int isSaveID;
		char SaveID[20];

		bool bWindowed;
		bool bDecompressDDS;
		bool bNoSoundCard;
		bool bUseDefaultIME;
		BYTE bSoftwareTiling;
		BYTE bCameraMode;
		bool bViewChat;
		bool bAlwaysShowName;
		bool bShowDamage;
		bool bShowSalesText;
#if defined(ENABLE_FOG_FIX)
		bool bIsFogModeEnabled;
#endif
#if defined(ENABLE_ENVIRONMENT_EFFECT_OPTION)
		bool bNightMode;
		bool bSnowMode;
		bool bSnowTextureMode;
#endif
#if defined(WJ_SHOW_MOB_INFO)
		bool bShowMobLevel;
		bool bShowMobAIFlag;
#endif
#if defined(ENABLE_OPTIMIZATION)
		bool bShowOtherCharAttacked;
#endif
#if defined(ENABLE_FOV_OPTION)
		float fFOV;
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		bool bAlwaysShowCountry;
#endif
	} TConfig;

public:
	CPythonSystem();
	virtual ~CPythonSystem();

	void Clear();
	void SetInterfaceHandler(PyObject* poHandler);
	void DestroyInterfaceHandler();

	// Config
	void SetDefaultConfig();
	bool LoadConfig();
	bool SaveConfig();
	void ApplyConfig();
	void SetConfig(TConfig* set_config);
	TConfig* GetConfig();
	void ChangeSystem();

	// Interface
	bool LoadInterfaceStatus();
	void SaveInterfaceStatus();
	bool isInterfaceConfig();
	const TWindowStatus& GetWindowStatusReference(int iIndex);

	DWORD GetWidth();
	DWORD GetHeight();
	DWORD GetBPP();
	DWORD GetFrequency();
	bool IsSoftwareCursor();
	bool IsWindowed();
	bool IsViewChat();
	bool IsAlwaysShowName();
	bool IsShowDamage();
	bool IsShowSalesText();
	bool IsUseDefaultIME();
	bool IsNoSoundCard();
	bool IsAutoTiling();
	bool IsSoftwareTiling();
	void SetSoftwareTiling(bool isEnable);
	void SetViewChatFlag(int iFlag);
	void SetAlwaysShowNameFlag(int iFlag);
	void SetShowDamageFlag(int iFlag);
	void SetShowSalesTextFlag(int iFlag);

	void SetCameraMode(BYTE bMode);
	BYTE GetCameraMode() const;

#if defined(ENABLE_FOG_FIX)
	void SetFogMode(bool bEnable);
	bool GetFogMode() const;
#endif

#if defined(ENABLE_ENVIRONMENT_EFFECT_OPTION)
	void SetNightModeOption(bool bEnable);
	bool GetNightModeOption() const;

	void SetSnowModeOption(bool bEnable);
	bool GetSnowModeOption() const;

	void SetSnowTextureModeOption(bool bEnable);
	bool GetSnowTextureModeOption() const;
#endif

#if defined(WJ_SHOW_MOB_INFO)
	void SetShowMobAIFlag(int iFlag);
	bool IsShowMobAIFlag();
	void SetShowMobLevel(int iFlag);
	bool IsShowMobLevel();
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	// Country Flag
	bool IsAlwaysShowCountry();
	void SetAlwaysShowCountryFlag(int iFlag);
#endif

#if defined(ENABLE_FOV_OPTION)
	// FoV
	float GetFOV();
	void SetFOV(const float c_fValue);
#endif

	// Window
	void SaveWindowStatus(int iIndex, int iVisible, int iMinimized, int ix, int iy, int iHeight);

	// SaveID
	int IsSaveID();
	const char* GetSaveID();
	void SetSaveID(int iValue, const char* c_szSaveID);

	/// Display
	void GetDisplaySettings();

	int GetResolutionCount();
	int GetFrequencyCount(int index);
	bool GetResolution(int index, OUT DWORD* width, OUT DWORD* height, OUT DWORD* bpp);
	bool GetFrequency(int index, int freq_index, OUT DWORD* frequncy);
	int GetResolutionIndex(DWORD width, DWORD height, DWORD bpp);
	int GetFrequencyIndex(int res_index, DWORD frequency);
	bool isViewCulling();

	// Sound
	float GetMusicVolume();
	int GetSoundVolume();
	void SetMusicVolume(float fVolume);
	void SetSoundVolumef(float fVolume);

	int GetDistance();

	// Shadow
	int GetShadowLevel();
	void SetShadowLevel(unsigned int level);

#if defined(ENABLE_SHADOW_RENDER_QUALITY_OPTION)
	int GetShadowQualityLevel();
	void SetShadowQualityLevel(unsigned int level);

	int GetShadowTargetLevel();
	void SetShadowTargetLevel(unsigned int level);
#endif

#if defined(ENABLE_CHAR_CONFIG)
	void SetDefaultCharConfig();
	void CreateCharConfigPath();
	void LoadCharConfig();
	void SaveCharConfig();
#endif

#if defined(ENABLE_OPTIMIZATION)
	void SetShowOtherCharAttacked(bool bEnable);
	bool IsShowOtherCharAttacked() const;
#endif

protected:
	TResolution m_ResolutionList[RESOLUTION_MAX_NUM];
	int m_ResolutionCount;

	TConfig m_Config;
	TConfig m_OldConfig;

	bool m_isInterfaceConfig;
	PyObject* m_poInterfaceHandler;
	TWindowStatus m_WindowStatus[WINDOW_MAX_NUM];

#if defined(_DEBUG) && defined(__IMGUI__)
public:
	PyObject* GetInterfaceHandler() const { return m_poInterfaceHandler; }
#endif
};
