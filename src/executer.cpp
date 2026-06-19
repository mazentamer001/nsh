#include "executer.hpp"

#include <sys/wait.h>
#include <unistd.h>

#include <fcntl.h>

#include <iostream>

#include "builtins.hpp"

void Executer::execute(const std::vector<std::string>& tokens) {
  if (Builtins::handle(tokens)) {
    return;
  }

  bool background = false;

  //check for background execution
  if (!tokens.empty() && tokens.back() == "&") background = true;

  //check for output redirection
  int redirectIndex = -1;
  std::string outputFile;

  for (size_t i = 0; i < tokens.size(); i++) {

    //check for background execution symbol '&' not at the end of the command
      if(tokens[i] == "&" && i != tokens.size() - 1) {
          std::cerr << "background execution symbol '&' must be at the end of the command\n";
          return;
      }

      if (tokens[i] == ">") {
          redirectIndex = i;

          if (redirectIndex == tokens.size() - 1) {
            std::cerr << "missing output file\n";
            return;
          }

          outputFile = tokens[redirectIndex + 1];
          break;
      }
  }

  std::vector<const char*> argv;

  //if there is output redirection, we only want to pass the tokens before the ">"
  unsigned int limit;
  if(redirectIndex != -1) {
    limit = redirectIndex;
  } else if (background) {
    limit = tokens.size() - 1;
  } else {
    limit = tokens.size();
  }

 for (size_t i = 0; i < limit; i++) {
    argv.push_back(tokens[i].c_str());
 }

  argv.push_back(nullptr);

  pid_t pid = fork();

  if (pid < 0) {  // fork failed
    std::cerr << tokens[0] << ": failed to execute command" << std::endl;
  } else if (pid == 0) {  // child process

    std::cout << "DEBUG: before redirection\n";

    if (redirectIndex != -1) {
      std::cout << "DEBUG: redirect detected\n";
      int fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);  //this will give us a handle to the output file, creating it if it doesn't exist and truncating it if it does

      if (fd < 0) {
          std::cerr << "failed to open file\n";
          exit(EXIT_FAILURE);
      }

      dup2(fd, STDOUT_FILENO);  //this redirects the standard output to the file by duplicating the file descriptor to STDOUT_FILENO
      close(fd);

      std::cout << "DEBUG: after dup2\n";  //<-- this line will be printed to the output file, not the console
    }

    int status = execvp(argv[0], const_cast<char* const*>(argv.data()));

    if (status != 0) {
      std::string msg = "failed to execute command";

      if (errno == ENOENT) {
        msg = "command not found";
      }

      std::cerr << tokens[0] << ": " << msg << std::endl;
    }
  } else {  // parent process (pid > 0)
    if(!background)
      waitpid(pid, nullptr, 0);
  }
}
