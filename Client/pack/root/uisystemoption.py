import dbg
import ui
import snd
import systemSetting
import net
import chat
import app
import localeInfo
import constInfo
import chrmgr
import player
import musicInfo

import uiSelectMusic
import background
if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
	import uiPhaseCurtain

import uiCommon

import sys
import uiScriptLocale
import wndMgr
if app.ENABLE_GRAPHIC_ON_OFF:
	import grp

MUSIC_FILENAME_MAX_LEN = 25
if app.ENABLE_LOCALE_CLIENT:
	LANGUAGE_MAX_LINE = 5

blockMode = 0

class OptionDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()

		if app.ENABLE_LOCALE_CLIENT:
			self.__LoadLocaSettingFile()
			self.__LoadLocaleListFile()

		self.__Load()

		if app.ENABLE_LOCALE_CLIENT:
			self.__CreateLanguageSelectWindow()

		if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
			self.curtain = uiPhaseCurtain.PhaseCurtain()
			self.curtain.speed = 0.03
			self.curtain.Hide()

		self.RefreshCameraMode()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		#print " -------------------------------------- DELETE SYSTEM OPTION DIALOG"

	def __Initialize(self):
		if not app.ENABLE_DISABLE_SOFTWARE_TILING:
			self.tilingMode = 0
		self.titleBar = 0
		self.changeMusicButton = 0
		self.selectMusicFile = 0
		self.ctrlMusicVolume = 0
		self.ctrlSoundVolume = 0
		self.musicListDlg = 0
		if not app.ENABLE_DISABLE_SOFTWARE_TILING:
			self.tilingApplyButton = 0
		self.cameraModeButtonList = []
		self.fogModeButtonList = []
		if not app.ENABLE_DISABLE_SOFTWARE_TILING:
			self.tilingModeButtonList = []
		self.ctrlShadowQuality = 0

		if app.ENABLE_FOG_FIX:
			self.fogButtonList = []

		if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
			self.nightModeButtonList = []
			self.snowModeButtonList = []
			self.snowTextureModeButtonList = []

		if app.ENABLE_SHADOW_RENDER_QUALITY_OPTION:
			self.shadowQualityButtonList = []
			self.shadowTargetButtonList = []

		if app.ENABLE_GRAPHIC_ON_OFF:
			self.effectOnOffButtonList = []
			self.effectApplyButton = None
			self.effectLevelIndex = None

			self.privateShopOnOffButtonList = []
			self.privateShopApplyButton = None
			self.privateShopLevelIndex = None

			self.dropItemOnOffButtonList = []
			self.dropItemApplyButton = None
			self.dropItemLevelIndex = None

			self.petOnOffButtonList = []
			self.npcNameOnOffButtonList = []

		if app.ENABLE_FOV_OPTION:
			self.fovController = None
			self.fovResetButton = None
			self.fovValueText = None

		if app.ENABLE_LOCALE_CLIENT:
			self.locale_list = []

			self.cur_code_page = app.GetDefaultCodePage()
			self.cur_locale = app.GetLocaleName()

			self.language_select_window = None
			self.language_select_window_width = 0
			self.language_select_window_height = 0

			self.language_select_list = []
			self.language_select_list_open = 0
			self.language_select_index = -1

			self.language_mouse_over_image = None

			self.language_scroll_bar = None
			self.language_scroll_bar_pos = 0
			self.language_scroll_bar_diff = 0

			self.language_change_window = None
			self.cur_language_text_window = None
			self.cur_language_text = None
			self.language_select_button = None
			self.language_change_button = None
			self.language_select_pivot_window = None

		self.questionDialog = None

		self.IsShow = False

	def Destroy(self):
		self.ClearDictionary()

		self.__Initialize()
		#print " -------------------------------------- DESTROY SYSTEM OPTION DIALOG"

	def __Load_LoadScript(self, fileName):
		try:
			pyScriptLoader = ui.PythonScriptLoader()
			pyScriptLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("System.OptionDialog.__Load_LoadScript")

	def __Load_BindObject(self):
		try:
			GetObject = self.GetChild
			self.titleBar = GetObject("titlebar")
			self.selectMusicFile = GetObject("bgm_file")
			self.changeMusicButton = GetObject("bgm_button")
			self.ctrlMusicVolume = GetObject("music_volume_controller")
			self.ctrlSoundVolume = GetObject("sound_volume_controller")
			self.cameraModeButtonList.append(GetObject("camera_short"))
			self.cameraModeButtonList.append(GetObject("camera_long"))

			if app.ENABLE_FOG_FIX:
				self.fogButtonList.append(GetObject("fog_off"))
				self.fogButtonList.append(GetObject("fog_on"))
			else:
				self.fogModeButtonList.append(GetObject("fog_level0"))
				self.fogModeButtonList.append(GetObject("fog_level1"))
				self.fogModeButtonList.append(GetObject("fog_level2"))

			if not app.ENABLE_DISABLE_SOFTWARE_TILING:
				self.tilingModeButtonList.append(GetObject("tiling_cpu"))
				self.tilingModeButtonList.append(GetObject("tiling_gpu"))
				self.tilingApplyButton = GetObject("tiling_apply")

			if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
				self.nightModeButtonList.append(GetObject("night_mode_off"))
				self.nightModeButtonList.append(GetObject("night_mode_on"))
				self.snowModeButtonList.append(GetObject("snow_mode_off"))
				self.snowModeButtonList.append(GetObject("snow_mode_on"))
				self.snowTextureModeButtonList.append(GetObject("snow_texture_mode_off"))
				self.snowTextureModeButtonList.append(GetObject("snow_texture_mode_on"))

			if app.ENABLE_GRAPHIC_ON_OFF:
				for i in xrange(1, 6):
					self.effectOnOffButtonList.append(GetObject("effect_level%d" % i))
					self.privateShopOnOffButtonList.append(GetObject("privateShop_level%d" % i))
					self.dropItemOnOffButtonList.append(GetObject("dropItem_level%d" % i))

				self.effectApplyButton = GetObject("effect_apply")
				self.privateShopApplyButton = GetObject("privateShop_apply")
				self.dropItemApplyButton = GetObject("dropItem_apply")

				self.petOnOffButtonList.append(GetObject("pet_on"))
				self.petOnOffButtonList.append(GetObject("pet_off"))

				self.npcNameOnOffButtonList.append(GetObject("npcName_on"))
				self.npcNameOnOffButtonList.append(GetObject("npcName_off"))

			if app.ENABLE_FOV_OPTION:
				self.fovController = GetObject("fov_controller")
				self.fovController.SetButtonVisual("d:/ymir work/ui/game/windows/",\
					"sliderbar_cursor_button01.tga",\
					"sliderbar_cursor_button01.tga",\
					"sliderbar_cursor_button01.tga")
				self.fovController.SetBackgroundVisual("d:/ymir work/ui/game/windows/sliderbar_small.tga")
				self.fovResetButton = GetObject("fov_reset_button")
				self.fovValueText = GetObject("fov_value_text")

				if localeInfo.IsARABIC():
					self.fovController.SetPosition(234, 5)

			if app.ENABLE_LOCALE_CLIENT:
				self.language_change_window = GetObject("language_change_window")
				self.cur_language_text_window = GetObject("cur_language_text_window")
				self.cur_language_text = GetObject("cur_language_text")
				self.language_select_button = GetObject("language_select_button")
				self.language_change_button = GetObject("language_change_button")
				self.language_select_pivot_window = GetObject("language_select_pivot_window")

			if app.ENABLE_SHADOW_RENDER_QUALITY_OPTION:
				self.shadowQualityButtonList.append(GetObject("shadow_quality_bad"))
				self.shadowQualityButtonList.append(GetObject("shadow_quality_average"))
				self.shadowQualityButtonList.append(GetObject("shadow_quality_good"))

				self.shadowTargetButtonList.append(GetObject("shadow_target_none"))
				self.shadowTargetButtonList.append(GetObject("shadow_target_ground"))
				self.shadowTargetButtonList.append(GetObject("shadow_target_ground_and_solo"))
				self.shadowTargetButtonList.append(GetObject("shadow_target_all"))

			#self.ctrlShadowQuality = GetObject("shadow_bar")
		except:
			import exception
			exception.Abort("OptionDialog.__Load_BindObject")

	def __Load(self):
		self.__Load_LoadScript("uiscript/systemoptiondialog.py")
		self.__Load_BindObject()

		self.SetCenterPosition()

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		self.ctrlMusicVolume.SetSliderPos(float(systemSetting.GetMusicVolume()))
		self.ctrlMusicVolume.SetEvent(ui.__mem_func__(self.OnChangeMusicVolume))

		self.ctrlSoundVolume.SetSliderPos(float(systemSetting.GetSoundVolume()) / 5.0)
		self.ctrlSoundVolume.SetEvent(ui.__mem_func__(self.OnChangeSoundVolume))

		#self.ctrlShadowQuality.SetSliderPos(float(systemSetting.GetShadowLevel()) / 5.0)
		#self.ctrlShadowQuality.SetEvent(ui.__mem_func__(self.OnChangeShadowQuality))

		self.changeMusicButton.SAFE_SetEvent(self.__OnClickChangeMusicButton)

		self.cameraModeButtonList[0].SAFE_SetEvent(self.__OnClickCameraModeShortButton)
		self.cameraModeButtonList[1].SAFE_SetEvent(self.__OnClickCameraModeLongButton)

		if app.ENABLE_FOG_FIX:
			self.fogButtonList[0].SAFE_SetEvent(self.__OnClickFogModeOffButton)
			self.fogButtonList[1].SAFE_SetEvent(self.__OnClickFogModeOnButton)
		else:
			self.fogModeButtonList[0].SAFE_SetEvent(self.__OnClickFogModeLevel0Button)
			self.fogModeButtonList[1].SAFE_SetEvent(self.__OnClickFogModeLevel1Button)
			self.fogModeButtonList[2].SAFE_SetEvent(self.__OnClickFogModeLevel2Button)

		if not app.ENABLE_DISABLE_SOFTWARE_TILING:
			self.tilingModeButtonList[0].SAFE_SetEvent(self.__OnClickTilingModeCPUButton)
			self.tilingModeButtonList[1].SAFE_SetEvent(self.__OnClickTilingModeGPUButton)

			self.tilingApplyButton.SAFE_SetEvent(self.__OnClickTilingApplyButton)

			self.__SetCurTilingMode()

		self.__ClickRadioButton(self.fogModeButtonList, constInfo.GET_FOG_LEVEL_INDEX())
		self.__ClickRadioButton(self.cameraModeButtonList, constInfo.GET_CAMERA_MAX_DISTANCE_INDEX())

		if app.ENABLE_FOG_FIX:
			self.__ClickRadioButton(self.fogButtonList, background.GetFogMode())

		if app.ENABLE_SHADOW_RENDER_QUALITY_OPTION:
			self.shadowQualityButtonList[background.SHADOW_QUALITY_BAD].SAFE_SetEvent(self.__OnClickChangeShadowQuality, background.SHADOW_QUALITY_BAD)
			self.shadowQualityButtonList[background.SHADOW_QUALITY_AVERAGE].SAFE_SetEvent(self.__OnClickChangeShadowQuality, background.SHADOW_QUALITY_AVERAGE)
			self.shadowQualityButtonList[background.SHADOW_QUALITY_GOOD].SAFE_SetEvent(self.__OnClickChangeShadowQuality, background.SHADOW_QUALITY_GOOD)
			self.__ClickRadioButton(self.shadowQualityButtonList, systemSetting.GetShadowQualityLevel())

			self.shadowTargetButtonList[background.SHADOW_TARGET_NONE].SAFE_SetEvent(self.__OnClickChangeShadowTarget, background.SHADOW_TARGET_NONE)
			self.shadowTargetButtonList[background.SHADOW_TARGET_GROUND].SAFE_SetEvent(self.__OnClickChangeShadowTarget, background.SHADOW_TARGET_GROUND)
			self.shadowTargetButtonList[background.SHADOW_TARGET_GROUND_AND_SOLO].SAFE_SetEvent(self.__OnClickChangeShadowTarget, background.SHADOW_TARGET_GROUND_AND_SOLO)
			self.shadowTargetButtonList[background.SHADOW_TARGET_ALL].SAFE_SetEvent(self.__OnClickChangeShadowTarget, background.SHADOW_TARGET_ALL)
			self.__ClickRadioButton(self.shadowTargetButtonList, systemSetting.GetShadowTargetLevel())

		if app.ENABLE_GRAPHIC_ON_OFF:
			self.__ClickRadioButton(self.effectOnOffButtonList, grp.GetEffectOnOffLevel())
			self.__ClickRadioButton(self.privateShopOnOffButtonList, grp.GetPrivateShopOnOffLevel())
			self.__ClickRadioButton(self.dropItemOnOffButtonList, grp.GetDropItemOnOffLevel())

			self.__ClickRadioButton(self.petOnOffButtonList, grp.GetPetOnOffStatus())
			self.__ClickRadioButton(self.npcNameOnOffButtonList, grp.GetNPCNameOnOffStatus())

		if app.ENABLE_FOV_OPTION:
			if self.fovController:
				self.fovController.SetSliderPos(float(systemSetting.GetFOV()) / float(app.MAX_CAMERA_PERSPECTIVE))
				self.fovController.SetEvent(ui.__mem_func__(self.__OnChangeFOV))

			if self.fovValueText:
				self.fovValueText.SetText(str(int(systemSetting.GetFOV())))

			if self.fovResetButton:
				self.fovResetButton.SetEvent(ui.__mem_func__(self.__OnClickFOVResetButton))

		if app.ENABLE_LOCALE_CLIENT:
			self.cur_language_text_window.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.__OnClickLanguageSelectButton))
			self.language_select_button.SetEvent(ui.__mem_func__(self.__OnClickLanguageSelectButton))
			self.language_change_button.SetEvent(ui.__mem_func__(self.__OnClickLanguageChangeButton))

		if musicInfo.fieldMusic == musicInfo.METIN2THEMA:
			self.selectMusicFile.SetText(uiSelectMusic.DEFAULT_THEMA)
		else:
			self.selectMusicFile.SetText(musicInfo.fieldMusic[:MUSIC_FILENAME_MAX_LEN])

		if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
			self.nightModeButtonList[0].SAFE_SetEvent(self.__OnClickNightModeOffButton)
			self.nightModeButtonList[1].SAFE_SetEvent(self.__OnClickNightModeOnButton)
			self.__InitNightModeOption()

			self.snowModeButtonList[0].SAFE_SetEvent(self.__OnClickSnowModeOffButton)
			self.snowModeButtonList[1].SAFE_SetEvent(self.__OnClickSnowModeOnButton)
			self.__InitSnowModeOption()

			self.snowTextureModeButtonList[0].SAFE_SetEvent(self.__OnClickSnowTextureModeOffButton)
			self.snowTextureModeButtonList[1].SAFE_SetEvent(self.__OnClickSnowTextureModeOnButton)
			self.__InitSnowTextureModeOption()

		if app.ENABLE_GRAPHIC_ON_OFF:
			for i in range(5):
				self.effectOnOffButtonList[i].SAFE_SetEvent(self.__OnClickEffectLevelButton, i)
				self.privateShopOnOffButtonList[i].SAFE_SetEvent(self.__OnClickPrivateShopLevelButton, i)
				self.dropItemOnOffButtonList[i].SAFE_SetEvent(self.__OnClickDropItemLevelButton, i)

			self.effectApplyButton.SAFE_SetEvent(self.__OnClickEffectApplyButton)
			self.privateShopApplyButton.SAFE_SetEvent(self.__OnClickPrivateShopApplyButton)
			self.dropItemApplyButton.SAFE_SetEvent(self.__OnClickDropItemApplyButton)

			self.petOnOffButtonList[0].SAFE_SetEvent(self.__OnClickPetButton, 0)
			self.petOnOffButtonList[1].SAFE_SetEvent(self.__OnClickPetButton, 1)

			self.npcNameOnOffButtonList[0].SAFE_SetEvent(self.__OnClickNPCNameButton, 0)
			self.npcNameOnOffButtonList[1].SAFE_SetEvent(self.__OnClickNPCNameButton, 1)

	if not app.ENABLE_DISABLE_SOFTWARE_TILING:
		def __OnClickTilingModeCPUButton(self):
			self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_CPU_TILING_1)
			self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_CPU_TILING_2)
			self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_CPU_TILING_3)
			self.__SetTilingMode(0)

		def __OnClickTilingModeGPUButton(self):
			self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_GPU_TILING_1)
			self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_GPU_TILING_2)
			self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_GPU_TILING_3)
			self.__SetTilingMode(1)

		def __OnClickTilingApplyButton(self):
			self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_TILING_EXIT)
			if 0 == self.tilingMode:
				background.EnableSoftwareTiling(1)
			else:
				background.EnableSoftwareTiling(0)

			net.ExitGame()

	def __OnClickChangeMusicButton(self):
		if not self.musicListDlg:
			self.musicListDlg = uiSelectMusic.FileListDialog()
			self.musicListDlg.SAFE_SetSelectEvent(self.__OnChangeMusic)

		self.musicListDlg.Open()

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			selButton = buttonList[buttonIndex]
		except IndexError:
			return

		for eachButton in buttonList:
			eachButton.SetUp()

		selButton.Down()

	def __SetTilingMode(self, index):
		self.__ClickRadioButton(self.tilingModeButtonList, index)
		self.tilingMode = index

	def RefreshCameraMode(self):
		index = systemSetting.GetCameraMode()
		constInfo.SET_CAMERA_MAX_DISTANCE_INDEX(index)
		self.__ClickRadioButton(self.cameraModeButtonList, index)
	
	def __SetCameraMode(self, index):
		systemSetting.SetCameraMode(index)
		self.RefreshCameraMode()

	def __SetFogLevel(self, index):
		constInfo.SET_FOG_LEVEL_INDEX(index)
		self.__ClickRadioButton(self.fogModeButtonList, index)

	def __OnClickCameraModeShortButton(self):
		self.__SetCameraMode(0)

	def __OnClickCameraModeLongButton(self):
		self.__SetCameraMode(1)

	def __OnClickFogModeLevel0Button(self):
		self.__SetFogLevel(0)

	def __OnClickFogModeLevel1Button(self):
		self.__SetFogLevel(1)

	def __OnClickFogModeLevel2Button(self):
		self.__SetFogLevel(2)

	if app.ENABLE_FOG_FIX:
		def __OnClickFogModeOnButton(self):
			background.SetFogMode(True)
			self.__ClickRadioButton(self.fogButtonList, 1)

		def __OnClickFogModeOffButton(self):
			background.SetFogMode(False)
			self.__ClickRadioButton(self.fogButtonList, 0)

	def __OnChangeMusic(self, fileName):
		self.selectMusicFile.SetText(fileName[:MUSIC_FILENAME_MAX_LEN])

		if musicInfo.fieldMusic != "":
			snd.FadeOutMusic("BGM/"+ musicInfo.fieldMusic)

		if fileName == uiSelectMusic.DEFAULT_THEMA:
			musicInfo.fieldMusic = musicInfo.METIN2THEMA
		else:
			musicInfo.fieldMusic = fileName

		musicInfo.SaveLastPlayFieldMusic()

		if musicInfo.fieldMusic != "":
			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)

	if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
		def __InitNightModeOption(self):
			self.NightModeOn = systemSetting.GetNightModeOption()
			self.__ClickRadioButton(self.nightModeButtonList, self.NightModeOn)
			background.SetNightModeOption(self.NightModeOn)

		def __InitSnowModeOption(self):
			self.SnowModeOn = systemSetting.GetSnowModeOption()
			self.__ClickRadioButton(self.snowModeButtonList, self.SnowModeOn)
			background.SetSnowModeOption(self.SnowModeOn)

		def __InitSnowTextureModeOption(self):
			self.SnowTextureModeOn = systemSetting.GetSnowTextureModeOption()
			self.__ClickRadioButton(self.snowTextureModeButtonList, self.SnowTextureModeOn)
			background.SetSnowTextureModeOption(self.SnowTextureModeOn)

		def __OnClickNightModeOffButton(self):
			self.__ClickRadioButton(self.nightModeButtonList, 0)
			self.__SetNightMode(0)

		def __OnClickNightModeOnButton(self):
			self.__ClickRadioButton(self.nightModeButtonList, 1)
			self.__SetNightMode(1)

		def __OnClickSnowModeOffButton(self):
			self.__ClickRadioButton(self.snowModeButtonList, 0)
			self.__SetSnowMode(0)

		def __OnClickSnowModeOnButton(self):
			self.__ClickRadioButton(self.snowModeButtonList, 1)
			self.__SetSnowMode(1)

		def __OnClickSnowTextureModeOffButton(self):
			self.__ClickRadioButton(self.snowTextureModeButtonList, 0)
			self.__SetSnowTextureMode(0)

		def __OnClickSnowTextureModeOnButton(self):
			self.__ClickRadioButton(self.snowTextureModeButtonList, 1)
			self.__SetSnowTextureMode(1)

		def __SetSnowMode(self, index):
			systemSetting.SetSnowModeOption(index)
			background.SetSnowModeOption(index)
			background.EnableSnowMode(index)

		def __SetSnowTextureMode(self, index):
			systemSetting.SetSnowTextureModeOption(index)
			background.SetSnowTextureModeOption(index)
			background.EnableSnowTextureMode()

		def __SetNightMode(self, index):
			systemSetting.SetNightModeOption(index)
			background.SetNightModeOption(index)

			if not background.GetDayMode():
				return

			if not background.IsBoomMap():
				return

			if 1 == index:
				self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToDark)
			else:
				self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToLight)

		def __DayMode_OnCompleteChangeToLight(self):
			background.ChangeEnvironmentData(background.DAY_MODE_LIGHT)
			self.curtain.FadeIn()

		def __DayMode_OnCompleteChangeToDark(self):
			background.RegisterEnvironmentData(background.DAY_MODE_DARK, constInfo.ENVIRONMENT_NIGHT)
			background.ChangeEnvironmentData(background.DAY_MODE_DARK)
			self.curtain.FadeIn()

	if app.ENABLE_SHADOW_RENDER_QUALITY_OPTION:
		def __OnClickChangeShadowQuality(self, shadow_quality):
			self.__ClickRadioButton(self.shadowQualityButtonList, shadow_quality)
			self.__SetShadowQualityLevel(shadow_quality)

		def __SetShadowQualityLevel(self, index):
			systemSetting.SetShadowQualityLevel(index)
			background.SetShadowQualityLevel(index)

		def __OnClickChangeShadowTarget(self, shadow_target):
			self.__ClickRadioButton(self.shadowTargetButtonList, shadow_target)
			self.__SetShadowTargetLevel(shadow_target)

		def __SetShadowTargetLevel(self, index):
			systemSetting.SetShadowTargetLevel(index)
			background.SetShadowTargetLevel(index)

	if app.ENABLE_GRAPHIC_ON_OFF:
		# Effect
		def __OnClickEffectLevelButton(self, effectLevelIdx):
			self.__ClickRadioButton(self.effectOnOffButtonList, effectLevelIdx)
			self.effectLevelIndex = effectLevelIdx

		def __OnClickEffectApplyButton(self):
			grp.SetEffectOnOffLevel(self.effectLevelIndex)

		# PrivateShop
		def __OnClickPrivateShopLevelButton(self, privateShopLevelIdx):
			self.__ClickRadioButton(self.privateShopOnOffButtonList, privateShopLevelIdx)
			self.privateShopLevelIndex = privateShopLevelIdx

		def __OnClickPrivateShopApplyButton(self):
			grp.SetPrivateShopOnOffLevel(self.privateShopLevelIndex)

		# DropItem
		def __OnClickDropItemLevelButton(self, dropItemLevelIdx):
			self.__ClickRadioButton(self.dropItemOnOffButtonList, dropItemLevelIdx)
			self.dropItemLevelIndex = dropItemLevelIdx

		def __OnClickDropItemApplyButton(self):
			grp.SetDropItemOnOffLevel(self.dropItemLevelIndex)

		# Pet
		def __OnClickPetButton(self, buttonIndex):
			self.__ClickRadioButton(self.petOnOffButtonList, buttonIndex)
			grp.SetPetOnOffStatus(buttonIndex)

		# NPC
		def __OnClickNPCNameButton(self, buttonIndex):
			self.__ClickRadioButton(self.npcNameOnOffButtonList, buttonIndex)
			grp.SetNPCNameOnOffStatus(buttonIndex)

	if app.ENABLE_FOV_OPTION:
		def __OnChangeFOV(self):
			pos = self.fovController.GetSliderPos()
			systemSetting.SetFOV(pos * float(app.MAX_CAMERA_PERSPECTIVE))

			if self.fovValueText:
				self.fovValueText.SetText(str(int(systemSetting.GetFOV())))

		def __OnClickFOVResetButton(self):
			self.fovController.SetSliderPos(float(app.DEFAULT_CAMERA_PERSPECTIVE) / float(app.MAX_CAMERA_PERSPECTIVE))
			systemSetting.SetFOV(float(app.DEFAULT_CAMERA_PERSPECTIVE))

			if self.fovValueText:
				self.fovValueText.SetText(str(int(systemSetting.GetFOV())))

	def OnChangeMusicVolume(self):
		pos = self.ctrlMusicVolume.GetSliderPos()
		snd.SetMusicVolume(pos * net.GetFieldMusicVolume())
		systemSetting.SetMusicVolume(pos)

	def OnChangeSoundVolume(self):
		pos = self.ctrlSoundVolume.GetSliderPos()
		snd.SetSoundVolumef(pos)
		systemSetting.SetSoundVolumef(pos)

	def OnChangeShadowQuality(self):
		pos = self.ctrlShadowQuality.GetSliderPos()
		systemSetting.SetShadowLevel(int(pos / 0.2))

	def OnCloseQuestionDialog(self):
		self.questionDialog.Close()
		self.questionDialog = None
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Show(self):
		ui.ScriptWindow.Show(self)
		self.IsShow = True

	def IsShowWindow(self):
		return self.IsShow

	def Close(self):
		if not app.ENABLE_DISABLE_SOFTWARE_TILING:
			self.__SetCurTilingMode()
		self.Hide()
		self.IsShow = False

		if self.questionDialog:
			self.OnCloseQuestionDialog()

		if app.ENABLE_LOCALE_CLIENT:
			self.__LanguageSelectShowHide(False)

	if not app.ENABLE_DISABLE_SOFTWARE_TILING:
		def __SetCurTilingMode(self):
			if background.IsSoftwareTiling():
				self.__SetTilingMode(0)
			else:
				self.__SetTilingMode(1)

	def __NotifyChatLine(self, text):
		chat.AppendChat(chat.CHAT_TYPE_INFO, text)

	if app.ENABLE_LOCALE_CLIENT:
		def __LoadLocaSettingFile(self):
			try:
				lines = open("loca.cfg", "r").readlines()
			except:
				import dbg
				dbg.LogBox("LoadLocaSettingFile(loca.cfg)")
				app.Abort()

			try:
				line = lines[0]
				tokens = line.split()

				if len(tokens) == 2:
					code_page_str, locale = tokens
					code_page = int(code_page_str)

					self.cur_code_page = code_page
					self.cur_locale = locale
				else:
					raise RuntimeError, "Unknown TokenSize"

			except:
				import dbg
				dbg.LogBox("%s: %s" % ("loca.cfg", line), "Error")
				raise

		def __LoadLocaleListFile(self):
			try:
				lines = open("locale_list.txt", "r").readlines()
			except:
				import dbg
				dbg.LogBox("LoadLocaleListFile(locale_list.txt)")
				app.Abort()

			lineIndex = 1

			for line in lines:
				try:
					line = line.strip()
					tokens = line.split(" ")

					if len(tokens) == 3:
						language, code_page_str, locale = tokens
						code_page = int(code_page_str)

						language_name = getattr(uiScriptLocale, "LANGUAGE_" + locale.upper(), None)
						if language_name:
							language = language_name

						self.locale_list.append({ "language" : language, "code_page" : code_page, "locale" : locale })
					else:
						raise RuntimeError, "Unknown TokenSize"

					lineIndex += 1

				except:
					import dbg
					dbg.LogBox("%s: line(%d): %s" % ("locale_list.txt", lineIndex, line), "Error")
					raise

		def __CreateLanguageSelectWindow(self):
			if self.language_select_list:
				return

			self.cur_language_text.SetText(self.__GetStringCurLanguage())

			button_height = 15
			button_count = min(LANGUAGE_MAX_LINE, len(self.locale_list))

			self.language_select_window_width = self.language_select_pivot_window.GetWidth()
			self.language_select_window_height = button_count * button_height

			self.language_select_window = ui.Window()
			self.language_select_window.AddFlag("float")
			self.language_select_window.SetSize(self.language_select_window_width, self.language_select_window_height)
			self.language_select_window.Hide()

			(x, y) = self.language_select_pivot_window.GetGlobalPosition()
			self.language_select_window.SetPosition(x, y)

			for index in range(button_count):
				button = ui.Button()
				button.SetParent(self.language_select_window)
				button.SetPosition(0, button_height * index)

				if index == 0:
					button.SetUpVisual("d:/ymir work/ui/quest_re/button_top.sub")
					button.SetDownVisual("d:/ymir work/ui/quest_re/button_top.sub")
					button.SetOverVisual("d:/ymir work/ui/quest_re/button_top.sub")
				elif index == (button_count - 1):
					button.SetUpVisual("d:/ymir work/ui/quest_re/button_bottom.sub")
					button.SetDownVisual("d:/ymir work/ui/quest_re/button_bottom.sub")
					button.SetOverVisual("d:/ymir work/ui/quest_re/button_bottom.sub")
				else:
					button.SetUpVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetDownVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetOverVisual("d:/ymir work/ui/quest_re/button_middle.sub")

				button.SetEvent(ui.__mem_func__(self.__OnClickLanguageSelect), index)
				button.SetOverEvent(ui.__mem_func__(self.__OnClickLanguageButtonOver), index)
				button.SetOverOutEvent(ui.__mem_func__(self.__OnClickLanguageButtonOverOut), index)
				button.SetText(self.locale_list[index]["language"])
				button.Show()

				self.language_select_list.append(button)

			self.language_mouse_over_image = ui.ImageBox()
			self.language_mouse_over_image.SetParent(self.language_select_window)
			self.language_mouse_over_image.AddFlag("not_pick")
			self.language_mouse_over_image.LoadImage("d:/ymir work/ui/quest_re/button_over.sub")
			self.language_mouse_over_image.Hide()

			self.language_scroll_bar = ui.ScrollBar()
			self.language_scroll_bar.SetParent(self.language_select_window)
			self.language_scroll_bar.AddFlag("float")
			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				self.language_scroll_bar.SetUpButtonUpVisual("d:/ymir work/ui/game/mailbox/scroll_up_arrow_button_default.sub")
				self.language_scroll_bar.SetUpButtonOverVisual("d:/ymir work/ui/game/mailbox/scroll_up_arrow_button_over.sub")
				self.language_scroll_bar.SetUpButtonDownVisual("d:/ymir work/ui/game/mailbox/scroll_up_arrow_button_down.sub")
				self.language_scroll_bar.SetDownButtonUpVisual("d:/ymir work/ui/game/mailbox/scroll_down_arrow_button_default.sub")
				self.language_scroll_bar.SetDownButtonOverVisual("d:/ymir work/ui/game/mailbox/scroll_down_arrow_button_over.sub")
				self.language_scroll_bar.SetDownButtonDownVisual("d:/ymir work/ui/game/mailbox/scroll_down_arrow_button_down.sub")
			self.language_scroll_bar.SetScrollBarSize(self.language_select_window_height)
			self.language_scroll_bar.SetPosition(self.language_select_window_width - 16, 0)
			self.language_scroll_bar.SetScrollEvent(ui.__mem_func__(self.__OnLanguageSelectScroll))
			self.language_scroll_bar.Hide()

			self.language_scroll_bar_diff = len(self.locale_list) - LANGUAGE_MAX_LINE
			if self.language_scroll_bar_diff > 0:
				scroll_step_size = 1.0 / self.language_scroll_bar_diff
				self.language_scroll_bar.SetScrollStep(scroll_step_size)

			if button_count >= LANGUAGE_MAX_LINE:
				self.language_scroll_bar.Show()
			else:
				self.language_scroll_bar.Hide()

			self.__AdjustLanguageSelectWindowPosition()

		def __OnClickLanguageSelect(self, index):
			if index >= len(self.locale_list):
				return

			self.__LanguageSelectShowHide(False)

			if self.cur_language_text:
				self.cur_language_text.SetText(self.locale_list[index]["language"])

			self.language_select_index = index

		def __OnClickLanguageButtonOver(self, index):
			if index >= len(self.language_select_list):
				return

			button = self.language_select_list[index]
			(x, y) = button.GetLocalPosition()

			self.language_mouse_over_image.SetPosition(x, y)
			self.language_mouse_over_image.Show()

		def __OnClickLanguageButtonOverOut(self, index):
			self.language_mouse_over_image.Hide()

		def __OnLanguageSelectScroll(self):
			self.language_scroll_bar_pos = int(self.language_scroll_bar.GetPos() * self.language_scroll_bar_diff)

			for index in xrange(len(self.language_select_list)):
				pos = index + self.language_scroll_bar_pos
				if pos >= len(self.locale_list):
					return

				self.language_select_list[index].SetText(self.locale_list[pos]["language"])
				self.language_select_list[index].SetEvent(ui.__mem_func__(self.__OnClickLanguageSelect), pos)
				self.language_select_list[index].SetOverEvent(ui.__mem_func__(self.__OnClickLanguageButtonOver), index)
				self.language_select_list[index].SetOverOutEvent(ui.__mem_func__(self.__OnClickLanguageButtonOverOut), index)

		def __AdjustLanguageSelectWindowPosition(self):
			if self.language_select_window and self.language_select_pivot_window:
				(x, y) = self.language_select_pivot_window.GetGlobalPosition()
				self.language_select_window.SetPosition(x, y)

		def __OnClickLanguageSelectButton(self):
			self.__CreateLanguageSelectWindow()

			if self.language_select_list_open:
				self.__LanguageSelectShowHide(False)
			else:
				self.__LanguageSelectShowHide(True)

		def __OnClickLanguageChangeButton(self):
			if self.__GetStringCurLanguage() == self.cur_language_text.GetText():
				return

			if self.language_select_index != -1:
				net.ExitGameLanguageChange()

		def __LanguageSelectShowHide(self, is_show):
			self.language_select_list_open = is_show

			if True == is_show:
				self.language_select_window.Show()
			else:
				self.language_select_window.Hide()

		def __GetCurLanguageKey(self):
			for index, lang in enumerate(self.locale_list):
				if lang["locale"] == self.cur_locale and lang["code_page"] == self.cur_code_page:
					return index
			return -1

		def __GetStringCurLanguage(self):
			for lang in self.locale_list:
				if lang["locale"] == self.cur_locale and lang["code_page"] == self.cur_code_page:
					return lang["language"]
			return "-"

		def __SaveLoca(self, code_page, locale):
			try:
				with open("loca.cfg", "wt") as file:
					file.write("%d %s" % (code_page, locale))
			except:
				import dbg
				dbg.LogBox("SaveLoca(loca.cfg)")
				app.Abort()

		def LanguageChange(self):
			if self.language_select_index == -1:
				return

			if self.language_select_index >= len(self.locale_list):
				return

			loca = self.locale_list[self.language_select_index]
			self.__SaveLoca(loca["code_page"], loca["locale"])

			app.SetReloadLocale(True)

		def OnTop(self):
			if self.language_select_window:
				self.language_select_window.SetTop()

		def OnMoveWindow(self, x, y):
			self.__AdjustLanguageSelectWindowPosition()

		if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
			def OnMouseWheelButtonUp(self):
				return self.language_scroll_bar.OnUp() if self.language_scroll_bar else False

			def OnMouseWheelButtonDown(self):
				return self.language_scroll_bar.OnDown() if self.language_scroll_bar else False
