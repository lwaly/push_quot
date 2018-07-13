#include <string.h>
#include <sstream>

using std::stringstream;

#include "bsw_audio.h"

bsw_audio::bsw_audio()
{
    m_ll_id = 0;
    m_ll_uuid = 0;
    m_i_audio_duration = 0;
    m_i_fs_id = 0;
    m_i_fs_group_id = 0;
    m_i_fs_area_id = 0;
    m_i_audio_uptime = 0;
    m_i_audio_type = 0;
    m_i_audio_down = 0;
    m_ui_has_bit &= 0x00000000u;
}

bsw_audio::bsw_audio(const bsw_audio &obj)
{
    m_ll_id = obj.m_ll_id;
    m_ll_uuid = obj.m_ll_uuid;
    m_str_audio_name = obj.m_str_audio_name;
    m_str_audio_path = obj.m_str_audio_path;
    m_i_audio_duration = obj.m_i_audio_duration;
    m_i_fs_id = obj.m_i_fs_id;
    m_i_fs_group_id = obj.m_i_fs_group_id;
    m_i_fs_area_id = obj.m_i_fs_area_id;
    m_i_audio_uptime = obj.m_i_audio_uptime;
    m_i_audio_type = obj.m_i_audio_type;
    m_i_audio_down = obj.m_i_audio_down;
    m_str_audio_pic = obj.m_str_audio_pic;
    m_ui_has_bit = obj.m_ui_has_bit;
}

bsw_audio & bsw_audio::operator =(const bsw_audio &obj)
{
    if (this == &obj) return *this;
    m_ll_id = obj.m_ll_id;
    m_ll_uuid = obj.m_ll_uuid;
    m_str_audio_name = obj.m_str_audio_name;
    m_str_audio_path = obj.m_str_audio_path;
    m_i_audio_duration = obj.m_i_audio_duration;
    m_i_fs_id = obj.m_i_fs_id;
    m_i_fs_group_id = obj.m_i_fs_group_id;
    m_i_fs_area_id = obj.m_i_fs_area_id;
    m_i_audio_uptime = obj.m_i_audio_uptime;
    m_i_audio_type = obj.m_i_audio_type;
    m_i_audio_down = obj.m_i_audio_down;
    m_str_audio_pic = obj.m_str_audio_pic;
    m_ui_has_bit = obj.m_ui_has_bit;
    return *this;
}

string bsw_audio::serialize() const
{
    string strOutPut, strTemp;
    stringstream strstr;

    strstr.clear();
    strstr.str();
    strstr << m_ll_id;
    strstr >> strTemp;
    strOutPut += "id=" + strTemp + "; ";

    strstr.clear();
    strstr.str();
    strstr << m_ll_uuid;
    strstr >> strTemp;
    strOutPut += "uuid=" + strTemp + "; ";

    strOutPut += "audio_name=" + m_str_audio_name + "; ";

    strOutPut += "audio_path=" + m_str_audio_path + "; ";

    strstr.clear();
    strstr.str();
    strstr << m_i_audio_duration;
    strstr >> strTemp;
    strOutPut += "audio_duration=" + strTemp + "; ";

    strstr.clear();
    strstr.str();
    strstr << m_i_fs_id;
    strstr >> strTemp;
    strOutPut += "fs_id=" + strTemp + "; ";

    strstr.clear();
    strstr.str();
    strstr << m_i_fs_group_id;
    strstr >> strTemp;
    strOutPut += "fs_group_id=" + strTemp + "; ";

    strstr.clear();
    strstr.str();
    strstr << m_i_fs_area_id;
    strstr >> strTemp;
    strOutPut += "fs_area_id=" + strTemp + "; ";

    strstr.clear();
    strstr.str();
    strstr << m_i_audio_uptime;
    strstr >> strTemp;
    strOutPut += "audio_uptime=" + strTemp + "; ";

    strstr.clear();
    strstr.str();
    strstr << m_i_audio_type;
    strstr >> strTemp;
    strOutPut += "audio_type=" + strTemp + "; ";

    strstr.clear();
    strstr.str();
    strstr << m_i_audio_down;
    strstr >> strTemp;
    strOutPut += "audio_down=" + strTemp + "; ";

    strOutPut += "audio_pic=" + m_str_audio_pic + "; ";

    return strOutPut;
}

bool bsw_audio::set_id(const int64 value)
{
    m_ll_id = value;
    m_ui_has_bit |= 0x00000001;
    return true;
}

bool bsw_audio::set_uuid(const int64 value)
{
    m_ll_uuid = value;
    m_ui_has_bit |= 0x00000002;
    return true;
}

bool bsw_audio::set_audio_name(const string &value)
{
    if (value.size() > 256)
    {        m_ui_has_bit &= ~0x00000004;
        return false;
    }
    m_str_audio_name = value;
    m_ui_has_bit |= 0x00000004;
    return true;
}

bool bsw_audio::set_audio_name(const char* value, size_t size)
{
    if (size > 256)    {        m_ui_has_bit &= ~0x00000100;
        return false;
    }
    m_str_audio_name.assign(value, size);
    m_ui_has_bit |= 0x00000004;
    return true;
}

bool bsw_audio::set_audio_name(const char* value)
{
    if (strlen(value) > 256)     {        m_ui_has_bit &= ~0x00000100;
        return false;
    }
    m_str_audio_name = value;
    m_ui_has_bit |= 0x00000004;
    return true;
}

bool bsw_audio::set_audio_path(const string &value)
{
    if (value.size() > 512)
    {        m_ui_has_bit &= ~0x00000008;
        return false;
    }
    m_str_audio_path = value;
    m_ui_has_bit |= 0x00000008;
    return true;
}

bool bsw_audio::set_audio_path(const char* value, size_t size)
{
    if (size > 512)    {        m_ui_has_bit &= ~0x00000200;
        return false;
    }
    m_str_audio_path.assign(value, size);
    m_ui_has_bit |= 0x00000008;
    return true;
}

bool bsw_audio::set_audio_path(const char* value)
{
    if (strlen(value) > 512)     {        m_ui_has_bit &= ~0x00000200;
        return false;
    }
    m_str_audio_path = value;
    m_ui_has_bit |= 0x00000008;
    return true;
}

bool bsw_audio::set_audio_duration(const int32 value)
{
    m_i_audio_duration = value;
    m_ui_has_bit |= 0x00000010;
    return true;
}

bool bsw_audio::set_fs_id(const int32 value)
{
    m_i_fs_id = value;
    m_ui_has_bit |= 0x00000020;
    return true;
}

bool bsw_audio::set_fs_group_id(const int32 value)
{
    m_i_fs_group_id = value;
    m_ui_has_bit |= 0x00000040;
    return true;
}

bool bsw_audio::set_fs_area_id(const int32 value)
{
    m_i_fs_area_id = value;
    m_ui_has_bit |= 0x00000080;
    return true;
}

bool bsw_audio::set_audio_uptime(const int32 value)
{
    m_i_audio_uptime = value;
    m_ui_has_bit |= 0x00000100;
    return true;
}

bool bsw_audio::set_audio_type(const int32 value)
{
    m_i_audio_type = value;
    m_ui_has_bit |= 0x00000200;
    return true;
}

bool bsw_audio::set_audio_down(const int32 value)
{
    m_i_audio_down = value;
    m_ui_has_bit |= 0x00000400;
    return true;
}

bool bsw_audio::set_audio_pic(const string &value)
{
    if (value.size() > 512)
    {        m_ui_has_bit &= ~0x00000800;
        return false;
    }
    m_str_audio_pic = value;
    m_ui_has_bit |= 0x00000800;
    return true;
}

bool bsw_audio::set_audio_pic(const char* value, size_t size)
{
    if (size > 512)    {        m_ui_has_bit &= ~0x00000200;
        return false;
    }
    m_str_audio_pic.assign(value, size);
    m_ui_has_bit |= 0x00000800;
    return true;
}

bool bsw_audio::set_audio_pic(const char* value)
{
    if (strlen(value) > 512)     {        m_ui_has_bit &= ~0x00000200;
        return false;
    }
    m_str_audio_pic = value;
    m_ui_has_bit |= 0x00000800;
    return true;
}

