#include "utils.hpp"
#include "Request.hpp"
#include "config/Config.hpp"
#include <iostream>
#include <arpa/inet.h>

// int main()
// {
//     // std::string a = "hi:";

//     // std::pair<std::string, std::string> res;

//     // res = parseHeader(a);

//     // std::cout << "first= " << res.first << ", second= " << res.second << std::endl;

//     try {
//         Request req;
        
//         req.parseRequest("POST / HTTP/1.1\r\nJump-secret: don't tell\r\nContent-Type: application/json\r\nUser-Agent: PostmanRuntime/7.29.0\r\nAccept: */*\r\nPostman-Token: 7375e222-013c-4ec1-9877-c4d552d31893\r\nHost: localhost:9999\r\nAccept-Encoding: gzip, deflate, br\r\nConnection: keep-alive\r\nContent-Length: 34\r\n\r\n{\n    \"A\": \"hi\",\n    \"B\": \"bye\"\n}\n");
//         req.printRequest();
//     }
//     catch (std::exception &exception){
//             std::cout << exception.what() <<std::endl;
//     }
// }

int main()
{

    try 
    {
        Config c("configs/default.conf");

        c.printConfigInfo();
    }
    catch(std::string error) 
    {  
        std::cout << "error: " << error << std::endl;
    }
    

    // std::string content = c.getContent();

    // std::cout<< content <<std::endl;

    // std::string block;

    // int index = 0;

    // while (ft_is_white_space(content[index]))
    // {
    //     index++;
    // }

    // std::cout << "index = " << index <<std::endl;
    // block = findBlock(content.substr(index, content.length()), "location");
    
    // if (block.length() == 0){
    //     std::cout << "get block fail" << std::endl;
    // }

    // std::cout << "block = " << block << std::endl;
    // std::cout << "content = " << content << content.length() << std::endl;
}