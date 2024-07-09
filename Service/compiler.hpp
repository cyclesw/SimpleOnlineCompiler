#pragma once

#include <unistd.h>
#include <sys/fcntl.h>

#include "log.hpp"
#include "util.hpp"

namespace ns_compiler {
    using namespace ns_log;
    using namespace ns_util;

    // 编译器基类
    class Compiler {
    public:
        virtual ~Compiler() = default;

        // 编译函数，接受文件名作为参数
        int Compile(const std::string& file_name) {
            // 打开标准错误和标准输出文件
            int _stderr = open(PathUtil::Stderr(file_name).c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
            int _stdout = open(PathUtil::Stdout(file_name).c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);

            // 检查文件是否成功打开
            if(_stderr < 0 || _stdout < 0) {
                LOG_ERROR("打开文件失败");
                return -1;
            }

            // 创建子进程进行编译
            pid_t pid = fork();
            if(pid < 0 ) {
                LOG_ERROR("线程创建失败: {}", strerror(errno));
                return -1;
            }
            else if(pid == 0) {
                // 子进程
                umask(0); // 重置文件掩码

                // 重定向标准错误和标准输出到文件
                dup2(_stderr, STDERR_FILENO);
                dup2(_stdout, STDOUT_FILENO);

                // 执行编译命令
                Execute(file_name);
                LOG_INFO("exec错误：{}", strerror(errno));
                return -1; // exec 出错
            }

            // 父进程等待子进程结束
            waitpid(pid, nullptr, 0);

            // 检查编译后的可执行文件是否存在
            if(!FileUtil::IsFileExists(PathUtil::Exe(file_name))) {
                LOG_INFO("文件编译失败");
                return -2;
            }
            return 0; // 编译成功
        }

    protected:
        // 执行具体编译命令的纯虚函数，需由子类实现
        virtual void Execute(const std::string& filename) = 0;
    };

    // C++ 编译器类，继承自 Compiler
    class CppCompiler : public Compiler {
    protected:
        // 执行 g++ 编译命令
        void Execute(const std::string& filename) override {
            execlp("g++", "g++", "-o", PathUtil::Exe(filename).c_str(),
                PathUtil::Src(filename).c_str(), nullptr);
        }
    };

    // 编译器工厂类，用于创建编译器对象
    class CompilerFactory {
    public:
        // 根据语言创建相应的编译器对象
        static std::unique_ptr<Compiler> CreateCompiler(const std::string& lang) {
            if(lang == "c_cpp")
                return std::make_unique<CppCompiler>();
            // 如果需要支持其他语言，可以在这里添加相应的编译器创建逻辑
            else
                return {};
        }
    };
}
