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

#include "libams.hpp"
#include "ns.h"

enum NsROAppControlDataInterfaceCmdId : u32 {
	GetAppControlData = 0,
	GetAppDesiredLanguage = 1,
	ConvertAppLanguageToLanguageCode = 2,
	ConvertLanguageCodeToAppLanguage = 3,
};

#define NS_RO_APP_CONTROL_DATA_INTERFACE_INFO(C, H) \
	AMS_SF_METHOD_INFO_F(C, H, NsROAppControlDataInterfaceCmdId, GetAppControlData, (u8 flag, u64 tid, const ams::sf::OutBuffer &buffer, ams::sf::Out<u64> out_count), (flag, tid, buffer, out_count)) \
	AMS_SF_METHOD_INFO_F(C, H, NsROAppControlDataInterfaceCmdId, GetAppDesiredLanguage, (u32 bitmask, ams::sf::Out<u8> out_langentry), (bitmask, out_langentry)) \
	AMS_SF_METHOD_INFO_F(C, H, NsROAppControlDataInterfaceCmdId, ConvertAppLanguageToLanguageCode, (u8 langentry, ams::sf::Out<u64> langcode), (langentry, langcode)) \
	AMS_SF_METHOD_INFO_F(C, H, NsROAppControlDataInterfaceCmdId, ConvertLanguageCodeToAppLanguage, (u64 langcode, ams::sf::Out<u8> langentry), (langcode, langentry))

AMS_SF_DEFINE_INTERFACE_F(NsROAppControlDataInterface, NS_RO_APP_CONTROL_DATA_INTERFACE_INFO);

class NsROAppControlDataService {
	private:
		ams::sm::MitmProcessInfo client_info;
		std::unique_ptr<Service> srv;
	public:
		NsROAppControlDataService(const ams::sm::MitmProcessInfo &cl, std::unique_ptr<Service> s) : client_info(cl), srv(std::move(s)) {}

		virtual ~NsROAppControlDataService() {
			serviceClose(srv.get());
		}

		constexpr const char* GetDisplayName() {
			return "NsROAppControlDataInterface";
		}

		NS_RO_APP_CONTROL_DATA_INTERFACE_INFO(_, AMS_SF_DECLARE_INTERFACE_METHODS);
};
static_assert(IsNsROAppControlDataInterface<NsROAppControlDataService>);

enum class NsSrvGetterCmdId : u32 {
	GetROAppControlDataInterface = 7989,
};

#define NS_SRV_GETTER_MITM_INTERFACE_INFO(C, H) \
	AMS_SF_METHOD_INFO_F(C, H, NsSrvGetterCmdId, GetROAppControlDataInterface, (ams::sf::Out<ams::sf::SharedPointer<NsROAppControlDataInterface>> out), (out))

AMS_SF_DEFINE_MITM_INTERFACE_F(NsServiceGetterMitmInterface, NS_SRV_GETTER_MITM_INTERFACE_INFO);

class NsServiceGetterMitmService : public ams::sf::MitmServiceImplBase {
	public:
		using ::ams::sf::MitmServiceImplBase::MitmServiceImplBase;
		NS_SRV_GETTER_MITM_INTERFACE_INFO(_, AMS_SF_DECLARE_INTERFACE_METHODS);

		constexpr const char* GetDisplayName() {
			return "NsServiceGetterInterface";
		}
};

#define NSAM2_MITM_SERVICE_NAME "ns:am2"

class NsAm2MitmService : public NsServiceGetterMitmService {
	public:
		using ::NsServiceGetterMitmService::NsServiceGetterMitmService;
		static bool ShouldMitm(const ams::sm::MitmProcessInfo& client_info);

		constexpr const char* GetDisplayName() {
			return NSAM2_MITM_SERVICE_NAME;
		}

		static constexpr ams::sm::ServiceName GetServiceName() {
			return ams::sm::ServiceName::Encode(NSAM2_MITM_SERVICE_NAME);
		}

		static constexpr size_t GetMaxSessions() {
			return 4;
		}
};
static_assert(IsNsServiceGetterMitmInterface<NsAm2MitmService>);

#define NSRO_MITM_SERVICE_NAME "ns:ro"

class NsRoMitmService : public NsServiceGetterMitmService {
	public:
		using ::NsServiceGetterMitmService::NsServiceGetterMitmService;
		static bool ShouldMitm(const ams::sm::MitmProcessInfo& client_info);

		constexpr const char* GetDisplayName() {
			return NSRO_MITM_SERVICE_NAME;
		}

		static constexpr ams::sm::ServiceName GetServiceName() {
			return ams::sm::ServiceName::Encode(NSRO_MITM_SERVICE_NAME);
		}

		static constexpr size_t GetMaxSessions() {
			return 4;
		}
};
static_assert(IsNsServiceGetterMitmInterface<NsRoMitmService>);
