/*
 * Copyright (c) 2018 Atmosphère-NX
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

#include <mutex>
#include <switch.h>
#include "nsvm_mitm_service.hpp"

void NsVmMitmService::PostProcess(IMitmServiceObject *obj, IpcResponseContext *ctx) {
	/* No commands need postprocessing. */    
}

Result NsVmMitmService::NeedsUpdateVulnerability(Out<u8> out) {
	out.SetValue(0);
	return 0;
}