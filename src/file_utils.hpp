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
#include <switch.h>
#include <stratosphere.hpp>

#ifdef ENABLE_LOGGING
	#define FILE_LOG(...) FileUtils::Log(__VA_ARGS__)
#else
	#define FILE_LOG(...) (void)0
#endif

class FileUtils {
	public:
		static void _InitializeThreadFunc(void *args);
		static void InitializeAsync();
		static bool IsInitialized();
		static void Log(const char *format, ...);
};