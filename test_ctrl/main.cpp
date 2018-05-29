#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <vector>

using namespace std;

typedef std::string String;





struct Log {
	size_t executed;
};


struct Control {
	pthread_t id;
	int status;

	virtual void start(){}
	virtual void stop(){}
};



struct Thread : Control {
	volatile bool continue_running;

	Thread(){
	}

	void start(){
		this->continue_running = true;
		pthread_create(&this->id, NULL, &this->task, this);
	}

	void stop(){
		this->continue_running = false;
	}

	void openConnection(){
	}

	static void* task(void* _arg){
		Thread* self = (Thread*) _arg;
		while( self->continue_running ){
			printf("opa %ld\n",self->id);
			sleep(1);
		}
		return NULL;
	}
};






struct Robot {
	vector<Control*> ctrlpkg;

	Robot(){
		Control* main = new Control();
		this->ctrlpkg.push_back(main);
	}

	void makeThread(){
		Control* ctrl = new Thread();
		ctrl->start();
		this->ctrlpkg.push_back(ctrl);
	}

	void wait(){
		void *status;
		pthread_join(ctrlpkg[1]->id, &status);
	}

	void logIniFunction(String name){
	}

	void logEndFunction(){
	}
};


Robot ROBOT;




void http_server(){
	
}


int main(){
	teste();

	return 0;
}
