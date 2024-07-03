#pragma once

#include <unistd.h>
#include <sys/fcntl.h>

#include "log.hpp"
#include "util.hpp"
#include "runner.hpp"


namespace ns_compier {
    using namespace ns_log;
    using namespace ns_util;

    static inline std::unordered_map<int, std::string> errTable {
        {}
    };

    class Compiler {
    public:
        virtual ~Compiler() = default;

        int Compie(const std::string& file_name) {
            int _stderr = open(PathUtil::Stderr(file_name).c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
            int _stdout = open(PathUtil::Stdout(file_name).c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);

            if(_stderr < 0 || _stdout < 0) {
                    LOG_ERROR("open error");
                    return -1;
            }

            pid_t pid = fork();
            if(pid < 0 ) {
                LOG_ERROR("线程创建失败:", strerror(errno));
                return -1;
            }
            else if(pid == 0) {
                umask(0);

                dup2(_stderr, STDERR_FILENO);
                dup2(_stdout, STDOUT_FILENO);

                Excute(file_name);
                LOG_INFO("exec错误：", strerror(errno));
                return -1;
            }

            waitpid(pid, nullptr, 0);
            if(!FileUtil::IsFileExists(PathUtil::Exe(file_name))) {
                LOG_INFO("文件编译失败");
                return -2;
            }
            return 0;
        }

    protected:
        virtual void Excute(const std::string& filename) = 0;
    };

    class CppCompiler : public Compiler{
    protected:
        void Excute(const std::string& filename) override {
            execlp("g++", "g++", "-o", PathUtil::Exe(filename).c_str(),
                PathUtil::Src(filename).c_str(), nullptr);
        }
    };

    class CompilerFactory {
    public:
        static std::unique_ptr<Compiler> CreateCompiler(const std::string& lang) {
            if(lang == "c_cpp")
                return std::make_unique<CppCompiler>();
            //更具是否需要编译增加
            else
                return {};
        }
    };
}