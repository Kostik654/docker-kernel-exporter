#include "port_listener.h"

int main(int argc, char *argv[])
{
    config_data confy;

    if (argc > 1)
        confy = upload_config_data(argv[1]);
    else // Uploading configuration from file with the default relative path: ./kernel-de.conf
        confy = upload_config_data();

    std::unique_ptr<Collector> collector1 = std::make_unique<Collector>(confy);

    std::unique_ptr<Listener> listener1 = std::make_unique<Listener>(confy, &collector1);

    if (!collector1->check_paths())
    {
        printf("Exit code: 1\n");
        exit(1);
    }

    collector1->printConfig();

    // Method address (of Collector class), pointer of obj collector1
    std::thread collection_thread(&Collector::startCollecting, collector1.get());
    printf("\n== Collector1 thread is started ==\n");

    std::thread listener_thread(&Listener::start_server, listener1.get());
    printf("\n== Listener1 thread is started ==\n");

    // Waiting for completion of threads
    collection_thread.join();
    listener_thread.join();

    printf("Exit code: %d\n", Collector::exit_code);

    return 0;
}
