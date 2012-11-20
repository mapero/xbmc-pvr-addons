noinst_LTLIBRARIES = libdvblinkremote.la

libdvblinkremote_la_SOURCES = 	add_manual_schedule_request.cpp \
	add_schedule_by_epg_request.cpp \
	add_schedule_request.cpp \
	add_schedule_request_serializer.cpp \
	channel.cpp \
	curlhttpclient.cpp \
	dvblinkremotecommunication.cpp \
	dvblinkremote.cpp \
	dvblinkremotehttp.cpp \
	epg.cpp \
	generic_response.cpp \
	get_object_request.cpp \
	get_recordings_request.cpp \
	get_schedules_request.cpp \
	http_live_stream_request.cpp \
	parental_lock.cpp \
	program.cpp \
	raw_http_stream_request.cpp \
	raw_udp_stream_request.cpp \
	recording.cpp \
	remove_object_request.cpp \
	remove_recording_request.cpp \
	remove_schedule_request.cpp \
	rtp_stream_request.cpp \
	schedule.cpp \
	stop_stream_request.cpp \
	stream.cpp \
	stream_request.cpp \
	stream_request_serializer.cpp \
	transcoded_video_stream_request.cpp \
	transcoding_options.cpp \
	util.cpp \
	windows_media_stream_request.cpp \
	xml_object_serializer.cpp

LIBS= @abs_top_srcdir@/lib/libcurl/lib/libcurl.la  @abs_top_srcdir@/lib/tinyxml2/libtinyxml2.la
 
INCLUDES=-I@abs_top_srcdir@/lib/libcurl/include -I..

$(LIB): libdvblinkremote.la
	cp -f .libs/libdvblinkremote.a .
	cp -f .libs/libdvblinkremote.la $(LIB)

