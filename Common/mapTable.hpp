//
// Created by lang liu on 2024/7/1.
//

#ifndef CODETODEC_HPP
#define CODETODEC_HPP

#include <unordered_map>
#include <string>

namespace ns_table {
    // enum Language{
    //     CPP = 1,
    //     PYTHON = 2,
    //     JAVASCRIPT = 3,
    //     CSHARP = 4
    // };

    inline const std::unordered_map<std::string, std::string> langSuffixTable = {
        {"c_cpp", ".cc"},
        {"python", ".py"},
        {"javascript", ".js"},
        {"csharp", ".cs"}
    };



    // inline const static std::unordered_map<Language, std::string> langSuffix = {
    //     {CPP, ".cc"},
    //     {PYTHON, ".py"},
    //     {JAVASCRIPT, ".js"},
    //     {CSHARP, ".cs"}
    // };

    inline const std::unordered_map<int, std::string> errTable = {
        {-1, "未知错误"},
        {-2, "编译错误"},
        {SIGSEGV, "段错误（非法内存访问）"},
        {SIGABRT, "程序异常终止"},
        {SIGFPE, "算术错误（如除以零或浮点溢出）"},
        {SIGILL, "非法指令"},
        {SIGTERM, "终止信号"},
        {SIGINT, "中断信号（通常是用户发出的，如按下 Ctrl+C）"},
        {SIGBUS, "总线错误"},
        {SIGSYS, "无效的系统调用"},
        {SIGPIPE, "管道破裂"}
        // more
    };
}

#endif //CODETODEC_HPP
