lib_LTLIBRARIES = libicpf.la
libicpf_la_SOURCES = callback.cpp exception.cpp circ_buffer.cpp \
			log.cpp dumpctx.cpp cfg.cpp cfg_xml.cpp config_property.cpp str_help.cpp \
			crc32.cpp file.cpp module.cpp conv.cpp mutex.cpp

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
#libicpf_la_LDFLAGS = $(all_libraries) 
libicpf_la_LIBADD = -ldl -lpthread
dist_include_HEADERS = callback.h exception.h dumpctx.h cfg.h circ_buffer.h module.h \
			file.h log.h cfg.h config_base.h config_property.h str_help.h crc32.h libicpf.h gen_types.h \
			conv.h err_codes.h gen_types.h macros.h mutex.h
AM_CFLAGS = -Wall -Wshadow -DLIBICPF_EXPORTS
AM_CXXFLAGS = -Wall -Wshadow -DLIBICPF_EXPORTS
