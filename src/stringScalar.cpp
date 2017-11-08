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

static void stringScalar(
    PvaClientPtr const &pva,
    string const & channelName,
    string const & providerName,
    string const & putValue)
{
    cout << "channelName " << channelName << " providerName " << providerName << endl;
    PvaClientPutPtr put(pva->channel(channelName,providerName)->put());
    PvaClientPutDataPtr putData(put->getData());
    putData->putString(putValue); put->put();

    string value = pva->channel(channelName)->get()->getData()->getString();
    cout << "get string with defaults " << value << endl;
    value = pva->channel(channelName,providerName,5.0)
       ->get("field(value,alarm,timeStamp)")->getData()->getString();
    cout << "get string without defaults " << value << endl;

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
    value = pvaData->getString();
    cout << "get string long way " << value << endl;
}


int main(int argc,char *argv[])
{
    int opt;
    string channelName("PVRdouble");
    string providerName("pva");
    string value("10.1");
    while ((opt = getopt(argc, argv, "hp:v:")) != -1) {
        switch (opt) {
        case 'h': 
             cout << " -h -p providerName -v value channelNames " << endl;
             cout << "default" << endl;
             cout << "-p " << providerName
                  << " -v " << value
                  << " " <<  channelName
             << endl;
            return 0;
        case 'p' :
            providerName = optarg;
            break;
        case 'v' :
            value = optarg;
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
           stringScalar(pva,channelNames[i],providerName,value);
        } catch (std::runtime_error e) {
            cerr << "exception " << e.what() << endl;
        }
    }
    return 0;
}
