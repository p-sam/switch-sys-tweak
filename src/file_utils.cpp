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
 
#include <switch.h>
#include <stratosphere.hpp>
#include <atomic>
#include <algorithm>
#include <strings.h>
#include <stdarg.h>

#include "file_utils.hpp"

static FsFileSystem g_sd_filesystem = {0};
static HosMutex g_log_mutex;
static std::atomic_bool g_has_initialized = false;

void FileUtils::_InitializeThreadFunc(void *args) {
	while (R_FAILED(fsInitialize())) {
		svcSleepThread(1000ULL);
	}
	
	while (R_FAILED(fsMountSdcard(&g_sd_filesystem))) {
		svcSleepThread(1000ULL);
	}
	
	g_has_initialized = true;

	FILE_LOG("=== boot ===\n");

	svcExitThread();
}

bool FileUtils::IsInitialized() {
	return g_has_initialized;
}

void FileUtils::Log(const char *format, ...) {
	std::scoped_lock<HosMutex> lock(g_log_mutex);
	va_list args;
	va_start(args, format);
	if(g_has_initialized) {
		fsFsCreateFile(&g_sd_filesystem, "/" TARGET ".log", 0, 0);
		FsFile log_file = {0};
		if(R_SUCCEEDED(fsFsOpenFile(&g_sd_filesystem, "/" TARGET ".log", FS_OPEN_WRITE | FS_OPEN_APPEND, &log_file))) {
			u64 offset = 0;
			if(R_SUCCEEDED(fsFileGetSize(&log_file, &offset))) {
				size_t len = vsnprintf(NULL, 0, format, args) * sizeof(char);
				char *buf = (char*)malloc(len + 1);
				va_start(args, format);
				vsnprintf(buf, len+1, format, args);
				if(R_SUCCEEDED(fsFileWrite(&log_file, offset, buf, len))) {
					fsFileFlush(&log_file);
				}
				free(buf);
			}
			fsFileClose(&log_file);
		}
	}
	va_end(args);
}

void FileUtils::InitializeAsync() {
	Thread init_thread = {0};
	if (R_SUCCEEDED(threadCreate(&init_thread, &FileUtils::_InitializeThreadFunc, NULL, 0x4000, 0x15, 0))) {
		threadStart(&init_thread);
	}
}