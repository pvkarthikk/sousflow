
#include <filesystem>
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

#include <sousflow.h>
#include <yaml-cpp/yaml.h>

using namespace Sousflow;



std::string get_current_dir() {
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
TaskVariables get_env_map() {
    TaskVariables env_map;

#ifdef _WIN32
    char* env_strings = GetEnvironmentStrings();
    if (env_strings) {
        char* current = env_strings;
        while (*current) {
            std::string env_entry(current);
            size_t pos = env_entry.find('=');
            if (pos != std::string::npos) {
                std::string key = env_entry.substr(0, pos);
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
std::string replace_variables(std::string input, TaskVariables variables){
    std::string output_str = input;
    for(auto variable:variables){
        std::string key = variable.first;
        std::string val = variable.second;
        std::regex rexp("\\$\\{\\s*(" + key + ")\\s*\\}");
        output_str = std::regex_replace(output_str,rexp,val);
    }
    return output_str;
}

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
TaskVariables parse_variable(YAML::Node task_property){
    TaskVariables output;
    for (YAML::const_iterator it_var = task_property.begin(); it_var != task_property.end(); ++it_var){
        std::string var_name = it_var->first.as<std::string>();
        std::string var_val = it_var->second.as<std::string>();
        output[var_name] = var_val;
    }
    return output;
}
TaskNode parse_task(std::string task_name, YAML::Node task_property, SousflowTasks sousflow){
    TaskNode output;
    TaskVariables local = sousflow.variables;
    local["name"] = task_name;
    local["time"] = get_time();
    output.name = task_name;
    for (YAML::const_iterator it = task_property.begin();it != task_property.end(); ++it){
        std::string property_name = it->first.as<std::string>();
        std::string property_value = it->second.as<std::string>();
        if(property_name == "cmd"){
            output.task.cmd = replace_variables(property_value, local);
        }
        else if(property_name == "args"){
            output.task.args = replace_variables(property_value, local);
        }
        else if(property_name == "log"){
            output.task.log = replace_variables(property_value, local);
        }
        else if(property_name == "root"){
            output.task.root = replace_variables(property_value, local);
        }
        else{
            // do nothing
        }
        if(output.task.root == ""){
            output.task.root = ".";
        }
        std::filesystem::path _temp;
        std::filesystem::path yml_dir_path(sousflow.yml_dir);
        
        _temp = yml_dir_path/std::filesystem::path(output.task.cmd);
        output.task.cmd = std::filesystem::absolute(_temp).string();
        
        _temp = yml_dir_path/std::filesystem::path(output.task.root);
        output.task.root = std::filesystem::absolute(_temp).string();
        
        if(output.task.log != ""){
            _temp = yml_dir_path/std::filesystem::path(output.task.log);
            output.task.log = std::filesystem::absolute(_temp).string();
        }
    }
    return output;
}
TaskGroupNode parse_group(std::string grp_name, YAML::Node grp_property, SousflowTasks sousflow){
    TaskGroupNode output;
    output.name = grp_name;
    for(unsigned int seq_index = 0;seq_index < grp_property.size();seq_index++){
        YAML::Node   seq_node = grp_property[seq_index];
        TaskNodes nodes;
        if(seq_node.IsSequence()){  // parallel task
            for(unsigned int ll_index = 0; ll_index < seq_node.size(); ll_index++){
                YAML::Node   ll_node  = seq_node[ll_index];
                std::string  ll_name  = ll_node.as<std::string>();
                if(sousflow.tasks.is_available(ll_name)){
                    TaskNode node = sousflow.tasks[ll_name];
                    nodes.add(node);
                }
                else if(sousflow.groups.is_available(ll_name)){
                    std::stringstream err;
                    err << "Group task("<<ll_name<<") not allowed to parallelise";
                    err << "inside another group("<<grp_name<<")";
                    throw std::runtime_error(err.str());

                }
                else{
                    std::stringstream err;
                    err << "'"<<ll_name<<"'is missing or defined before '"<<grp_name<<"'";
                    err << "inside another group("<<grp_name<<")";
                    throw std::runtime_error(err.str());
                }
            }
        }
        else{                       // sequence task
            std::string task_name = seq_node.as<std::string>();
            if(sousflow.tasks.is_available(task_name)){
                TaskNode node = sousflow.tasks[task_name];
                nodes.add(node);
            }
            else if(sousflow.groups.is_available(task_name)){
                TaskGroupNode node = sousflow.groups[task_name];
                // for(TaskNodes tasks:node.group){
                //     nodes.add(tasks);
                // }
                // nodes.add(node.group);
            }
            else{
                std::stringstream err;
                err << "'"<<task_name<<"'is missing or defined before '"<<grp_name<<"'";
                err << "inside another group("<<grp_name<<")";
                throw std::runtime_error(err.str());
            }
        }
        if(nodes.size() != 0){
            output.group.push_back(nodes);
        }
    }
    return output;
}
SousflowTasks parse(std::string yml_path_str){
    SousflowTasks output;
    // get absolute path from yml_path_str
    output.yml_path = std::filesystem::absolute(yml_path_str).string();
    // get yml directory path from yml_path_str
    output.yml_dir  = std::filesystem::path(yml_path_str).parent_path().string();
    // get environment map and update it TaskVariables
    output.variables = get_env_map();
    // update built-in variables cwd and yml_dir into TaskVariables 
    output.variables["cwd"]= get_current_dir();
    // parse yaml file
    YAML::Node config = YAML::LoadFile(output.yml_path);
    if(!config.IsMap()){
        return output;
    }
    // iterate each node
    for (YAML::const_iterator it = config.begin(); it != config.end(); ++it) {
        std::string name = it->first.as<std::string>();
        YAML::Node  property = it->second;
        // parse variables if presents
        if(is_variable(name, property)){
            TaskVariables user_variables = parse_variable(property);
            output.variables.merge(user_variables);
        }
        else if(is_task(name, property)){
            TaskNode _task = parse_task(name, property, output);
            output.tasks.add(_task);
        }
        else if(is_group(name, property)){
            TaskGroupNode _group = parse_group(name, property, output);
            output.groups.add(_group);
        }
        else{
            // do nothing
        }
    }
    
    return output;
}   