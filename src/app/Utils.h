#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

namespace FQW::Receiver::Utils
{

/// @brief Валидация gpx-файла
/// @return true, если файл корректный (соответствует схеме GPX 1.1 с www.topografix.com) и 
/// false в противном случае
/// @throw FQWException если произошла ошибка во время валидации
bool gpxFileValidate(const std::string & data);

} // namespace FQW::Receiver::Utils

#endif // __UTILS_H__
