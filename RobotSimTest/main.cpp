#include "mySocket.h"
#include "PktDef.h"
#include <iostream>
#include <bitset>  // for CRC calculation

int main() {
    std::cout << "[CLIENT] Creating socket (UDP)..." << std::endl;
    MySocket client(SocketType::CLIENT, "127.0.0.1", 5000, ConnectionType::UDP);

    std::cout << "[CLIENT] Building packet..." << std::endl;
    PktDef packet;
    packet.setCMD(CMDType::DRIVE);       // Set DRIVE
    packet.setPktCount(1);               // Packet counter

    // Fill drive command body
    driveBody drive;
    drive.direction = FORWARD;           // 1 = FORWARD
    drive.duration = 10;                 // Duration: 10s
    drive.speed = 85;                    // Speed: 85%

    // Confirm size
    std::cout << "[DEBUG] sizeof(driveBody): " << sizeof(driveBody) << std::endl;

    // Set body data (only 3 bytes)
    packet.setBodyData(reinterpret_cast<unsigned char*>(&drive), 3);

    std::cout << "[DEBUG] packet.getLength(): " << static_cast<int>(packet.getLength()) << std::endl;
    // Generate raw packet
    unsigned char* rawPacket = packet.genPacket();
    int packetSize = HEADERSIZE + 1 + packet.getLength() + 1;

    // Clear ACK flag (bit 3) to avoid simulator crash
    rawPacket[2] &= ~(1 << 3);  // Make sure only DRIVE = 1

    // Recalculate CRC manually (parity bit count)
    unsigned char correctCRC = 0;
    for (int i = 0; i < packetSize - 1; ++i) {
        std::bitset<8> bits(rawPacket[i]);
        correctCRC += bits.count();
    }
    rawPacket[packetSize - 1] = correctCRC;

    //  Show full packet for debug
    std::cout << "[CLIENT] Raw Packet (" << packetSize << " bytes): ";
    for (int i = 0; i < packetSize; ++i)
        std::cout << std::hex << std::uppercase << (static_cast<int>(rawPacket[i]) & 0xFF) << " ";
    std::cout << std::dec << std::endl;

    // Send to simulator
    std::cout << "[CLIENT] Sending packet..." << std::endl;
    client.SendData(reinterpret_cast<const char*>(rawPacket), packetSize);
    std::cout << "[CLIENT] Packet sent to simulator." << std::endl;

    // Try to receive reply
    char buffer[DEFAULT_SIZE] = {};
    int bytesReceived = client.GetData(buffer);

    if (bytesReceived > 0) {
        std::cout << "[CLIENT] Received response (" << bytesReceived << " bytes): ";
        for (int i = 0; i < bytesReceived; ++i)
            std::cout << std::hex << std::uppercase << (static_cast<unsigned int>(buffer[i]) & 0xFF) << " ";
        std::cout << std::dec << std::endl;
    }
    else {
        std::cout << "[CLIENT] No response from simulator." << std::endl;
    }

    return 0;
}

