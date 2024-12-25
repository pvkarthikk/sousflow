#include <iostream>
#include <chrono>
#include <thread>
void sleep(int duration_ms){ 
  std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
}
int main(int argc, char** argv){
    if(argc != 4){
        std::cerr << "Invalid number of commands" << std::endl;
        std::cerr << "task <count> <sleep_ms> <exit_code>"<< std::endl;
        return 1;
    }
    int count    = std::atoi(argv[1]);
    int duration = std::atoi(argv[2]);
    int exitcode = std::atoi(argv[3]);
    while(count != 0){
        std::cout << count << std::endl;
        sleep(duration);
        count--;
    }
    if(exitcode != 0){
        std::cerr << "Error occured" << std::endl;
    }
    return exitcode;
}