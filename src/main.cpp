#include "additional_funcs.h"
#include "collector.h"

#include <iostream>
#include <thread>

int main()
{

    // Uploading configuration from file
    config_data confy = upload_config_data();

    std::unique_ptr<Collector> collector1 = std::make_unique<Collector>(confy);

    if (!collector1->check_paths())
    {
        printf("Exit code: 1\n");
        exit(1);
    }

    collector1->printConfig();

    // Method address (of Collector class), pointer of obj collector1
    std::thread collection_thread(&Collector::startCollecting, collector1.get());

    // Waiting for completion of second thread
    collection_thread.join();

    return 0;
}
