#pragma once

#include <pthread.h>

class Lock {
	public:
		Lock() {
			pthread_mutex_init(&_lock, NULL);
		}
		~Lock() {
			pthread_mutex_destroy(&_lock);
		}
		void acquire() {
			pthread_mutex_lock(&_lock);
		}
		void release() {
			pthread_mutex_unlock(&_lock);
		}
	protected:
		pthread_mutex_t _lock;
};

class ScopeLock {
	public:
		ScopeLock(Lock *lock) : _lock(lock), _external(true) {
			_lock->acquire();
		}
		ScopeLock() : _lock(new Lock), _external(false) {
			_lock->acquire();
		}
		~ScopeLock() {
			_lock->release();
			if (!_external) {
				delete _lock;
				_lock = NULL;
			}
		}
	private:
		Lock *_lock;
		bool _external;
};
