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

#pragma once
#include <stratosphere.hpp>
#include <time.h>
#include <vector>
#include <string>
#include <atomic>
#include <cstdarg>

#define FILE_LOG_FILE_PATH "/" TARGET ".txt"

class FileUtils {
	public:
		static void Exit();
		static ams::Result Initialize();
		static bool IsInitialized();
		static bool WaitInitialized();
		static ams::Result InitializeAsync();
		static void LogLine(const char *format, ...);
};
