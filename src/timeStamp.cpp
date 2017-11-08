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

static void timeStamp(PvaClientPtr const &pva,string const & channelName,string const & providerName )
{
    cout << "channelName " << channelName << " providerName " << providerName << endl;
    PVStructurePtr pvStructure(
       pva->channel(channelName,providerName,5.0)
         ->get("field(value,alarm,timeStamp)")
         ->getData()
         ->getPVStructure());
    TimeStamp timeStamp;
    PVTimeStamp pvTimeStamp; 
    pvTimeStamp.attach(pvStructure->getSubField("timeStamp"));
    pvTimeStamp.get(timeStamp);

    char timeText[32];
    epicsTimeStamp epicsTS;
    epicsTS.secPastEpoch = timeStamp.getEpicsSecondsPastEpoch();
    epicsTS.nsec = timeStamp.getNanoseconds();
    epicsTimeToStrftime(timeText, sizeof(timeText), "%Y.%m.%d %H:%M:%S.%f", &epicsTS);
    cout << "time " << timeText << " userTag " << timeStamp.getUserTag() << endl;    
}

int main(int argc,char *argv[])
{
    int opt;                    /* getopt() current option */
    string channelName("PVRdouble");
    string providerName("pva");
    while ((opt = getopt(argc, argv, "hp:")) != -1) {
        switch (opt) {
        case 'h':
             cout << " -h -p providerName channelNames " << endl;
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
            timeStamp(pva,channelNames[i],providerName);
        } catch (std::runtime_error e) {
            cerr << "exception " << e.what() << endl;
        }
    }
    return 0;
}
