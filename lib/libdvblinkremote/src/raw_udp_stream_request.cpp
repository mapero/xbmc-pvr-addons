/***************************************************************************
 * Copyright (C) 2012 Marcus Efraimsson.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************/

#include "dvblinkremote.h"
#include "request.h"

using namespace dvblinkremote;

RawUdpStreamRequest::RawUdpStreamRequest(const std::string& serverAddress, const long channelDvbLinkId, const std::string& clientId, const std::string& clientAddress, const unsigned short int streamingPort)
  : m_clientAddress(clientAddress), m_streamingPort(streamingPort), StreamRequest(serverAddress, channelDvbLinkId, clientId, DVBLINK_REMOTE_STREAM_TYPE_RAW_UDP)
{ }

RawUdpStreamRequest::~RawUdpStreamRequest()
{

}

std::string& RawUdpStreamRequest::GetClientAddress() 
{ 
  return m_clientAddress; 
}

long RawUdpStreamRequest::GetStreamingPort() 
{ 
  return m_streamingPort; 
}
