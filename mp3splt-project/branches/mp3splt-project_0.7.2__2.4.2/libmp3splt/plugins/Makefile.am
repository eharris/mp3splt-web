plugindir=$(libdir)/libmp3splt

plugin_LTLIBRARIES = 

#ccommon_LDFLAGS = -module -export-dynamic -avoid-version
common_CPPFLAGS = -I$(top_srcdir)/include/libmp3splt -I$(top_srcdir)/src
common_LDFLAGS = -L$(top_builddir)/src -L$(top_builddir)/src/.libs -L/lib \
   -no-undefined -lm -lmp3splt @LIBINTL@

if WIN32
common_LDFLAGS += -lz -lws2_32 -lintl
endif

#mp3 plugin
if MP3_PLUGIN

plugin_LTLIBRARIES += libsplt_mp3.la
libsplt_mp3_la_SOURCES = mp3.c mp3.h mp3_silence.c mp3_silence.h mp3_utils.c mp3_utils.h \
silence_processors.c silence_processors.h

libsplt_mp3_la_CPPFLAGS = $(common_CPPFLAGS) @MAD_CFLAGS@
libsplt_mp3_la_LDFLAGS = $(common_LDFLAGS) @MAD_LIBS@

if ID3TAG
libsplt_mp3_la_CPPFLAGS += @ID3_CFLAGS@
libsplt_mp3_la_LIBADD = @ID3_LIBS@
else
libsplt_mp3_la_CPPFLAGS += -DNO_ID3TAG
endif

endif

#OGG plugin
if OGG_PLUGIN

plugin_LTLIBRARIES += libsplt_ogg.la
libsplt_ogg_la_SOURCES = ogg.c ogg.h ogg_silence.c ogg_silence.h ogg_utils.c ogg_utils.h \
silence_processors.c silence_processors.h ogg_new_stream_handler.c ogg_new_stream_handler.h

libsplt_ogg_la_CPPFLAGS = $(common_CPPFLAGS) @OGG_CFLAGS@ @VORBIS_CFLAGS@
libsplt_ogg_la_LDFLAGS = $(common_LDFLAGS) @VORBISFILE_LIBS@ @VORBIS_LIBS@ @OGG_LIBS@ 

endif

