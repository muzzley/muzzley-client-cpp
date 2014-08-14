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
#include <pthread.h>
#include <stream/SocketStreams.h>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace __gnu_cxx;

#define MUZZLEY_ENDPOINT_ADDRESS "geoplatform.muzzley.com"
#define MUZZLEY_ENDPOINT_PORT 80
#define MUZZLEY_ENDPOINT_PATH "/ws"

#ifndef CRLF
#define CRLF "\r\n"
#endif

namespace muzzley {
	
#define N_EVENT_TYPES 11
	enum EventType {
		Connect = 0,
		Handshake = 1,
		ActivityCreated = 2,
		ActivityJoined = 3,
		ActivityTerminated = 4,
		ParticipantJoined = 5,
		ParticipantQuit = 6,
		SignalingMessage = 7,
		AppLoggedIn = 8,
		UserLoggedIn = 9,
		__INTERNAL_ParticipantJoined__ = 10
	};
	enum MessageType {
		RequestInitiatedByEndpoint = 1,
		ReplyToEndpoint = 2,
		RequestInitiatedMuzzleyCore = 3,
		ReplyToMuzzleyCore = 4,
		Signaling = 5
	};

	class Client;
	typedef std::function<bool(muzzley::JSONObj&, muzzley::Client&)> Handler;
	typedef Handler Callback;
	typedef map<int, pair<string, Callback> > CallbackQueue;
	typedef map<long, string> ParticipantList;

	class Client {
		public:
			Client();
			virtual ~Client();

#if __cplusplus >= 201103L
			virtual void on(muzzley::EventType _type, muzzley::Handler _handler) final;
			virtual void trigger(muzzley::EventType _type, muzzley::JSONObj& _data) final;
			#endif
			virtual void start() final;

			virtual void connectApp(string _app_token, string _activity_id = "");
			virtual void connectUser(string _user_token, string _activity_id);
			virtual void loginApp(string _app_token) final;
			virtual void loginUser(string _user_token) final;

			virtual void createActivity();
			virtual void joinActivity(string _activity_id = "");
			virtual void quit();
			virtual void participantQuit();

			virtual void participantReady(muzzley::Callback _callback = NULL);
			virtual void changeWidget(long _participant_id, string _widget, muzzley::Callback _callback = NULL);
			virtual void changeWidget(long _participant_id, string _widget, muzzley::JSONObj& _options, muzzley::Callback _callback = NULL);
			virtual void setupComponent(long _participant_id, string _component, string _component_id, string _action, muzzley::Callback _callback = NULL);
			virtual void setupComponent(long _participant_id, string _component, string _component_id, string _action, muzzley::JSONObj& _options, muzzley::Callback _callback = NULL);

			virtual void sendSignal(long _participant_id, string _type, muzzley::Callback _callback = NULL);
			virtual void sendSignal(long _participant_id, string _type, muzzley::JSONObj& _data, muzzley::Callback _callback = NULL);
			virtual void sendSignal(string _type, muzzley::Callback _callback = NULL);
			virtual void sendSignal(string _type, muzzley::JSONObj& _data, muzzley::Callback _callback = NULL);

			virtual void sendWidgetData(string _widget, string _component, string _event_type, string _event_value);

			virtual bool connect(string _host, uint16_t _port, string _path) final;
			virtual void disconnect() final;
			virtual void reconnect() final;
			virtual bool read() final;
			virtual bool write(muzzley::JSONObj& _data, muzzley::Callback _callback = NULL) final;
			virtual bool reply(muzzley::JSONObj& _data_received, muzzley::JSONObj& _reply) final;

			void setLoopAssynchronous(bool _assync);

			const string& getActivityId() const;
			const string& getDeviceId() const;
			const string& getSessionId() const;
			const long getParticipantId() const;
			const ParticipantList& getParticipants() const;

			bool isReplyNeeded(muzzley::JSONObj& _data_received) const;
			bool isAppLoggedin() const;
			bool isInitiatingMaster() const;
			bool isConnected() const;
			bool isLoopAssynchronous() const;
			bool isStaticActivity() const;
			bool isUserLoggedin() const;

		private:
			std::vector<muzzley::Handler> __handlers[N_EVENT_TYPES];
			long __serial;
			int __op_code;

			string __activity_id;
			string __message;
			string __session_id;
			string __device_id;
			long __participant_id;

			muzzley::socketstream __channel;

			CallbackQueue __queue;
			ParticipantList __participants;

			bool __is_static_activity;
			bool __is_connected;
			bool __is_to_reconnect;
			bool __is_loop_assynchronous;
			bool __is_app_loggedin;
			bool __is_user_loggedin;
			bool __is_initiating_master;
			bool __has_handshake;

			pthread_mutex_t* __mtx;
			pthread_t* __thr;
			pthread_mutexattr_t __attr;

			bool handshake(muzzley::Handler _callback);
			void process(muzzley::JSONObj& _received, muzzley::EventType* _type);

			void run();
			static void* launch(void* thread);

	};

	void printbits(unsigned char _b);
}
