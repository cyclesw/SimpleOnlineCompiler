//
// Created by lang liu on 24-4-23.
//

#ifndef OJ_UTIL_HPP
#define OJ_UTIL_HPP

#include "log.hpp"
#include <sys/time.h>
#include <sys/stat.h>
#include <fstream>
#include <atomic>
#include <unordered_map>
#include <filesystem>
#include <vector>

namespace ns_util
{
    using namespace ns_log;

    // 定义文件后缀名的映射表
    static inline std::unordered_map<std::string, std::string> suffixTable {
        {"c_cpp", ".cc"},
        {"csharp", ".cs"},
        {"python", ".py"},
        {"javascript", ".js"}
    };

    // 定义可执行文件后缀名的映射表
    static inline std::unordered_map<std::string, std::string> excuteTable {
        {"c_cpp", ".exe"},
        {"csharp", ".cs"},
        {"javascript", ".js"},
        {"python", ".py"}
    };

    // 时间工具类
    class TimeUtil
    {
    public:
        // 获取时间戳（秒）
        static std::string GetTimeStamp()
        {
            struct timeval _tv{};
            gettimeofday(&_tv, nullptr);    // 获取时间戳
            return std::to_string(_tv.tv_sec);
        }

        // 获取时间戳（毫秒），用于生成随机文件名
        static std::string GetTimeNs()
        {
            struct timeval _tv{};
            gettimeofday(&_tv, nullptr);
            return std::to_string(_tv.tv_sec * 1000 + _tv.tv_usec / 1000);
        }
    };

    // 文件路径和后缀名变量
    static std::string path;
    static std::string srcSuffix;
    static std::string excuteSuffix;
    static const std::string temp_path = "./template/";

    // 路径工具类
    class PathUtil
    {
    public:
        // 初始化模板路径和后缀名
        static void InitTemplate(const std::string& lang) {
            path = temp_path + lang + "/";
            srcSuffix = suffixTable.at(lang);
            excuteSuffix = excuteTable.at(lang);
        }

        // 添加后缀名到文件名
        static std::string AddSuffix(const std::string &file_name, const std::string& suffix)
        {
            std::string path_name = path;
            path_name += file_name;
            path_name += suffix;
            return path_name;
        }

        // 获取源文件路径
        static std::string Src(const std::string& file_name)
        {
            return AddSuffix(file_name, srcSuffix);
        }

        // 获取可执行文件路径
        static std::string Exe(const std::string& file_name)
        {
            return AddSuffix(file_name, excuteSuffix);
        }

        // 获取编译错误文件路径
        static std::string CompilerError(const std::string& file_name)
        {
            return AddSuffix(file_name, ".compile_error");
        }

        // 获取标准输入文件路径
        static std::string Stdin(const std::string& file_name)
        {
            return AddSuffix(file_name, ".stdin");
        }

        // 获取标准输出文件路径
        static std::string Stdout(const std::string& file_name)
        {
            return AddSuffix(file_name, ".stdout");
        }

        // 获取标准错误文件路径
        static std::string Stderr(const std::string& file_name)
        {
            return AddSuffix(file_name, ".stderr");
        }
    };

    // 文件工具类
    class FileUtil
    {
    public:
        // 移除文件夹中的所有文件（递归）
        static void RemoveAllFile(const std::string& dir) {
            std::vector<std::filesystem::path> removeArray;

            try {
                for(auto& it : std::filesystem::directory_iterator(dir)) {
                    if(std::filesystem::is_regular_file(it.path()))
                        removeArray.push_back(it.path());
                    else if (std::filesystem::is_directory(it.path()))
                        RemoveAllFile(it.path().string());
                }

                for(auto& it : removeArray) {
                    std::filesystem::remove(it);
                }
            }
            catch (const std::filesystem::filesystem_error& e) {
                LOG_ERROR("移除全部文件失败: {}", e.what());
            }
        }

        // 移除指定文件
        static void RemoveFile(const std::string& filename) {
            if(std::filesystem::exists(filename)) {
                std::filesystem::remove(filename);
            }
        }

        // 检查文件是否存在
        static bool IsFileExists(const std::string & path_name)
        {
            struct stat st = {};
            if(stat(path_name.c_str(), &st) == 0)
                return true;

            return false;
        }

        // 生成全局唯一的文件名
        static std::string UniqFileName()
        {
            static std::atomic_uint id(0);
            ++id;
            std::string ms = TimeUtil::GetTimeNs();
            std::string uniq_id = std::to_string(id);
            return ms + uniq_id;
        }

        // 将内容写入文件
        static bool WriteFile(const std::string & path_name, const std::string & content)
        {
            std::ofstream out(path_name);
            if(!out.is_open())
            {
                LOG_ERROR("write file failed!");
                return false;
            }
            out.write(content.c_str(), (int)content.size());
            out.close();
            return true;
        }

        // 读取文件内容
        static bool ReadFile(const std::string& path_name, std::string* content)
        {
            content->clear();

            std::ifstream in(path_name);
            if(!in.is_open())
            {
                LOG_ERROR("read file({}) failed!", path_name.c_str());
                return false;
            }
            in.seekg(0, std::ios::end);
            auto size = in.tellg(); // 获取文件大小
            in.seekg(0, std::ios::beg);

            content->resize(size);
            in.read(content->data(), size);

            return true;
        }
    };

};

#endif //OJ_UTIL_HPP