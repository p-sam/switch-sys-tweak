#include "virtual_controller.hpp"
#include <cstring>

Result VirtualController::Initialize() { 
	Result rc = hiddbgInitialize();
	if(R_SUCCEEDED(rc)) {
		hiddbgAttachHdlsWorkBuffer();
	}
	return rc;
}

void VirtualController::Exit() {
	hiddbgReleaseHdlsWorkBuffer();
	hiddbgExit();
}

VirtualController::VirtualController() {
	this->hdlsHandle = 0;
	this->hidDeviceInfo.deviceType = HidDeviceType_FullKey15;
	this->hidDeviceInfo.npadInterfaceType = NpadInterfaceType_Bluetooth;

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
		this->hdlsHandle = 0;
	}

	return rc;
}

void VirtualController::Disconnect() {
	if(this->Connected()) {
		hiddbgDetachHdlsVirtualDevice(this->hdlsHandle);
		this->hdlsHandle = 0;
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

	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_A, KEY_A);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_B, KEY_B);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_SELECT, KEY_MINUS);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_START, KEY_PLUS);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_DRIGHT, KEY_DRIGHT);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_DLEFT, KEY_DLEFT);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_DUP, KEY_DUP);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_DDOWN, KEY_DDOWN);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_R, KEY_R);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_L, KEY_L);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_X, KEY_X);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_Y, KEY_Y);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_LSTICK, KEY_LSTICK);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_RSTICK, KEY_RSTICK);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_ZL, KEY_ZL);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_ZR, KEY_ZR);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_CAPTURE, KEY_CAPTURE);
	_UPDATE_BUTTON(CONTROLLER_PACKET_KEY_HOME, KEY_HOME);
#undef _UPDATE_BUTTON

	this->hidState.joysticks[JOYSTICK_LEFT].dx = packet->leftStick.dx * (JOYSTICK_MAX / CONTROLLER_PACKET_STICK_MAX);
	this->hidState.joysticks[JOYSTICK_LEFT].dy = packet->leftStick.dy * (JOYSTICK_MAX / CONTROLLER_PACKET_STICK_MAX);
	this->hidState.joysticks[JOYSTICK_RIGHT].dx = packet->rightStick.dx * (JOYSTICK_MAX / CONTROLLER_PACKET_STICK_MAX);
	this->hidState.joysticks[JOYSTICK_RIGHT].dy = packet->rightStick.dy * (JOYSTICK_MAX / CONTROLLER_PACKET_STICK_MAX);
}

Result VirtualController::FlushState() {
	if(!this->Connected()) {
		return 0;
	}

	bool attached;
	Result rc = hiddbgIsHdlsVirtualDeviceAttached(this->hdlsHandle, &attached);

	if(R_FAILED(rc) || !attached) {
		this->Disconnect();
		rc = this->Connect();
	}

	if(R_SUCCEEDED(rc)) {
		rc = hiddbgSetHdlsState(this->hdlsHandle, &this->hidState);
	}

	return rc;
}