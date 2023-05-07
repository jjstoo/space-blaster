#ifndef SOCKET_H
#define SOCKET_H

#include "SDL2/SDL_net.h"
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class Socket {
  public:
    Socket();
    ~Socket();

    typedef std::shared_ptr<Socket> Ptr;

    ///
    /// \brief init initializes the instance to use given UDP port and packet
    /// size
    /// \param port port number
    /// \param packet_size packet size (max 1500 to avoid fragmentation)
    /// \return true if initialization succeeded
    ///
    bool init(uint16_t port, uint16_t packet_size);

    ///
    /// \brief recv read one incoming packet if available
    /// \param buffer string buffer to store packet data to
    /// \return true if a packet was received
    ///
    bool recv(uint8_t *buffer);

    ///
    /// \brief send sends data to specified channel or broadcasts to all bound
    /// channels
    /// \param data buffer to send
    /// \param len data length
    /// \param channel target channel. -1 (default) for broadcast.
    ///
    void send(uint8_t *data, int len, int channel = -1);

    ///
    /// \brief bind given host to a free channel
    /// \param host host address
    /// \param port target port
    ///
    int bind(char *host, uint16_t port);

  private:

    ///
    /// \brief resolveHost_ resolves host routing
    /// \param host hostname or address
    /// \param port UDP port
    /// \return IPaddress struct for the host
    ///
    IPaddress resolveHost_(char *host, uint16_t port);

    ///
    /// \brief broadcast_ sends data to all bound hosts
    /// \param data to send
    ///
    void broadcast_(UDPpacket *data);

    ///
    /// \brief send_ sends data to given channel
    /// \param data to send
    /// \param len data length
    /// \param channel UDP channel
    ///
    void send_(UDPpacket *data, int len, int channel);

    std::mutex socket_mutex_;
    UDPsocket sock_;
    UDPpacket *packet_;
    uint16_t port_;
    uint16_t packet_size_;
    bool bound_ = false;
    int channel_;
    std::vector<int> channels_;
};

#endif // SOCKET_H
