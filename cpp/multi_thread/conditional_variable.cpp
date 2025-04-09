#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <iostream>
#include <thread>
#include <queue>

using namespace std;

template <typename T> class ChannelThread;

template <typename T> class Channel {
    private:
	bool _ready;
	bool _alive;
	mutex _m;
	condition_variable _cv;
	queue<T> _data;

    public:
	Channel()
	{
		this->_ready = false;
		this->_alive = true;
	};

	virtual ~Channel()
	{
		this->terminate();
	}

	void buffer(T n)
	{
		lock_guard<mutex> lock(this->_m);
		this->_data.push(n);
	}

	T receive(void)
	{
		T n;

		{
			lock_guard<mutex> lock(this->_m);
			if (!this->_data.empty()) {
				n = this->_data.front();
				this->_data.pop();
			}
		}

		return n;
	}

	bool is_empty(void)
	{
		lock_guard<mutex> lock(this->_m);
		return this->_data.empty();
	}

	void wait_till_ready(void)
	{
		{
			unique_lock<mutex> lock_unique(this->_m);
			this->_cv.wait(lock_unique, [this] {
				if (!this->_alive) {
					return true;
				}

				return this->_ready;
			});

			if (this->_ready) {
				this->_ready = false;
			}
		}
	}

	void send(void)
	{
		lock_guard<mutex> lock(this->_m);
		this->_ready = true;
		this->_cv.notify_one();
	}

	void terminate(void)
	{
		lock_guard<mutex> lock(this->_m);
		this->_alive = false;
		this->_cv.notify_one();
	}

	bool alive(void)
	{
		lock_guard<mutex> lock(this->_m);
		return this->_alive;
	}
};

template <typename T> class ChannelThread : public Channel<T> {
    public:
	ChannelThread(function<void(ChannelThread<T> &)> func)
		: Channel<T>()
		, _func(func)
		, _t(this->_func, std::ref(*this))
	{
	}

	~ChannelThread()
	{
		if (_t.joinable()) {
			_t.join();
		}
	}

	ChannelThread(const ChannelThread &) = delete;
	ChannelThread &operator=(const ChannelThread &) = delete;
	ChannelThread(ChannelThread &&) = delete;
	ChannelThread &operator=(ChannelThread &&) = delete;

    private:
	function<void(ChannelThread<T> &)> _func;
	thread _t;
};

template <typename T> ChannelThread<T> &operator<<(ChannelThread<T> &ch, T &t)
{
	ch.buffer(t);
	return ch;
}

template <typename T> T &operator>>(ChannelThread<T> &ch, T &t)
{
	t = ch.receive();
	return t;
}

void worker(ChannelThread<int> &channel)
{
	int i;
	while (true) {
		channel.wait_till_ready();

		while (!channel.is_empty()) {
			channel >> i;
			cout << "got from channel: " << i << endl;
		}

		if (!channel.alive()) {
			return;
		}
	}
}

int main(void)
{
	int i;
	int sleep;
	ChannelThread<int> t1(worker);

	for (i = 1; i <= 100; i++) {
		sleep = 1;
		t1 << i;
		cout << "buffering " << i << endl;

		if (i % 5 == 0) {
			t1.send();
			cout << "sending" << endl;

			sleep = 3;
		}

		if (i == 6) {
			cout << "setting terminated" << endl;
			t1.terminate();
			break;
		}

		this_thread::sleep_for(chrono::seconds(sleep));
	}

	cout << "done" << endl;

	return 0;
}