#include "hotkey_service.hpp"

#include "file_utils.hpp"

#ifdef ENABLE_CUSTOM_HOTKEY
#include "hotkey_custom.inc"
#endif

static HidNpadIdType g_npad_types[] = {
	HidNpadIdType_No1,
	HidNpadIdType_No2,
	HidNpadIdType_No3,
	HidNpadIdType_No4,
};

HotkeyService::HotkeyService(): threadMutex(false) {
	R_ABORT_UNLESS(ams::os::CreateThread(&this->thread, &HotkeyService::ProcessThreadFunc, this, NULL, 0x8000, 31));
	this->running = false;
	memset(this->hotkeyCache, 0, sizeof(this->hotkeyCache));
}

HotkeyService::~HotkeyService() {
	this->Stop();
	ams::os::DestroyThread(&this->thread);
}

void HotkeyService::Start() {
	std::scoped_lock lock(this->threadMutex);
	if(this->running) {
		return;
	}

	this->running = true;
	ams::os::StartThread(&this->thread);
}

void HotkeyService::Stop() {
	std::scoped_lock lock(this->threadMutex);
	if(!this->running) {
		return;
	}

	this->running = false;
	ams::os::WaitThread(&this->thread);
}

Result HotkeyService::Initialize() {
	Result rc = hidInitialize();
	hidInitializeNpad();
	hidSetSupportedNpadIdType(g_npad_types, sizeof(g_npad_types) / sizeof(g_npad_types[0]));
	hidSetSupportedNpadStyleSet(HidNpadStyleSet_NpadFullCtrl | HidNpadStyleTag_NpadSystemExt | HidNpadStyleTag_NpadLucia);

	if(R_SUCCEEDED(rc)) {
		rc = smGetService(&this->appletSrv, "appletAE");
		FileUtils::LogLine("smGetService: %x", rc);
	}

	if(R_SUCCEEDED(rc)) {
		rc = serviceConvertToDomain(&this->appletSrv);
		FileUtils::LogLine("serviceConvertToDomain: %x", rc);
	}

	if(R_SUCCEEDED(rc)) {
		serviceAssumeDomain(&this->appletSrv);
		rc = serviceDispatch(&this->appletSrv, 1000,
			.out_num_objects = 1,
			.out_objects = &this->appletIDebugFunctionsSrv,
		);
		FileUtils::LogLine("serviceDispatchIn get debug: %x", rc);
	}

	return rc;
}

void HotkeyService::Exit() {
	hidExit();
	serviceClose(&this->appletIDebugFunctionsSrv);
	serviceClose(&this->appletSrv);
}

void HotkeyService::ProcessHidState(HotkeyCache* cache, u32 deviceType, u64 buttons) {
	u64 tick = armGetSystemTick();

	for(u8 type = 0; type < HotkeyType_EnumMax; type++) {
		if(HotkeyService::ComboPressed((HotkeyType)type, deviceType, buttons)) {
			if(cache->combos[type].tick == 0) {
				cache->combos[type].tick = tick;
			} else if(!cache->combos[type].longPressed && (tick - cache->combos[type].tick > HotkeyService::ComboLongPressDuration((HotkeyType)type))) {
				cache->combos[type].longPressed = true;
				this->Action((HotkeyType)type, true);
			}
		} else if(cache->combos[type].tick != 0) {
			if(cache->combos[type].longPressed) {
				cache->combos[type].longPressed = false;
			} else {
				this->Action((HotkeyType)type, false);
			}

			cache->combos[type].tick = 0;
		}
	}
}

void HotkeyService::Action(HotkeyType type, bool longPressed) {
	FileUtils::LogLine("%s: type = %u; longPress = %u", __PRETTY_FUNCTION__, type, longPressed);

	u32 buttonType;
	switch(type) {
		case HotkeyType_Capture:
			buttonType = longPressed ? AppletSystemButtonType_CaptureButtonLongPressing : AppletSystemButtonType_CaptureButtonShortPressing;
			break;
		case HotkeyType_Home:
			buttonType = longPressed ? AppletSystemButtonType_HomeButtonLongPressing : AppletSystemButtonType_HomeButtonShortPressing;
			break;
		default:
			buttonType = 0;
			break;
	}

	if(buttonType != 0) {
		Result rc = serviceDispatchIn(&this->appletIDebugFunctionsSrv, 10, buttonType);
		FileUtils::LogLine("%s: serviceDispatchIn(&this->appletIDebugFunctionsSrv, 10, %u): %x", __PRETTY_FUNCTION__, buttonType, rc);
	}

#ifdef ENABLE_CUSTOM_HOTKEY
	if(type == HotkeyType_Custom) {
		__CustomHotkeyAction(longPressed);
	}
#endif
}

bool HotkeyService::ComboPressed(HotkeyType type, u32 deviceType, u64 buttons) {
	switch(type) {
		case HotkeyType_Capture:
			return deviceType & HidNpadStyleTag_NpadLucia ? ((buttons & HidNpadButton_ZL) && (buttons & HidNpadButton_L)) : false;
		case HotkeyType_Home:
			return deviceType & HidNpadStyleTag_NpadLucia ? ((buttons & HidNpadButton_ZR) && (buttons & HidNpadButton_R)) : false;
		case HotkeyType_Custom:
			return deviceType & HidNpadStyleTag_NpadLucia ? ((buttons & HidNpadButton_ZL) && (buttons & HidNpadButton_ZR)) : ((buttons & HidNpadButton_StickL) && (buttons & HidNpadButton_StickR));
		default:
			return false;
	}
}

u64 HotkeyService::ComboLongPressDuration(HotkeyType type) {
	switch(type) {
		case HotkeyType_Capture:
			return armNsToTicks(500000000ULL);
		case HotkeyType_Home:
			return armNsToTicks(1000000000ULL);
		case HotkeyType_Custom:
			return armNsToTicks(1000000000ULL);
		default:
			return 0;
	}
}

void HotkeyService::PollHidStates() {
	for(unsigned i = 0; i < (sizeof(this->hotkeyCache) / sizeof(this->hotkeyCache[0])); i++) {
		HidNpadIdType id = g_npad_types[i];
		HidNpadSystemExtState npadState;
		size_t npadStatesCount = hidGetNpadStatesSystemExt(id, &npadState, 1);

		if(npadStatesCount && (npadState.attributes & HidNpadAttribute_IsConnected)) {
			this->ProcessHidState(&this->hotkeyCache[i], hidGetNpadStyleSet(id), npadState.buttons);
		} else {
			this->ProcessHidState(&this->hotkeyCache[i], 0, 0);
		}
	}
}

void HotkeyService::ProcessThreadFunc(void *arg) {
	HotkeyService* service = (HotkeyService*)arg;
	svcSleepThread(6000000000ULL);
	FileUtils::LogLine("%s: Starting", __PRETTY_FUNCTION__);

	Result rc = service->Initialize();
	if(R_FAILED(rc)) {
		FileUtils::LogLine("%s: this->Initialize() -> 0x%x", __PRETTY_FUNCTION__, rc);
	}

	FileUtils::LogLine("%s: Started", __PRETTY_FUNCTION__);
	while(service->running) {
		if(R_SUCCEEDED(rc)) {
			service->PollHidStates();
		}
		svcSleepThread(4000000ULL);
	}

	service->Exit();
}
