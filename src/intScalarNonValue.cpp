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


static void intScalar(
    PvaClientPtr const &pva,
    string const & channelName,
    string const & providerName,
    string const & request)
{
    cout << "channelName " << channelName 
         << " providerName " << providerName 
         << " request " << request 
         << endl;
    PvaClientPutPtr put(pva->channel(channelName,providerName)->put(request));
    PvaClientPutDataPtr putData(put->getData());
    PVStructurePtr  pvStruct(putData->getPVStructure());
    // search down the structure looking only at the first field of each sub-structure.
    // stop when the first field is not a structure
    while(true) 
    {
        PVStructurePtr subStruct(pvStruct->getSubField<PVStructure>(pvStruct->getFieldOffset()+1));
        if(!subStruct) break;
        pvStruct = subStruct;
    }
    // 
    PVIntPtr pvIntPut(pvStruct->getSubField<PVInt>(pvStruct->getFieldOffset()+1));
    if(!pvIntPut) {
        cout << "did not find an int32 field\n";
        return;
    }
    pvIntPut->put(1); put->put();
    PvaClientGetPtr get(pva->channel(channelName,providerName)->get(request));
    get->get();
    PvaClientGetDataPtr getData(get->getData());
    PVStructurePtr pvStructure(getData->getPVStructure());
    while(true) 
    {
        PVStructurePtr subStruct(pvStructure->getSubField<PVStructure>(pvStructure->getFieldOffset()+1));
        if(!subStruct) break;
        pvStructure = subStruct;
    }
    PVIntPtr pvIntGet(pvStructure->getSubField<PVInt>(pvStructure->getFieldOffset()+1));
    cout << "get int " << pvIntGet->get() << endl;
    pvIntPut->put(2); put->put();
    get->get();
    getData = get->getData();
    pvStructure = getData->getPVStructure();
    while(true) 
    {
        PVStructurePtr subStruct(pvStructure->getSubField<PVStructure>(pvStructure->getFieldOffset()+1));
        if(!subStruct) break;
        pvStructure = subStruct;
    }
    pvIntGet = pvStructure->getSubField<PVInt>(pvStructure->getFieldOffset()+1);
    cout << "get int again " << pvIntGet->get() << endl;
}

int main(int argc,char *argv[])
{
    int opt;
    string channelName("PVRint");
    string providerName("pva");
    string request("value");
    while ((opt = getopt(argc, argv, "hp:r:")) != -1) {
        switch (opt) {
        case 'h': 
             cout << " -h -p providerName -r request channelNames " << endl;
             cout << "default" << endl;
             cout << "-p " << providerName 
                  << " -r " << request
                  << " " <<  channelName
             << endl;
            return 0;
        case 'p' :
            providerName = optarg;
            break;
        case 'r' :
            request = optarg;
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
            intScalar(pva,channelNames[i],providerName,request);
        } catch (std::exception& e) {
            cerr << "exception " << e.what() << endl;
        }
    }
    return 0;
}
