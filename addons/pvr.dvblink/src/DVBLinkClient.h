#pragma once

#include "platform/os.h"
#include "libdvblinkremote\src\dvblinkremote.h"
#include "libdvblinkremote\src\curlhttpclient.h"
#include "xbmc_pvr_types.h"
#include "libXBMC_pvr.h"

using namespace dvblinkremote;
using namespace dvblinkremotehttp;

class DVBLinkClient
{
public:
	DVBLinkClient(CHelper_libXBMC_pvr *PVR, std::string hostname, long port, std::string username, std::string password);
	~DVBLinkClient(void);
	int GetChannelsAmount();
	PVR_ERROR GetChannels(ADDON_HANDLE handle, bool bRadio);

	bool GetStatus();
private:

	CurlHttpClient* httpClient; 
	IDVBLinkRemoteConnection* dvblinkRemoteCommunication;
	bool connected;	
	ChannelList* channels;
	CHelper_libXBMC_pvr *PVR;
};

