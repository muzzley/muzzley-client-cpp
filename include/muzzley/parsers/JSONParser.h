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

#include <muzzley/json/JSONObj.h>
#include <muzzley/parsers/JSONTokenizer.h>

namespace muzzley {

	/**
	 * \brief JSON parser class, wrapper for the bisonc++ (<http://bisoncpp.sourceforge.net/>) and flexc++ (<http://flexcpp.sourceforge.net/>) generated parsers. 
	 * 
	 * The grammar files can be found at <https://github.com/muzzley/muzzley-client-cpp/blob/master/src/parsers/JSON.b> and <https://github.com/muzzley/muzzley-client-cpp/blob/master/src/parsers/JSON.f>.
	 * This class should no be used directly but through the muzzley::JSONPtr **parse** method or '<<' operator, for your convenience.
	 */
	class JSONParser: public JSONTokenizer {
	public:
		/**
		 * \brief Creates a new HTTPRep instance, pointing to a *null* object.
		 *
		 * @param _in  the std::istream where the string representation of the HTTP message is stream for parsing 
		 * @param _out the std::ostream to where the transformed HTTP message may be outputed. Just for reference, *this* parser will not output a transform byte buffer, instead it will instantiate a muzzley::JSONPtr class derivative.
		 */
		 JSONParser(std::istream &_in = std::cin, std::ostream &_out = std::cout);
		/**
		 * \brief Destroys the current JSONParser instance, freeing all allocated memory.
		 */
		 virtual ~JSONParser();

		/**
		 * \brief Access method for passing in the muzzley::JSONPtr object to be populated during parsing
		 *
		 * In order to determine what kind of JSON type the parser outputed, the muzzley::JSONPtr::type() method should be called, for instance:
		 *
		 *     std::ifstream _in;
		 *     _in.open("/home/me/message.json");
		 *     muzzley::JSONPtr _ptr;
		 *     _in >> _ptr;
		 *     switch(_ptr->type()) {
		 *         case muzzley::JSString : {
		 *             ...
		 *         }
		 *         ...
		 *     }
		 *
		 * @param _root the muzzley::JSONPtr object to be populated during parsing
		 */
		 void switchRoots(JSONPtr& _root);
		/**
		 * \brief Write-access method for switching both input and output streams
		 *
		 * @param _in  the input stream to be used for parsing, from now on
		 * @param _out the output stream to be used for transformation output, from now on
		 */
		 void switchStreams(std::istream &_in = std::cin, std::ostream &_out = std::cout);
	};

}
