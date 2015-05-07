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

#include <muzzley/config.h>
#ifdef HAVE_HTTP_CLIENT

#define DEBUG_JSON

#include <ostream>
#include <vector>
#include <map>
#include <memory>
#include <muzzley/text/convert.h>
#include <muzzley/text/manip.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

#ifndef CRLF
#define CRLF "\r\n"
#endif

namespace muzzley {

	/**
	 * \brief Alias for an header entry to be ignored.
	 */
	extern string nil_header;
	/**
	 * \brief Table of textual representations for HTTP request methods.
	 */
	extern const char* method_names[];
	/**
	 * \brief Table of textual representations for HTTP status codes.
	 */
	extern const char* status_names[];

	/**
	 * \brief Enumeration of muzzley::HTTPObj types: either is a request or reply object (muzzley::HTTPReqT or muzzley::HTTPRepT).
	 */
	enum HTTPType {
		HTTPRequest,
		HTTPReply
	};

	/**
	 * \brief Enumeration of allowed HTTP request methods, as described in RFC 723X (<http://tools.ietf.org/html/rfc7231#section-4>)
	 */	
	enum HTTPMethod {
		HTTPGet = 0,
		HTTPPut = 1,
		HTTPPost = 2,
		HTTPDelete = 3,
		HTTPHead = 4,
		HTTPTrace = 5,
		HTTPOptions = 6,
		HTTPPatch = 7,
		HTTPConnect = 8
	};

	/**
	 * \brief Enumeration of allowed HTTP status codes, as described in RFC 723X (<http://tools.ietf.org/html/rfc7231#section-6>)
	 */	
	enum HTTPStatus {
		HTTP100 = 100,
		HTTP101 = 101,
		HTTP102 = 102,
		HTTP200 = 200,
		HTTP201 = 201,
		HTTP202 = 202,
		HTTP203 = 203,
		HTTP204 = 204,
		HTTP205 = 205,
		HTTP206 = 206,
		HTTP207 = 207,
		HTTP208 = 208,
		HTTP226 = 226,
		HTTP300 = 300,
		HTTP301 = 301,
		HTTP302 = 302,
		HTTP303 = 303,
		HTTP304 = 304,
		HTTP305 = 305,
		HTTP306 = 306,
		HTTP307 = 307,
		HTTP308 = 308,
		HTTP400 = 400,
		HTTP401 = 401,
		HTTP402 = 402,
		HTTP403 = 403,
		HTTP404 = 404,
		HTTP405 = 405,
		HTTP406 = 406,
		HTTP407 = 407,
		HTTP408 = 408,
		HTTP409 = 409,
		HTTP410 = 410,
		HTTP411 = 411,
		HTTP412 = 412,
		HTTP413 = 413,
		HTTP414 = 414,
		HTTP415 = 415,
		HTTP416 = 416,
		HTTP417 = 417,
		HTTP422 = 422,
		HTTP423 = 423,
		HTTP424 = 424,
		HTTP425 = 425,
		HTTP426 = 426,
		HTTP427 = 427,
		HTTP428 = 428,
		HTTP429 = 429,
		HTTP430 = 430,
		HTTP431 = 431,
		HTTP500 = 500,
		HTTP501 = 501,
		HTTP502 = 502,
		HTTP503 = 503,
		HTTP504 = 504,
		HTTP505 = 505,
		HTTP506 = 506,
		HTTP507 = 507,
		HTTP508 = 508,
		HTTP509 = 509,
		HTTP510 = 510,
		HTTP511 = 511
	};

	/**
	 * \brief Type definition for the string map used to hold both header name and value.
	 */
	typedef map< string, string > HeaderMap;
	/**
	 * \brief Type definition for the string map used to hold both parameter name and value.
	 */
	typedef map< string, string > ParameterMap;

	/**
	 * \brief HTTP element super class. 
	 *
	 * It may be, either, a muzzley::HTTPReq or a muzzley::HTTPRep
	 */
	class HTTPObj {
	public:
		/**
		 * \brief Creates a new HTTPObj instance.
		 */
		HTTPObj();
		/**
		 * \brief Destroys the current HTTPObj instance, freeing all allocated memory.
		 */
		virtual ~HTTPObj();

		/**
		 * \brief Read-access method for retrieving the BODY entity.
		 *
		 * @return the BODY entity or an empty string if it doesn't exists
		 */
		string& body();
		/**
		 * \brief Write-access method for instantiating the BODY entity.
		 * 
		 * *NOTE*: it **DOES NOT** automatically instantiate or change the *Content-Length* header, this has to be done manually.
		 *
		 * @param _body the string containing the BODY entity
		 */
		void body(string);
		/**
		 * \brief Read-access method for retrieving the header name/value map.
		 *
		 * @return the header map
		 */
		HeaderMap& headers();
		/**
		 * \brief Read-access method for retrieving the value for the header named *name*.
		 *
		 * @param  _name the header name to retrieve the value for
		 *
		 * @return       the value for the header named *name*
		 */
		string header(const char* _name);
		/**
		 * \brief Write-access method for adding an HTTP header to *this* object.
		 *
		 * @param _name  the header name
		 * @param _value the header value
		 */
		void header(const char* _name, const char* _value);
		/**
		 * \brief Write-access method for adding an HTTP header to *this* object.
		 *
		 * @param _name  the header name
		 * @param _value the header value
		 */
		void header(const char* _name, string _value);
		/**
		 * \brief Write-access method for adding an HTTP header to *this* object.
		 *
		 * @param _name  the header name
		 * @param _value the header value
		 */
		void header(string _name, string _value);

	protected:
		string __body;
		HeaderMap __headers;
	};

	/**
	 * \brief Smart shared pointer to a muzzley::HTTPObj.
	 */
	class HTTPPtr :public shared_ptr< HTTPObj > {
	public:
		/**
		 * \brief Creates a new HTTPPtr instance, pointing to a *null* object.
		 */
		HTTPPtr();
		/**
		 * \brief Creates a new HTTPPtr instance, pointing to the *target* object.
		 */
		HTTPPtr(HTTPObj* _target);
		/**
		 * \brief Destroys the current HTTPPtr instance. It will only free the pointed object if there are no more *shared_ptr* objects pointing to it.
		 */
		virtual ~HTTPPtr();
	};

	/**
	 * \brief Retrieves the enumerated representation of an HTTP request method textual representation.
	 *
	 * @param _in  the HTTP request method textual representation
	 * @param _out the HTTP request method enumerated representation
	 */
	void fromstr(string& _in, HTTPMethod* _out);

	/**
	 * \brief Object representation of an HTTP request.
	 */
	class HTTPReqT: public HTTPObj {
	public:
		/**
		 * \brief Creates a new HTTPReqT instance.
		 */		
		HTTPReqT();
		/**
		 * \brief Destroys the current HTTPReqT instance, freeing all allocated memory.
		 */
		virtual ~HTTPReqT();

		/**
		 * \brief Read-access method for retrieving the HTTP request method for *this* instance.
		 *
		 * @return the enumerated value for the HTTP request method
		 */
		HTTPMethod method();
		/**
		 * \brief Write-access method that instantiates *this* instance enumerated HTTP request method.
		 *
		 * @param HTTPMethod the enumerated representation of the HTTP request method
		 */
		void method(HTTPMethod);
		/**
		 * \brief Read-access method for retrieving the HTTP request resource path
		 *
		 * @return the textual representation of the HTTP request resource path
		 */
		string& url();
		/**
		 * \brief Write-access method that instantiates *this* instance HTTP request resource path.
		 *
		 * @param string the textual representation of the HTTP request resource path
		 */
		void url(string);
		/**
		 * \brief Read-access method for retrieving *this* instance HTTP request query string.
		 *
		 * @return the HTTP request query string
		 */
		string& query();
		/**
		 * \brief Write-access method that instantiates *this* instance HTTP request query string.
		 *
		 * @param string the query string
		 */
		void query(string);
		/**
		 * \brief Read-access method for retrieving the name/value pair map representation of the HTTP request query string.
		 *
		 * @return the name/value pair map representation of the query string
		 */
		ParameterMap& params();
		/**
		 * \brief Read-access method for retrieving the value for an HTTP request query string named *name*
		 *
		 * @param  _name the query string parameter name
		 *
		 * @return       the value for the query string parameter named *name*
		 */
		string param(const char* _name);
		/**
		 * \brief Write-access method for setting the value of the HTTP request query string named *name* to *value*
		 *
		 * @param _name  the query string parameter name
		 * @param _value the query string parameter value
		 */
		void param(const char* _name, const char* _value);
		/**
		 * \brief Write-access method for setting the value of the HTTP request query string named *name* to *value*
		 *
		 * @param _name  the query string parameter name
		 * @param _value the query string parameter value
		 */
		void param(const char* _name, string _value);
		/**
		 * \brief Write-access method for setting the value of the HTTP request query string named *name* to *value*
		 *
		 * @param _name  the query string parameter name
		 * @param _value the query string parameter value
		 */
		void param(string _name, string _value);

		/**
		 * \brief Retrieves the textual representation of *this* HTTP request instance.
		 *
		 * @param _out the textual representation for the HTTP request
		 */
		virtual void stringify(string& _out);
		/**
		 * \brief Outputs to the std::ostring *out* the textual representation of *this* HTTP request instance.
		 *
		 * @param _out the std::ostream to output the HTTP representation to
		 */
		virtual void stringify(ostream& _out);

	private:
		string __url;
		string __query;
		HTTPMethod __method;
		ParameterMap __params;
	};

	/**
	 * \brief Object representation of an HTTP reply.
	 */
	class HTTPRepT: public HTTPObj {
	public:
		/**
		 * \brief Creates a new HTTPRepT instance.
		 */		
		HTTPRepT();
		/**
		 * \brief Destroys the current HTTPRepT instance, freeing all allocated memory.
		 */
		virtual ~HTTPRepT();

		/**
		 * \brief Read-access method for retrieving the enumerated represetation of *this* HTTP reply status code.
		 *
		 * @return the enumerated representation of *this* HTTP reply status code
		 */
		HTTPStatus status();
		/**
		 * \brief Write-access method for instantiating *this* HTTP reply status code
		 *
		 * @param HTTPStatus the enumerated representation of the HTTP reply status code
		 */
		void status(HTTPStatus);

		/**
		 * \brief Retrieves the textual representation of *this* HTTP reply instance.
		 *
		 * @param _out the textual representation for the HTTP reply
		 */
		virtual void stringify(string& _out);
		/**
		 * \brief Outputs to the std::ostring *out* the textual representation of *this* HTTP reply instance.
		 *
		 * @param _out the std::ostream to output the HTTP representation to
		 */
		virtual void stringify(ostream& _out);

	private:
		HTTPStatus __status;
	};

	/**
	 * \brief Smart shared pointer to a muzzley::HTTPReqT.
	 */
	class HTTPReq :public shared_ptr< HTTPReqT > {
	public:
		/**
		 * \brief Creates a new HTTPReq instance, pointing to a *null* object.
		 */
		HTTPReq();
		/**
		 * \brief Creates a new HTTPReq instance, pointing to the *target* object.
		 */
		HTTPReq(HTTPReqT* _target);
		/**
		 * \brief Destroys the current HTTPReq instance. It will only free the pointed object if there are no more *shared_ptr* objects pointing to it.
		 */
		virtual ~HTTPReq();	

		/**
		 * \brief Parses the HTTP request available in *in* into the muzzley::HTTPReqT object pointed by *this* instance
		 *
		 * @param _in the input stream holding the HTTP request streamable representation
		 */
		virtual void parse(istream& _in);

		/**
		 * \brief Friendly *<<* std::ostream operator override that ouputs the textual representation of a muzzley::HTTPReq to an std::ostream object. 
		 */
		friend ostream& operator<<(ostream& _out, HTTPReq& _in) {
			_in->stringify(_out);
			return _out;
		};

		/**
		 * \brief Friendly *>>* std::istream operator override that parses the textual representation available on an std::istream object into a of a muzzley::HTTPReq object.
		 */
		friend istream& operator>>(istream& _in, HTTPReq& _out) {
			_out.parse(_in);
			return _in;
		};
	};

	/**
	 * \brief Smart shared pointer to a muzzley::HTTPRepT.
	 */
	class HTTPRep : public shared_ptr< HTTPRepT > {
	public:
		/**
		 * \brief Creates a new HTTPRep instance, pointing to a *null* object.
		 */
		HTTPRep();
		/**
		 * \brief Creates a new HTTPRep instance, pointing to the *target* object.
		 */
		HTTPRep(HTTPRepT* _target);
		/**
		 * \brief Destroys the current HTTPRep instance. It will only free the pointed object if there are no more *shared_ptr* objects pointing to it.
		 */
		virtual ~HTTPRep();	

		/**
		 * \brief Parses the HTTP reply available in *in* into the muzzley::HTTPRepT object pointed by *this* instance
		 *
		 * @param _in the input stream holding the HTTP reply streamable representation
		 */
		virtual void parse(istream& _in);

		/**
		 * \brief Friendly *<<* std::ostream operator override that ouputs the textual representation of a muzzley::HTTPRep to an std::ostream object. 
		 */
		friend ostream& operator<<(ostream& _out, HTTPRep& _in) {
			_in->stringify(_out);
			return _out;
		};

		/**
		 * \brief Friendly *>>* std::istream operator override that parses the textual representation available on an std::istream object into a of a muzzley::HTTPRep object.
		 */
		friend istream& operator>>(istream& _in, HTTPRep& _out) {
			_out.parse(_in);
			return _in;
		};
	};

	void init(HTTPReq& _out);
	void init(HTTPRep& _out);
}
#endif