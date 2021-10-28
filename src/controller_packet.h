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

#include <switch.h>

#define CONTROLLER_PACKET_MAGIC (0x33434332)
#define CONTROLLER_PACKET_STICK_MAX (0x9C)

enum {
	CONTROLLER_PACKET_KEY_A = BIT(0),
	CONTROLLER_PACKET_KEY_B = BIT(1),
	CONTROLLER_PACKET_KEY_SELECT = BIT(2),
	CONTROLLER_PACKET_KEY_START = BIT(3),
	CONTROLLER_PACKET_KEY_DRIGHT = BIT(4),
	CONTROLLER_PACKET_KEY_DLEFT = BIT(5),
	CONTROLLER_PACKET_KEY_DUP = BIT(6),
	CONTROLLER_PACKET_KEY_DDOWN = BIT(7),
	CONTROLLER_PACKET_KEY_R = BIT(8),
	CONTROLLER_PACKET_KEY_L = BIT(9),
	CONTROLLER_PACKET_KEY_X = BIT(10),
	CONTROLLER_PACKET_KEY_Y = BIT(11),
	CONTROLLER_PACKET_KEY_LSTICK = BIT(12),
	CONTROLLER_PACKET_KEY_RSTICK = BIT(13),
	CONTROLLER_PACKET_KEY_ZL = BIT(14),
	CONTROLLER_PACKET_KEY_ZR = BIT(15),
	CONTROLLER_PACKET_KEY_EXIT = BIT(17),
	CONTROLLER_PACKET_KEY_CAPTURE = BIT(18),
	CONTROLLER_PACKET_KEY_HOME = BIT(19),
};

typedef struct {
	s16 dx;
	s16 dy;
} ControllerPacketStick;

typedef struct {
	s16 x;
	s16 y;
	s16 z;
} ControllerPacketVector;

typedef struct {
	u32 magic; // CONTROLLER_PACKET_MAGIC
	u64 tick;
	u32 keysHeld;
	ControllerPacketStick leftStick;
	ControllerPacketStick rightStick;
} ControllerPacket;

