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

#define DEBUG 1

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <muzzley.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

int main(int argc, char* argv[]) {

	// Create the Muzzley client
	muzzley::Client _client;

	// Register listener to be invoked when app succesfully logs in.
	//
	// Don't bother to store the returned deviceId or sessionId, the Client class already does that,
	// access it through the 'getDeviceId'  and 'getSessionId' method.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::ActivityCreated, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		cout << "Activity created with id " << _data["d"]["activityId"] << endl << flush;
		cout << "You can now pair your smartphone, using the muzzley app, to the activity id " << _data["d"]["activityId"] << endl << flush;
		return true;
	});

	_client.on(muzzley::ParticipantJoined, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		long participant_id = _data["d"]["participant"]["id"];
		string name = _data["d"]["participant"]["name"];
		cout << "Participant " << name << "(" << participant_id << ")" << " joined." << endl << flush;

		_client.changeWidget(participant_id, "switch", [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
			cout << "Widget successfully changed" << endl << flush;
		});
		return true;
	});

	_client.on(muzzley::WidgetAction, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		int _switch_value = (int) _data["d"]["v"];
		cout << "User pressed the switch to " << (_switch_value ? "1 (On)" : "0 (Off)") << endl << flush;
		return true;
	});

	// If set to 'true', the client will start a thread to listen for messages,
	// i.e. you will keep control of your program.
	_client.setLoopAssynchronous(true);

	// Connects the application to the Muzzley server.
	//
	// It will start the application loop assynchronously,
	// i.e. the program will continue to execute anything below this line,
	// be sure that you keep it alive.
	_client.connectApp("c1bc8d7d6bc38873");


	while (true) {
		cout << "At the main thread" << endl << flush; 
		sleep(5);
	}

	return 0;
}
