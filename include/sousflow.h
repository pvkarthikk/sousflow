#ifndef SOUSFLOW_H
#define SOUSFLOW_H
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
namespace Sousflow{
    typedef struct{
        std::string cmd;
        std::string args;
        std::string root;
        std::string log;
    }Task;
    typedef struct{
        std::string name;
        Task        task;
    }TaskNode;
    class TaskNodes{
        private:
        public:
            std::vector<TaskNode> nodes;
            bool is_available(std::string name){
                for(TaskNode task:this->nodes){
                    if(task.name == name){
                        return true;
                    }
                }
                return false;
            }
            void add(std::string name, Task task){
                this->add(TaskNode{name, task});
            }
            size_t size(){
                return this->nodes.size();
            }
            void add(TaskNode node){
                if(is_available(node.name)){
                    std::stringstream err;
                    err << "Task '"<<node.name<<"' already exists";
                    throw std::runtime_error(err.str());
                }
                this->nodes.push_back(node);
            }
            void add(std::vector<TaskNode> nodes){
                for(TaskNode node:nodes){
                    this->add(node);
                }
            }
            TaskNode operator[](std::string name){
                for(TaskNode task:this->nodes){
                    if(task.name == name){
                        return task;
                    }
                }
                std::stringstream err;
                err << "Task '"<<name<<"' not found";
                throw std::runtime_error(err.str());
            }
    };
    typedef std::vector<TaskNodes>  TaskGroup;
    typedef struct{
        std::string name;
        TaskGroup   group;
    }TaskGroupNode;
    class TaskGroupNodes{
        private:
            std::vector<TaskGroupNode> nodes;
        public:
            bool is_available(std::string name){
                for(TaskGroupNode group:this->nodes){
                    if(group.name == name){
                        return true;
                    }
                }
                return false;
            }
            void add(std::string name, TaskGroup group){
                this->add(TaskGroupNode{name, group});
            }
            void add(TaskGroupNode node){
                if(is_available(node.name)){
                    std::stringstream err;
                    err << "Group '"<<node.name<<"' already exists";
                    throw std::runtime_error(err.str());
                }
                this->nodes.push_back(node);
            }
            TaskGroupNode operator[](std::string name){
                for(TaskGroupNode group:this->nodes){
                    if(group.name == name){
                        return group;
                    }
                }
                std::stringstream err;
                err << "Group '"<<name<<"' not found";
                throw std::runtime_error(err.str());
            }
    };
    typedef std::unordered_map<std::string, std::string> TaskVariables;
    typedef struct{
        std::string     yml_path;
        std::string     yml_dir;
        TaskNodes       tasks;
        TaskGroupNodes  groups;
        TaskVariables   variables;
    }SousflowTasks;
    SousflowTasks parse(std::string yml_path_str);
}

#endif
