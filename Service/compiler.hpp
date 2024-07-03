#pragma once

#include "log.hpp"
#include "util.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <mapTable.hpp>
#include <sys/wait.h>

using namespace ns_log;
using namespace ns_util;
using namespace ns_table;

//TODO 推倒重做
class Compiler {
public:
    static bool Compile(const std::string& file_name, const std::string& language)
    {
        pid_t pid = fork();
        if(pid < 0)
        {
            LOG_ERROR("fork error");
            return false;
        }
        else if(pid == 0)
        {
            // child process
            umask(0);
            // int _stderr = open(PathUtil::CompilerError(file_name).c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            int _stderr = open(PathUtil::Stderr(file_name).c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if(_stderr < 0)
            {
                LOG_ERROR("open error");
                exit(1);
            }
            dup2(_sterr, STDERR_FILENO);
            //TODO switch？ table？ other？？？？草，屎山代码
            LOG_ERROR("execlp error");
            exit(1);
        }
        else
        {
            // parent process
            waitpid(pid, nullptr, 0);
            if(FileUtil::IsFileExists(PathUtil::Exe(file_name) ) )
            {
                LOG_INFO("Compile success");
                return true;
            }
            LOG_INFO("Compile failed");
            return false;
        }

    }
};
