/*****************************************************************************
 * Copyright (C) 2011-2016 Michael Ira Krufky
 *
 * Author: Michael Ira Krufky <mkrufky@linuxtv.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "decoder.h"
//#include "dvbpsi/descriptor.h"

#include "functions.h"

#include "tabl_cd.h"

#define TABLEID 0xcd

#define CLASS_MODULE "[System Time Table (ATSC)]"

#define dPrintf(fmt, arg...) __dPrintf(DBG_DECODE, fmt, ##arg)

using namespace dvbtee::decode;
using namespace valueobj;

static std::string TABLE_NAME = "STT";

void stt::store(const dvbpsi_atsc_stt_t * const p_stt)
{
	if (!p_stt) return;

	time_t o_time = get<time_t>("time");

	time_t m_time = atsc_datetime_utc(p_stt->i_system_time);

	//dbg_time("%s", ctime(&m_time));

	descriptors.decode(p_stt->p_first_descriptor);

	set<Array>("descriptors", descriptors);

	set("time", m_time);

	setValid(true);

	dPrintf("%s", toJson().c_str());

	if ((o_time != m_time) && (m_watcher)) {
		m_watcher->updateTable(TABLEID, (Table*)this);
	}
}

bool stt::ingest(TableStore *s, const dvbpsi_atsc_stt_t * const t, TableWatcher *w)
{
	return s->setOnly<const dvbpsi_atsc_stt_t, stt>(TABLEID, t, w);
}


stt::stt(Decoder *parent, TableWatcher *watcher)
 : Table(parent, TABLE_NAME, TABLEID, watcher)
{
	//store table later (probably repeatedly)
}

stt::stt(Decoder *parent, TableWatcher *watcher, const dvbpsi_atsc_stt_t * const p_stt)
 : Table(parent, TABLE_NAME, TABLEID, watcher)
{
	store(p_stt);
}

stt::~stt()
{
	//
}

REGISTER_TABLE_FACTORY(TABLEID, const dvbpsi_atsc_stt_t, stt)
