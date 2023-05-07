#include "socket.h"
#include "../blaster.h"
#include <memory>
#include <string>

Socket::Socket() {}

Socket::~Socket() {
    SDLNet_UDP_Close(sock_);
    SDLNet_Quit();
}

bool Socket::init(uint16_t port, uint16_t packet_size) {
    LOG("Opening socket on port %d", port);

    port_ = port;
    packet_size_ = packet_size;
    if (SDLNet_Init() < 0) {
        LOG("SDLNet_Init: %s\n", SDLNet_GetError());
        return false;
    }

    if (!(sock_ = SDLNet_UDP_Open(port_))) {
        LOG("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return false;
    }

    if (!(packet_ = SDLNet_AllocPacket(packet_size_))) {
        LOG("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

bool Socket::recv(uint8_t *buffer) {
    socket_mutex_.lock();
    if (SDLNet_UDP_Recv(sock_, packet_)) {
        socket_mutex_.unlock();
        buffer = packet_->data;
        return true;
    } else {
        socket_mutex_.unlock();
        return false;
    }
}

IPaddress Socket::resolveHost_(char *host, uint16_t port) {
    IPaddress destAddr;
    SDLNet_ResolveHost(&destAddr, host, port);
    return destAddr;
}

int Socket::bind(char *host, uint16_t port) {
    IPaddress destAddr = resolveHost_(host, port);
    socket_mutex_.lock();
    int channel = SDLNet_UDP_Bind(sock_, -1, &destAddr);

    if (channel == -1 && channels_.size() == 0) {
        LOG("Error binding on host %s", host);
        bound_ = false;
    } else {
        LOG("Bound to %s:%d on channel %d", host, port, channel);
        channels_.push_back(channel);
        bound_ = true;
    }

    socket_mutex_.unlock();
    return channel;
}

void Socket::send(uint8_t *data, int len, int channel) {
    UDPpacket *p = SDLNet_AllocPacket(packet_size_);
    p->data = data;
    p->len = len;

    if (channel == -1)
        broadcast_(p);
    else
        send_(p, len, channel);

    SDLNet_FreePacket(p);
}

void Socket::send_(UDPpacket *data, int len, int channel) {
    socket_mutex_.lock();
    if (SDLNet_UDP_Send(sock_, channel, data) == 0) {
        LOG("SDLNet_UDP_SendV: %s\n", SDLNet_GetError());
    }
    socket_mutex_.unlock();
}

void Socket::broadcast_(UDPpacket *data) {
    socket_mutex_.lock();
    for (auto host : channels_) {
        if (SDLNet_UDP_Send(sock_, host, data) == 0) {
            LOG("SDLNet_UDP_SendV: %s\n", SDLNet_GetError());
        }
    }
    socket_mutex_.unlock();
}
