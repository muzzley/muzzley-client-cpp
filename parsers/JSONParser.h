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


#pragma once

#include <json/JSONObj.h>
#include <parsers/JSONTokenizer.h>

namespace muzzley {

	class JSONParser: public JSONTokenizer {
		public:
			JSONParser(std::istream &_in = std::cin, std::ostream &_out = std::cout, JSONObj* _rootobj = NULL, JSONArr* _rootarr = NULL);
			virtual ~JSONParser();

			void switchRoots(JSONObj* _rootobj = NULL, JSONArr* _rootarr = NULL);
			void switchStreams(std::istream &_in = std::cin, std::ostream &_out = std::cout);
	};

}
