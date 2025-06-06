import ui
import uiScriptLocale
import net
import app
import dbg
import player
import background
import wndMgr

import localeInfo
import chrmgr
import colorInfo
import constInfo
import osfInfo

import playerSettingModule
import stringCommander
import emotion

####################################
# 빠른 실행을 위한 모듈 로딩 분담
####################################
import uiRefine
import uiToolTip
import uiAttachMetin
import uiPickMoney
import uiChat
import uiMessenger
import uiHelp
import uiWhisper
import uiPointReset
import uiShop
import uiExchange
import uiSystem
import uiOption
import uiRestart
####################################

IsLoaded = False

class LoadingWindow(ui.ScriptWindow):
	def __init__(self, stream):
		#print "NEW LOADING WINDOW -------------------------------------------------------------------------------"
		ui.Window.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, self)

		self.stream = stream
		self.loadingImage = 0
		self.loadingGage = 0
		self.errMsg = 0
		self.update = 0
		self.playerX = 0
		self.playerY = 0
		self.loadStepList = []

	def __del__(self):
		#print "---------------------------------------------------------------------------- DELETE LOADING WINDOW"
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, 0)
		ui.Window.__del__(self)

	def Open(self):
		#print "OPEN LOADING WINDOW -------------------------------------------------------------------------------"

		#app.HideCursor()

		try:
			pyScrLoader = ui.PythonScriptLoader()

			if localeInfo.IsARABIC():
				pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "LoadingWindow.py")
			else:
				pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "LoadingWindow.py")
				#pyScrLoader.LoadScriptFile(self, "UIScript/LoadingWindow.py")
		except:
			import exception
			exception.Abort("LodingWindow.Open - LoadScriptFile Error")

		try:
			self.loadingImage = self.GetChild("BackGround")
			self.errMsg = self.GetChild("ErrorMessage")
			self.loadingGage = self.GetChild("FullGage")
			if app.ENABLE_LOADING_TIP:
				self.loadingTipBackground = self.GetChild("TipBackground")
				self.loadingTip = self.GetChild("LoadingTip")

				if osfInfo.SHOW_LOADING_PROGRESS == True:
					loadingTipProgress = ui.TextLine()
					loadingTipProgress.SetParent(self.loadingTip)
					loadingTipProgress.SetPosition(0, 25)
					loadingTipProgress.Show()
					self.loadingTipProgress = loadingTipProgress

		except:
			import exception
			exception.Abort("LodingWindow.Open - LoadScriptFile Error")

		self.errMsg.Hide()

		if localeInfo.IsEUROPE() and not localeInfo.IsBRAZIL():
			imgFileNameDict = {
				0 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading0.sub",
				1 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading1.sub",
				2 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading2.sub",
				3 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading3.sub",
				#4 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading4.sub",
				#5 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading5.sub",
				#6 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading6.sub",
				#7 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading7.sub",
				#8 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading8.sub",
				#9 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading9.sub",
				#10 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading10.sub",
				#11 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading11.sub",
			}
		elif localeInfo.IsBRAZIL():
			imgFileNameDict = {
				0 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading0.sub",
				1 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading1.sub",
				2 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading2.sub",
				3 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading3.sub",
				4 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading4.sub",
			}
		elif localeInfo.IsCIBN10():
			imgFileNameDict = {
				0 : app.GetLocalePath() + "/ui/loading/loading0.jpg",
				1 : app.GetLocalePath() + "/ui/loading/loading1.jpg",
				2 : app.GetLocalePath() + "/ui/loading/loading2.jpg",
				3 : app.GetLocalePath() + "/ui/loading/loading3.jpg",
				4 : app.GetLocalePath() + "/ui/loading/loading4.jpg",
				5 : app.GetLocalePath() + "/ui/loading/loading5.jpg",
				6 : app.GetLocalePath() + "/ui/loading/loading6.jpg",
				7 : app.GetLocalePath() + "/ui/loading/loading7.jpg",
			}
		elif localeInfo.IsYMIR() or localeInfo.IsWE_KOREA() or localeInfo.IsCANADA() or localeInfo.IsJAPAN():
			imgFileNameDict = {
				0 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading0.sub",
				1 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading1.sub",
				2 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading2.sub",
				3 : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "loading/loading3.sub",
			}
		elif constInfo.SUB2_LOADING_ENABLE:
			imgFileNameDict = {
				0 : "d:/ymir work/uiloading/background_loading_warrior.sub",
				1 : "d:/ymir work/uiloading/background_loading_assassin.sub",
				2 : "d:/ymir work/uiloading/background_loading_shaman.sub",
				3 : "d:/ymir work/uiloading/background_loading_sura.sub",
				4 : "d:/ymir work/uiloading/background_loading_assassin2.sub",
				5 : "d:/ymir work/uiloading/background_loading_sura2.sub",
				6 : "d:/ymir work/uiloading/background_loading_assassin3.sub",
				7 : "d:/ymir work/uiloading/background_loading_assassin3.sub",
			}
		else:
			imgFileNameDict = {
				0 : "d:/ymir work/ui/intro/pattern/background_loading_warrior.jpg",
				1 : "d:/ymir work/ui/intro/pattern/background_loading_assassin.jpg",
				2 : "d:/ymir work/ui/intro/pattern/background_loading_shaman.jpg",
			}

		try:
			imgFileName = imgFileNameDict[app.GetRandom(0, len(imgFileNameDict) - 1)]
			self.loadingImage.LoadImage(imgFileName)

		except:
			print "LoadingWindow.Open.LoadImage - %s File Load Error" % (imgFileName)
			self.loadingImage.Hide()

		width = float(wndMgr.GetScreenWidth()) / float(self.loadingImage.GetWidth())
		height = float(wndMgr.GetScreenHeight()) / float(self.loadingImage.GetHeight())

		self.loadingImage.SetScale(width, height)
		self.loadingGage.SetPercentage(2, 100)

		if app.ENABLE_LOADING_TIP:
			self.loadingTip.SetText(player.GetLoadingTip())

		self.Show()

		chrSlot = self.stream.GetCharacterSlot()
		net.SendSelectCharacterPacket(chrSlot)

		app.SetFrameSkip(0)

	def Close(self):
		#print "---------------------------------------------------------------------------- CLOSE LOADING WINDOW"

		app.SetFrameSkip(1)

		self.loadStepList = []
		self.loadingImage = 0
		self.loadingGage = 0
		self.errMsg = 0
		self.ClearDictionary()
		self.Hide()

	def OnPressEscapeKey(self):
		app.SetFrameSkip(1)
		self.stream.SetLoginPhase()
		return True

	def __SetNext(self, next):
		if next:
			self.update = ui.__mem_func__(next)
		else:
			self.update = 0

	def __SetProgress(self, p):
		if self.loadingGage:
			if osfInfo.EXTENDED_LOADING_GUAGE:
				self.loadingGage.SetPercentage(p, 160)
			else:
				self.loadingGage.SetPercentage(2+98*p/100, 100)

	def DEBUG_LoadData(self, playerX, playerY):
		self.playerX = playerX
		self.playerY = playerY

		self.__RegisterSkill() ## 로딩 중간에 실행 하면 문제 발생
		self.__RegisterTitleName()
		if app.ENABLE_LEFT_SEAT:
			self.__RegisterLeftSeatText()
		if app.ENABLE_GUILD_LEADER_GRADE_NAME:
			self.__RegisterGuildLeaderGradeName()
		self.__RegisterColor()
		self.__InitData()
		# self.__LoadMap()
		self.__LoadSound()
		self.__LoadEffect()
		self.__LoadWarrior()
		self.__LoadAssassin()
		self.__LoadSura()
		self.__LoadShaman()
		self.__LoadWolfman()
		self.__LoadSkill()
		self.__LoadEnemy()
		self.__LoadNPC()
		self.__LoadRaceHeight()
		self.__StartGame()

	def LoadData(self, playerX, playerY):
		self.playerX = playerX
		self.playerY = playerY

		self.__RegisterDungeonMapName()
		self.__RegisterSkill() ## 로딩 중간에 실행 하면 문제 발생
		self.__RegisterTitleName()
		if app.ENABLE_LEFT_SEAT:
			self.__RegisterLeftSeatText()
		if app.ENABLE_GUILD_LEADER_GRADE_NAME:
			self.__RegisterGuildLeaderGradeName()
		self.__RegisterColor()
		self.__RegisterEmotionIcon()

		self.loadStepList = [
			(0, ui.__mem_func__(self.__InitData)),
			#(10, ui.__mem_func__(self.__LoadMap)),
			(30, ui.__mem_func__(self.__LoadSound)),
			(40, ui.__mem_func__(self.__LoadEffect)),
			(50, ui.__mem_func__(self.__LoadWarrior)),
			(60, ui.__mem_func__(self.__LoadAssassin)),
			(70, ui.__mem_func__(self.__LoadSura)),
			(80, ui.__mem_func__(self.__LoadShaman)),
			(85, ui.__mem_func__(self.__LoadWolfman)),
			(90, ui.__mem_func__(self.__LoadSkill)),
			(93, ui.__mem_func__(self.__LoadEnemy)),
			(97, ui.__mem_func__(self.__LoadNPC)),

			# GUILD_BUILDING
			(98, ui.__mem_func__(self.__LoadGuildBuilding)),
			# END_OF_GUILD_BUILDING

			# RACE HEIGHT
			(99, ui.__mem_func__(self.__LoadRaceHeight)),
			# END_OF_RACE_HEIGHT

			(100, ui.__mem_func__(self.__StartGame)),
		]

		if osfInfo.EXTENDED_LOADING_GUAGE:
			global IsLoaded
			if not IsLoaded:
				tmpLoadStepList = tuple(zip(*self.loadStepList))[0]
				for progress in set(range(tmpLoadStepList[0], tmpLoadStepList[-1] + 1)).difference(tmpLoadStepList):
					self.loadStepList.append((progress, lambda: None))
				self.loadStepList.sort()
				IsLoaded = True

		self.__SetProgress(0)
		#self.__SetNext(self.__LoadMap)

	def OnUpdate(self):
		if len(self.loadStepList) > 0:
			(progress, runFunc) = self.loadStepList[0]

			try:
				if app.ENABLE_LOADING_TIP and app.WJ_MULTI_TEXTLINE:
					if osfInfo.SHOW_LOADING_PROGRESS == True:
						if localeInfo.IsARABIC():
							self.loadingTipProgress.SetText("%%%s" % progress)
						else:
							self.loadingTipProgress.SetText("%s%%" % progress)

				runFunc()
			except:
				self.errMsg.Show()
				self.loadStepList = []

				## 이곳에서 syserr.txt 를 보낸다.

				import dbg
				dbg.TraceError(" !!! Failed to load game data : STEP [%d]" % (progress))

				#import shutil
				#import os
				#shutil.copyfile("syserr.txt", "errorlog.txt")
				#os.system("errorlog.exe")

				app.Exit()

				return

			self.loadStepList.pop(0)

			self.__SetProgress(progress)

	def __InitData(self):
		playerSettingModule.LoadGameData("INIT")

	def __RegisterDungeonMapName(self):
		background.RegisterDungeonMapName("metin2_map_spiderdungeon")
		background.RegisterDungeonMapName("metin2_map_monkeydungeon")
		background.RegisterDungeonMapName("metin2_map_monkeydungeon_02")
		background.RegisterDungeonMapName("metin2_map_monkeydungeon_03")
		background.RegisterDungeonMapName("metin2_map_deviltower1")

	def __RegisterSkill(self):
		race = net.GetMainActorRace()
		group = net.GetMainActorSkillGroup()
		empire = net.GetMainActorEmpire()

		playerSettingModule.RegisterSkill(race, group, empire)

	def __RegisterTitleName(self):
		for i in xrange(len(localeInfo.TITLE_NAME_LIST)):
			chrmgr.RegisterTitleName(i, localeInfo.TITLE_NAME_LIST[i])

	if app.ENABLE_LEFT_SEAT:
		def __RegisterLeftSeatText(self):
			chrmgr.RegisterLeftSeatText(localeInfo.LEFT_SEAT)

	if app.ENABLE_GUILD_LEADER_GRADE_NAME:
		def __RegisterGuildLeaderGradeName(self):
			for i in xrange(len(localeInfo.GUILD_LEADER_GRADE_NAME_LIST)):
				chrmgr.RegisterGuildLeaderGradeName(i, localeInfo.GUILD_LEADER_GRADE_NAME_LIST[i])

	def __RegisterColor(self):
		## Name
		NAME_COLOR_DICT = {
			chrmgr.NAMECOLOR_PC : colorInfo.CHR_NAME_RGB_PC,
			chrmgr.NAMECOLOR_NPC : colorInfo.CHR_NAME_RGB_NPC,
			chrmgr.NAMECOLOR_MOB : colorInfo.CHR_NAME_RGB_MOB,
			chrmgr.NAMECOLOR_PVP : colorInfo.CHR_NAME_RGB_PVP,
			chrmgr.NAMECOLOR_PK : colorInfo.CHR_NAME_RGB_PK,
			chrmgr.NAMECOLOR_PARTY : colorInfo.CHR_NAME_RGB_PARTY,
			chrmgr.NAMECOLOR_WARP : colorInfo.CHR_NAME_RGB_WARP,
			chrmgr.NAMECOLOR_WAYPOINT : colorInfo.CHR_NAME_RGB_WAYPOINT,
			#chrmgr.NAMECOLOR_TARGET_VICTIM : colorInfo.CHR_NAME_RGB_TARGET_VICTIM,

			chrmgr.NAMECOLOR_EMPIRE_MOB : colorInfo.CHR_NAME_RGB_EMPIRE_MOB,
			chrmgr.NAMECOLOR_EMPIRE_NPC : colorInfo.CHR_NAME_RGB_EMPIRE_NPC,
			chrmgr.NAMECOLOR_EMPIRE_PC+1 : colorInfo.CHR_NAME_RGB_EMPIRE_PC_A,
			chrmgr.NAMECOLOR_EMPIRE_PC+2 : colorInfo.CHR_NAME_RGB_EMPIRE_PC_B,
			chrmgr.NAMECOLOR_EMPIRE_PC+3 : colorInfo.CHR_NAME_RGB_EMPIRE_PC_C,
		}

		for name, rgb in NAME_COLOR_DICT.items():
			chrmgr.RegisterNameColor(name, rgb[0], rgb[1], rgb[2])

		## Title
		TITLE_COLOR_DICT = (
			colorInfo.TITLE_RGB_GOOD_4,
			colorInfo.TITLE_RGB_GOOD_3,
			colorInfo.TITLE_RGB_GOOD_2,
			colorInfo.TITLE_RGB_GOOD_1,
			colorInfo.TITLE_RGB_NORMAL,
			colorInfo.TITLE_RGB_EVIL_1,
			colorInfo.TITLE_RGB_EVIL_2,
			colorInfo.TITLE_RGB_EVIL_3,
			colorInfo.TITLE_RGB_EVIL_4,
		)
		count = 0
		for rgb in TITLE_COLOR_DICT:
			chrmgr.RegisterTitleColor(count, rgb[0], rgb[1], rgb[2])
			count += 1

	def __RegisterEmotionIcon(self):
		emotion.RegisterEmotionIcons()

	# def __LoadMap(self):
		# net.Warp(self.playerX, self.playerY)

	def __LoadSound(self):
		playerSettingModule.LoadGameData("SOUND")

	def __LoadEffect(self):
		playerSettingModule.LoadGameData("EFFECT")

	def __LoadWarrior(self):
		playerSettingModule.LoadGameData("WARRIOR")

	def __LoadAssassin(self):
		playerSettingModule.LoadGameData("ASSASSIN")

	def __LoadSura(self):
		playerSettingModule.LoadGameData("SURA")

	def __LoadShaman(self):
		playerSettingModule.LoadGameData("SHAMAN")

	def __LoadWolfman(self):
		playerSettingModule.LoadGameData("WOLFMAN")

	def __LoadSkill(self):
		playerSettingModule.LoadGameData("SKILL")

	def __LoadEnemy(self):
		playerSettingModule.LoadGameData("ENEMY")

	def __LoadNPC(self):
		playerSettingModule.LoadGameData("NPC")

	# GUILD_BUILDING
	def __LoadGuildBuilding(self):
		playerSettingModule.LoadGuildBuildingList(localeInfo.GUILD_BUILDING_LIST_TXT)
	# END_OF_GUILD_BUILDING

	def __LoadRaceHeight(self):
		playerSettingModule.LoadGameData("RACE_HEIGHT")

	def __StartGame(self):
		background.SetViewDistanceSet(background.DISTANCE0, 25600)
		"""
		background.SetViewDistanceSet(background.DISTANCE1, 19200)
		background.SetViewDistanceSet(background.DISTANCE2, 12800)
		background.SetViewDistanceSet(background.DISTANCE3, 9600)
		background.SetViewDistanceSet(background.DISTANCE4, 6400)
		"""
		background.SelectViewDistanceNum(background.DISTANCE0)

		app.SetGlobalCenterPosition(self.playerX, self.playerY)

		net.StartGame()
