import app
import ui
import grp
import net
import guild
import messenger
import localeInfo
import constInfo
import uiToolTip
import uiGameOption

import uiCommon
from _weakref import proxy

if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
	import wndMgr

FRIEND = 0
GUILD = 1
if app.ENABLE_MESSENGER_GM:
	GM = 2
if app.ENABLE_MESSENGER_BLOCK:
	BLOCK = 3

class MessengerItem(ui.Window):

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		COUNTRY_FLAG_IMAGE_PATH_DICT = {
			"AE" : "d:/ymir work/ui/intro/login/server_flag_AE.sub",
			"ALL" : "d:/ymir work/ui/intro/login/server_flag_ALL.sub",
			"BR" : "d:/ymir work/ui/intro/login/server_flag_BR.sub",
			"CZ" : "d:/ymir work/ui/intro/login/server_flag_CZ.sub",
			"DE" : "d:/ymir work/ui/intro/login/server_flag_DE.sub",
			"DK" : "d:/ymir work/ui/intro/login/server_flag_DK.sub",
			"ES" : "d:/ymir work/ui/intro/login/server_flag_ES.sub",
			"EU" : "d:/ymir work/ui/intro/login/server_flag_EU.sub",
			"FR" : "d:/ymir work/ui/intro/login/server_flag_FR.sub",
			"GR" : "d:/ymir work/ui/intro/login/server_flag_GR.sub",
			"HU" : "d:/ymir work/ui/intro/login/server_flag_HU.sub",
			"IT" : "d:/ymir work/ui/intro/login/server_flag_IT.sub",
			"KR" : "d:/ymir work/ui/intro/login/server_flag_KR.sub",
			"NL" : "d:/ymir work/ui/intro/login/server_flag_NL.sub",
			"PL" : "d:/ymir work/ui/intro/login/server_flag_PL.sub",
			"PT" : "d:/ymir work/ui/intro/login/server_flag_PT.sub",
			"RO" : "d:/ymir work/ui/intro/login/server_flag_RO.sub",
			"RU" : "d:/ymir work/ui/intro/login/server_flag_RU.sub",
			"SP1" : "d:/ymir work/ui/intro/login/server_flag_SP1.sub",
			"SP2" : "d:/ymir work/ui/intro/login/server_flag_SP2.sub",
			"SP3" : "d:/ymir work/ui/intro/login/server_flag_SP3.sub",
			"SP4" : "d:/ymir work/ui/intro/login/server_flag_SP4.sub",
			"SP5" : "d:/ymir work/ui/intro/login/server_flag_SP5.sub",
			"SP6" : "d:/ymir work/ui/intro/login/server_flag_SP6.sub",
			"SP7" : "d:/ymir work/ui/intro/login/server_flag_SP7.sub",
			"TR" : "d:/ymir work/ui/intro/login/server_flag_TR.sub",
			"UK" : "d:/ymir work/ui/intro/login/server_flag_UK.sub"
		}

	def __init__(self, getParentEvent):
		ui.Window.__init__(self)

		self.SetParent(getParentEvent())
		self.AddFlag("float")

		self.name = ""
		self.image = ui.ImageBox()
		self.image.AddFlag("not_pick")
		self.image.SetParent(self)
		self.image.Show()
		self.text = ui.TextLine()
		self.text.SetParent(self)
		if app.WJ_MULTI_TEXTLINE:
			self.text.DisableEnterToken()
		self.text.SetPosition(20, 2)
		self.text.Show()

		self.lovePoint = -1
		self.lovePointToolTip = None

		if app.ENABLE_MESSENGER_DETAILS:
			self.offlineTime = 0
			self.offlineTimeToolTip = None
			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				self.countryFlagImage = None

		self.isSelected = False

		self.getParentEvent = getParentEvent

	def __del__(self):
		ui.Window.__del__(self)

	def SetName(self, name):
		self.name = name
		if name:
			self.text.SetText(name)
			self.SetSize(20 + 6*len(name) + 4, 16)

			if localeInfo.IsARABIC():
				self.text.SetPosition(20 + 6*len(name) + 4, 2)

	if app.ENABLE_MESSENGER_DETAILS:
		def SetOfflineTime(self, offlineTime):
			self.offlineTime = offlineTime

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			def SetLanguage(self, country):
				if country:
					flag = country.upper()
				else:
					flag = ""

				if flag in self.COUNTRY_FLAG_IMAGE_PATH_DICT.keys():
					image = ui.ImageBox()
					image.SetParent(self)
					image.LoadImage(self.COUNTRY_FLAG_IMAGE_PATH_DICT[flag])
					image.SetScale(0.8, 0.8)
					image.SetPosition(-22, 0)
					image.Show()
					self.countryFlagImage = image

	def SetLovePoint(self, lovePoint):
		self.lovePoint = lovePoint

	def Select(self):
		self.isSelected = True

	def UnSelect(self):
		self.isSelected = False

	def GetName(self):
		return self.name

	def GetStepWidth(self):
		return 0

	# Whisper
	def CanWhisper(self):
		return False

	def IsOnline(self):
		return False

	def OnWhisper(self):
		pass

	# Remove
	def CanRemove(self):
		return False

	def OnRemove(self):
		return False

	# Warp
	def CanWarp(self):
		return False

	def OnWarp(self):
		pass

	def OnMouseOverIn(self):
		if -1 != self.lovePoint:
			if not self.lovePointToolTip:
				self.lovePointToolTip = uiToolTip.ToolTip(100)
				self.lovePointToolTip.SetTitle(self.name)
				self.lovePointToolTip.AppendTextLine(localeInfo.AFF_LOVE_POINT % (self.lovePoint))
				self.lovePointToolTip.ResizeToolTip()
			self.lovePointToolTip.ShowToolTip()

		if app.ENABLE_MESSENGER_DETAILS:
			if self.offlineTime != 0:
				offlineTime = app.GetGlobalTimeStamp() - self.offlineTime
				if offlineTime > 60:
					offlineTimeText = localeInfo.MESSENGER_OFFLINE_TIME_TOOLTIP % (localeInfo.SecondToDHM(offlineTime))

					tooltip = uiToolTip.ToolTip(11 * len(offlineTimeText))
					tooltip.SetTitle(self.name)
					tooltip.AppendTextLine(offlineTimeText)
					tooltip.ResizeToolTip()
					tooltip.ShowToolTip()
					self.offlineTimeToolTip = tooltip

	def OnMouseOverOut(self):
		if self.lovePointToolTip:
			self.lovePointToolTip.HideToolTip()

		if app.ENABLE_MESSENGER_DETAILS:
			if self.offlineTimeToolTip:
				self.offlineTimeToolTip.HideToolTip()

	def OnMouseLeftButtonDown(self):
		self.getParentEvent().OnSelectItem(self)

	def OnMouseLeftButtonDoubleClick(self):
		self.getParentEvent().OnDoubleClickItem(self)

	def OnRender(self):
		if self.isSelected:
			x, y = self.GetGlobalPosition()
			grp.SetColor(grp.GenerateColor(0.0, 0.0, 0.7, 0.7))
			grp.RenderBar(x+16, y, self.GetWidth()-16, self.GetHeight())

class MessengerMemberItem(MessengerItem):
	STATE_OFFLINE = 0
	STATE_ONLINE = 1

	IMAGE_FILE_NAME = {
		"ONLINE" : "d:/ymir work/ui/game/windows/messenger_list_online.sub",
		"OFFLINE" : "d:/ymir work/ui/game/windows/messenger_list_offline.sub",
	}

	def __init__(self, getParentEvent):
		MessengerItem.__init__(self, getParentEvent)
		self.key = None
		self.state = self.STATE_OFFLINE
		self.Offline()

	def __del__(self):
		MessengerItem.__del__(self)

	def GetStepWidth(self):
		return 15

	def SetKey(self, key):
		self.key = key

	def IsSameKey(self, key):
		return self.key == key

	def IsOnline(self):
		if self.STATE_ONLINE == self.state:
			return True

		return False

	def Online(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["ONLINE"])
		self.state = self.STATE_ONLINE

	def Offline(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["OFFLINE"])
		self.state = self.STATE_OFFLINE

	def CanWhisper(self):
		if self.IsOnline():
			return True

		return False

	def OnWhisper(self):
		if self.IsOnline():
			self.getParentEvent().whisperButtonEvent(self.GetName())

	def Select(self):
		MessengerItem.Select(self)

class MessengerGroupItem(MessengerItem):
	IMAGE_FILE_NAME = {
		"OPEN" : "d:/ymir work/ui/game/windows/messenger_list_open.sub",
		"CLOSE" : "d:/ymir work/ui/game/windows/messenger_list_close.sub",
	}

	def __init__(self, getParentEvent):
		self.isOpen = False
		self.memberList = []

		MessengerItem.__init__(self, getParentEvent)

	def __del__(self):
		MessengerItem.__del__(self)

	def AppendMember(self, member, key, name):
		member.SetKey(key)
		member.SetName(name)
		self.memberList.append(member)
		return member

	def RemoveMember(self, item):
		for i in xrange(len(self.memberList)):
			if item == self.memberList[i]:
				del self.memberList[i]
				return

	def ClearMember(self):
		self.memberList = []

	def FindMember(self, key):
		list = filter(lambda argMember, argKey = key: argMember.IsSameKey(argKey), self.memberList)
		if list:
			return list[0]

		return None

	def GetLoginMemberList(self):
		return filter(MessengerMemberItem.IsOnline, self.memberList)

	def GetLogoutMemberList(self):
		return filter(lambda arg: not arg.IsOnline(), self.memberList)

	def IsOpen(self):
		return self.isOpen

	def Open(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["OPEN"])
		self.isOpen = True

	def Close(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["CLOSE"])
		self.isOpen = False

		map(ui.Window.Hide, self.memberList)

	def Select(self):

		if self.IsOpen():
			self.Close()
		else:
			self.Open()

		MessengerItem.Select(self)
		self.getParentEvent().OnRefreshList()

class MessengerFriendItem(MessengerMemberItem):
	def __init__(self, getParentEvent):
		MessengerMemberItem.__init__(self, getParentEvent)

	def __del__(self):
		MessengerMemberItem.__del__(self)

	def CanRemove(self):
		return True

	def OnRemove(self):
		messenger.RemoveFriend(self.key)
		net.SendMessengerRemovePacket(self.key, self.name)
		return True

if app.ENABLE_MESSENGER_GM:
	class MessengerGMItem(MessengerMemberItem):
		def __init__(self, getParentEvent):
			MessengerMemberItem.__init__(self, getParentEvent)

		def CanRemove(self):
			return False

if app.ENABLE_MESSENGER_BLOCK:
	class MessengerBlockFriendItem(MessengerMemberItem):
		def __init__(self, getParentEvent):
			MessengerMemberItem.__init__(self, getParentEvent)

		def __del__(self):
			MessengerMemberItem.__del__(self)

		def CanRemove(self):
			return True

		def OnRemove(self):
			net.SendMessengerBlockRemovePacket(self.key, self.name)
			return True

	class MessengerBlockGroup(MessengerGroupItem):
		def __init__(self, getParentEvent):
			MessengerGroupItem.__init__(self, getParentEvent)
			self.SetName(localeInfo.MESSENGER_BLOCK)
			self.AddFlag("float")

		def __del__(self):
			MessengerGroupItem.__del__(self)

		def AppendMember(self, key, name):
			item = MessengerBlockFriendItem(self.getParentEvent)
			return MessengerGroupItem.AppendMember(self, item, key, name)

class MessengerGuildItem(MessengerMemberItem):
	def __init__(self, getParentEvent):
		MessengerMemberItem.__init__(self, getParentEvent)

	def __del__(self):
		MessengerMemberItem.__del__(self)

	def CanWarp(self):
		if not self.IsOnline():
			return False
		return True

	def OnWarp(self):
		net.SendGuildUseSkillPacket(155, self.key)

	def CanRemove(self):
		for i in xrange(guild.ENEMY_GUILD_SLOT_MAX_COUNT):
			if guild.GetEnemyGuildName(i) != "":
				return False

		if guild.MainPlayerHasAuthority(guild.AUTH_REMOVE_MEMBER):
			if guild.IsMemberByName(self.name):
				return True

		return False

	def OnRemove(self):
		net.SendGuildRemoveMemberPacket(self.key)
		return True

class MessengerFriendGroup(MessengerGroupItem):
	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeInfo.MESSENGER_FRIEND)

	def __del__(self):
		MessengerGroupItem.__del__(self)

	def AppendMember(self, key, name):
		item = MessengerFriendItem(self.getParentEvent)
		return MessengerGroupItem.AppendMember(self, item, key, name)

if app.ENABLE_MESSENGER_GM:
	class MessengerGMGroup(MessengerGroupItem):
		def __init__(self, getParentEvent):
			MessengerGroupItem.__init__(self, getParentEvent)
			self.SetName(localeInfo.MESSENGER_GM)

		def AppendMember(self, key, name):
			item = MessengerGMItem(self.getParentEvent)
			return MessengerGroupItem.AppendMember(self, item, key, name)

class MessengerGuildGroup(MessengerGroupItem):
	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeInfo.MESSENGER_GUILD)
		self.AddFlag("float")

	def __del__(self):
		MessengerGroupItem.__del__(self)

	def AppendMember(self, key, name):
		item = MessengerGuildItem(self.getParentEvent)
		return MessengerGroupItem.AppendMember(self, item, key, name)

class MessengerFamilyGroup(MessengerGroupItem):
	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeInfo.MESSENGER_FAMILY)
		self.AddFlag("float")

		self.lover = None

	def __del__(self):
		MessengerGroupItem.__del__(self)

	def AppendMember(self, key, name):
		item = MessengerGuildItem(self.getParentEvent)
		self.lover = item
		return MessengerGroupItem.AppendMember(self, item, key, name)

	def GetLover(self):
		return self.lover

###################################################################################################
###################################################################################################
###################################################################################################

class MessengerWindow(ui.ScriptWindow):
	START_POSITION = 40

	class ResizeButton(ui.DragButton):
		def OnMouseOverIn(self):
			app.SetCursor(app.VSIZE)

		def OnMouseOverOut(self):
			app.SetCursor(app.NORMAL)

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		messenger.SetMessengerHandler(self)

		self.board = None
		self.groupList = []
		self.showingItemList = []
		self.selectedItem = None
		self.whisperButtonEvent = lambda *arg: None
		self.familyGroup = None

		self.guildButtonEvent = None

		self.showingPageSize = 0
		self.startLine = 0

		self.isLoaded = 0

		if app.ENABLE_CHATTING_WINDOW_RENEWAL:
			self.interface = None

		self.__AddGroup()
		messenger.RefreshGuildMember()

	def Show(self):
		if self.isLoaded == 0:
			self.isLoaded = 1

			self.__LoadWindow()
			self.OnRefreshList()
			self.OnResizeDialog()

		ui.ScriptWindow.Show(self)
		if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
			wndMgr.SetWheelTopWindow(self.hWnd)

	def __LoadWindow(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "UIScript/MessengerWindow.py")

		try:
			self.board = self.GetChild("board")
			self.scrollBar = self.GetChild("ScrollBar")
			self.whisperButton = self.GetChild("WhisperButton")
			self.removeButton = self.GetChild("RemoveButton")
			self.addFriendButton = self.GetChild("AddFriendButton")
			self.addBlockButton = self.GetChild("BlockFriendButton")
			if not app.ENABLE_MESSENGER_BLOCK:
				self.addBlockButton.Hide()
			self.guildButton = self.GetChild("GuildButton")

		except:
			import exception
			exception.Abort("MessengerWindow.__LoadWindow.__Bind")

		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.whisperButton.SetEvent(ui.__mem_func__(self.OnPressWhisperButton))
		self.removeButton.SetEvent(ui.__mem_func__(self.OnPressRemoveButton))
		self.addFriendButton.SetEvent(ui.__mem_func__(self.OnPressAddFriendButton))
		if app.ENABLE_MESSENGER_BLOCK:
			self.addBlockButton.SetEvent(ui.__mem_func__(self.OnPressAddBlockButton))
		self.guildButton.SetEvent(ui.__mem_func__(self.OnPressGuildButton))

		width = self.GetWidth()
		height = self.GetHeight()
		if not app.ENABLE_MESSENGER_BLOCK:
			self.addFriendButton.SetPosition(-60, 30)
			self.whisperButton.SetPosition(-20, 30)
			self.removeButton.SetPosition(20, 30)
			self.guildButton.SetPosition(60, 30)

		self.whisperButton.Disable()
		self.removeButton.Disable()

		resizeButton = self.ResizeButton()
		resizeButton.AddFlag("restrict_x")
		resizeButton.SetParent(self)
		resizeButton.SetSize(self.GetWidth(), 10)
		resizeButton.SetWindowVerticalAlignBottom()
		resizeButton.SetPosition(0, 0)
		resizeButton.Show()
		self.resizeButton = resizeButton
		self.resizeButton.SetMoveEvent(ui.__mem_func__(self.OnResizeDialog))
		self.resizeButton.SetPosition(0, 300)

		for list in self.groupList:
			list.SetTop()

	def __del__(self):
		messenger.SetMessengerHandler(None)
		ui.ScriptWindow.__del__(self)

	if app.ENABLE_CHATTING_WINDOW_RENEWAL:
		def BindInterface(self, interface):
			self.interface = proxy(interface)

	def Destroy(self):
		self.board = None
		self.scrollBar = None
		self.resizeButton = None
		self.friendNameBoard = None
		self.questionDialog = None
		self.popupDialog = None
		self.inputDialog = None
		self.familyGroup = None

		self.whisperButton = None
		self.removeButton = None
		if app.ENABLE_MESSENGER_BLOCK:
			self.blockFriendNameBoard = None

		if app.ENABLE_CHATTING_WINDOW_RENEWAL:
			self.interface = None

	def OnCloseQuestionDialog(self):
		self.questionDialog.Close()
		self.questionDialog = None
		return True

	def Close(self):
		self.questionDialog = None
		self.Hide()

	def SetSize(self, width, height):
		ui.ScriptWindow.SetSize(self, width, height)
		if self.board:
			self.board.SetSize(width, height)

	def OnResizeDialog(self):
		x, y = self.resizeButton.GetLocalPosition()
		if y < 140:
			self.resizeButton.SetPosition(x, 140)
			return
		self.SetSize(self.GetWidth(), y + self.resizeButton.GetHeight())

		self.showingPageSize = y - (self.START_POSITION + 26)
		self.scrollBar.SetScrollBarSize(self.showingPageSize)

		self.__LocateMember()

		self.resizeButton.TurnOffCallBack()
		self.UpdateRect()
		self.resizeButton.TurnOnCallBack()

	def __LocateMember(self):
		if self.isLoaded == 0:
			return

		if self.showingPageSize / 20 >= len(self.showingItemList):
			self.scrollBar.Hide()
			self.startLine = 0
		else:
			if self.showingItemList:
				self.scrollBar.SetMiddleBarSize(float(self.showingPageSize/20) / float(len(self.showingItemList)))
			self.scrollBar.Show()

		#####

		yPos = self.START_POSITION
		heightLimit = self.GetHeight() - (self.START_POSITION + 13)

		map(ui.Window.Hide, self.showingItemList)

		for item in self.showingItemList[self.startLine:]:
			item.SetPosition(20 + item.GetStepWidth(), yPos)
			item.SetTop()
			item.Show()

			yPos += 20
			if yPos > heightLimit:
				break

	def __AddGroup(self):
		member = MessengerFriendGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()
		self.groupList.append(member)

		member = MessengerGuildGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()
		self.groupList.append(member)

		if app.ENABLE_MESSENGER_GM:
			member = MessengerGMGroup(ui.__mem_func__(self.GetSelf))
			member.Open()
			member.Show()
			self.groupList.append(member)

		if app.ENABLE_MESSENGER_BLOCK:
			member = MessengerBlockGroup(ui.__mem_func__(self.GetSelf))
			member.Open()
			member.Show()
			self.groupList.append(member)

	def __AddFamilyGroup(self):
		member = MessengerFamilyGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()

		self.familyGroup = member

	def ClearGuildMember(self):
		self.groupList[GUILD].ClearMember()

	def SetWhisperButtonEvent(self, event):
		self.whisperButtonEvent = event

	def SetGuildButtonEvent(self, event):
		self.guildButtonEvent = event

	def OnCloseInputDialog(self):
		self.inputDialog.Close()
		self.inputDialog = None
		return True

	def OnPressGuildButton(self):
		self.guildButtonEvent()

	def OnPressAddFriendButton(self):
		friendNameBoard = uiCommon.InputDialog()
		friendNameBoard.SetTitle(localeInfo.MESSENGER_ADD_FRIEND)
		friendNameBoard.SetAcceptEvent(ui.__mem_func__(self.OnAddFriend))
		friendNameBoard.SetCancelEvent(ui.__mem_func__(self.OnCancelAddFriend))
		friendNameBoard.Open()
		self.friendNameBoard = friendNameBoard

	def OnAddFriend(self):
		text = self.friendNameBoard.GetText()
		if text:
			net.SendMessengerAddByNamePacket(text)
		self.friendNameBoard.Close()
		self.friendNameBoard = None
		return True

	def OnCancelAddFriend(self):
		self.friendNameBoard.Close()
		self.friendNameBoard = None
		return True

	if app.ENABLE_MESSENGER_BLOCK:
		def OnPressAddBlockButton(self):
			blockNameBoard = uiCommon.InputDialog()
			blockNameBoard.SetTitle(localeInfo.MESSENGER_ADD_BLOCK_FRIEND)
			blockNameBoard.SetAcceptEvent(ui.__mem_func__(self.OnBlockFriend))
			blockNameBoard.SetCancelEvent(ui.__mem_func__(self.OnCancelBlockFriend))
			blockNameBoard.Open()
			self.blockFriendNameBoard = blockNameBoard

		def OnBlockFriend(self):
			text = self.blockFriendNameBoard.GetText()
			if text:
				net.SendMessengerBlockAddByNamePacket(text)
			self.blockFriendNameBoard.Close()
			self.blockFriendNameBoard = None
			return True

		def OnCancelBlockFriend(self):
			self.blockFriendNameBoard.Close()
			self.blockFriendNameBoard = None
			return True

	def OnPressWhisperButton(self):
		if self.selectedItem:
			self.selectedItem.OnWhisper()

	def OnPressRemoveButton(self):
		if self.selectedItem:
			if self.selectedItem.CanRemove():
				self.questionDialog = uiCommon.QuestionDialog()
				self.questionDialog.SetText(localeInfo.MESSENGER_DO_YOU_DELETE)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnRemove))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()

	def OnRemove(self):
		if self.selectedItem:
			if self.selectedItem.CanRemove():
				map(lambda arg, argDeletingItem = self.selectedItem : arg.RemoveMember(argDeletingItem), self.groupList)
				self.selectedItem.OnRemove()
				if not app.ENABLE_MESSENGER_LOCK_FIX:
					self.selectedItem.UnSelect()
					self.selectedItem = None
				self.OnRefreshList()

		self.OnCloseQuestionDialog()

	def OnScroll(self):
		scrollLineCount = len(self.showingItemList) - (self.showingPageSize/20)
		startLine = int(scrollLineCount * self.scrollBar.GetPos())

		if startLine != self.startLine:
			self.startLine = startLine
			self.__LocateMember()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
		def OnMouseWheelButtonUp(self):
			if self.scrollBar:
				self.scrollBar.OnUp()
				return True

			return False

		def OnMouseWheelButtonDown(self):
			if self.scrollBar:
				self.scrollBar.OnDown()
				return True

			return False

	## CallBack
	def OnSelectItem(self, item):

		if self.selectedItem:
			if item != self.selectedItem:
				self.selectedItem.UnSelect()

		self.selectedItem = item

		if self.selectedItem:
			self.selectedItem.Select()

			if self.selectedItem.CanWhisper():
				self.whisperButton.Enable()
			else:
				self.whisperButton.Disable()

			if self.selectedItem.CanRemove():
				self.removeButton.Enable()
			else:
				self.removeButton.Disable()

	def OnDoubleClickItem(self, item):
		if not self.selectedItem:
			return

		if self.selectedItem.IsOnline():
			self.OnPressWhisperButton()

	def GetSelf(self):
		return self

	def OnRefreshList(self):
		self.showingItemList = []

		if self.familyGroup:
			self.showingItemList.append(self.familyGroup)
			if self.familyGroup.GetLover():
				self.showingItemList.append(self.familyGroup.GetLover())

		for group in self.groupList:
			self.showingItemList.append(group)

			if group.IsOpen():
				loginMemberList = group.GetLoginMemberList()
				logoutMemberList = group.GetLogoutMemberList()

				if loginMemberList or logoutMemberList:
					for member in loginMemberList:
						self.showingItemList.append(member)
					for member in logoutMemberList:
						self.showingItemList.append(member)
				else:
					item = MessengerItem(ui.__mem_func__(self.GetSelf))
					item.SetName(localeInfo.MESSENGER_EMPTY_LIST)
					self.showingItemList.append(item)

		self.__LocateMember()
		if app.ENABLE_CHATTING_WINDOW_RENEWAL:
			if self.interface:
				self.interface.RefreshWhisperMessengerButtons()

	def RefreshMessenger(self):
		self.OnRefreshList()

	## EventHandler
	def __AddList(self, groupIndex, key, name):
		group = self.groupList[groupIndex]
		member = group.FindMember(key)
		if not member:
			member = group.AppendMember(key, name)
			#if not app.ENABLE_MESSENGER_LOCK_FIX:
			self.OnSelectItem(None)

		return member

	def OnRemoveList(self, groupIndex, key):
		if app.ENABLE_MESSENGER_BLOCK:
			if app.ENABLE_MESSENGER_LOCK_FIX:
				if self.selectedItem:
					if isinstance(self.selectedItem, MessengerMemberItem):
						if self.selectedItem.key == key:
							self.selectedItem.UnSelect()
							self.selectedItem = None

							if self.whisperButton:
								self.whisperButton.Disable()
							if self.removeButton:
								self.removeButton.Disable()
		else:
			if app.ENABLE_MESSENGER_LOCK_FIX:
				if self.selectedItem:
					if self.selectedItem.key == key:
						self.selectedItem.UnSelect()
						self.selectedItem = None

		group = self.groupList[groupIndex]
		group.RemoveMember(group.FindMember(key))
		self.OnRefreshList()

	def OnRemoveAllList(self, groupIndex):
		group = self.groupList[groupIndex]
		group.ClearMember()
		self.OnRefreshList()

	if app.ENABLE_MESSENGER_DETAILS:
		def OnLogin(self, groupIndex, key, offlineTime = 0, country = ""):
			name = key
			group = self.groupList[groupIndex]
			member = self.__AddList(groupIndex, key, key)
			member.SetName(name)
			member.SetOfflineTime(offlineTime)
			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				member.SetLanguage(country)
			member.Online()
			self.OnRefreshList()

		def OnLogout(self, groupIndex, key, offlineTime = 0, country = ""):
			name = None
			group = self.groupList[groupIndex]
			member = self.__AddList(groupIndex, key, name)
			name = key
			member.SetName(name)
			member.SetOfflineTime(offlineTime)
			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				member.SetLanguage(country)
			member.Offline()
			self.OnRefreshList()
	else:
		def OnLogin(self, groupIndex, key, name = None):
			if not name:
				name = key
			group = self.groupList[groupIndex]
			member = self.__AddList(groupIndex, key, name)
			member.SetName(name)
			member.Online()
			self.OnRefreshList()

		def OnLogout(self, groupIndex, key, name = None):
			group = self.groupList[groupIndex]
			member = self.__AddList(groupIndex, key, name)
			if not name:
				name = key
			member.SetName(name)
			member.Offline()
			self.OnRefreshList()

	def OnAddLover(self, name, lovePoint):
		if not self.familyGroup:
			self.__AddFamilyGroup()

		member = self.familyGroup.AppendMember(0, name)

		member.SetName(name)
		member.SetLovePoint(lovePoint)
		member.Offline()
		self.OnRefreshList()

	def OnUpdateLovePoint(self, lovePoint):
		if not self.familyGroup:
			return

		lover = self.familyGroup.GetLover()
		if not lover:
			return

		lover.SetLovePoint(lovePoint)

	def OnLoginLover(self):
		if not self.familyGroup:
			return

		lover = self.familyGroup.GetLover()
		if not lover:
			return

		lover.Online()

	def OnLogoutLover(self):
		if not self.familyGroup:
			return

		lover = self.familyGroup.GetLover()
		if not lover:
			return

		lover.Offline()

	def ClearLoverInfo(self):
		if not self.familyGroup:
			return

		self.familyGroup.ClearMember()
		self.familyGroup = None
		self.OnRefreshList()