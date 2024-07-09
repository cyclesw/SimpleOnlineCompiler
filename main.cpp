#include "compile_run.hpp"
#include <httplib.h>


int main()
{
    using namespace httplib;
    using namespace ns_util;



    // system("pwd");

    Server svr;

    svr.set_base_dir("./wwwroot");
    
    svr.Post("/run", [](const Request & req, Response &res) {
        std::string in_json = req.body;
        std::string out_json;
        CompileAndRun::Start(in_json, &out_json);
        res.set_content(out_json, "application/json");
    });

    // 启动服务器
    
    svr.listen("0.0.0.0", 8080);

    return 0;
}