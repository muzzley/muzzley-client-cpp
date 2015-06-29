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

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <streambuf>
#include <iostream>
#include <istream>
#include <ostream>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <muzzley/exceptions/ClosedException.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

namespace muzzley {

	template<typename Char>
	class basic_socketbuf : public std::basic_streambuf<Char> {
	public:
		typedef Char __char_type;
		typedef std::basic_streambuf<__char_type> __buf_type;
		typedef std::basic_ostream<__char_type> __stream_type;
		typedef typename __buf_type::int_type __int_type;
		typedef typename std::basic_streambuf<Char>::traits_type __traits_type;

	protected:

		static const int char_size = sizeof(__char_type);
		static const int SIZE = 128;
		__char_type obuf[SIZE];
		__char_type ibuf[SIZE];

		int __sock;

	public:
		basic_socketbuf() : __sock(0) {
			__buf_type::setp(obuf, obuf + (SIZE - 1));
			__buf_type::setg(ibuf, ibuf, ibuf);
		};

		virtual ~basic_socketbuf() {
			sync();
		}

		void set_socket(int _sock) {
			this->__sock = _sock;
			if (_sock != 0) {
				int iOption = 1; 
				setsockopt(this->__sock, SOL_SOCKET, SO_KEEPALIVE, (const char *) &iOption,  sizeof(int));
			}
		}

		int get_socket() {
			return this->__sock;
		}

		virtual bool __good() {
			return this->__sock != 0;
		}

	protected:

		int output_buffer() {
			if (!__good()) {
				return __traits_type::eof();
			}

			int _num = __buf_type::pptr() - __buf_type::pbase();
			int _actually_written = -1;
			if ((_actually_written = ::send(__sock, reinterpret_cast<char*>(obuf), _num * char_size, MSG_NOSIGNAL)) < 0) {
				if (_actually_written < 0) {
					::shutdown(this->__sock, SHUT_RDWR);
					::close(this->__sock);
					this->__sock = 0;
				}
				return __traits_type::eof();
			}
			__buf_type::pbump(-_actually_written);
			return _actually_written;
		}

		virtual __int_type overflow(__int_type c) {
			if (c != __traits_type::eof()) {
				*__buf_type::pptr() = c;
				__buf_type::pbump(1);
			}

			if (output_buffer() == __traits_type::eof()) {
				return __traits_type::eof();
			}
			return c;
		}

		virtual int sync() {
			if (output_buffer() == __traits_type::eof()) {
				return __traits_type::eof();
			}
			return 0;
		}

		virtual __int_type underflow() {
			if (__buf_type::gptr() < __buf_type::egptr()) {
				return *__buf_type::gptr();
			}

			if (!__good()) {
				return __traits_type::eof();
			}

			int num = -1;
			if ((num = ::recv(__sock, reinterpret_cast<char*>(ibuf), SIZE * char_size, MSG_NOSIGNAL)) <= 0) {
				if (num < 0) {
					::shutdown(this->__sock, SHUT_RDWR);
					::close(this->__sock);
					this->__sock = 0;
				}
				return __traits_type::eof();
			}
			__buf_type::setg(ibuf, ibuf, ibuf + num);
			return *__buf_type::gptr();
		}
	};

	typedef basic_socketbuf<char> socketbuf;
	typedef basic_socketbuf<wchar_t> wsocketbuf;

	template<typename Char>
	class basic_socketstream : public std::basic_iostream<Char> {
	public:
		typedef Char __char_type;
		typedef std::basic_iostream<__char_type> __stream_type;
		typedef basic_socketbuf<__char_type> __buf_type;

	protected:
		__buf_type __buf;

	public:
		basic_socketstream() :
		__stream_type(&__buf) {
		};

		basic_socketstream(int s) : __stream_type(&__buf) {
			__buf.set_socket(s);
		}
		virtual ~basic_socketstream() {
			this->close();
		}

		void assign(int _sockfd) {
			__buf.set_socket(_sockfd);
		}

		void close() {
			__stream_type::flush();
			__stream_type::clear();
			if (__buf.get_socket() != 0) {
				::shutdown(__buf.get_socket(), SHUT_RDWR);
				::close(__buf.get_socket());
			}
			__buf.set_socket(0);
		}

		bool is_open() {
			bool _return = (__buf.get_socket() != 0 && __buf.__good());
			return _return;
		}

		bool ready() {
			fd_set sockset;
			FD_ZERO(&sockset);
			FD_SET(__buf.get_socket(), &sockset);
			return select(__buf.get_socket() + 1, &sockset, nullptr, nullptr, nullptr) == 1;
		}

		__buf_type& buffer() {
			return this->__buf;
		}

		bool open(const std::string& _host, uint16_t _port) {
			this->close();
			int _sd = socket(AF_INET, SOCK_STREAM, 0);
			sockaddr_in _sin;
			hostent *_he = gethostbyname(_host.c_str());
			if (_he == nullptr) {
				return false;
			}

			std::copy(reinterpret_cast<char*>(_he->h_addr), reinterpret_cast<char*>(_he->h_addr) + _he->h_length, reinterpret_cast<char*>(&_sin.sin_addr.s_addr));
			_sin.sin_family = AF_INET;
			_sin.sin_port = htons(_port);

			if (::connect(_sd, reinterpret_cast<sockaddr*>(& _sin), sizeof(_sin)) < 0) {
				__stream_type::setstate(std::ios::failbit);
				__buf.set_socket(0);
				return false;
			}
			else {
				__buf.set_socket(_sd);
			}
			return true;
		}
	};

	typedef basic_socketstream<char> socketstream;
	typedef basic_socketstream<wchar_t> wsocketstream;

	template<typename Char>
	class basic_serversocketstream : public std::basic_iostream<Char> {
	public:
		typedef Char __char_type;
		typedef std::basic_iostream<__char_type> __stream_type;
		typedef basic_socketbuf<__char_type> __buf_type;

	protected:
		__buf_type __buf;
		int __sockfd;

	public:
		basic_serversocketstream() :
		__stream_type(&__buf) {
		};

		basic_serversocketstream(int s) : __stream_type(&__buf) {
			__buf.set_socket(s);
		}
		virtual ~basic_serversocketstream() {
			this->close();
		}

		void close() {
			__stream_type::flush();
			__stream_type::clear();
			if (__buf.get_socket() != 0) {
				::shutdown(__buf.get_socket(), SHUT_RDWR);
				::close(__buf.get_socket());
			}
			__buf.set_socket(0);
		}

		bool is_open() {
			return __buf.get_socket() != 0 && __buf.__good();
		}

		bool ready() {
			fd_set sockset;
			FD_ZERO(&sockset);
			FD_SET(__buf.get_socket(), &sockset);
			return select(__buf.get_socket() + 1, &sockset, nullptr, nullptr, nullptr) == 1;
		}

		__buf_type& buffer() {
			return this->__buf;
		}

		bool bind(uint16_t _port) {
			this->__sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (this->__sockfd < 0) {
				__stream_type::setstate(std::ios::failbit);
				return false;
			}

			int _opt = 1;
			if (setsockopt(this->__sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &_opt, sizeof(_opt)) == SO_ERROR) {
				::shutdown(this->__sockfd, SHUT_RDWR);
				::close(this->__sockfd);
				this->__sockfd = 0;
				__stream_type::setstate(std::ios::failbit);
				return false;
			}

			struct sockaddr_in _serv_addr;
			bzero((char *) &_serv_addr, sizeof(_serv_addr));
			_serv_addr.sin_family = AF_INET;
			_serv_addr.sin_addr.s_addr = INADDR_ANY;
			_serv_addr.sin_port = htons(_port);
			if (::bind(this->__sockfd, (struct sockaddr *) &_serv_addr, sizeof(_serv_addr)) < 0) {
				::shutdown(this->__sockfd, SHUT_RDWR);
				::close(this->__sockfd);
				this->__sockfd = 0;
				__buf.set_socket(0);
				__stream_type::setstate(std::ios::failbit);
				return false;
			}
			::listen(this->__sockfd, 100);
			__buf.set_socket(this->__sockfd);
			return true;
		}

		bool accept(socketstream* _out) {
			if (this->__sockfd != -1) {
				struct sockaddr_in* _cli_addr = new struct sockaddr_in();
				socklen_t _clilen = sizeof(struct sockaddr_in);
				int _newsockfd = ::accept(this->__sockfd, (struct sockaddr *) _cli_addr, &_clilen);

				if (_newsockfd < 0) {
					return false;
				}

				struct linger _so_linger;
				_so_linger.l_onoff = 1;
				_so_linger.l_linger = 30;
				::setsockopt(_newsockfd,SOL_SOCKET, SO_LINGER, &_so_linger, sizeof _so_linger);
				_out->assign(_newsockfd);
				return true;
			}
			return false;
		}

		bool accept(int* _out) {
			if (this->__sockfd != -1) {
				struct sockaddr_in* _cli_addr = new struct sockaddr_in();
				socklen_t _clilen = sizeof(struct sockaddr_in);
				int _newsockfd = ::accept(this->__sockfd, (struct sockaddr *) _cli_addr, &_clilen);

				if (_newsockfd < 0) {
					return false;
				}

				struct linger _so_linger;
				_so_linger.l_onoff = 1;
				_so_linger.l_linger = 30;
				::setsockopt(_newsockfd,SOL_SOCKET, SO_LINGER, &_so_linger, sizeof _so_linger);
				*_out = _newsockfd;
				return true;
			}
			return false;
		}
	};

	typedef basic_serversocketstream<char> serversocketstream;
	typedef basic_serversocketstream<wchar_t> wserversocketstream;

}
