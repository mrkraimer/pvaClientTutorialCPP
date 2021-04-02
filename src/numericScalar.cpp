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


static void numericScalar(PvaClientPtr const &pva,string const & channelName,string const & providerName )
{
    cout << "channelName " << channelName << " providerName " << providerName << endl;
    PvaClientPutPtr put(pva->channel(channelName,providerName)->put());
    PvaClientPutDataPtr putData(put->getData());
    putData->putDouble(1.0); put->put();

    // folowing uses defaults
    double value = pva->channel(channelName)->get()->getData()->getDouble();
    cout << "get double with defaults " << value << endl;
    // following is like the previous but without defaults
    value = pva->channel(channelName,providerName,5.0)->get("field(value,alarm,timeStamp)")->getData()->getDouble();
    cout << "get double without defaults " << value << endl;

    // following uses no shortcuts except for getDouble
    putData->putDouble(2.0); put->put();
    PvaClientChannelPtr pvaChannel(pva->createChannel(channelName,providerName));
    pvaChannel->issueConnect();
    Status status = pvaChannel->waitConnect(2.0);
    if(!status.isOK()) {cout << " connect failed\n"; return;}
    PvaClientGetPtr pvaGet(pvaChannel->createGet());
    pvaGet->issueConnect();
    status = pvaGet->waitConnect();
    if(!status.isOK()) {cout << " createGet failed\n"; return;}
    pvaGet->issueGet();
    status = pvaGet->waitGet();
    if(!status.isOK()) {cout << " get failed\n"; return;}
    PvaClientGetDataPtr pvaData(pvaGet->getData());
    value = pvaData->getDouble();
    cout << "get double long way " << value << endl;
}

int main(int argc,char *argv[])
{
    int opt;
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
            numericScalar(pva,channelNames[i],providerName);
        } catch (std::exception& e) {
            cerr << "exception " << e.what() << endl;
        }
    }
    return 0;
}
