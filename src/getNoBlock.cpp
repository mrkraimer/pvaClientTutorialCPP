/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */

/**
 * @author mrk
 */

/* Author: Marty Kraimer */
#include <epicsGetopt.h>
#include <iostream>

#include <pv/pvaClient.h>

using namespace std;
using namespace epics::pvData;
using namespace epics::pvAccess;
using namespace epics::pvaClient;

class ClientGet;
typedef std::tr1::shared_ptr<ClientGet> ClientGetPtr;

class ClientGet :
    public PvaClientChannelStateChangeRequester,
    public PvaClientGetRequester,
    public std::tr1::enable_shared_from_this<ClientGet>
{
private:
    string channelName;
    string providerName;
    string request;
    bool channelConnected;
    bool getConnected;

    PvaClientChannelPtr pvaClientChannel;
    PvaClientGetPtr pvaClientGet;

    void init(PvaClientPtr const &pvaClient)
    {
        pvaClientChannel = pvaClient->createChannel(channelName,providerName);
        pvaClientChannel->setStateChangeRequester(shared_from_this());
        pvaClientChannel->issueConnect();
    }

public:
    POINTER_DEFINITIONS(ClientGet);
    ClientGet(
        const string &channelName,
        const string &providerName,
        const string &request)
    : channelName(channelName),
      providerName(providerName),
      request(request),
      channelConnected(false),
      getConnected(false)
    {
        cout<< "channelName " << channelName << " providerName " << providerName << endl;
    }
    
    static ClientGetPtr create(
        PvaClientPtr const &pvaClient,
        const string & channelName,
        const string & providerName,
        const string  & request)
    {
       ClientGetPtr client(ClientGetPtr(
             new ClientGet(channelName,providerName,request)));
        client->init(pvaClient);
        return client;
    }

    virtual void channelStateChange(PvaClientChannelPtr const & channel, bool isConnected)
    {
        channelConnected = isConnected;
        if(isConnected) {
            if(!pvaClientGet) {
                pvaClientGet = pvaClientChannel->createGet(request);
                pvaClientGet->setRequester(shared_from_this());
                pvaClientGet->issueConnect();
            }
        }
    }

    virtual void channelGetConnect(
        const epics::pvData::Status& status,
        PvaClientGetPtr const & clientGet)
    {
         getConnected = true;
         cout << "channelGetConnect " << channelName << " status " << status << endl;
    }

    virtual void getDone(
        const epics::pvData::Status& status,
        PvaClientGetPtr const & clientGet)
    {
        cout << "channelGetDone " << channelName << " status " << status << endl;
    }

    void get()
    {  
        if(!channelConnected) {
            cout << channelName << " channel not connected\n";
            return;
        }
        if(!getConnected) {
            cout << channelName << " channelGet not connected\n";
            return;
        }
        pvaClientGet->get();
        PvaClientGetDataPtr data = pvaClientGet->getData();
        BitSetPtr bitSet =  data->getChangedBitSet();
        if(bitSet->cardinality()>0) {
             cout << "changed " << channelName << endl;
             data->showChanged(cout);
             cout << "bitSet " << *bitSet << endl;
        }
    }
   
};


int main(int argc,char *argv[])
{
    int opt;
    string channelName("PVRdouble");
    string provider("pva");
    string request("value,alarm,timeStamp");
    while ((opt = getopt(argc, argv, "hp:r:")) != -1) {
        switch (opt) {
        case 'h': 
             cout << " -h -p provider -r request channelNames " << endl;
             cout << "default" << endl;
             cout << "-p " << provider 
                  << " -r " << request
                  << " " <<  channelName
             << endl;
            return 0;
        case 'p' :
            provider = optarg;
            break;
        case 'r' :
            request = optarg;
            break;
        default :
            break;
        }
    }
    try {   
        vector<string> channelNames;
        vector<ClientGetPtr> clientGets;
        int nPvs = argc - optind;       /* Remaining arg list are PV names */
        if (nPvs==0)
        {
            channelNames.push_back(channelName);
            nPvs = 1;
        } else {
            for (int n = 0; optind < argc; n++, optind++) channelNames.push_back(argv[optind]);
        }
        PvaClientPtr pva= PvaClient::get(provider);
        for(int i=0; i<nPvs; ++i) {
            clientGets.push_back(ClientGet::create(pva,channelNames[i],provider,request));
        }
        while(true) {
            string str;
            getline(cin,str);
            if(str.compare("exit")==0){
                 break;
            }
            for(int i=0; i<nPvs; ++i) {
                try {
                    clientGets[i]->get();
                } catch (std::runtime_error e) {
                   cerr << "exception " << e.what() << endl;
                }
            }
        }
    } catch (std::runtime_error e) {
            cerr << "exception " << e.what() << endl;
            return 1;
    }
    return 0;
}
