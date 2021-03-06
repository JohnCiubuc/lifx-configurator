#include <iostream>



// Number to hex string
template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I)<<1)
{
    static const char* digits = "0123456789ABCDEF";
    std::string rc(hex_len,'0');
    for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4)
        rc[i] = digits[(w>>j) & 0x0f];
    return rc;
}

// Prepends for each byte
std::string bytePrepend(std::string s)
{
    std::string sign = "\\x";
    s.insert(0, sign);
    return s;
}

// Flips hex string
std::string flipHex(std::string s)
{

    std::string out;
    for(int i = s.length()-1; i >= 0; i-=2)
        out += std::string(1,s[i-1]) + std::string(1,s[i]);
    return out;
}

// Converts hex string into byte sequence
std::string hexstr2byte(std::string hex)
{
    std::string byteStr;

    if(hex.length()%2==1)
        hex.insert(0, "0");

    for(int i = hex.length()-1; i >= 0; i-=2)
        byteStr += bytePrepend(std::string(std::string(1,hex[i-1]) + std::string(1,hex[i])));

    return byteStr;

}

enum messages
{
    NullType = -1,
    SetColor,
    SetPower
};

struct payload_s
{
    std::string ip;
    std::string port = "56700";

    std::string brightness = "FFFF";
    std::string powerLevel = "FFFF";
    std::string duration = "00040000";
    std::string hue = "5555";
    std::string saturation = "FFFF";
    std::string kelvin = "AC0D";
};
// Builds frame header
// But also builds command
std::string buildHeader(messages PacketType)
{
    std::string frameHeader="003400000000000000000000000000000000000000000000000000000000";
    // Adds function and other stuff. Change if interested
    switch(PacketType)
    {
    case SetColor:
        frameHeader += "66000000";
        break;
    case SetPower:
        frameHeader += "75000000";
        break;
    }
    std::string byteStr;
    for(int i = 0; i < frameHeader.length(); i+=2)
        byteStr += bytePrepend(std::string(std::string(1,frameHeader[i]) + std::string(1,frameHeader[i+1])));

    return byteStr;
}

std::string buildPayload(messages PacketType, payload_s Payload)
{
    std::string payload;

    switch(PacketType)
    {
    case SetColor:
        payload = "00";
        payload += Payload.hue;
        payload += Payload.saturation;
        payload += Payload.brightness;
        payload += Payload.kelvin;
        break;
    case SetPower:
        payload += Payload.powerLevel;
        break;
    }
    //Duration
    payload += Payload.duration;

    std::string packet;
    // Builds packet
    for(int i = 0; i < payload.length(); i+=2)
        packet += bytePrepend(std::string(std::string(1,payload[i]) + std::string(1,payload[i+1])));
    return packet;
}


void lifxHelpMenu()
{

    std::cout <<"Usage: lifx-configurator [OPTIONS] .... [VALUE]\n";
    std::cout << "\tReturns packet required to set desired BRIGHTNESS to a LifX™ bulb\n\n";
    std::cout << "Options:\n\n";
    std::cout << "\t-ip, --ip-address\n";
    std::cout << "\t\t\t The IP Port of Bulb.\n";
    std::cout << "\t-b, --brightness\n";
    std::cout << "\t\t\t Accepts a BRIGHTNESS value from 0 to 100. Default is 100\n";
    std::cout << "\t-h, --hue\n";
    std::cout << "\t\t\t Accepts a HUE value from 1 to 360. Default is 120\n";
    std::cout << "\t-s, --saturation\n";
    std::cout << "\t\t\t Accepts a SATURATION value from 0 to 100. Default is 100\n";
    std::cout << "\t-k, --kelvin\n";
    std::cout << "\t\t\t Accepts a KELVIN value in the form of ####K where # is a number. Default is 3500K\n";
    std::cout << "\t-p, --power\n";
    std::cout << "\t\t\t Accepts a POWER value of 0 (off) or 1 (on).\n";
    std::cout << "\n\nModifiers:\n\n";
    std::cout << "\t-d, --duration\n";
    std::cout << "\t\t\t Accepts a DURATION value in MILLISECONDS. Default is 1000\n";
    std::cout << "\t-po, --packet-only\n";
    std::cout << "\t\t\t Outputs packet only without sending. 0 (off) or 1 (on)\n";

    std::cout << std::endl;
}

void packetType_Error()
{
    std::cout << "ERROR: More than 1 Packet Type requested. Please choose only one Packet Type to generate\n\n";
    lifxHelpMenu();
}

int main(int argc, char ** argv)
{
    messages PacketType = NullType;
    payload_s Payload;
    bool bPacketOnly = false;

    int duration = 1000;

//Payload.ip = "192.168.1.230";

    // Parser:
    if(argc > 1 && argc % 2 == 1)
    {
        for(int argi = 1; argi < argc; argi += 2)
        {
            std::string args = std::string(argv[argi]);

            // Packets
            if(args.compare("-b") == 0 || args.compare("--brightness") == 0)
            {
                if (PacketType != NullType)
                {
                    packetType_Error();
                    return 0;
                }
                PacketType = SetColor;

                int bLevel;
                sscanf(argv[argi+1], "%d", &bLevel);

                bLevel = static_cast<int>(65535*(bLevel / 100.));

                Payload.brightness = flipHex(n2hexstr(bLevel,4));
            }
            if(args.compare("-h") == 0 || args.compare("--hue") == 0)
            {
                PacketType = SetColor;

                int bLevel;
                sscanf(argv[argi+1], "%d", &bLevel);

                bLevel = static_cast<int>(65535*(bLevel / 360.));

                Payload.hue = flipHex(n2hexstr(bLevel,4));
            }
            if(args.compare("-k") == 0 || args.compare("--kelvin") == 0)
            {
                PacketType = SetColor;

                char * kelvin = argv[argi+1];
                int kSize = 0;
                for(;kelvin[kSize] != '\0';++kSize);


                if(kSize != 5)
                {
                    packetType_Error();
                    return 0;
                }
                kelvin[kSize-1] = '\0';

                int bLevel;
                sscanf(kelvin, "%d", &bLevel);

                Payload.kelvin = flipHex(n2hexstr(bLevel,4));
            }
            if(args.compare("-s") == 0 || args.compare("--saturation") == 0)
            {
                PacketType = SetColor;

                int bLevel;
                sscanf(argv[argi+1], "%d", &bLevel);

                bLevel = static_cast<int>(65535*(bLevel / 100.));

                Payload.saturation = flipHex(n2hexstr(bLevel,4));
            }
            if(args.compare("-p") == 0 || args.compare("--power") == 0)
            {
                if (PacketType != NullType)
                {
                    packetType_Error();
                    return 0;
                }
                PacketType = SetPower;
                int bLevel;
                sscanf(argv[argi+1], "%d", &bLevel);
                if(bLevel == 1) Payload.powerLevel = flipHex(n2hexstr(65535,4));
                else if(bLevel ==0)
                    Payload.powerLevel = flipHex(n2hexstr(0,4));
                else
                {
                    packetType_Error();
                    return 0;
                }
            }
            // Modifiers
            if(args.compare("-d") == 0 || args.compare("--duration") == 0)
            {
                sscanf(argv[argi+1], "%d", &duration);
            }
            // Modifiers
            if(args.compare("-po") == 0 || args.compare("--packet-only") == 0)
            {
                int b;
                sscanf(argv[argi+1], "%d", &b);
                bPacketOnly = b == 1 ? true:false;
            }
            // Modifiers
            if(args.compare("-ip") == 0 || args.compare("--ip-address") == 0)
            {
                Payload.ip = argv[argi+1];
            }
        }

        if (PacketType == NullType)
        {
            packetType_Error();
            return 0;
        }

        Payload.duration = flipHex(n2hexstr(duration,8));
        std::string packet = buildHeader(PacketType);

        packet += buildPayload(PacketType, Payload);

        // Gets packet size for packet size header
        int packetSize = 0;
        for(int i = 0; i < packet.length(); ++i)
            if(packet[i] == 'x') packetSize++;

        packetSize += 2;
        std::string sizeFieldPre, sizeField;


        // Adds packet size header
        if(packetSize <= 255)
        {
            sizeFieldPre = bytePrepend(n2hexstr(packetSize,2));
            packet.insert(0,"\\x00");
            packet.insert(0,sizeFieldPre);
        }
        else
        {
            sizeFieldPre = n2hexstr(packetSize,4);
            for(int i = 0; i < sizeFieldPre.length(); i+=2)
                sizeField += bytePrepend(std::string(std::string(1,sizeFieldPre[i]) + std::string(1,sizeFieldPre[i+1])));
            packet.insert(0,sizeField);
        }


        // Prints packet
        if(bPacketOnly)
            std::cout << packet<<std::endl;
        else
        {

            std::string bash_udp = "#/bin/bash \n echo -e \"";
            bash_udp += packet;
            bash_udp += "\" > /dev/udp/";
            bash_udp += Payload.ip;
            bash_udp += "/";
            bash_udp += Payload.port;

            system(bash_udp.c_str());
        }




    }
    else
    {
        lifxHelpMenu();
        return 0;
    }
    return 0;
}
