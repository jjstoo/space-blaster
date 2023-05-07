#include "messagehandler.h"
#include "buffers/schema_generated.h"

MessageHandler::MessageHandler(int port, int host_port, char *host_address,
                               MultiplayerRole multiplayerRole) {
    multiplayerRole_ = multiplayerRole;
    socket_.reset(new Socket());
    socket_->init(static_cast<uint16_t>(port), PACKET_SIZE);

    if (host_address) {
        host_channel_ =
            socket_->bind(host_address, static_cast<uint16_t>(host_port));
    }
}

std::vector<Blaster::Messages::Point>
MessageHandler::getFlatOutline_(Polygon *source) {
    // Tranform ship outline to vector of "Point" structs
    std::vector<Blaster::Messages::Point> outline;
    for (int i = 0; i < source->points; i++) {
        outline.push_back(
            Blaster::Messages::Point{static_cast<int>(source->outline[i].x),
                                     static_cast<int>(source->outline[i].x)});
    }

    return outline;
}

Blaster::Messages::Color MessageHandler::getFlatColor_(Polygon *source) {
    SDL_Color color = source->getColor();
    return Blaster::Messages::Color{color.r, color.g, color.b, color.a};
}

MessageBuffer MessageHandler::buildShipMessage(Ship *ship) {
    // Initialize builder (memory allocation will increase if needed)
    MessageBuffer builder(1024);

    // Save outline to serializable variable
    auto _outline =
        builder.CreateVectorOfStructs(getFlatOutline_(ship->getBody()));

    // Save position
    Blaster::Messages::Point _pos{static_cast<int>(ship->getPosX()),
                                  static_cast<int>(ship->getPosY())};

    // Color
    Blaster::Messages::Color _color = getFlatColor_(ship->getBody());

    // Render mode for ship
    Blaster::Messages::RenderMode _renderMode =
        Blaster::Messages::RenderMode_Polygon;

    // Create serializable message
    auto shipMsg = Blaster::Messages::CreateVisualObject(
        builder, &_pos, _outline, _renderMode, &_color, ship->getVelX(),
        ship->getVelY());
    auto msg = Blaster::Messages::CreateBlasterMessage(
        builder, 0, Blaster::Messages::MessageType_VisualObject,
        shipMsg.Union());

    // Serialize
    builder.Finish(msg);

    // Pass builder to caller
    return builder;
}

void MessageHandler::send(uint8_t *data, unsigned int len) {
    send_mutex_.lock();
    send_buffer_.push(Msg{data, len});
    send_mutex_.unlock();
}

void MessageHandler::send(MessageBuffer data) {
    send(data.GetBufferPointer(), data.GetSize());
}

void MessageHandler::send(char *data) {
    send(reinterpret_cast<uint8_t *>(data),
         static_cast<unsigned int>(strlen(data)));
}
