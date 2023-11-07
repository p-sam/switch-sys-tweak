/*
 * Copyright (c) 2020 p-sam
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

#include "virtual_controller_service.hpp"

#include "file_utils.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

static const SocketInitConfig g_socketInitConfig = {
	.tcp_tx_buf_size = 0x100,
	.tcp_rx_buf_size = 0x100,
	.tcp_tx_buf_max_size = 0x100,
	.tcp_rx_buf_max_size = 0x100,

	.udp_tx_buf_size = 0x100,
	.udp_rx_buf_size = 0x8000,

	.sb_efficiency = 1,
};

VirtualControllerService::VirtualControllerService(): threadMutex(false) {

	R_ABORT_UNLESS(ams::os::CreateThread(&this->thread, &VirtualControllerService::ProcessThreadFunc, this, NULL, 0x2000, 31));
	this->running = false;
	this->sock = -1;
}

VirtualControllerService::~VirtualControllerService() {
	this->Stop();
	ams::os::DestroyThread(&this->thread);
}

void VirtualControllerService::Start() {
	std::scoped_lock lock(this->threadMutex);
	if(this->running) {
		return;
	}

	this->running = true;
	ams::os::StartThread(&this->thread);
}

void VirtualControllerService::Stop() {
	std::scoped_lock lock(this->threadMutex);
	if(!this->running) {
		return;
	}

	this->running = false;
	ams::os::WaitThread(&this->thread);
}

void VirtualControllerService::BindServer() {
	struct sockaddr_in servAddr;

	this->sock = socket(AF_INET, SOCK_DGRAM, 0);

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port = htons(VIRTUAL_CONTROLLER_PORT);

	bind(this->sock, (const struct sockaddr*)&servAddr, sizeof(servAddr));
}

void VirtualControllerService::CloseServer() {
	close(this->sock);
	this->sock = -1;
}

bool VirtualControllerService::ReceiveMessages() {
	ControllerPacket packet;

	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);

	ssize_t n = recvfrom(sock, &packet, sizeof(packet), 0, (sockaddr*)&clientAddr, &addrLen);
	if(n != sizeof(packet)) {
		return false;
	}

	if(packet.magic != CONTROLLER_PACKET_MAGIC) {
		return true;
	}

	VirtualController* controller;
	std::map<std::uint32_t, VirtualController*>::iterator it = this->controllers.find(clientAddr.sin_addr.s_addr);
	if(it != this->controllers.end()) {
		controller = it->second;
	} else {
		FileUtils::LogLine("%s: new controller #0x%x", __PRETTY_FUNCTION__, clientAddr.sin_addr.s_addr);
		controller = new VirtualController();
		R_ABORT_UNLESS(controller->Connect());
		this->controllers[clientAddr.sin_addr.s_addr] = controller;
	}

	if(packet.buttons & CONTROLLER_PACKET_KEY_EXIT) {
		FileUtils::LogLine("%s: controller #0x%x exited", __PRETTY_FUNCTION__, clientAddr.sin_addr.s_addr);
		delete controller;
		this->controllers.erase(clientAddr.sin_addr.s_addr);
		return true;
	}

	controller->SetState(&packet);
	Result rc = controller->FlushState();
	if(R_FAILED(rc)) {
		FileUtils::LogLine("%s: controller #0x%x flushState method failed with rc 0x%x", __PRETTY_FUNCTION__, clientAddr.sin_addr.s_addr, rc);
		return false;
	}

	return true;
}

void VirtualControllerService::Cleanup(bool force) {
	std::map<std::uint32_t, VirtualController*>::const_iterator it = this->controllers.cbegin();
	while(it != this->controllers.cend()) {
		if(force || it->second->Inactive()) {
			FileUtils::LogLine("%s: cleaning up controller #0x%x", __PRETTY_FUNCTION__, it->first);
			delete it->second;
			it = this->controllers.erase(it);
		} else {
			it++;
		}
	}
}

void VirtualControllerService::ProcessThreadFunc(void* arg) {
	VirtualControllerService* service = (VirtualControllerService*)arg;
	svcSleepThread(6000000000ULL);
	FileUtils::LogLine("%s: Starting", __PRETTY_FUNCTION__);
	R_ABORT_UNLESS(socketInitialize(&g_socketInitConfig));
	R_ABORT_UNLESS(VirtualController::Initialize());

	service->BindServer();

	FileUtils::LogLine("%s: listening on UDP port %d (MAGIC: 0x%08X; PACKET_SIZE: 0x%X)", __PRETTY_FUNCTION__, VIRTUAL_CONTROLLER_PORT, CONTROLLER_PACKET_MAGIC, sizeof(ControllerPacket));

	u64 lastCleanupTicks = armGetSystemTick();
	u64 errorCount = 0;

	while(service->running) {
		if(!service->ReceiveMessages()) {
			errorCount++;
			if(errorCount >= 100) {
				FileUtils::LogLine("%s: service->ReceiveMessages() errored %d times; restarting the server", __PRETTY_FUNCTION__, errorCount);
				service->CloseServer();
				service->BindServer();
				errorCount = 0;
			}
		}

		if(armTicksToNs(armGetSystemTick() - lastCleanupTicks) > 1000000000ULL) {
			service->Cleanup(false);
			lastCleanupTicks = armGetSystemTick();
		}
	}

	service->Cleanup(true);
	VirtualController::Exit();
	socketExit();

	service->CloseServer();
}
