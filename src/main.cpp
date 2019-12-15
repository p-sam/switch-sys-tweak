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

#include <switch.h>
#include <stratosphere.hpp>

#ifdef HAVE_NSVM_SAFE
#include "nsvm_mitm_service.hpp"
#endif
#ifdef HAVE_NSAM_CONTROL
#include "nsam2_mitm_service.hpp"
#endif

#include "file_utils.hpp"

extern "C" {
	extern u32 __start__;

	u32 __nx_applet_type = AppletType_None;
	u32 __nx_fs_num_sessions = 1;
	u32 __nx_fsdev_direntry_cache_size = 1;

	#define INNER_HEAP_SIZE 0x20000
	size_t nx_inner_heap_size = INNER_HEAP_SIZE;
	char   nx_inner_heap[INNER_HEAP_SIZE];

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
	ams::hos::SetVersionForLibnx();
	R_ASSERT(smInitialize());

#ifdef HAVE_NSVM_SAFE
	R_ASSERT(nsvmInitialize());
#endif
#ifdef HAVE_NSAM_CONTROL
	R_ASSERT(nsInitialize());
#endif

	ams::CheckApiVersion();
}

void __appExit(void) {
#ifdef HAVE_NSVM_SAFE
	nsvmExit();
#endif
#ifdef HAVE_NSAM_CONTROL
	nsExit();
#endif
	smExit();
}

constexpr size_t NumServers = 0
#ifdef HAVE_NSVM_SAFE
	+ 1
#endif
#ifdef HAVE_NSAM_CONTROL
	+ 1
#endif
;

static_assert(NumServers > 0, "At least one feature should be enabled.");

constexpr size_t MaxSessions = 1
#ifdef HAVE_NSVM_SAFE
	+ NsVmMitmService::GetMaxSessions()
#endif
#ifdef HAVE_NSAM_CONTROL
	+ NsAm2MitmService::GetMaxSessions()
#endif
;

using MitmManager = ams::sf::hipc::ServerManager<NumServers, ams::sf::hipc::DefaultServerManagerOptions, MaxSessions>;

int main(int argc, char **argv)
{
	MitmManager serverManager;

	R_ASSERT(FileUtils::Initialize());

#ifdef HAVE_NSVM_SAFE
	R_ASSERT((serverManager.RegisterMitmServer<NsVmMitmService>(NsVmMitmService::GetServiceName())));
#endif
#ifdef HAVE_NSAM_CONTROL
	R_ASSERT((serverManager.RegisterMitmServer<NsAm2MitmService>(NsAm2MitmService::GetServiceName())));
#endif

	serverManager.LoopProcess();

	return 0;
}

