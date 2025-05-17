#include "port_listener.h"

Listener::Listener(config_data config, Collector &collector_ref) : COLLECTOR_REF(collector_ref)
{
    this->IPV4_ADDRESS = config.ipv4_address;
    this->PORT = config.port;
    this->ENDPOINT = config.endpoint;
};

void Listener::start_server(size_t l_delay)
{
    int server_fd, new_socket;

    sockaddr_in address;

    int addr_len = sizeof(address);

    // creating socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Socket creation failed";
        exit(399);
    };

    // enabling the address reusing to avoid the TIME_WAIT error because of the often server restarting
    int addrreuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &addrreuse, sizeof(addrreuse)) < 0)
    {
        std::cerr << "Reusing address configuration failed";
        exit(398);
    };

    // configuring address
    address.sin_family = AF_INET; // ipv4
    inet_pton(AF_INET, this->IPV4_ADDRESS.c_str(), &address.sin_addr.s_addr);
    address.sin_port = htons(this->PORT);

    // binding
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Binding failed";
        exit(397);
    };

    // listening
    if (listen(server_fd, 10) < 0)
    {
        std::cerr << "Listening error";
        close(server_fd);
        exit(396);
    };

    printf("== Server is listening %s:%d ==\n", this->IPV4_ADDRESS.c_str(), this->PORT);

    while (!STOP_FLAG)
    {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_len);

        if (new_socket < 0)
            continue;

        std::thread(&Listener::handle_client, this, new_socket).detach();

        std::this_thread::sleep_for(std::chrono::milliseconds(l_delay));
    };

    close(server_fd);
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

    // printf("listener: incoming responce: %s\n", endpoint.c_str());

    if (method == "GET" && endpoint == this->ENDPOINT)
    {
        std::ostringstream oss;
        std::string content;

        // printf("GET metrics\n");

        do
        {
            if (!this->COLLECTOR_REF.lock_data)
            {
                content = this->COLLECTOR_REF.collected_data;
                break;
            }

        } while (this->COLLECTOR_REF.lock_data);

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
