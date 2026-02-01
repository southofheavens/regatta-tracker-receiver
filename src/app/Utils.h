#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

namespace FQW::Receiver::Utils
{

/// @brief Валидация GPX-файла
/// @param data Строка, хранящая содержимое GPX-файла
/// @return true, если файл корректный (соответствует схеме GPX 1.1 с www.topografix.com) и 
/// false в противном случае
/// @throw FQWException если произошла ошибка во время валидации
bool gpxFileValidate(const std::string & data);

//// @brief Загружает GPX-файл в S3/MinIO через HTTP PUT
/// @param bucket Название бакета
/// @param objectName Имя файла в бакете
/// @param data Содержимое файла
/// @param endpoint URL сервера MinIO/S3, например "http://127.0.0.1:9000"
/// @param accessKey Access Key
/// @param secretKey Secret Key
/// @throw FQWException при ошибке
void uploadFileToS3(
    const std::string & bucket,
    const std::string & objectName,
    const std::string & data,
    const std::string & endpoint,
    const std::string & accessKey,
    const std::string & secretKey);

} // namespace FQW::Receiver::Utils

#endif // __UTILS_H__
