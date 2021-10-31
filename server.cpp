#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "controler.h"
#include "request.h"
#include "httprequestparser.h"
#include <boost/beast/http/message.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <vector>
#include <stdio.h>

using namespace std;
using namespace boost::asio;
using namespace boost::placeholders;

#define PORT 8080
#define BUFFER_SIZE 4096

typedef boost::system::error_code ec;

io_service service;

vector<string> split(string str, char delimiter);


class server
    : public boost::enable_shared_from_this<server>
{

    typedef server self_type;

    server() : skt(service), is_on(false){}

public:
    typedef boost::shared_ptr<server> pointer;
    

    void start(){
        is_on = true;
        do_read();
    }

    static pointer new_(){
        pointer new_(new server);
        return new_;
    }

    void stop(){
        if (!is_on) 
            return;
        is_on = false;
        skt.close();
    }

    ip::tcp::socket& sock() { 
        return skt; 
    }

    void on_read(const ec& err, size_t bytes){
        if (!err) {
            Request request;
            HttpRequestParser parser;
            HttpRequestParser::ParseResult res = parser.parse(request, request_buffer, request_buffer + sizeof(request_buffer));
            cout << request.inspect() << endl;

            vector<string> result = split(request.uri, '/');
            if (result.size() < 1) {
                stop();
                return; 
            }
            if(strcmp(result[1].c_str(), "table") == 0) { 
                UserControler uc = UserControler(request);
            } else { 
                stop();
                return;
            }

            // boost::beast::http::response <boost::beast::http::string_body> rp;
            // boost::beast::http::read(stream, rp);
            if( res == HttpRequestParser::ParsingCompleted )
            {

            }
            else
            {
                cout << "Parsing failed" << endl;
                stop();
            }
        }
        stop();
    }

    void on_write(const ec &err, size_t bytes){
        do_read();
    }

    void do_read(){
        skt.async_read_some(
            boost::asio::buffer(request_buffer, BUFFER_SIZE), 
            boost::bind(&server::on_read, shared_from_this(), _1, _2)
        );
    }



    void do_write(const std::string& msg){
        if (!is_on) return;
        std::copy(msg.begin(), msg.end(), write_buffer_);
        std::cout << write_buffer_ << '\n';
        // skt.async_write_some(boost::asio::buffer(write_buffer_, msg.size()), boost::bind(&server::on_write, shared_from_this(), _1, _2));
        boost::asio::async_write(skt, boost::asio::buffer(write_buffer_, BUFFER_SIZE), boost::bind(&server::on_write, shared_from_this(), _1, _2));
    }


private:

    ip::tcp::socket skt;
    char request_buffer[BUFFER_SIZE];
    char write_buffer_[BUFFER_SIZE];
    bool is_on;

};


ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), PORT));


void handle_accept(server::pointer s, const ec &err)
{
    s->start();
    server::pointer s2 = server::new_();
    acceptor.async_accept(s2->sock(), boost::bind(handle_accept, s2, _1));

}



int main(int argc, char* argv[])
{
    
    if (argc >= 3 && strcmp(argv[1], "controller") == 0) {
        
        FILE *in=fopen("generator.h","r");
        FILE *out=fopen(argv[2], "w");

        char line[1024];
        char *pLine;
 
        while(!feof(in)){
            pLine = fgets(line, 1024, in);
            fprintf(out, "%s", pLine);
        }
        fclose(in);
        fclose(out);
        return 0;

    } else {
        printf("실행됨");
        server::pointer s = server::new_();
        acceptor.async_accept(s->sock(), boost::bind(handle_accept, s, _1));
        service.run();
    }
    return 0;
}

vector<string> split(string input, char delimiter) { 
    vector<string> answer;
    stringstream ss(input);
    string temp;
 
    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }
 
    return answer;
}