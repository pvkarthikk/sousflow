#include <sousflow.h>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "utility.hpp"
using namespace Sousflow;

bool is_task(std::string task_name, YAML::Node task_property){
    if (task_property.IsMap()) {
        for (YAML::const_iterator it = task_property.begin();
            it != task_property.end(); ++it) {
            std::string property_name = it->first.as<std::string>();
            if (property_name == "cmd") {
                return true;
            }
        }
    }
    return false;
}
bool is_group(std::string task_name, YAML::Node task_property){
    if (task_property.IsSequence()) {
        return true;
    }
    return false;
}
bool is_variable(std::string task_name, YAML::Node task_property){
    if (task_property.IsMap() && task_name == "variables") {
        return true;
    }
    return false;
}
std::map<std::string, std::string> parse_variable(YAML::Node variable_node){
    std::map<std::string, std::string> output;
    for (YAML::const_iterator it_var = variable_node.begin(); it_var != variable_node.end(); ++it_var){
        std::string var_name = it_var->first.as<std::string>();
        std::string var_val = it_var->second.as<std::string>();
        output[var_name] = var_val;
    }
    return output;
}
Task parse_task(YAML::Node task_node){
    Task out;
    for (YAML::const_iterator it = task_node.begin();it != task_node.end(); ++it){
        std::string property_name = it->first.as<std::string>();
        if(property_name == "cmd"){
            out.cmd = it->second.as<std::string>();
        }
        else if(property_name == "args"){
            out.args = it->second.as<std::string>();
        }
        else if(property_name == "log"){
            out.log = it->second.as<std::string>();
        }
        else if(property_name == "root"){
            out.root = it->second.as<std::string>();
        }
        else if(property_name == "env"){
            if(it->second.IsMap()){
                for (YAML::const_iterator it_env = it->second.begin();it_env != it->second.end(); ++it_env){
                    std::string env_name = it_env->first.as<std::string>();
                    std::string env_val = it_env->second.as<std::string>();
                    out.env[env_name] = env_val;
                }
            }
            else{
                throw std::runtime_error("Invalid env type");
            }
        }
        else{
            // do nothing
        }        
    }
    return out;
}
TaskGroup parse_group(YAML::Node group_node){
    TaskGroup out;
    for(unsigned int seq_idx = 0;seq_idx < group_node.size();seq_idx++){
        YAML::Node   seq_node = group_node[seq_idx];
        if(seq_node.IsSequence()){
            std::vector<std::string> ll_list;
            for(unsigned int ll_idx = 0; ll_idx < seq_node.size(); ll_idx++){
                YAML::Node   ll_node = seq_node[ll_idx];
                std::string  name = ll_node.as<std::string>();
                ll_list.push_back(name);
            }
            if(ll_list.size() > 0){
                out.push_back(ll_list);
            }
        }
        else{
            std::string name = seq_node.as<std::string>();
            out.push_back({name});
        }
    }
    return out;
}
TaskYml Sousflow::parse(std::string yml_path_str){
    TaskYml out;
    out.path = yml_path_str;
    YAML::Node config = YAML::LoadFile(out.path);
    if(!config.IsMap()){
        throw std::runtime_error("Invalid file type "+ out.path);
    }
    for(YAML::const_iterator it = config.begin(); it != config.end(); ++it){
        std::string name = it->first.as<std::string>();
        YAML::Node  property = it->second;
        if(is_variable(name, property)){
            std::map<std::string, std::string> user_variables = parse_variable(property);
            out.variables.merge(user_variables);
        }
        else if(is_task(name, property)){
            out.tasks[name] = parse_task(property);
        }
        else if(is_group(name, property)){
            out.groups[name] = parse_group(property);
        }
        else{
            // do nothing
        }
    }
    return out;
}
TaskYml Sousflow::post_parse(TaskYml yml){
    TaskYml out = yml;
    
    // update task variables
    for(auto task:out.tasks){
        std::string task_name = task.first;
        Task task_obj = task.second;
        std::map<std::string, std::string> local = Sousflow::Utility::get_environment_variables();
        local.insert(out.variables.begin(), out.variables.end());
        local["name"] = task_name;
        local["time"] = Sousflow::Utility::get_time();
        local["cwd"]  = Sousflow::Utility::get_cwd();
        local["yml_dir"] = std::filesystem::path(out.path).parent_path().string();
        task_obj.cmd = Sousflow::Utility::replace_variables(task_obj.cmd, local);
        task_obj.args = Sousflow::Utility::replace_variables(task_obj.args, local);
        task_obj.root = Sousflow::Utility::replace_variables(task_obj.root, local);
        task_obj.log = Sousflow::Utility::replace_variables(task_obj.log, local);
        out.tasks[task_name] = task_obj;
    }
    return out;
}

SousflowRunner::SousflowRunner(std::string yml_path_str){
    this->m_yml = Sousflow::parse(yml_path_str);
    this->m_yml = Sousflow::post_parse(this->m_yml);
}
void SousflowRunner::start(std::string name){

}
