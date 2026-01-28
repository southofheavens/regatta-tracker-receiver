#include <ReceivePartHandler.h>

namespace FQW::Receiver
{

void ReceivePartHandler::handlePart(const Poco::Net::MessageHeader & header, std::istream & stream)
{
    std::string disp;
    if (header.has("Content-Disposition")) {
        disp = header["Content-Disposition"];
    }

    std::string filename = "upload.gpx";
    auto pos = disp.find("filename=");
    if (pos != std::string::npos) 
    {
        filename = disp.substr(pos + 10);
        if (!filename.empty() && filename.back() == '"') {
            filename.pop_back();
        }
        if (!filename.empty() && filename.front() == '"') {
            filename.erase(0, 1);
        }
    }

    std::cout << "Receiving file: " << filename << std::endl;

    std::ofstream out(filename, std::ios::binary);
    Poco::StreamCopier::copyStream(stream, out);

    std::cout << "File saved.\n";
}

} // namespace FQW::Receiver 
