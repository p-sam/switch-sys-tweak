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
		static void ProcessThreadFunc(void *arg);
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
