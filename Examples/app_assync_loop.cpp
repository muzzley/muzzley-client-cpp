#define DEBUG 1

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <muzzley.h>

using namespace std;
using namespace __gnu_cxx;

int main(int argc, char* argv[]) {

	// Create the Muzzley client
	muzzley::Client _client;

	// Register listener to be invoked when app succesfully logs in.
	//
	// Don't bother to store the returned deviceId or sessionId, the Client class already does that,
	// access it through the 'getDeviceId'  and 'getSessionId' method.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::AppLoggedIn, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		_client.createActivity();
		return true;
	});

	// Register listener to be invoked when activity is sucessfully created.
	//
	// Don't bother to store the returned activityId, the Client class already does that,
	// access it through the 'getActivityId' method.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::ActivityCreated, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (!!_data["s"] && ((string) _data["d"]["activityId"]) != "121345") {

			_client.sendSignal("changeWidget", JSON(
				"w"<< "gamepad" <<
				"c"<< "ba" <<
				"v"<< "a" <<
				"e"<< "press"
			),
			[] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
				cout << "signal accepted by server" << endl << flush;
			});
		}
		return true;
	});

	// Register listener to be invoked when app receives a signal message.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::SignalingMessage, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		cout << _data << endl << flush;
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

	return 0;
}
