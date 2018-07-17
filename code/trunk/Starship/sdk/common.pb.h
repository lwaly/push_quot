// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: common.proto

#ifndef PROTOBUF_common_2eproto__INCLUDED
#define PROTOBUF_common_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace common {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_common_2eproto();
void protobuf_AssignDesc_common_2eproto();
void protobuf_ShutdownFile_common_2eproto();

class errorinfo;
class client_info;

// ===================================================================

class errorinfo : public ::google::protobuf::Message {
 public:
  errorinfo();
  virtual ~errorinfo();

  errorinfo(const errorinfo& from);

  inline errorinfo& operator=(const errorinfo& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const errorinfo& default_instance();

  void Swap(errorinfo* other);

  // implements Message ----------------------------------------------

  errorinfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const errorinfo& from);
  void MergeFrom(const errorinfo& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 error_code = 1;
  inline bool has_error_code() const;
  inline void clear_error_code();
  static const int kErrorCodeFieldNumber = 1;
  inline ::google::protobuf::int32 error_code() const;
  inline void set_error_code(::google::protobuf::int32 value);

  // optional bytes error_info = 2;
  inline bool has_error_info() const;
  inline void clear_error_info();
  static const int kErrorInfoFieldNumber = 2;
  inline const ::std::string& error_info() const;
  inline void set_error_info(const ::std::string& value);
  inline void set_error_info(const char* value);
  inline void set_error_info(const void* value, size_t size);
  inline ::std::string* mutable_error_info();
  inline ::std::string* release_error_info();
  inline void set_allocated_error_info(::std::string* error_info);

  // optional bytes error_client_show = 3;
  inline bool has_error_client_show() const;
  inline void clear_error_client_show();
  static const int kErrorClientShowFieldNumber = 3;
  inline const ::std::string& error_client_show() const;
  inline void set_error_client_show(const ::std::string& value);
  inline void set_error_client_show(const char* value);
  inline void set_error_client_show(const void* value, size_t size);
  inline ::std::string* mutable_error_client_show();
  inline ::std::string* release_error_client_show();
  inline void set_allocated_error_client_show(::std::string* error_client_show);

  // @@protoc_insertion_point(class_scope:common.errorinfo)
 private:
  inline void set_has_error_code();
  inline void clear_has_error_code();
  inline void set_has_error_info();
  inline void clear_has_error_info();
  inline void set_has_error_client_show();
  inline void clear_has_error_client_show();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* error_info_;
  ::std::string* error_client_show_;
  ::google::protobuf::int32 error_code_;
  friend void  protobuf_AddDesc_common_2eproto();
  friend void protobuf_AssignDesc_common_2eproto();
  friend void protobuf_ShutdownFile_common_2eproto();

  void InitAsDefaultInstance();
  static errorinfo* default_instance_;
};
// -------------------------------------------------------------------

class client_info : public ::google::protobuf::Message {
 public:
  client_info();
  virtual ~client_info();

  client_info(const client_info& from);

  inline client_info& operator=(const client_info& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const client_info& default_instance();

  void Swap(client_info* other);

  // implements Message ----------------------------------------------

  client_info* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const client_info& from);
  void MergeFrom(const client_info& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required uint32 app_type = 1;
  inline bool has_app_type() const;
  inline void clear_app_type();
  static const int kAppTypeFieldNumber = 1;
  inline ::google::protobuf::uint32 app_type() const;
  inline void set_app_type(::google::protobuf::uint32 value);

  // required uint32 client_type = 2;
  inline bool has_client_type() const;
  inline void clear_client_type();
  static const int kClientTypeFieldNumber = 2;
  inline ::google::protobuf::uint32 client_type() const;
  inline void set_client_type(::google::protobuf::uint32 value);

  // required bytes version = 3;
  inline bool has_version() const;
  inline void clear_version();
  static const int kVersionFieldNumber = 3;
  inline const ::std::string& version() const;
  inline void set_version(const ::std::string& value);
  inline void set_version(const char* value);
  inline void set_version(const void* value, size_t size);
  inline ::std::string* mutable_version();
  inline ::std::string* release_version();
  inline void set_allocated_version(::std::string* version);

  // required bytes deviceInfo = 4;
  inline bool has_deviceinfo() const;
  inline void clear_deviceinfo();
  static const int kDeviceInfoFieldNumber = 4;
  inline const ::std::string& deviceinfo() const;
  inline void set_deviceinfo(const ::std::string& value);
  inline void set_deviceinfo(const char* value);
  inline void set_deviceinfo(const void* value, size_t size);
  inline ::std::string* mutable_deviceinfo();
  inline ::std::string* release_deviceinfo();
  inline void set_allocated_deviceinfo(::std::string* deviceinfo);

  // required bytes ip = 5;
  inline bool has_ip() const;
  inline void clear_ip();
  static const int kIpFieldNumber = 5;
  inline const ::std::string& ip() const;
  inline void set_ip(const ::std::string& value);
  inline void set_ip(const char* value);
  inline void set_ip(const void* value, size_t size);
  inline ::std::string* mutable_ip();
  inline ::std::string* release_ip();
  inline void set_allocated_ip(::std::string* ip);

  // @@protoc_insertion_point(class_scope:common.client_info)
 private:
  inline void set_has_app_type();
  inline void clear_has_app_type();
  inline void set_has_client_type();
  inline void clear_has_client_type();
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_deviceinfo();
  inline void clear_has_deviceinfo();
  inline void set_has_ip();
  inline void clear_has_ip();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 app_type_;
  ::google::protobuf::uint32 client_type_;
  ::std::string* version_;
  ::std::string* deviceinfo_;
  ::std::string* ip_;
  friend void  protobuf_AddDesc_common_2eproto();
  friend void protobuf_AssignDesc_common_2eproto();
  friend void protobuf_ShutdownFile_common_2eproto();

  void InitAsDefaultInstance();
  static client_info* default_instance_;
};
// ===================================================================


// ===================================================================

// errorinfo

// required int32 error_code = 1;
inline bool errorinfo::has_error_code() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void errorinfo::set_has_error_code() {
  _has_bits_[0] |= 0x00000001u;
}
inline void errorinfo::clear_has_error_code() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void errorinfo::clear_error_code() {
  error_code_ = 0;
  clear_has_error_code();
}
inline ::google::protobuf::int32 errorinfo::error_code() const {
  // @@protoc_insertion_point(field_get:common.errorinfo.error_code)
  return error_code_;
}
inline void errorinfo::set_error_code(::google::protobuf::int32 value) {
  set_has_error_code();
  error_code_ = value;
  // @@protoc_insertion_point(field_set:common.errorinfo.error_code)
}

// optional bytes error_info = 2;
inline bool errorinfo::has_error_info() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void errorinfo::set_has_error_info() {
  _has_bits_[0] |= 0x00000002u;
}
inline void errorinfo::clear_has_error_info() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void errorinfo::clear_error_info() {
  if (error_info_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    error_info_->clear();
  }
  clear_has_error_info();
}
inline const ::std::string& errorinfo::error_info() const {
  // @@protoc_insertion_point(field_get:common.errorinfo.error_info)
  return *error_info_;
}
inline void errorinfo::set_error_info(const ::std::string& value) {
  set_has_error_info();
  if (error_info_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    error_info_ = new ::std::string;
  }
  error_info_->assign(value);
  // @@protoc_insertion_point(field_set:common.errorinfo.error_info)
}
inline void errorinfo::set_error_info(const char* value) {
  set_has_error_info();
  if (error_info_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    error_info_ = new ::std::string;
  }
  error_info_->assign(value);
  // @@protoc_insertion_point(field_set_char:common.errorinfo.error_info)
}
inline void errorinfo::set_error_info(const void* value, size_t size) {
  set_has_error_info();
  if (error_info_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    error_info_ = new ::std::string;
  }
  error_info_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:common.errorinfo.error_info)
}
inline ::std::string* errorinfo::mutable_error_info() {
  set_has_error_info();
  if (error_info_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    error_info_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:common.errorinfo.error_info)
  return error_info_;
}
inline ::std::string* errorinfo::release_error_info() {
  clear_has_error_info();
  if (error_info_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = error_info_;
    error_info_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void errorinfo::set_allocated_error_info(::std::string* error_info) {
  if (error_info_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete error_info_;
  }
  if (error_info) {
    set_has_error_info();
    error_info_ = error_info;
  } else {
    clear_has_error_info();
    error_info_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:common.errorinfo.error_info)
}

// optional bytes error_client_show = 3;
inline bool errorinfo::has_error_client_show() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void errorinfo::set_has_error_client_show() {
  _has_bits_[0] |= 0x00000004u;
}
inline void errorinfo::clear_has_error_client_show() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void errorinfo::clear_error_client_show() {
  if (error_client_show_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    error_client_show_->clear();
  }
  clear_has_error_client_show();
}
inline const ::std::string& errorinfo::error_client_show() const {
  // @@protoc_insertion_point(field_get:common.errorinfo.error_client_show)
  return *error_client_show_;
}
inline void errorinfo::set_error_client_show(const ::std::string& value) {
  set_has_error_client_show();
  if (error_client_show_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    error_client_show_ = new ::std::string;
  }
  error_client_show_->assign(value);
  // @@protoc_insertion_point(field_set:common.errorinfo.error_client_show)
}
inline void errorinfo::set_error_client_show(const char* value) {
  set_has_error_client_show();
  if (error_client_show_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    error_client_show_ = new ::std::string;
  }
  error_client_show_->assign(value);
  // @@protoc_insertion_point(field_set_char:common.errorinfo.error_client_show)
}
inline void errorinfo::set_error_client_show(const void* value, size_t size) {
  set_has_error_client_show();
  if (error_client_show_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    error_client_show_ = new ::std::string;
  }
  error_client_show_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:common.errorinfo.error_client_show)
}
inline ::std::string* errorinfo::mutable_error_client_show() {
  set_has_error_client_show();
  if (error_client_show_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    error_client_show_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:common.errorinfo.error_client_show)
  return error_client_show_;
}
inline ::std::string* errorinfo::release_error_client_show() {
  clear_has_error_client_show();
  if (error_client_show_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = error_client_show_;
    error_client_show_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void errorinfo::set_allocated_error_client_show(::std::string* error_client_show) {
  if (error_client_show_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete error_client_show_;
  }
  if (error_client_show) {
    set_has_error_client_show();
    error_client_show_ = error_client_show;
  } else {
    clear_has_error_client_show();
    error_client_show_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:common.errorinfo.error_client_show)
}

// -------------------------------------------------------------------

// client_info

// required uint32 app_type = 1;
inline bool client_info::has_app_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void client_info::set_has_app_type() {
  _has_bits_[0] |= 0x00000001u;
}
inline void client_info::clear_has_app_type() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void client_info::clear_app_type() {
  app_type_ = 0u;
  clear_has_app_type();
}
inline ::google::protobuf::uint32 client_info::app_type() const {
  // @@protoc_insertion_point(field_get:common.client_info.app_type)
  return app_type_;
}
inline void client_info::set_app_type(::google::protobuf::uint32 value) {
  set_has_app_type();
  app_type_ = value;
  // @@protoc_insertion_point(field_set:common.client_info.app_type)
}

// required uint32 client_type = 2;
inline bool client_info::has_client_type() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void client_info::set_has_client_type() {
  _has_bits_[0] |= 0x00000002u;
}
inline void client_info::clear_has_client_type() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void client_info::clear_client_type() {
  client_type_ = 0u;
  clear_has_client_type();
}
inline ::google::protobuf::uint32 client_info::client_type() const {
  // @@protoc_insertion_point(field_get:common.client_info.client_type)
  return client_type_;
}
inline void client_info::set_client_type(::google::protobuf::uint32 value) {
  set_has_client_type();
  client_type_ = value;
  // @@protoc_insertion_point(field_set:common.client_info.client_type)
}

// required bytes version = 3;
inline bool client_info::has_version() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void client_info::set_has_version() {
  _has_bits_[0] |= 0x00000004u;
}
inline void client_info::clear_has_version() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void client_info::clear_version() {
  if (version_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    version_->clear();
  }
  clear_has_version();
}
inline const ::std::string& client_info::version() const {
  // @@protoc_insertion_point(field_get:common.client_info.version)
  return *version_;
}
inline void client_info::set_version(const ::std::string& value) {
  set_has_version();
  if (version_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    version_ = new ::std::string;
  }
  version_->assign(value);
  // @@protoc_insertion_point(field_set:common.client_info.version)
}
inline void client_info::set_version(const char* value) {
  set_has_version();
  if (version_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    version_ = new ::std::string;
  }
  version_->assign(value);
  // @@protoc_insertion_point(field_set_char:common.client_info.version)
}
inline void client_info::set_version(const void* value, size_t size) {
  set_has_version();
  if (version_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    version_ = new ::std::string;
  }
  version_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:common.client_info.version)
}
inline ::std::string* client_info::mutable_version() {
  set_has_version();
  if (version_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    version_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:common.client_info.version)
  return version_;
}
inline ::std::string* client_info::release_version() {
  clear_has_version();
  if (version_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = version_;
    version_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void client_info::set_allocated_version(::std::string* version) {
  if (version_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete version_;
  }
  if (version) {
    set_has_version();
    version_ = version;
  } else {
    clear_has_version();
    version_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:common.client_info.version)
}

// required bytes deviceInfo = 4;
inline bool client_info::has_deviceinfo() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void client_info::set_has_deviceinfo() {
  _has_bits_[0] |= 0x00000008u;
}
inline void client_info::clear_has_deviceinfo() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void client_info::clear_deviceinfo() {
  if (deviceinfo_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    deviceinfo_->clear();
  }
  clear_has_deviceinfo();
}
inline const ::std::string& client_info::deviceinfo() const {
  // @@protoc_insertion_point(field_get:common.client_info.deviceInfo)
  return *deviceinfo_;
}
inline void client_info::set_deviceinfo(const ::std::string& value) {
  set_has_deviceinfo();
  if (deviceinfo_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    deviceinfo_ = new ::std::string;
  }
  deviceinfo_->assign(value);
  // @@protoc_insertion_point(field_set:common.client_info.deviceInfo)
}
inline void client_info::set_deviceinfo(const char* value) {
  set_has_deviceinfo();
  if (deviceinfo_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    deviceinfo_ = new ::std::string;
  }
  deviceinfo_->assign(value);
  // @@protoc_insertion_point(field_set_char:common.client_info.deviceInfo)
}
inline void client_info::set_deviceinfo(const void* value, size_t size) {
  set_has_deviceinfo();
  if (deviceinfo_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    deviceinfo_ = new ::std::string;
  }
  deviceinfo_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:common.client_info.deviceInfo)
}
inline ::std::string* client_info::mutable_deviceinfo() {
  set_has_deviceinfo();
  if (deviceinfo_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    deviceinfo_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:common.client_info.deviceInfo)
  return deviceinfo_;
}
inline ::std::string* client_info::release_deviceinfo() {
  clear_has_deviceinfo();
  if (deviceinfo_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = deviceinfo_;
    deviceinfo_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void client_info::set_allocated_deviceinfo(::std::string* deviceinfo) {
  if (deviceinfo_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete deviceinfo_;
  }
  if (deviceinfo) {
    set_has_deviceinfo();
    deviceinfo_ = deviceinfo;
  } else {
    clear_has_deviceinfo();
    deviceinfo_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:common.client_info.deviceInfo)
}

// required bytes ip = 5;
inline bool client_info::has_ip() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void client_info::set_has_ip() {
  _has_bits_[0] |= 0x00000010u;
}
inline void client_info::clear_has_ip() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void client_info::clear_ip() {
  if (ip_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ip_->clear();
  }
  clear_has_ip();
}
inline const ::std::string& client_info::ip() const {
  // @@protoc_insertion_point(field_get:common.client_info.ip)
  return *ip_;
}
inline void client_info::set_ip(const ::std::string& value) {
  set_has_ip();
  if (ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ip_ = new ::std::string;
  }
  ip_->assign(value);
  // @@protoc_insertion_point(field_set:common.client_info.ip)
}
inline void client_info::set_ip(const char* value) {
  set_has_ip();
  if (ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ip_ = new ::std::string;
  }
  ip_->assign(value);
  // @@protoc_insertion_point(field_set_char:common.client_info.ip)
}
inline void client_info::set_ip(const void* value, size_t size) {
  set_has_ip();
  if (ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ip_ = new ::std::string;
  }
  ip_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:common.client_info.ip)
}
inline ::std::string* client_info::mutable_ip() {
  set_has_ip();
  if (ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ip_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:common.client_info.ip)
  return ip_;
}
inline ::std::string* client_info::release_ip() {
  clear_has_ip();
  if (ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = ip_;
    ip_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void client_info::set_allocated_ip(::std::string* ip) {
  if (ip_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete ip_;
  }
  if (ip) {
    set_has_ip();
    ip_ = ip;
  } else {
    clear_has_ip();
    ip_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:common.client_info.ip)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace common

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_common_2eproto__INCLUDED