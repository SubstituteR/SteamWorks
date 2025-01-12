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

static ISteamUGC* GetUGCPointer(void)
{
    return g_SteamWorks.pSWGameServer->GetUGC();
}

static cell_t sm_TestUGC(IPluginContext* pContext, const cell_t* params)
{
    ISteamUGC* pUGC = GetUGCPointer();
    printf("Running\n");
	if (pUGC == NULL)
	{
        printf("Invalid Handle\n");
		return BAD_HANDLE;
	}
    printf("Downloading\n");
    pUGC->BInitWorkshopForGameServer(550, R"(C:\Users\Admin\test_workshop)");
    pUGC->DownloadItem(3404846348, true);
    return 1;
}


static sp_nativeinfo_t ugcnatives[] = {
	{"SteamWorks_TestUGC",				sm_TestUGC},
	{NULL,											NULL}
};

SteamWorksUGCNatives::SteamWorksUGCNatives()
{
    sharesys->AddNatives(myself, ugcnatives);
}

SteamWorksUGCNatives::~SteamWorksUGCNatives()
{
}