#pragma once

#include "compiler.hpp"
#include <iostream>
#include <sys/resource.h>
#include <sys/wait.h>

using namespace ns_log;
using namespace ns_util;

// Runner 基类，负责运行已编译的程序
class Runner {
public:
    // 运行指定的程序文件，并限制其 CPU 和内存使用
    int Run(const std::string& filename, const int cpu_limit, const int mem_limit) {
        // 获取输入、输出和错误文件路径
        std::string _execute = PathUtil::Exe(filename);
        std::string _stdin = PathUtil::Stdin(filename);
        std::string _stdout = PathUtil::Stdout(filename);
        std::string _stderr = PathUtil::Stderr(filename);

        umask(0); // 重置文件掩码
        // 打开标准输入、输出和错误文件
        int stdin_fd = open(_stdin.c_str(), O_CREAT | O_WRONLY, 0644);
        int stdout_fd = open(_stdout.c_str(), O_CREAT | O_WRONLY, 0644);
        int stderr_fd = open(_stderr.c_str(), O_CREAT | O_WRONLY, 0644);

        // 创建子进程以执行程序
        pid_t pid = fork();
        if(pid < 0) {
            LOG_ERROR("fork error");
            close(stdin_fd);
            close(stdout_fd);
            close(stderr_fd);
            return -1;
        }
        else if(pid == 0) {
            // 子进程
            dup2(stdin_fd, STDIN_FILENO); // 重定向标准输入到文件
            dup2(stdout_fd, STDOUT_FILENO); // 重定向标准输出到文件
            dup2(stderr_fd, STDERR_FILENO); // 重定向标准错误到文件

            signal(SIGXCPU, [](int sig) { // 处理超时信号
                std::cerr << "timeout" << std::endl;
            });

            SetProcLimit(mem_limit, cpu_limit); // 设置进程资源限制
            Excute(_execute); // 执行具体的程序
            LOG_ERROR("execute error");
            exit(-1); // exec 出错
        }

        // 父进程等待子进程完成
        int status;
        waitpid(pid, &status, 0);
        close(stderr_fd);
        close(stdout_fd);
        close(stdin_fd);

        return WEXITSTATUS(status); // 返回子进程的退出状态
    }

    virtual ~Runner() = default;

protected:
    // 纯虚函数，由子类实现具体的执行逻辑
    virtual void Excute(const std::string& filename) = 0;

    // 设置进程的内存和 CPU 使用限制
    static void SetProcLimit(const int m_limit, const int c_limit) {
        rlimit memLimit{};
        memLimit.rlim_cur = m_limit * 1024; // 内存限制（KB 转换为字节）
        memLimit.rlim_max = RLIM_INFINITY; // 最大内存限制
        setrlimit(RLIMIT_AS, &memLimit);

        rlimit cpuLimit{};
        cpuLimit.rlim_cur = c_limit; // CPU 时间限制（秒）
        cpuLimit.rlim_max = RLIM_INFINITY; // 最大 CPU 时间限制
        setrlimit(RLIMIT_CPU, &cpuLimit);
    }
};

// C++ 运行器类，继承自 Runner
class CppRunner : public Runner {
protected:
    // 使用 execlp 执行编译后的 C++ 程序
    void Excute(const std::string& exec) override {
        execlp(exec.c_str(), exec.c_str(), nullptr);
    }
};

// C# 运行器类，继承自 Runner
class CsharpRunner : public Runner {
protected:
    // 使用 execlp 执行 C# 程序
    void Excute(const std::string &filename) override {
        execlp("dotnet", "dotnet", "run",  filename.c_str(), "--project", path.c_str(), nullptr);
    }
};

// JavaScript 运行器类，继承自 Runner
class JsRunner : public Runner {
protected:
    // 使用 execlp 执行 JavaScript 程序
    void Excute(const std::string &filename) override {
        execlp("node", "node", filename.c_str(), nullptr);
    }
};

// Python 运行器类，继承自 Runner
class PyRunner : public Runner {
protected:
    // 使用 execlp 执行 Python 程序
    void Excute(const std::string &filename) override {
        execlp("python3", "python3", filename.c_str(), nullptr);
    }
};

// 运行器工厂类，用于创建运行器对象
class RunnerFactory {
public:
    // 根据语言创建相应的运行器对象
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
