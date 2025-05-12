#include "additional_funcs.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

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

    if (!fs::exists(filepath))
    {
        printf("Configuration file [%s] does not exists. Using default values.\n", filepath.c_str());
        return cfg;
    }
    else
        try
        {

            printf("\nUploading configuration from file: %s\n", filepath.c_str());

            std::string line;
            std::ifstream infile(filepath);
            std::vector<std::string> lines;

            int line_num = -1;

            while (std::getline(infile, line))
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
                    }
                    else if (param_name == "metrics_file")
                    {
                        cfg.default_metrics_file = value_;
                    }
                    else if (param_name == "dockerd_path")
                    {
                        cfg.default_dockerd_base_path = value_;
                    }
                    else
                    {
                        std::cerr << "Error: param [" << param_name << "] does not exist" << std::endl;
                        exit(2);
                    }

                    printf("%s = %s\n", param_name.c_str(), value_.c_str());
                    // std::cout << param_name << " = " << value_ << std::endl;
                }
            }

            return cfg;
        }
        catch (const std::exception &err)
        {
            std::cerr << "Reading configuration file [" << filepath.c_str() << "] error occurred: " << err.what() << std::endl;
            // returns default values
            return config_data();
        }
}
