#pragma once

#include "compiler.hpp"
#include <cstring>
#include <sys/resource.h>


using namespace ns_log;
using namespace ns_util;

class Runner
{
public:
        static void SetProcLimit(const int c_limit, const int m_limit)
        {
            struct rlimit cpu_limit{};
            cpu_limit.rlim_max = RLIM_INFINITY;
            cpu_limit.rlim_cur = c_limit;
            setrlimit(RLIMIT_CPU, &cpu_limit);

            struct rlimit mem_limit{};
            mem_limit.rlim_max = RLIM_INFINITY;
            mem_limit.rlim_cur = m_limit * 1024;    // kb
            setrlimit(RLIMIT_AS, &mem_limit);
        }

        static int Run(const std::string& file_name, const int cpu_limit,
                    const int mem_limit, const int option)
        {
            std::string _execute = PathUtil::Exe(file_name);
            std::string _stdin = PathUtil::Stdin(file_name);
            std::string _stdout = PathUtil::Stdout(file_name);
            std::string _stderr = PathUtil::Stderr(file_name);

            umask(0);
            int stdin_fd = open(_stdin.c_str(), O_CREAT | O_WRONLY, 0644);
            int stdout_fd = open(_stdout.c_str(), O_CREAT | O_WRONLY, 0644);
            int stderr_fd = open(_stderr.c_str(), O_CREAT | O_WRONLY, 0644);

            pid_t pid = fork();
            if(pid < 0)
            {
                LOG_ERROR("fork error->{}", strerror(errno));
                close(stdin_fd);
                close(stdout_fd);
                close(stderr_fd);
                return -1;  // fork error
            }
            else if(pid == 0)
            {
                // child
                dup2(stdin_fd, STDIN_FILENO);
                dup2(stdout_fd, STDOUT_FILENO);
                dup2(stderr_fd, STDERR_FILENO);

                SetProcLimit(cpu_limit, mem_limit);

                Choose(_execute, option);

                LOG_ERROR("exec error->{}", strerror(errno));
                exit(-1);
                // set time limit
            }
            else
            {
                // parent
                int status;
                waitpid(pid, &status, 0);
                close(stdin_fd);
                close(stdout_fd);
                close(stderr_fd);
                return WEXITSTATUS(status);
            }
        }
private:
    static void Choose(const std::string& exec, const int option) {
        switch (option) {
            case CPP:
                execlp(exec.c_str(), exec.c_str(), NULL);
                break;
            case PYTHON:
                execlp("python3", exec.c_str(), NULL);
                break;
            case JAVASCRIPT:
                execlp("node", exec.c_str(), NULL);
                break;
            case CSHARP:
                execlp("dotnet", "run", exec.c_str());
        }

    }
};