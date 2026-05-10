#ifndef __STRING_INTEGRAL_HPP__
#define __STRING_INTEGRAL_HPP__

/**
 * @brief 字符串与整数之间的转换。
 * 
 * @par REQUIRES
 * TO_COMPLETE
 * 
 * @par PROVIDES
 * TO_COMPLETE
 */

#include <string>
#include <sstream>
#include "SFML/Graphics.hpp"

template<typename T> concept ConvertToString = requires (T t) { (std::string)t; };
/**
 * @brief 将任意类型转为字符串。
 * @param t 待转换的类型
 * @return 转换后的字符串
 */
template<typename InputType> requires ConvertToString<InputType>
std::string ToStr(InputType t) noexcept
{
    return t;
}
template<typename T> concept StreamToString = requires (T t, std::stringstream ss) { ss << t; };
template<typename InputType> requires (!ConvertToString<InputType>) && StreamToString<InputType>
std::string ToStr(InputType t) noexcept
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

/**
 * @brief 将字符串转为整数。
 * @param s 待转换的字符串
 * @return 转换后的整数
 */
template<typename ReturnType = int> requires std::is_integral_v<ReturnType>
ReturnType ToNum(const std::string &s) noexcept
{
    ReturnType result = 0;
    bool mark = false;
    for (auto c : s) {
        if (c == '-') {
            mark = true;
            continue;
        } else if (c == '+') {
            mark = false;
            continue;
        }
        result = result * 10 + (c - '0');
    }
    if (mark) result = -result;
    return result;
}
int ToNum(char c) noexcept;

std::string Encrypt(const std::string &str, const std::string &key = "&!#^zY-zE)>2d9'cD|*6F<g.h8:_7H=m") noexcept;

std::string Decrypt(const std::string &str, const std::string &key = "&!#^zY-zE)>2d9'cD|*6F<g.h8:_7H=m") noexcept;

#endif
