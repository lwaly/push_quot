#ifndef BSW_AUDIO_H_
#define BSW_AUDIO_H_

#include <string>

using std::string;

#ifndef int8
#define int8    char
#endif

#ifndef uint8
#define uint8    unsigned char
#endif

#ifndef uint16
#define uint16    short
#endif

#ifndef uint16
#define uint16    unsigned short
#endif

#ifndef int32
#define int32    int
#endif

#ifndef uint32
#define uint32    unsigned int
#endif

#ifndef int64
#define int64    long long
#endif

#ifndef uint64
#define uint64    unsigned long long
#endif

class bsw_audio
{
public:
    bsw_audio();
    bsw_audio(const bsw_audio &obj);
    ~bsw_audio() {}

    bsw_audio & operator = (const bsw_audio &obj);

    inline const char* table_name() {return "bsw_audio";}
    inline uint32 get_count() {return 12;}
    string serialize() const;

    inline const char* col_id() {return "id";}
    inline const char* col_uuid() {return "uuid";}
    inline const char* col_audio_name() {return "audio_name";}
    inline const char* col_audio_path() {return "audio_path";}
    inline const char* col_audio_duration() {return "audio_duration";}
    inline const char* col_fs_id() {return "fs_id";}
    inline const char* col_fs_group_id() {return "fs_group_id";}
    inline const char* col_fs_area_id() {return "fs_area_id";}
    inline const char* col_audio_uptime() {return "audio_uptime";}
    inline const char* col_audio_type() {return "audio_type";}
    inline const char* col_audio_down() {return "audio_down";}
    inline const char* col_audio_pic() {return "audio_pic";}

    bool set_id(const int64 value);
    bool set_uuid(const int64 value);
    bool set_audio_name(const string &value);
    bool set_audio_name(const char* value, size_t size);
    bool set_audio_name(const char* value);
    bool set_audio_path(const string &value);
    bool set_audio_path(const char* value, size_t size);
    bool set_audio_path(const char* value);
    bool set_audio_duration(const int32 value);
    bool set_fs_id(const int32 value);
    bool set_fs_group_id(const int32 value);
    bool set_fs_area_id(const int32 value);
    bool set_audio_uptime(const int32 value);
    bool set_audio_type(const int32 value);
    bool set_audio_down(const int32 value);
    bool set_audio_pic(const string &value);
    bool set_audio_pic(const char* value, size_t size);
    bool set_audio_pic(const char* value);

    inline int64 id() {return m_ll_id;}
    inline int64 uuid() {return m_ll_uuid;}
    inline const string &audio_name() {return m_str_audio_name;}
    inline const string &audio_path() {return m_str_audio_path;}
    inline int32 audio_duration() {return m_i_audio_duration;}
    inline int32 fs_id() {return m_i_fs_id;}
    inline int32 fs_group_id() {return m_i_fs_group_id;}
    inline int32 fs_area_id() {return m_i_fs_area_id;}
    inline int32 audio_uptime() {return m_i_audio_uptime;}
    inline int32 audio_type() {return m_i_audio_type;}
    inline int32 audio_down() {return m_i_audio_down;}
    inline const string &audio_pic() {return m_str_audio_pic;}

    inline bool has_id() {return (m_ui_has_bit & 0x00000001) != 0;}
    inline bool has_uuid() {return (m_ui_has_bit & 0x00000002) != 0;}
    inline bool has_audio_name() {return (m_ui_has_bit & 0x00000004) != 0;}
    inline bool has_audio_path() {return (m_ui_has_bit & 0x00000008) != 0;}
    inline bool has_audio_duration() {return (m_ui_has_bit & 0x00000010) != 0;}
    inline bool has_fs_id() {return (m_ui_has_bit & 0x00000020) != 0;}
    inline bool has_fs_group_id() {return (m_ui_has_bit & 0x00000040) != 0;}
    inline bool has_fs_area_id() {return (m_ui_has_bit & 0x00000080) != 0;}
    inline bool has_audio_uptime() {return (m_ui_has_bit & 0x00000100) != 0;}
    inline bool has_audio_type() {return (m_ui_has_bit & 0x00000200) != 0;}
    inline bool has_audio_down() {return (m_ui_has_bit & 0x00000400) != 0;}
    inline bool has_audio_pic() {return (m_ui_has_bit & 0x00000800) != 0;}

    inline void clear_has_id() {m_ui_has_bit &= ~0x00000001;}
    inline void clear_has_uuid() {m_ui_has_bit &= ~0x00000002;}
    inline void clear_has_audio_name() {m_ui_has_bit &= ~0x00000004;}
    inline void clear_has_audio_path() {m_ui_has_bit &= ~0x00000008;}
    inline void clear_has_audio_duration() {m_ui_has_bit &= ~0x00000010;}
    inline void clear_has_fs_id() {m_ui_has_bit &= ~0x00000020;}
    inline void clear_has_fs_group_id() {m_ui_has_bit &= ~0x00000040;}
    inline void clear_has_fs_area_id() {m_ui_has_bit &= ~0x00000080;}
    inline void clear_has_audio_uptime() {m_ui_has_bit &= ~0x00000100;}
    inline void clear_has_audio_type() {m_ui_has_bit &= ~0x00000200;}
    inline void clear_has_audio_down() {m_ui_has_bit &= ~0x00000400;}
    inline void clear_has_audio_pic() {m_ui_has_bit &= ~0x00000800;}

private:
    uint32 m_ui_has_bit;
    int64 m_ll_id;
    int64 m_ll_uuid;
    string m_str_audio_name;
    string m_str_audio_path;
    int32 m_i_audio_duration;
    int32 m_i_fs_id;
    int32 m_i_fs_group_id;
    int32 m_i_fs_area_id;
    int32 m_i_audio_uptime;
    int32 m_i_audio_type;
    int32 m_i_audio_down;
    string m_str_audio_pic;
};

#endif
