#include "../sir.hh"
#include <iostream>

using namespace std;

int main(int argc, char** argv) {

    try {
        sir::logger log(SIRL_ALL, -1, 0, SIRO_DEFAULT, 0, "sups");

        log.debug << "debug text" << endl;

        if (log.debug.fail())
            cerr << "debug failed! error: " << log.geterror() << endl;

        log.info << "info text" << endl;  
        log.notice << "notice text" << endl;  
        log.warn << "warn text" << endl;  
        log.error << "error text" << endl;  
        log.crit << "crit text" << endl;  
        log.alert << "alert text" << endl;  
        log.emerg << "emerg text" << endl;  

        log.info << "break you" << endl;

    } catch (sir::lib_exception& e) {
        cerr << e.what() << endl;
    }

    return 0;
}
