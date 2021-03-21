/*
 * Copyright (c) 2021 p-sam
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

#ifdef HAVE_NSVM_SAFE
#include "nsvm_mitm_service.hpp"
#endif

#if defined(HAVE_NSAM_CONTROL) || defined(HAVE_NSRO_CONTROL)
#include "ns_srvget_mitm_service.hpp"
#endif

enum {
#ifdef HAVE_NSVM_SAFE
	MitmManagerPort_NsVm,
#endif
#ifdef HAVE_NSAM_CONTROL
	MitmManagerPort_NsAm2,
#endif
#ifdef HAVE_NSRO_CONTROL
	MitmManagerPort_NsRo,
#endif
	MitmManagerPort_Count,
};

static constexpr size_t MitmManagerMaxSessions = 1
#ifdef HAVE_NSVM_SAFE
	+ NsVmMitmService::GetMaxSessions()
#endif
#ifdef HAVE_NSAM_CONTROL
	+ NsAm2MitmService::GetMaxSessions()
#endif
#ifdef HAVE_NSRO_CONTROL
	+ NsRoMitmService::GetMaxSessions()
#endif
;

class MitmManager : public ams::sf::hipc::ServerManager<MitmManagerPort_Count, ams::sf::hipc::DefaultServerManagerOptions, MitmManagerMaxSessions> {
	private:
		virtual ams::Result OnNeedsToAccept(int port_index, Server* server) override;
	public:
		ams::Result RegisterServers();

		static constexpr bool HasAtLeastOneServiceDefined() {
			return MitmManagerPort_Count > 0;
		}
};
