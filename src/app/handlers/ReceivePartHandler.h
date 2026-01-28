#ifndef __RECEIVE_PART_HANDLER_H__
#define __RECEIVE_PART_HANDLER_H__

#include <iostream>
#include <fstream>

#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/PartHandler.h>
#include <Poco/StreamCopier.h>
#include <Poco/Exception.h>

namespace FQW::Receiver
{

class ReceivePartHandler : public Poco::Net::PartHandler 
{
public:
    ReceivePartHandler() = default;

    void handlePart(const Poco::Net::MessageHeader& header,
        std::istream& stream) final;
};

} // namespace FQW::Receiver

#endif // __RECEIVE_PART_HANDLER_H__
