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

static HandleType_t GetSteamUGCQueryHandle(void)
{
    return g_SteamWorks.pSWUGCQueryHandle->GetUGCQueryHandle();
}

static UGCQueryHandle_t* GetQueryHandlePointer(IPluginContext *pContext, cell_t Handle)
{
    HandleError err;
    HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

    UGCQueryHandle_t* pRequest;

    if ((err = handlesys->ReadHandle(Handle, GetSteamUGCQueryHandle(), &sec, (void**)&pRequest))
        != HandleError_None)
    {
        printf("BAD BAD BAD BAD BAD");
        pContext->ThrowNativeError("Invalid Handle %x (error: %d)", Handle, err);
        return NULL;
    }

    return pRequest;
}

static std::vector<PublishedFileId_t> itemIdStringsToIds(IPluginContext* pContext, const cell_t* params)
{
    std::int32_t size = params[2];
    cell_t* itemIdStrings;
    std::vector<PublishedFileId_t> itemIds(size);
    pContext->LocalToPhysAddr(params[1], &itemIdStrings);

	for(auto i = 0; i < size; ++i)
	{
        char* itemIdString;
        pContext->LocalToString(itemIdStrings[i], &itemIdString);
        itemIds.push_back(std::stoll(itemIdString));
    }
    return itemIds;
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

static cell_t sm_CreateQueryDetailsRequest(IPluginContext* pContext, const cell_t* params)
{
    if (!has_init)
        return has_init;

    const auto ugc = GetUGCPointer();
    if (!ugc)
        return false;

    auto files = itemIdStringsToIds(pContext, params);

    auto queryHandle = ugc->CreateQueryUGCDetailsRequest(files.data(), files.size());

    if (queryHandle == k_UGCQueryHandleInvalid)
        return BAD_HANDLE;

    auto pointer = new UGCQueryHandle_t(queryHandle);

    Handle_t handle = handlesys->CreateHandle(GetSteamUGCQueryHandle(), pointer, pContext->GetIdentity(), myself->GetIdentity(), NULL);

	if (handle != BAD_HANDLE)
        return static_cast<cell_t>(handle);


    ugc->ReleaseQueryUGCRequest(*pointer);
    delete pointer;
    return BAD_HANDLE;
}

static cell_t sm_TestHandle(IPluginContext* pContext, const cell_t* params)
{
    UGCQueryHandle_t* pRequest = GetQueryHandlePointer(pContext, params[1]);
    printf("debugging two %p: %llu\n", pRequest, *pRequest);
    return 0;

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
    {"SteamWorks_UGC_CreateQueryDetailsRequest", sm_CreateQueryDetailsRequest},
    {"SteamWorks_UGC_TEST2", sm_TestHandle},
	{NULL,											NULL}
};

SteamWorksUGCNatives::SteamWorksUGCNatives()
{
    sharesys->AddNatives(myself, ugcnatives);
}

SteamWorksUGCNatives::~SteamWorksUGCNatives()
{
}