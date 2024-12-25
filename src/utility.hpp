#pragma once

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <ctime>
#include <regex>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <cstdlib>
#include <unistd.h>
#define GetCurrentDir getcwd
extern char **environ;
#endif

namespace Sousflow{
    namespace Utility{

        std::string get_cwd() {
            char buffer[FILENAME_MAX];
            if (GetCurrentDir(buffer, FILENAME_MAX)) {
                return std::string(buffer);
            } else {
                return std::string();
            }
        }
        
        std::string get_time(){
            // Example of the very popular RFC 3339 format UTC time
            std::time_t time = std::time({});
            char timeString[std::size("yyyy_mm_ddThh_mm_ssZ")];
            std::strftime(std::data(timeString), std::size(timeString),
                        "%Y%b%d_%H%M%S", std::gmtime(&time));
            return std::string(timeString);
        }
        
        std::map<std::string,std::string> get_environment_variables() {
            std::map<std::string,std::string> env_map;
        #ifdef _WIN32
            char* env_strings = GetEnvironmentStrings();
            if (env_strings) {
                char* current = env_strings;
                while (*current) {
                    std::string env_entry(current);
                    size_t pos = env_entry.find('=');
                    if (pos != std::string::npos) {
                        std::string key = env_entry.substr(0, pos);
                        key = "ENV:"+key;
                        std::string value = env_entry.substr(pos + 1);
                        env_map[key] = value;
                    }
                    current += env_entry.size() + 1;
                }
                FreeEnvironmentStrings(env_strings);
            }
        #else
            for (char** current = environ; *current; ++current) {
                std::string env_entry(*current);
                size_t pos = env_entry.find('=');
                if (pos != std::string::npos) {
                    std::string key = env_entry.substr(0, pos);
                    std::string value = env_entry.substr(pos + 1);
                    env_map[key] = value;
                }
            }
        #endif
            return env_map;
        }

        std::string replace_variables(std::string input, std::map<std::string,std::string> variables){
            std::string output_str = input;
            for(auto variable:variables){
                std::string key = variable.first;
                std::string val = variable.second;
                std::regex rexp("\\$\\{\\s*(" + key + ")\\s*\\}");
                output_str = std::regex_replace(output_str,rexp,val);
            }
            return output_str;
        }
    }
}

#endif//UTILITY_HPP