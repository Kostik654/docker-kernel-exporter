#include "port_listener.h"

Listener::Listener(config_data config, Collector &collector_ref) : COLLECTOR_REF(collector_ref)
{
    this->IPV4_ADDRESS = config.ipv4_address;
    this->PORT = config.port;
    this->ENDPOINT = config.endpoint;
};

void Listener::start_server(size_t l_delay)
{
    while (!STOP_FLAG)
    {
        std::string resp_content = this->COLLECTOR_REF.collected_data;

        std::this_thread::sleep_for(std::chrono::milliseconds(l_delay));
    }
};

void Listener::stop_server()
{
    this->STOP_FLAG = true;
};

void Listener::handle_client(size_t client_socket)
{

    char buffer[1024] = {0};
    read(client_socket, buffer, sizeof(buffer));

    std::string responce;

    std::istringstream req_stream(buffer);
    std::string method;
    std::string endpoint;
    std::string http_v;

    req_stream >> method >> endpoint >> http_v;

    if (method == "GET" && endpoint == this->ENDPOINT)
    {
        std::ostringstream oss;
        std::string content;

        do
        {
            if (!this->COLLECTOR_REF.is_writing)
            {
                content = COLLECTOR_REF.collected_data;
                break;
            }
        } while (this->COLLECTOR_REF.is_writing);

        oss << "HTTP/1.1 200 OK\r\n";
        oss << "Content-Type: text/plain; version=0.0.4\r\n";
        oss << "Content-Length: " << content.size() << "\r\n";
        oss << "Connection: close\r\n\r\n";
        oss << content;
        responce = oss.str();
    }
    else
    {
        std::string body = "404 Not Found";
        std::ostringstream oss;
        oss << "HTTP/1.1 " << body << "\r\n";
        oss << "Content-Type: text/plain\r\n";
        oss << "Content-Length: " << body.size() << "\r\n";
        oss << "Connection: close\r\n\r\n";
        oss << body;
        responce = oss.str();
    }
    send(client_socket, responce.c_str(), responce.size(), 0);
    close(client_socket);
};

Listener::~Listener()
{
    this->stop_server();
};
