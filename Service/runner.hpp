#pragma once

#include "compiler.hpp"
#include <cstring>
#include <sys/resource.h>
#include <sys/wait.h>


using namespace ns_log;
using namespace ns_util;

class Runner {
public:
    int Run(const std::string& filename, const int cpu_limit,
            const int mem_limit) {
        std::string _execute = PathUtil::Exe(filename);
        std::string _stdin = PathUtil::Stdin(filename);
        std::string _stdout = PathUtil::Stdout(filename);
        std::string _stderr = PathUtil::Stderr(filename);

        umask(0);
        int stdin_fd = open(_stdin.c_str(), O_CREAT | O_WRONLY, 0644);
        int stdout_fd = open(_stdout.c_str(), O_CREAT | O_WRONLY, 0644);
        int stderr_fd = open(_stderr.c_str(), O_CREAT | O_WRONLY, 0644);

        pid_t pid = fork();
        if(pid < 0) {
            LOG_ERROR("fork error");
            close(stdin_fd);
            close(stdout_fd);
            close(stderr_fd);
            return -1;
        }
        else if(pid == 0) {
            dup2(stdin_fd, STDIN_FILENO);
            dup2(stdout_fd, STDOUT_FILENO);
            dup2(stderr_fd, STDERR_FILENO);
            SetProcLimit(mem_limit);
            Excute(_execute);
            LOG_ERROR("execute error");
            exit(-1);
        }

        int status;
        waitpid(pid, &status, 0);
        close(stderr_fd);
        close(stdout_fd);
        close(stdin_fd);

        return WEXITSTATUS(status);
    }
    virtual  ~Runner() = default;
protected:
    virtual void Excute(const std::string& filename) = 0;

    static void SetProcLimit(const int m_limit) {
        struct rlimit memLimit{};
        memLimit.rlim_cur = m_limit * 1024;
        memLimit.rlim_max = RLIM_INFINITY;
        setrlimit(RLIMIT_AS, &memLimit);
    }

};

class CppRunner : public Runner {
protected:
    void Excute(const std::string& exec) override {
        execlp(exec.c_str(), exec.c_str(), nullptr);
    }
};

class CsharpRunner : public Runner {
protected:
    void Excute(const std::string &filename) override {
        execlp("dotnet", "dotnet", "run",  filename.c_str(), "--project", path.c_str(), nullptr);
    }
};

class JsRunner : public Runner {
protected:
    void Excute(const std::string &filename) override {
        execlp("node", "node", filename.c_str(), nullptr);
    }
};

class PyRunner : public Runner {
protected:
    void Excute(const std::string &filename) override {
        execlp("python3", "python3", filename.c_str(), nullptr);
    }
};

class RunnerFactory {
public:
    static std::unique_ptr<Runner> CreateRunner(const std::string& language) {
        if(language == "c_cpp")
            return std::make_unique<CppRunner>();
        else if (language == "csharp")
            return std::make_unique<CsharpRunner>();
        else if(language == "python")
            return std::make_unique<PyRunner>();
        else
            return std::make_unique<JsRunner>();
    }
};