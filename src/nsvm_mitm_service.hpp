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

class NsVmMitmService : public IMitmServiceObject {
	public:
		enum class CommandId : u32 {
			NeedsUpdateVulnerability = 1200,
		};
	public:
		NsVmMitmService(std::shared_ptr<Service> s, u64 pid, sts::ncm::TitleId tid) : IMitmServiceObject(s, pid, tid) {}

		static bool ShouldMitm(u64 pid, sts::ncm::TitleId tid);

		static void PostProcess(IMitmServiceObject *obj, IpcResponseContext *ctx);

	protected:
		/* Overridden commands. */
		Result NeedsUpdateVulnerability(Out<u8> out);
	public:
		DEFINE_SERVICE_DISPATCH_TABLE {
			MAKE_SERVICE_COMMAND_META(NsVmMitmService, NeedsUpdateVulnerability),
		};
		static void AddToManager(SessionManagerBase *manager);
};