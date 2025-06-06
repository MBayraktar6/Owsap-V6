#include "StdAfx.h"

#include "PythonApplication.h"

#include "../EterLib/Camera.h"
#include "../EterPack/EterPackManager.h"

extern bool PERF_CHECKER_RENDER_GAME;
extern D3DXCOLOR g_fSpecularColor;
extern BOOL bVisibleNotice = true;
extern BOOL bTestServerFlag = FALSE;
extern int TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE = 0;

PyObject* appShowWebPage(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bType;
	if (!PyTuple_GetInteger(poArgs, 0, &bType))
		return Py_BuildException();

	char* szWebPage;
	if (!PyTuple_GetString(poArgs, 1, &szWebPage))
		return Py_BuildException();

	PyObject* poRect = PyTuple_GetItem(poArgs, 2);
	if (!PyTuple_Check(poRect))
		return Py_BuildException();

	RECT rcWebPage;
	rcWebPage.left = PyInt_AsLong(PyTuple_GetItem(poRect, 0));
	rcWebPage.top = PyInt_AsLong(PyTuple_GetItem(poRect, 1));
	rcWebPage.right = PyInt_AsLong(PyTuple_GetItem(poRect, 2));
	rcWebPage.bottom = PyInt_AsLong(PyTuple_GetItem(poRect, 3));

	CPythonApplication::Instance().ShowWebPage(bType, szWebPage, rcWebPage);
	return Py_BuildNone();
}

PyObject* appMoveWebPage(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bType;
	if (!PyTuple_GetInteger(poArgs, 0, &bType))
		return Py_BuildException();

	PyObject* poRect = PyTuple_GetItem(poArgs, 1);
	if (!PyTuple_Check(poRect))
		return Py_BuildException();

	RECT rcWebPage;
	rcWebPage.left = PyInt_AsLong(PyTuple_GetItem(poRect, 0));
	rcWebPage.top = PyInt_AsLong(PyTuple_GetItem(poRect, 1));
	rcWebPage.right = PyInt_AsLong(PyTuple_GetItem(poRect, 2));
	rcWebPage.bottom = PyInt_AsLong(PyTuple_GetItem(poRect, 3));

	CPythonApplication::Instance().MoveWebPage(bType, rcWebPage);
	return Py_BuildNone();
}

PyObject* appHideWebPage(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bType;
	if (!PyTuple_GetInteger(poArgs, 0, &bType))
		return Py_BuildException();

	CPythonApplication::Instance().HideWebPage(bType);
	return Py_BuildNone();
}

PyObject* appIsWebPageMode(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bType;
	if (!PyTuple_GetInteger(poArgs, 0, &bType))
		return Py_BuildException();

	return Py_BuildValue("b", CPythonApplication::Instance().IsWebPageMode(bType));
}

PyObject* appEnablePerformanceTime(PyObject* poSelf, PyObject* poArgs)
{
	char* szMode;
	if (!PyTuple_GetString(poArgs, 0, &szMode))
		return Py_BuildException();

	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 1, &nEnable))
		return Py_BuildException();

	bool isEnable = nEnable ? true : false;

	if (strcmp(szMode, "RENDER_GAME") == 0)
		PERF_CHECKER_RENDER_GAME = isEnable;

	return Py_BuildNone();
}

/////////////////////////////////////////////////////

extern BOOL HAIR_COLOR_ENABLE;
extern BOOL USE_ARMOR_SPECULAR;
extern BOOL USE_WEAPON_SPECULAR;
extern BOOL SKILL_EFFECT_UPGRADE_ENABLE;
extern BOOL RIDE_HORSE_ENABLE;
extern double g_specularSpd;

// TEXTTAIL_LIVINGTIME_CONTROL
extern void TextTail_SetLivingTime(long livingTime);

PyObject* appSetTextTailLivingTime(PyObject* poSelf, PyObject* poArgs)
{
	float livingTime;
	if (!PyTuple_GetFloat(poArgs, 0, &livingTime))
		return Py_BuildException();

	TextTail_SetLivingTime(livingTime * 1000);

	return Py_BuildNone();
}
// END_OF_TEXTTAIL_LIVINGTIME_CONTROL

PyObject* appSetHairColorEnable(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	HAIR_COLOR_ENABLE = nEnable;

	return Py_BuildNone();
}

PyObject* appSetArmorSpecularEnable(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	USE_ARMOR_SPECULAR = nEnable;

	return Py_BuildNone();
}

PyObject* appSetWeaponSpecularEnable(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	USE_WEAPON_SPECULAR = nEnable;

	return Py_BuildNone();
}

PyObject* appSetSkillEffectUpgradeEnable(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	SKILL_EFFECT_UPGRADE_ENABLE = nEnable;

	return Py_BuildNone();
}

PyObject* SetTwoHandedWeaponAttSpeedDecreaseValue(PyObject* poSelf, PyObject* poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();

	TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE = iValue;

	return Py_BuildNone();
}

PyObject* appSetRideHorseEnable(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	RIDE_HORSE_ENABLE = nEnable;

	return Py_BuildNone();
}

PyObject* appSetCameraMaxDistance(PyObject* poSelf, PyObject* poArgs)
{
	float fMax;
	if (!PyTuple_GetFloat(poArgs, 0, &fMax))
		return Py_BuildException();

	const std::string c_rstrMapFileName = CPythonBackground::Instance().GetWarpMapName();

	if (!c_rstrMapFileName.compare("metin2_map_n_flame_dragon"))
		fMax = 6000.0f;
	else if (!c_rstrMapFileName.compare("metin2_12zi_stage"))
		fMax = 5000.0f;
	else if (!c_rstrMapFileName.compare("metin2_map_defensewave"))
		fMax = 5000.0f;
	else if (!c_rstrMapFileName.compare("metin2_map_miniboss_01"))
		fMax = 5000.0f;
	else if (!c_rstrMapFileName.compare("metin2_map_miniboss_02"))
		fMax = 5000.0f;
	else if (!c_rstrMapFileName.compare("metin2_map_mists_of_island"))
		fMax = 5000.0f;

	CCamera::SetCameraMaxDistance(fMax);
	return Py_BuildNone();
}

PyObject* appSetControlFP(PyObject* poSelf, PyObject* poArgs)
{
	_controlfp(_PC_24, _MCW_PC);
	return Py_BuildNone();
}

PyObject* appSetSpecularSpeed(PyObject* poSelf, PyObject* poArgs)
{
	float fSpeed;
	if (!PyTuple_GetFloat(poArgs, 0, &fSpeed))
		return Py_BuildException();

	g_specularSpd = fSpeed;

	return Py_BuildNone();
}

PyObject* appSetMinFog(PyObject* poSelf, PyObject* poArgs)
{
	float fMinFog;
	if (!PyTuple_GetFloat(poArgs, 0, &fMinFog))
		return Py_BuildException();

	CPythonApplication::Instance().SetMinFog(fMinFog);
	return Py_BuildNone();
}

PyObject* appSetFrameSkip(PyObject* poSelf, PyObject* poArgs)
{
	int nFrameSkip;
	if (!PyTuple_GetInteger(poArgs, 0, &nFrameSkip))
		return Py_BuildException();

	CPythonApplication::Instance().SetFrameSkip(nFrameSkip ? true : false);
	return Py_BuildNone();
}

// LOCALE

PyObject* appForceSetLocale(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	char* szLocalePath;
	if (!PyTuple_GetString(poArgs, 1, &szLocalePath))
		return Py_BuildException();

	LocaleService_ForceSetLocale(szName, szLocalePath);

	return Py_BuildNone();
}

PyObject* appGetLocaleServiceName(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", LocaleService_GetName());
}

// 
bool LoadLocaleData(const char* localePath);

PyObject* appSetCHEONMA(PyObject* poSelf, PyObject* poArgs)
{
	int enable;
	if (!PyTuple_GetInteger(poArgs, 0, &enable))
		return Py_BuildException();

	LocaleService_SetCHEONMA(enable ? true : false);
	return Py_BuildNone();
}

PyObject* appIsCHEONMA(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", LocaleService_IsCHEONMA());
}

PyObject* appLoadLocaleAddr(PyObject* poSelf, PyObject* poArgs)
{
	char* addrPath;
	if (!PyTuple_GetString(poArgs, 0, &addrPath))
		return Py_BuildException();

	FILE* fp = fopen(addrPath, "rb");
	if (!fp)
		return Py_BuildException();

	fseek(fp, 0, SEEK_END);

	int size = ftell(fp);
	char* enc = (char*)_alloca(size);
	fseek(fp, 0, SEEK_SET);
	fread(enc, size, 1, fp);
	fclose(fp);

	static const unsigned char key[16] = {
		0x82, 0x1b, 0x34, 0xae,
		0x12, 0x3b, 0xfb, 0x17,
		0xd7, 0x2c, 0x39, 0xae,
		0x41, 0x98, 0xf1, 0x63
	};

	char* buf = (char*)_alloca(size);
	//int decSize = 
	old_tea_decrypt((unsigned long*)buf, (const unsigned long*)enc, (const unsigned long*)key, size);
	unsigned int retSize = *(unsigned int*)buf;
	char* ret = buf + sizeof(unsigned int);
	return Py_BuildValue("s#", ret, retSize);
}

PyObject* appLoadLocaleData(PyObject* poSelf, PyObject* poArgs)
{
	char* localePath;
	if (!PyTuple_GetString(poArgs, 0, &localePath))
		return Py_BuildException();

	return Py_BuildValue("i", LoadLocaleData(localePath));
}

PyObject* appGetLocaleName(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", LocaleService_GetLocaleName());
}

PyObject* appGetLocalePath(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", LocaleService_GetLocalePath());
}
// END_OF_LOCALE

PyObject* appGetDefaultCodePage(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", LocaleService_GetCodePage());
}

PyObject* appGetImageInfo(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	BOOL canLoad = FALSE;
	ILuint uWidth = 0;
	ILuint uHeight = 0;

	ILuint uImg;
	ilGenImages(1, &uImg);
	ilBindImage(uImg);
	if (ilLoad(IL_TYPE_UNKNOWN, szFileName))
	{
		canLoad = TRUE;
		uWidth = ilGetInteger(IL_IMAGE_WIDTH);
		uHeight = ilGetInteger(IL_IMAGE_HEIGHT);
	}

	ilDeleteImages(1, &uImg);

	return Py_BuildValue("iii", canLoad, uWidth, uHeight);
}

PyObject* appIsExistFile(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	bool isExist = CEterPackManager::Instance().isExist(szFileName);

	return Py_BuildValue("i", isExist);
}

PyObject* appGetFileList(PyObject* poSelf, PyObject* poArgs)
{
	char* szFilter;
	if (!PyTuple_GetString(poArgs, 0, &szFilter))
		return Py_BuildException();

	PyObject* poList = PyList_New(0);

	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(wfd));

	HANDLE hFind = FindFirstFile(szFilter, &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			PyObject* poFileName = PyString_FromString(wfd.cFileName);
			PyList_Append(poList, poFileName);
		} while (FindNextFile(hFind, &wfd));

		FindClose(hFind);
	}

	return poList;
}

PyObject* appUpdateGame(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().UpdateGame();
	return Py_BuildNone();
}

PyObject* appRenderGame(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().RenderGame();
	return Py_BuildNone();
}

PyObject* appSetMouseHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonApplication::Instance().SetMouseHandler(poHandler);
	return Py_BuildNone();
}

PyObject* appCreate(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	int width;
	if (!PyTuple_GetInteger(poArgs, 1, &width))
		return Py_BuildException();

	int height;
	if (!PyTuple_GetInteger(poArgs, 2, &height))
		return Py_BuildException();

	int Windowed;
	if (!PyTuple_GetInteger(poArgs, 3, &Windowed))
		return Py_BuildException();

	CPythonApplication& rkApp = CPythonApplication::Instance();
	if (!rkApp.Create(poSelf, szName, width, height, Windowed))
	{
		//return Py_BuildNone();
		return NULL;
	}

	return Py_BuildNone();
}

PyObject* appLoop(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().Loop();

	return Py_BuildNone();
}

PyObject* appGetInfo(PyObject* poSelf, PyObject* poArgs)
{
	int nInfo;
	if (!PyTuple_GetInteger(poArgs, 0, &nInfo))
		return Py_BuildException();

	std::string stInfo;
	CPythonApplication::Instance().GetInfo(nInfo, &stInfo);
	return Py_BuildValue("s", stInfo.c_str());
}

PyObject* appProcess(PyObject* poSelf, PyObject* poArgs)
{
	if (CPythonApplication::Instance().Process())
		return Py_BuildValue("i", 1);

	return Py_BuildValue("i", 0);
}

PyObject* appAbort(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().Abort();
	return Py_BuildNone();
}

PyObject* appExit(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().Exit();
	return Py_BuildNone();
}

PyObject* appSetCamera(PyObject* poSelf, PyObject* poArgs)
{
	float Distance;
	if (!PyTuple_GetFloat(poArgs, 0, &Distance))
		return Py_BuildException();

	float Pitch;
	if (!PyTuple_GetFloat(poArgs, 1, &Pitch))
		return Py_BuildException();

	float Rotation;
	if (!PyTuple_GetFloat(poArgs, 2, &Rotation))
		return Py_BuildException();

	float fDestinationHeight;
	if (!PyTuple_GetFloat(poArgs, 3, &fDestinationHeight))
		return Py_BuildException();

	CPythonApplication::Instance().SetCamera(Distance, Pitch, Rotation, fDestinationHeight);
	return Py_BuildNone();
}

PyObject* appGetCamera(PyObject* poSelf, PyObject* poArgs)
{
	float Distance, Pitch, Rotation, DestinationHeight;
	CPythonApplication::Instance().GetCamera(&Distance, &Pitch, &Rotation, &DestinationHeight);

	return Py_BuildValue("ffff", Distance, Pitch, Rotation, DestinationHeight);
}

PyObject* appGetCameraPitch(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetPitch());
}

PyObject* appGetCameraRotation(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetRotation());
}

PyObject* appGetTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetGlobalTime());
}

PyObject* appGetGlobalTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetServerTime());
}

PyObject* appGetGlobalTimeStamp(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetServerTimeStamp());
}

PyObject* appGetUpdateFPS(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetUpdateFPS());
}

PyObject* appGetRenderFPS(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetRenderFPS());
}

PyObject* appRotateCamera(PyObject* poSelf, PyObject* poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().RotateCamera(iDirection);
	return Py_BuildNone();
}

PyObject* appPitchCamera(PyObject* poSelf, PyObject* poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().PitchCamera(iDirection);
	return Py_BuildNone();
}

PyObject* appZoomCamera(PyObject* poSelf, PyObject* poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().ZoomCamera(iDirection);
	return Py_BuildNone();
}

PyObject* appMovieRotateCamera(PyObject* poSelf, PyObject* poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().MovieRotateCamera(iDirection);
	return Py_BuildNone();
}

PyObject* appMoviePitchCamera(PyObject* poSelf, PyObject* poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().MoviePitchCamera(iDirection);
	return Py_BuildNone();
}

PyObject* appMovieZoomCamera(PyObject* poSelf, PyObject* poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().MovieZoomCamera(iDirection);
	return Py_BuildNone();
}

PyObject* appMovieResetCamera(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().MovieResetCamera();
	return Py_BuildNone();
}

PyObject* appGetFaceSpeed(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetFaceSpeed());
}

PyObject* appGetRenderTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("fi",
		CPythonApplication::Instance().GetAveRenderTime(),
		CPythonApplication::Instance().GetCurRenderTime());
}

PyObject* appGetUpdateTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetCurUpdateTime());
}

PyObject* appGetLoad(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetLoad());
}
PyObject* appGetFaceCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetFaceCount());
}

PyObject* appGetAvaiableTextureMememory(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CGraphicBase::GetAvailableTextureMemory());
}

PyObject* appSetFPS(PyObject* poSelf, PyObject* poArgs)
{
	int iFPS;
	if (!PyTuple_GetInteger(poArgs, 0, &iFPS))
		return Py_BuildException();

	CPythonApplication::Instance().SetFPS(iFPS);

	return Py_BuildNone();
}

PyObject* appSetGlobalCenterPosition(PyObject* poSelf, PyObject* poArgs)
{
	int x;
	if (!PyTuple_GetInteger(poArgs, 0, &x))
		return Py_BuildException();

	int y;
	if (!PyTuple_GetInteger(poArgs, 1, &y))
		return Py_BuildException();

	CPythonApplication::Instance().SetGlobalCenterPosition(x, y);
	return Py_BuildNone();
}


PyObject* appSetCenterPosition(PyObject* poSelf, PyObject* poArgs)
{
	float fx;
	if (!PyTuple_GetFloat(poArgs, 0, &fx))
		return Py_BuildException();

	float fy;
	if (!PyTuple_GetFloat(poArgs, 1, &fy))
		return Py_BuildException();

	float fz;
	if (!PyTuple_GetFloat(poArgs, 2, &fz))
		return Py_BuildException();

	CPythonApplication::Instance().SetCenterPosition(fx, -fy, fz);
	return Py_BuildNone();
}

PyObject* appGetCursorPosition(PyObject* poSelf, PyObject* poArgs)
{
	long lx, ly;
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.GetMousePosition(lx, ly);

	return Py_BuildValue("ii", lx, ly);
}

PyObject* appRunPythonFile(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	bool ret = CPythonLauncher::Instance().RunFile(szFileName);
	return Py_BuildValue("i", ret);
}

PyObject* appIsPressed(PyObject* poSelf, PyObject* poArgs)
{
	int iKey;
	if (!PyTuple_GetInteger(poArgs, 0, &iKey))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonApplication::Instance().IsPressed(iKey));
}

PyObject* appSetCursor(PyObject* poSelf, PyObject* poArgs)
{
	/*
		char * szName;
		if (!PyTuple_GetString(poArgs, 0, &szName))
			return Py_BuildException();

		if (!CPythonApplication::Instance().SetHardwareCursor(szName))
			return Py_BuildException("Wrong Cursor Name [%s]", szName);
	*/
	int iCursorNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iCursorNum))
		return Py_BuildException();

	if (!CPythonApplication::Instance().SetCursorNum(iCursorNum))
		return Py_BuildException("Wrong Cursor Name [%d]", iCursorNum);

	return Py_BuildNone();
}

PyObject* appGetCursor(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetCursorNum());
}

PyObject* appShowCursor(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().SetCursorVisible(TRUE);

	return Py_BuildNone();
}

PyObject* appHideCursor(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().SetCursorVisible(FALSE);

	return Py_BuildNone();
}

PyObject* appIsShowCursor(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", TRUE == CPythonApplication::Instance().GetCursorVisible());
}

PyObject* appIsLiarCursorOn(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", TRUE == CPythonApplication::Instance().GetLiarCursorOn());
}

PyObject* appSetSoftwareCursor(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_SOFTWARE);
	return Py_BuildNone();
}

PyObject* appSetHardwareCursor(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_HARDWARE);
	return Py_BuildNone();
}

PyObject* appSetConnectData(PyObject* poSelf, PyObject* poArgs)
{
	char* szIP;
	if (!PyTuple_GetString(poArgs, 0, &szIP))
		return Py_BuildException();

	int iPort;
	if (!PyTuple_GetInteger(poArgs, 1, &iPort))
		return Py_BuildException();

	CPythonApplication::Instance().SetConnectData(szIP, iPort);

	return Py_BuildNone();
}

PyObject* appGetConnectData(PyObject* poSelf, PyObject* poArgs)
{
	std::string strIP;
	int iPort;

	CPythonApplication::Instance().GetConnectData(strIP, iPort);

	return Py_BuildValue("si", strIP.c_str(), iPort);
}

PyObject* appGetRandom(PyObject* poSelf, PyObject* poArgs)
{
	int from;
	if (!PyTuple_GetInteger(poArgs, 0, &from))
		return Py_BuildException();

	int to;
	if (!PyTuple_GetInteger(poArgs, 1, &to))
		return Py_BuildException();

	if (from > to)
	{
		int tmp = from;
		from = to;
		to = tmp;
	}

	return Py_BuildValue("i", random_range(from, to));
}

PyObject* appGetRotatingDirection(PyObject* poSelf, PyObject* poArgs)
{
	float fSource;
	if (!PyTuple_GetFloat(poArgs, 0, &fSource))
		return Py_BuildException();
	float fTarget;
	if (!PyTuple_GetFloat(poArgs, 1, &fTarget))
		return Py_BuildException();

	return Py_BuildValue("i", GetRotatingDirection(fSource, fTarget));
}

PyObject* appGetDegreeDifference(PyObject* poSelf, PyObject* poArgs)
{
	float fSource;
	if (!PyTuple_GetFloat(poArgs, 0, &fSource))
		return Py_BuildException();
	float fTarget;
	if (!PyTuple_GetFloat(poArgs, 1, &fTarget))
		return Py_BuildException();

	return Py_BuildValue("f", GetDegreeDifference(fSource, fTarget));
}

PyObject* appSleep(PyObject* poSelf, PyObject* poArgs)
{
	int iTime;
	if (!PyTuple_GetInteger(poArgs, 0, &iTime))
		return Py_BuildException();

	Sleep(iTime);

	return Py_BuildNone();
}

PyObject* appSetDefaultFontName(PyObject* poSelf, PyObject* poArgs)
{
	char* szFontName;
	if (!PyTuple_GetString(poArgs, 0, &szFontName))
		return Py_BuildException();

	// DEFAULT_FONT
	DefaultFont_SetName(szFontName);
	// END_OF_DEFAULT_FONT

	return Py_BuildNone();
}

PyObject* appSetGuildSymbolPath(PyObject* poSelf, PyObject* poArgs)
{
	char* szPathName;
	if (!PyTuple_GetString(poArgs, 0, &szPathName))
		return Py_BuildException();

	SetGuildSymbolPath(szPathName);

	return Py_BuildNone();
}

PyObject* appEnableSpecialCameraMode(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().EnableSpecialCameraMode();
	return Py_BuildNone();
}

PyObject* appSetCameraSpeed(PyObject* poSelf, PyObject* poArgs)
{
	int iPercentage;
	if (!PyTuple_GetInteger(poArgs, 0, &iPercentage))
		return Py_BuildException();

	CPythonApplication::Instance().SetCameraSpeed(iPercentage);

	CCamera* pCamera = CCameraManager::Instance().GetCurrentCamera();
	if (pCamera)
		pCamera->SetResistance(float(iPercentage) / 100.0f);
	return Py_BuildNone();
}

PyObject* appIsFileExist(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	return Py_BuildValue("i", -1 != _access(szFileName, 0));
}

PyObject* appSetCameraSetting(PyObject* poSelf, PyObject* poArgs)
{
	int ix;
	if (!PyTuple_GetInteger(poArgs, 0, &ix))
		return Py_BuildException();
	int iy;
	if (!PyTuple_GetInteger(poArgs, 1, &iy))
		return Py_BuildException();
	int iz;
	if (!PyTuple_GetInteger(poArgs, 2, &iz))
		return Py_BuildException();

	int iZoom;
	if (!PyTuple_GetInteger(poArgs, 3, &iZoom))
		return Py_BuildException();
	int iRotation;
	if (!PyTuple_GetInteger(poArgs, 4, &iRotation))
		return Py_BuildException();
	int iPitch;
	if (!PyTuple_GetInteger(poArgs, 5, &iPitch))
		return Py_BuildException();

	CPythonApplication::SCameraSetting CameraSetting;
	ZeroMemory(&CameraSetting, sizeof(CameraSetting));
	CameraSetting.v3CenterPosition.x = float(ix);
	CameraSetting.v3CenterPosition.y = float(iy);
	CameraSetting.v3CenterPosition.z = float(iz);
	CameraSetting.fZoom = float(iZoom);
	CameraSetting.fRotation = float(iRotation);
	CameraSetting.fPitch = float(iPitch);
	CPythonApplication::Instance().SetEventCamera(CameraSetting);
	return Py_BuildNone();
}

PyObject* appSaveCameraSetting(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonApplication::Instance().SaveCameraSetting(szFileName);
	return Py_BuildNone();
}

PyObject* appLoadCameraSetting(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	bool bResult = CPythonApplication::Instance().LoadCameraSetting(szFileName);
	return Py_BuildValue("i", bResult);
}

PyObject* appSetDefaultCamera(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().SetDefaultCamera();
	return Py_BuildNone();
}

PyObject* appSetSightRange(PyObject* poSelf, PyObject* poArgs)
{
	int iRange;
	if (!PyTuple_GetInteger(poArgs, 0, &iRange))
		return Py_BuildException();

	CPythonApplication::Instance().SetForceSightRange(iRange);
	return Py_BuildNone();
}

extern int g_iAccumulationTime;

PyObject* apptestGetAccumulationTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", g_iAccumulationTime);
}

PyObject* apptestResetAccumulationTime(PyObject* poSelf, PyObject* poArgs)
{
	g_iAccumulationTime = 0;
	return Py_BuildNone();
}

PyObject* apptestSetSpecularColor(PyObject* poSelf, PyObject* poArgs)
{
	float fr;
	if (!PyTuple_GetFloat(poArgs, 0, &fr))
		return Py_BuildException();
	float fg;
	if (!PyTuple_GetFloat(poArgs, 1, &fg))
		return Py_BuildException();
	float fb;
	if (!PyTuple_GetFloat(poArgs, 2, &fb))
		return Py_BuildException();
	g_fSpecularColor = D3DXCOLOR(fr, fg, fb, 1.0f);
	return Py_BuildNone();
}

PyObject* appSetVisibleNotice(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();
	bVisibleNotice = iFlag;
	return Py_BuildNone();
}

PyObject* appIsVisibleNotice(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", bVisibleNotice);
}

PyObject* appEnableTestServerFlag(PyObject* poSelf, PyObject* poArgs)
{
	bTestServerFlag = TRUE;
	return Py_BuildNone();
}

PyObject* appIsEnableTestServerFlag(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", bTestServerFlag);
}

class CTextLineLoader
{
public:
	CTextLineLoader(const char* c_szFileName)
	{
		const VOID* pvData;
		CMappedFile kFile;
		if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
			return;

		m_kTextFileLoader.Bind(kFile.Size(), pvData);
	}

	DWORD GetLineCount()
	{
		return m_kTextFileLoader.GetLineCount();
	}

	const char* GetLine(DWORD dwIndex)
	{
		if (dwIndex >= GetLineCount())
			return "";

		return m_kTextFileLoader.GetLineString(dwIndex).c_str();
	}

protected:
	CMemoryTextFileLoader m_kTextFileLoader;
};

PyObject* appOpenTextFile(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CTextLineLoader* pTextLineLoader = new CTextLineLoader(szFileName);

	return Py_BuildValue("i", (int)pTextLineLoader);
}

PyObject* appCloseTextFile(PyObject* poSelf, PyObject* poArgs)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, 0, &iHandle))
		return Py_BuildException();

	CTextLineLoader* pTextFileLoader = (CTextLineLoader*)iHandle;
	delete pTextFileLoader;

	return Py_BuildNone();
}

PyObject* appGetTextFileLineCount(PyObject* poSelf, PyObject* poArgs)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, 0, &iHandle))
		return Py_BuildException();

	CTextLineLoader* pTextFileLoader = (CTextLineLoader*)iHandle;
	return Py_BuildValue("i", pTextFileLoader->GetLineCount());
}

PyObject* appGetTextFileLine(PyObject* poSelf, PyObject* poArgs)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, 0, &iHandle))
		return Py_BuildException();
	int iLineIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iLineIndex))
		return Py_BuildException();

	CTextLineLoader* pTextFileLoader = (CTextLineLoader*)iHandle;
	return Py_BuildValue("s", pTextFileLoader->GetLine(iLineIndex));
}

PyObject* appSetGuildMarkPath(PyObject* poSelf, PyObject* poArgs)
{
	char* path;
	if (!PyTuple_GetString(poArgs, 0, &path))
		return Py_BuildException();

	char newPath[256];
	char* ext = strstr(path, ".tga");

	if (ext)
	{
		int extPos = ext - path;
		strncpy(newPath, path, extPos);
		newPath[extPos] = '\0';
	}
	else
		strncpy(newPath, path, sizeof(newPath) - 1);

	CGuildMarkManager::Instance().SetMarkPathPrefix(newPath);
	return Py_BuildNone();
}

PyObject* appIsDevStage(PyObject* poSelf, PyObject* poArgs)
{
	int nIsDevelopmentStage = 0;
#if defined(LOCALE_SERVICE_STAGE_DEVELOPMENT)
	nIsDevelopmentStage = 1;
#endif
	return Py_BuildValue("i", nIsDevelopmentStage);
}

PyObject* appIsTestStage(PyObject* poSelf, PyObject* poArgs)
{
	int nIsTestStage = 0;
#if defined(LOCALE_SERVICE_STAGE_TEST)
	nIsTestStage = 1;
#endif
	return Py_BuildValue("i", nIsTestStage);
}

PyObject* appIsLiveStage(PyObject* poSelf, PyObject* poArgs)
{
	int nIsLiveStage = 0;
#if !defined(LOCALE_SERVICE_STAGE_TEST) && !defined(LOCALE_SERVICE_STAGE_DEVELOPMENT)
	nIsLiveStage = 1;
#endif
	return Py_BuildValue("i", nIsLiveStage);
}

PyObject* appLogoOpen(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	int nIsSuccess = 1; //CPythonApplication::Instance().OnLogoOpen(szName);
	CMovieMan::Instance().PlayLogo(szName);

	return Py_BuildValue("i", nIsSuccess);
}

PyObject* appLogoUpdate(PyObject* poSelf, PyObject* poArgs)
{
	int nIsRun = 0; //CPythonApplication::Instance().OnLogoUpdate();
	return Py_BuildValue("i", nIsRun);
}

PyObject* appLogoRender(PyObject* poSelf, PyObject* poArgs)
{
	//CPythonApplication::Instance().OnLogoRender();
	return Py_BuildNone();
}

PyObject* appLogoClose(PyObject* poSelf, PyObject* poArgs)
{
	//CPythonApplication::Instance().OnLogoClose();
	return Py_BuildNone();
}

PyObject* appFlashApplication(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().FlashApplication();
	return Py_BuildNone();
}


#if defined(ENABLE_LOCALE_CLIENT)
PyObject* appReloadLocaConfig(PyObject* poSelf, PyObject* poArgs)
{
	if (CPythonApplication::Instance().GetReloadLocale())
	{
		LocaleService_LoadLocaConfig();
		SetDefaultCodePage(LocaleService_GetCodePage());
	}
	return Py_BuildNone();
}

PyObject* appSetReloadLocale(PyObject* poSelf, PyObject* poArgs)
{
	bool bReload;
	if (!PyTuple_GetBoolean(poArgs, 0, &bReload))
		return Py_BuildException();

	CPythonApplication::Instance().SetReloadLocale(bReload);
	return Py_BuildNone();
}

PyObject* appGetReloadLocale(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonApplication::Instance().GetReloadLocale());
}
#endif

PyObject* appGetName(PyObject* poSelf, PyObject* poArgs)
{
	TCHAR szAppName[MAX_PATH];
	GetModuleFileName(NULL, szAppName, MAX_PATH);
	return Py_BuildValue("s", szAppName);
}

PyObject* appGetCurrentTime(PyObject* poSelf, PyObject* poArgs)
{
	time_t t = time(nullptr);
	char szBuffer[100];
	if (strftime(szBuffer, sizeof(szBuffer), "%Y.%m.%d  %T", localtime(&t)))
		return Py_BuildValue("s", szBuffer);

	return Py_BuildValue("s", "");
}

PyObject* appGetTimeZone(PyObject* poSelf, PyObject* poArgs)
{
	//char szCodePage[100];
	//_snprintf(szCodePage, sizeof(szCodePage), ".%d", LocaleService_GetCodePage());

	//std::locale::global(std::locale(szCodePage));
	time_t t = time(nullptr);
	char szBuffer[100];
	if (strftime(szBuffer, sizeof(szBuffer), "%Z", localtime(&t)))
		return Py_BuildValue("s", szBuffer);

	return Py_BuildValue("s", "");
}

PyObject* appGetLocalTime(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwTime;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwTime))
		return Py_BuildException();

	const time_t time = static_cast<time_t>(dwTime);
	struct tm* t = localtime(&time);
	if (!t)
		return Py_BuildNone();

	PyObject* dict = PyTuple_New(6);
	PyTuple_SetItem(dict, 0, PyInt_FromLong(t->tm_year + 1900));
	PyTuple_SetItem(dict, 1, PyInt_FromLong(t->tm_mon + 1));
	PyTuple_SetItem(dict, 2, PyInt_FromLong(t->tm_mday));
	PyTuple_SetItem(dict, 3, PyInt_FromLong(t->tm_hour));
	PyTuple_SetItem(dict, 4, PyInt_FromLong(t->tm_min));
	PyTuple_SetItem(dict, 5, PyInt_FromLong(t->tm_sec));
	return dict;
}

PyObject* appGetTimeString(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwTime;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwTime))
		return Py_BuildException();

	const time_t time = static_cast<time_t>(dwTime);
	struct tm* t = localtime(&time);
	if (!t)
		return Py_BuildValue("s", "ERROR");

	char buf[256];
	snprintf(buf, sizeof(buf), "%04d/%02d/%02d %02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	return Py_BuildValue("s", buf);
}

#include <shellapi.h>
PyObject* appShellExecute(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	ShellExecute(NULL, "open", szName, NULL, NULL, SW_NORMAL);
	return Py_BuildNone();
}

#if defined(ENABLE_QUEST_RENEWAL)
PyObject* appGetTextLength(PyObject* poSelf, PyObject* poArgs)
{
	int iLength = 0;

	char* szString;
	if (PyTuple_GetString(poArgs, 0, &szString))
		iLength = MultiByteToWideChar(GetDefaultCodePage(), 0, szString, -1, nullptr, 0);

	return Py_BuildValue("i", iLength);
}

PyObject* appGetTextWidth(PyObject* poSelf, PyObject* poArgs)
{
	char* szString;
	if (!PyTuple_GetString(poArgs, 0, &szString))
		return Py_BuildValue("i", 0);

	const DWORD dwDefaultCodePage = GetDefaultCodePage();
	int iLength = MultiByteToWideChar(dwDefaultCodePage, 0, szString, -1, nullptr, 0);

	wchar_t* wText = (wchar_t*)_alloca(2 * iLength);

	iLength = MultiByteToWideChar(dwDefaultCodePage, 0, szString, -1, wText, iLength);

	CGraphicText* pkDefaultFont = static_cast<CGraphicText*>(DefaultFont_GetResource());
	if (!pkDefaultFont)
		return Py_BuildValue("i", 0);

	CGraphicFontTexture* pFont = pkDefaultFont->GetFontTexturePointer();
	if (!pFont)
		return Py_BuildValue("i", 0);

	float fWidth = 0.0f;
	for (int i = 0; i < iLength; ++i)
	{
		if (!wText[i])
			continue;

		CGraphicFontTexture::TCharacterInfomation* pCharacterInfomation = pFont->GetCharacterInfomation(dwDefaultCodePage, wText[i]);
		if (!pCharacterInfomation)
			continue;

		fWidth += pCharacterInfomation->advance;
	}

	return Py_BuildValue("i", (int)ceilf(fWidth));
}
#endif

#if defined(ENABLE_MONSTER_CARD)
PyObject* appIllustratedCreate(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().IllustratedCreate();
	return Py_BuildNone();
}
#endif

#if defined(ENABLE_MYSHOP_DECO)
PyObject* appMyShopDecoBGCreate(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().MyShopDecoBGCreate();
	return Py_BuildNone();
}
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
PyObject* appYutnoriCreate(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().YutnoriCreate();
	return Py_BuildNone();
}
#endif

PyObject* appSetcmrZ(PyObject* poSelf, PyObject* poArgs)
{
	float fDistance;
	if (!PyTuple_GetFloat(poArgs, 0, &fDistance))
		return Py_BuildException();

	CCamera::IncreaseCameraMaxDistance(fDistance);

	CInstanceBase* pkInstMain = CPythonCharacterManager::Instance().GetMainInstancePtr();
	if (pkInstMain)
	{
		pkInstMain->GetGraphicThingInstanceRef().EnableSkipCollision();
		pkInstMain->EnableSkipHorseCollision();
	}

	return Py_BuildNone();
}

void initapp()
{
	static PyMethodDef s_methods[] =
	{
		{ "IsDevStage", appIsDevStage, METH_VARARGS },
		{ "IsTestStage", appIsTestStage, METH_VARARGS },
		{ "IsLiveStage", appIsLiveStage, METH_VARARGS },

		// TEXTTAIL_LIVINGTIME_CONTROL
		{ "SetTextTailLivingTime", appSetTextTailLivingTime, METH_VARARGS },
		// END_OF_TEXTTAIL_LIVINGTIME_CONTROL

		{ "EnablePerformanceTime", appEnablePerformanceTime, METH_VARARGS },
		{ "SetHairColorEnable", appSetHairColorEnable, METH_VARARGS },

		{ "SetArmorSpecularEnable", appSetArmorSpecularEnable, METH_VARARGS },
		{ "SetWeaponSpecularEnable", appSetWeaponSpecularEnable, METH_VARARGS },
		{ "SetSkillEffectUpgradeEnable", appSetSkillEffectUpgradeEnable, METH_VARARGS },
		{ "SetTwoHandedWeaponAttSpeedDecreaseValue", SetTwoHandedWeaponAttSpeedDecreaseValue, METH_VARARGS },
		{ "SetRideHorseEnable", appSetRideHorseEnable, METH_VARARGS },

		{ "SetCameraMaxDistance", appSetCameraMaxDistance, METH_VARARGS },
		{ "SetMinFog", appSetMinFog, METH_VARARGS },
		{ "SetFrameSkip", appSetFrameSkip, METH_VARARGS },
		{ "GetImageInfo", appGetImageInfo, METH_VARARGS },
		{ "GetInfo", appGetInfo, METH_VARARGS },
		{ "UpdateGame", appUpdateGame, METH_VARARGS },
		{ "RenderGame", appRenderGame, METH_VARARGS },
		{ "Loop", appLoop, METH_VARARGS },
		{ "Create", appCreate, METH_VARARGS },
		{ "Process", appProcess, METH_VARARGS },
		{ "Exit", appExit, METH_VARARGS },
		{ "Abort", appAbort, METH_VARARGS },
		{ "SetMouseHandler", appSetMouseHandler, METH_VARARGS },
		{ "IsExistFile", appIsExistFile, METH_VARARGS },
		{ "GetFileList", appGetFileList, METH_VARARGS },
		{ "ShellExecute", appShellExecute, METH_VARARGS },

		{ "SetCamera", appSetCamera, METH_VARARGS },
		{ "GetCamera", appGetCamera, METH_VARARGS },
		{ "GetCameraPitch", appGetCameraPitch, METH_VARARGS },
		{ "GetCameraRotation", appGetCameraRotation, METH_VARARGS },
		{ "GetTime", appGetTime, METH_VARARGS },
		{ "GetGlobalTime", appGetGlobalTime, METH_VARARGS },
		{ "GetGlobalTimeStamp", appGetGlobalTimeStamp, METH_VARARGS },
		{ "GetUpdateFPS", appGetUpdateFPS, METH_VARARGS },
		{ "GetRenderFPS", appGetRenderFPS, METH_VARARGS },

		{ "RotateCamera", appRotateCamera, METH_VARARGS },
		{ "PitchCamera", appPitchCamera, METH_VARARGS },
		{ "ZoomCamera", appZoomCamera, METH_VARARGS },
		{ "MovieRotateCamera", appMovieRotateCamera, METH_VARARGS },
		{ "MoviePitchCamera", appMoviePitchCamera, METH_VARARGS },
		{ "MovieZoomCamera", appMovieZoomCamera, METH_VARARGS },
		{ "MovieResetCamera", appMovieResetCamera, METH_VARARGS },

		{ "GetAvailableTextureMemory", appGetAvaiableTextureMememory, METH_VARARGS },
		{ "GetRenderTime", appGetRenderTime, METH_VARARGS },
		{ "GetUpdateTime", appGetUpdateTime, METH_VARARGS },
		{ "GetLoad", appGetLoad, METH_VARARGS },
		{ "GetFaceSpeed", appGetFaceSpeed, METH_VARARGS },
		{ "GetFaceCount", appGetFaceCount, METH_VARARGS },
		{ "SetFPS", appSetFPS, METH_VARARGS },
		{ "SetGlobalCenterPosition", appSetGlobalCenterPosition, METH_VARARGS },
		{ "SetCenterPosition", appSetCenterPosition, METH_VARARGS },
		{ "GetCursorPosition", appGetCursorPosition, METH_VARARGS },

		{ "GetRandom", appGetRandom, METH_VARARGS },
		{ "RunPythonFile", appRunPythonFile, METH_VARARGS },
		{ "IsWebPageMode", appIsWebPageMode, METH_VARARGS },
		{ "ShowWebPage", appShowWebPage, METH_VARARGS },
		{ "MoveWebPage", appMoveWebPage, METH_VARARGS },
		{ "HideWebPage", appHideWebPage, METH_VARARGS },
		{ "IsPressed", appIsPressed, METH_VARARGS },
		{ "SetCursor", appSetCursor, METH_VARARGS },
		{ "GetCursor", appGetCursor, METH_VARARGS },
		{ "ShowCursor", appShowCursor, METH_VARARGS },
		{ "HideCursor", appHideCursor, METH_VARARGS },
		{ "IsShowCursor", appIsShowCursor, METH_VARARGS },
		{ "IsLiarCursorOn", appIsLiarCursorOn, METH_VARARGS },
		{ "SetSoftwareCursor", appSetSoftwareCursor, METH_VARARGS },
		{ "SetHardwareCursor", appSetHardwareCursor, METH_VARARGS },

		{ "SetConnectData", appSetConnectData, METH_VARARGS },
		{ "GetConnectData", appGetConnectData, METH_VARARGS },

		{ "GetRotatingDirection", appGetRotatingDirection, METH_VARARGS },
		{ "GetDegreeDifference", appGetDegreeDifference, METH_VARARGS },
		{ "Sleep", appSleep, METH_VARARGS },
		{ "SetDefaultFontName", appSetDefaultFontName, METH_VARARGS },
		{ "SetGuildSymbolPath", appSetGuildSymbolPath, METH_VARARGS },

		{ "EnableSpecialCameraMode", appEnableSpecialCameraMode, METH_VARARGS },
		{ "SetCameraSpeed", appSetCameraSpeed, METH_VARARGS },

		{ "SaveCameraSetting", appSaveCameraSetting, METH_VARARGS },
		{ "LoadCameraSetting", appLoadCameraSetting, METH_VARARGS },
		{ "SetDefaultCamera", appSetDefaultCamera, METH_VARARGS },
		{ "SetCameraSetting", appSetCameraSetting, METH_VARARGS },

		{ "SetSightRange", appSetSightRange, METH_VARARGS },

		{ "IsFileExist", appIsFileExist, METH_VARARGS },
		{ "OpenTextFile", appOpenTextFile, METH_VARARGS },
		{ "CloseTextFile", appCloseTextFile, METH_VARARGS },
		{ "GetTextFileLineCount", appGetTextFileLineCount, METH_VARARGS },
		{ "GetTextFileLine", appGetTextFileLine, METH_VARARGS },

		// LOCALE
		{ "GetLocaleServiceName", appGetLocaleServiceName, METH_VARARGS },
		{ "GetLocaleName", appGetLocaleName, METH_VARARGS },
		{ "GetLocalePath", appGetLocalePath, METH_VARARGS },
		{ "ForceSetLocale", appForceSetLocale, METH_VARARGS },
		// END_OF_LOCALE

		// CHEONMA
		{ "LoadLocaleAddr", appLoadLocaleAddr, METH_VARARGS },
		{ "LoadLocaleData", appLoadLocaleData, METH_VARARGS },
		{ "SetCHEONMA", appSetCHEONMA, METH_VARARGS },
		{ "IsCHEONMA", appIsCHEONMA, METH_VARARGS },
		// END_OF_CHEONMA

		{ "GetDefaultCodePage", appGetDefaultCodePage, METH_VARARGS },
		{ "SetControlFP", appSetControlFP, METH_VARARGS },
		{ "SetSpecularSpeed", appSetSpecularSpeed, METH_VARARGS },

		{ "testGetAccumulationTime", apptestGetAccumulationTime, METH_VARARGS },
		{ "testResetAccumulationTime", apptestResetAccumulationTime, METH_VARARGS },
		{ "testSetSpecularColor", apptestSetSpecularColor, METH_VARARGS },

		{ "SetVisibleNotice", appSetVisibleNotice, METH_VARARGS },
		{ "IsVisibleNotice", appIsVisibleNotice, METH_VARARGS },
		{ "EnableTestServerFlag", appEnableTestServerFlag, METH_VARARGS },
		{ "IsEnableTestServerFlag", appIsEnableTestServerFlag, METH_VARARGS },

		{ "SetGuildMarkPath", appSetGuildMarkPath, METH_VARARGS },

		{ "OnLogoUpdate", appLogoUpdate, METH_VARARGS },
		{ "OnLogoRender", appLogoRender, METH_VARARGS },
		{ "OnLogoOpen", appLogoOpen, METH_VARARGS },
		{ "OnLogoClose", appLogoClose, METH_VARARGS },

		{ "FlashApplication", appFlashApplication, METH_VARARGS },

#if defined(ENABLE_LOCALE_CLIENT)
		{ "ReloadLocaConfig", appReloadLocaConfig, METH_VARARGS },
		{ "SetReloadLocale", appSetReloadLocale, METH_VARARGS },
		{ "GetReloadLocale", appGetReloadLocale, METH_VARARGS },
#endif

		{ "GetName", appGetName, METH_VARARGS },
		{ "GetCurrentTime", appGetCurrentTime, METH_VARARGS },
		{ "GetTimeZone", appGetTimeZone, METH_VARARGS },
		{ "GetLocalTime", appGetLocalTime, METH_VARARGS },
		{ "GetTimeString", appGetTimeString, METH_VARARGS },

#if defined(ENABLE_QUEST_RENEWAL)
		{ "GetTextLength", appGetTextLength, METH_VARARGS },
		{ "GetTextWidth", appGetTextWidth, METH_VARARGS },
#endif

#if defined(ENABLE_MONSTER_CARD)
		{ "IllustratedCreate", appIllustratedCreate, METH_VARARGS },
#endif
#if defined(ENABLE_MYSHOP_DECO)
		{ "MyShopDecoBGCreate", appMyShopDecoBGCreate, METH_VARARGS },
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		{ "YutnoriCreate", appYutnoriCreate, METH_VARARGS },
#endif

		{ "SetcmrZ", appSetcmrZ, METH_VARARGS },

		{ NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("app", s_methods);

	PyModule_AddIntConstant(poModule, "INFO_ITEM", CPythonApplication::INFO_ITEM);
	PyModule_AddIntConstant(poModule, "INFO_ACTOR", CPythonApplication::INFO_ACTOR);
	PyModule_AddIntConstant(poModule, "INFO_EFFECT", CPythonApplication::INFO_EFFECT);
	PyModule_AddIntConstant(poModule, "INFO_TEXTTAIL", CPythonApplication::INFO_TEXTTAIL);

	PyModule_AddIntConstant(poModule, "DEGREE_DIRECTION_SAME", DEGREE_DIRECTION_SAME);
	PyModule_AddIntConstant(poModule, "DEGREE_DIRECTION_RIGHT", DEGREE_DIRECTION_RIGHT);
	PyModule_AddIntConstant(poModule, "DEGREE_DIRECTION_LEFT", DEGREE_DIRECTION_LEFT);

	PyModule_AddIntConstant(poModule, "VK_LEFT", VK_LEFT);
	PyModule_AddIntConstant(poModule, "VK_RIGHT", VK_RIGHT);
	PyModule_AddIntConstant(poModule, "VK_UP", VK_UP);
	PyModule_AddIntConstant(poModule, "VK_DOWN", VK_DOWN);
	PyModule_AddIntConstant(poModule, "VK_HOME", VK_HOME);
	PyModule_AddIntConstant(poModule, "VK_END", VK_END);
	PyModule_AddIntConstant(poModule, "VK_DELETE", VK_DELETE);

	PyModule_AddIntConstant(poModule, "DIK_ESCAPE", DIK_ESCAPE);
	PyModule_AddIntConstant(poModule, "DIK_ESC", DIK_ESCAPE); // 편의를 위해
	PyModule_AddIntConstant(poModule, "DIK_1", DIK_1);
	PyModule_AddIntConstant(poModule, "DIK_2", DIK_2);
	PyModule_AddIntConstant(poModule, "DIK_3", DIK_3);
	PyModule_AddIntConstant(poModule, "DIK_4", DIK_4);
	PyModule_AddIntConstant(poModule, "DIK_5", DIK_5);
	PyModule_AddIntConstant(poModule, "DIK_6", DIK_6);
	PyModule_AddIntConstant(poModule, "DIK_7", DIK_7);
	PyModule_AddIntConstant(poModule, "DIK_8", DIK_8);
	PyModule_AddIntConstant(poModule, "DIK_9", DIK_9);
	PyModule_AddIntConstant(poModule, "DIK_0", DIK_0);
	PyModule_AddIntConstant(poModule, "DIK_MINUS", DIK_MINUS); /* - on main keyboard */
	PyModule_AddIntConstant(poModule, "DIK_EQUALS", DIK_EQUALS);
	PyModule_AddIntConstant(poModule, "DIK_BACK", DIK_BACK); /* backspace */
	PyModule_AddIntConstant(poModule, "DIK_TAB", DIK_TAB);
	PyModule_AddIntConstant(poModule, "DIK_Q", DIK_Q);
	PyModule_AddIntConstant(poModule, "DIK_W", DIK_W);
	PyModule_AddIntConstant(poModule, "DIK_E", DIK_E);
	PyModule_AddIntConstant(poModule, "DIK_R", DIK_R);
	PyModule_AddIntConstant(poModule, "DIK_T", DIK_T);
	PyModule_AddIntConstant(poModule, "DIK_Y", DIK_Y);
	PyModule_AddIntConstant(poModule, "DIK_U", DIK_U);
	PyModule_AddIntConstant(poModule, "DIK_I", DIK_I);
	PyModule_AddIntConstant(poModule, "DIK_O", DIK_O);
	PyModule_AddIntConstant(poModule, "DIK_P", DIK_P);
	PyModule_AddIntConstant(poModule, "DIK_LBRACKET", DIK_LBRACKET);
	PyModule_AddIntConstant(poModule, "DIK_RBRACKET", DIK_RBRACKET);
	PyModule_AddIntConstant(poModule, "DIK_RETURN", DIK_RETURN); /* Enter on main keyboard */
	PyModule_AddIntConstant(poModule, "DIK_LCONTROL", DIK_LCONTROL);
	PyModule_AddIntConstant(poModule, "DIK_A", DIK_A);
	PyModule_AddIntConstant(poModule, "DIK_S", DIK_S);
	PyModule_AddIntConstant(poModule, "DIK_D", DIK_D);
	PyModule_AddIntConstant(poModule, "DIK_F", DIK_F);
	PyModule_AddIntConstant(poModule, "DIK_G", DIK_G);
	PyModule_AddIntConstant(poModule, "DIK_H", DIK_H);
	PyModule_AddIntConstant(poModule, "DIK_J", DIK_J);
	PyModule_AddIntConstant(poModule, "DIK_K", DIK_K);
	PyModule_AddIntConstant(poModule, "DIK_L", DIK_L);
	PyModule_AddIntConstant(poModule, "DIK_SEMICOLON", DIK_SEMICOLON);
	PyModule_AddIntConstant(poModule, "DIK_APOSTROPHE", DIK_APOSTROPHE);
	PyModule_AddIntConstant(poModule, "DIK_GRAVE", DIK_GRAVE); /* accent grave */
	PyModule_AddIntConstant(poModule, "DIK_LSHIFT", DIK_LSHIFT);
	PyModule_AddIntConstant(poModule, "DIK_BACKSLASH", DIK_BACKSLASH);
	PyModule_AddIntConstant(poModule, "DIK_Z", DIK_Z);
	PyModule_AddIntConstant(poModule, "DIK_X", DIK_X);
	PyModule_AddIntConstant(poModule, "DIK_C", DIK_C);
	PyModule_AddIntConstant(poModule, "DIK_V", DIK_V);
	PyModule_AddIntConstant(poModule, "DIK_B", DIK_B);
	PyModule_AddIntConstant(poModule, "DIK_N", DIK_N);
	PyModule_AddIntConstant(poModule, "DIK_M", DIK_M);
	PyModule_AddIntConstant(poModule, "DIK_COMMA", DIK_COMMA);
	PyModule_AddIntConstant(poModule, "DIK_PERIOD", DIK_PERIOD); /* . on main keyboard */
	PyModule_AddIntConstant(poModule, "DIK_SLASH", DIK_SLASH); /* / on main keyboard */
	PyModule_AddIntConstant(poModule, "DIK_RSHIFT", DIK_RSHIFT);
	PyModule_AddIntConstant(poModule, "DIK_MULTIPLY", DIK_MULTIPLY); /* * on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_LALT", DIK_LMENU); /* left Alt */
	PyModule_AddIntConstant(poModule, "DIK_SPACE", DIK_SPACE);
	PyModule_AddIntConstant(poModule, "DIK_CAPITAL", DIK_CAPITAL);
	PyModule_AddIntConstant(poModule, "DIK_F1", DIK_F1);
	PyModule_AddIntConstant(poModule, "DIK_F2", DIK_F2);
	PyModule_AddIntConstant(poModule, "DIK_F3", DIK_F3);
	PyModule_AddIntConstant(poModule, "DIK_F4", DIK_F4);
	PyModule_AddIntConstant(poModule, "DIK_F5", DIK_F5);
	PyModule_AddIntConstant(poModule, "DIK_F6", DIK_F6);
	PyModule_AddIntConstant(poModule, "DIK_F7", DIK_F7);
	PyModule_AddIntConstant(poModule, "DIK_F8", DIK_F8);
	PyModule_AddIntConstant(poModule, "DIK_F9", DIK_F9);
	PyModule_AddIntConstant(poModule, "DIK_F10", DIK_F10);
	PyModule_AddIntConstant(poModule, "DIK_NUMLOCK", DIK_NUMLOCK);
	PyModule_AddIntConstant(poModule, "DIK_SCROLL", DIK_SCROLL); /* Scroll Lock */
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD7", DIK_NUMPAD7);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD8", DIK_NUMPAD8);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD9", DIK_NUMPAD9);
	PyModule_AddIntConstant(poModule, "DIK_SUBTRACT", DIK_SUBTRACT); /* - on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD4", DIK_NUMPAD4);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD5", DIK_NUMPAD5);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD6", DIK_NUMPAD6);
	PyModule_AddIntConstant(poModule, "DIK_ADD", DIK_ADD); /* + on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD1", DIK_NUMPAD1);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD2", DIK_NUMPAD2);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD3", DIK_NUMPAD3);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD0", DIK_NUMPAD0);
	PyModule_AddIntConstant(poModule, "DIK_DECIMAL", DIK_DECIMAL); /* . on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_F11", DIK_F11);
	PyModule_AddIntConstant(poModule, "DIK_F12", DIK_F12);
	PyModule_AddIntConstant(poModule, "DIK_NEXTTRACK", DIK_NEXTTRACK); /* Next Track */
	PyModule_AddIntConstant(poModule, "DIK_NUMPADENTER", DIK_NUMPADENTER); /* Enter on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_RCONTROL", DIK_RCONTROL);
	PyModule_AddIntConstant(poModule, "DIK_MUTE", DIK_MUTE); /* Mute */
	PyModule_AddIntConstant(poModule, "DIK_CALCULATOR", DIK_CALCULATOR); /* Calculator */
	PyModule_AddIntConstant(poModule, "DIK_PLAYPAUSE", DIK_PLAYPAUSE); /* Play / Pause */
	PyModule_AddIntConstant(poModule, "DIK_MEDIASTOP", DIK_MEDIASTOP); /* Media Stop */
	PyModule_AddIntConstant(poModule, "DIK_VOLUMEDOWN", DIK_VOLUMEDOWN); /* Volume - */
	PyModule_AddIntConstant(poModule, "DIK_VOLUMEUP", DIK_VOLUMEUP); /* Volume + */
	PyModule_AddIntConstant(poModule, "DIK_WEBHOME", DIK_WEBHOME); /* Web home */
	PyModule_AddIntConstant(poModule, "DIK_NUMPADCOMMA", DIK_NUMPADCOMMA); /* , on numeric keypad (NEC PC98) */
	PyModule_AddIntConstant(poModule, "DIK_DIVIDE", DIK_DIVIDE); /* / on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_SYSRQ", DIK_SYSRQ);
	PyModule_AddIntConstant(poModule, "DIK_RALT", DIK_RMENU); /* right Alt */
	PyModule_AddIntConstant(poModule, "DIK_PAUSE", DIK_PAUSE); /* Pause */
	PyModule_AddIntConstant(poModule, "DIK_HOME", DIK_HOME); /* Home on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_UP", DIK_UP); /* UpArrow on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_PGUP", DIK_PRIOR); /* PgUp on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_LEFT", DIK_LEFT); /* LeftArrow on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_RIGHT", DIK_RIGHT); /* RightArrow on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_END", DIK_END); /* End on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_DOWN", DIK_DOWN); /* DownArrow on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_PGDN", DIK_NEXT); /* PgDn on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_INSERT", DIK_INSERT); /* Insert on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_DELETE", DIK_DELETE); /* Delete on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_LWIN", DIK_LWIN); /* Left Windows key */
	PyModule_AddIntConstant(poModule, "DIK_RWIN", DIK_RWIN); /* Right Windows key */
	PyModule_AddIntConstant(poModule, "DIK_APPS", DIK_APPS); /* AppMenu key */

	// Cursor
	PyModule_AddIntConstant(poModule, "NORMAL", CPythonApplication::CURSOR_SHAPE_NORMAL);
	PyModule_AddIntConstant(poModule, "ATTACK", CPythonApplication::CURSOR_SHAPE_ATTACK);
	PyModule_AddIntConstant(poModule, "TARGET", CPythonApplication::CURSOR_SHAPE_TARGET);
	PyModule_AddIntConstant(poModule, "TALK", CPythonApplication::CURSOR_SHAPE_TALK);
	PyModule_AddIntConstant(poModule, "CANT_GO", CPythonApplication::CURSOR_SHAPE_CANT_GO);
	PyModule_AddIntConstant(poModule, "PICK", CPythonApplication::CURSOR_SHAPE_PICK);

	PyModule_AddIntConstant(poModule, "DOOR", CPythonApplication::CURSOR_SHAPE_DOOR);
	PyModule_AddIntConstant(poModule, "CHAIR", CPythonApplication::CURSOR_SHAPE_CHAIR);
	PyModule_AddIntConstant(poModule, "MAGIC", CPythonApplication::CURSOR_SHAPE_MAGIC);
	PyModule_AddIntConstant(poModule, "BUY", CPythonApplication::CURSOR_SHAPE_BUY);
	PyModule_AddIntConstant(poModule, "SELL", CPythonApplication::CURSOR_SHAPE_SELL);

	PyModule_AddIntConstant(poModule, "CAMERA_ROTATE", CPythonApplication::CURSOR_SHAPE_CAMERA_ROTATE);
	PyModule_AddIntConstant(poModule, "HSIZE", CPythonApplication::CURSOR_SHAPE_HSIZE);
	PyModule_AddIntConstant(poModule, "VSIZE", CPythonApplication::CURSOR_SHAPE_VSIZE);
	PyModule_AddIntConstant(poModule, "HVSIZE", CPythonApplication::CURSOR_SHAPE_HVSIZE);
#if defined(ENABLE_FISHING_GAME)
	PyModule_AddIntConstant(poModule, "FISH", CPythonApplication::CURSOR_SHAPE_FISH);
#endif

	PyModule_AddIntConstant(poModule, "CAMERA_TO_POSITIVE", CPythonApplication::CAMERA_TO_POSITIVE);
	PyModule_AddIntConstant(poModule, "CAMERA_TO_NEGATIVE", CPythonApplication::CAMERA_TO_NEGITIVE);
	PyModule_AddIntConstant(poModule, "CAMERA_STOP", CPythonApplication::CAMERA_STOP);

#if defined(ENABLE_DELETE_FAILURE_TYPE)
	PyModule_AddIntConstant(poModule, "DELETE_FAILURE_NORMAL", CPythonApplication::DELETE_FAILURE_NORMAL);
	PyModule_AddIntConstant(poModule, "DELETE_FAILURE_HAVE_SEALED_ITEM", CPythonApplication::DELETE_FAILURE_HAVE_SEALED_ITEM);
	PyModule_AddIntConstant(poModule, "DELETE_FAILURE_PRIVATE_CODE_ERROR", CPythonApplication::DELETE_FAILURE_PRIVATE_CODE_ERROR);
	PyModule_AddIntConstant(poModule, "DELETE_FAILURE_LIMITE_LEVEL_HIGHER", CPythonApplication::DELETE_FAILURE_LIMITE_LEVEL_HIGHER);
	PyModule_AddIntConstant(poModule, "DELETE_FAILURE_LIMITE_LEVEL_LOWER", CPythonApplication::DELETE_FAILURE_LIMITE_LEVEL_LOWER);
	PyModule_AddIntConstant(poModule, "DELETE_FAILURE_REMAIN_TIME", CPythonApplication::DELETE_FAILURE_REMAIN_TIME);
	PyModule_AddIntConstant(poModule, "DELETE_FAILURE_GUILD_MEMBER", CPythonApplication::DELETE_FAILURE_GUILD_MEMBER);
	PyModule_AddIntConstant(poModule, "DELETE_FAILURE_MARRIAGE", CPythonApplication::DELETE_FAILURE_MARRIAGE);
	PyModule_AddIntConstant(poModule, "DELETE_FAILURE_LAST_CHAR_SAFEBOX", CPythonApplication::DELETE_FAILURE_LAST_CHAR_SAFEBOX);
	PyModule_AddIntConstant(poModule, "DELETE_FAILURE_ATTR67", CPythonApplication::DELETE_FAILURE_ATTR67);
	PyModule_AddIntConstant(poModule, "DELETE_FAILURE_PREMIUM_PRIVATE_SHOP", CPythonApplication::DELETE_FAILURE_PREMIUM_PRIVATE_SHOP);
#endif

#if defined(ENABLE_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_COSTUME_SYSTEM", 0);
#endif

#if defined(ENABLE_ENERGY_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_ENERGY_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ENERGY_SYSTEM", 0);
#endif

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_DRAGON_SOUL_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DRAGON_SOUL_SYSTEM", 0);
#endif

#if defined(ENABLE_NEW_EQUIPMENT_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_EQUIPMENT_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_EQUIPMENT_SYSTEM", 0);
#endif

#if defined(ENABLE_SERVER_SELECT_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_SERVER_SELECT_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SERVER_SELECT_RENEWAL", 0);
#endif

#if defined(ENABLE_CHANNEL_LIST)
	PyModule_AddIntConstant(poModule, "ENABLE_CHANNEL_LIST", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CHANNEL_LIST", 0);
#endif

#if defined(LOGIN_COUNT_DOWN_UI_MODIFY)
	PyModule_AddIntConstant(poModule, "LOGIN_COUNT_DOWN_UI_MODIFY", 1);
#else
	PyModule_AddIntConstant(poModule, "LOGIN_COUNT_DOWN_UI_MODIFY", 0);
#endif

#if defined(DISABLE_WOLFMAN_CREATION)
	PyModule_AddIntConstant(poModule, "DISABLE_WOLFMAN_CREATION", 1);
#else
	PyModule_AddIntConstant(poModule, "DISABLE_WOLFMAN_CREATION", 0);
#endif

#if defined(ENABLE_PLAYER_PER_ACCOUNT5)
	PyModule_AddIntConstant(poModule, "ENABLE_PLAYER_PER_ACCOUNT5", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PLAYER_PER_ACCOUNT5", 0);
#endif

#if defined(ENABLE_678TH_SKILL)
	PyModule_AddIntConstant(poModule, "ENABLE_678TH_SKILL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_678TH_SKILL", 0);
#endif

#if defined(ENABLE_SELECT_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_SELECT_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SELECT_RENEWAL", 0);
#endif

#if defined(ENABLE_CREATE_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_CREATE_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CREATE_RENEWAL", 0);
#endif

#if defined(ENABLE_CHEQUE_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_CHEQUE_SYSTEM", 1);
	PyModule_AddIntConstant(poModule, "CHEQUE_INPUT_LIMIT", 4);
	PyModule_AddIntConstant(poModule, "EXCHANGE_CHEQUE_DIGIT_LIMIT", 3);
	PyModule_AddIntConstant(poModule, "EXCHANGE_CHEQUE_MAX", CPythonExchange::EXCHANGE_CHEQUE_MAX);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CHEQUE_SYSTEM", 0);
#endif

	PyModule_AddIntConstant(poModule, "YANG_PICKUP_INPUT_DIGIT_LIMIT", 8);
	PyModule_AddIntConstant(poModule, "EXCHANGE_YANG_MAX", CPythonExchange::EXCHANGE_YANG_MAX);

#if defined(ENABLE_ENVIRONMENT_EFFECT_OPTION)
	PyModule_AddIntConstant(poModule, "ENABLE_ENVIRONMENT_EFFECT_OPTION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ENVIRONMENT_EFFECT_OPTION", 0);
#endif

#if defined(ENABLE_NEW_USER_CARE)
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_USER_CARE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_USER_CARE", 0);
#endif

#if defined(WJ_SHOW_MOB_INFO)
	PyModule_AddIntConstant(poModule, "WJ_SHOW_MOB_INFO", 1);
#else
	PyModule_AddIntConstant(poModule, "WJ_SHOW_MOB_INFO", 0);
#endif

#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
	PyModule_AddIntConstant(poModule, "WJ_ENABLE_PICKUP_ITEM_EFFECT", 1);
#else
	PyModule_AddIntConstant(poModule, "WJ_ENABLE_PICKUP_ITEM_EFFECT", 0);
#endif

#if defined(WJ_ENABLE_TRADABLE_ICON)
	PyModule_AddIntConstant(poModule, "WJ_ENABLE_TRADABLE_ICON", 1);
#else
	PyModule_AddIntConstant(poModule, "WJ_ENABLE_TRADABLE_ICON", 0);
#endif

#if defined(ENABLE_MESSENGER_LOCK_FIX)
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_LOCK_FIX", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_LOCK_FIX", 0);
#endif

#if defined(ENABLE_MOUNT_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_MOUNT_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MOUNT_COSTUME_SYSTEM", 0);
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_ACCE_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ACCE_COSTUME_SYSTEM", 0);
#endif

#if defined(ENABLE_ACCE_SECOND_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_ACCE_SECOND_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ACCE_SECOND_COSTUME_SYSTEM", 0);
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_AURA_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_AURA_COSTUME_SYSTEM", 0);
#endif

#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_WEAPON_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_WEAPON_COSTUME_SYSTEM", 0);
#endif

#if defined(ENABLE_GEM_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_GEM_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GEM_SYSTEM", 0);
#endif

#if defined(ENABLE_SEND_TARGET_INFO)
	PyModule_AddIntConstant(poModule, "ENABLE_SEND_TARGET_INFO", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SEND_TARGET_INFO", 0);
#endif

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_EXTEND_INVEN_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EXTEND_INVEN_SYSTEM", 0);
#endif

#if defined(ENABLE_ELEMENT_ADD)
	PyModule_AddIntConstant(poModule, "ENABLE_ELEMENT_ADD", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ELEMENT_ADD", 0);
#endif

#if defined(ENABLE_NEW_DROP_DIALOG)
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_DROP_DIALOG", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_DROP_DIALOG", 0);
#endif

#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_LEADER_GRADE_NAME", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_LEADER_GRADE_NAME", 0);
#endif

#if defined(ENABLE_VIEW_TARGET_DECIMAL_HP)
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_DECIMAL_HP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_DECIMAL_HP", 0);
#endif

#if defined(ENABLE_VIEW_TARGET_PLAYER_HP)
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_PLAYER_HP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_PLAYER_HP", 0);
#endif

#if defined(ENABLE_MESSENGER_GM)
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_GM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_GM", 0);
#endif

#if defined(ENABLE_MESSENGER_DETAILS)
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_DETAILS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_DETAILS", 0);
#endif

#if defined(ENABLE_MESSENGER_BLOCK)
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_BLOCK", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_BLOCK", 0);
#endif

#if defined(ENABLE_DETAILS_UI)
	PyModule_AddIntConstant(poModule, "ENABLE_DETAILS_UI", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DETAILS_UI", 0);
#endif

#if defined(ENABLE_QUIVER_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_QUIVER_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_QUIVER_SYSTEM", 0);
#endif

#if defined(ENABLE_FOG_FIX)
	PyModule_AddIntConstant(poModule, "ENABLE_FOG_FIX", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_FOG_FIX", 0);
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_CHANGE_LOOK_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CHANGE_LOOK_SYSTEM", 0);
#endif

#if defined(ENABLE_SET_ITEM)
	PyModule_AddIntConstant(poModule, "ENABLE_SET_ITEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SET_ITEM", 0);
#endif

#if defined(ENABLE_EXPRESSING_EMOTION)
	PyModule_AddIntConstant(poModule, "ENABLE_EXPRESSING_EMOTION", 1);
	PyModule_AddIntConstant(poModule, "SPECIAL_ACTION_START_INDEX", EMOTION_PUSH_UP);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EXPRESSING_EMOTION", 0);
	PyModule_AddIntConstant(poModule, "SPECIAL_ACTION_START_INDEX", 0);
#endif

#if defined(ENABLE_REFINE_MSG_ADD)
	PyModule_AddIntConstant(poModule, "ENABLE_REFINE_MSG_ADD", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_REFINE_MSG_ADD", 0);
#endif

#if defined(ENABLE_GACHA)
	PyModule_AddIntConstant(poModule, "ENABLE_GACHA", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GACHA", 0);
#endif

#if defined(ENABLE_PENDANT)
	PyModule_AddIntConstant(poModule, "ENABLE_PENDANT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PENDANT", 0);
#endif

#if defined(ENABLE_GLOVE_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_GLOVE_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GLOVE_SYSTEM", 0);
#endif

#if defined(ENABLE_ADDITIONAL_EQUIPMENT_PAGE)
	PyModule_AddIntConstant(poModule, "ENABLE_ADDITIONAL_EQUIPMENT_PAGE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ADDITIONAL_EQUIPMENT_PAGE", 0);
#endif

#if defined(ENABLE_LOADING_TIP)
	PyModule_AddIntConstant(poModule, "ENABLE_LOADING_TIP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_LOADING_TIP", 0);
#endif

#if defined(ENABLE_SOULBIND_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_SOULBIND_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SOULBIND_SYSTEM", 0);
#endif

#if defined(ENABLE_DRAGON_SOUL_SEAL)
	PyModule_AddIntConstant(poModule, "ENABLE_DRAGON_SOUL_SEAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DRAGON_SOUL_SEAL", 0);
#endif

#if defined(ENABLE_UN_SEAL_SCROLL_PLUS)
	PyModule_AddIntConstant(poModule, "ENABLE_UN_SEAL_SCROLL_PLUS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_UN_SEAL_SCROLL_PLUS", 0);
#endif

#if defined(ENABLE_DISABLE_SOFTWARE_TILING)
	PyModule_AddIntConstant(poModule, "ENABLE_DISABLE_SOFTWARE_TILING", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DISABLE_SOFTWARE_TILING", 0);
#endif

#if defined(ENABLE_GRAPHIC_ON_OFF)
	PyModule_AddIntConstant(poModule, "ENABLE_GRAPHIC_ON_OFF", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GRAPHIC_ON_OFF", 0);
#endif

#if defined(ENABLE_QUEST_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_QUEST_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_QUEST_RENEWAL", 0);
#endif

#if defined(WJ_MULTI_TEXTLINE)
	PyModule_AddIntConstant(poModule, "WJ_MULTI_TEXTLINE", 1);
#else
	PyModule_AddIntConstant(poModule, "WJ_MULTI_TEXTLINE", 0);
#endif

#if defined(ENABLE_MOVE_CHANNEL)
	PyModule_AddIntConstant(poModule, "ENABLE_MOVE_CHANNEL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MOVE_CHANNEL", 0);
#endif

#if defined(ENABLE_SHOPEX_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_SHOPEX_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SHOPEX_RENEWAL", 0);
#endif

#if defined(DISABLE_INDEX_FILE)
	PyModule_AddIntConstant(poModule, "DISABLE_INDEX_FILE", 1);
#else
	PyModule_AddIntConstant(poModule, "DISABLE_INDEX_FILE", 0);
#endif

#if defined(ENABLE_SOUL_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_SOUL_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SOUL_SYSTEM", 0);
#endif

#if defined(ENABLE_MINI_GAME_CATCH_KING)
	PyModule_AddIntConstant(poModule, "ENABLE_MINI_GAME_CATCH_KING", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MINI_GAME_CATCH_KING", 0);
#endif

#if defined(ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL", 0);
#endif

#if defined(ENABLE_DS_GRADE_MYTH)
	PyModule_AddIntConstant(poModule, "ENABLE_DS_GRADE_MYTH", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DS_GRADE_MYTH", 0);
#endif

#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_PRIVATESHOP_SEARCH_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PRIVATESHOP_SEARCH_SYSTEM", 0);
#endif

#if defined(ENABLE_ITEM_SOCKET6)
	PyModule_AddIntConstant(poModule, "ENABLE_ITEM_SOCKET6", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ITEM_SOCKET6", 0);
#endif

#if defined(ENABLE_STONE_OF_BLESS)
	PyModule_AddIntConstant(poModule, "ENABLE_STONE_OF_BLESS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_STONE_OF_BLESS", 0);
#endif

#if defined(ENABLE_DICE_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_DICE_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DICE_SYSTEM", 0);
#endif

#if defined(ENABLE_DS_SET)
	PyModule_AddIntConstant(poModule, "ENABLE_DS_SET", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DS_SET", 0);
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_LANGUAGE_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_LANGUAGE_SYSTEM", 0);
#endif

#if defined(ENABLE_SKILLBOOK_COMB_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_SKILLBOOK_COMB_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SKILLBOOK_COMB_SYSTEM", 0);
#endif

#if defined(ENABLE_SHOPEX_TAB4)
	PyModule_AddIntConstant(poModule, "ENABLE_SHOPEX_TAB4", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SHOPEX_TAB4", 0);
#endif

#if defined(ENABLE_MAGIC_REDUCTION)
	PyModule_AddIntConstant(poModule, "ENABLE_MAGIC_REDUCTION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MAGIC_REDUCTION", 0);
#endif

#if defined(ENABLE_SKILL_COOLTIME_UPDATE)
	PyModule_AddIntConstant(poModule, "ENABLE_SKILL_COOLTIME_UPDATE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SKILL_COOLTIME_UPDATE", 0);
#endif

#if defined(ENABLE_HEART_BEAT_MALL)
	PyModule_AddIntConstant(poModule, "ENABLE_HEART_BEAT_MALL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_HEART_BEAT_MALL", 0);
#endif

#if defined(ENABLE_FOV_OPTION)
	PyModule_AddIntConstant(poModule, "ENABLE_FOV_OPTION", 1);
	PyModule_AddIntConstant(poModule, "DEFAULT_CAMERA_PERSPECTIVE", c_fDefaultCameraPerspective);
	PyModule_AddIntConstant(poModule, "MIN_CAMERA_PERSPECTIVE", c_fMinCameraPerspective);
	PyModule_AddIntConstant(poModule, "MAX_CAMERA_PERSPECTIVE", c_fMaxCameraPerspective);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_FOV_OPTION", 0);
#endif

#if defined(ENABLE_HIGH_LIGHT_IMAGE)
	PyModule_AddIntConstant(poModule, "ENABLE_HIGH_LIGHT_IMAGE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_HIGH_LIGHT_IMAGE", 0);
#endif

#if defined(ENABLE_CONQUEROR_LEVEL)
	PyModule_AddIntConstant(poModule, "ENABLE_CONQUEROR_LEVEL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CONQUEROR_LEVEL", 0);
#endif

#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
	PyModule_AddIntConstant(poModule, "WJ_SHOW_PARTY_ON_MINIMAP", 1);
#else
	PyModule_AddIntConstant(poModule, "WJ_SHOW_PARTY_ON_MINIMAP", 0);
#endif

#if defined(ENABLE_PARTY_CHANNEL_FIX)
	PyModule_AddIntConstant(poModule, "ENABLE_PARTY_CHANNEL_FIX", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PARTY_CHANNEL_FIX", 0);
#endif

#if defined(ENABLE_SHADOW_RENDER_QUALITY_OPTION)
	PyModule_AddIntConstant(poModule, "ENABLE_SHADOW_RENDER_QUALITY_OPTION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SHADOW_RENDER_QUALITY_OPTION", 0);
#endif

#if defined(ENABLE_MYSHOP_DECO)
	PyModule_AddIntConstant(poModule, "ENABLE_MYSHOP_DECO", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MYSHOP_DECO", 0);
#endif

#if defined(ENABLE_CHATTING_WINDOW_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_CHATTING_WINDOW_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CHATTING_WINDOW_RENEWAL", 0);
#endif

#if defined(ENABLE_MAILBOX)
	PyModule_AddIntConstant(poModule, "ENABLE_MAILBOX", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MAILBOX", 0);
#endif

#if defined(ENABLE_KEYCHANGE_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_KEYCHANGE_SYSTEM", 1);
#	if defined(ENABLE_DSS_KEY_SELECT)
	PyModule_AddIntConstant(poModule, "ENABLE_DSS_KEY_SELECT", 1);
#	else
	PyModule_AddIntConstant(poModule, "ENABLE_DSS_KEY_SELECT", 0);
#	endif
#else
	PyModule_AddIntConstant(poModule, "ENABLE_KEYCHANGE_SYSTEM", 0);
#endif

#if defined(ENABLE_APPLY_RANDOM)
	PyModule_AddIntConstant(poModule, "ENABLE_APPLY_RANDOM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_APPLY_RANDOM", 0);
#endif

#if defined(ENABLE_SLOT_SCALE)
	PyModule_AddIntConstant(poModule, "ENABLE_SLOT_SCALE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SLOT_SCALE", 0);
#endif

#if defined(ENABLE_DS_CHANGE_ATTR)
	PyModule_AddIntConstant(poModule, "ENABLE_DS_CHANGE_ATTR", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DS_CHANGE_ATTR", 0);
#endif

#if defined(ENABLE_DEFENSE_WAVE)
	PyModule_AddIntConstant(poModule, "ENABLE_DEFENSE_WAVE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DEFENSE_WAVE", 0);
#endif

#if defined(ENABLE_RANKING_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_RANKING_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RANKING_SYSTEM", 0);
#endif

#if defined(ENABLE_CLIP_MASK)
	PyModule_AddIntConstant(poModule, "ENABLE_CLIP_MASK", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CLIP_MASK", 0);
#endif

#if defined(ENABLE_MOUSE_WHEEL_TOP_WINDOW)
	PyModule_AddIntConstant(poModule, "ENABLE_MOUSE_WHEEL_TOP_WINDOW", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MOUSE_WHEEL_TOP_WINDOW", 0);
#endif

#if defined(ENABLE_LOOTING_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_LOOTING_SYSTEM", 1);
#	if defined(ENABLE_PREMIUM_LOOTING)
	PyModule_AddIntConstant(poModule, "ENABLE_PREMIUM_LOOTING", 1);
#	else
	PyModule_AddIntConstant(poModule, "ENABLE_PREMIUM_LOOTING", 0);
#	endif
#else
	PyModule_AddIntConstant(poModule, "ENABLE_LOOTING_SYSTEM", 0);
#endif

#if defined(ENABLE_MOVE_COSTUME_ATTR)
	PyModule_AddIntConstant(poModule, "ENABLE_MOVE_COSTUME_ATTR", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MOVE_COSTUME_ATTR", 0);
#endif

#if defined(ENABLE_CHANGED_ATTR)
	PyModule_AddIntConstant(poModule, "ENABLE_CHANGED_ATTR", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CHANGED_ATTR", 0);
#endif

#if defined(ENABLE_POPUP_NOTICE)
	PyModule_AddIntConstant(poModule, "ENABLE_POPUP_NOTICE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_POPUP_NOTICE", 0);
#endif

#if defined(RENDER_TARGET)
	PyModule_AddIntConstant(poModule, "RENDER_TARGET", 1);
#else
	PyModule_AddIntConstant(poModule, "RENDER_TARGET", 0);
#endif

#if defined(ENABLE_SLOT_COOLTIME_REFRESH_FIX)
	PyModule_AddIntConstant(poModule, "ENABLE_SLOT_COOLTIME_REFRESH_FIX", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SLOT_COOLTIME_REFRESH_FIX", 0);
#endif

#if defined(ENABLE_MINI_GAME_RUMI)
	PyModule_AddIntConstant(poModule, "ENABLE_MINI_GAME_RUMI", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MINI_GAME_RUMI", 0);
#endif
	
#if defined(ENABLE_OKEY_EVENT_FLAG_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_OKEY_EVENT_FLAG_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_OKEY_EVENT_FLAG_RENEWAL", 0);
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
	PyModule_AddIntConstant(poModule, "ENABLE_MINI_GAME_YUTNORI", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MINI_GAME_YUTNORI", 0);
#endif

#if defined(ENABLE_YUTNORI_EVENT_FLAG_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_YUTNORI_EVENT_FLAG_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_YUTNORI_EVENT_FLAG_RENEWAL", 0);
#endif

#if defined(ENABLE_OX_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_OX_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_OX_RENEWAL", 0);
#endif

#if defined(ENABLE_INPUT_CANCEL)
	PyModule_AddIntConstant(poModule, "ENABLE_INPUT_CANCEL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_INPUT_CANCEL", 0);
#endif

#if defined(ENABLE_QUEST_WIDTH_EXPANSION)
	PyModule_AddIntConstant(poModule, "ENABLE_QUEST_WIDTH_EXPANSION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_QUEST_WIDTH_EXPANSION", 0);
#endif

#if defined(ENABLE_LUCKY_BOX)
	PyModule_AddIntConstant(poModule, "ENABLE_LUCKY_BOX", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_LUCKY_BOX", 0);
#endif

#if defined(ENABLE_ATTR_6TH_7TH)
	PyModule_AddIntConstant(poModule, "ENABLE_ATTR_6TH_7TH", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ATTR_6TH_7TH", 0);
#endif

#if defined(ENABLE_FISHING_GAME)
	PyModule_AddIntConstant(poModule, "ENABLE_FISHING_GAME", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_FISHING_GAME", 0);
#endif

#if defined(ENABLE_HELP_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_HELP_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_HELP_RENEWAL", 0);
#endif

#if defined(ENABLE_CEF_BROWSER)
	PyModule_AddIntConstant(poModule, "BROWSER_TYPE_ITEM_MALL", CefWebBrowser::EBrowserType::BROWSER_TYPE_ITEM_MALL);
	PyModule_AddIntConstant(poModule, "BROWSER_TYPE_DUTCH", CefWebBrowser::EBrowserType::BROWSER_TYPE_DUTCH);
	PyModule_AddIntConstant(poModule, "BROWSER_TYPE_POPUP_NOTICE", CefWebBrowser::EBrowserType::BROWSER_TYPE_POPUP_NOTICE);
#endif

#if defined(ENABLE_CLIENT_TIMER)
	PyModule_AddIntConstant(poModule, "ENABLE_CLIENT_TIMER", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CLIENT_TIMER", 0);
#endif

#if defined(ENABLE_CUBE_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_CUBE_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CUBE_RENEWAL", 0);
#endif

#if defined(ENABLE_CHAR_CONFIG)
	PyModule_AddIntConstant(poModule, "ENABLE_CHAR_CONFIG", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CHAR_CONFIG", 0);
#endif

#if defined(ENABLE_OPTIMIZATION)
	PyModule_AddIntConstant(poModule, "ENABLE_OPTIMIZATION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_OPTIMIZATION", 0);
#endif

#if defined(ENABLE_DELETE_FAILURE_TYPE)
	PyModule_AddIntConstant(poModule, "ENABLE_DELETE_FAILURE_TYPE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DELETE_FAILURE_TYPE", 0);
#endif

#if defined(ENABLE_DELETE_FAILURE_TYPE_ADD)
	PyModule_AddIntConstant(poModule, "ENABLE_DELETE_FAILURE_TYPE_ADD", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DELETE_FAILURE_TYPE_ADD", 0);
#endif

#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
	PyModule_AddIntConstant(poModule, "ENABLE_SNOWFLAKE_STICK_EVENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SNOWFLAKE_STICK_EVENT", 0);
#endif

#if defined(ENABLE_GEM_CONVERTER)
	PyModule_AddIntConstant(poModule, "ENABLE_GEM_CONVERTER", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GEM_CONVERTER", 0);
#endif

#if defined(ENABLE_GAME_OPTION_ESCAPE)
	PyModule_AddIntConstant(poModule, "ENABLE_GAME_OPTION_ESCAPE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GAME_OPTION_ESCAPE", 0);
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_REFINE_ELEMENT_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_REFINE_ELEMENT_SYSTEM", 0);
#endif

#if defined(ENABLE_HIDE_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_HIDE_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_HIDE_COSTUME_SYSTEM", 0);
#endif

#if defined(ENABLE_EXTEND_MALLBOX)
	PyModule_AddIntConstant(poModule, "ENABLE_EXTEND_MALLBOX", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EXTEND_MALLBOX", 0);
#endif
	
#if defined(ENABLE_MONSTER_CARD)
	PyModule_AddIntConstant(poModule, "ENABLE_MONSTER_CARD", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MONSTER_CARD", 0);
#endif

#if defined(RENDER_TARGET)
#	if defined(ENABLE_MONSTER_CARD)
	PyModule_AddIntConstant(poModule, "RENDER_TARGET_INDEX_ILLUSTRATED", CRenderTargetManager::RENDER_TARGET_INDEX_ILLUSTRATED);
#	endif
#	if defined(ENABLE_MYSHOP_DECO)
	PyModule_AddIntConstant(poModule, "RENDER_TARGET_INDEX_MYSHOPDECO", CRenderTargetManager::RENDER_TARGET_INDEX_MYSHOPDECO);
#	endif
#	if defined(ENABLE_MINI_GAME_YUTNORI)
	PyModule_AddIntConstant(poModule, "RENDER_TARGET_INDEX_YUTNORI", CRenderTargetManager::RENDER_TARGET_INDEX_YUTNORI);
#	endif
#endif

#if defined(ENABLE_PARTY_PROFICY)
	PyModule_AddIntConstant(poModule, "ENABLE_PARTY_PROFICY", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PARTY_PROFICY", 0);
#endif

#if defined(ENABLE_PARTY_INSIGHT)
	PyModule_AddIntConstant(poModule, "ENABLE_PARTY_INSIGHT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PARTY_INSIGHT", 0);
#endif
	
#if defined(ENABLE_BALANCE_IMPROVING)
	PyModule_AddIntConstant(poModule, "ENABLE_BALANCE_IMPROVING", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_BALANCE_IMPROVING", 0);
#endif

#if defined(ENABLE_SAFEBOX_IMPROVING)
	PyModule_AddIntConstant(poModule, "ENABLE_SAFEBOX_IMPROVING", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SAFEBOX_IMPROVING", 0);
#endif

#if defined(ENABLE_ATLAS_GM_GOTO)
	PyModule_AddIntConstant(poModule, "ENABLE_ATLAS_GM_GOTO", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ATLAS_GM_GOTO", 0);
#endif

#if defined(ENABLE_EVENT_BANNER)
	PyModule_AddIntConstant(poModule, "ENABLE_EVENT_BANNER", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EVENT_BANNER", 0);
#endif

#if defined(ENABLE_EVENT_BANNER)
	PyModule_AddIntConstant(poModule, "ENABLE_EVENT_BANNER", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EVENT_BANNER", 0);
#endif

#if defined(ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL", 0);
#endif

#if defined(ENABLE_LOCALE_CLIENT)
	PyModule_AddIntConstant(poModule, "ENABLE_LOCALE_CLIENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_LOCALE_CLIENT", 0);
#endif

#if defined(ENABLE_QUEST_REQUEST_EVENT)
	PyModule_AddIntConstant(poModule, "ENABLE_QUEST_REQUEST_EVENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_QUEST_REQUEST_EVENT", 0);
#endif
	
#if defined(ENABLE_LEFT_SEAT)
	PyModule_AddIntConstant(poModule, "ENABLE_LEFT_SEAT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_LEFT_SEAT", 0);
#endif

#if defined(ENABLE_AFFECT_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_AFFECT_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_AFFECT_RENEWAL", 0);
#endif

#if defined(ENABLE_ELEMENTAL_DUNGEON)
	PyModule_AddIntConstant(poModule, "ENABLE_ELEMENTAL_DUNGEON", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ELEMENTAL_DUNGEON", 0);
#endif

#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_DRAGONLAIR_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_DRAGONLAIR_SYSTEM", 0);
#endif

#if defined(ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM", 0);
#endif

#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
	PyModule_AddIntConstant(poModule, "ENABLE_SUMMER_EVENT_ROULETTE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SUMMER_EVENT_ROULETTE", 0);
#endif

#if defined(ENABLE_FLOWER_EVENT)
	PyModule_AddIntConstant(poModule, "ENABLE_FLOWER_EVENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_FLOWER_EVENT", 0);
#endif

#ifdef ENABLE_QUEEN_NETHIS
	PyModule_AddIntConstant(poModule, "ENABLE_QUEEN_NETHIS", true);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_QUEEN_NETHIS", false);
#endif

	PyModule_AddIntConstant(poModule, "PLAYER_PER_ACCOUNT", PLAYER_PER_ACCOUNT4);
	PyModule_AddStringConstant(poModule, "VERSION", VER_FILE_VERSION_STR);
}
