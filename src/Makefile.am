lib_LTLIBRARIES = libmuzzley.la

libmuzzley_la_LIBADD = -lpthread
libmuzzley_la_LDFLAGS = -version-info 0:2:0
libmuzzley_la_CPPFLAGS = -std=c++0x -I../include

libmuzzley_la_SOURCES = \
./base/assert.cpp\
./log/log.cpp\
./exceptions/InterruptedException.cpp\
./exceptions/CastException.cpp\
./exceptions/ParserEOF.cpp\
./exceptions/ClosedException.cpp\
./exceptions/AssertionException.cpp\
./exceptions/NoAttributeNameException.cpp\
./exceptions/SyntaxErrorException.cpp\
./parsers/JSONLexer.cpp\
./parsers/json.cpp\
./parsers/JSONTokenizer.cpp\
./parsers/JSONTokenizerLexer.cpp\
./parsers/JSONParser.cpp\
./parsers/HTTPLexer.cpp\
./parsers/HTTPParser.cpp \
./parsers/HTTPTokenizer.cpp \
./parsers/HTTPTokenizerLexer.cpp \
./parsers/http.cpp \
./text/ascii.cpp\
./text/utf8.cpp\
./text/manip.cpp\
./text/url.cpp\
./text/convert.cpp\
./text/base64.cpp\
./stream/SocketStreams.cpp\
./stream/SSLSocketStreams.cpp\
./resource/Client.cpp\
./json/JSONObj.cpp \
./http/HTTPElement.cpp \
./http/HTTPRep.cpp \
./http/HTTPReq.cpp