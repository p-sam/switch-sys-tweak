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
#include <stratosphere.hpp>

static Mutex g_log_mutex;
static std::atomic_bool g_has_initialized = false;

extern "C" void __libnx_init_time(void);

static void _FileUtils_InitializeThreadFunc(void *args) {
	FileUtils::Initialize();
	svcExitThread();
}

bool FileUtils::IsInitialized() {
	return g_has_initialized;
}

void FileUtils::LogLine(const char *format, ...) {
#ifdef ENABLE_LOGGING
	va_list args;
	va_start(args, format);
	if (g_has_initialized) {
		mutexLock(&g_log_mutex);

		FILE *file = fopen(FILE_LOG_FILE_PATH, "a");
		if (file) {
			time_t timer  = time(NULL);
			struct tm* timerTm = localtime(&timer);

			va_start(args, format);
			fprintf(file, "[%04d-%02d-%02d %02d:%02d:%02d] ", timerTm->tm_year+1900, timerTm->tm_mon+1, timerTm->tm_mday, timerTm->tm_hour, timerTm->tm_min, timerTm->tm_sec);
			vfprintf(file, format, args);
			fprintf(file, "\n");
			fclose(file);
		}
		mutexUnlock(&g_log_mutex);
	}
	va_end(args);
#endif
}

void FileUtils::InitializeAsync() {
	Thread initThread = {0};
	threadCreate(&initThread, _FileUtils_InitializeThreadFunc, NULL, NULL, 0x4000, 0x15, 0);
	threadStart(&initThread);
}

Result FileUtils::Initialize() {
	Result rc = 0;

	mutexInit(&g_log_mutex);

	if (R_SUCCEEDED(rc)) {
		rc = timeInitialize();
	}

	__libnx_init_time();

	if (R_SUCCEEDED(rc)) {
		rc = fsInitialize();
	}

	if (R_SUCCEEDED(rc)) {
		rc = fsdevMountSdmc();
	}

	if (R_SUCCEEDED(rc)) {
		g_has_initialized = true;

		FileUtils::LogLine("=== " TARGET " ===");
	}

	return rc;
}

void FileUtils::Exit() {
	if (!g_has_initialized) {
		return;
	}

	g_has_initialized = false;

	fsdevUnmountAll();
	fsExit();
	timeExit();
}
