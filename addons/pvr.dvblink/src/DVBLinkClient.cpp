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
	channels = NULL;
	schedules = NULL;
	recordings = NULL;

	GetChannelsRequest* request = new GetChannelsRequest();
	channels = new ChannelList();

	if ((status = dvblinkRemoteCommunication->GetChannels(*request, *channels)) == DVBLINK_REMOTE_STATUS_OK) {
		connected = true;
	}else
	{
		XBMC->Log(LOG_ERROR, "Could not get channels from DVBLink Server '%s' on port '%i'", hostname, port);
	}
	delete(request);
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

int DVBLinkClient::GetTimersAmount()
{
	if (schedules != NULL)
	{
		PLATFORM::CLockObject critsec(m_mutex);
		return schedules->size();
	}
	return -1;
}

PVR_ERROR DVBLinkClient::GetTimers(ADDON_HANDLE handle)
{
	PLATFORM::CLockObject critsec(m_mutex);
	GetSchedulesRequest * getSchedulesRequest = new GetSchedulesRequest();
	
	if (schedules != NULL)
	{
	delete(schedules);
	}
	schedules = new ScheduleList();

	DVBLinkRemoteStatusCode status;
	if ((status = dvblinkRemoteCommunication->GetSchedules(*getSchedulesRequest, *schedules)) == DVBLINK_REMOTE_STATUS_OK) {
		
		for (std::vector<Schedule*>::iterator it = schedules->begin(); it < schedules->end(); it++) 
		{
			Schedule* schedule = (Schedule*)*it;
			PVR_TIMER xbmcTimer;
			memset(&xbmcTimer, 0, sizeof(PVR_TIMER));

			xbmcTimer.iClientIndex = atoi(schedule->GetID().c_str());
			xbmcTimer.iClientChannelUid = atoi(schedule->GetChannelID().c_str());
			xbmcTimer.state = PVR_TIMER_STATE_SCHEDULED;
			if (schedule->Type == EPG)
			{
				Program& p = schedule->GetProgram();
				xbmcTimer.startTime =p.GetStartTime();
				xbmcTimer.endTime = p.GetStartTime() + p.GetDuration();
				PVR_STRCPY(xbmcTimer.strTitle,p.GetTitle().c_str());
				PVR_STRCPY(xbmcTimer.strSummary,p.ShortDescription.c_str());
				xbmcTimer.iEpgUid = atoi(schedule->GetProgramID().c_str());
			}else{
				xbmcTimer.startTime = schedule->StartTime;
				xbmcTimer.endTime = schedule->StartTime + schedule->Duration;
				PVR_STRCPY(xbmcTimer.strTitle,schedule->GetTitle().c_str());
			}

			PVR->TransferTimerEntry(handle, &xbmcTimer);
		}

		delete(getSchedulesRequest);
	}else{
		delete(getSchedulesRequest);

		return PVR_ERROR_FAILED;
	}
	
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR DVBLinkClient::AddTimer(const PVR_TIMER &timer)
{
	PLATFORM::CLockObject critsec(m_mutex);
	DVBLinkRemoteStatusCode status;
	AddScheduleRequest * addScheduleRequest = NULL;
	char channelId [33];
	_itoa (timer.iClientChannelUid,channelId,10);
	if (timer.iEpgUid != 0)
	{
		char programId [33];
		_itoa (timer.iEpgUid,programId,10);
		addScheduleRequest = new AddScheduleByEpgRequest(channelId,programId,timer.bIsRepeating);
	}else{
		
		addScheduleRequest = new AddManualScheduleRequest(channelId, timer.startTime, timer.endTime - timer.startTime, DAILY,timer.strTitle);
	}
	if ( (status = dvblinkRemoteCommunication->AddSchedule(*addScheduleRequest)) == DVBLINK_REMOTE_STATUS_OK)
	{
		return PVR_ERROR_NO_ERROR;
	}
	
	return PVR_ERROR_FAILED;
}

PVR_ERROR DVBLinkClient::DeleteTimer(const PVR_TIMER &timer)
{
	PLATFORM::CLockObject critsec(m_mutex);
	DVBLinkRemoteStatusCode status;
	char scheduleId [33];
	_itoa (timer.iClientIndex,scheduleId,10);
	
	RemoveScheduleRequest * removeSchedule = new RemoveScheduleRequest(scheduleId);


	if ((status = dvblinkRemoteCommunication->RemoveSchedule(*removeSchedule)) == DVBLINK_REMOTE_STATUS_OK) {
		delete removeSchedule;
		return PVR_ERROR_NO_ERROR;
	}else{
		XBMC->Log(LOG_ERROR, "Could not delete Timer");
		return PVR_ERROR_FAILED;
	}

	
}

int DVBLinkClient::GetRecordingsAmount()
{
	if (recordings != NULL)
	{
		PLATFORM::CLockObject critsec(m_mutex);
		return recordings->size();
	}
	return -1;
}

std::string DVBLinkClient::GetBuildInRecorderObjectID()
{
	DVBLinkRemoteStatusCode status;
	GetObjectRequest * getObjectsRequest = new GetObjectRequest(hostname.c_str(), "", OBJECT_TYPE_UNKNOWN, ITEM_TYPE_UNKNOWN,0,-1,true);
	GetObjectResult * getObjectsResult = new GetObjectResult();
	if ((status = dvblinkRemoteCommunication->GetObject(*getObjectsRequest, *getObjectsResult)) == DVBLINK_REMOTE_STATUS_OK) {

		for(std::vector<Container*>::iterator it = getObjectsResult->Containers.begin(); it < getObjectsResult->Containers.end(); it++)
		{
			Container * container = (Container *) *it;
			if (strcmp(container->SourceId.c_str(), DVBLINK_BUILD_IN_RECORDER_SOURCE_ID) == 0)
			{
				//TODO: PAE: delete request / response objects
				return container->ObjectID;
			}

		}
	}
	delete getObjectsRequest;
	delete getObjectsResult;
	return "";
}


std::string DVBLinkClient::GetRecordedTVByDateObjectID(const std::string& buildInRecoderObjectID)
{
	DVBLinkRemoteStatusCode status;
	GetObjectRequest * getRecordedTVRequest = new GetObjectRequest(hostname.c_str(),buildInRecoderObjectID, OBJECT_TYPE_UNKNOWN, ITEM_TYPE_UNKNOWN,0,-1,true);
	GetObjectResult * getRecordedTVResult = new GetObjectResult();

	if ((status = dvblinkRemoteCommunication->GetObject(*getRecordedTVRequest, *getRecordedTVResult)) == DVBLINK_REMOTE_STATUS_OK) {
		for(std::vector<Container*>::iterator it = getRecordedTVResult->Containers.begin(); it < getRecordedTVResult->Containers.end(); it++)
		{
			Container * container = (Container *) *it;
			if (strcmp(container->Name.c_str(), "By Date") == 0)
			{
				//TODO: PAE: delete request / response objects
				return container->ObjectID;
			}
		}

	}
	delete getRecordedTVRequest;
	delete getRecordedTVResult;


}

PVR_ERROR DVBLinkClient::DeleteRecording(const PVR_RECORDING& recording)
{
	DVBLinkRemoteStatusCode status;
	RemoveObjectRequest * deleteObj = new RemoveObjectRequest(recording.strRecordingId);
	if ((status = dvblinkRemoteCommunication->RemoveObject(*deleteObj)) == DVBLINK_REMOTE_STATUS_OK) {
		delete deleteObj;
		return PVR_ERROR_NO_ERROR;
	}
	return PVR_ERROR_FAILED;
}


PVR_ERROR DVBLinkClient::GetRecordings(ADDON_HANDLE handle)
{

	DVBLinkRemoteStatusCode status;

	std::string recoderObjectId = GetBuildInRecorderObjectID();
	std::string recordingsByDateObjectId = GetRecordedTVByDateObjectID(recoderObjectId);

	GetObjectRequest * getRecordedTVRequest = new GetObjectRequest(hostname.c_str(),recordingsByDateObjectId, OBJECT_TYPE_UNKNOWN, ITEM_TYPE_UNKNOWN,0,-1,true);
	GetObjectResult * getRecordedTVResult = new GetObjectResult();

	if ((status = dvblinkRemoteCommunication->GetObject(*getRecordedTVRequest, *getRecordedTVResult)) == DVBLINK_REMOTE_STATUS_OK) {
		for(std::vector<Item*>::iterator it = getRecordedTVResult->Items.begin(); it < getRecordedTVResult->Items.end(); it++)
		{
			Item * item = (Item *) *it;

			if (item->Type == ITEM_TYPE_RECORDED_TV)
			{
				RecordedTV * tvItem = (RecordedTV *) item;
				PVR_RECORDING xbmcRecording;
				memset(&xbmcRecording, 0, sizeof(PVR_RECORDING));
				PVR_STRCPY(xbmcRecording.strRecordingId,tvItem->ObjectID.c_str());
				PVR_STRCPY(xbmcRecording.strTitle,tvItem->VInfo.Title.c_str());
				xbmcRecording.recordingTime = tvItem->VInfo.StartTime;
				PVR_STRCPY(xbmcRecording.strPlot, tvItem->VInfo.ShortDescription.c_str());
				PVR_STRCPY(xbmcRecording.strStreamURL, tvItem->Url.c_str());
				xbmcRecording.iDuration = tvItem->VInfo.Duration;
				PVR_STRCPY(xbmcRecording.strChannelName, tvItem->ChannelName.c_str());
				PVR->TransferRecordingEntry(handle, &xbmcRecording);
			}
		}
	
	}
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
	PLATFORM::CLockObject critsec(m_mutex);
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
				broadcast.strTitle = p->GetTitle().c_str();

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
		delete(epgSearchRequest);
		delete(epgSearchResult);
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
