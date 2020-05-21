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
#include <stratosphere.hpp>

#include "ns.h"

#define NSAM2_MITM_SERVICE_NAME "ns:am2"

class NsROAppControlDataService : public ams::sf::IServiceObject {
	private:
		enum class CommandId : u32 {
			GetAppControlData = 0,
			GetAppDesiredLanguage = 1,
			ConvertAppLanguageToLanguageCode = 2,
			ConvertLanguageCodeToAppLanguage = 3,
		};
	private:
		ams::sm::MitmProcessInfo client_info;
		std::unique_ptr<Service> srv;
	public:
		NsROAppControlDataService(const ams::sm::MitmProcessInfo &cl, std::unique_ptr<Service> s) : client_info(cl), srv(std::move(s)) {}

		virtual ~NsROAppControlDataService() {
			serviceClose(srv.get());
		}
	private:
		ams::Result GetAppControlData(u8 flag, u64 tid, const ams::sf::OutBuffer &buffer, ams::sf::Out<u64> out_count);
		ams::Result GetAppDesiredLanguage(u32 bitmask, ams::sf::Out<u8> out_langentry);
		ams::Result ConvertAppLanguageToLanguageCode(u8 langentry, ams::sf::Out<u64> langcode);
		ams::Result ConvertLanguageCodeToAppLanguage(u64 langcode, ams::sf::Out<u8> langentry);
	public:
		DEFINE_SERVICE_DISPATCH_TABLE {
			MAKE_SERVICE_COMMAND_META(GetAppControlData),
			MAKE_SERVICE_COMMAND_META(GetAppDesiredLanguage),
			MAKE_SERVICE_COMMAND_META(ConvertAppLanguageToLanguageCode),
			MAKE_SERVICE_COMMAND_META(ConvertLanguageCodeToAppLanguage),
		};
};

class NsAm2MitmService : public ams::sf::IMitmServiceObject {
	private:
		enum class CommandId : u32 {
			GetROAppControlDataInterface = 7989,
		};
	public:
		NsAm2MitmService(std::shared_ptr<Service> &&s, const ams::sm::MitmProcessInfo &c) : ams::sf::IMitmServiceObject(std::forward<std::shared_ptr<Service>>(s), c) {}

		static bool ShouldMitm(const ams::sm::MitmProcessInfo &client_info);

	protected:
		/* Overridden commands. */
		ams::Result GetROAppControlDataInterface(ams::sf::Out<std::shared_ptr<NsROAppControlDataService>> out);
	public:
		DEFINE_SERVICE_DISPATCH_TABLE {
			MAKE_SERVICE_COMMAND_META(GetROAppControlDataInterface),
		};

		static constexpr ams::sm::ServiceName GetServiceName() {
			return ams::sm::ServiceName::Encode(NSAM2_MITM_SERVICE_NAME);
		}

		static constexpr size_t GetMaxSessions() {
			return 4;
		}
};