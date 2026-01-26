#ifndef __RECEIVE_PART_HANDLER_H__
#define __RECEIVE_PART_HANDLER_H__

#include <iostream>
#include <fstream>

#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/PartHandler.h>
#include <Poco/StreamCopier.h>
#include <Poco/Exception.h>

namespace FQW::Receiver::Handlers
{

class ReceivePartHandler : public Poco::Net::PartHandler 
{
public:
    ReceivePartHandler() = default;

    void handlePart(const Poco::Net::MessageHeader& header,
        std::istream& stream) final
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
};

} // namespace FQW::Receiver::Handlers

#endif // __RECEIVE_PART_HANDLER_H__
