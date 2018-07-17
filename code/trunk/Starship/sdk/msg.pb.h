// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: msg.proto

#ifndef PROTOBUF_msg_2eproto__INCLUDED
#define PROTOBUF_msg_2eproto__INCLUDED

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

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_msg_2eproto();
void protobuf_AssignDesc_msg_2eproto();
void protobuf_ShutdownFile_msg_2eproto();

class MsgHead;
class MsgBody;
class OrdinaryResponse;

// ===================================================================

class MsgHead : public ::google::protobuf::Message {
 public:
  MsgHead();
  virtual ~MsgHead();

  MsgHead(const MsgHead& from);

  inline MsgHead& operator=(const MsgHead& from) {
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
  static const MsgHead& default_instance();

  void Swap(MsgHead* other);

  // implements Message ----------------------------------------------

  MsgHead* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MsgHead& from);
  void MergeFrom(const MsgHead& from);
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

  // required fixed32 cmd = 1;
  inline bool has_cmd() const;
  inline void clear_cmd();
  static const int kCmdFieldNumber = 1;
  inline ::google::protobuf::uint32 cmd() const;
  inline void set_cmd(::google::protobuf::uint32 value);

  // required fixed32 msgbody_len = 2;
  inline bool has_msgbody_len() const;
  inline void clear_msgbody_len();
  static const int kMsgbodyLenFieldNumber = 2;
  inline ::google::protobuf::uint32 msgbody_len() const;
  inline void set_msgbody_len(::google::protobuf::uint32 value);

  // required fixed32 seq = 3;
  inline bool has_seq() const;
  inline void clear_seq();
  static const int kSeqFieldNumber = 3;
  inline ::google::protobuf::uint32 seq() const;
  inline void set_seq(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:MsgHead)
 private:
  inline void set_has_cmd();
  inline void clear_has_cmd();
  inline void set_has_msgbody_len();
  inline void clear_has_msgbody_len();
  inline void set_has_seq();
  inline void clear_has_seq();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 cmd_;
  ::google::protobuf::uint32 msgbody_len_;
  ::google::protobuf::uint32 seq_;
  friend void  protobuf_AddDesc_msg_2eproto();
  friend void protobuf_AssignDesc_msg_2eproto();
  friend void protobuf_ShutdownFile_msg_2eproto();

  void InitAsDefaultInstance();
  static MsgHead* default_instance_;
};
// -------------------------------------------------------------------

class MsgBody : public ::google::protobuf::Message {
 public:
  MsgBody();
  virtual ~MsgBody();

  MsgBody(const MsgBody& from);

  inline MsgBody& operator=(const MsgBody& from) {
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
  static const MsgBody& default_instance();

  void Swap(MsgBody* other);

  // implements Message ----------------------------------------------

  MsgBody* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MsgBody& from);
  void MergeFrom(const MsgBody& from);
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

  // required bytes body = 1;
  inline bool has_body() const;
  inline void clear_body();
  static const int kBodyFieldNumber = 1;
  inline const ::std::string& body() const;
  inline void set_body(const ::std::string& value);
  inline void set_body(const char* value);
  inline void set_body(const void* value, size_t size);
  inline ::std::string* mutable_body();
  inline ::std::string* release_body();
  inline void set_allocated_body(::std::string* body);

  // optional uint32 session_id = 2;
  inline bool has_session_id() const;
  inline void clear_session_id();
  static const int kSessionIdFieldNumber = 2;
  inline ::google::protobuf::uint32 session_id() const;
  inline void set_session_id(::google::protobuf::uint32 value);

  // optional string session = 3;
  inline bool has_session() const;
  inline void clear_session();
  static const int kSessionFieldNumber = 3;
  inline const ::std::string& session() const;
  inline void set_session(const ::std::string& value);
  inline void set_session(const char* value);
  inline void set_session(const char* value, size_t size);
  inline ::std::string* mutable_session();
  inline ::std::string* release_session();
  inline void set_allocated_session(::std::string* session);

  // optional bytes additional = 4;
  inline bool has_additional() const;
  inline void clear_additional();
  static const int kAdditionalFieldNumber = 4;
  inline const ::std::string& additional() const;
  inline void set_additional(const ::std::string& value);
  inline void set_additional(const char* value);
  inline void set_additional(const void* value, size_t size);
  inline ::std::string* mutable_additional();
  inline ::std::string* release_additional();
  inline void set_allocated_additional(::std::string* additional);

  // @@protoc_insertion_point(class_scope:MsgBody)
 private:
  inline void set_has_body();
  inline void clear_has_body();
  inline void set_has_session_id();
  inline void clear_has_session_id();
  inline void set_has_session();
  inline void clear_has_session();
  inline void set_has_additional();
  inline void clear_has_additional();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* body_;
  ::std::string* session_;
  ::std::string* additional_;
  ::google::protobuf::uint32 session_id_;
  friend void  protobuf_AddDesc_msg_2eproto();
  friend void protobuf_AssignDesc_msg_2eproto();
  friend void protobuf_ShutdownFile_msg_2eproto();

  void InitAsDefaultInstance();
  static MsgBody* default_instance_;
};
// -------------------------------------------------------------------

class OrdinaryResponse : public ::google::protobuf::Message {
 public:
  OrdinaryResponse();
  virtual ~OrdinaryResponse();

  OrdinaryResponse(const OrdinaryResponse& from);

  inline OrdinaryResponse& operator=(const OrdinaryResponse& from) {
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
  static const OrdinaryResponse& default_instance();

  void Swap(OrdinaryResponse* other);

  // implements Message ----------------------------------------------

  OrdinaryResponse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const OrdinaryResponse& from);
  void MergeFrom(const OrdinaryResponse& from);
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

  // required int32 err_no = 1;
  inline bool has_err_no() const;
  inline void clear_err_no();
  static const int kErrNoFieldNumber = 1;
  inline ::google::protobuf::int32 err_no() const;
  inline void set_err_no(::google::protobuf::int32 value);

  // optional bytes err_msg = 2;
  inline bool has_err_msg() const;
  inline void clear_err_msg();
  static const int kErrMsgFieldNumber = 2;
  inline const ::std::string& err_msg() const;
  inline void set_err_msg(const ::std::string& value);
  inline void set_err_msg(const char* value);
  inline void set_err_msg(const void* value, size_t size);
  inline ::std::string* mutable_err_msg();
  inline ::std::string* release_err_msg();
  inline void set_allocated_err_msg(::std::string* err_msg);

  // @@protoc_insertion_point(class_scope:OrdinaryResponse)
 private:
  inline void set_has_err_no();
  inline void clear_has_err_no();
  inline void set_has_err_msg();
  inline void clear_has_err_msg();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* err_msg_;
  ::google::protobuf::int32 err_no_;
  friend void  protobuf_AddDesc_msg_2eproto();
  friend void protobuf_AssignDesc_msg_2eproto();
  friend void protobuf_ShutdownFile_msg_2eproto();

  void InitAsDefaultInstance();
  static OrdinaryResponse* default_instance_;
};
// ===================================================================


// ===================================================================

// MsgHead

// required fixed32 cmd = 1;
inline bool MsgHead::has_cmd() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MsgHead::set_has_cmd() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MsgHead::clear_has_cmd() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MsgHead::clear_cmd() {
  cmd_ = 0u;
  clear_has_cmd();
}
inline ::google::protobuf::uint32 MsgHead::cmd() const {
  // @@protoc_insertion_point(field_get:MsgHead.cmd)
  return cmd_;
}
inline void MsgHead::set_cmd(::google::protobuf::uint32 value) {
  set_has_cmd();
  cmd_ = value;
  // @@protoc_insertion_point(field_set:MsgHead.cmd)
}

// required fixed32 msgbody_len = 2;
inline bool MsgHead::has_msgbody_len() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MsgHead::set_has_msgbody_len() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MsgHead::clear_has_msgbody_len() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MsgHead::clear_msgbody_len() {
  msgbody_len_ = 0u;
  clear_has_msgbody_len();
}
inline ::google::protobuf::uint32 MsgHead::msgbody_len() const {
  // @@protoc_insertion_point(field_get:MsgHead.msgbody_len)
  return msgbody_len_;
}
inline void MsgHead::set_msgbody_len(::google::protobuf::uint32 value) {
  set_has_msgbody_len();
  msgbody_len_ = value;
  // @@protoc_insertion_point(field_set:MsgHead.msgbody_len)
}

// required fixed32 seq = 3;
inline bool MsgHead::has_seq() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void MsgHead::set_has_seq() {
  _has_bits_[0] |= 0x00000004u;
}
inline void MsgHead::clear_has_seq() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void MsgHead::clear_seq() {
  seq_ = 0u;
  clear_has_seq();
}
inline ::google::protobuf::uint32 MsgHead::seq() const {
  // @@protoc_insertion_point(field_get:MsgHead.seq)
  return seq_;
}
inline void MsgHead::set_seq(::google::protobuf::uint32 value) {
  set_has_seq();
  seq_ = value;
  // @@protoc_insertion_point(field_set:MsgHead.seq)
}

// -------------------------------------------------------------------

// MsgBody

// required bytes body = 1;
inline bool MsgBody::has_body() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MsgBody::set_has_body() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MsgBody::clear_has_body() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MsgBody::clear_body() {
  if (body_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    body_->clear();
  }
  clear_has_body();
}
inline const ::std::string& MsgBody::body() const {
  // @@protoc_insertion_point(field_get:MsgBody.body)
  return *body_;
}
inline void MsgBody::set_body(const ::std::string& value) {
  set_has_body();
  if (body_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    body_ = new ::std::string;
  }
  body_->assign(value);
  // @@protoc_insertion_point(field_set:MsgBody.body)
}
inline void MsgBody::set_body(const char* value) {
  set_has_body();
  if (body_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    body_ = new ::std::string;
  }
  body_->assign(value);
  // @@protoc_insertion_point(field_set_char:MsgBody.body)
}
inline void MsgBody::set_body(const void* value, size_t size) {
  set_has_body();
  if (body_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    body_ = new ::std::string;
  }
  body_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:MsgBody.body)
}
inline ::std::string* MsgBody::mutable_body() {
  set_has_body();
  if (body_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    body_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:MsgBody.body)
  return body_;
}
inline ::std::string* MsgBody::release_body() {
  clear_has_body();
  if (body_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = body_;
    body_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MsgBody::set_allocated_body(::std::string* body) {
  if (body_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete body_;
  }
  if (body) {
    set_has_body();
    body_ = body;
  } else {
    clear_has_body();
    body_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:MsgBody.body)
}

// optional uint32 session_id = 2;
inline bool MsgBody::has_session_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MsgBody::set_has_session_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MsgBody::clear_has_session_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MsgBody::clear_session_id() {
  session_id_ = 0u;
  clear_has_session_id();
}
inline ::google::protobuf::uint32 MsgBody::session_id() const {
  // @@protoc_insertion_point(field_get:MsgBody.session_id)
  return session_id_;
}
inline void MsgBody::set_session_id(::google::protobuf::uint32 value) {
  set_has_session_id();
  session_id_ = value;
  // @@protoc_insertion_point(field_set:MsgBody.session_id)
}

// optional string session = 3;
inline bool MsgBody::has_session() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void MsgBody::set_has_session() {
  _has_bits_[0] |= 0x00000004u;
}
inline void MsgBody::clear_has_session() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void MsgBody::clear_session() {
  if (session_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    session_->clear();
  }
  clear_has_session();
}
inline const ::std::string& MsgBody::session() const {
  // @@protoc_insertion_point(field_get:MsgBody.session)
  return *session_;
}
inline void MsgBody::set_session(const ::std::string& value) {
  set_has_session();
  if (session_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    session_ = new ::std::string;
  }
  session_->assign(value);
  // @@protoc_insertion_point(field_set:MsgBody.session)
}
inline void MsgBody::set_session(const char* value) {
  set_has_session();
  if (session_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    session_ = new ::std::string;
  }
  session_->assign(value);
  // @@protoc_insertion_point(field_set_char:MsgBody.session)
}
inline void MsgBody::set_session(const char* value, size_t size) {
  set_has_session();
  if (session_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    session_ = new ::std::string;
  }
  session_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:MsgBody.session)
}
inline ::std::string* MsgBody::mutable_session() {
  set_has_session();
  if (session_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    session_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:MsgBody.session)
  return session_;
}
inline ::std::string* MsgBody::release_session() {
  clear_has_session();
  if (session_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = session_;
    session_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MsgBody::set_allocated_session(::std::string* session) {
  if (session_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete session_;
  }
  if (session) {
    set_has_session();
    session_ = session;
  } else {
    clear_has_session();
    session_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:MsgBody.session)
}

// optional bytes additional = 4;
inline bool MsgBody::has_additional() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void MsgBody::set_has_additional() {
  _has_bits_[0] |= 0x00000008u;
}
inline void MsgBody::clear_has_additional() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void MsgBody::clear_additional() {
  if (additional_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    additional_->clear();
  }
  clear_has_additional();
}
inline const ::std::string& MsgBody::additional() const {
  // @@protoc_insertion_point(field_get:MsgBody.additional)
  return *additional_;
}
inline void MsgBody::set_additional(const ::std::string& value) {
  set_has_additional();
  if (additional_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    additional_ = new ::std::string;
  }
  additional_->assign(value);
  // @@protoc_insertion_point(field_set:MsgBody.additional)
}
inline void MsgBody::set_additional(const char* value) {
  set_has_additional();
  if (additional_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    additional_ = new ::std::string;
  }
  additional_->assign(value);
  // @@protoc_insertion_point(field_set_char:MsgBody.additional)
}
inline void MsgBody::set_additional(const void* value, size_t size) {
  set_has_additional();
  if (additional_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    additional_ = new ::std::string;
  }
  additional_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:MsgBody.additional)
}
inline ::std::string* MsgBody::mutable_additional() {
  set_has_additional();
  if (additional_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    additional_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:MsgBody.additional)
  return additional_;
}
inline ::std::string* MsgBody::release_additional() {
  clear_has_additional();
  if (additional_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = additional_;
    additional_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MsgBody::set_allocated_additional(::std::string* additional) {
  if (additional_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete additional_;
  }
  if (additional) {
    set_has_additional();
    additional_ = additional;
  } else {
    clear_has_additional();
    additional_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:MsgBody.additional)
}

// -------------------------------------------------------------------

// OrdinaryResponse

// required int32 err_no = 1;
inline bool OrdinaryResponse::has_err_no() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void OrdinaryResponse::set_has_err_no() {
  _has_bits_[0] |= 0x00000001u;
}
inline void OrdinaryResponse::clear_has_err_no() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void OrdinaryResponse::clear_err_no() {
  err_no_ = 0;
  clear_has_err_no();
}
inline ::google::protobuf::int32 OrdinaryResponse::err_no() const {
  // @@protoc_insertion_point(field_get:OrdinaryResponse.err_no)
  return err_no_;
}
inline void OrdinaryResponse::set_err_no(::google::protobuf::int32 value) {
  set_has_err_no();
  err_no_ = value;
  // @@protoc_insertion_point(field_set:OrdinaryResponse.err_no)
}

// optional bytes err_msg = 2;
inline bool OrdinaryResponse::has_err_msg() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void OrdinaryResponse::set_has_err_msg() {
  _has_bits_[0] |= 0x00000002u;
}
inline void OrdinaryResponse::clear_has_err_msg() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void OrdinaryResponse::clear_err_msg() {
  if (err_msg_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    err_msg_->clear();
  }
  clear_has_err_msg();
}
inline const ::std::string& OrdinaryResponse::err_msg() const {
  // @@protoc_insertion_point(field_get:OrdinaryResponse.err_msg)
  return *err_msg_;
}
inline void OrdinaryResponse::set_err_msg(const ::std::string& value) {
  set_has_err_msg();
  if (err_msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    err_msg_ = new ::std::string;
  }
  err_msg_->assign(value);
  // @@protoc_insertion_point(field_set:OrdinaryResponse.err_msg)
}
inline void OrdinaryResponse::set_err_msg(const char* value) {
  set_has_err_msg();
  if (err_msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    err_msg_ = new ::std::string;
  }
  err_msg_->assign(value);
  // @@protoc_insertion_point(field_set_char:OrdinaryResponse.err_msg)
}
inline void OrdinaryResponse::set_err_msg(const void* value, size_t size) {
  set_has_err_msg();
  if (err_msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    err_msg_ = new ::std::string;
  }
  err_msg_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:OrdinaryResponse.err_msg)
}
inline ::std::string* OrdinaryResponse::mutable_err_msg() {
  set_has_err_msg();
  if (err_msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    err_msg_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:OrdinaryResponse.err_msg)
  return err_msg_;
}
inline ::std::string* OrdinaryResponse::release_err_msg() {
  clear_has_err_msg();
  if (err_msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = err_msg_;
    err_msg_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void OrdinaryResponse::set_allocated_err_msg(::std::string* err_msg) {
  if (err_msg_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete err_msg_;
  }
  if (err_msg) {
    set_has_err_msg();
    err_msg_ = err_msg;
  } else {
    clear_has_err_msg();
    err_msg_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:OrdinaryResponse.err_msg)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_msg_2eproto__INCLUDED