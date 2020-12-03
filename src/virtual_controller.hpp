#pragma once

#include <switch.h>
#include "controller_packet.h"

class VirtualController {
	protected:
		HiddbgHdlsDeviceInfo hidDeviceInfo;
		HiddbgHdlsState hidState;
		HiddbgHdlsHandle hdlsHandle;

		u64 clientTick;
		u64 lastUpdateTick;

	public:
		VirtualController();
		~VirtualController();

		inline bool Inactive() { return armTicksToNs(armGetSystemTick() - this->lastUpdateTick) > 10000000000ULL; }
		inline bool Connected() { return this->hdlsHandle.handle != 0; }
		Result Connect();
		void Disconnect();
		void SetState(ControllerPacket* packet);
		Result FlushState();

		static Result Initialize();
		static void Exit();
};

