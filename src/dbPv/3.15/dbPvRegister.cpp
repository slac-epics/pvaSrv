/**
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution.
 */
/**
 * @author mrk
 */

/* Author: Marty Kraimer */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>
#include <memory>

#include <cantProceed.h>
#include <epicsStdio.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <epicsThread.h>
#include <errlog.h>
#include <iocsh.h>

#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/pvAccess.h>

#define epicsExportSharedSymbols

#include "dbPv.h"
#include <epicsExport.h>

using namespace epics::pvData;
using namespace epics::pvAccess;
using namespace epics::pvaSrv;

static void dbPvRegister(void)
{
    static int firstTime = 1;
    if (firstTime) {
        firstTime = 0;
        getDbPvProvider();
    }
}

extern "C" {
    epicsExportRegistrar(dbPvRegister);
}
