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

#include "file_utils.hpp"

static ams::os::Mutex g_log_mutex{false};
static ams::os::ThreadType g_init_thread;
static std::atomic_bool g_has_initialized = false;

extern "C" void __libnx_init_time(void);
static void _FileUtils_InitializeThreadFunc(void* args) {
	R_ABORT_UNLESS(FileUtils::Initialize());
}

bool FileUtils::IsInitialized() {
	return g_has_initialized;
}

bool FileUtils::WaitInitialized() {
	if(!g_has_initialized) {
		if(g_init_thread.state == ams::os::ThreadType::State_NotInitialized) {
			return false;
		}
		ams::os::WaitThread(&g_init_thread);
	}
	return true;
}

void FileUtils::LogLine(const char* format, ...) {
#ifdef ENABLE_LOGGING
	va_list args;
	va_start(args, format);
	if (g_has_initialized) {
		std::scoped_lock lock(g_log_mutex);

		FILE* file = fopen(FILE_LOG_FILE_PATH, "a");
		if (file) {
			time_t timer  = time(NULL);
			struct tm* timerTm = localtime(&timer);

			va_start(args, format);
			fprintf(file, "[%04d-%02d-%02d %02d:%02d:%02d] ", timerTm->tm_year+1900, timerTm->tm_mon+1, timerTm->tm_mday, timerTm->tm_hour, timerTm->tm_min, timerTm->tm_sec);
			vfprintf(file, format, args);
			fprintf(file, "\n");
			fclose(file);
		}
	}
	va_end(args);
#endif
}

ams::Result FileUtils::InitializeAsync() {
	s32 currentPriority = ams::os::GetThreadPriority(ams::os::GetCurrentThread());

	R_TRY(ams::os::CreateThread(&g_init_thread, &_FileUtils_InitializeThreadFunc, NULL, NULL, 0x2000, currentPriority));
	ams::os::StartThread(&g_init_thread);

	return ams::ResultSuccess();
}

ams::Result FileUtils::Initialize() {
#if ENABLE_LOGGING
	R_TRY(timeInitialize());

	__libnx_init_time();
	timeExit();
#endif

	R_TRY(fsInitialize());
	R_TRY(fsdevMountSdmc());

	g_has_initialized = true;
	FileUtils::LogLine("=== " TARGET " ===");

	return ams::ResultSuccess();
}

void FileUtils::Exit() {
	ams::os::WaitThread(&g_init_thread);
	ams::os::DestroyThread(&g_init_thread);

	if (!g_has_initialized) {
		return;
	}

	g_has_initialized = false;

	fsdevUnmountAll();
	fsExit();
	timeExit();
}
