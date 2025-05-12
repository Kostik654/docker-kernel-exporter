#include "collector.h"

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
    printf("\n== Collector thread is started ==\n");

    // Waiting for completion of second thread
    collection_thread.join();

    printf("Exit code: 0\n");

    return 0;
}
