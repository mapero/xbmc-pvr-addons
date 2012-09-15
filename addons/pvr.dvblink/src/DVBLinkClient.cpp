#include "DVBLinkClient.h"



DVBLinkClient::DVBLinkClient(CHelper_libXBMC_pvr *PVR, std::string hostname, long port, std::string username, std::string password)
{
	this->PVR = PVR;
	connected = false;
	httpClient = new CurlHttpClient();
	dvblinkRemoteCommunication = DVBLinkRemote::Connect((HttpClient&)*httpClient, hostname.c_str(), port, username.c_str(), password.c_str());

	DVBLinkRemoteStatusCode status;
	GetChannelsRequest* request = new GetChannelsRequest();
	channels = new ChannelList();

	if ((status = dvblinkRemoteCommunication->GetChannels(*request, *channels)) == DVBLINK_REMOTE_STATUS_OK) {
		connected = true;
		/*
		// std::cout << "Channels:" << std::endl;
		//std::cout << "----------------------" << std::endl;

		for (std::vector<Channel*>::iterator it = response->begin(); it < response->end(); it++) 
		{
			printf("[%d]\t%s\n", (*it)->Number, (*it)->GetName().c_str());
		}
		*/
	}


}

bool DVBLinkClient::GetStatus()
{
	return connected;
}



int DVBLinkClient::GetChannelsAmount()
{
	return channels->size();
}

PVR_ERROR DVBLinkClient::GetChannels(ADDON_HANDLE handle, bool bRadio)
{
	for (std::vector<Channel*>::iterator it = channels->begin(); it < channels->end(); it++) 
	{
		Channel* channel = (*it);

//		if (channel->Radio == bRadio)
//		{
			PVR_CHANNEL xbmcChannel;
			memset(&xbmcChannel, 0, sizeof(PVR_CHANNEL));
			xbmcChannel.bIsRadio = false;
			xbmcChannel.iChannelNumber =channel->Number;
			xbmcChannel.iEncryptionSystem = 0;
			xbmcChannel.iUniqueId = channel->GetDvbLinkID();
			
		//	char * tmpChannelName = new char[channel->GetName().length()];
			strcpy(xbmcChannel.strChannelName, channel->GetName().c_str());

			//char * tmpStreamUrl = new char[ChannelName.length()];
			//CStdString      stream;
			//stream.Format("pvr://stream/tv/%i.ts", DVBLinkChannelId);
			//strcpy(tmpStreamUrl, stream.c_str());
			strcpy(xbmcChannel.strStreamURL, "pvr://stream/tv/");
			//pvrchannel.strInputFormat = "";
			//pvrchannel.strIconPath = "";
			PVR->TransferChannelEntry(handle, &xbmcChannel);

	}

	return PVR_ERROR_NO_ERROR;

}

DVBLinkClient::~DVBLinkClient(void)
{
	delete dvblinkRemoteCommunication;
	delete httpClient;
	delete channels;
}
