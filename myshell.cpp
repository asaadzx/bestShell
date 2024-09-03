#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <filesystem>

// Function declarations
void executeCommand(const std::vector<std::string>& args);
void view_Directory(const std::filesystem::path& path);

int main() {
    std::string user_input;
    std::vector<std::string> args;
    std::filesystem::path current_path = std::filesystem::current_path();

    while (true) {
        std::cout << ">>> ";  // Shell prompt
        std::getline(std::cin, user_input);  // Get user input

        view_Directory(current_path); // To view the where are u

        if (user_input.empty()) {
            continue;
        }

        // Parse input
        std::istringstream iss(user_input);
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }

        // Check for exit command
        if (args[0] == "exit") {
            break;
        } else if (user_input == "up") {
            current_path = current_path.parent_path();
        } else if (user_input == "help") {
            std::cout << "to exit type 'exit' ";
        }else {
            std::filesystem::path new_path = current_path / user_input;
            if (std::filesystem::exists(new_path) && std::filesystem::is_directory(new_path)) {
                current_path = new_path;
            } else {
                std::cout << "Invalid directory.\n";
            }
        }

        // Execute command
        executeCommand(args);

        // Clear arguments for the next input
        args.clear();
    }

    return 0;
}

void executeCommand(const std::vector<std::string>& args) {
    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Failed to fork process" << std::endl;
    } else if (pid == 0) {
        // Convert vector to array of C-strings
        std::vector<char*> argv(args.size() + 1);
        for (size_t i = 0; i < args.size(); ++i) {
            argv[i] = const_cast<char*>(args[i].c_str());
        }
        argv[args.size()] = nullptr;

        // Execute command
        if (execvp(argv[0], argv.data()) == -1) {
            std::cerr << "Command execution failed" << std::endl;
        }
        exit(EXIT_FAILURE);  // Exit child process if execvp fails
    } else {
        int status;
        waitpid(pid, &status, 0);  // Wait for child process to finish
    }
}

void view_Directory(const std::filesystem::path& path) {
    std::cout << "Content of " << path << ":\n";
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::cout << (entry.is_directory() ? "[DIR]" : "    ")
                  << entry.path().filename().string() << '\n';
    }
}
