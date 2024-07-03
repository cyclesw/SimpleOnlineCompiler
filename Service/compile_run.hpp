#pragma once

#include "runner.hpp"
#include "compiler.hpp"
#include "mapTable.hpp"
#include <json/json.h>

using namespace ns_log;
using namespace ns_util;

//json in:
// code: 代码
// language: 编译器：
// compiled：是否需要编译
// cpu_limit:
// mem_limit:

//json out:
// reason: 错误原因
// status: 状态码
// stderr: 错误输出
// stdout: 标准输出



class CompileAndRun
{
private:
    static std::string codeToDesc(int status) {
        return errTable.at(status);
    }


public:
    //TODO 增加可维护性
    // 编译并运行
    static void RemoveFile(const std::string& file_name)
    {
        std::string cmd = "rm -f ./temp/" + file_name + "*"; 
        system(cmd.c_str());
    }

    static void Start(std::string& in_json, std::string* out_json)
    {
        // 转格式
        Json::Value in_value;
        Json::Value out_value;
        Json::Reader reader;

        if (!reader.parse(in_json, in_value))
        {
            LOG_ERROR("CompileAndRun::Start parse json error");
            return;
        }
        Json::StyledWriter styledWriter;
        LOG_DEBUG("in_value: {}", styledWriter.write(in_value));
        std::string code = in_value["code"].asString();
        int cpu_limit = in_value["cpu_limit"].asInt();
        int mem_limit = in_value["mem_limit"].asInt();
        std::string lang = in_value["language"].asString();
        bool isCompiled = in_value["compiled"].asBool();
        int status_code = 0;
        PathUtil::GetTemplatePath(lang);
        std::string file_name = FileUtil::UniqFileName();

        if(code.empty())
        {
            LOG_ERROR("CompileAndRun::Start code is empty");
            status_code = -1;
            goto END;
        }

        if(!FileUtil::WriteFile(PathUtil::Src(file_name), code))
        {
            status_code = -1;   //其他错误
            goto END;
        }

        if(isCompiled && !Compiler::Compile(file_name, lang))
        {
            status_code = -2;   //编译错误
            goto END;
        }

        status_code = Runner::Run(file_name, cpu_limit, mem_limit, lang);
        // LOG_INFO("CompileAndRun::Start status_code: {}", status_code);
        if(status_code < 0)
        {
            status_code = -1;   //未知错误
            goto END;
        }
        else if(status_code > 0)
        {
            goto END;
        }

    END:
        out_value["status"] = status_code;
        out_value["reason"] = codeToDesc(status_code);
        if(status_code == 0)
        {
            std::string _stdout;
            FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout);
            out_value["stdout"] = _stdout;
        }
        else
        {
            std::string _stderr;
            FileUtil::ReadFile(PathUtil::Stderr(file_name), &_stderr);
            out_value["stderr"] = _stderr;
        }
       
        Json::StyledWriter writer;
        *out_json = writer.write(out_value);
        LOG_DEBUG("out_json: {}", *out_json);
        // RemoveFile(file_name);   //release
    }

};