#ifndef SOUSFLOW_H
#define SOUSFLOW_H
#include <string>
#include <vector>
#include <map>

namespace Sousflow{
    typedef struct{
        std::string                         cmd;
        std::string                         args;
        std::string                         root;
        std::string                         log;
        std::map<std::string, std::string>  env;
    }Task;
    typedef std::vector<std::vector<std::string>> TaskGroup;
    typedef struct{
        std::string                         path;
        std::map<std::string, std::string>  variables;
        std::map<std::string, Task>         tasks;
        std::map<std::string, TaskGroup>    groups;
    }TaskYml;

    TaskYml parse(std::string yml_path_str);
    TaskYml post_parse(TaskYml yml);
    class SousflowRunner{
        private:
            TaskYml m_yml;
        public:
            SousflowRunner(std::string yml_path_str);
            void start(std::string name);
    };
}
#endif
