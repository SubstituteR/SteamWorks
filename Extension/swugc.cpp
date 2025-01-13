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

#include "swugc.h"
#include <cstdio>
#include <filesystem>
#include <iostream>

bool has_init = false;

static ISteamUGC* GetUGCPointer(void)
{
    return g_SteamWorks.pSWGameServer->GetUGC();
}

static cell_t sm_TestUGC(IPluginContext* pContext, const cell_t* params)
{
    if (!has_init)
        return has_init;

    ISteamUGC* pUGC = GetUGCPointer();
    printf("Running\n");
	if (pUGC == NULL)
	{
        printf("Invalid Handle\n");
		return BAD_HANDLE;
	}
    printf("Downloading\n");
    pUGC->DownloadItem(3404846348, false);
    return 1;
}

static cell_t sm_InitWorkshopForGameServer(IPluginContext* pContext, const cell_t* params)
{
    if (has_init)
        return has_init;

    const auto ugc = GetUGCPointer();
    if (!ugc)
        return false;

	char* location;
    const DepotId_t depot = params[1];
    pContext->LocalToString(params[2], &location);

    char realpath[PLATFORM_MAX_PATH];
    g_pSM->BuildPath(Path_Game, realpath, sizeof(realpath), "%s", location);

    printf("Directory is %s\n", realpath);

    printf("As Path(): %s\n", std::filesystem::path(realpath).string().c_str());

    has_init = ugc->BInitWorkshopForGameServer(depot, realpath);
    return has_init;
}

static cell_t sm_DownloadItem(IPluginContext* pContext, const cell_t* params)
{
    if (!has_init)
        return has_init;

    const auto ugc = GetUGCPointer();
    if (!ugc)
        return false;

    char* item;
    pContext->LocalToString(params[1], &item);
	const auto item_id = std::stoull(item);
    printf("User has request to download the following item %llu\n", item_id);
	return ugc->DownloadItem(item_id, params[2]);
}

static cell_t sm_CreateHardlink(IPluginContext* pContext, const cell_t* params)
{
    char* source;
    char* dest;
    pContext->LocalToString(params[1], &source);
    pContext->LocalToString(params[2], &dest);
    std::filesystem::create_hard_link(source, dest);
    return 0;
}


static sp_nativeinfo_t ugcnatives[] = {
    {"SteamWorks_TestUGC", sm_TestUGC},
    {"CreateHardlink", sm_CreateHardlink},
    {"SteamWorks_InitWorkshopForGameServer", sm_InitWorkshopForGameServer},
    {"SteamWorks_UGC_DownloadItem", sm_DownloadItem},
	{NULL,											NULL}
};

SteamWorksUGCNatives::SteamWorksUGCNatives()
{
    sharesys->AddNatives(myself, ugcnatives);
}

SteamWorksUGCNatives::~SteamWorksUGCNatives()
{
}