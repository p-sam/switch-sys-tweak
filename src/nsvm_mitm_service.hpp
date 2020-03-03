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
#include "ams.hpp"

#define NSVM_MITM_SERVICE_NAME "ns:vm"

class NsVmMitmService : public ams::sf::IMitmServiceObject {
	public:
		enum class CommandId : u32 {
			NeedsUpdateVulnerability = 1200,
		};
	public:
		NsVmMitmService(std::shared_ptr<Service> &&s, const ams::sm::MitmProcessInfo &c) : ams::sf::IMitmServiceObject(std::forward<std::shared_ptr<Service>>(s), c) {}

		static bool ShouldMitm(const ams::sm::MitmProcessInfo &client_info);

	protected:
		/* Overridden commands. */
		ams::Result NeedsUpdateVulnerability(ams::sf::Out<u8> out);
	public:
		DEFINE_SERVICE_DISPATCH_TABLE {
			MAKE_SERVICE_COMMAND_META(NeedsUpdateVulnerability),
		};

		static constexpr ams::sm::ServiceName GetServiceName() {
			return ams::sm::ServiceName::Encode(NSVM_MITM_SERVICE_NAME);
		}

		static constexpr size_t GetMaxSessions() {
			return 16;
		}
};