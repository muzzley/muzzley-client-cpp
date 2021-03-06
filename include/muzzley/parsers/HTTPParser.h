/*
The MIT License (MIT)

Copyright (c) 2014 Pedro (n@zgul) Figueiredo <pedro.figueiredo@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <muzzley/config.h>
#ifdef HAVE_HTTP_CLIENT

#include <muzzley/http/HTTPObj.h>
#include <muzzley/parsers/HTTPTokenizer.h>

namespace muzzley {

	/**
	 * \brief HTTP parser class, wrapper for the bisonc++ (<http://bisoncpp.sourceforge.net/>) and flexc++ (<http://flexcpp.sourceforge.net/>) generated parsers. 
	 * 
	 * The grammar files can be found at <https://github.com/muzzley/muzzley-client-cpp/blob/master/src/parsers/HTTP.b> and <https://github.com/muzzley/muzzley-client-cpp/blob/master/src/parsers/HTTP.f>.
	 * This class should no be used directly but through the muzzley::HTTPReq or muzzley::HTTPRep **parse** method or ** << ** operator, for your convenience.
	 */
	class HTTPParser : public HTTPTokenizer {
	public:
		/**
		 * \brief Creates a new HTTPRep instance, pointing to a *null* object.
		 *
		 * @param _in  the std::istream where the textual representation of the HTTP message is stream for parsing 
		 * @param _out the std::ostream to where the transformed HTTP message may be outputed. Just for reference, *this* parser will not output a transform byte buffer, instead it will instantiate a muzzley::HTTOObj class derivative.
		 */
		HTTPParser(std::istream &_in = std::cin, std::ostream &_out = std::cout);
		/**
		 * \brief Destroys the current HTTPParser instance, freeing all allocated memory.
		 */
		virtual ~HTTPParser();

		/**
		 * \brief Access method for passing in the muzzley::HTTPReq object to be populated during parsing
		 *
		 * @param _root the muzzley::HTTPReq object to be populated during parsing
		 */
		void switchRoots(HTTPReq& _root);
		/**
		 * \brief Access method for passing in the muzzley::HTTPRep object to be populated during parsing
		 *
		 * @param _root the muzzley::HTTPRep object to be populated during parsing
		 */
		void switchRoots(HTTPRep& _root);
		/**
		 * \brief Write-access method for switching both input and output streams
		 *
		 * @param _in  the input stream to be used for parsing, from now on
		 * @param _out the output stream to be used for transformation output, from now on
		 */
		void switchStreams(std::istream &_in = std::cin, std::ostream &_out = std::cout);
	};

}
#endif