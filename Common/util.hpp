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

namespace ns_util
{
    using namespace ns_log;
    static inline std::unordered_map<std::string, std::string> suffixTable {
        {"c_cpp", ".cc"},
        {"csharp", ".cs"},
        {"python", ".py"},
        {"javascript", ".js"}
    };

    static inline std::unordered_map<std::string, std::string> excuteTable {
        {"c_cpp", ".exe"},
        {"csharp", ".cs"},
        {"javascript", ".js"},
        {"python", ".py"}
    };

    class TimeUtil
    {
    public:
        static std::string GetTimeStamp()
        {
            struct timeval _tv{};
            gettimeofday(&_tv, nullptr);
            return std::to_string(_tv.tv_sec);
        }

        static std::string GetTimeNs()
        {   //获取时间毫秒，用于生成随机性文件
            struct timeval _tv{};
            gettimeofday(&_tv, nullptr);
            return std::to_string(_tv.tv_sec * 1000 + _tv.tv_usec / 1000);
        }
    };

    static std::string path;
    static std::string srcSuffix;
    static std::string excuteSuffix;
    static const std::string temp_path = "./template/";
    //fixme basedir
    class PathUtil
    {
    public:
        static void InitTemplate(const std::string& lang) {
            path = temp_path + lang + "/";
            srcSuffix = suffixTable.at(lang);
            excuteSuffix = excuteTable.at(lang);
        }

        static std::string AddSuffix(const std::string &file_name, const std::string& suffix)
        {
            std::string path_name = path;
            path_name += file_name;
            path_name += suffix;
            return path_name;
        }

        static std::string Src(const std::string& file_name)
        {
            return AddSuffix(file_name, srcSuffix);
        }

        static std::string Exe(const std::string& file_name)
        {
            return AddSuffix(file_name, excuteSuffix);
        }

        static std::string CompilerError(const std::string& file_name)
        {
            return AddSuffix(file_name, ".compile_error");
        }

        static std::string Stdin(const std::string& file_name)
        {
            return AddSuffix(file_name, ".stdin");
        }

        static std::string Stdout(const std::string& file_name)
        {
            return AddSuffix(file_name, ".stdout");
        }

        static std::string Stderr(const std::string& file_name)
        {
            return AddSuffix(file_name, ".stderr");
        }
    };

    class FileUtil
    {
    private:
    public:
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

        static void RemoveFile(const std::string& filename) {
            if(std::filesystem::exists(filename)) {
                std::filesystem::remove(filename);
            }
        }

        static bool IsFileExists(const std::string & path_name)
        {
            struct stat st = {};
            if(stat(path_name.c_str(), &st) == 0)
                return true;

            return false;
        }

        static std::string UniqFileName()
        {
            static std::atomic_uint id(0);
            ++id;
            std::string ms = TimeUtil::GetTimeNs();
            std::string uniq_id = std::to_string(id);
            return ms + uniq_id;
        }

        static bool WriteFile(const std::string & path_name, const std::string & content)
        {   //将源代码写入文件
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
            auto size = in.tellg();
            in.seekg(0, std::ios::beg);

            content->resize(size);
            in.read((char*)content->c_str(), size);

//            while (std::getline(in, line))
//            {
//                *(content) += line;
//                (*content) += (keep ? "\n" : "");
//            }
            return true;
        }

    };

};


#endif //OJ_UTIL_HPP
