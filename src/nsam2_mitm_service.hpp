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
	AMS_SF_METHOD_INFO_F(C, H, NsROAppControlDataInterfaceCmdId, GetAppControlData, (u8 flag, u64 tid, const ams::sf::OutBuffer &buffer, ams::sf::Out<u64> out_count)) \
	AMS_SF_METHOD_INFO_F(C, H, NsROAppControlDataInterfaceCmdId, GetAppDesiredLanguage, (u32 bitmask, ams::sf::Out<u8> out_langentry)) \
	AMS_SF_METHOD_INFO_F(C, H, NsROAppControlDataInterfaceCmdId, ConvertAppLanguageToLanguageCode, (u8 langentry, ams::sf::Out<u64> langcode)) \
	AMS_SF_METHOD_INFO_F(C, H, NsROAppControlDataInterfaceCmdId, ConvertLanguageCodeToAppLanguage, (u64 langcode, ams::sf::Out<u8> langentry))

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

		NS_RO_APP_CONTROL_DATA_INTERFACE_INFO(_, AMS_SF_IMPL_DECLARE_INTERFACE_METHODS);
};
static_assert(IsNsROAppControlDataInterface<NsROAppControlDataService>);

enum class NsAm2CmdId : u32 {
	GetROAppControlDataInterface = 7989,
};

#define NSAM2_MITM_SERVICE_NAME "ns:am2"

#define NS_AM2_MITM_INTERFACE_INFO(C, H) \
	AMS_SF_METHOD_INFO_F(C, H, NsAm2CmdId, GetROAppControlDataInterface, (ams::sf::Out<std::shared_ptr<NsROAppControlDataInterface>> out))

AMS_SF_DEFINE_MITM_INTERFACE_F(NsAm2MitmInterface, NS_AM2_MITM_INTERFACE_INFO);

class NsAm2MitmService : public ams::sf::MitmServiceImplBase {
	public:
		using ::ams::sf::MitmServiceImplBase::MitmServiceImplBase;
		static bool ShouldMitm(const ams::sm::MitmProcessInfo& client_info);

		NS_AM2_MITM_INTERFACE_INFO(_, AMS_SF_IMPL_DECLARE_INTERFACE_METHODS)

		static constexpr ams::sm::ServiceName GetServiceName() {
			return ams::sm::ServiceName::Encode(NSAM2_MITM_SERVICE_NAME);
		}

		static constexpr size_t GetMaxSessions() {
			return 4;
		}
};
static_assert(IsNsAm2MitmInterface<NsAm2MitmService>);
