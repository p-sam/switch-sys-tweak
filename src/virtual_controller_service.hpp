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

#pragma once

#include <stratosphere.hpp>
#include "virtual_controller.hpp"

#define VIRTUAL_CONTROLLER_PORT 4123

class VirtualControllerService
{
	public:
		VirtualControllerService();
		virtual ~VirtualControllerService();
		void Start();
		void Stop();

	protected:
		static void ProcessThreadFunc(void* arg);
		void BindServer();
		void CloseServer();
		bool ReceiveMessages();
		void Cleanup(bool force);

		int sock;
		std::map<uint32_t, VirtualController*> controllers;
		bool running;
		ams::os::ThreadType thread;
		ams::os::Mutex threadMutex;
};
