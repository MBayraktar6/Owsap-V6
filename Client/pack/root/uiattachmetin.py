import dbg
import player
import item
import net
import snd
import ui
import uiToolTip
import localeInfo
import app

class AttachMetinDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadScript()

		self.metinItemPos = 0
		self.targetItemPos = 0

		if app.ENABLE_EXTEND_INVEN_SYSTEM:
			self.metinItemWindow = None
			self.targetItemWindow = None

	def __LoadScript(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/attachstonedialog.py")

		except:
			import exception
			exception.Abort("AttachStoneDialog.__LoadScript.LoadObject")

		try:
			self.board = self.GetChild("Board")
			self.titleBar = self.GetChild("TitleBar")
			self.metinImage = self.GetChild("MetinImage")
			self.GetChild("AcceptButton").SetEvent(ui.__mem_func__(self.Accept))
			self.GetChild("CancelButton").SetEvent(ui.__mem_func__(self.Close))

		except:
			import exception
			exception.Abort("AttachStoneDialog.__LoadScript.BindObject")

		oldToolTip = uiToolTip.ItemToolTip()
		oldToolTip.SetParent(self)
		oldToolTip.SetPosition(15, 38)
		oldToolTip.SetFollow(False)
		oldToolTip.Show()
		self.oldToolTip = oldToolTip

		newToolTip = uiToolTip.ItemToolTip()
		newToolTip.SetParent(self)
		newToolTip.SetPosition(230 + 20, 38)
		newToolTip.SetFollow(False)
		newToolTip.Show()
		self.newToolTip = newToolTip

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.board = 0
		self.titleBar = 0
		self.metinImage = 0
		self.toolTip = 0

	def CanAttachMetin(self, slot, metin):
		if item.METIN_NORMAL == metin:
			if player.METIN_SOCKET_TYPE_SILVER == slot or player.METIN_SOCKET_TYPE_GOLD == slot:
				return True

		elif item.METIN_SUNGMA == metin:
			if player.METIN_SOCKET_TYPE_SILVER == slot or player.METIN_SOCKET_TYPE_GOLD == slot:
				return True

	def Open(self, metinItemWindow, metinItemPos, targetItemWindow, targetItemPos):
		self.metinItemWindow = metinItemWindow
		self.metinItemPos = metinItemPos

		self.targetItemWindow = targetItemWindow
		self.targetItemPos = targetItemPos

		metinIndex = player.GetItemIndex(metinItemWindow, metinItemPos)
		itemIndex = player.GetItemIndex(targetItemWindow, targetItemPos)

		self.oldToolTip.ClearToolTip()
		self.newToolTip.ClearToolTip()

		item.SelectItem(metinIndex)
		metinSubType = item.GetItemSubType()

		## Metin Image
		try:
			self.metinImage.LoadImage(item.GetIconImageFileName())
		except:
			dbg.TraceError("AttachMetinDialog.Open.LoadImage - Failed to find item data")

		## New Item ToolTip
		## New Item Data
		metinSlot = [player.GetItemMetinSocket(targetItemWindow, targetItemPos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			slotData = metinSlot[i]
			if self.CanAttachMetin(slotData, metinSubType):
				metinSlot[i] = metinIndex
				break

		refine_element = player.GetItemRefineElement(targetItemWindow, targetItemPos) if app.ENABLE_REFINE_ELEMENT_SYSTEM else None
		attrSlot = [player.GetItemAttribute(targetItemWindow, targetItemPos, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
		apply_random_list = [player.GetItemApplyRandom(targetItemWindow, targetItemPos, i) for i in xrange(player.APPLY_RANDOM_SLOT_MAX_NUM)]
		set_value = player.GetItemSetValue(targetItemWindow, targetItemPos) if app.ENABLE_SET_ITEM else 0

		self.newToolTip.AddItemData(itemIndex, metinSlot, attrSlot, None, 0, 0, targetItemWindow, targetItemPos, refine_element, apply_random_list, set_value)

		## Old Item ToolTip
		item.SelectItem(metinIndex)

		## Old Item Data
		refine_element = player.GetItemRefineElement(targetItemWindow, targetItemPos) if app.ENABLE_REFINE_ELEMENT_SYSTEM else None
		metinSlot = [player.GetItemMetinSocket(targetItemWindow, targetItemPos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		attrSlot = [player.GetItemAttribute(targetItemWindow, targetItemPos, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
		apply_random_list = [player.GetItemApplyRandom(targetItemWindow, targetItemPos, i) for i in xrange(player.APPLY_RANDOM_SLOT_MAX_NUM)]
		set_value = player.GetItemSetValue(targetItemWindow, targetItemPos) if app.ENABLE_SET_ITEM else 0

		self.oldToolTip.ResizeToolTipWidth(self.newToolTip.GetWidth())
		self.oldToolTip.AddItemData(itemIndex, metinSlot, attrSlot, None, 0, 0, targetItemWindow, targetItemPos, refine_element, apply_random_list, set_value)

		self.UpdateDialog()
		self.SetTop()
		self.Show()

	def UpdateDialog(self):
		newWidth = 15 + self.oldToolTip.GetWidth() + 45 + self.newToolTip.GetWidth() + 15
		newHeight = self.newToolTip.GetHeight() + 98

		self.newToolTip.SetPosition(15 + self.oldToolTip.GetWidth() + 45, 38)

		if localeInfo.IsARABIC():
			self.board.SetPosition(newWidth, 0)

			(x, y) = self.titleBar.GetLocalPosition()
			self.titleBar.SetPosition(newWidth - 15, y)

		self.board.SetSize(newWidth, newHeight)
		self.titleBar.SetWidth(newWidth - 15)
		self.SetSize(newWidth, newHeight)

		(x, y) = self.GetLocalPosition()
		self.SetPosition(x, y)

	def Accept(self):
		net.SendItemUseToItemPacket(self.metinItemWindow, self.metinItemPos, self.targetItemWindow, self.targetItemPos)

		snd.PlaySound("sound/ui/metinstone_insert.wav")
		self.Close()

	def Close(self):
		self.Hide()
