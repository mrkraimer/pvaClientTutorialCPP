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


static void intArray(
    PvaClientPtr const &pva,
    string const & channelName,
    string const & providerName)
{
    cout << "channelName " << channelName 
         << " providerName " << providerName 
         << endl;
    PvaClientPutPtr put(pva->channel(channelName,providerName)->put());
    PvaClientPutDataPtr putData(put->getData());
    PVIntArrayPtr pvData(putData->getPVStructure()->getSubField<PVIntArray>("value"));
    if(!pvData) {
         cout << "value is not an PVIntArray\n";
         return;
    }
    size_t num = 5;
    shared_vector<int32> data(num);
    for(size_t i=0; i<num; ++i) data[i] = i;
    pvData->replace(freeze(data)); put->put();

    // folowing uses defaults
    shared_vector<const int> value;
    value = pva->channel(channelName)->get()->getData()
       ->getPVStructure()->getSubField<PVIntArray>("value")->view();
    cout << "get intArray with defaults " << value << endl;
    // following is like the previous but without defaults
    value = pva->channel(channelName,providerName,5.0)
         ->get("field(value,alarm,timeStamp)")->getData()
         ->getPVStructure()->getSubField<PVIntArray>("value")->view();
    cout << "get intArray without defaults " << value << endl;

    data = shared_vector<int>(num);
    for(size_t i=0; i<num; ++i) {
       int32 val = i*2;
       data[i] = val;
    }
    pvData->replace(freeze(data)); put->put();
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
    value = pvaData->getPVStructure()->getSubField<PVIntArray>("value")->view();
    cout << "get intArray long way" << value << endl;
}

int main(int argc,char *argv[])
{
    int opt;
    string channelName("PVRintArray");
    string providerName("pva");
    while ((opt = getopt(argc, argv, "hp:")) != -1) {
        switch (opt) {
        case 'h': 
             cout << " -h -p providerName channelNames " << endl;
             cout << "default" << endl;
             cout << "-p " << providerName 
                  << " " <<  channelName
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
            intArray(pva,channelNames[i],providerName);
        } catch (std::exception& e) {
            cerr << "exception " << e.what() << endl;
        }
    }
    return 0;
}
