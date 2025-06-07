#
# OSF Info
# Child of constInfo.py
#
# General Options
#

import app

ENABLE_HYPERLINK_ITEM_ICON = True # Show item icon on hyperlink.

if app.ENABLE_QUEST_RENEWAL:
	ENABLE_QUEST_LETTER_CATEGORY = False # Show quest letters by category

ENABLE_AUTO_RESIZE_POPUP_DIALOG = True # Auto resize popup dialog

USE_NEW_WHISPER_LETTER_CATEGORY = True # Use the new letter category for normal and gm players

SHOW_LOADING_PROGRESS = True # Show loading progress
EXTENDED_LOADING_GUAGE = True # Loading guage extended with description
FILTER_EMOJI_NOTICE = True # Filter emojis on tip notice or big notice to chat line

SHOW_EMOJI_IN_CHAT_LINE = True # Show emoji in chat line
SHOW_EMOJI_IN_WHISPER_LINE = False # Show emoji in whisper line
SHOW_USE_IMAGE_TEXT_LINE = True # Show use item image text line in tooltip
SHOW_SELL_IMAGE_TEXT_LINE = False # Show sell item image text line in tooltip

SHOW_REFINE_ITEM_DESC = True # Show refine item description

SHOW_REFINE_PERCENTAGE = True # Show refine success percentage

MOUNT_ITEM_ATTR_TOOLTIP = True # Show quest item attributes on tooltip
PET_ITEM_ATTR_TOOLTIP = True # Show quest item attributes on tooltip
SHOW_ITEM_VNUM_TOOTIP = True # Show the item vnum

LOGIN_UI_MODIFY = False # Login countdown

##################################################################################

def StripColor(text):
	import re

	regex = '\|c([a-zA-Z0-9]){0,8}|'
	search = re.search(regex, text)
	if search:
		text = re.sub(regex, '', text)

	return text

def IsImageFilter(text):
	import re

	regex = '\|I.*\|i'
	search = re.search(regex, text)
	if search:
		return True

	return False

def IsHyperLinkFilter(text):
	import re

	regex = '\|H.*\|h'
	search = re.search(regex, text)
	if search:
		return True

	return False

def EmojiFilter(text):
	import re

	EMOJI_DIC = {
		':o' : 'emoji/amazed',
		':)' : 'emoji/happiness',
		':D' : 'emoji/happy',
		':\\' : 'emoji/indifferent',
		':p' : 'emoji/tongue',
		':P' : 'emoji/tongue',
		'xP' : 'emoji/dead',
		':X' : 'emoji/muted',
		':x' : 'emoji/muted',
		'>:D' : 'emoji/evil',
		':3' : 'emoji/in-love',
		'<3' : 'emoji/heart',
		':*' : 'emoji/kiss',
		':O' : 'emoji/surprised',
		'>:(' : 'emoji/angry',
		':||' : 'emoji/quiet',
	}

	for emoji_str, emoji_img in EMOJI_DIC.iteritems():
		text = text.replace(emoji_str, "|I" + emoji_img + "|i")

	return text
