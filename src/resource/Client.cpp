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

#include <muzzley/resource/Client.h>
#include <muzzley/base/assert.h>
#include <muzzley/parsers/json.h>
#include <pthread.h>
#include <iterator>
#include <bitset>
#include <muzzley/log/log.h>

#define HEARTBEAT_SECONDS 75

muzzley::Client::Client() : __serial( 1 ),  __endpoint_host("geoplatform.muzzley.com") {
	this->__mtx = new pthread_mutex_t();
	this->__thr = new pthread_t();
	pthread_mutexattr_init(&this->__attr);
	pthread_mutex_init(this->__mtx, &this->__attr);

	this->__is_loop_assynchronous = false;
	this->__op_code = -1;

	this->__is_app_loggedin = false;
	this->__is_user_loggedin = false;
	this->__is_initiating_master = false;
	this->__is_running = false;
	this->__is_one_step_initialization = false;
	this->__has_handshake = false;

	this->__participant_id = -1;

	this->__heartbeat_timer = nullptr;

	muzzley::log_fd = &cout;

	this->on(muzzley::Connect, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		if (_client.__is_one_step_initialization) {
			return true;
		}
		if (!!_data["s"] && !!_data["d"]["deviceId"]) {
			_client.__device_id.assign((string) _data["d"]["deviceId"]);
		}
		return true;
	});

	this->on(muzzley::AppLoggedIn, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		if (_client.__is_one_step_initialization) {
			return true;
		}
		if (!!_data["s"] && !!_data["d"]["sessionId"]) {
			_client.__session_id.assign((string) _data["d"]["sessionId"]);
			_client.__is_app_loggedin = true;
			_client.__is_user_loggedin = false;
			_client.createActivity(_client.__activity_id);
		}
		return true;
	});

	this->on(muzzley::UserLoggedIn, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		if (_client.__is_one_step_initialization) {
			return true;
		}
		if (!!_data["s"] && !!_data["d"]["sessionId"]) {
			_client.__session_id.assign((string) _data["d"]["sessionId"]);
			_client.__is_app_loggedin = false;
			_client.__is_user_loggedin = true;
			_client.joinActivity(_client.__activity_id);
		}
		return true;
	});

	this->on(muzzley::ActivityJoined, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		if (_client.__is_one_step_initialization) {
			return true;
		}
		if ((bool)_data["s"] == true) {
			_client.__participant_id = (long long) _data["d"]["participant"]["id"];
			_client.participantReady();
			return true;
		}
		else if ((bool) _data["s"] == false && !!_data["d"]["connectTo"]) {
			_client.disconnect();
			_client.__endpoint_host.assign((string) _data["d"]["connectTo"]);
			_client.connectUser(_client.__user_token, _client.__activity_id);
			return true;
		}
		return true;
	});

	this->on(muzzley::ActivityCreated, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		if (_client.__is_one_step_initialization) {
			return true;
		}
		if ((bool)_data["s"] == true && !!_data["d"]["activity"]["activityId"]) {
			_client.__activity_id.assign((string) _data["d"]["activity"]["activityId"]);
			_client.__is_initiating_master = true;
			return true;
		}
		else if ((bool)_data["s"] == true && !!_data["d"]["activityId"]) {
			_client.__activity_id.assign((string) _data["d"]["activityId"]);
			_client.__is_initiating_master = true;
			return true;
		}
		else if ((bool) _data["s"] == false && !!_data["d"]["connectTo"]) {
			_client.disconnect();
			_client.__endpoint_host.assign((string) _data["d"]["connectTo"]);
			_client.connectApp(_client.__app_token, _client.__activity_id);
			return false;
		}
		return true;
	});

	this->on(muzzley::ActivityTerminated, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		_client.__activity_id.assign("");
		_client.__is_initiating_master = false;
		if (_client.isReplyNeeded(_data)) {
			muzzley::Message _m(JSON(
				"s" << true
				));
			_client.reply(_data, _m);
		}
		return true;
	});

	this->on(muzzley::ParticipantQuit, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		if (!!_data["d"]["participantId"]) {
			pthread_mutex_lock(_client.__mtx);
			muzzley::ParticipantList::iterator _found = _client.__participants.find((long long) _data["d"]["participantId"]);
			if (_found != _client.__participants.end()) {
				_client.__participants.erase(_found);
			}
			pthread_mutex_unlock(_client.__mtx);

			if (_client.isReplyNeeded(_data)) {
				muzzley::Message _m(JSON(
					"s" << true
					));
				_client.reply(_data, _m);
			}
		}
		else if (_client.isReplyNeeded(_data)) {
			muzzley::Message _m(JSON(
				"s" << false
				));
			_client.reply(_data, _m);
		}
		return true;
	});

	this->on(muzzley::ParticipantJoined, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		if (!!_data["h"]["pid"] && !!_data["d"]) {
			long long _pid = (long long) _data["h"]["pid"];

			pthread_mutex_lock(_client.__mtx);
			muzzley::ParticipantList::iterator _found = _client.__participants.find(_pid);
			if (_found != _client.__participants.end()) {
				muzzley::JSONObj _participant = (muzzley::JSONObj&) muzzley::fromstr(_found->second);
				((muzzley::JSONObj) _data) << "d" << _participant;
			}
			pthread_mutex_unlock(_client.__mtx);

			if (_client.isReplyNeeded(_data)) {
				muzzley::Message _m(JSON(
					"h" << JSON (
						"pid" << _pid
						) <<
					"s" << true
					));
				_client.reply(_data, _m);
			}
		}
		else {
			muzzley::Message _m(JSON(
				"s" << false
				));
			_client.reply(_data, _m);
		}
		return true;
	});

	this->on(muzzley::__INTERNAL_ParticipantJoined__, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		if (!!_data["d"]["participant"]["id"]) {
			if (((string) _data["d"]["participant"]["deviceId"]) == _client.__device_id) {
				_client.__participant_id = (long long) _data["d"]["participant"]["id"];
			}
			pthread_mutex_lock(_client.__mtx);
			_client.__participants.insert(make_pair((long long) _data["d"]["participant"]["id"], (string) _data["d"]));
			pthread_mutex_unlock(_client.__mtx);

			if (_client.isReplyNeeded(_data)) {
				muzzley::Message _m(JSON(
					"s" << true
					));
				_client.reply(_data, _m);
			}
		}
		else if (_client.isReplyNeeded(_data)) {
			muzzley::Message _m(JSON(
				"s" << false
				));
			_client.reply(_data, _m);
		}
		return true;
	});

	this->on(muzzley::ChangeWidget, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		if (_client.isReplyNeeded(_data)) {
			muzzley::Message _m(JSON(
				"s" << true
				));
			_client.reply(_data, _m);
		}
		return true;
	});

	this->on(muzzley::SetupComponent, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		if (_client.isReplyNeeded(_data)) {
			muzzley::Message _m(JSON(
				"s" << true
				));
			_client.reply(_data, _m);
		}
		return true;
	});

	this->on(muzzley::__INTERNAL_PublishUpdate__, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		auto _found = _client.__stack.find((string) _data["h"]["ch"]);
		if (_found != _client.__stack.end()) {
			return _found->second(_data, _client);
		}
		return true;
	});

}

muzzley::Client::~Client() {
	pthread_mutexattr_destroy(&this->__attr);
	pthread_mutex_destroy(this->__mtx);
	delete this->__mtx;
	delete this->__thr;
	pthread_exit(nullptr);
}

void muzzley::Client::on(muzzley::EventType _type, muzzley::Handler _handler) {
	this->__handlers[_type].push_back(_handler);
}

bool muzzley::Client::trigger(muzzley::EventType _type, muzzley::Message& _data) {
	if (_type < 0 || _type > N_EVENT_TYPES) {
		return false;
	}
	for (std::vector<muzzley::Handler>::iterator _i = this->__handlers[_type].begin(); _i != this->__handlers[_type].end(); _i++) {
		if (!(* _i)(_data, * this)) {
			return false;
		}
	}
	return true;
}

void muzzley::Client::on(muzzley::EventType _type, muzzley::Subscription& _to_property, muzzley::Callback _callback) {
	assertz(_type == muzzley::Published, "the '_type' parameter must be muzzley::Published", 500, 300);
	this->subscribe(_to_property, _callback);
}

void muzzley::Client::off(muzzley::EventType _type, muzzley::Subscription& _from_property) {
	assertz(_type == muzzley::Published, "the '_type' parameter must be muzzley::Published", 500, 300);
	this->unsubscribe(_from_property);
}

bool muzzley::Client::trigger(muzzley::EventType _type, muzzley::Subscription& _to_property, muzzley::Message& _payload, muzzley::Callback _callback) {
	assertz(_type == muzzley::Publish, "the '_type' parameter must be muzzley::Publish", 500, 300);
	try {
		this->publish(_to_property, _payload, _callback);
	}
	catch (muzzley::AssertionException& _e) {
		throw;
	}
	return true;
}

bool muzzley::Client::connect(string _host, uint16_t _port, string _path) {
	string _message("GET ");
	_message.insert(_message.length(), _path);
	_message.insert(_message.length(), " HTTP/1.1");
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Upgrade: websocket");
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Connection: Upgrade");
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Host: ");
	_message.insert(_message.length(), _host);
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Origin: C++WSClient");
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Sec-WebSocket-Version: 13");
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), "Sec-WebSocket-Key: ");

	string _key;
	for (int i = 0; i != 16; i++) {
		_key.push_back(255 * random());
	}
	muzzley::base64_encode(_key);
	_message.insert(_message.length(), _key);
	_message.insert(_message.length(), CRLF);
	_message.insert(_message.length(), CRLF);

	while (!this->__channel.open(_host, _port)) {
		muzzley::Message _empty;
		if (!this->trigger(muzzley::Reconnect, _empty)) {
			throw muzzley::ClosedException("could not connect to host");
		}
		sleep(HEARTBEAT_SECONDS);
	}
#ifdef MUZZLEY_DEBUG
	{
		string _log("connecting to ");
		_log.insert(_log.length(), _host);
		_log.insert(_log.length(), ":");
		muzzley::tostr(_log, _port);
		muzzley::log(_log, muzzley::notice);
	}
#endif
	this->__channel << _message << flush;

	string _reply;
#ifdef MUZZLEY_DEBUG
	string _change_protocol;
#endif
	do {
		getline(this->__channel, _reply);
#ifdef MUZZLEY_DEBUG
		_change_protocol.insert(_change_protocol.length(), _reply);
		_change_protocol.insert(_change_protocol.length(), "\n");
#endif
		muzzley::trim(_reply);
	}
	while (_reply != "");
#ifdef MUZZLEY_DEBUG
	{
		string _log("received WebSocket handshake");
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->arm();
	return true;
}

void muzzley::Client::disconnect() {
	this->__channel << (uint8_t) 0x87;
	this->__channel << (uint8_t) 0x00;
	this->__channel << flush;
	this->__channel.close();
}

bool muzzley::Client::reconnect() {
	if (this->isConnected()) {
		this->disconnect();
	}
	this->disarm();
	this->__queue.erase(this->__queue.begin(), this->__queue.end());
	this->__participants.erase(this->__participants.begin(), this->__participants.end());
	this->__stack.erase(this->__stack.begin(), this->__stack.end());
	this->__namespaces.erase(this->__namespaces.begin(), this->__namespaces.end());
	muzzley::Message _empty;
	if (this->trigger(muzzley::Reconnect, _empty)) {
		if (this->__app_token.length() != 0) {
			if (this->__is_one_step_initialization) {
				this->initApp(this->__app_token);
			}
			else {
				this->connectApp(this->__app_token, this->__activity_id);
			}
		}
		else {
			if (this->__is_one_step_initialization) {
				this->initUser(this->__user_token);			
			}
			else {
				this->connectUser(this->__user_token, this->__activity_id);
			}
		}
		return true;
	}
	return false;
}

void __HEARTBEAT_CALLBACK__(union sigval _arg) {
	muzzley::Client * _client = (muzzley::Client *) _arg.sival_ptr;
	_client->disconnect();
}

bool muzzley::Client::arm() {
	struct sigevent _se;
	struct itimerspec _ts;

	_se.sigev_notify = SIGEV_THREAD;
	_se.sigev_value.sival_ptr = this;
	_se.sigev_notify_function = __HEARTBEAT_CALLBACK__;
	_se.sigev_notify_attributes = nullptr;

	if ( -1 == timer_create(CLOCK_REALTIME, & _se, & this->__heartbeat_timer)) {
		return false;
	}

	_ts.it_value.tv_sec = HEARTBEAT_SECONDS;
	_ts.it_value.tv_nsec = 0;
	_ts.it_interval.tv_sec = HEARTBEAT_SECONDS;
	_ts.it_interval.tv_nsec = 0 ;

	if (-1 == timer_settime(this->__heartbeat_timer, 0, & _ts, nullptr)) {
		return false;
	}
	return true;
}

bool muzzley::Client::disarm() {
	if (this->__heartbeat_timer != nullptr) {
		timer_delete(this->__heartbeat_timer);
		this->__heartbeat_timer = nullptr;
	}
	return true;
}

bool muzzley::Client::read() {
	unsigned char _hdr;
	this->__channel >> noskipws >> _hdr;

	bool _fin = _hdr & 0x80;
	this->__op_code = _hdr & 0x0F;
	this->__channel >> noskipws >> _hdr;
	bool _mask = _hdr & 0x80;

	int _len = _hdr & 0x7F;
	if (_len == 126) {
		this->__channel >> noskipws >> _hdr;
		_len = (int) _hdr;
		_len <<= 8;
		this->__channel >> noskipws >> _hdr;
		_len += (int) _hdr;
	}
	else if (_len == 127) {
		this->__channel >> noskipws >> _hdr;
		_len = (int) _hdr;
		for (int _i = 0; _i < 7; _i++) {
			_len <<= 8;
			this->__channel >> noskipws >> _hdr;
			_len += (int) _hdr;
		}
	}

	if (_mask) {
		for (int _i = 0; _i < 4; _i++) {
			this->__channel >> noskipws >> _hdr;
		}
	}

	for (int _i = 0; _i != _len; _i++) {
		this->__channel >> noskipws >> _hdr;
		this->__message.push_back((char) _hdr);
	}

	return _fin;
}

bool muzzley::Client::write(muzzley::Message& _message, muzzley::Callback _callback) {
	if ((muzzley::MessageType) ((int) _message["h"]["t"]) == muzzley::RequestInitiatedByEndpoint && !!_message["h"]["cid"]) {
		string _action = (string) _message["a"];
		pthread_mutex_lock(this->__mtx);
		this->__queue.insert(make_pair(this->__serial, make_pair(_action, _callback)));
		pthread_mutex_unlock(this->__mtx);
		this->__serial++;
	}

	string _to_send;
	muzzley::tostr(_to_send, _message);
	return this->write(_to_send);
}

bool muzzley::Client::write(string& _to_send) {
	if (!this->isConnected()) {
		if (!this->reconnect()) {
			return false;
		}
	}
	int _len = _to_send.length();

	this->__channel << (unsigned char) 0x81;
	if (_len > 65535) {
		this->__channel << (unsigned char) 0x7F;
		this->__channel << (unsigned char) 0x00;
		this->__channel << (unsigned char) 0x00;
		this->__channel << (unsigned char) 0x00;
		this->__channel << (unsigned char) 0x00;
		this->__channel << ((unsigned char) ((_len >> 24) & 0xFF));
		this->__channel << ((unsigned char) ((_len >> 16) & 0xFF));
		this->__channel << ((unsigned char) ((_len >> 8) & 0xFF));
		this->__channel << ((unsigned char) (_len & 0xFF));
	}
	else if (_len > 125) {
		this->__channel << (unsigned char) 0xFE;
		this->__channel << ((unsigned char) (_len >> 8));
		this->__channel << ((unsigned char) (_len & 0xFF));
	}
	else {
		this->__channel << (unsigned char) (0x80 | ((unsigned char) _len));
	}
	for (int _i = 0; _i != 4; _i++) {
		this->__channel << (unsigned char) 0x00;
	}

	this->__channel << _to_send << flush;

	return true;
}

bool muzzley::Client::reply(muzzley::Message& _data_received, muzzley::Message& _reply) {
	assertz(!!_reply["s"], "attribute 's' is mandatory, in the '_data_received' argument", 500, 108);
	assertz(!!_data_received["h"]["t"], "there is no message type in the '_data_received' argument", 500, 107);
	assertz(!!_data_received["h"]["cid"], "there is no message correlation id ('cid') in the '_data_received' argument", 500, 107);
	muzzley::MessageType _type = (muzzley::MessageType) ((int) _data_received["h"]["t"]);

	switch (_type) {
		case muzzley::RequestInitiatedByEndpoint: {
			if (_reply["h"]->ok()) {
				((muzzley::JSONObj) _reply["h"]) <<
				"t" << (int) muzzley::ReplyToEndpoint <<
				"cid" << (string) _data_received["h"]["cid"];
			}
			else {
				_reply <<
				"h" << JSON(
					"t" << (int) muzzley::ReplyToEndpoint <<
					"cid" << (string) _data_received["h"]["cid"]
				);
			}
			break;
		}
		case muzzley::RequestInitiatedMuzzleyCore: {
			if (_reply["h"]->ok()) {
				((muzzley::JSONObj) _reply["h"]) <<
				"t" << (int) muzzley::ReplyToEndpoint <<
				"cid" << (string) _data_received["h"]["cid"];
			}
			else {
				_reply <<
				"h" << JSON(
					"t" << (int) muzzley::ReplyToMuzzleyCore <<
					"cid" << (string) _data_received["h"]["cid"]
				);
			}
			break;
		}
		default:
			return false;
	}
	if (_data_received["a"]->ok() && ((string) _data_received["a"]) == "publish") {
		muzzley::JSONObj _d = _reply.getData();
		muzzley::JSONObj _data;
		for (auto _f : _d) {
			_data << _f.first << _f.second;
		}
		_d << 
			"ns" << _data_received["d"]["ns"] <<
			"p" << JSON(
				"io" << "i" <<
				"profile" << _data_received["d"]["p"]["profile"] <<
				"channel" << _data_received["d"]["p"]["channel"] <<
				"component" << _data_received["d"]["p"]["component"] <<
				"property" << _data_received["d"]["p"]["property"] <<
				"data" << _data
			);
	}
	return this->write(_reply);
}

void muzzley::Client::connectApp(string _app_token, string _activity_id) {
	this->__app_token.assign(_app_token.data());
	this->__activity_id.assign(_activity_id.data());
	this->__is_static_activity = _activity_id.length() != 0;
	if (this->connect(this->__endpoint_host, MUZZLEY_ENDPOINT_PORT, MUZZLEY_ENDPOINT_PATH)) {
		if (this->__is_loop_assynchronous) {
			this->start();
			this->handshake([ _app_token ] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
				_client.__has_handshake = true;
				_client.trigger(muzzley::Connect, _data);
				_client.loginApp(_app_token);
				return true;
			});
		}
		else {
			this->handshake([ _app_token ] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
				_client.__has_handshake = true;
				_client.trigger(muzzley::Connect, _data);
				_client.loginApp(_app_token);
				return true;
			});
			this->run();
		}
	}
}

void muzzley::Client::connectUser(string _user_token, string _activity_id) {
	assertz(_activity_id.length() != 0, "when connecting a user, the '_activity_id' parameter must be instantiated with a non-empty string", 500, 100);
	this->__user_token.assign(_user_token.data());
	this->__activity_id.assign(_activity_id.data());
	this->__is_static_activity = _activity_id.length() != 0;
	if (this->connect(this->__endpoint_host, MUZZLEY_ENDPOINT_PORT, MUZZLEY_ENDPOINT_PATH)) {
		if (this->__is_loop_assynchronous) {
			this->start();
			this->handshake([ _user_token ] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
				_client.__has_handshake = true;
				_client.trigger(muzzley::Connect, _data);
				_client.loginUser(_user_token);
				return true;
			});
		}
		else {
			this->handshake([ _user_token ] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
				_client.__has_handshake = true;
				_client.trigger(muzzley::Connect, _data);
				_client.loginUser(_user_token);
				return true;
			});
			this->run();
		}
	}
}

void muzzley::Client::loginApp(string _app_token) {
	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "loginApp" <<
		"d" << JSON(
			"token" << _app_token
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("login app -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message);
}

void muzzley::Client::loginUser(string _user_token) {
	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "loginUser" <<
		"d" << JSON(
			"token" << _user_token
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("login user -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message);
}

void muzzley::Client::initApp(string _app_token) {
	this->__app_token.assign(_app_token.data());
	this->__is_static_activity = false;
	this->__is_one_step_initialization = true;
	if (this->connect(this->__endpoint_host, MUZZLEY_ENDPOINT_PORT, MUZZLEY_ENDPOINT_PATH)) {
		muzzley::Callback _callback = [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
			if ((bool) _data["s"] == false) {
				return false;
			}
			_client.__has_handshake = true;
			if (!!_data["d"]["handshake"]["deviceId"]) {
				_client.__device_id.assign((string) _data["d"]["handshake"]["deviceId"]);
				muzzley::Message _d(JSON( "d" << (muzzley::JSONObj&) _data["d"]["handshake"]));
				_client.trigger(muzzley::Connect, _d);
			}
			if (!!_data["d"]["loginApp"]["sessionId"]) {
				_client.__session_id.assign((string) _data["d"]["loginApp"]["sessionId"]);
				_client.__is_app_loggedin = true;
				_client.__is_user_loggedin = false;
				muzzley::Message _d(JSON( "d" << (muzzley::JSONObj&) _data["d"]["loginApp"]));
				_client.trigger(muzzley::AppLoggedIn, _d);
			}
			if (!!_data["d"]["create"]["channel"]["id"]) {
				_client.__channel_id.assign((string) _data["d"]["create"]["channel"]["id"]);
			}
			if (!!_data["d"]["create"]["activity"]["activityId"]) {
				_client.__activity_id.assign((string) _data["d"]["create"]["activity"]["activityId"]);
				_client.__is_initiating_master = true;
				muzzley::Message _d(JSON( "d" << (muzzley::JSONObj&) _data["d"]["create"]["activity"]));
				_client.trigger(muzzley::ActivityCreated, _d);
			}
			return true;
		};

		muzzley::Message _message(JSON(
			"h" << JSON(
				"cid" << this->__serial <<
				"t" << 1
				) <<
			"a" << "init" <<
			"d" << JSON(
				"handshake"<< JSON(
					"protocolVersion" << "2.0" <<
					"lib" << "C++ 0.1.0" <<
					"userAgent" << "C++ Lib"
//<< "deviceId" << "68748e45-36ef-422b-bcc2-d6edcea7ce75"
					) <<
				"loginApp" << JSON(
					"token" << _app_token
					)
				)
			));

#ifdef MUZZLEY_DEBUG
		{
			string _log("login app -> ");
			muzzley::tostr(_log, _message);
			muzzley::log(_log, muzzley::notice);
		}
#endif

		if (this->__is_loop_assynchronous) {
			if (!this->__is_running) {
				this->__is_running = true;
				this->start();
			}
			this->write(_message, _callback);
		}
		else {
			this->write(_message, _callback);
			if (!this->__is_running) {
				this->__is_running = true;
				this->run();
			}
		}
	}
}

void muzzley::Client::initUser(string _user_token) {
	this->__user_token.assign(_user_token.data());
	this->__is_static_activity = false;
	this->__is_one_step_initialization = true;
	if (this->connect(this->__endpoint_host, MUZZLEY_ENDPOINT_PORT, MUZZLEY_ENDPOINT_PATH)) {
		muzzley::Callback _callback = [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
			if ((bool) _data["s"] == false) {
				return false;
			}
			_client.__has_handshake = true;
			if (!!_data["d"]["handshake"]["deviceId"]) {
				_client.__device_id.assign((string) _data["d"]["handshake"]["deviceId"]);
				muzzley::Message _d(JSON( "d" << (muzzley::JSONObj&) _data["d"]["handshake"]));
				_client.trigger(muzzley::Connect, _d);
			}
			if (!!_data["d"]["loginUser"]["sessionId"]) {
				_client.__session_id.assign((string) _data["d"]["loginUser"]["sessionId"]);
				_client.__is_app_loggedin = false;
				_client.__is_user_loggedin = true;
				muzzley::Message _d(JSON( "d" << (muzzley::JSONObj&) _data["d"]["loginUser"]));
				_client.trigger(muzzley::UserLoggedIn, _d);
			}
			if (!!_data["d"]["join"]["channel"]["id"]) {
				_client.__channel_id.assign((string) _data["d"]["join"]["channel"]["id"]);
			}
			if (!!_data["d"]["join"]["participant"]["id"]) {
				_client.__participant_id = (long long) _data["d"]["join"]["participant"]["id"];
				_client.participantReady();
				muzzley::Message _d(JSON( "d" << (muzzley::JSONObj&) _data["d"]["join"]["participant"]));
				_client.trigger(muzzley::ActivityJoined, _d);
			}
			return true;
		};

		muzzley::Message _message(JSON(
			"h" << JSON(
				"cid" << this->__serial <<
				"t" << 1
				) <<
			"a" << "init" <<
			"d" << JSON(
				"handshake"<< JSON(
					"protocolVersion" << "2.0" <<
					"lib" << "C++ 0.1.0" <<
					"userAgent" << "C++ Lib"
//<< "deviceId" << "68748e45-36ef-422b-bcc2-d6edcea7ce75"
					) <<
				"loginUser" << JSON(
					"token" << _user_token
					)
				)
			));

#ifdef MUZZLEY_DEBUG
		{
			string _log("login user -> ");
			muzzley::tostr(_log, _message);
			muzzley::log(_log, muzzley::notice);
		}
#endif

		if (this->__is_loop_assynchronous) {
			if (!this->__is_running) {
				this->__is_running = true;
				this->start();
			}
			this->write(_message, _callback);
		}
		else {
			this->write(_message, _callback);
			if (!this->__is_running) {
				this->__is_running = true;
				this->run();
			}
		}
	}	
}

void muzzley::Client::createActivity(string _activity_id) {
	assertz(!this->__is_one_step_initialization, "can not create an activity after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	assertz(this->__is_app_loggedin, "activity can only be created by Activity Master, you must be logged in as as app (use connectApp method).", 500, 101);

	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "create"
		));

	if (_activity_id.length() != 0) {
		this->__activity_id.assign(_activity_id.data());
	}
	else {
		_activity_id.assign(this->__activity_id.data());
	}

	if (_activity_id.length() != 0) {
		_message <<
		"d" << JSON(
			"activityId" << _activity_id
			);
	}

#ifdef MUZZLEY_DEBUG
	{
		string _log("creating activity -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message);
}

void muzzley::Client::joinActivity(string _activity_id) {
	assertz(!this->__is_one_step_initialization, "can not join an activity after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	assertz(_activity_id.length() != 0 || (_activity_id.length() == 0 && this->__activity_id.length() != 0), "an activityId must be provided, either in joinActivity, connectApp or connectUser method invocations", 500, 103);
	assertz(this->__is_user_loggedin, "joinActivity can only be invoked by a user", 500, 106);

	if (_activity_id.length() != 0) {
		this->__activity_id.assign(_activity_id.data());
	}
	else {
		_activity_id.assign(this->__activity_id.data());
	}

	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "join" <<
		"d" << JSON(
			"activityId" << _activity_id
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("joining activity ");
		_log.insert(_log.length(), _activity_id);
		_log.insert(_log.length(), " -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message);
}



void muzzley::Client::quit() {
	assertz(!this->__is_one_step_initialization, "can not quit an activity after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	assertz(this->__is_initiating_master, "quit can only be invoked by the initiating Activity Master, you must create the activity before quiting (use createActivity method) or use participantQuit method instead.", 500, 102);

	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "quit"
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("quiting -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message);
}

void muzzley::Client::participantQuit() {
	assertz(!this->__is_one_step_initialization, "can not quit an activity after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	assertz(this->__participant_id != -1, "in order to quit you must have joined first (use joinActivity method)", 500, 105);

	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "participantQuit" <<
		"d" << JSON(
			"participantId" << this->__participant_id
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("participant quiting -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message);
}

void muzzley::Client::participantReady(muzzley::Callback _callback) {
	assertz(!this->__is_one_step_initialization, "can not ready a participant after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	assertz(this->__participant_id != -1, "in order to be ready you must have joined first (use joinActivity method)", 500, 106);

	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << "ready"
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("participant ready -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::changeWidget(long long _participant_id, string _widget, muzzley::Callback _callback) {
	assertz(!this->__is_one_step_initialization, "can not change widget after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	muzzley::JSONObj _options;
	_options <<
	"widget" << _widget;
	this->changeWidget(_participant_id, _options, _callback);
}

void muzzley::Client::changeWidget(long long _participant_id, string _widget, muzzley::JSONObj& _params, muzzley::Callback _callback) {
	assertz(!this->__is_one_step_initialization, "can not change widget after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	muzzley::JSONObj _options;
	_options <<
	"widget" << _widget <<
	"params" << _params;
	this->changeWidget(_participant_id, _options, _callback);
}

void muzzley::Client::changeWidget(long long _participant_id, muzzley::JSONObj& _options, muzzley::Callback _callback) {
	assertz(!this->__is_one_step_initialization, "can not change widget after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	string _widget;
	pthread_mutex_lock(this->__mtx);
	bool _found = this->__participants.find(_participant_id) != this->__participants.end();
	pthread_mutex_unlock(this->__mtx);

	assertz(_found, "the provided participantId has not yet joined or already quit", 500, 104);

	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1 <<
			"pid" << _participant_id
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << "changeWidget" <<
			"d" << _options
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("change widget, participant");
		muzzley::tostr(_log, _participant_id);
		_log.insert(_log.length(), " -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::setupComponent(long long _participant_id, string _component, string _component_id, string _action, muzzley::Callback _callback) {
	assertz(!this->__is_one_step_initialization, "can not setup component after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	pthread_mutex_lock(this->__mtx);
	bool _found = this->__participants.find(_participant_id) != this->__participants.end();
	pthread_mutex_unlock(this->__mtx);

	assertz(_found, "the provided participantId has not yet joined or already quit", 500, 104);

	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1 <<
			"pid" << _participant_id
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << "setupComponent" <<
			"d" << JSON(
				"c" << _component <<
				"id" << _component_id <<
				"a" << _action
				)
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("setup component, participant");
		muzzley::tostr(_log, _participant_id);
		_log.insert(_log.length(), " -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::setupComponent(long long _participant_id, string _component, string _component_id, string _action, muzzley::JSONObj& _options, muzzley::Callback _callback) {
	assertz(!this->__is_one_step_initialization, "can not setup component after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	pthread_mutex_lock(this->__mtx);
	bool _found = this->__participants.find(_participant_id) != this->__participants.end();
	pthread_mutex_unlock(this->__mtx);

	assertz(_found, "the provided participantId has not yet joined or already quit", 500, 104);

	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1 <<
			"pid" << _participant_id
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << "setupComponent" <<
			"d" << JSON(
				"c" << _component <<
				"id" << _component_id <<
				"a" << _action <<
				"p" << _options
				)
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("setup component, participant");
		muzzley::tostr(_log, _participant_id);
		_log.insert(_log.length(), " -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::sendSignal(long long _participant_id, string _type, muzzley::Callback _callback) {
	assertz(!this->__is_one_step_initialization, "can not send signal after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	pthread_mutex_lock(this->__mtx);
	bool _found = this->__participants.find(_participant_id) != this->__participants.end();
	pthread_mutex_unlock(this->__mtx);

	assertz(_found, "the provided participantId has not yet joined or already quit", 500, 104);

	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1 <<
			"pid" << _participant_id
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << _type
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending signal to participant ");
		muzzley::tostr(_log, _participant_id);
		_log.insert(_log.length(), " -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::sendSignal(long long _participant_id, string _type, muzzley::JSONObj& _data, muzzley::Callback _callback) {
	assertz(!this->__is_one_step_initialization, "can not send signal after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	pthread_mutex_lock(this->__mtx);
	bool _found = this->__participants.find(_participant_id) != this->__participants.end();
	pthread_mutex_unlock(this->__mtx);

	assertz(_found, "the provided participantId has not yet joined or already quit", 500, 104);

	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1 <<
			"pid" << _participant_id
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << _type <<
			"d" << _data
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending signal to participant ");
		muzzley::tostr(_log, _participant_id);
		_log.insert(_log.length(), " -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::sendSignal(string _type, muzzley::Callback _callback) {
	assertz(!this->__is_one_step_initialization, "can not send signal after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << _type
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending signal -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::sendSignal(string _type, muzzley::JSONObj& _data, muzzley::Callback _callback) {
	assertz(!this->__is_one_step_initialization, "can not send signal after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"a" << _type <<
			"d" << _data
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending signal -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message, _callback);
}

void muzzley::Client::sendWidgetData(string _widget, string _component, string _event_type, string _event_value) {
	assertz(!this->__is_one_step_initialization, "can not send signal after invoking 'initUser/initApp' since it initializes protocol version 2.0 (pub/sub communication pattern).", 500, 101);
	muzzley::Message _message(JSON(
		"h" << JSON(
			"t" << 5
			) <<
		"a" << "signal" <<
		"d" << JSON(
			"w" << _widget <<
			"c" << _component <<
			"e" << _event_type <<
			"v" << _event_value
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending widget data -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	this->write(_message);
}


void muzzley::Client::subscribe(muzzley::Subscription& _to_property, muzzley::Callback _callback) {
	assertz(this->__is_one_step_initialization, "can not subscribe after invoking 'connectUser/connectApp' since it initializes protocol version 1.2 (signaling communication pattern).", 500, 101);
	assertz(_to_property["namespace"]->ok() || _to_property["profile"]->ok() || _to_property["channel"]->ok() || _to_property["component"]->ok() || _to_property["component"]->ok() || _to_property["property"]->ok(), "you must subscribe to something, subscription object must not be empty.", 500, 200);

	string _ns_channel("mc:");
	muzzley::JSONObj _data;
	muzzley::JSONObj _payload;

	if (_to_property["namespace"]->ok()) {
		_data << "ns" << (string) _to_property["namespace"];
		_ns_channel.insert(_ns_channel.length(), (string) _to_property["namespace"]);
	}
	else {
		_ns_channel.insert(_ns_channel.length(), "*");
	}
	_ns_channel.insert(_ns_channel.length(), ":ps:");
	if (_to_property["profile"]->ok()) {
		_payload << "profile" << (string) _to_property["profile"];
		_ns_channel.insert(_ns_channel.length(), (string) _to_property["profile"]);
	}
	else {
		_ns_channel.insert(_ns_channel.length(), "*");
	}
	_ns_channel.insert(_ns_channel.length(), ":");
	if (_to_property["channel"]->ok()) {
		_payload << "channel" << (string) _to_property["channel"];
		_ns_channel.insert(_ns_channel.length(), (string) _to_property["channel"]);
	}
	else {
		_ns_channel.insert(_ns_channel.length(), "*");
	}
	_ns_channel.insert(_ns_channel.length(), ":");
	if (_to_property["component"]->ok()) {
		_payload << "component" << (string) _to_property["component"];
		_ns_channel.insert(_ns_channel.length(), (string) _to_property["component"]);
	}
	else {
		_ns_channel.insert(_ns_channel.length(), "*");
	}
	_ns_channel.insert(_ns_channel.length(), ":");
	if (_to_property["property"]->ok()) {
		_payload << "property" << (string) _to_property["property"];
		_ns_channel.insert(_ns_channel.length(), (string) _to_property["property"]);
	}
	else {
		_ns_channel.insert(_ns_channel.length(), "*");
	}
	_ns_channel.insert(_ns_channel.length(), ":");

	_data << "p" << _payload;

	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "subscribe" <<
		"d" << _data
	));

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending widget data -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif
	this->write(_message,  [ _ns_channel, _callback ] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		if (!!_data["s"]) {
			_client.__stack.insert(pair<string, muzzley::Callback>((string) _data["d"]["channel"]["id"], _callback));
			auto _found = _client.__namespaces.find(_ns_channel);
			if (_found == _client.__namespaces.end()) {
				_client.__namespaces.insert(pair< string, vector< string > >(_ns_channel, vector<string>()));
				_found = _client.__namespaces.find(_ns_channel);
			}
			_found->second.push_back((string) _data["d"]["channel"]["id"]);
		}
		return true;
	});
}

void muzzley::Client::unsubscribe(muzzley::Subscription& _to_property) {
	assertz(this->__is_one_step_initialization, "can not publish after invoking 'connectUser/connectApp' since it initializes protocol version 1.2 (signaling communication pattern).", 500, 101);
	assertz(_to_property["namespace"]->ok() || _to_property["profile"]->ok() || _to_property["channel"]->ok() || _to_property["component"]->ok() || _to_property["component"]->ok() || _to_property["property"]->ok(), "you must subscribe to something, subscription object must not be empty.", 500, 200);

	string _ns_channel("mc:");
	muzzley::JSONObj _data;
	muzzley::JSONObj _payload;

	if (_to_property["namespace"]->ok()) {
		_data << "ns" << (string) _to_property["namespace"];
		_ns_channel.insert(_ns_channel.length(), (string) _to_property["namespace"]);
	}
	else {
		_ns_channel.insert(_ns_channel.length(), "*");
	}
	_ns_channel.insert(_ns_channel.length(), ":ps:");
	if (_to_property["profile"]->ok()) {
		_payload << "profile" << (string) _to_property["profile"];
		_ns_channel.insert(_ns_channel.length(), (string) _to_property["profile"]);
	}
	else {
		_ns_channel.insert(_ns_channel.length(), "*");
	}
	_ns_channel.insert(_ns_channel.length(), ":");
	if (_to_property["channel"]->ok()) {
		_payload << "channel" << (string) _to_property["channel"];
		_ns_channel.insert(_ns_channel.length(), (string) _to_property["channel"]);
	}
	else {
		_ns_channel.insert(_ns_channel.length(), "*");
	}
	_ns_channel.insert(_ns_channel.length(), ":");
	if (_to_property["component"]->ok()) {
		_payload << "component" << (string) _to_property["component"];
		_ns_channel.insert(_ns_channel.length(), (string) _to_property["component"]);
	}
	else {
		_ns_channel.insert(_ns_channel.length(), "*");
	}
	_ns_channel.insert(_ns_channel.length(), ":");
	if (_to_property["property"]->ok()) {
		_payload << "property" << (string) _to_property["property"];
		_ns_channel.insert(_ns_channel.length(), (string) _to_property["property"]);
	}
	else {
		_ns_channel.insert(_ns_channel.length(), "*");
	}
	_ns_channel.insert(_ns_channel.length(), ":");

	_data << "p" << _payload;

	auto _found = this->__namespaces.find(_ns_channel);
	assertz(_found != this->__namespaces.end(), "this property has not been subscribed", 500, 205);

	for (auto _ns : _found->second) {
		muzzley::Message _message(JSON(
			"h" << JSON(
				"cid" << this->__serial <<
				"t" << 1 <<
				"ch" << _ns
			) <<
			"a" << "unsubscribe" <<
			"d" << _data
		));

#ifdef MUZZLEY_DEBUG
		{
			string _log("sending widget data -> ");
			muzzley::tostr(_log, _message);
			muzzley::log(_log, muzzley::notice);
		}
#endif
		this->write(_message,  [ _ns ] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
			if (!!_data["s"]) {
				auto _ch = _client.__stack.find(_ns);
				if (_ch != _client.__stack.end()) {
					_client.__stack.erase(_ch);
				}
			}
			return true;
		});
	}
	this->__namespaces.erase(_found);
}

void muzzley::Client::publish(muzzley::Subscription& _to_property, muzzley::Message& _payload, muzzley::Callback _callback) {
	assertz(this->__is_one_step_initialization, "can not publish after invoking 'connectUser/connectApp' since it initializes protocol version 1.2 (signaling communication pattern).", 500, 101);
	assertz(!!_to_property["namespace"], "field 'namespace' must be included in '_to_property' parameter (e.g., _to_proprety << \"namespace\" << \"iot\").", 500, 200);
	assertz(!!_to_property["profile"], "field 'profile' must be included in '_to_property' parameter (e.g., _to_proprety << \"profile\" << \"internal-profile-id\").", 500, 201);
	assertz(!!_to_property["channel"], "field 'channel' must be included in '_to_property' parameter (e.g., _to_proprety << \"channel\" << \"remote-channel-id\").", 500, 202);
	assertz(!!_to_property["component"], "field 'component' must be included in '_to_property' parameter (e.g., _to_proprety << \"component\" << \"component-1\").", 500, 203);
	assertz(!!_to_property["property"], "field 'property' must be included in '_to_property' parameter (e.g., _to_proprety << \"property\" << \"prop-1\").", 500, 204);

	assertz(!!_payload["d"]["io"], "field 'io' must be included in '_payload' parameter (e.g., _payload.setData(JSON(\"io\" << \"w\")) ).", 500, 206);

	muzzley::Message _message(JSON(
		"h" << JSON(
			"t" << 5
		) <<
		"a" << "publish" <<
		"d" << JSON(
			"ns" << (string) _to_property["namespace"] <<
			"p" << JSON(
				"io" << (string) _payload["d"]["io"] <<
				"profile" << (string) _to_property["profile"] <<
				"channel" << (string) _to_property["channel"] <<
				"component" << (string) _to_property["component"] <<
				"property" << (string) _to_property["property"]
			)
		)
	));
	if (!!_payload["h"]["u"]) {
		_message["h"] << "u" << _payload["h"]["u"];
	}
	if (!!_payload["d"]["data"]) {
		_message["d"]["p"] << "data" << _payload["d"]["data"];
	}

	if (_callback != nullptr) {
		_message["h"] << "cid" << this->__serial << "t" << 1;
	}

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending widget data -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif
	this->write(_message, _callback);
}

void muzzley::Client::setLoopAssynchronous(bool _assync) {
	this->__is_loop_assynchronous = _assync;
}

const string& muzzley::Client::getActivityId() const {
	return __activity_id;
}

const string& muzzley::Client::getDeviceId() const {
	return __device_id;
}

const string& muzzley::Client::getSessionId() const {
	return __session_id;
}

const string& muzzley::Client::getChannelId() const {
	return __channel_id;
}

const long long muzzley::Client::getParticipantId() const {
	return __participant_id;
}

bool muzzley::Client::isReplyNeeded(muzzley::Message& _data_received) const {
	muzzley::MessageType _type = (muzzley::MessageType) ((int) _data_received["h"]["t"]);
	return _type == muzzley::RequestInitiatedByEndpoint || _type == muzzley::RequestInitiatedMuzzleyCore;
}

bool muzzley::Client::isAppLoggedin() const {
	return __is_app_loggedin;
}

bool muzzley::Client::isInitiatingMaster() const {
	return __is_initiating_master;
}

bool muzzley::Client::isConnected() {
	return __channel.is_open();
}

bool muzzley::Client::isLoopAssynchronous() const {
	return __is_loop_assynchronous;
}

bool muzzley::Client::isStaticActivity() const {
	return __is_static_activity;
}

bool muzzley::Client::isUserLoggedin() const {
	return __is_user_loggedin;
}

const muzzley::ParticipantList& muzzley::Client::getParticipants() const {
	return __participants;
}

const string& muzzley::Client::getEndpointHost() const {
	return __endpoint_host;
}
void muzzley::Client::setEndpointHost(string _endpoint_host) {
	this->__endpoint_host.assign(_endpoint_host);
}

bool muzzley::Client::handshake(muzzley::Handler _callback) {
	muzzley::Message _message(JSON(
		"h" << JSON(
			"cid" << this->__serial <<
			"t" << 1
			) <<
		"a" << "handshake" <<
		"d" << JSON(
			"protocolVersion" << "2.0" <<
			"lib" << "C++ 0.1.0"
			)
		));

#ifdef MUZZLEY_DEBUG
	{
		string _log("sending handshake -> ");
		muzzley::tostr(_log, _message);
		muzzley::log(_log, muzzley::notice);
	}
#endif

	return this->write(_message, _callback);
}

bool muzzley::Client::process(muzzley::Message& _received, muzzley::EventType* _type) {
	int _interaction = (int) _received["h"]["t"];
	string _action;
	string _signal_action;
	switch (_interaction) {
// Requests originated at the server or client
		case muzzley::RequestInitiatedByEndpoint:
// Fall through
		case muzzley::RequestInitiatedMuzzleyCore:
// Fall through
		case muzzley::Signaling:
		_action.assign((string) _received["a"]);
		if (_action == "signal" && !!_received["d"] && !!_received["d"]["a"]) {
			_signal_action.assign((string) _received["d"]["a"]);
		}
		break;
// Replies to requests initiated here
		case muzzley::ReplyToEndpoint:
// Fall through
		case muzzley::ReplyToMuzzleyCore:
		pthread_mutex_lock(this->__mtx);
		CallbackQueue::iterator _req_data = this->__queue.find((int) _received["h"]["cid"]);
		pthread_mutex_unlock(this->__mtx);

		if (_req_data != this->__queue.end()) {
			_action.assign(_req_data->second.first);
			if (_req_data->second.second != nullptr) {
				(_req_data->second.second)(_received, *this);
			}

			pthread_mutex_lock(this->__mtx);
			this->__queue.erase(_req_data);
			pthread_mutex_unlock(this->__mtx);
		}
	}

	if (_action == "handshake") {
		*_type = muzzley::Handshake;
	}
	else if (_action == "loginApp") {
		*_type = muzzley::AppLoggedIn;
	}
	else if (_action == "loginUser") {
		*_type = muzzley::UserLoggedIn;
	}
	else if (_action == "create") {
		*_type = muzzley::ActivityCreated;
	}
	else if (_action == "join") {
		*_type = muzzley::ActivityJoined;
	}
	else if (_action == "activityTerminated") {
		*_type = muzzley::ActivityTerminated;
	}
	else if (_action == "participantJoined") {
		*_type = muzzley::__INTERNAL_ParticipantJoined__;
	}
	else if (_action == "participantQuit") {
		*_type = muzzley::ParticipantQuit;
	}
	else if (_action == "publish") {
		*_type = muzzley::__INTERNAL_PublishUpdate__;
	}
	else if (_action == "signal") {
		if (_signal_action == "ready") {
			*_type = muzzley::ParticipantJoined;
		}
		else if (_signal_action == "changeWidget") {
			*_type = muzzley::ChangeWidget;
		}
		else if (_signal_action == "setupComponent") {
			*_type = muzzley::SetupComponent;
		}
		else if (_signal_action == "" && !!_received["d"]["w"] && !!_received["d"]["c"]) {
// {"c":"ba","e":"release","v":"a","w":"gamepad"}
			*_type = muzzley::WidgetAction;
		}
		else {
			if (_interaction == muzzley::ReplyToEndpoint) {
// Don't trigger a Signaling Message event
// if this is a reply to one we sent before.
				return false;
			}
			*_type = muzzley::SignalingMessage;
		}
	}

	return true;
}

void muzzley::Client::run() {
	for (; true;) {
		try {
			if (!this->isConnected()) {
				if (!this->reconnect()) {
					return;
				}
			}

			if (this->read()) {
				if (this->__message == "h") {
					this->disarm();
					this->write(this->__message);
					this->__message.clear();
					this->arm();
					continue;
				}
#ifdef MUZZLEY_DEBUG
				{
					string _log("received a message\n<- ");
					_log.insert(_log.length(), this->__message);
					muzzley::log(_log, muzzley::info);
				}
#endif
				switch (this->__op_code) {
					case 0x0: {
						break;
					}
					case 0x01: {
						muzzley::Message _received((muzzley::JSONObj&) muzzley::fromstr(this->__message));
						this->__message.clear();
						this->__op_code = -1;
						muzzley::EventType _type;
						if (this->process(_received, &_type)) {
							this->trigger(_type, _received);
						}
						break;
					}
					case 0x02: {
						break;
					}
					case 0x08: {
						this->disconnect();
						break;
					}
					case 0x09: {
						break;
					}
				}

			}
		}
		catch (muzzley::AssertionException& e) {
#ifdef MUZZLEY_DEBUG
			{
				string _log("assertion error: ");
				_log.insert(_log.length(), e.description());
				muzzley::log(_log, muzzley::error);
			}
#endif
			throw;
		}
	}
}

void muzzley::printbits(unsigned char _b) {
	bitset<8> _bs(_b);
	cout << _bs.to_string() << endl << flush;
}

void muzzley::Client::start() {
	if (pthread_equal(pthread_self(), *this->__thr)) {
// Thread already started
		return;
	}
	pthread_create(this->__thr, 0, &Client::launch, this);
}

void* muzzley::Client::launch(void* _thread) {
	Client* _running = static_cast<Client*>(_thread);
	_running->run();
	return nullptr;
}

muzzley::Message::Message() : muzzley::JSONObj() {
}

muzzley::Message::Message(muzzley::JSONObj& _rhs) : muzzley::JSONObj(_rhs) {
}

muzzley::Message::~Message(){
}

muzzley::MessageType muzzley::Message::getMessageType(){
	return (muzzley::MessageType) ((int) (* this)["h"]["t"]);
}

string muzzley::Message::getCorrelationID(){
	return (string) (* this)["h"]["cid"];
}

string muzzley::Message::getParticipantID(){
	return (string) (* this)["h"]["pid"];
}

string muzzley::Message::getChannelID(){
	return (string) (* this)["h"]["ch"];
}

string muzzley::Message::getAction(){
	return (string) (* this)["a"];
}

muzzley::JSONObj& muzzley::Message::getData(){
	return (muzzley::JSONObj&) (* this)["d"];
}

bool muzzley::Message::getStatus(){
	return (bool) (* this)["s"];
}

string muzzley::Message::getStatusMessage(){
	return (string) (* this)["m"];
}

void muzzley::Message::getSubscriptionInfo(muzzley::Subscription& _out){
	if (((string) (* this)["a"]) == "publish") {
		_out.setNamespace((string) (* this)["d"]["ns"]);
		_out.setProfile((string) (* this)["d"]["p"]["profile"]);
		_out.setChannel((string) (* this)["d"]["p"]["channel"]);
		_out.setComponent((string) (* this)["d"]["p"]["component"]);
		_out.setProperty((string) (* this)["d"]["p"]["property"]);
	}
}

void muzzley::Message::setMessageType(muzzley::MessageType _in){
	if (!(* this)["h"]) {
		(* this) << "h" << JSON( "t" << (int) _in);
	}
	else {
		(* this)["h"] <<  "t" << (int) _in;
	}
}

void muzzley::Message::setCorrelationID(string _in){
	if (!(* this)["h"]) {
		(* this) << "h" << JSON( "cid" << _in);
	}
	else {
		(* this)["h"] <<  "cid" << _in;
	}
}

void muzzley::Message::setParticipantID(string _in){
	if (!(* this)["h"]) {
		(* this) << "h" << JSON( "pid" << _in);
	}
	else {
		(* this)["h"] <<  "pid" << _in;
	}
}

void muzzley::Message::setChannelID(string _in){
	if (!(* this)["h"]) {
		(* this) << "h" << JSON( "ch" << _in);
	}
	else {
		(* this)["h"] <<  "ch" << _in;
	}
}

void muzzley::Message::setAction(string _in){
	(* this) <<  "a" << _in;
}

void muzzley::Message::setData(muzzley::JSONObj& _in){
	(* this) <<  "d" << _in;
}

void muzzley::Message::setUser(string _in){
	if (!(* this)["h"]) {
		(* this) << "h" << JSON( "u" << _in);
	}
	else {
		(* this)["h"] <<  "u" << _in;
	}
}

void muzzley::Message::setStatus(bool _in){
	(* this) <<  "s" << (bool) _in;
}

void muzzley::Message::setStatusMessage(string _in){
	(* this) <<  "m" << _in;
}

bool muzzley::Message::isError(){
	auto _found = (* this)->find("s");
	if (_found == (* this)->end()) {
		return false;
	}
	return !(bool) (* this)["s"];
}

bool muzzley::Message::isRequest(){
	return (this->getMessageType() == RequestInitiatedByEndpoint) || (this->getMessageType() == RequestInitiatedMuzzleyCore) || (this->getMessageType() == Signaling);
}

bool muzzley::Message::isReponse(){
	return (this->getMessageType() == ReplyToEndpoint) || (this->getMessageType() == ReplyToMuzzleyCore);
}

bool muzzley::Message::isReplyNeeded(){
	return (this->getMessageType() == RequestInitiatedByEndpoint) || (this->getMessageType() == RequestInitiatedMuzzleyCore);
}

muzzley::Subscription::Subscription() : muzzley::JSONObj() {
}

muzzley::Subscription::Subscription(muzzley::JSONObj& _rhs) : muzzley::JSONObj(_rhs) {
}

muzzley::Subscription::~Subscription() {
}

string muzzley::Subscription::getNamespace() {
	return (string) (* this)["namespace"];
}

string muzzley::Subscription::getProfile() {
	return (string) (* this)["profile"];
}

string muzzley::Subscription::getChannel() {
	return (string) (* this)["channel"];
}

string muzzley::Subscription::getComponent() {
	return (string) (* this)["component"];
}

string muzzley::Subscription::getProperty() {
	return (string) (* this)["property"];
}

void muzzley::Subscription::setNamespace(string _in) {
	(* this) << "namespace" << _in;
}

void muzzley::Subscription::setProfile(string _in) {
	(* this) << "profile" << _in;
}

void muzzley::Subscription::setChannel(string _in) {
	(* this) << "channel" << _in;
}

void muzzley::Subscription::setComponent(string _in) {
	(* this) << "component" << _in;
}

void muzzley::Subscription::setProperty(string _in) {
	(* this) << "property" << _in;
}

