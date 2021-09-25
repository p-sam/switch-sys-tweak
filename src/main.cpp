/*
 * Copyright (c) 2018 p-sam
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

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <malloc.h>

#ifdef HAVE_VCON
#include "virtual_controller_service.hpp"
#endif

#ifdef HAVE_HOTKEY
#include "hotkey_service.hpp"
#endif

#include "libams.hpp"
#include "file_utils.hpp"
#include "mitm_manager.hpp"

extern "C" {
	extern u32 __start__;

	u32 __nx_applet_type = AppletType_None;
	u32 __nx_fs_num_sessions = 1;

	#define INNER_HEAP_SIZE 0x20000
	size_t nx_inner_heap_size = INNER_HEAP_SIZE;
	char nx_inner_heap[INNER_HEAP_SIZE];

	void __libnx_initheap(void);
	void __appInit(void);
	void __appExit(void);

	/* Exception handling. */
	alignas(16) u8 __nx_exception_stack[ams::os::MemoryPageSize];
	u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);
	void __libnx_exception_handler(ThreadExceptionDump *ctx);
}

namespace ams {
	ams::ncm::ProgramId CurrentProgramId = {0x00FF747765616BFFul};

	namespace result {
		bool CallFatalOnResultAssertion = true;
	}
}

void __libnx_exception_handler(ThreadExceptionDump *ctx) {
	ams::CrashHandler(ctx);
}

void __libnx_initheap(void) {
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	/* Newlib */
	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end   = (char*)addr + size;
}

void __appInit(void) {
	ams::sm::Initialize();
	ams::hos::InitializeForStratosphere();
}

void __appExit(void) {
	ams::sm::Finalize();
}

#if !defined(HAVE_VCON) && !defined(HAVE_HOTKEY)
static_assert(MitmManager::HasAtLeastOneServiceDefined() > 0, "At least one feature should be enabled.");
#endif

int main(int argc, char **argv)
{
	MitmManager serverManager;

	R_ABORT_UNLESS(FileUtils::InitializeAsync());
	R_ABORT_UNLESS(serverManager.RegisterServers());

#ifdef HAVE_HOTKEY
	HotkeyService hotkeyService;
	FileUtils::LogLine("Starting HotkeyService");
	hotkeyService.Start();
#endif

#ifdef HAVE_VCON
	VirtualControllerService vconService;
	FileUtils::LogLine("Starting VirtualControllerService");
	vconService.Start();
#endif

	FileUtils::LogLine("serverManager.LoopProcess()");
	serverManager.LoopProcess();

#ifdef HAVE_VCON
	vconService.Stop();
#endif

	return 0;
}

