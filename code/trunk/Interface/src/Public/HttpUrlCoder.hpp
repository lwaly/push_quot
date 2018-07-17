/*******************************************************************************
 * Project:  Interface
 * @file     HttpUrlCoder.hpp
 * @brief    url 编解码
 * @author   wfh
 * @date:    2016年12月31日
 * @note     
 * Modify history:
 ******************************************************************************/

#ifndef _HTTP_URL_CODER_HPP_
#define _HTTP_URL_CODER_HPP_

#include <string>
#include <cstdio>
#include <vector>
#include <algorithm>

namespace mg
{
    char *url_encode(const char *s, int len, int *new_length);
    void url_encode(std::string &s);
    int url_decode(char *str, int len);
    void url_decode(std::string &str);
    void url_decode_original(std::string &str);
};

#endif /* _HTTP_URL_CODER_HPP_ */
