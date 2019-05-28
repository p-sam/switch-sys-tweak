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
#include "ns_shim.h"

typedef enum {
	NsAm2Cmd_GetROAppControlDataInterface = 7989
} NsAm2Cmd;

typedef enum {
	NsROAppControlDataInterfaceCmd_GetAppControlData = 0,
	NsROAppControlDataInterfaceCmd_GetAppDesiredLanguage = 1,
	NsROAppControlDataInterfaceCmd_ConvertAppLanguageToLanguageCode = 2,
	NsROAppControlDataInterfaceCmd_ConvertLanguageCodeToAppLanguage = 3,
} NsROAppControlDataInterfaceCmd;

class NsROAppControlDataService : public IServiceObject {
	private:
		u64 title_id;
		u64 process_id;
		std::unique_ptr<Service> srv;
	public:
		NsROAppControlDataService(u64 tid, u64 pid, Service *s) : title_id(tid), process_id(pid), srv(s) {
			/* ... */
		}

		NsROAppControlDataService(u64 tid, u64 pid, std::unique_ptr<Service> s) : title_id(tid), process_id(pid), srv(std::move(s)) {
			/* ... */
		}

		NsROAppControlDataService(u64 tid, u64 pid, Service s) : title_id(tid), process_id(pid) {
			srv = std::make_unique<Service>(s);
		}

		virtual ~NsROAppControlDataService() {
			serviceClose(srv.get());
		}
	private:
		Result GetAppControlData(u8 flag, u64 tid, OutBuffer<char> buffer, Out<u64> out_count);
		Result GetAppDesiredLanguage(u32 bitmask, Out<u8> out_langentry);
		Result ConvertAppLanguageToLanguageCode(u8 langentry, Out<u64> langcode);
		Result ConvertLanguageCodeToAppLanguage(u64 langcode, Out<u8> langentry);
	public:
		DEFINE_SERVICE_DISPATCH_TABLE {
			MakeServiceCommandMeta<NsROAppControlDataInterfaceCmd_GetAppControlData, &NsROAppControlDataService::GetAppControlData>(),
			MakeServiceCommandMeta<NsROAppControlDataInterfaceCmd_GetAppDesiredLanguage, &NsROAppControlDataService::GetAppDesiredLanguage>(),
			MakeServiceCommandMeta<NsROAppControlDataInterfaceCmd_ConvertAppLanguageToLanguageCode, &NsROAppControlDataService::ConvertAppLanguageToLanguageCode>(),
			MakeServiceCommandMeta<NsROAppControlDataInterfaceCmd_ConvertLanguageCodeToAppLanguage, &NsROAppControlDataService::ConvertLanguageCodeToAppLanguage>(),
		};
};

class NsAm2MitmService : public IMitmServiceObject {
	public:
		NsAm2MitmService(std::shared_ptr<Service> s, u64 pid) : IMitmServiceObject(s, pid) {}

		static bool ShouldMitm(u64 pid, u64 tid);

		static void PostProcess(IMitmServiceObject *obj, IpcResponseContext *ctx);

	protected:
		/* Overridden commands. */
		Result GetROAppControlDataInterface(Out<std::shared_ptr<NsROAppControlDataService>> out_intf);
	public:
		DEFINE_SERVICE_DISPATCH_TABLE {
			MakeServiceCommandMeta<NsAm2Cmd_GetROAppControlDataInterface, &NsAm2MitmService::GetROAppControlDataInterface>(),
		};
		static void AddToManager(SessionManagerBase *manager);
};