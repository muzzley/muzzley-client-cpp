/*
Copyright (c) 2014, Muzzley

Permission to use, copy, modify, and/or distribute this software for 
any purpose with or without fee is hereby granted, provided that the 
above copyright notice and this permission notice appear in all 
copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE 
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR 
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER 
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
PERFORMANCE OF THIS SOFTWARE.
*/

#include <muzzley/base/assert.h>

#include <muzzley/exceptions/AssertionException.h>
#include <muzzley/exceptions/CastException.h>
#include <muzzley/exceptions/ClosedException.h>
#include <muzzley/exceptions/InterruptedException.h>
#include <muzzley/exceptions/NoAttributeNameException.h>
#include <muzzley/exceptions/ParserEOF.h>
#include <muzzley/exceptions/SyntaxErrorException.h>

#include <muzzley/json/JSONObj.h>

#include <muzzley/http/HTTPObj.h>

#include <muzzley/parsers/json.h>
#include <muzzley/parsers/http.h>

#include <muzzley/resource/Client.h>

#include <muzzley/stream/SocketStreams.h>
#include <muzzley/stream/SSLSocketStreams.h>

#include <muzzley/text/convert.h>
#include <muzzley/text/manip.h>

#include <muzzley/log/log.h>

typedef int semid_t;
