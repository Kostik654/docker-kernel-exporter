#include "filesystem_funcs.h"

namespace fs = std::filesystem;

bool check_object_path(std::string obj_path, std::string obj_name, bool is_dir)
{
    if (is_dir)
    {
        if (!(fs::exists(obj_path) && fs::is_directory(obj_path)))
        {
            std::cerr << ">> Directory [" << obj_path << "] of object [" << obj_name << "] does not exist" << std::endl;
            return false;
        }
    }
    else
    {
        if (!(fs::exists(obj_path) && fs::is_regular_file(obj_path)))
        {
            std::cerr << ">> File [" << obj_path << "] of object [" << obj_name << "] does not exist" << std::endl;
            return false;
        }
    }

    return true;
};

bool output_metrics(std::string content, std::string filepath)
{

    try
    {
        std::ofstream out(filepath);
        out << content;
        out.close();
        return true;
    }
    catch (const std::exception &err)
    {
        std::cerr << "Writing metrics to file [" << filepath << "] error occurred: " << err.what() << std::endl;
        return false;
    }
}

config_data upload_config_data(std::string filepath)
{
    config_data cfg;

    std::ifstream conf_file(filepath);
    std::string line;
    std::vector<std::string> lines;

    bool scrapeperiod_initialized = false;
    bool metricsfile_initialized = false;
    bool dockerdpath_initialized = false;

    if (!fs::exists(filepath))
    {
        printf("Configuration file [%s] does not exists. Using default values.\n", filepath.c_str());
        return cfg;
    }
    else
        try
        {
            printf("\nUploading configuration from file: %s\n", filepath.c_str());

            if (conf_file.is_open())
            {
                while (std::getline(conf_file, line))
                {

                    std::string clean_line{""};

                    // delete all spaces
                    for (char c : line)
                    {
                        if (!std::isspace(static_cast<unsigned char>(c)))
                            clean_line = clean_line + c;
                    }

                    if (clean_line.empty() || clean_line[0] == '#')
                        continue;

                    // returns npos if char # was not found
                    size_t comment_pos = clean_line.find('#');
                    if (comment_pos != std::string::npos)
                        clean_line = clean_line.substr(0, comment_pos);

                    size_t eq_pos = clean_line.find('=');
                    if (eq_pos == std::string::npos)
                    {
                        std::cerr << "Syntax error, '=' was not found in line: " << line << std::endl;
                        exit(2);
                    }
                    else
                    {

                        if (clean_line.length() < 2 || clean_line[0] == '=')
                        {
                            std::cerr << "Error: line [" << line << "] is empty for parsing" << std::endl;
                            exit(2);
                        }

                        std::string param_name = clean_line.substr(0, eq_pos);
                        std::string value_ = clean_line.substr(eq_pos + 1);

                        if (param_name == "scrape_period")
                        {
                            cfg.scrape_period = std::stoi(value_);
                            scrapeperiod_initialized = true;
                        }
                        else if (param_name == "metrics_file")
                        {
                            cfg.default_metrics_file = value_;
                            metricsfile_initialized = true;
                        }
                        else if (param_name == "dockerd_path")
                        {
                            cfg.default_dockerd_base_path = value_;
                            dockerdpath_initialized = true;
                        }
                        else
                        {
                            std::cerr << "Error: param [" << param_name << "] does not exist" << std::endl;
                            exit(2);
                        }

                        printf(">> uploader: %s = %s\n", param_name.c_str(), value_.c_str());

                        if (metricsfile_initialized && dockerdpath_initialized && scrapeperiod_initialized)
                        {
                            conf_file.close();
                            break;
                        }
                    }
                }
            }
            else
            {
                std::cerr << ">> Error: unable to open the configuration file [" << filepath << "].\nThe uninitialized vars will be set to defaults.\n";
                return config_data();
            }

            if (metricsfile_initialized && dockerdpath_initialized && scrapeperiod_initialized)
                printf("\n== All the configuration variables were updated successfully ==\n");
            else
            {
                printf("\nWARNING: Not all the configuration variables were updated successfully.\n The uninitialized vars will be set to defaults.\n");
                conf_file.close();
            }
            return cfg;
        }
        catch (const std::exception &err)
        {
            std::cerr << "Reading configuration file [" << filepath.c_str() << "] error occurred: " << err.what() << std::endl;
            printf("\n== Not all the configuration variables were updated successfully. Uninitialized vars will be set to defaults. ==\n");
            conf_file.close();
            // returns default values
            return config_data();
        }
}
