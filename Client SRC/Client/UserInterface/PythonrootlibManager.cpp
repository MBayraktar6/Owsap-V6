#include "StdAfx.h"

#if defined(_CYTHON)
#include "PythonrootlibManager.h"
#include <Python-2.7/python.h>

#if defined(_DEBUG)
#pragma comment (lib, "Debug/rootlib.lib")
#else
#pragma comment (lib, "Release/rootlib.lib")
#endif

struct rootlib_SMethodDef
{
	const char* func_name;
	void (*func)();
};

PyMODINIT_FUNC initcolorInfo();
PyMODINIT_FUNC initconsoleModule();
PyMODINIT_FUNC initconstInfo();
PyMODINIT_FUNC initdebugInfo();
PyMODINIT_FUNC initdragon_soul_refine_settings();
PyMODINIT_FUNC initemotion();
PyMODINIT_FUNC initexception();
PyMODINIT_FUNC initgame();
PyMODINIT_FUNC initinterfaceModule();
PyMODINIT_FUNC initintroCreate();
PyMODINIT_FUNC initintroEmpire();
PyMODINIT_FUNC initintroLoading();
PyMODINIT_FUNC initintroLogin();
PyMODINIT_FUNC initintroLogo();
PyMODINIT_FUNC initintroSelect();
PyMODINIT_FUNC initlocaleInfo();
PyMODINIT_FUNC initmouseModule();
PyMODINIT_FUNC initmusicInfo();
PyMODINIT_FUNC initnetworkModule();
PyMODINIT_FUNC initNew_introCreate();
PyMODINIT_FUNC initNew_introSelect();
PyMODINIT_FUNC initosfInfo();
PyMODINIT_FUNC initplayerSettingModule();
PyMODINIT_FUNC initPrototype();
PyMODINIT_FUNC initserverCommandParser();
PyMODINIT_FUNC initserverInfo();
PyMODINIT_FUNC initstringCommander();
PyMODINIT_FUNC initsystem();
PyMODINIT_FUNC initui();
PyMODINIT_FUNC inituiAcce();
PyMODINIT_FUNC inituiAffectShower();
PyMODINIT_FUNC inituiAttachMetin();
PyMODINIT_FUNC inituiAttr67Add();
PyMODINIT_FUNC inituiAuction();
PyMODINIT_FUNC inituiAura();
PyMODINIT_FUNC inituiAutoBan();
PyMODINIT_FUNC inituiCandidate();
PyMODINIT_FUNC inituiChangeLook();
PyMODINIT_FUNC inituiCharacter();
PyMODINIT_FUNC inituiCharacterDetails();
PyMODINIT_FUNC inituiChat();
PyMODINIT_FUNC inituiCommon();
PyMODINIT_FUNC inituiCube();
PyMODINIT_FUNC inituiCubeRenewal();
PyMODINIT_FUNC inituiDragonSoul();
PyMODINIT_FUNC inituiEquipmentDialog();
PyMODINIT_FUNC inituiEscapePopup();
PyMODINIT_FUNC inituiEx();
PyMODINIT_FUNC inituiExchange();
PyMODINIT_FUNC inituiFishingGame();
PyMODINIT_FUNC inituiFlowerEvent();
PyMODINIT_FUNC inituiGameButton();
PyMODINIT_FUNC inituiGameOption();
PyMODINIT_FUNC inituiGemShop();
PyMODINIT_FUNC inituiGuild();
PyMODINIT_FUNC inituiGuildDragonLairRanking();
PyMODINIT_FUNC inituiHelp();
PyMODINIT_FUNC inituiInventory();
PyMODINIT_FUNC inituiItemCombination();
PyMODINIT_FUNC inituiKeyChange();
PyMODINIT_FUNC inituiLootingSystem();
PyMODINIT_FUNC inituiLuckyBox();
PyMODINIT_FUNC inituiMailBox();
PyMODINIT_FUNC inituiMapNameShower();
PyMODINIT_FUNC inituiMessenger();
PyMODINIT_FUNC inituiMiniGame();
PyMODINIT_FUNC inituiMiniGameCatchKing();
PyMODINIT_FUNC inituiMiniGameRumi();
PyMODINIT_FUNC inituiMiniGameRoulette();
PyMODINIT_FUNC inituiMiniGameYutnori();
PyMODINIT_FUNC inituiMiniMap();
PyMODINIT_FUNC inituiMyShopDecoration();
PyMODINIT_FUNC inituiOption();
PyMODINIT_FUNC inituiParty();
PyMODINIT_FUNC inituiPhaseCurtain();
PyMODINIT_FUNC inituiPickETC();
PyMODINIT_FUNC inituiPickMoney();
PyMODINIT_FUNC inituiPlayerGauge();
PyMODINIT_FUNC inituiPointReset();
PyMODINIT_FUNC inituiPopupNotice();
PyMODINIT_FUNC inituiPrivateShop();
PyMODINIT_FUNC inituiPrivateShopBuilder();
PyMODINIT_FUNC inituiPrivateShopSearch();
PyMODINIT_FUNC inituiQuest();
PyMODINIT_FUNC inituiRankingBoard();
PyMODINIT_FUNC inituiRefine();
PyMODINIT_FUNC inituiRestart();
PyMODINIT_FUNC inituiSafebox();
PyMODINIT_FUNC inituiScriptLocale();
PyMODINIT_FUNC inituiSelectAttr();
PyMODINIT_FUNC inituiSelectItem();
PyMODINIT_FUNC inituiSelectItemEx();
PyMODINIT_FUNC inituiSelectMusic();
PyMODINIT_FUNC inituiShop();
PyMODINIT_FUNC inituiSkillBookCombination();
PyMODINIT_FUNC inituiSnowflakeStickEvent();
PyMODINIT_FUNC inituiSystem();
PyMODINIT_FUNC inituiSystemOption();
PyMODINIT_FUNC inituiTarget();
PyMODINIT_FUNC inituiTaskBar();
PyMODINIT_FUNC inituiTip();
PyMODINIT_FUNC inituiToolTip();
PyMODINIT_FUNC inituiUploadMark();
PyMODINIT_FUNC inituiWeb();
PyMODINIT_FUNC inituiWhisper();

rootlib_SMethodDef rootlib_init_methods[] =
{
	{ "colorInfo", initcolorInfo },
	{ "consoleModule", initconsoleModule },
	{ "constInfo", initconstInfo },
	{ "debugInfo", initdebugInfo },
	{ "dragon_soul_refine_settings", initdragon_soul_refine_settings },
	{ "emotion", initemotion },
	{ "exception", initexception },
	{ "game", initgame },
	{ "interfaceModule", initinterfaceModule },
	{ "introCreate", initintroCreate },
	{ "introEmpire", initintroEmpire },
	{ "introLoading", initintroLoading },
	{ "introLogin", initintroLogin },
	{ "introLogo", initintroLogo },
	{ "introSelect", initintroSelect },
	{ "localeInfo", initlocaleInfo },
	{ "mouseModule", initmouseModule },
	{ "musicInfo", initmusicInfo },
	{ "networkModule", initnetworkModule },
	{ "New_introCreate", initNew_introCreate },
	{ "New_introSelect", initNew_introSelect },
	{ "osfInfo", initosfInfo },
	{ "playerSettingModule", initplayerSettingModule },
	{ "Prototype", initPrototype },
	{ "serverCommandParser", initserverCommandParser },
	{ "serverInfo", initserverInfo },
	{ "stringCommander", initstringCommander },
	{ "system", initsystem },
	{ "ui", initui },
	{ "uiAcce", inituiAcce },
	{ "uiAffectShower", inituiAffectShower },
	{ "uiAttachMetin", inituiAttachMetin },
	{ "uiAttr67Add", inituiAttr67Add },
	{ "uiAuction", inituiAuction },
	{ "uiAura", inituiAura },
	{ "uiAutoBan", inituiAutoBan },
	{ "uiCandidate", inituiCandidate },
	{ "uiChangeLook", inituiChangeLook },
	{ "uiCharacter", inituiCharacter },
	{ "uiCharacterDetails", inituiCharacterDetails },
	{ "uiChat", inituiChat },
	{ "uiCommon", inituiCommon },
	{ "uiCube", inituiCube },
	{ "uiCubeRenewal", inituiCubeRenewal },
	{ "uiDragonSoul", inituiDragonSoul },
	{ "uiEquipmentDialog", inituiEquipmentDialog },
	{ "uiEscapePopup", inituiEscapePopup },
	{ "uiEx", inituiEx },
	{ "uiExchange", inituiExchange },
	{ "uiFishingGame", inituiFishingGame },
	{ "uiFlowerEvent", inituiFlowerEvent },
	{ "uiGameButton", inituiGameButton },
	{ "uiGameOption", inituiGameOption },
	{ "uiGemShop", inituiGemShop },
	{ "uiGuild", inituiGuild },
	{ "uiGuildDragonLairRanking", inituiGuildDragonLairRanking },
	{ "uiHelp", inituiHelp },
	{ "uiInventory", inituiInventory },
	{ "uiItemCombination", inituiItemCombination },
	{ "uiKeyChange", inituiKeyChange },
	{ "uiLootingSystem", inituiLootingSystem },
	{ "uiLuckyBox", inituiLuckyBox },
	{ "uiMailBox", inituiMailBox },
	{ "uiMapNameShower", inituiMapNameShower },
	{ "uiMessenger", inituiMessenger },
	{ "uiMiniGame", inituiMiniGame },
	{ "uiMiniGameCatchKing", inituiMiniGameCatchKing },
	{ "uiMiniGameRumi", inituiMiniGameRumi },
	{ "uiMiniGameRoulette", inituiMiniGameRoulette },
	{ "uiMiniGameYutnori", inituiMiniGameYutnori },
	{ "uiMiniMap", inituiMiniMap },
	{ "uiMyShopDecoration", inituiMyShopDecoration },
	{ "uiOption", inituiOption },
	{ "uiParty", inituiParty },
	{ "uiPhaseCurtain", inituiPhaseCurtain },
	{ "uiPickETC", inituiPickETC },
	{ "uiPickMoney", inituiPickMoney },
	{ "uiPlayerGauge", inituiPlayerGauge },
	{ "uiPointReset", inituiPointReset },
	{ "uiPopupNotice", inituiPopupNotice },
	{ "uiPrivateShop", inituiPrivateShop },
	{ "uiPrivateShopBuilder", inituiPrivateShopBuilder },
	{ "uiPrivateShopSearch", inituiPrivateShopSearch },
	{ "uiQuest", inituiQuest },
	{ "uiRankingBoard", inituiRankingBoard },
	{ "uiRefine", inituiRefine },
	{ "uiRestart", inituiRestart },
	{ "uiSafebox", inituiSafebox },
	{ "uiScriptLocale", inituiScriptLocale },
	{ "uiSelectAttr", inituiSelectAttr },
	{ "uiSelectItem", inituiSelectItem },
	{ "uiSelectItemEx", inituiSelectItemEx },
	{ "uiSelectMusic", inituiSelectMusic },
	{ "uiShop", inituiShop },
	{ "uiSkillBookCombination", inituiSkillBookCombination },
	{ "uiSnowflakeStickEvent", inituiSnowflakeStickEvent },
	{ "uiSystem", inituiSystem },
	{ "uiSystemOption", inituiSystemOption },
	{ "uiTarget", inituiTarget },
	{ "uiTaskBar", inituiTaskBar },
	{ "uiTip", inituiTip },
	{ "uiToolTip", inituiToolTip },
	{ "uiUploadMark", inituiUploadMark },
	{ "uiWeb", inituiWeb },
	{ "uiWhisper", inituiWhisper },
	{ NULL, NULL },
};

static PyObject* rootlib_isExist(PyObject* self, PyObject* args)
{
	char* func_name;

	if (!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != rootlib_init_methods[i].func_name; i++)
	{
		if (0 == _stricmp(rootlib_init_methods[i].func_name, func_name))
		{
			return Py_BuildValue("i", 1);
		}
	}
	return Py_BuildValue("i", 0);
}

static PyObject* rootlib_moduleImport(PyObject* self, PyObject* args)
{
	char* func_name;

	if (!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != rootlib_init_methods[i].func_name; i++)
	{
		if (0 == _stricmp(rootlib_init_methods[i].func_name, func_name))
		{
			rootlib_init_methods[i].func();
			if (PyErr_Occurred())
				return NULL;

			PyObject* m = PyDict_GetItemString(PyImport_GetModuleDict(), rootlib_init_methods[i].func_name);
			if (m == NULL)
			{
				PyErr_SetString(PyExc_SystemError, "dynamic module not initialized properly");
				return NULL;
			}
			Py_INCREF(m);
			return Py_BuildValue("S", m);
		}
	}
	return NULL;
}

static PyObject* rootlib_getList(PyObject* self, PyObject* args)
{
	int iTupleSize = 0;
	while (NULL != rootlib_init_methods[iTupleSize].func_name) { iTupleSize++; }

	PyObject* retTuple = PyTuple_New(iTupleSize);
	for (int i = 0; NULL != rootlib_init_methods[i].func_name; i++)
	{
		PyObject* retSubString = PyString_FromString(rootlib_init_methods[i].func_name);
		PyTuple_SetItem(retTuple, i, retSubString);
	}
	return retTuple;
}

void initrootlibManager()
{
	static struct PyMethodDef methods[] =
	{
		{ "isExist", rootlib_isExist, METH_VARARGS },
		{ "moduleImport", rootlib_moduleImport, METH_VARARGS },
		{ "getList", rootlib_getList, METH_VARARGS },
		{ NULL, NULL },
	};

	PyObject* m;
	m = Py_InitModule("rootlib", methods);
}
#endif
