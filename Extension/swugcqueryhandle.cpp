/*
    This file is part of SourcePawn SteamWorks.

    SourcePawn SteamWorks is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, as per version 3 of the License.

    SourcePawn SteamWorks is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SourcePawn SteamWorks.  If not, see <http://www.gnu.org/licenses/>.

    Author: Riley Strickland (SubstituteR).
*/

#include "swugcqueryhandle.h"

static ISteamUGC* GetUGCPointer(void)
{
    return g_SteamWorks.pSWGameServer->GetUGC();
}

SteamWorksUGCQueryHandle::SteamWorksUGCQueryHandle()
{
    this->typeUGCQuery = handlesys->CreateType("UGCQueryHandle", this, 0, NULL, NULL, myself->GetIdentity(), NULL);
}

SteamWorksUGCQueryHandle::~SteamWorksUGCQueryHandle()
{
    handlesys->RemoveType(this->typeUGCQuery, myself->GetIdentity());
}

void SteamWorksUGCQueryHandle::OnHandleDestroy(HandleType_t type, void* object)
{
    if (type != this->typeUGCQuery || !object)
        return;
    const auto ugc = GetUGCPointer();
    const auto handle = static_cast<UGCQueryHandle_t*>(object);

	if (ugc) // Release this request from Steam if we have the interface.
		GetUGCPointer()->ReleaseQueryUGCRequest(*handle);
	delete handle;
}

bool SteamWorksUGCQueryHandle::GetHandleApproxSize(HandleType_t type, void* object, unsigned int* pSize)
{
    if (type == this->typeUGCQuery && pSize)
    {
        *pSize = sizeof(UGCQueryHandle_t);
        return true;
    }
    return false;
}

HandleType_t SteamWorksUGCQueryHandle::GetUGCQueryHandle(void) const
{
    return this->typeUGCQuery;
}