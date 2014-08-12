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
#include <parsers/JSONLexer.h>

namespace muzzley {

	class JSONTokenizerLexer: public JSONLexer {
		public:
			JSONTokenizerLexer(std::istream &_in = std::cin, std::ostream &_out = std::cout, JSONObj* _rootobj = NULL, JSONArr* _rootarr = NULL);
			virtual ~JSONTokenizerLexer();

			void switchRoots(JSONObj* _rootobj, JSONArr* _rootarr);

			void result(muzzley::JSONType _in);
			void finish(muzzley::JSONType _in);

			void init(muzzley::JSONType _in_type, const string _in_str);
			void init(muzzley::JSONType _in_type);
			void init(bool _in);
			void init(long long _in);
			void init(double _in);
			void init(string _in);

			void add();

			JSONObj* __root_obj;
			JSONArr* __root_arr;
			JSONType __root_type;
			JSONElement* __value;
			JSONElement* __parent;
			vector<JSONElement*> __context;
	};

}
