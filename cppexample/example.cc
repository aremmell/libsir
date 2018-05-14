#include "../sir.hh"
#include <iostream>
#include <thread>

using namespace std;

void thread_fn(std::string say, sir::logger& log) {
    for (size_t n = 0; n < 1000; n++) {
        sir::stringstream_t strm;
        strm << say << ": " << n;
        log.debug(strm);
    }
}

int main(int argc, char** argv) {

    try {
        sir::logger log("sups");
  
        log.debug(sir::serializable::format("sups bruv? %d\n", 12343));
        /* std::thread t1(thread_fn, "number juan", std::ref(log));
        std::thread t2(thread_fn, "number two", std::ref(log));
        std::thread t3(thread_fn, "number tree", std::ref(log));

        t1.join();
        t2.join();
        t3.join();

        cout << "all threads completed running." << endl; */

    } catch (sir::lib_exception& e) {
        cerr << e.what() << endl;
    }

    return 0;
}
