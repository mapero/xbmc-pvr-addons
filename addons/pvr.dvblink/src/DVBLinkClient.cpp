#include "DVBLinkClient.h"
#include "..\util\StdString.h"


DVBLinkClient::DVBLinkClient(CHelper_libXBMC_addon  *XBMC, CHelper_libXBMC_pvr *PVR,DVBLINK_STREAMTYPE streamtype,std::string clientname, std::string hostname, long port, std::string username, std::string password)
{
	this->PVR = PVR;
	this->XBMC = XBMC;
	this->streamtype = streamtype;
	this->clientname = clientname;
	this->hostname = hostname;
	connected = false;
	httpClient = new CurlHttpClient();
	dvblinkRemoteCommunication = DVBLinkRemote::Connect((HttpClient&)*httpClient, hostname.c_str(), port, username.c_str(), password.c_str());

	DVBLinkRemoteStatusCode status;
	GetChannelsRequest* request = new GetChannelsRequest();
	channels = new ChannelList();

	if ((status = dvblinkRemoteCommunication->GetChannels(*request, *channels)) == DVBLINK_REMOTE_STATUS_OK) {
		connected = true;
	}else
	{
		XBMC->Log(LOG_ERROR, "Could not get channels from DVBLink Server '%s' on port '%i'", hostname, port);
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

		bool isRadio = (channel->Type == RD_CHANNEL_RADIO);

		if (isRadio == bRadio)
		{
			PVR_CHANNEL xbmcChannel;
			memset(&xbmcChannel, 0, sizeof(PVR_CHANNEL));
			xbmcChannel.bIsRadio = false;
			xbmcChannel.iChannelNumber =channel->Number;
			xbmcChannel.iEncryptionSystem = 0;
			xbmcChannel.iUniqueId = channel->GetDvbLinkID();
			PVR_STRCPY(xbmcChannel.strChannelName,channel->GetName().c_str());
			CStdString stream;
			if(channel->Type == RD_CHANNEL_RADIO)
				stream.Format("pvr://stream/radio/%i.ts", channel->GetDvbLinkID());
			else
				stream.Format("pvr://stream/tv/%i.ts", channel->GetDvbLinkID());

			PVR_STRCPY(xbmcChannel.strStreamURL, stream.c_str());
			//pvrchannel.strInputFormat = "";
			//pvrchannel.strIconPath = "";
			PVR->TransferChannelEntry(handle, &xbmcChannel);
		}
	}
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR DVBLinkClient::GetTimers(ADDON_HANDLE handle)
{
	return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR DVBLinkClient::GetRecordings(ADDON_HANDLE handle)
{
	/*
	DVBLinkRemoteStatusCode status;
	GetRecordingsRequest* getRecordingsRequest = new GetRecordingsRequest();
	RecordingList* recordingList = new RecordingList();

	if ((status = dvblinkRemoteCommunication->GetRecordings(*getRecordingsRequest, *recordingList)) == DVBLINK_REMOTE_STATUS_OK) {


		for (std::vector<Recording*>::iterator it = recordingList->begin(); it < recordingList->end(); it++) 
		{
			Recording* recording = (Recording*)*it;
			Program& p = recording->GetProgram();
			

		}
	}
	*/
	return PVR_ERROR_NO_ERROR;
}

const char * DVBLinkClient::GetLiveStreamURL(const PVR_CHANNEL &channel)
{
	switch(streamtype)
	{
	case HTTP:
		RawHttpStreamRequest * streamRequest = new RawHttpStreamRequest(hostname.c_str(), channel.iUniqueId, clientname.c_str());
		stream = new Stream();
		DVBLinkRemoteStatusCode status;
		if ((status = dvblinkRemoteCommunication->PlayChannel(*streamRequest, *stream)) == DVBLINK_REMOTE_STATUS_OK) {
			delete streamRequest;
			return stream->GetUrl().c_str();
		}
		break;
	}

	XBMC->Log(LOG_ERROR, "Could not get stream URL for channel '%s' (%i)", channel.strChannelName, channel.iUniqueId);

	return "";
}

void DVBLinkClient::StopStreaming()
{
	StopStreamRequest * request = new StopStreamRequest(clientname);
	DVBLinkRemoteStatusCode status;
	if ((status = dvblinkRemoteCommunication->StopChannel(*request)) == DVBLINK_REMOTE_STATUS_OK) {
		delete stream;
		delete request;
	}else{
		XBMC->Log(LOG_ERROR, "Could not stop stream");
	}
}

void DVBLinkClient::SetEPGGenre(Program *program, EPG_TAG *tag)
{

	if (program->IsCatNews)
	{
		tag->iGenreType = 0x20;
		tag->iGenreSubType = 0x02;
	}
	if (program->IsCatDocumentary)
	{
		tag->iGenreType = 0x20;
		tag->iGenreSubType = 0x03;
	}

	if (program->IsCatEducational)
	{
		tag->iGenreType = 0x90;
	}

	if (program->IsCatSports)
	{
		tag->iGenreType = 0x40;
	}

	if (program->IsCatMovie)
	{
		tag->iGenreType = 0x10;
		tag->iGenreSubType =program->IsCatComedy ? 0x04 : 0;
		tag->iGenreSubType = program->IsCatRomance ? 0x06 : 0;
		tag->iGenreSubType = program->IsCatDrama ? 0x08 : 0;
		tag->iGenreSubType = program->IsCatThriller ? 0x01 : 0;
		tag->iGenreSubType = program->IsCatScifi ? 0x03 : 0;
		tag->iGenreSubType = program->IsCatSoap ? 0x05 : 0;
		tag->iGenreSubType = program->IsCatHorror ? 0x03 : 0;
	}

	if (program->IsCatKids)
	{
		tag->iGenreType = 0x50;
	}

	if (program->IsCatMusic)
	{
		tag->iGenreType = 0x60;
	}

	if (program->IsCatSpecial)
	{
		tag->iGenreType = 0xB0;
	}

}

PVR_ERROR DVBLinkClient::GetEPGForChannel(ADDON_HANDLE handle, const PVR_CHANNEL& channel, time_t iStart, time_t iEnd)
{
	char channelId [33];
	_itoa (channel.iUniqueId,channelId,10);
	EpgSearchRequest* epgSearchRequest = new EpgSearchRequest(channelId, iStart, iEnd);
	EpgSearchResult* epgSearchResult = new EpgSearchResult();
	DVBLinkRemoteStatusCode status;

	if ((status = dvblinkRemoteCommunication->SearchEpg(*epgSearchRequest, *epgSearchResult)) == DVBLINK_REMOTE_STATUS_OK) {
		for (std::vector<ChannelEpgData*>::iterator it = epgSearchResult->begin(); it < epgSearchResult->end(); it++) 
		{
			ChannelEpgData* channelEpgData = (ChannelEpgData*)*it;
			EpgData& epgData = channelEpgData->GetEpgData();
			for (std::vector<Program*>::iterator pIt = epgData.begin(); pIt < epgData.end(); pIt++) 
			{

				Program* p = (Program*)*pIt;
				EPG_TAG broadcast;
				memset(&broadcast, 0, sizeof(EPG_TAG));

				
				broadcast.iUniqueBroadcastId  = atoi(p->GetID().c_str());
				broadcast.strTitle            = p->GetTitle().c_str();
				broadcast.iChannelNumber      = channel.iChannelNumber;
				broadcast.startTime           = p->GetStartTime();
				broadcast.endTime             = p->GetStartTime() + p->GetDuration();
				broadcast.strPlotOutline      = p->SubTitle.c_str();
				broadcast.strPlot             = p->ShortDescription.c_str();
				broadcast.strIconPath         = "";
				broadcast.iGenreType          = 0;
				broadcast.iGenreSubType       = 0;
				broadcast.strGenreDescription = "";
				broadcast.firstAired          = 0;
				broadcast.iParentalRating     = 0;
				broadcast.iStarRating         = p->Rating;
				broadcast.bNotify             = false;
				broadcast.iSeriesNumber       = 0;
				broadcast.iEpisodeNumber      = p->EpisodeNumber;
				broadcast.iEpisodePartNumber  = 0;
				broadcast.strEpisodeName      = "";

				SetEPGGenre(p, &broadcast);

				PVR->TransferEpgEntry(handle, &broadcast);
			}
		}
	}else
	{
		XBMC->Log(LOG_ERROR, "Not EPG data found for channel : %s with id : %i", channel.strChannelName, channel.iUniqueId);

		return PVR_ERROR_FAILED;
	}

	return PVR_ERROR_NO_ERROR;
}

DVBLinkClient::~DVBLinkClient(void)
{
	delete dvblinkRemoteCommunication;
	delete httpClient;
	delete channels;
}
