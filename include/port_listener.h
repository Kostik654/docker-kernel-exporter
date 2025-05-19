#ifndef PORT_LISTENER_H
#define PORT_LISTENER_H
#include "collector.h"

class Listener
{

private:
    std::atomic<bool> STOP_FLAG{false};
    unsigned int PORT;
    std::string IPV4_ADDRESS;
    std::string ENDPOINT;
    Collector &COLLECTOR_REF;

public:

    Listener(config_data config, Collector &collector_ref);

    void start_server(size_t l_delay = 200); // listening cycle delay in milliseconds

    void stop_server();

    void handle_client(size_t socket);

    ~Listener();

};

#endif