/**
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution.
 */

#include <dbAccess.h>
#include <epicsStdio.h>
#include <asDbLib.h>

#define epicsExportSharedSymbols
#include <caSecurity.h>

using namespace epics::pvData;
using namespace epics::pvAccess;
using namespace epics::pvaSrv;


SecuritySession::shared_pointer CAServerSecurityPlugin::createSession(
        osiSockAddr const & /*remoteAddress*/,
        SecurityPluginControl::shared_pointer const & control,
        PVField::shared_pointer const & data) throw (SecurityException)
{
    PVStructure::shared_pointer s = std::tr1::dynamic_pointer_cast<PVStructure>(data);
    if (s)
    {
        PVString::shared_pointer pvUser = s->getSubField<PVString>("user");
        PVString::shared_pointer pvHost = s->getSubField<PVString>("host");
        if (!pvUser)
            throw SecurityException("client must provide a structure with 'user' string field.");
        if (!pvHost)
            throw SecurityException("client must provide a structure with 'host' string field.");

        SecuritySession::shared_pointer ss(
                    new CAServerSecuritySession(shared_from_this(), pvUser->get(), pvHost->get())
                    );

// TODO possible race condition !!!
        control->authenticationCompleted(Status::Ok);
        return ss;
    }
    else
        throw SecurityException("client must provide a structure with 'user' and 'host' string fields.");

}



Status CAServerChannelSecuritySession::m_noAccessStatus(Status::STATUSTYPE_ERROR, "no access");

CAServerChannelSecuritySession::CAServerChannelSecuritySession(std::string const & channelName,
                               const char * user,
                               char * host)
    throw (SecurityException)
{
    struct dbAddr dbAddr;
    long result = dbNameToAddr(channelName.c_str(), &dbAddr);
    if (result != 0)
        throw NoChannelException();

    long status = asAddClient(
            &m_asClientPvt,
            (ASMEMBERPVT)asDbGetMemberPvt(&dbAddr),
            asDbGetAsl(&dbAddr),
            user,
            host);
    if (status != 0 && status != S_asLib_asNotActive)
        throw SecurityException("no room for security table");
}

CAServerChannelSecuritySession::~CAServerChannelSecuritySession() {
    close();
}

void CAServerChannelSecuritySession::close() {
    // multiple calls are OK
    asRemoveClient(&m_asClientPvt);
}
