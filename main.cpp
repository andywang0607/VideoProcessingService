#include <iostream>
#include <string>

#include <pistache/common.h>
#include <pistache/cookie.h>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/http_headers.h>
#include <pistache/net.h>
#include <pistache/peer.h>

#include "nlohmann/json.hpp"
#include "src/Facade/videoservice.h"

using namespace Pistache;
using json = nlohmann::json;
using namespace std;

static VideoService service;

class MyHandler : public Http::Handler
{

    HTTP_PROTOTYPE(MyHandler)

    void onRequest(
        const Http::Request& req,
        Http::ResponseWriter response) override
    {

        if (req.resource() == "/open")
        {
            if (req.method() == Http::Method::Post)
            {
                cout << "open post start" <<endl;
                using namespace Http;
                json body = json::parse(req.body());
                std::string url = body["url"];
                cout << "url" << url <<endl;
                bool res = service.open(url.c_str());
                res ? response.send(Http::Code::Ok, "Ok") : response.send(Http::Code::Ok, "Failed");
            }
        }
        else if (req.resource() == "/start")
        {
            if (req.method() == Http::Method::Post)
            {
                service.start();
                response.send(Http::Code::Ok, req.body(), MIME(Text, Json));
            }

        }
        else if (req.resource() == "/stop")
        {
            if (req.method() == Http::Method::Post)
            {
                service.stop();
                response.send(Http::Code::Ok, "Ok");
            }
        }
        else if (req.resource() == "/getParamInt")
        {
            if (req.method() == Http::Method::Post)
            {
                using namespace Http;

                std::string target = req.body().data();
                cout << "getParamInt target: " << target;
                uint64_t result = service.getParameterInt((ParamInt)stoi(target));
                response.send(Http::Code::Ok, to_string(result));
            }
        }
        else if (req.resource() == "/getVideoRGBFrame")
        {
            if (req.method() == Http::Method::Post)
            {
                using namespace Http;
                auto data = service.getVideoRGBFrame();
                json res;
                if(data){
                     string rgb = string((*data).first.begin(), (*data).first.end());
                     int64_t pts = (*data).second;
                     res["rgb"] = rgb;
                     res["pts"] = pts;
                     response.send(Http::Code::Ok, res.dump());
                } else {
                    response.send(Http::Code::Ok, "null");
                }
            }
        }
        else if (req.resource() == "/getPcmData")
        {
            if (req.method() == Http::Method::Post)
            {
                using namespace Http;
                auto data = service.getPcmData();
                json res;
                if(data){
                     string pcm = string((*data).first.begin(), (*data).first.end());
                     int64_t pts = (*data).second;
                     res["pcm"] = pcm;
                     res["pts"] = pts;
                     response.send(Http::Code::Ok, res.dump());
                } else {
                    response.send(Http::Code::Ok, "null");
                }
            }
        }
        else if (req.resource() == "/exception")
        {
            throw std::runtime_error("Exception thrown in the handler");
        }
        else if (req.resource() == "/timeout")
        {
            response.timeoutAfter(std::chrono::seconds(2));
        }
        else
        {
            response.send(Http::Code::Not_Found);
        }
    }

    void onTimeout(
        const Http::Request& /*req*/,
        Http::ResponseWriter response) override
    {
        response
            .send(Http::Code::Request_Timeout, "Timeout")
            .then([=](ssize_t) {}, PrintException());
    }
};

int main(int argc, char* argv[])
{
    Port port(9080);

    int thr = 2;

    if (argc >= 2)
    {
        port = static_cast<uint16_t>(std::stol(argv[1]));

        if (argc == 3)
            thr = std::stoi(argv[2]);
    }

    Address addr(Ipv4::any(), port);

    auto server = std::make_shared<Http::Endpoint>(addr);

    auto opts = Http::Endpoint::options()
                    .threads(thr);
    server->init(opts);
    server->setHandler(Http::make_handler<MyHandler>());
    server->serve();
}
