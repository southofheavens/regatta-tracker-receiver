#include <Utils.h>

#include <fstream>
#include <iostream>

#include <Poco/TemporaryFile.h>

#include <fqw-devkit/lib/General.h>
#include <libxml/xmlschemas.h>
#include <libxml/parser.h>

namespace 
{

const std::string xsd_filename = "gpx.xsd";

} //

namespace FQW::Receiver::Utils
{

bool gpxFileValidate(const std::string & data)
{
    Poco::TemporaryFile tempGpx;

    try {
        tempGpx.createFile();
    } 
    catch (...) {
        throw FQW::Devkit::FQWException("Internal server error. Try repeating the request", 
            Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    }

    std::ofstream ofs(tempGpx.path(), std::ios::binary);
    if (not ofs.is_open()) {
        throw FQW::Devkit::FQWException("Internal server error. Try repeating the request", 
            Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    }

    ofs << data;
    ofs.close();

    //

    xmlSchemaParserCtxtPtr parser_ctxt;
    xmlSchemaPtr schema;
    xmlSchemaValidCtxtPtr valid_ctxt;
    xmlDocPtr doc;
    int ret;

    xmlInitParser();

    parser_ctxt = xmlSchemaNewParserCtxt(xsd_filename.c_str());
    schema = xmlSchemaParse(parser_ctxt);
    valid_ctxt = xmlSchemaNewValidCtxt(schema);

    doc = xmlReadFile(tempGpx.path().c_str(), NULL, 0);
    ret = xmlSchemaValidateDoc(valid_ctxt, doc);

    xmlFreeDoc(doc);
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlCleanupParser();

    return ret == 0; 
} 

} // namespace FQW::Receiver::Utils
