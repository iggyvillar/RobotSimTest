#include "mySocket.h"
#include "PktDef.h"
#include <iostream>

int main() {
    // STEP 1: Set up the TCP client socket to connect to simulator at localhost:5000
    MySocket client(SocketType::CLIENT, "127.0.0.1", 5000, ConnectionType::TCP);
    client.ConnectTCP();

    // STEP 2: Create a packet using PktDef
    PktDef packet;
    packet.setCMD(CMDType::DRIVE);    // Drive command
    packet.setPktCount(1);            // Packet count

    // STEP 3: Prepare a drive body structure
    driveBody drive;
    drive.direction = FORWARD;        // from PktDef.h: 1 = forward
    drive.duration = 10;              // 10 seconds
    drive.speed = 5;                  // speed level

    // STEP 4: Set body data in the packet
    packet.setBodyData(reinterpret_cast<unsigned char*>(&drive), sizeof(driveBody));

    // STEP 5: Generate raw packet and send
    unsigned char* rawPacket = packet.genPacket();
    int packetSize = HEADERSIZE + 1 + packet.getLength() + 1;

    client.SendData(reinterpret_cast<const char*>(rawPacket), packetSize);
    std::cout << "Packet sent to simulator." << std::endl;

    // STEP 6 (Optional): Receive ACK/NACK from simulator
    char buffer[DEFAULT_SIZE] = {};
    int bytesReceived = client.GetData(buffer);
    if (bytesReceived > 0) {
        std::cout << "Received response from simulator (" << bytesReceived << " bytes): ";
        for (int i = 0; i < bytesReceived; ++i) {
            std::cout << std::hex << (static_cast<unsigned int>(buffer[i]) & 0xFF) << " ";
        }
        std::cout << std::endl;
    }
    else {
        std::cout << "No response from simulator." << std::endl;
    }

    // STEP 7: Disconnect
    client.DisconnectTCP();
    return 0;
}
