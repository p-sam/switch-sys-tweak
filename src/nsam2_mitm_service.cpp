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
#include "nsam2_mitm_service.hpp"
#include "file_utils.hpp"
#include "ini.h"

#define NSAM2_MITM_SERVICE_NAME "ns:am2"

static int _ProcessControlDataIniHandler(void *user, const char *section, const char *name, const char *value) {
	NsAppControlData* data = (NsAppControlData*)user;

	if (strcasecmp(section, "override_nacp") == 0) {
		if (strcasecmp(name, "name") == 0) {
			for(unsigned int i = 0; i < sizeof(data->nacp.lang_entries) / sizeof(data->nacp.lang_entries[0]); i++) {
				strncpy(data->nacp.lang_entries[i].name, value, sizeof(data->nacp.lang_entries[i].name)-1);
			}
		} else if (strcasecmp(name, "author") == 0) {
			for(unsigned int i = 0; i < sizeof(data->nacp.lang_entries) / sizeof(data->nacp.lang_entries[0]); i++) {
				strncpy(data->nacp.lang_entries[i].author, value, sizeof(data->nacp.lang_entries[i].author)-1);
			}
		} else if (strcasecmp(name, "display_version") == 0) {
			strncpy(data->nacp.display_version, value, sizeof(data->nacp.display_version)-1);
		}
	}

	return 1;
}

static void _ProcessControlData(u64 tid, NsAppControlData* data, u64* size) {
	char path[FS_MAX_PATH] = {0};
	snprintf(path, sizeof(path)-1, "/atmosphere/titles/%016lx/config.ini", tid);
	ini_parse(path, _ProcessControlDataIniHandler, data);
	snprintf(path, sizeof(path)-1, "/atmosphere/titles/%016lx/icon.jpg", tid);
	
	FILE* f = fopen(path, "rb");
	if(f != NULL) {
		size_t read = fread(&data->icon, 1, sizeof(data->icon), f);
		*size = sizeof(data->nacp) + read;
		fclose(f);
	}
}

void NsAm2MitmService::PostProcess(IMitmServiceObject *obj, IpcResponseContext *ctx) {}

bool NsAm2MitmService::ShouldMitm(u64 pid, u64 tid) {
	bool should_mitm = (tid == TitleId_AppletQlaunch);
	FileUtils::LogLine("\"%s\"<>::ShouldMitm(%ld, 0x%016lx); // %s", NSAM2_MITM_SERVICE_NAME, pid, tid, should_mitm ? "true" : "false");
	return should_mitm;
}

Result NsAm2MitmService::GetROAppControlDataInterface(Out<std::shared_ptr<NsROAppControlDataService>> out_intf) {
	std::shared_ptr<NsROAppControlDataService> intf = nullptr;
	u32 out_domain_id = 0;
	Result rc = ResultSuccess;

	ON_SCOPE_EXIT {
		if (R_SUCCEEDED(rc)) {
			out_intf.SetValue(std::move(intf));
			if (out_intf.IsDomain()) {
				out_intf.ChangeObjectId(out_domain_id);
			}
		}
	};

	Service s;
	rc = nsGetROAppControlDataInterfaceFwd(this->forward_service.get(), NsAm2Cmd_GetROAppControlDataInterface, &s);
	if (R_SUCCEEDED(rc)) {
		intf = std::make_shared<NsROAppControlDataService>(this->title_id, this->process_id, s);
		if (out_intf.IsDomain()) {
			out_domain_id = s.object_id;
		}
	}

	FileUtils::LogLine("\"%s\"<%ld|0x%016lx>::GetROAppControlDataInterface(); // %x", NSAM2_MITM_SERVICE_NAME, process_id, title_id, rc);
	return rc;
}

void NsAm2MitmService::AddToManager(SessionManagerBase *manager) {
	AddMitmServerToManager<NsAm2MitmService>(manager, NSAM2_MITM_SERVICE_NAME, 16);
}

Result NsROAppControlDataService::GetAppControlData(u8 flag, u64 tid, OutBuffer<char> buffer, Out<u64> out_size) {
	Result rc = nsGetAppControlDataFwd(this->srv.get(), NsROAppControlDataInterfaceCmd_GetAppControlData, flag, tid, buffer.buffer, buffer.num_elements, out_size.GetPointer());
	FileUtils::LogLine("\"%s\"<%ld|0x%016lx>::GetAppControlData(%u, 0x%016lx, buf[0x%lx]); // %x[0x%lx]", NSAM2_MITM_SERVICE_NAME, process_id, title_id, flag, tid, buffer.num_elements, rc, out_size.GetValue());

	if(R_SUCCEEDED(rc) && buffer.num_elements == sizeof(NsAppControlData)) {
		_ProcessControlData(tid, (NsAppControlData*)buffer.buffer, out_size.GetPointer());
	}
	return rc;
}

Result NsROAppControlDataService::GetAppDesiredLanguage(u32 bitmask, Out<u8> out_langentry) {
	Result rc = nsGetAppDesiredLanguageFwd(this->srv.get(), NsROAppControlDataInterfaceCmd_GetAppDesiredLanguage, bitmask, out_langentry.GetPointer());
	FileUtils::LogLine("\"%s\"<%ld|0x%016lx>::GetAppDesiredLanguage(0x%08x); // %x[%u]", NSAM2_MITM_SERVICE_NAME, process_id, title_id, bitmask, rc, out_langentry.GetValue());
	return rc;
}

Result NsROAppControlDataService::ConvertAppLanguageToLanguageCode(u8 langentry, Out<u64> out_langcode) {
	Result rc = nsConvertAppLanguageToLanguageCodeFwd(this->srv.get(), NsROAppControlDataInterfaceCmd_GetAppDesiredLanguage, langentry, out_langcode.GetPointer());
	FileUtils::LogLine("\"%s\"<%ld|0x%016lx>::ConvertAppLanguageToLanguageCode(0x%02x); // %x[%u]", NSAM2_MITM_SERVICE_NAME, process_id, title_id, langentry, rc, out_langcode.GetValue());
	return rc;
}

Result NsROAppControlDataService::ConvertLanguageCodeToAppLanguage(u64 langcode, Out<u8> out_langentry) {
	Result rc = nsConvertLanguageCodeToAppLanguageFwd(this->srv.get(), NsROAppControlDataInterfaceCmd_GetAppDesiredLanguage, langcode, out_langentry.GetPointer());
	FileUtils::LogLine("\"%s\"<%ld|0x%016lx>::ConvertLanguageCodeToAppLanguage(0x%016lx); // %x[0x%02x]", NSAM2_MITM_SERVICE_NAME, process_id, title_id, langcode, rc, out_langentry.GetValue());
	return rc;
}