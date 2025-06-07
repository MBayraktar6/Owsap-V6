#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "../UserInterface/Locale_inc.h"
#include "../EterLib/StdAfx.h"
#include "../ScriptLib/StdAfx.h"

#include "PythonGraphic.h"
#if defined(ENABLE_GRAPHIC_ON_OFF)
#include "PythonGraphicOnOff.h"
#endif
#include "PythonWindowManager.h"

void initgrp();
void initgrpImage();
void initgrpText();
void initgrpThing();
void initscriptWindow();
void initwndMgr();
