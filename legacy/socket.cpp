#include "socket.h"
#include "boost/asio.hpp"
using namespace boost::asio;
int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr)
{
    int result;
    addrinfo* result_list = NULL;
    addrinfo hints = {};
    hints.ai_family = family;
    hints.ai_socktype = SOCK_DGRAM; // without this flag, getaddrinfo will return 3x the number of addresses (one for each socket type).
    result = getaddrinfo(hostname, service, &hints, &result_list);
    if (result == 0)
    {
        //ASSERT(result_list->ai_addrlen <= sizeof(sockaddr_in));
        memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
        freeaddrinfo(result_list);
    }

    return result;
}void errno_abort(const char* header)
{
    perror(header);
    exit(EXIT_FAILURE);
}
void sendPacket2(std::string packet, std::string addr, std::string port){
    
io_service io_service;
ip::udp::socket socket(io_service);
ip::udp::endpoint remote_endpoint;

socket.open(ip::udp::v4());

remote_endpoint = ip::udp::endpoint(ip::address::from_string("192.168.1.230"), 56700);

boost::system::error_code err;
socket.send_to(buffer(packet,packet.length()), remote_endpoint, 0, err);

socket.close();

std::cout << err << std::endl;

}
void sendPacket(std::string packet, std::string addr, std::string port)
{

    int result = 0;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    char szIP[100];

    sockaddr_in addrListen = {}; // zero-int, sin_port is 0, which picks a random port for bind.
    addrListen.sin_family = AF_INET;
    result = bind(sock, (sockaddr*)&addrListen, sizeof(addrListen));
    if (result == -1)
    {
       int lasterror = errno;
       std::cout << "error: " << lasterror;
       exit(1);
    }


    sockaddr_storage addrDest = {};
    result = resolvehelper(addr.c_str(), AF_INET, port.c_str(), &addrDest);
    if (result != 0)
    {
       int lasterror = errno;
       std::cout << "error: " << lasterror;
       exit(1);
    }
    int recvBufSize = packet.length()*2;
    int err = setsockopt(sock, SOL_SOCKET, SO_RCVBUF,
                         (const char*)&recvBufSize, sizeof(recvBufSize));

    std::cout << err << "\n\n";
    std::cout << addr.c_str() << "\n\n";
    std::cout << port.c_str() << "\n\n";
    std::cout << packet << "\n\n";
    std::cout << packet.length() << "\n\n";
    std::cout << strlen(packet.c_str()) << "\n\n";

    result = sendto(sock, packet.c_str(), packet.length()+1, 0, (sockaddr*)&addrDest, sizeof(addrDest));

    std::cout << result << " bytes sent" << std::endl;

}
