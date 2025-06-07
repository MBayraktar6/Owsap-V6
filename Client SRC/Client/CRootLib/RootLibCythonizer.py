from sys import path as sys_path
from os.path import splitext as op_splitext

libpath = r'..\Py2Lib\lib'
sys_path.append(libpath)
rootpath = r'.'

capNameList = {
	"colorinfo" : "colorInfo",
	"consolemodule" : "consoleModule",
	"constinfo" : "constInfo",
	"debuginfo" : "debugInfo",
	"dragon_soul_refine_settings" : "dragon_soul_refine_settings",
	"emotion" : "emotion",
	"exception" : "exception",
	"game" : "game",
	"interfacemodule" : "interfaceModule",
	"introcreate" : "introCreate",
	"introempire" : "introEmpire",
	"introloading" : "introLoading",
	"intrologin" : "introLogin",
	"intrologo" : "introLogo",
	"introselect" : "introSelect",
	"localeinfo" : "localeInfo",
	"mousemodule" : "mouseModule",
	"musicinfo" : "musicInfo",
	"networkmodule" : "networkModule",
	"new_introcreate" : "New_introCreate",
	"new_introselect" : "New_introSelect",
	"osfinfo" : "osfInfo",
	"playersettingmodule" : "playerSettingModule",
	"prototype" : "Prototype",
	"servercommandparser" : "serverCommandParser",
	"serverinfo" : "serverInfo",
	"stringcommander" : "stringCommander",
	"system" : "system",
	"ui" : "ui",
	"uiacce" : "uiAcce",
	"uiaffectshower" : "uiAffectShower",
	"uiattachmetin" : "uiAttachMetin",
	"uiattr67add" : "uiAttr67Add",
	"uiauction" : "uiAuction",
	"uiaura" : "uiAura",
	"uiautoban" : "uiAutoBan",
	"uicandidate" : "uiCandidate",
	"uichangelook" : "uiChangeLook",
	"uicharacter" : "uiCharacter",
	"uicharacterdetails" : "uiCharacterDetails",
	"uichat" : "uiChat",
	"uicommon" : "uiCommon",
	"uicube" : "uiCube",
	"uicuberenewal" : "uiCubeRenewal",
	"uidragonsoul" : "uiDragonSoul",
	"uiequipmentdialog" : "uiEquipmentDialog",
	"uiescapepopup" : "uiEscapePopup",
	"uiex" : "uiEx",
	"uiexchange" : "uiExchange",
	"uifishinggame" : "uiFishingGame",
	"uiflowerevent" : "uiFlowerEvent",
	"uigamebutton" : "uiGameButton",
	"uigameoption" : "uiGameOption",
	"uigemshop" : "uiGemShop",
	"uiguild" : "uiGuild",
	"uiguilddragonlairranking" : "uiGuildDragonLairRanking",
	"uihelp" : "uiHelp",
	"uiinventory" : "uiInventory",
	"uiitemcombination" : "uiItemCombination",
	"uikeychange" : "uiKeyChange",
	"uilootingsystem" : "uiLootingSystem",
	"uiluckybox" : "uiLuckyBox",
	"uimailbox" : "uiMailBox",
	"uimapnameshower" : "uiMapNameShower",
	"uimessenger" : "uiMessenger",
	"uiminigame" : "uiMiniGame",
	"uiminigamecatchking" : "uiMiniGameCatchKing",
	"uiminigamerumi" : "uiMiniGameRumi",
	"uiminigameroulette" : "uiMiniGameRoulette",
	"uiminigameyutnori" : "uiMiniGameYutnori",
	"uiminimap" : "uiMiniMap",
	"uimyshopdecoration" : "uiMyShopDecoration",
	"uioption" : "uiOption",
	"uiparty" : "uiParty",
	"uiphasecurtain" : "uiPhaseCurtain",
	"uipicketc" : "uiPickETC",
	"uipickmoney" : "uiPickMoney",
	"uiplayergauge" : "uiPlayerGauge",
	"uipointreset" : "uiPointReset",
	"uipopupnotice" : "uiPopupNotice",
	"uiprivateshop" : "uiPrivateShop",
	"uiprivateshopbuilder" : "uiPrivateShopBuilder",
	"uiprivateshopsearch" : "uiPrivateShopSearch",
	"uiquest" : "uiQuest",
	"uirankingboard" : "uiRankingBoard",
	"uirefine" : "uiRefine",
	"uirestart" : "uiRestart",
	"uisafebox" : "uiSafebox",
	"uiscriptlocale" : "uiScriptLocale",
	"uiselectattr" : "uiSelectAttr",
	"uiselectitem" : "uiSelectItem",
	"uiselectitemex" : "uiSelectItemEx",
	"uiselectmusic" : "uiSelectMusic",
	"uishop" : "uiShop",
	"uiskillbookcombination" : "uiSkillBookCombination",
	"uisnowflakestickevent" : "uiSnowflakeStickEvent",
	"uisystem" : "uiSystem",
	"uisystemoption" : "uiSystemOption",
	"uitarget" : "uiTarget",
	"uitaskbar" : "uiTaskBar",
	"uitip" : "uiTip",
	"uitooltip" : "uiToolTip",
	"uiuploadmark" : "uiUploadMark",
	"uiweb" : "uiWeb",
	"uiwhisper" : "uiWhisper",
}

def checkCapName(x):
	base, ext = op_splitext(x)
	try:
		return capNameList[base.lower()] + ext
	except KeyError:
		return x

#import utils
import imp
fp, pathname, description = imp.find_module('utils', [libpath])
utils = imp.load_module('utils', fp, pathname, description)

pys = utils.findMatchedFiles(rootpath, "*.py")
if __file__ in pys:
	pys.remove(__file__)
pys = [checkCapName(x) for x in pys]

import cythonizer
moduleLst = cythonizer.run(pys, forceRecompile=True)
moduleNameLst = []
sourceFileLst = []

for m in moduleLst:
	for source in m.sources:
		base, ext = op_splitext(source)
		moduleName = base.split('/')[-1]
		moduleNameLst.append(moduleName)
		sourceFileLst.append(base + (".cpp" if "c++" == m.language else ".c"))

import sourceWriter
sourceFileName = sourceWriter.run(moduleNameLst, 'rootlib')
print "%s successfully created." % sourceFileName
