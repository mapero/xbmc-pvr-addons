#pragma once

#include "platform/os.h"
#include "libdvblinkremote\src\dvblinkremote.h"
#include "libdvblinkremote\src\curlhttpclient.h"
#include "xbmc_pvr_types.h"
#include "libXBMC_addon.h"
#include "libXBMC_pvr.h"
#include "client.h"


using namespace dvblinkremote;
using namespace dvblinkremotehttp;
using namespace ADDON;

class DVBLinkClient
{
public:
	DVBLinkClient(CHelper_libXBMC_addon *XBMC, CHelper_libXBMC_pvr *PVR, DVBLINK_STREAMTYPE streamtype, std::string clientname, std::string hostname, long port, std::string username, std::string password);
	~DVBLinkClient(void);
	int GetChannelsAmount();
	PVR_ERROR GetChannels(ADDON_HANDLE handle, bool bRadio);
	PVR_ERROR GetEPGForChannel(ADDON_HANDLE handle, const PVR_CHANNEL& channel, time_t iStart, time_t iEnd);
	PVR_ERROR GetRecordings(ADDON_HANDLE handle);
	PVR_ERROR GetTimers(ADDON_HANDLE handle);
	bool GetStatus();
	const char * GetLiveStreamURL(const PVR_CHANNEL &channel);
	void StopStreaming();
private:

	CurlHttpClient* httpClient; 
	IDVBLinkRemoteConnection* dvblinkRemoteCommunication;
	bool connected;

	ChannelList* channels;
	Stream * stream;
	CHelper_libXBMC_pvr *PVR;
	CHelper_libXBMC_addon  *XBMC; 
	DVBLINK_STREAMTYPE streamtype;
	std::string clientname;
	std::string hostname;
	void SetEPGGenre(Program *program, EPG_TAG *tag);

};

/*!
 * @brief PVR macros for string exchange
 */
#define PVR_STRCPY(dest, source) do { strncpy(dest, source, sizeof(dest)-1); dest[sizeof(dest)-1] = '\0'; } while(0)
#define PVR_STRCLR(dest) memset(dest, 0, sizeof(dest))