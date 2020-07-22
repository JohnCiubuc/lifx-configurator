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

// Builds frame header
// But also builds command
std::string buildHeader()
{
    std::string frameHeader="003400000000000000000000000000000000000000000000000000000000";
    // Adds function and other stuff. Change if interested
    // Currently only does SetColor function (https://lan.developer.lifx.com/v2.0/docs/light-messages#section-setcolor-102)
    frameHeader += "66000000";
    std::string byteStr;
    for(int i = 0; i < frameHeader.length(); i+=2)
        byteStr += bytePrepend(std::string(std::string(1,frameHeader[i]) + std::string(1,frameHeader[i+1])));

    return byteStr;
}

int main(int argc, char ** argv)
{

    // Default, 100%
    std::string brightnessLevel = flipHex(n2hexstr(65535,4));

    // Parser:
    if(argc > 1 && argc % 2 == 1)
    {
        // Gets custom brightness value
        for(int argi = 1; argi < argc; argi += 2)
        {
            std::string args = std::string(argv[argi]);
            if(args.compare("-b") == 0 || args.compare("--brightness") == 0)
            {
                int bLevel;
                sscanf(argv[argi+1], "%d", &bLevel);

                bLevel = static_cast<int>(65535*(bLevel / 100.));

                brightnessLevel = flipHex(n2hexstr(bLevel,4));
            }
        }
        
        std::string packet = buildHeader();

        // Other settings for SetColor function
        std::string payload ="005555FFFF";

        payload += brightnessLevel;

        // Rest of settings for SetColor function
        payload += "AC0D00040000";


        // Builds packet
        for(int i = 0; i < payload.length(); i+=2)
            packet += bytePrepend(std::string(std::string(1,payload[i]) + std::string(1,payload[i+1])));

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
        std::cout <<"Usage: lifx-packet-builder [OPTION] ... [VALUE]\n";
        std::cout << "\tReturns packet required to set desired BRIGHTNESS to a LifX™ bulb\n\n";
        std::cout << "Options:\n\n";
        std::cout << "\t-b, --brightness\n";
        std::cout << "\t\t\t Accepts a BRIGHTNESS value from 0 to 100.";

        std::cout << std::endl;
        return 0;
    }
    return 0;
}
