/*
 * Copyright (c) 2021 p-sam
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stratosphere.hpp>
#include "virtual_controller.hpp"

typedef enum {
	HotkeyType_Capture = 0,
	HotkeyType_Home,
	HotkeyType_Custom,
	HotkeyType_EnumMax
} HotkeyType;

typedef struct {
	u64 tick;
	bool longPressed;
} HotkeyCacheCombo;

typedef struct {
	HotkeyCacheCombo combos[HotkeyType_EnumMax];
} HotkeyCache;

class HotkeyService
{
	public:
		HotkeyService();
		virtual ~HotkeyService();
		void Start();
		void Stop();

		Result Initialize();
		void Exit();

		void Action(HotkeyType type, bool longPressed);
		static bool ComboPressed(HotkeyType type, u32 deviceType, u64 buttons);
		static u64 ComboLongPressDuration(HotkeyType type);

	protected:
		static void ProcessThreadFunc(void *arg);
		void PollHidStates();
		void ProcessHidState(HotkeyCache* cache, u32 deviceType, u64 buttons);

		bool running;
		ams::os::ThreadType thread;
		ams::os::Mutex threadMutex;

		HotkeyCache hotkeyCache[4];
		Service appletSrv;
		Service appletIDebugFunctionsSrv;
};