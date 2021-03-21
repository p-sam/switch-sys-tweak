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

// Avoid copying prototypes, cmd ids, etc as much as possible

#define AMS_CONCAT_NAMESPACE(A, B) A::B

#define AMS_SF_METHOD_INFO_F(CLASSNAME, HANDLER, ENUM_TYPE, NAME, ARGS, ARGNAMES) \
	AMS_SF_METHOD_INFO_9(CLASSNAME, HANDLER, (u32)AMS_CONCAT_NAMESPACE(ENUM_TYPE, NAME), ams::Result, NAME, ARGS, ARGNAMES, ams::hos::Version_Min, ams::hos::Version_Max)

#define AMS_SF_DECLARE_INTERFACE_METHODS(CLASSNAME, CMD_ID, RETURN, NAME, ARGS, ARGNAMES, VERSION_MIN, VERSION_MAX) \
	RETURN NAME ARGS;

#define AMS_SF_DEFINE_INTERFACE_F(NAME, MACRO) \
	AMS_SF_DEFINE_INTERFACE(ams::sf_interface, NAME, MACRO); \
	using AMS_CONCAT_NAMESPACE(::ams::sf_interface, NAME); \
	using AMS_CONCAT_NAMESPACE(::ams::sf_interface, Is##NAME)

#define AMS_SF_DEFINE_MITM_INTERFACE_F(NAME, MACRO) \
	AMS_SF_DEFINE_MITM_INTERFACE(ams::sf_mitm_interface, NAME, MACRO); \
	using AMS_CONCAT_NAMESPACE(::ams::sf_mitm_interface, NAME); \
	using AMS_CONCAT_NAMESPACE(::ams::sf_mitm_interface, Is##NAME)
