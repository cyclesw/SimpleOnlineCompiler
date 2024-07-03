//
// Created by lang liu on 24-4-23.
//

#ifndef _OJ_UTIL_HPP
#define _OJ_UTIL_HPP

#include "log.hpp"
#include <sys/time.h>
#include <sys/stat.h>
#include <fstream>
#include <atomic>
#include <mapTable.hpp>

namespace ns_util
{
    using namespace ns_log;
    using namespace ns_table;

    class TimeUtil
    {
    public:
        static std::string GetTimeStamp()
        {
            struct timeval _tv;
            gettimeofday(&_tv, nullptr);
            return std::to_string(_tv.tv_sec);
        }

        static std::string GetTimeNs()
        {   //获取时间毫秒，用于生成随机性文件
            struct timeval _tv;
            gettimeofday(&_tv, nullptr);
            return std::to_string(_tv.tv_sec * 1000 + _tv.tv_usec / 1000);
        }
    };


    //fixme basedir
    class PathUtil
    {
    private:
        static std::string path;
        static std::string srcSuffix;
        static const std::string temp_path;
    public:
        static void GetTemplatePath(const std::string& lang) {
            path = temp_path + lang;
            srcSuffix = langToSuffix(lang);
        }

        static std::string langToSuffix(const std::string& lang) {
            return langSuffixTable.at(lang);
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
            return AddSuffix(file_name, ".exe");
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

    const std::string PathUtil::temp_path = "./template";

    class FileUtil
    {
    public:
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


#endif //_OJ_UTIL_HPP
