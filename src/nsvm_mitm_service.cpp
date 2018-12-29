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
#include "nsvm_mitm_service.hpp"
#include "file_utils.hpp"

#define NSVM_MITM_SERVICE_NAME "ns:vm"

void NsVmMitmService::PostProcess(IMitmServiceObject *obj, IpcResponseContext *ctx) {}

bool NsVmMitmService::ShouldMitm(u64 pid, u64 tid) {
	bool should_mitm = true;//(tid == 0x010000000000100AULL || tid == 0x010000000000100FULL); // Webapplet online + offline
	FILE_LOG("\"%s\"<>::ShouldMitm(%ld, 0x%lx); // %s\n", NSVM_MITM_SERVICE_NAME, pid, tid, should_mitm ? "true" : "false");
	return should_mitm;
}

Result NsVmMitmService::NeedsUpdateVulnerability(Out<u8> out) {
	Result rc = 0;
	out.SetValue(0);
	FILE_LOG("\"%s\"<%ld|0x%lx>::NeedsUpdateVulnerability(); // %x[%x]\n", NSVM_MITM_SERVICE_NAME, process_id, title_id, out.GetValue(), rc);
	return rc;
}

void NsVmMitmService::AddToManager(SessionManagerBase *manager) {
	AddMitmServerToManager<NsVmMitmService>(manager, NSVM_MITM_SERVICE_NAME, 4);
}