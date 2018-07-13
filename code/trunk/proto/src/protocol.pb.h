// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: protocol.proto

#ifndef PROTOBUF_protocol_2eproto__INCLUDED
#define PROTOBUF_protocol_2eproto__INCLUDED

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
#include "common.pb.h"
// @@protoc_insertion_point(includes)

namespace protocol {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_protocol_2eproto();
void protobuf_AssignDesc_protocol_2eproto();
void protobuf_ShutdownFile_protocol_2eproto();

class ulogin;
class ulogin_ack;
class ulogout;
class ulogout_ack;
class forced_offline_ntf;

// ===================================================================

class ulogin : public ::google::protobuf::Message {
 public:
  ulogin();
  virtual ~ulogin();

  ulogin(const ulogin& from);

  inline ulogin& operator=(const ulogin& from) {
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
  static const ulogin& default_instance();

  void Swap(ulogin* other);

  // implements Message ----------------------------------------------

  ulogin* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ulogin& from);
  void MergeFrom(const ulogin& from);
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

  // required uint32 uid = 1;
  inline bool has_uid() const;
  inline void clear_uid();
  static const int kUidFieldNumber = 1;
  inline ::google::protobuf::uint32 uid() const;
  inline void set_uid(::google::protobuf::uint32 value);

  // required bytes auth_code = 2;
  inline bool has_auth_code() const;
  inline void clear_auth_code();
  static const int kAuthCodeFieldNumber = 2;
  inline const ::std::string& auth_code() const;
  inline void set_auth_code(const ::std::string& value);
  inline void set_auth_code(const char* value);
  inline void set_auth_code(const void* value, size_t size);
  inline ::std::string* mutable_auth_code();
  inline ::std::string* release_auth_code();
  inline void set_allocated_auth_code(::std::string* auth_code);

  // required bytes password = 3;
  inline bool has_password() const;
  inline void clear_password();
  static const int kPasswordFieldNumber = 3;
  inline const ::std::string& password() const;
  inline void set_password(const ::std::string& value);
  inline void set_password(const char* value);
  inline void set_password(const void* value, size_t size);
  inline ::std::string* mutable_password();
  inline ::std::string* release_password();
  inline void set_allocated_password(::std::string* password);

  // required .common.client_info client_info = 4;
  inline bool has_client_info() const;
  inline void clear_client_info();
  static const int kClientInfoFieldNumber = 4;
  inline const ::common::client_info& client_info() const;
  inline ::common::client_info* mutable_client_info();
  inline ::common::client_info* release_client_info();
  inline void set_allocated_client_info(::common::client_info* client_info);

  // optional bytes ser_nodeIdentify = 5;
  inline bool has_ser_nodeidentify() const;
  inline void clear_ser_nodeidentify();
  static const int kSerNodeIdentifyFieldNumber = 5;
  inline const ::std::string& ser_nodeidentify() const;
  inline void set_ser_nodeidentify(const ::std::string& value);
  inline void set_ser_nodeidentify(const char* value);
  inline void set_ser_nodeidentify(const void* value, size_t size);
  inline ::std::string* mutable_ser_nodeidentify();
  inline ::std::string* release_ser_nodeidentify();
  inline void set_allocated_ser_nodeidentify(::std::string* ser_nodeidentify);

  // @@protoc_insertion_point(class_scope:protocol.ulogin)
 private:
  inline void set_has_uid();
  inline void clear_has_uid();
  inline void set_has_auth_code();
  inline void clear_has_auth_code();
  inline void set_has_password();
  inline void clear_has_password();
  inline void set_has_client_info();
  inline void clear_has_client_info();
  inline void set_has_ser_nodeidentify();
  inline void clear_has_ser_nodeidentify();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* auth_code_;
  ::std::string* password_;
  ::common::client_info* client_info_;
  ::std::string* ser_nodeidentify_;
  ::google::protobuf::uint32 uid_;
  friend void  protobuf_AddDesc_protocol_2eproto();
  friend void protobuf_AssignDesc_protocol_2eproto();
  friend void protobuf_ShutdownFile_protocol_2eproto();

  void InitAsDefaultInstance();
  static ulogin* default_instance_;
};
// -------------------------------------------------------------------

class ulogin_ack : public ::google::protobuf::Message {
 public:
  ulogin_ack();
  virtual ~ulogin_ack();

  ulogin_ack(const ulogin_ack& from);

  inline ulogin_ack& operator=(const ulogin_ack& from) {
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
  static const ulogin_ack& default_instance();

  void Swap(ulogin_ack* other);

  // implements Message ----------------------------------------------

  ulogin_ack* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ulogin_ack& from);
  void MergeFrom(const ulogin_ack& from);
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

  // required .common.errorinfo error = 1;
  inline bool has_error() const;
  inline void clear_error();
  static const int kErrorFieldNumber = 1;
  inline const ::common::errorinfo& error() const;
  inline ::common::errorinfo* mutable_error();
  inline ::common::errorinfo* release_error();
  inline void set_allocated_error(::common::errorinfo* error);

  // optional uint32 beat_time = 2;
  inline bool has_beat_time() const;
  inline void clear_beat_time();
  static const int kBeatTimeFieldNumber = 2;
  inline ::google::protobuf::uint32 beat_time() const;
  inline void set_beat_time(::google::protobuf::uint32 value);

  // optional uint32 login_prompt = 3;
  inline bool has_login_prompt() const;
  inline void clear_login_prompt();
  static const int kLoginPromptFieldNumber = 3;
  inline ::google::protobuf::uint32 login_prompt() const;
  inline void set_login_prompt(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:protocol.ulogin_ack)
 private:
  inline void set_has_error();
  inline void clear_has_error();
  inline void set_has_beat_time();
  inline void clear_has_beat_time();
  inline void set_has_login_prompt();
  inline void clear_has_login_prompt();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::common::errorinfo* error_;
  ::google::protobuf::uint32 beat_time_;
  ::google::protobuf::uint32 login_prompt_;
  friend void  protobuf_AddDesc_protocol_2eproto();
  friend void protobuf_AssignDesc_protocol_2eproto();
  friend void protobuf_ShutdownFile_protocol_2eproto();

  void InitAsDefaultInstance();
  static ulogin_ack* default_instance_;
};
// -------------------------------------------------------------------

class ulogout : public ::google::protobuf::Message {
 public:
  ulogout();
  virtual ~ulogout();

  ulogout(const ulogout& from);

  inline ulogout& operator=(const ulogout& from) {
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
  static const ulogout& default_instance();

  void Swap(ulogout* other);

  // implements Message ----------------------------------------------

  ulogout* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ulogout& from);
  void MergeFrom(const ulogout& from);
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

  // required uint32 uid = 1;
  inline bool has_uid() const;
  inline void clear_uid();
  static const int kUidFieldNumber = 1;
  inline ::google::protobuf::uint32 uid() const;
  inline void set_uid(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:protocol.ulogout)
 private:
  inline void set_has_uid();
  inline void clear_has_uid();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 uid_;
  friend void  protobuf_AddDesc_protocol_2eproto();
  friend void protobuf_AssignDesc_protocol_2eproto();
  friend void protobuf_ShutdownFile_protocol_2eproto();

  void InitAsDefaultInstance();
  static ulogout* default_instance_;
};
// -------------------------------------------------------------------

class ulogout_ack : public ::google::protobuf::Message {
 public:
  ulogout_ack();
  virtual ~ulogout_ack();

  ulogout_ack(const ulogout_ack& from);

  inline ulogout_ack& operator=(const ulogout_ack& from) {
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
  static const ulogout_ack& default_instance();

  void Swap(ulogout_ack* other);

  // implements Message ----------------------------------------------

  ulogout_ack* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ulogout_ack& from);
  void MergeFrom(const ulogout_ack& from);
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

  // optional .common.errorinfo error = 1;
  inline bool has_error() const;
  inline void clear_error();
  static const int kErrorFieldNumber = 1;
  inline const ::common::errorinfo& error() const;
  inline ::common::errorinfo* mutable_error();
  inline ::common::errorinfo* release_error();
  inline void set_allocated_error(::common::errorinfo* error);

  // @@protoc_insertion_point(class_scope:protocol.ulogout_ack)
 private:
  inline void set_has_error();
  inline void clear_has_error();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::common::errorinfo* error_;
  friend void  protobuf_AddDesc_protocol_2eproto();
  friend void protobuf_AssignDesc_protocol_2eproto();
  friend void protobuf_ShutdownFile_protocol_2eproto();

  void InitAsDefaultInstance();
  static ulogout_ack* default_instance_;
};
// -------------------------------------------------------------------

class forced_offline_ntf : public ::google::protobuf::Message {
 public:
  forced_offline_ntf();
  virtual ~forced_offline_ntf();

  forced_offline_ntf(const forced_offline_ntf& from);

  inline forced_offline_ntf& operator=(const forced_offline_ntf& from) {
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
  static const forced_offline_ntf& default_instance();

  void Swap(forced_offline_ntf* other);

  // implements Message ----------------------------------------------

  forced_offline_ntf* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const forced_offline_ntf& from);
  void MergeFrom(const forced_offline_ntf& from);
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

  // optional .common.errorinfo error = 1;
  inline bool has_error() const;
  inline void clear_error();
  static const int kErrorFieldNumber = 1;
  inline const ::common::errorinfo& error() const;
  inline ::common::errorinfo* mutable_error();
  inline ::common::errorinfo* release_error();
  inline void set_allocated_error(::common::errorinfo* error);

  // @@protoc_insertion_point(class_scope:protocol.forced_offline_ntf)
 private:
  inline void set_has_error();
  inline void clear_has_error();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::common::errorinfo* error_;
  friend void  protobuf_AddDesc_protocol_2eproto();
  friend void protobuf_AssignDesc_protocol_2eproto();
  friend void protobuf_ShutdownFile_protocol_2eproto();

  void InitAsDefaultInstance();
  static forced_offline_ntf* default_instance_;
};
// ===================================================================


// ===================================================================

// ulogin

// required uint32 uid = 1;
inline bool ulogin::has_uid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ulogin::set_has_uid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ulogin::clear_has_uid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ulogin::clear_uid() {
  uid_ = 0u;
  clear_has_uid();
}
inline ::google::protobuf::uint32 ulogin::uid() const {
  // @@protoc_insertion_point(field_get:protocol.ulogin.uid)
  return uid_;
}
inline void ulogin::set_uid(::google::protobuf::uint32 value) {
  set_has_uid();
  uid_ = value;
  // @@protoc_insertion_point(field_set:protocol.ulogin.uid)
}

// required bytes auth_code = 2;
inline bool ulogin::has_auth_code() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ulogin::set_has_auth_code() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ulogin::clear_has_auth_code() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ulogin::clear_auth_code() {
  if (auth_code_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    auth_code_->clear();
  }
  clear_has_auth_code();
}
inline const ::std::string& ulogin::auth_code() const {
  // @@protoc_insertion_point(field_get:protocol.ulogin.auth_code)
  return *auth_code_;
}
inline void ulogin::set_auth_code(const ::std::string& value) {
  set_has_auth_code();
  if (auth_code_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    auth_code_ = new ::std::string;
  }
  auth_code_->assign(value);
  // @@protoc_insertion_point(field_set:protocol.ulogin.auth_code)
}
inline void ulogin::set_auth_code(const char* value) {
  set_has_auth_code();
  if (auth_code_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    auth_code_ = new ::std::string;
  }
  auth_code_->assign(value);
  // @@protoc_insertion_point(field_set_char:protocol.ulogin.auth_code)
}
inline void ulogin::set_auth_code(const void* value, size_t size) {
  set_has_auth_code();
  if (auth_code_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    auth_code_ = new ::std::string;
  }
  auth_code_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:protocol.ulogin.auth_code)
}
inline ::std::string* ulogin::mutable_auth_code() {
  set_has_auth_code();
  if (auth_code_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    auth_code_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:protocol.ulogin.auth_code)
  return auth_code_;
}
inline ::std::string* ulogin::release_auth_code() {
  clear_has_auth_code();
  if (auth_code_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = auth_code_;
    auth_code_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ulogin::set_allocated_auth_code(::std::string* auth_code) {
  if (auth_code_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete auth_code_;
  }
  if (auth_code) {
    set_has_auth_code();
    auth_code_ = auth_code;
  } else {
    clear_has_auth_code();
    auth_code_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.ulogin.auth_code)
}

// required bytes password = 3;
inline bool ulogin::has_password() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ulogin::set_has_password() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ulogin::clear_has_password() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ulogin::clear_password() {
  if (password_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    password_->clear();
  }
  clear_has_password();
}
inline const ::std::string& ulogin::password() const {
  // @@protoc_insertion_point(field_get:protocol.ulogin.password)
  return *password_;
}
inline void ulogin::set_password(const ::std::string& value) {
  set_has_password();
  if (password_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    password_ = new ::std::string;
  }
  password_->assign(value);
  // @@protoc_insertion_point(field_set:protocol.ulogin.password)
}
inline void ulogin::set_password(const char* value) {
  set_has_password();
  if (password_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    password_ = new ::std::string;
  }
  password_->assign(value);
  // @@protoc_insertion_point(field_set_char:protocol.ulogin.password)
}
inline void ulogin::set_password(const void* value, size_t size) {
  set_has_password();
  if (password_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    password_ = new ::std::string;
  }
  password_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:protocol.ulogin.password)
}
inline ::std::string* ulogin::mutable_password() {
  set_has_password();
  if (password_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    password_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:protocol.ulogin.password)
  return password_;
}
inline ::std::string* ulogin::release_password() {
  clear_has_password();
  if (password_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = password_;
    password_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ulogin::set_allocated_password(::std::string* password) {
  if (password_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete password_;
  }
  if (password) {
    set_has_password();
    password_ = password;
  } else {
    clear_has_password();
    password_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.ulogin.password)
}

// required .common.client_info client_info = 4;
inline bool ulogin::has_client_info() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ulogin::set_has_client_info() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ulogin::clear_has_client_info() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ulogin::clear_client_info() {
  if (client_info_ != NULL) client_info_->::common::client_info::Clear();
  clear_has_client_info();
}
inline const ::common::client_info& ulogin::client_info() const {
  // @@protoc_insertion_point(field_get:protocol.ulogin.client_info)
  return client_info_ != NULL ? *client_info_ : *default_instance_->client_info_;
}
inline ::common::client_info* ulogin::mutable_client_info() {
  set_has_client_info();
  if (client_info_ == NULL) client_info_ = new ::common::client_info;
  // @@protoc_insertion_point(field_mutable:protocol.ulogin.client_info)
  return client_info_;
}
inline ::common::client_info* ulogin::release_client_info() {
  clear_has_client_info();
  ::common::client_info* temp = client_info_;
  client_info_ = NULL;
  return temp;
}
inline void ulogin::set_allocated_client_info(::common::client_info* client_info) {
  delete client_info_;
  client_info_ = client_info;
  if (client_info) {
    set_has_client_info();
  } else {
    clear_has_client_info();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.ulogin.client_info)
}

// optional bytes ser_nodeIdentify = 5;
inline bool ulogin::has_ser_nodeidentify() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void ulogin::set_has_ser_nodeidentify() {
  _has_bits_[0] |= 0x00000010u;
}
inline void ulogin::clear_has_ser_nodeidentify() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void ulogin::clear_ser_nodeidentify() {
  if (ser_nodeidentify_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ser_nodeidentify_->clear();
  }
  clear_has_ser_nodeidentify();
}
inline const ::std::string& ulogin::ser_nodeidentify() const {
  // @@protoc_insertion_point(field_get:protocol.ulogin.ser_nodeIdentify)
  return *ser_nodeidentify_;
}
inline void ulogin::set_ser_nodeidentify(const ::std::string& value) {
  set_has_ser_nodeidentify();
  if (ser_nodeidentify_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ser_nodeidentify_ = new ::std::string;
  }
  ser_nodeidentify_->assign(value);
  // @@protoc_insertion_point(field_set:protocol.ulogin.ser_nodeIdentify)
}
inline void ulogin::set_ser_nodeidentify(const char* value) {
  set_has_ser_nodeidentify();
  if (ser_nodeidentify_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ser_nodeidentify_ = new ::std::string;
  }
  ser_nodeidentify_->assign(value);
  // @@protoc_insertion_point(field_set_char:protocol.ulogin.ser_nodeIdentify)
}
inline void ulogin::set_ser_nodeidentify(const void* value, size_t size) {
  set_has_ser_nodeidentify();
  if (ser_nodeidentify_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ser_nodeidentify_ = new ::std::string;
  }
  ser_nodeidentify_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:protocol.ulogin.ser_nodeIdentify)
}
inline ::std::string* ulogin::mutable_ser_nodeidentify() {
  set_has_ser_nodeidentify();
  if (ser_nodeidentify_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ser_nodeidentify_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:protocol.ulogin.ser_nodeIdentify)
  return ser_nodeidentify_;
}
inline ::std::string* ulogin::release_ser_nodeidentify() {
  clear_has_ser_nodeidentify();
  if (ser_nodeidentify_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = ser_nodeidentify_;
    ser_nodeidentify_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ulogin::set_allocated_ser_nodeidentify(::std::string* ser_nodeidentify) {
  if (ser_nodeidentify_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete ser_nodeidentify_;
  }
  if (ser_nodeidentify) {
    set_has_ser_nodeidentify();
    ser_nodeidentify_ = ser_nodeidentify;
  } else {
    clear_has_ser_nodeidentify();
    ser_nodeidentify_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.ulogin.ser_nodeIdentify)
}

// -------------------------------------------------------------------

// ulogin_ack

// required .common.errorinfo error = 1;
inline bool ulogin_ack::has_error() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ulogin_ack::set_has_error() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ulogin_ack::clear_has_error() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ulogin_ack::clear_error() {
  if (error_ != NULL) error_->::common::errorinfo::Clear();
  clear_has_error();
}
inline const ::common::errorinfo& ulogin_ack::error() const {
  // @@protoc_insertion_point(field_get:protocol.ulogin_ack.error)
  return error_ != NULL ? *error_ : *default_instance_->error_;
}
inline ::common::errorinfo* ulogin_ack::mutable_error() {
  set_has_error();
  if (error_ == NULL) error_ = new ::common::errorinfo;
  // @@protoc_insertion_point(field_mutable:protocol.ulogin_ack.error)
  return error_;
}
inline ::common::errorinfo* ulogin_ack::release_error() {
  clear_has_error();
  ::common::errorinfo* temp = error_;
  error_ = NULL;
  return temp;
}
inline void ulogin_ack::set_allocated_error(::common::errorinfo* error) {
  delete error_;
  error_ = error;
  if (error) {
    set_has_error();
  } else {
    clear_has_error();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.ulogin_ack.error)
}

// optional uint32 beat_time = 2;
inline bool ulogin_ack::has_beat_time() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ulogin_ack::set_has_beat_time() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ulogin_ack::clear_has_beat_time() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ulogin_ack::clear_beat_time() {
  beat_time_ = 0u;
  clear_has_beat_time();
}
inline ::google::protobuf::uint32 ulogin_ack::beat_time() const {
  // @@protoc_insertion_point(field_get:protocol.ulogin_ack.beat_time)
  return beat_time_;
}
inline void ulogin_ack::set_beat_time(::google::protobuf::uint32 value) {
  set_has_beat_time();
  beat_time_ = value;
  // @@protoc_insertion_point(field_set:protocol.ulogin_ack.beat_time)
}

// optional uint32 login_prompt = 3;
inline bool ulogin_ack::has_login_prompt() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ulogin_ack::set_has_login_prompt() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ulogin_ack::clear_has_login_prompt() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ulogin_ack::clear_login_prompt() {
  login_prompt_ = 0u;
  clear_has_login_prompt();
}
inline ::google::protobuf::uint32 ulogin_ack::login_prompt() const {
  // @@protoc_insertion_point(field_get:protocol.ulogin_ack.login_prompt)
  return login_prompt_;
}
inline void ulogin_ack::set_login_prompt(::google::protobuf::uint32 value) {
  set_has_login_prompt();
  login_prompt_ = value;
  // @@protoc_insertion_point(field_set:protocol.ulogin_ack.login_prompt)
}

// -------------------------------------------------------------------

// ulogout

// required uint32 uid = 1;
inline bool ulogout::has_uid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ulogout::set_has_uid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ulogout::clear_has_uid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ulogout::clear_uid() {
  uid_ = 0u;
  clear_has_uid();
}
inline ::google::protobuf::uint32 ulogout::uid() const {
  // @@protoc_insertion_point(field_get:protocol.ulogout.uid)
  return uid_;
}
inline void ulogout::set_uid(::google::protobuf::uint32 value) {
  set_has_uid();
  uid_ = value;
  // @@protoc_insertion_point(field_set:protocol.ulogout.uid)
}

// -------------------------------------------------------------------

// ulogout_ack

// optional .common.errorinfo error = 1;
inline bool ulogout_ack::has_error() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ulogout_ack::set_has_error() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ulogout_ack::clear_has_error() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ulogout_ack::clear_error() {
  if (error_ != NULL) error_->::common::errorinfo::Clear();
  clear_has_error();
}
inline const ::common::errorinfo& ulogout_ack::error() const {
  // @@protoc_insertion_point(field_get:protocol.ulogout_ack.error)
  return error_ != NULL ? *error_ : *default_instance_->error_;
}
inline ::common::errorinfo* ulogout_ack::mutable_error() {
  set_has_error();
  if (error_ == NULL) error_ = new ::common::errorinfo;
  // @@protoc_insertion_point(field_mutable:protocol.ulogout_ack.error)
  return error_;
}
inline ::common::errorinfo* ulogout_ack::release_error() {
  clear_has_error();
  ::common::errorinfo* temp = error_;
  error_ = NULL;
  return temp;
}
inline void ulogout_ack::set_allocated_error(::common::errorinfo* error) {
  delete error_;
  error_ = error;
  if (error) {
    set_has_error();
  } else {
    clear_has_error();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.ulogout_ack.error)
}

// -------------------------------------------------------------------

// forced_offline_ntf

// optional .common.errorinfo error = 1;
inline bool forced_offline_ntf::has_error() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void forced_offline_ntf::set_has_error() {
  _has_bits_[0] |= 0x00000001u;
}
inline void forced_offline_ntf::clear_has_error() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void forced_offline_ntf::clear_error() {
  if (error_ != NULL) error_->::common::errorinfo::Clear();
  clear_has_error();
}
inline const ::common::errorinfo& forced_offline_ntf::error() const {
  // @@protoc_insertion_point(field_get:protocol.forced_offline_ntf.error)
  return error_ != NULL ? *error_ : *default_instance_->error_;
}
inline ::common::errorinfo* forced_offline_ntf::mutable_error() {
  set_has_error();
  if (error_ == NULL) error_ = new ::common::errorinfo;
  // @@protoc_insertion_point(field_mutable:protocol.forced_offline_ntf.error)
  return error_;
}
inline ::common::errorinfo* forced_offline_ntf::release_error() {
  clear_has_error();
  ::common::errorinfo* temp = error_;
  error_ = NULL;
  return temp;
}
inline void forced_offline_ntf::set_allocated_error(::common::errorinfo* error) {
  delete error_;
  error_ = error;
  if (error) {
    set_has_error();
  } else {
    clear_has_error();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.forced_offline_ntf.error)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace protocol

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_protocol_2eproto__INCLUDED