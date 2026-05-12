#include "mantis/driver_impl/virtual/socket/net_utils/msocket.h"
#include "mantis/lib/errors/exceptions.h"
#include "mantis/lib/utilities/prints.h"
#include <cstring>
#include <netinet/in.h>

constexpr int MAX_CONNECTIONS_IN_QUEUE = 1;

mantis::net::msocket::msocket(params::msocket_params _sock_params) : sock_params(std::move(_sock_params)) {
    this->sockfd = socket(this->sock_params.domain, this->sock_params.type, this->sock_params.protocol);
    if (this->sockfd < 0) {
        throw mantis::runtime_error("Failed to create socket with: " + std::to_string(this->sock_params.domain) + ", " +
                                    std::to_string(this->sock_params.type) + ", " +
                                    std::to_string(this->sock_params.protocol));
    }

    memset(&this->sockaddr, '\0', sizeof(this->sockaddr));
}

mantis::net::msocket::~msocket() {
    if (this->sockfd >= 0) {
        ::close(this->sockfd);
    }
}

mantis::errors::error_code mantis::net::msocket::connect() {
    this->sockaddr.sin_family = this->sock_params.domain;
    this->sockaddr.sin_port = htons(this->sock_params.port);

    if (inet_pton(this->sock_params.domain, this->sock_params.host.c_str(), &this->sockaddr.sin_addr.s_addr) <= 0) {
        perror("socket address conversion failed");
        throw mantis::runtime_error("Failed to convert host: " + this->sock_params.host);
    }

    socklen_t addrlen = sizeof(this->sockaddr);
    if (::connect(this->sockfd, (struct sockaddr*)&this->sockaddr, addrlen) < 0) {
        return mantis::errors::error_code::SOCKET_CONECTION_FAILURE;
    };

    return mantis::errors::error_code::SUCCESS;
}

#define MAX_UDP_SIZE 65000

mantis::errors::error_code mantis::net::msocket::send(const char* data, size_t size) {
    size_t total_bytes_sent{0};
    ssize_t bytes_sent{0};
    while (total_bytes_sent < size) {
        int bytes_to_send = this->sock_params.type == SOCK_STREAM      ? size
                            : (size - total_bytes_sent) > MAX_UDP_SIZE ? MAX_UDP_SIZE
                                                                       : size - total_bytes_sent;
        bytes_sent = ::send(this->sockfd, data + total_bytes_sent, bytes_to_send, this->sock_params.flags);
        if (bytes_sent <= 0 && total_bytes_sent == 0) {
            perror("Total send failure");
            return mantis::errors::error_code::TOTAL_SOCKET_SEND_FAILURE;
        }

        if (bytes_sent <= 0) {
            perror("partial send failure");
            return mantis::errors::error_code::PARTIAL_SOCKET_SEND_FAILURE;
        }

        total_bytes_sent += bytes_sent;
    }

    return mantis::errors::error_code::SUCCESS;
}

mantis::errors::error_code mantis::net::msocket::bind() {
    this->sockaddr.sin_family = this->sock_params.domain;
    this->sockaddr.sin_port = htons(this->sock_params.port);

    this->sockfd = ::socket(this->sock_params.domain, this->sock_params.type, this->sock_params.flags);
    if (this->sockfd < 0) {
        perror("Failed to get fd");
        return errors::error_code::FAILED_TO_GET_SOCKET_FD;
    }

    if (inet_pton(this->sock_params.domain, this->sock_params.host.c_str(), &this->sockaddr.sin_addr.s_addr) <= 0) {
        perror("socket address conversion failed");
        throw mantis::runtime_error("Failed to convert host: " + this->sock_params.host);
    }

    socklen_t addrlen = sizeof(sockaddr);
    if (::bind(this->sockfd, (struct sockaddr*)&this->sockaddr, addrlen) < 0) {
        perror("bind failed");
        ::close(this->sockfd);
        this->sockfd = -1;
        return errors::error_code::FAILED_TO_BIND;
    }

    if (this->sock_params.type == SOCK_STREAM) {
        if (::listen(this->sockfd, MAX_CONNECTIONS_IN_QUEUE) < 0) {
            perror("listen");
            ::close(this->sockfd);
            this->sockfd = -1;
            return errors::error_code::FAILED_TO_LISTEN;
        }
    }

    auto err = this->accept();
    while (!errors::succeeded(err)) {
        utils::perror("Failed to accept client. Retrying...");
        err = this->accept();
    }
    return mantis::errors::error_code::SUCCESS;
}

mantis::errors::error_code mantis::net::msocket::accept() {
    memset(&this->client_fd, '\0', sizeof(this->client_sockaddr));

    socklen_t client_len = sizeof(this->client_sockaddr);

    if (this->sock_params.type == SOCK_STREAM) {
        utils::pinfo("Awaiting TCP connection...");
        int new_client_fd = ::accept(this->sockfd, (struct sockaddr*)&client_sockaddr.sin_addr, &client_len);
        if (new_client_fd < 0) {
            perror("Accept failed");
            return errors::error_code::FAILED_TO_ACCEPT_CLIENT;
        }

        char client_ip[INET_ADDRSTRLEN]; // TODO hardcoded to ipv4
        inet_ntop(AF_INET, &client_sockaddr.sin_addr, client_ip, sizeof(client_ip));

        mantis::utils::pinfo(std::string("Accepted Connection from ip: ") + client_ip);
        this->client_fd = new_client_fd;
    } else if (this->sock_params.domain == SOCK_DGRAM) {
        this->client_fd = this->sockfd;
    }
    return mantis::errors::error_code::SUCCESS;
}

mantis::errors::error_code mantis::net::msocket::recv(char* data, size_t size) {
    size_t bytes_needed = size;
    while (bytes_needed > 0) {
        ssize_t bytes_to_recv;
        if (this->sock_params.type == SOCK_STREAM) { // receive whole buff when using tcp, max size when udp
            bytes_to_recv = bytes_needed;
        } else {
            bytes_to_recv = (bytes_needed > MAX_UDP_SIZE) ? MAX_UDP_SIZE : bytes_needed;
        }

        ssize_t bytes_received =
            ::recv(this->client_fd, data + (size - bytes_needed), bytes_to_recv, this->sock_params.flags);

        if (bytes_received == 0) {
            mantis::utils::pwarn("Client disconnected. Attempting client reconnect...");
            auto err = this->accept();
            if (!errors::succeeded(err)) {
                return err;
            }
            continue;
        }
        
        // TODO: may be better to put the accept() outside in the channel and 
        // just return an error here, also consider desired partial failure behaviour
        // atm you just dont get the data, this 
        if (bytes_received < 0) {
            if (bytes_needed != size) {
                perror("total recv failure");
            } else {
                perror("partial recv failure");
                return mantis::errors::error_code::PARTIAL_SOCKET_RECV_FAILURE;
            }
            mantis::utils::pwarn("Something went wrong in msocket::recv. Attempting client reconnect...");
            auto err = this->accept();
            if (!errors::succeeded(err)) {
                return err;
            }
            continue;
        }

        bytes_needed -= bytes_received;
    }
    return mantis::errors::error_code::SUCCESS;
}