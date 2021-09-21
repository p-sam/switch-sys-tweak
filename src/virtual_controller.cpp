#include "virtual_controller.hpp"
#include <cstring>

static HiddbgHdlsSessionId g_hdlsSessionId;

Result VirtualController::Initialize() { 
	Result rc = hiddbgInitialize();
	if(R_SUCCEEDED(rc)) {
		rc = hiddbgAttachHdlsWorkBuffer(&g_hdlsSessionId);
	}
	return rc;
}

void VirtualController::Exit() {
	hiddbgReleaseHdlsWorkBuffer(g_hdlsSessionId);
	hiddbgExit();
}

VirtualController::VirtualController() {
	this->hdlsHandle = {0};
	this->hidDeviceInfo.deviceType = HidDeviceType_FullKey15;
	this->hidDeviceInfo.npadInterfaceType = HidNpadInterfaceType_Bluetooth;

	this->hidDeviceInfo.singleColorBody = 0xFF735A01;
	this->hidDeviceInfo.singleColorButtons = 0xFF000000;
	this->hidDeviceInfo.colorLeftGrip = 0xFF464646;
	this->hidDeviceInfo.colorRightGrip = 0xFF464646;

	memset(&this->hidState, 0, sizeof(this->hidState));

	this->clientTick = 0;
	this->lastUpdateTick = armGetSystemTick();
}

VirtualController::~VirtualController() {
	this->Disconnect();
}

Result VirtualController::Connect() {
	Result rc = 0;

	if(!this->Connected()) {
		rc = hiddbgAttachHdlsVirtualDevice(&this->hdlsHandle, &this->hidDeviceInfo);
	}

	if(R_FAILED(rc)) {
		this->hdlsHandle = {0};
	}

	return rc;
}

void VirtualController::Disconnect() {
	if(this->Connected()) {
		hiddbgDetachHdlsVirtualDevice(this->hdlsHandle);
		this->hdlsHandle = {0};
	}
}

void VirtualController::SetState(ControllerPacket* packet) {
	if(packet->tick < this->clientTick) {
		return;
	}

	this->lastUpdateTick = armGetSystemTick();
	this->clientTick = packet->tick;

	this->hidState.buttons = 0;

#define _UPDATE_BUTTON(packet_mask, hid_mask) if(packet->keysHeld & (packet_mask)) \
	this->hidState.buttons |= hid_mask

	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_A, HidNpadButton_A);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_B, HidNpadButton_B);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_SELECT, HidNpadButton_Minus);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_START, HidNpadButton_Plus);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_DRIGHT, HidNpadButton_Right);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_DLEFT, HidNpadButton_Left);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_DUP, HidNpadButton_Up);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_DDOWN, HidNpadButton_Down);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_R, HidNpadButton_R);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_L, HidNpadButton_L);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_X, HidNpadButton_X);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_Y, HidNpadButton_Y);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_LSTICK, HidNpadButton_StickL);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_RSTICK, HidNpadButton_StickR);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_ZL, HidNpadButton_ZL);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_ZR, HidNpadButton_ZR);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_CAPTURE, HiddbgNpadButton_Capture);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_HOME, HiddbgNpadButton_Home);
#undef _UPDATE_BUTTON

	this->hidState.analog_stick_l.x = packet->leftStick.dx * (JOYSTICK_MAX / CONTROLLER_PACKET_STICK_MAX);
	this->hidState.analog_stick_l.y = packet->leftStick.dy * (JOYSTICK_MAX / CONTROLLER_PACKET_STICK_MAX);
	this->hidState.analog_stick_r.x = packet->rightStick.dx * (JOYSTICK_MAX / CONTROLLER_PACKET_STICK_MAX);
	this->hidState.analog_stick_r.y = packet->rightStick.dy * (JOYSTICK_MAX / CONTROLLER_PACKET_STICK_MAX);
}

Result VirtualController::FlushState() {
	if(!this->Connected()) {
		return 0;
	}

	bool attached;
	Result rc = hiddbgIsHdlsVirtualDeviceAttached(g_hdlsSessionId, this->hdlsHandle, &attached);

	if(R_FAILED(rc) || !attached) {
		this->Disconnect();
		rc = this->Connect();
	}

	if(R_SUCCEEDED(rc)) {
		rc = hiddbgSetHdlsState(this->hdlsHandle, &this->hidState);
	}

	return rc;
}
