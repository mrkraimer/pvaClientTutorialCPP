/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */

/**
 * @author Marty Kraimer
 * @date 2017.11
 */
#include <epicsGetopt.h>
#include <iostream>

#include <pv/pvaClient.h>

using namespace std;
using namespace epics::pvData;
using namespace epics::pvAccess;
using namespace epics::pvaClient;


static void alarm(PvaClientPtr const &pva,string const & channelName,string const & providerName )
{
    cout << "channelName " << channelName << " providerName " << providerName << endl;
    PVStructurePtr pvStructure(
       pva->channel(channelName,providerName,5.0)
         ->get("field(value,alarm,timeStamp)")
         ->getData()
         ->getPVStructure());
    Alarm alarm;
    PVAlarm pvAlarm;
    pvAlarm.attach(pvStructure->getSubField("alarm"));
    pvAlarm.get(alarm);
    string message = alarm.getMessage();
    string severity = (*AlarmSeverityFunc::getSeverityNames())[alarm.getSeverity()];
    string status = (*AlarmStatusFunc::getStatusNames())[alarm.getStatus()];
    cout << "alarm message " << '"' << message << '"' << " severity " << severity << " status " << status << endl;
}

int main(int argc,char *argv[])
{
    int opt;
    string channelName("PVRdouble");
    string providerName("pva");
    while ((opt = getopt(argc, argv, "hp:")) != -1) {
        switch (opt) {
        case 'h':
             cout << " -h -p providerName channelNames" << endl;
             cout << "default" << endl;
             cout << "-p " << providerName << " " <<  channelName
             << endl;
            return 0;
        case 'p' :
            providerName = optarg;
            break;
        default :
            break;
        }
    }
    vector<string> channelNames;
    int nPvs = argc - optind;       /* Remaining arg list are PV names */
    if (nPvs==0)
    {
        channelNames.push_back(channelName);
    } else {
        for (int n = 0; optind < argc; n++, optind++) channelNames.push_back(argv[optind]);
    }
    PvaClientPtr pva= PvaClient::get(providerName);
    for(size_t i=0; i<channelNames.size(); ++i) {
        try {
            alarm(pva,channelNames[i],providerName);
        } catch (std::exception& e) {
            cerr << "exception " << e.what() << endl;
        }
    }
    return 0;
}
