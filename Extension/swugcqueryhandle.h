#pragma once

#include <IHandleSys.h>
#include <isteamugc.h>

#include "smsdk_ext.h"

class SteamWorksUGCQueryHandle :
    public IHandleTypeDispatch
{
public:
    SteamWorksUGCQueryHandle();
    virtual ~SteamWorksUGCQueryHandle();
    void OnHandleDestroy(HandleType_t type, void* object) override;
    bool GetHandleApproxSize(HandleType_t, void*, unsigned int*) override;

    HandleType_t GetUGCQueryHandle(void) const;

private:
    HandleType_t typeUGCQuery;
};

#include <extension.h>