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

struct payload_s{

  std::string brightnessLevel;
  std::string powerLevel;
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
        payload ="005555FFFF";

        payload += Payload.brightnessLevel;

        // Rest of settings for SetColor function
        payload += "AC0D00040000";

        break;
    case SetPower:
        payload += Payload.powerLevel;
        payload += "00040000";
        break;
    }

    std::string packet;
  // Builds packet
  for(int i = 0; i < payload.length(); i+=2)
      packet += bytePrepend(std::string(std::string(1,payload[i]) + std::string(1,payload[i+1])));
    return packet;
}


void lifxHelpMenu()
{
    std::cout <<"Usage: lifx-packet-builder [OPTION] ... [VALUE]\n";
    std::cout << "\tReturns packet required to set desired BRIGHTNESS to a LifX™ bulb\n\n";
    std::cout << "Options:\n\n";
    std::cout << "\t-b, --brightness\n";
    std::cout << "\t\t\t Accepts a BRIGHTNESS value from 0 to 100.";
    std::cout << "\t-p, --power\n";
    std::cout << "\t\t\t Accepts a POWER value of 0 (off) or 1 (on).";

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

    // Parser:
    if(argc > 1 && argc % 2 == 1)
    {
        // Gets custom brightness value
        for(int argi = 1; argi < argc; argi += 2)
        {
            std::string args = std::string(argv[argi]);
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

                Payload.brightnessLevel = flipHex(n2hexstr(bLevel,4));
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

                if(bLevel == 1){
                    Payload.powerLevel = flipHex(n2hexstr(65535,4));
                }
                else if(bLevel ==0){
                    Payload.powerLevel = flipHex(n2hexstr(0,4));
                }
                else{
                    packetType_Error();
                    return 0;
                }
            }
        }

        if (PacketType == NullType)
        {
            packetType_Error();
            return 0;
        }
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
        std::cout << packet<<std::endl;

    }
    else
    {
        std::cout <<"Usage: lifx-configurator [OPTION] ... [VALUE]\n";
        std::cout << "\tReturns packet required to set desired BRIGHTNESS to a LifX™ bulb\n\n";
        std::cout << "Options:\n\n";
        std::cout << "\t-b, --brightness\n";
        std::cout << "\t\t\t Accepts a BRIGHTNESS value from 0 to 100.\n";
        std::cout << "\t-p, --power\n";
        std::cout << "\t\t\t Accepts a POWER value of 0 (off) or 1 (on).\n";

        std::cout << std::endl;
        return 0;
    }
    return 0;
}
