#include "compile_run.hpp"
#include <iostream>
#include <httplib.h>

int main()
{
    using namespace httplib;
    using namespace ns_util;

    Server svr;

    svr.set_base_dir("./wwwroot");
    
    svr.Post("/run", [](const Request & req, Response &res) {
        system("pwd");
        std::string in_json = req.body;
        std::string out_json;
        LOG_DEBUG("{}", req.body);
        CompileAndRun::Start(in_json, &out_json);       
        // LOG_DEBUG("{}", out_json);
        res.set_content(out_json, "application/json");
    });

    // 启动服务器
    
    svr.listen("0.0.0.0", 8080);

    return 0;
}