#if !defined(__INC_PYTHON_BACKGROUND_H__)
#define __INC_PYTHON_BACKGROUND_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../GameLib/MapManager.h"
#include "../GameLib/TerrainDecal.h"
#include "../GameLib/SnowEnvironment.h"

class CInstanceBase;

class CPythonBackground : public CMapManager, public CSingleton<CPythonBackground>
{
public:
	enum EShadow
	{
		SHADOW_NONE,
		SHADOW_GROUND,
		SHADOW_GROUND_AND_SOLO,
		SHADOW_ALL,
		SHADOW_ALL_HIGH,
		SHADOW_ALL_MAX,
	};

#if defined(ENABLE_SHADOW_RENDER_QUALITY_OPTION)
	enum EShadowQuality
	{
		SHADOW_QUALITY_BAD,
		SHADOW_QUALITY_AVERAGE,
		SHADOW_QUALITY_GOOD,
	};

	enum EShadowTarget
	{
		SHADOW_TARGET_NONE,
		SHADOW_TARGET_GROUND,
		SHADOW_TARGET_GROUND_AND_SOLO,
		SHADOW_TARGET_ALL,
	};
#endif

	enum
	{
		DISTANCE0,
		DISTANCE1,
		DISTANCE2,
		DISTANCE3,
		DISTANCE4,
		NUM_DISTANCE_SET
	};

	enum
	{
		DAY_MODE_LIGHT,
		DAY_MODE_DARK,
	};

	typedef struct SVIEWDISTANCESET
	{
		float m_fFogStart;
		float m_fFogEnd;
		float m_fFarClip;
		D3DXVECTOR3 m_v3SkyBoxScale;
	} TVIEWDISTANCESET;

public:
	CPythonBackground();
	virtual ~CPythonBackground();

	void Initialize();

	void Destroy();
	void Create();

	void GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY);
	void LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY);

	void EnableTerrainOnlyForHeight();
	bool SetSplatLimit(int iSplatNum);
	bool SetVisiblePart(int ePart, bool isVisible);
	bool SetShadowLevel(int eLevel);
#if defined(ENABLE_SHADOW_RENDER_QUALITY_OPTION)
	bool SetShadowQualityLevel(int eLevel);
	bool SetShadowTargetLevel(int eLevel);
#endif
	void RefreshShadowLevel();
	void SelectViewDistanceNum(int eNum);
	void SetViewDistanceSet(int eNum, float fFarClip);
	float GetFarClip();

	DWORD GetRenderShadowTime();
	void GetDistanceSetInfo(int* peNum, float* pfStart, float* pfEnd, float* pfFarClip);

	bool GetPickingPoint(D3DXVECTOR3* v3IntersectPt);
	bool GetPickingPointWithRay(const CRay& rRay, D3DXVECTOR3* v3IntersectPt);
	bool GetPickingPointWithRayOnlyTerrain(const CRay& rRay, D3DXVECTOR3* v3IntersectPt);
	BOOL GetLightDirection(D3DXVECTOR3& rv3LightDirection);

	void Update(float fCenterX, float fCenterY, float fCenterZ);

	void CreateCharacterShadowTexture();
	void ReleaseCharacterShadowTexture();
	void Render();
	void RenderSnow();
	void RenderPCBlocker();
	void RenderCollision();
	void RenderCharacterShadowToTexture();
	void RenderSky();
	void RenderCloud();
	void RenderWater();
	void RenderEffect();
	void RenderBeforeLensFlare();
	void RenderAfterLensFlare();

	bool CheckAdvancing(CInstanceBase* pInstance);

	void SetCharacterDirLight();
	void SetBackgroundDirLight();

	void ChangeToDay();
	void ChangeToNight();
	int GetDayMode() { return m_iDayMode; }

	void EnableSnowEnvironment();
	void DisableSnowEnvironment();

	void SetXMaxTree(int iGrade);

	void ClearGuildArea();
	void RegisterGuildArea(int isx, int isy, int iex, int iey);

	void CreateTargetEffect(DWORD dwID, DWORD dwChrVID);
	void CreateTargetEffect(DWORD dwID, long lx, long ly);
	void DeleteTargetEffect(DWORD dwID = 0);

	void CreateSpecialEffect(DWORD dwID, float fx, float fy, float fz, const char* c_szFileName);
	void DeleteSpecialEffect(DWORD dwID);

#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	void CreatePrivateShopPos(DWORD dwID, DWORD dwChrVID);
	void CreatePrivateShopPos(DWORD dwID, long lx, long ly);
	void DeletePrivateShopPos(DWORD dwID);
#endif

#if defined(ENABLE_CONQUEROR_LEVEL)
	bool HasSungMaAttr();
	const CMapBase::SSungmaAttr& GetSungMaAttrData();
#endif

	void Warp(DWORD dwX, DWORD dwY);

	void VisibleGuildArea();
	void DisableGuildArea();

	void RegisterDungeonMapName(const char* c_szMapName);
	TMapInfo* GlobalPositionToMapInfo(DWORD dwGlobalX, DWORD dwGlobalY);
	const char* GetWarpMapName();

#if defined(ENABLE_ENVIRONMENT_EFFECT_OPTION)
public:
	bool IsBoomMap(const std::string& sMapName) const;
	bool IsSnowMap(const std::string& sMapName) const;
	int GetDayMode() const;

	void SetXMasShowEvent(bool bMode);
	bool GetXMasShowEvent() const;

	void SetNightModeOption(bool bMode);
	bool GetNightModeOption() const;

	void SetSnowModeOption(bool bMode);
	bool GetSnowModeOption() const;

	void SetSnowTextureModeOption(bool bMode);
	bool GetSnowTextureModeOption() const;

private:
	bool m_bXMasShowEvent;
	bool m_bNightMode;
	bool m_bSnowMode;
	bool m_bSnowTextureMode;
#endif

#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
public:
	void SetLateSummerEvent(bool bEnable);
	bool GetLateSummerEvent() const;

private:
	bool m_bLateSummerEvent;
#endif

protected:
	void __CreateProperty();
	bool __IsSame(std::set<int>& rleft, std::set<int>& rright);

protected:
	std::string m_strMapName;

private:
	CSnowEnvironment m_SnowEnvironment;

	int m_iDayMode;
	int m_iXMasTreeGrade;

	int m_eShadowLevel;
#if defined(ENABLE_SHADOW_RENDER_QUALITY_OPTION)
	int m_eShadowQualityLevel;
	int m_eShadowTargetLevel;
#endif
	int m_eViewDistanceNum;

	BOOL m_bVisibleGuildArea;

	DWORD m_dwRenderShadowTime;

	DWORD m_dwBaseX;
	DWORD m_dwBaseY;

	TVIEWDISTANCESET m_ViewDistanceSet[NUM_DISTANCE_SET];

	std::set<int> m_kSet_iShowingPortalID;
	std::set<std::string> m_kSet_strDungeonMapName;
	std::map<DWORD, DWORD> m_kMap_dwTargetID_dwChrID;

	struct SReserveTargetEffect
	{
		int ilx;
		int ily;
	};
	std::map<DWORD, SReserveTargetEffect> m_kMap_dwID_kReserveTargetEffect;
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	std::map<DWORD, DWORD> m_kMap_dwTargetID_dwPrivateShopChrID;
	std::map<DWORD, SReserveTargetEffect> m_kMap_dwID_kReservePrivateShopPosTargetEffect;
#endif

	struct SFindWarpMapName
	{
		DWORD m_dwX, m_dwY;
		SFindWarpMapName(DWORD dwX, DWORD dwY)
		{
			m_dwX = dwX;
			m_dwY = dwY;
		}
		bool operator() (TMapInfo& rMapInfo)
		{
			if (m_dwX < rMapInfo.m_dwBaseX || m_dwX >= rMapInfo.m_dwEndX || m_dwY < rMapInfo.m_dwBaseY || m_dwY >= rMapInfo.m_dwEndY)
				return false;
			return true;
		}
	};
};

#endif // __INC_PYTHON_BACKGROUND_H__
