#pragma once

#include "runner.hpp"
#include "compiler.hpp"
#include "util.hpp"
#include <json/json.h>

using namespace ns_log;
using namespace ns_util;
using namespace ns_compiler;

// json 输入格式
// code: 代码
// language: 编译器
// cpu_limit: CPU 限制
// mem_limit: 内存限制

// json 输出格式
// reason: 错误原因
// status: 状态码
// stderr: 错误输出
// stdout: 标准输出

class CompileAndRun
{
private:
    // 将状态码转换为描述信息
    static std::string codeToDesc(int status) {
        static std::unordered_map<int, std::string> errTable = {
            {-1, "未知错误"},
            {-2, "编译错误"},
            {0, "运行成功"},
            // 更多状态码描述
        };
        return errTable[status];
    }

public:
    // 编译并运行
    static void Start(std::string& in_json, std::string* out_json)
    {
        // 转换 JSON 格式
        Json::Value in_value;
        Json::Value out_value;
        Json::Reader reader;

        if (!reader.parse(in_json, in_value))
        {
            LOG_ERROR("CompileAndRun::Start parse json error");
            return;
        }

        // 记录输入的 JSON 数据
        Json::StyledWriter styledWriter;
        LOG_DEBUG("in_value: {}", styledWriter.write(in_value));

        // 从 JSON 中提取参数
        std::string code = in_value["code"].asString();
        int cpu_limit = in_value["cpu_limit"].asInt();
        int mem_limit = in_value["mem_limit"].asInt();
        std::string lang = in_value["language"].asString();

        int status_code = 0;

        // 初始化模板路径和后缀名
        PathUtil::InitTemplate(lang);
        std::string file_name = FileUtil::UniqFileName(); // 生成唯一文件名

        // 创建编译器和运行器对象
        auto Cp = CompilerFactory::CreateCompiler(lang);
        auto Rn = RunnerFactory::CreateRunner(lang);

        if(code.empty())
        {
            // 如果代码为空，记录错误并设置状态码
            LOG_ERROR("CompileAndRun::Start code is empty");
            status_code = -1;
            goto END;
        }

        // 将代码写入文件
        if(!FileUtil::WriteFile(PathUtil::Src(file_name), code))
        {
            status_code = -1; // 其他错误
            goto END;
        }

        // 编译代码
        if(Cp)
        {
            if(Cp->Compile(file_name) < 0) {
                status_code = -2; // 编译错误
                goto END;
            }
        }

        // 运行编译后的程序
        status_code = Rn->Run(file_name, cpu_limit, mem_limit);
        if(status_code < 0)
        {
            status_code = -1; // 未知错误
            goto END;
        }
        else if(status_code > 0)
        {
            goto END;
        }

    END:
        // 构建输出 JSON 数据
        out_value["status"] = status_code;
        out_value["reason"] = codeToDesc(status_code);
        if(status_code == 0)
        {
            std::string _stdout;
            FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout); // 读取标准输出
            out_value["stdout"] = _stdout;
        }
        else
        {
            std::string _stderr;
            FileUtil::ReadFile(PathUtil::Stderr(file_name), &_stderr); // 读取错误输出
            out_value["stderr"] = _stderr;
        }

        Json::StyledWriter writer;
        *out_json = writer.write(out_value); // 将结果写入输出 JSON
        LOG_DEBUG("out_json: {}", *out_json);

        // 清理生成的文件
        RemoveFile(file_name);
    }

private:
    // 移除生成的文件
    static void RemoveFile(const std::string& filename) {
        auto filepath = path + filename;
        auto filesrc = filepath + srcSuffix;
        auto fileexe = filepath + excuteSuffix;

        auto fileout = PathUtil::Stderr(filename);
        auto filein = PathUtil::Stdin(filename);
        auto fileerr = PathUtil::Stdout(filename);

        FileUtil::RemoveFile(filesrc);
        FileUtil::RemoveFile(fileexe);
        FileUtil::RemoveFile(fileout);
        FileUtil::RemoveFile(filein);
        FileUtil::RemoveFile(fileerr);
    }
};