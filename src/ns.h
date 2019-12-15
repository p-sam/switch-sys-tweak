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

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	NacpVideoCaptureMode_Disabled = 0,
	NacpVideoCaptureMode_Manual = 1,
	NacpVideoCaptureMode_Automatic = 2,
} PACKED NacpVideoCaptureMode;

typedef struct {
	NacpLanguageEntry lang_entries[16];

	char isbn[37];
	u8 startup_user_account;
	u8 user_account_switch_lock;
	u8 addon_content_registration_type;
	u32 app_attribute;
	u32 supported_languages;
	u32 parental_control;
	u8 screenshot;
	NacpVideoCaptureMode video_capture_mode;
	u8 data_loss_confirmation;
	u8 playlog_policy;
	u64 presence_group_title_id;
	char rating_rage[32];
	char display_version[10];
	u64 addon_content_base_title_id;
	u64 savedata_owner_id;
	u64 user_account_savedata_size;
	u64 user_account_savedata_journal_size;
	u64 device_savedata_size;
	u64 device_savedata_journal_size;
	u64 bcat_delivery_cache_storage_size;
	u64 application_error_code_category;
	u64 local_communication_ids[8];
	u8 logo_type;
	u8 logo_handling;
	u8 runtime_addon_content_install;
	char unk_x30F3[3];
	u8 crash_report;
	u8 hdcp;
	u64 seed_for_pseudo_device_id;
	char bcat_passphrase[65];
	char unk_x3141[7];
	u64 user_account_savedata_max_size;
	u64 user_account_savedata_max_journal_size;
	u64 device_savedata_max_size;
	u64 device_savedata_max_journal_size;
	u64 temporary_storage_size;
	u64 cache_storage_size;
	u64 cache_storage_journal_size;
	u64 cache_storage_max_size;
	u64 cache_storage_max_index;
	u64 playlog_queryable_title_ids[16];
	u8 playlog_query_capability;
	u8 repair;
	u8 program_index;
	u8 required_network_service_license_on_launch;
	char unk_x3214[3564];
} Nacp;

typedef struct {
	Nacp nacp;
	char icon[0x20000];
} NsAppControlData;

#ifdef __cplusplus
}
#endif