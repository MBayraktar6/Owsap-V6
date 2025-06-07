#
# Title: Fishing Game
# Description: Fishing game, catch the fish.
# Author: Owsap
# Last Date: 2022.10.28
#
# Skype: owsap.
# Discord: owsap
#
# Website: https://owsap.dev/
# GitHub: https://github.com/Owsap
# M2Dev: https://metin2.dev/profile/544-owsap/
#

import event
import app
import wndMgr
import dbg
import player
import ui
import uiScriptLocale
import math

DEBUG = False

class FishGame(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__Load()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.Destroy()

	def __Initialize(self):
		## Objects
		self.backGroundWater = None # ImageBox
		self.goalCircle = None # ui.ImageBox
		self.fishImage = None
		self.hitImage = None
		self.missImage = None
		self.waveImage = None
		self.sharkImage = None
		self.timerGauge = None # ui.ExpandedImageBox
		self.timerGaugeVal = 15 #sec
		self.hitCount = None
		self.hitCountText = None # ui.TextLine

		## Debug
		self.debugText_fish_pos_Local = None # TextLine
		self.debugText_goal_pos_Local = None # TextLine
		self.debugText_mouse_pos_Local = None # TextLine

		self.hit_image_list = []
		self.miss_image_list = []
		self.wave_image_list = []
		self.navigationArea = None # ui.Box

		self.hit_image_list = [
			"d:/ymir work/ui/game/fishing/fishing_effect_hit.sub"
		]
		self.miss_image_list = [
			"d:/ymir work/ui/game/fishing/fishing_effect_miss.sub"
		]
		self.wave_image_list = [
			"d:/ymir work/ui/game/fishing/wave/fishing_effect_wave_1.sub",
			"d:/ymir work/ui/game/fishing/wave/fishing_effect_wave_2.sub",
			"d:/ymir work/ui/game/fishing/wave/fishing_effect_wave_3.sub",
			"d:/ymir work/ui/game/fishing/wave/fishing_effect_wave_4.sub",
		]
		self.fish_image_list = [
			"d:/ymir work/ui/game/fishing/fish/fishing_fish_1.sub",
			"d:/ymir work/ui/game/fishing/fish/fishing_fish_2.sub",
			"d:/ymir work/ui/game/fishing/fish/fishing_fish_3.sub",
			"d:/ymir work/ui/game/fishing/fish/fishing_fish_4.sub"
		]

		self.fishSpeed = 3
		self.fishMoving = False
		self.fishInGoal = False
		self.fishWaitDelay = 0
		self.fishingLimitDelay = 0
		self.fishingHanging = 0

		self.hitImageShowDelay = 0
		self.missImageShowDelay = 0
		self.sharkImageShowDelay = 0

		self.xFishDest = 0
		self.yFishDest = 0

		self.goalCount = 0

	def __Load(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/FishingGameWindow.py")

			self.board = self.GetChild("board") # board_with_titlebar
			self.backGroundWater = self.GetChild("fishing_background_water") # ui.ImageBox

			self.goalCircle = self.GetChild("fishing_goal_circle") # ui.ImageBox
			self.hitCount = self.GetChild("fishing_goal_count") # ui.ExpandedImageBox
			self.fishing_goal_count_text = self.GetChild("fishing_goal_count_text") # ui.ExpandedImageBox

			self.fishing_timer_baseImg = self.GetChild("fishing_timer_baseImg") # ui.ImageBox
			self.timerGauge = self.GetChild("fishing_timer_gauge") # ui.ExpandedImageBox

			#self.fishing_water_navArea = self.GetChild("fishing_water_navArea") # ui.Box

			## Debug
			if DEBUG:
				self.debug_text_circle_pos = self.GetChild("debug_text_circle_pos") # ui.TextLine
				self.debug_text_fish_pos = self.GetChild("debug_text_fish_pos") # ui.TextLine
				self.debug_text_mouse_pos = self.GetChild("debug_text_mouse_pos") # ui.TextLine
		except:
			import exception
			exception.Abort("FishGame.Load.UIScript/FishingGameWindow.py")

		if self.board:
			self.board.SetCloseEvent(ui.__mem_func__(self.Close))

		if self.backGroundWater:
			self.backGroundWater.SetEvent(ui.__mem_func__(self.OnMouseLeftButtonDownEvent), "mouse_click")
			self.backGroundWater.OnMouseOverIn = ui.__mem_func__(self.OnMouseOverIn)
			self.backGroundWater.OnMouseOverOut = ui.__mem_func__(self.OnMouseOverOut)

		if DEBUG:
			self.debug_text_circle_pos.SetPosition(50, 50)
			self.debug_text_fish_pos.SetPosition(50, 50 + (12 * 1))
			self.debug_text_mouse_pos.SetPosition(50, 50 + (12 * 2))

		self.CreateFishImage()

	def Destroy(self):
		self.__Initialize()

	def OnUpdate(self):
		hitImageShowTime = max(0, self.hitImageShowDelay - app.GetTime())
		if hitImageShowTime <= 0:

			self.fishingHanging = False

			self.hitCount.SetDiffuseColor(1.0, 1.0, 1.0, 1.0)

		missImageShowTime = max(0, self.missImageShowDelay - app.GetTime())
		if missImageShowTime <= 0:
			self.timerGauge.SetDiffuseColor(1, 1, 1)

		fishingLimitTime = max(0, self.fishingLimitDelay - app.GetTime())
		self.timerGauge.SetPercentage(fishingLimitTime, self.timerGaugeVal)

		if fishingLimitTime <= 0:
			self.Close()

		elif fishingLimitTime <= 5:
			self.timerGauge.SetDiffuseColor(1, 0, 0)

	def OnRender(self):
		if not self.fishImage:
			return

		## Fish Render
		(xFish, yFish) = self.fishImage.GetLocalPosition()
		if DEBUG:
			self.debug_text_fish_pos.SetText("%d, %d" % (xFish, yFish))

		LEFT_CMD = 0
		RIGHT_CMD = 0
		UP_CMD = 0
		DOWN_CMD = 0

		if xFish not in range(self.xFishDest - self.fishSpeed, self.xFishDest + self.fishSpeed) and\
			yFish not in range(self.yFishDest - self.fishSpeed, self.yFishDest + self.fishSpeed):

			self.fishMoving = True

			if self.xFishDest > 0 and self.xFishDest < xFish:
				LEFT_CMD = True
			if self.xFishDest > 0 and self.xFishDest > xFish:
				RIGHT_CMD = True
			if self.yFishDest > 0 and self.yFishDest < yFish:
				UP_CMD = True
			if self.yFishDest > 0 and self.yFishDest > yFish:
				DOWN_CMD = True

		else:
			if self.fishMoving == True:
				# Set fish waiting time before moving again.
				self.fishWaitDelay = app.GetTime() + (app.GetRandom(0.1, 1.0) * 0.1)
				self.fishMoving = False

			fishMoveTime = max(0, self.fishWaitDelay - app.GetTime())
			if fishMoveTime <= 0:
				self.CreateWaveEffect(xFish, yFish, 6.0)
				self.__OnMoveFish()

		## Navigation Commands
		if UP_CMD and LEFT_CMD:
			if yFish > 0 and xFish > 0:
				self.fishImage.SetRotation(320)
				self.fishImage.SetPosition(xFish - self.fishSpeed, yFish - self.fishSpeed)
		elif UP_CMD and RIGHT_CMD:
			if yFish > 0 and xFish < self.backGroundWater.GetWidth() - 30:
				self.fishImage.SetRotation(45)
				self.fishImage.SetPosition(xFish + self.fishSpeed, yFish - self.fishSpeed)
		elif DOWN_CMD and LEFT_CMD:
			if yFish < self.backGroundWater.GetHeight() - 30 and xFish > 0:
				self.fishImage.SetRotation(225)
				self.fishImage.SetPosition(xFish - self.fishSpeed, yFish + self.fishSpeed)
		elif DOWN_CMD and RIGHT_CMD:
			if yFish < self.backGroundWater.GetHeight() - 30 and xFish < self.backGroundWater.GetWidth() - 30:
				self.fishImage.SetRotation(135)
				self.fishImage.SetPosition(xFish + self.fishSpeed, yFish + self.fishSpeed)
		else:
			if UP_CMD:
				if yFish > 0:
					self.fishImage.SetRotation(0)
					self.fishImage.SetPosition(xFish, yFish - self.fishSpeed)
			elif DOWN_CMD:
				if yFish < self.backGroundWater.GetHeight() - 30:
					self.fishImage.SetRotation(180)
					self.fishImage.SetPosition(xFish, yFish + self.fishSpeed)
			elif LEFT_CMD:
				if xFish > 0:
					self.fishImage.SetRotation(270)
					self.fishImage.SetPosition(xFish - self.fishSpeed, yFish)
			elif RIGHT_CMD:
				if xFish < self.backGroundWater.GetWidth() - 30:
					self.fishImage.SetRotation(90)
					self.fishImage.SetPosition(xFish + self.fishSpeed, yFish)
			else:
				pass

		## Miss & Hit Image Effect
		if self.missImage:
			(xMiss, yMiss) = self.missImage.GetLocalPosition()
			missImageShowTime = max(0, self.missImageShowDelay - app.GetTime())
			if missImageShowTime > 0.0:
				self.missImage.SetPosition(xMiss + missImageShowTime + 1.0, yMiss - missImageShowTime - 1.0)
			else:
				self.missImage.Hide()

		if self.hitImage:
			(xHit, yHit) = self.hitImage.GetLocalPosition()
			hitImageShowTime = max(0, self.hitImageShowDelay - app.GetTime())
			if hitImageShowTime > 0.0:
				self.hitImage.SetPosition(xHit + hitImageShowTime, yHit - hitImageShowTime - 0.1)
			else:
				self.hitImage.Hide()

		if self.sharkImage:
			(xShark, yShark) = self.sharkImage.GetLocalPosition()
			sharkImageShowTime = max(0, self.sharkImageShowDelay - app.GetTime())
			if sharkImageShowTime > 0.0:
				self.sharkImage.SetAlpha(sharkImageShowTime)
			else:
				self.sharkImage.Hide()

		## Circle Render
		if self.fishingHanging != True:
			(xCircle, yCircle) = self.goalCircle.GetLocalPosition()
			if DEBUG:
				self.debug_text_circle_pos.SetText("%d, %d" % (xCircle, yCircle))

			xCenterCircle = ((self.goalCircle.GetWidth() / 2) + xCircle) - 14 # 125
			yCenterCircle = ((self.goalCircle.GetHeight() / 2) + yCircle) - 13 # 100
			circleRadius = 65.0

			circleDistance = math.sqrt((xCenterCircle - xFish) ** 2 + (yCenterCircle - yFish) ** 2)
			if circleDistance < circleRadius:
				self.fishInGoal = True
				self.goalCircle.SetDiffuseColor(1.0, 0.7, 0.7, 1.0)
			else:
				self.fishInGoal = False
				self.goalCircle.SetDiffuseColor(1.0, 1.0, 1.0, 1.0)

		## Mouse Position Debug
		if DEBUG:
			(xWater, yWater) = self.backGroundWater.GetGlobalPosition()
			(xMouse, yMouse) = wndMgr.GetMousePosition()
			(xCursor, yCursor) = (xMouse - xWater - 15, yMouse - yWater - 15)
			if DEBUG:
				self.debug_text_mouse_pos.SetText("%d, %d" % (xCursor, yCursor))

	def __OnMoveFish(self):
		## Movable positions inside the goalCircle
		xyMoveMin = [ 15, 15 ]
		xyMoveMax = [ 230, 180]

		smartFish = False
		while smartFish != True:
			xDest, yDest = (app.GetRandom(15 * 100, 230 * 100), app.GetRandom(15 * 100, 180 * 100))

			xDest /= 100
			yDest /= 100

			## Check if the new destination position is close to the previous position.
			#if abs(self.xFishDest - xDest) <= 30 or abs(self.yFishDest - yDest) <= 30:
				#continue

			## Check the distance of the fish from the circle radius.
			## Make the fish avoid the circle as much as possible.
			(xCircle, yCircle) = self.goalCircle.GetLocalPosition()
			xCenterCircle = ((self.goalCircle.GetWidth() / 2) + xCircle) - 14
			yCenterCircle = ((self.goalCircle.GetHeight() / 2) + yCircle) - 13
			circleRadius = 65.0

			destDist = math.sqrt((xCenterCircle - xDest) ** 2 + (yCenterCircle - yDest) ** 2)
			if destDist > circleRadius:
				smartFish = 1
				break
			else:
				continue

		self.xFishDest = xDest
		self.yFishDest = yDest

	def InitFishing(self):
		self.fishingLimitDelay = app.GetTime() + self.timerGaugeVal
		self.goalCount = 0
		if self.fishSpeed >= 5:
			self.CreateSharkEffect()

		self.fishing_goal_count_text.SetText("%d / %d" % (self.goalCount, 3))
		self.__OnMoveFish()

	def OnMouseLeftButtonDownEvent(self):
		if self.fishingHanging != False:
			return

		## Clicked position.
		(xWater, yWater) = self.backGroundWater.GetGlobalPosition()
		(xCursor, yCursor) = wndMgr.GetMousePosition()
		(xClick, yClick) = (xCursor - xWater - 15, yCursor - yWater - 15)

		## Fish position.
		(xFish, yFish) = self.fishImage.GetLocalPosition()
		xFishGap = xClick - xFish
		yFishGap = yClick - yFish
		fishGap = 15

		if xFishGap > -fishGap and xFishGap < fishGap and yFishGap > -fishGap and yFishGap < fishGap and self.fishInGoal:
			self.CreateHitEffect(xClick, yClick)

			self.fishingHanging = True

			self.goalCircle.SetDiffuseColor(1.0, 1.0, 1.0)
			self.hitCount.SetDiffuseColor(0.5, 0.5, 0.5)

			self.goalCount += 1
			self.fishing_goal_count_text.SetText("%d / %d" % (self.goalCount, 3))

			player.FishingGameGoal(self.goalCount)
		else:
			self.CreateMissEffect(xClick, yClick)
			if self.timerGauge:
				self.timerGauge.SetDiffuseColor(1, 0, 0)
			self.fishingLimitDelay -= 0.7

		if self.goalCount >= 3:
			self.Close()

	def OnMouseOverIn(self):
		app.SetCursor(app.FISH)

	def OnMouseOverOut(self):
		app.SetCursor(app.NORMAL)

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Open(self, level):
		self.fishSpeed = max(1, min(int(level), 10))
		self.InitFishing()
		self.Show()

	def QuitFishing(self):
		self.ClearFishing()
		player.FishingGameQuit()

	def Show(self):
		self.SetCenterPosition()
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.QuitFishing()

		app.SetCursor(app.NORMAL)
		self.Hide()

	def ClearFishing(self):
		self.fishSpeed = 0
		self.fishMoving = False
		self.fishInGoal = False
		self.fishWaitDelay = 0
		self.fishingLimitDelay = 0
		self.fishingHanging = 0

		self.hitImageShowDelay = 0
		self.missImageShowDelay = 0

		self.xFishDest = 0
		self.yFishDest = 0

	def CreateFishImage(self):
		self.fishImage = ui.AniImageBox()
		self.fishImage.SetParent(self.backGroundWater)
		self.fishImage.SetPosition(215, 160)
		self.fishImage.SetDelay(6.0)
		for fileName in self.fish_image_list:
			self.fishImage.AppendImage(fileName)
		self.fishImage.Show()

	def CreateHitEffect(self, x, y):
		self.hitImage = ui.ExpandedImageBox()
		self.hitImage.SetParent(self.backGroundWater)
		self.hitImage.SetPosition(x, y)
		self.hitImage.SetAlpha(1.0) #6
		for fileName in self.hit_image_list:
			self.hitImage.LoadImage(fileName)
		self.hitImage.Show()

		self.hitImage.AddFlag("not_pick")
		self.hitImage.AddFlag("float")

		self.hitImageShowDelay = app.GetTime() + 0.5

	def CreateMissEffect(self, x, y):
		self.missImage = ui.ExpandedImageBox()
		self.missImage.SetParent(self.backGroundWater)
		self.missImage.SetPosition(x + 10, y - 5)
		self.missImage.SetAlpha(1.0)
		for fileName in self.miss_image_list:
			self.missImage.LoadImage(fileName)
		self.missImage.Show()

		self.missImage.AddFlag("not_pick")
		self.missImage.AddFlag("float")

		self.missImageShowDelay = app.GetTime() + 0.2

	def CreateWaveEffect(self, x, y, delay):
		self.waveImage = ui.AniImageBox()
		self.waveImage.SetParent(self.backGroundWater)
		self.waveImage.SetPosition(x, y)
		self.waveImage.SetDelay(delay)
		self.waveImage.SetEndFrameEvent(ui.__mem_func__(self.__OnEndWaveEffect))
		for fileName in self.wave_image_list:
			self.waveImage.AppendImage(fileName)
		self.waveImage.Show()

		self.waveImage.AddFlag("not_pick")
		self.waveImage.AddFlag("float")

	def __OnEndWaveEffect(self):
		if self.waveImage:
			self.waveImage.Hide()

	def CreateSharkEffect(self):
		(xCircle, yCircle) = self.goalCircle.GetLocalPosition()
		xCenterCircle = ((self.goalCircle.GetWidth() / 2) + xCircle) - 14 # 125
		yCenterCircle = ((self.goalCircle.GetHeight() / 2) + yCircle) - 13 # 100

		self.sharkImage = ui.ExpandedImageBox()
		self.sharkImage.SetParent(self.backGroundWater)
		self.sharkImage.SetPosition(xCircle + 11, yCircle + 11)
		self.sharkImage.SetAlpha(1.0)
		self.sharkImage.LoadImage("d:/ymir work/ui/game/fishing/shark.png")
		self.sharkImage.Show()

		self.sharkImage.AddFlag("not_pick")
		self.sharkImage.AddFlag("float")

		self.sharkImageShowDelay = app.GetTime() + 1.0
