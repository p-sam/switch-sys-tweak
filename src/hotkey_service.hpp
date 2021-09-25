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