#include "../01/sender.hpp"

int main(int argc, char *argv[]) {
    // set data exchange filename
    // string filename = parse_arguments(argc, argv);

    std::ofstream log_stream;
    if(log_init(log_stream, "sender.log", Info) < 0) {
        cout << "[sender] Open log error!" << endl;
        return E_LOG_OPEN;
    }

    // continuously fork subprocesses
    char *shared_memory_application_transport = get_shared_memory(kRandomDataSize);
    pid_t pid = fork();
    if (fork) { // in parent, execute layer 5
        sender_application_layer();
    } else { // in child
        pid = fork();
        if (pid) { // in parent, execute layer 4
            sender_transport_layer();
        } else { // in child
            pid = fork();
            if (pid) { // in parent, execute layer 3
                sender_network_layer();
            } else { // in child
                pid = fork();
                if (pid) { // in parent, execute layer 2
                    sender_datalink_layer();
                } else {
                    pid = fork();
                    if (pid) { // in parent, execute layer 1
                        sender_physical_layer();
                    }                    
                }
            }
        }
    }
}

int sender_physical_layer_entrance() {
    // exit when parent dies
    prctl(PR_SET_PDEATHSIG, SIGHUP);

    // init log file

    // 
}
