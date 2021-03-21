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

enum NsVmCmdId : u32 {
	NeedsUpdateVulnerability = 1200,
};

#define NSVM_MITM_INTERFACE_INFO(C, H) \
	AMS_SF_METHOD_INFO_F(C, H, NsVmCmdId, NeedsUpdateVulnerability, (ams::sf::Out<u8> out), (out))

AMS_SF_DEFINE_MITM_INTERFACE_F(NsVmMitmInterface, NSVM_MITM_INTERFACE_INFO);

#define NSVM_MITM_SERVICE_NAME "ns:vm"

class NsVmMitmService : public ams::sf::MitmServiceImplBase {
	public:
		using ::ams::sf::MitmServiceImplBase::MitmServiceImplBase;
		static bool ShouldMitm(const ams::sm::MitmProcessInfo& client_info);

		NSVM_MITM_INTERFACE_INFO(_, AMS_SF_DECLARE_INTERFACE_METHODS);

		constexpr const char* GetDisplayName() {
			return NSVM_MITM_SERVICE_NAME;
		}

		static constexpr ams::sm::ServiceName GetServiceName() {
			return ams::sm::ServiceName::Encode(NSVM_MITM_SERVICE_NAME);
		}

		static constexpr size_t GetMaxSessions() {
			return 16;
		}
};
static_assert(IsNsVmMitmInterface<NsVmMitmService>);
