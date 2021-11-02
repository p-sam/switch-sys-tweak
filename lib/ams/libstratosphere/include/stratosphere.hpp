/*
 * Copyright (c) 2018-2020 Atmosphère-NX
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

/* libvapours (pulls in util, svc, results). */
#include <vapours.hpp>

/* Libstratosphere definitions. */
#include <stratosphere/ams/impl/ams_system_thread_definitions.hpp>

/* Sadly required shims. */
#include <stratosphere/svc/svc_stratosphere_shims.hpp>

/* Critical modules with no dependencies. */
#include <stratosphere/ams.hpp>
#include <stratosphere/os.hpp>
#include <stratosphere/dd.hpp>
#include <stratosphere/lmem.hpp>
#include <stratosphere/mem.hpp>

/* Pull in all ID definitions from NCM. */
#include <stratosphere/ncm/ncm_ids.hpp>

/* At this point, just include the rest alphabetically. */
/* TODO: Figure out optimal order. */
#include <stratosphere/hos.hpp>
#include <stratosphere/ncm.hpp>
#include <stratosphere/sf.hpp>
#include <stratosphere/sm.hpp>
#include <stratosphere/spl.hpp>
#include <stratosphere/tipc.hpp>
