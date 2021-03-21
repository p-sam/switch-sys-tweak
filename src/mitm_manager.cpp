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

#include "mitm_manager.hpp"
#include "file_utils.hpp"

ams::Result MitmManager::OnNeedsToAccept(int port_index, Server* server) {
	std::shared_ptr<Service> fsrv;
	ams::sm::MitmProcessInfo client_info;
	server->AcknowledgeMitmSession(std::addressof(fsrv), std::addressof(client_info));

#define _MITM_ACCEPT(INTERFACE, SERVICE) \
	this->AcceptMitmImpl(server, ams::sf::CreateSharedObjectEmplaced<INTERFACE, SERVICE>(decltype(fsrv)(fsrv), client_info), fsrv)

	switch (port_index) {
#ifdef HAVE_NSVM_SAFE
		case MitmManagerPort_NsVm:
			return _MITM_ACCEPT(NsVmMitmInterface, NsVmMitmService);
#endif
#ifdef HAVE_NSAM_CONTROL
		case MitmManagerPort_NsAm2:
			return _MITM_ACCEPT(NsServiceGetterMitmInterface, NsAm2MitmService);
#endif
#ifdef HAVE_NSRO_CONTROL
		case MitmManagerPort_NsRo:
			return _MITM_ACCEPT(NsServiceGetterMitmInterface, NsRoMitmService);
#endif
		AMS_UNREACHABLE_DEFAULT_CASE();
	}

#undef _MITM_ACCEPT
}

ams::Result MitmManager::RegisterServers() {
#define _MITM_REGISTER_SERVER(SERVICE, PORT) \
	FileUtils::LogLine("Registering %s", #SERVICE); \
	R_TRY((this->RegisterMitmServer<SERVICE>(PORT, SERVICE::GetServiceName())))

#ifdef HAVE_NSVM_SAFE
	 _MITM_REGISTER_SERVER(NsVmMitmService, MitmManagerPort_NsVm);
#endif
#ifdef HAVE_NSAM_CONTROL
	 _MITM_REGISTER_SERVER(NsAm2MitmService, MitmManagerPort_NsAm2);
#endif
#ifdef HAVE_NSRO_CONTROL
	 _MITM_REGISTER_SERVER(NsRoMitmService, MitmManagerPort_NsRo);
#endif

#undef _MITM_REGISTER_SERVER

	return ams::ResultSuccess();
}
