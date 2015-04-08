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

//
// You can compile this application with the following command:
// g++ -std=c++0x -I/usr/include/muzzley examples/app.cpp -o myapp -lmuzzley -lpthread
//
// And run it like this:
// ./myapp
// 
// Once running, you can pair your smartphone to this app using
// the mobile muzzley application and the "Activity Id" that is
// written to the screen.
//

#define APP_TOKEN "c1bc8d7d6bc38873" // Get yours at muzzley.com

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <muzzley/muzzley.h>
#include <muzzley/parsers/http.h>
#include <muzzley/stream/SSLSocketStreams.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

void lock(int& semaphore) {
	// Tests for '0' on the semaphore and increments '1' when '0' is observed
	struct sembuf lock[2] = { { (short unsigned int) 0, 0 }, { (short unsigned int) 0, 1 } };
	semop(semaphore, lock, 2);	
}

void unlock(int& semaphore) {
	// Decrements '1' on the semaphore, unblocking threads waiting for '0'
	struct sembuf unlock[1] = { { (short unsigned int) 0, -1 } };
	semop(semaphore, unlock, 1);	
}

void stop(int sig) {
	key_t key = ftok("multithreading.cpp", 1); // connects to the same semaphore
	int semaphore = semget(key, 1, IPC_CREAT | 0777);
	semctl(semaphore, 0, IPC_RMID);
	exit(0);
}

int main(int argc, char* argv[]) {
	string _component_id = ${due to public usage of demo channels and to the load balacing infra-strucuture, when using the muzzley:_demo-channel_, replace this with a unique string, for instance you IP address combined with the timestamp};

	// Create the Muzzley client
	muzzley::Client _client;

	key_t key = ftok("multithreading.cpp", 1); // any existing file that is only used by your program will do
	int semaphore = semget(key, 1, IPC_CREAT | 0777);
	if (semaphore == 0) {
	}

	// Register listener to be invoked when activity is sucessfully created.
	//
	// Don't bother to store the returned activityId, the Client class already does that,
	// access it through the 'getActivityId' method.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::AppLoggedIn, [ & semaphore, & _component_id ] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		cout << "App logged in with created with session id " << (string) _data["d"]["sessionId"] << endl << flush;

		muzzley::Subscription _s1;
		_s1.setNamespace("iot");
		_s1.setProfile("muzzley");
		_s1.setChannel("_demo-channel_");
		_s1.setComponent(_component_id);
		_s1.setProperty("intensity");

		muzzley::Message _m1;
		_m1.setData(JSON(
			"io" << "i" <<
			"data" << JSON(
				"value" << 125 << 
				"unit" << "lm"
			)
		));
		// Ensure that this is the only thread accessing the semaphore and block others
		lock(semaphore);
		_client.trigger(muzzley::Publish, _s1, _m1);
		// Unblock semaphore
		unlock(semaphore);

		muzzley::Message _m2;
		_m2.setData(JSON(
			"io" << "r"
		));

		// Ensure that this is the only thread accessing the semaphore and block others
		lock(semaphore);
		_client.trigger(muzzley::Publish, _s1, _m2, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
			_data->prettify(cout);
			cout << endl << flush;			
			return true;
		}); 
		// Unblock semaphore
		unlock(semaphore);

		return true;

	});

	// Adds listeners for SIGKILL, for gracefull stop
	//
	// It will be invoked when the user hits Ctrl-c
	// For a list of allocated sempahores (and other shared resources), execute the 'ipcs' command on a tty
	struct sigaction action;
	action.sa_handler = stop;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGTERM, &action, 0);
	sigaction(SIGQUIT, &action, 0);
	sigaction(SIGINT, &action, 0);

	// Connects the application to the Muzzley server, using the one-step-initialization process.
	//
	// It will start the application loop synchronously,
	// i.e. the program will not execute anything below this line.
	_client.initApp(APP_TOKEN);

	// Destroy the semaphore
	semctl(semaphore, 0, IPC_RMID);

	return 0;
}
