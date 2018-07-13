// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: oss_sys.proto

#ifndef PROTOBUF_oss_5fsys_2eproto__INCLUDED
#define PROTOBUF_oss_5fsys_2eproto__INCLUDED

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
void  protobuf_AddDesc_oss_5fsys_2eproto();
void protobuf_AssignDesc_oss_5fsys_2eproto();
void protobuf_ShutdownFile_oss_5fsys_2eproto();

class ConfigInfo;
class WorkerLoad;
class ConnectWorker;
class TargetWorker;

// ===================================================================

class ConfigInfo : public ::google::protobuf::Message {
 public:
  ConfigInfo();
  virtual ~ConfigInfo();

  ConfigInfo(const ConfigInfo& from);

  inline ConfigInfo& operator=(const ConfigInfo& from) {
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
  static const ConfigInfo& default_instance();

  void Swap(ConfigInfo* other);

  // implements Message ----------------------------------------------

  ConfigInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ConfigInfo& from);
  void MergeFrom(const ConfigInfo& from);
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

  // required string file_name = 1;
  inline bool has_file_name() const;
  inline void clear_file_name();
  static const int kFileNameFieldNumber = 1;
  inline const ::std::string& file_name() const;
  inline void set_file_name(const ::std::string& value);
  inline void set_file_name(const char* value);
  inline void set_file_name(const char* value, size_t size);
  inline ::std::string* mutable_file_name();
  inline ::std::string* release_file_name();
  inline void set_allocated_file_name(::std::string* file_name);

  // required string file_content = 2;
  inline bool has_file_content() const;
  inline void clear_file_content();
  static const int kFileContentFieldNumber = 2;
  inline const ::std::string& file_content() const;
  inline void set_file_content(const ::std::string& value);
  inline void set_file_content(const char* value);
  inline void set_file_content(const char* value, size_t size);
  inline ::std::string* mutable_file_content();
  inline ::std::string* release_file_content();
  inline void set_allocated_file_content(::std::string* file_content);

  // @@protoc_insertion_point(class_scope:ConfigInfo)
 private:
  inline void set_has_file_name();
  inline void clear_has_file_name();
  inline void set_has_file_content();
  inline void clear_has_file_content();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* file_name_;
  ::std::string* file_content_;
  friend void  protobuf_AddDesc_oss_5fsys_2eproto();
  friend void protobuf_AssignDesc_oss_5fsys_2eproto();
  friend void protobuf_ShutdownFile_oss_5fsys_2eproto();

  void InitAsDefaultInstance();
  static ConfigInfo* default_instance_;
};
// -------------------------------------------------------------------

class WorkerLoad : public ::google::protobuf::Message {
 public:
  WorkerLoad();
  virtual ~WorkerLoad();

  WorkerLoad(const WorkerLoad& from);

  inline WorkerLoad& operator=(const WorkerLoad& from) {
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
  static const WorkerLoad& default_instance();

  void Swap(WorkerLoad* other);

  // implements Message ----------------------------------------------

  WorkerLoad* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const WorkerLoad& from);
  void MergeFrom(const WorkerLoad& from);
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

  // required int32 pid = 1;
  inline bool has_pid() const;
  inline void clear_pid();
  static const int kPidFieldNumber = 1;
  inline ::google::protobuf::int32 pid() const;
  inline void set_pid(::google::protobuf::int32 value);

  // required int32 load = 2;
  inline bool has_load() const;
  inline void clear_load();
  static const int kLoadFieldNumber = 2;
  inline ::google::protobuf::int32 load() const;
  inline void set_load(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:WorkerLoad)
 private:
  inline void set_has_pid();
  inline void clear_has_pid();
  inline void set_has_load();
  inline void clear_has_load();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::int32 pid_;
  ::google::protobuf::int32 load_;
  friend void  protobuf_AddDesc_oss_5fsys_2eproto();
  friend void protobuf_AssignDesc_oss_5fsys_2eproto();
  friend void protobuf_ShutdownFile_oss_5fsys_2eproto();

  void InitAsDefaultInstance();
  static WorkerLoad* default_instance_;
};
// -------------------------------------------------------------------

class ConnectWorker : public ::google::protobuf::Message {
 public:
  ConnectWorker();
  virtual ~ConnectWorker();

  ConnectWorker(const ConnectWorker& from);

  inline ConnectWorker& operator=(const ConnectWorker& from) {
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
  static const ConnectWorker& default_instance();

  void Swap(ConnectWorker* other);

  // implements Message ----------------------------------------------

  ConnectWorker* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ConnectWorker& from);
  void MergeFrom(const ConnectWorker& from);
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

  // required int32 worker_index = 1;
  inline bool has_worker_index() const;
  inline void clear_worker_index();
  static const int kWorkerIndexFieldNumber = 1;
  inline ::google::protobuf::int32 worker_index() const;
  inline void set_worker_index(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:ConnectWorker)
 private:
  inline void set_has_worker_index();
  inline void clear_has_worker_index();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::int32 worker_index_;
  friend void  protobuf_AddDesc_oss_5fsys_2eproto();
  friend void protobuf_AssignDesc_oss_5fsys_2eproto();
  friend void protobuf_ShutdownFile_oss_5fsys_2eproto();

  void InitAsDefaultInstance();
  static ConnectWorker* default_instance_;
};
// -------------------------------------------------------------------

class TargetWorker : public ::google::protobuf::Message {
 public:
  TargetWorker();
  virtual ~TargetWorker();

  TargetWorker(const TargetWorker& from);

  inline TargetWorker& operator=(const TargetWorker& from) {
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
  static const TargetWorker& default_instance();

  void Swap(TargetWorker* other);

  // implements Message ----------------------------------------------

  TargetWorker* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const TargetWorker& from);
  void MergeFrom(const TargetWorker& from);
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

  // required string worker_identify = 2;
  inline bool has_worker_identify() const;
  inline void clear_worker_identify();
  static const int kWorkerIdentifyFieldNumber = 2;
  inline const ::std::string& worker_identify() const;
  inline void set_worker_identify(const ::std::string& value);
  inline void set_worker_identify(const char* value);
  inline void set_worker_identify(const char* value, size_t size);
  inline ::std::string* mutable_worker_identify();
  inline ::std::string* release_worker_identify();
  inline void set_allocated_worker_identify(::std::string* worker_identify);

  // required string node_type = 3;
  inline bool has_node_type() const;
  inline void clear_node_type();
  static const int kNodeTypeFieldNumber = 3;
  inline const ::std::string& node_type() const;
  inline void set_node_type(const ::std::string& value);
  inline void set_node_type(const char* value);
  inline void set_node_type(const char* value, size_t size);
  inline ::std::string* mutable_node_type();
  inline ::std::string* release_node_type();
  inline void set_allocated_node_type(::std::string* node_type);

  // optional string err_msg = 4;
  inline bool has_err_msg() const;
  inline void clear_err_msg();
  static const int kErrMsgFieldNumber = 4;
  inline const ::std::string& err_msg() const;
  inline void set_err_msg(const ::std::string& value);
  inline void set_err_msg(const char* value);
  inline void set_err_msg(const char* value, size_t size);
  inline ::std::string* mutable_err_msg();
  inline ::std::string* release_err_msg();
  inline void set_allocated_err_msg(::std::string* err_msg);

  // @@protoc_insertion_point(class_scope:TargetWorker)
 private:
  inline void set_has_err_no();
  inline void clear_has_err_no();
  inline void set_has_worker_identify();
  inline void clear_has_worker_identify();
  inline void set_has_node_type();
  inline void clear_has_node_type();
  inline void set_has_err_msg();
  inline void clear_has_err_msg();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* worker_identify_;
  ::std::string* node_type_;
  ::std::string* err_msg_;
  ::google::protobuf::int32 err_no_;
  friend void  protobuf_AddDesc_oss_5fsys_2eproto();
  friend void protobuf_AssignDesc_oss_5fsys_2eproto();
  friend void protobuf_ShutdownFile_oss_5fsys_2eproto();

  void InitAsDefaultInstance();
  static TargetWorker* default_instance_;
};
// ===================================================================


// ===================================================================

// ConfigInfo

// required string file_name = 1;
inline bool ConfigInfo::has_file_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ConfigInfo::set_has_file_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ConfigInfo::clear_has_file_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ConfigInfo::clear_file_name() {
  if (file_name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_name_->clear();
  }
  clear_has_file_name();
}
inline const ::std::string& ConfigInfo::file_name() const {
  // @@protoc_insertion_point(field_get:ConfigInfo.file_name)
  return *file_name_;
}
inline void ConfigInfo::set_file_name(const ::std::string& value) {
  set_has_file_name();
  if (file_name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_name_ = new ::std::string;
  }
  file_name_->assign(value);
  // @@protoc_insertion_point(field_set:ConfigInfo.file_name)
}
inline void ConfigInfo::set_file_name(const char* value) {
  set_has_file_name();
  if (file_name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_name_ = new ::std::string;
  }
  file_name_->assign(value);
  // @@protoc_insertion_point(field_set_char:ConfigInfo.file_name)
}
inline void ConfigInfo::set_file_name(const char* value, size_t size) {
  set_has_file_name();
  if (file_name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_name_ = new ::std::string;
  }
  file_name_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ConfigInfo.file_name)
}
inline ::std::string* ConfigInfo::mutable_file_name() {
  set_has_file_name();
  if (file_name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_name_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ConfigInfo.file_name)
  return file_name_;
}
inline ::std::string* ConfigInfo::release_file_name() {
  clear_has_file_name();
  if (file_name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = file_name_;
    file_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ConfigInfo::set_allocated_file_name(::std::string* file_name) {
  if (file_name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete file_name_;
  }
  if (file_name) {
    set_has_file_name();
    file_name_ = file_name;
  } else {
    clear_has_file_name();
    file_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:ConfigInfo.file_name)
}

// required string file_content = 2;
inline bool ConfigInfo::has_file_content() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ConfigInfo::set_has_file_content() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ConfigInfo::clear_has_file_content() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ConfigInfo::clear_file_content() {
  if (file_content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_content_->clear();
  }
  clear_has_file_content();
}
inline const ::std::string& ConfigInfo::file_content() const {
  // @@protoc_insertion_point(field_get:ConfigInfo.file_content)
  return *file_content_;
}
inline void ConfigInfo::set_file_content(const ::std::string& value) {
  set_has_file_content();
  if (file_content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_content_ = new ::std::string;
  }
  file_content_->assign(value);
  // @@protoc_insertion_point(field_set:ConfigInfo.file_content)
}
inline void ConfigInfo::set_file_content(const char* value) {
  set_has_file_content();
  if (file_content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_content_ = new ::std::string;
  }
  file_content_->assign(value);
  // @@protoc_insertion_point(field_set_char:ConfigInfo.file_content)
}
inline void ConfigInfo::set_file_content(const char* value, size_t size) {
  set_has_file_content();
  if (file_content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_content_ = new ::std::string;
  }
  file_content_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ConfigInfo.file_content)
}
inline ::std::string* ConfigInfo::mutable_file_content() {
  set_has_file_content();
  if (file_content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_content_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ConfigInfo.file_content)
  return file_content_;
}
inline ::std::string* ConfigInfo::release_file_content() {
  clear_has_file_content();
  if (file_content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = file_content_;
    file_content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ConfigInfo::set_allocated_file_content(::std::string* file_content) {
  if (file_content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete file_content_;
  }
  if (file_content) {
    set_has_file_content();
    file_content_ = file_content;
  } else {
    clear_has_file_content();
    file_content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:ConfigInfo.file_content)
}

// -------------------------------------------------------------------

// WorkerLoad

// required int32 pid = 1;
inline bool WorkerLoad::has_pid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void WorkerLoad::set_has_pid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void WorkerLoad::clear_has_pid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void WorkerLoad::clear_pid() {
  pid_ = 0;
  clear_has_pid();
}
inline ::google::protobuf::int32 WorkerLoad::pid() const {
  // @@protoc_insertion_point(field_get:WorkerLoad.pid)
  return pid_;
}
inline void WorkerLoad::set_pid(::google::protobuf::int32 value) {
  set_has_pid();
  pid_ = value;
  // @@protoc_insertion_point(field_set:WorkerLoad.pid)
}

// required int32 load = 2;
inline bool WorkerLoad::has_load() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void WorkerLoad::set_has_load() {
  _has_bits_[0] |= 0x00000002u;
}
inline void WorkerLoad::clear_has_load() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void WorkerLoad::clear_load() {
  load_ = 0;
  clear_has_load();
}
inline ::google::protobuf::int32 WorkerLoad::load() const {
  // @@protoc_insertion_point(field_get:WorkerLoad.load)
  return load_;
}
inline void WorkerLoad::set_load(::google::protobuf::int32 value) {
  set_has_load();
  load_ = value;
  // @@protoc_insertion_point(field_set:WorkerLoad.load)
}

// -------------------------------------------------------------------

// ConnectWorker

// required int32 worker_index = 1;
inline bool ConnectWorker::has_worker_index() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ConnectWorker::set_has_worker_index() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ConnectWorker::clear_has_worker_index() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ConnectWorker::clear_worker_index() {
  worker_index_ = 0;
  clear_has_worker_index();
}
inline ::google::protobuf::int32 ConnectWorker::worker_index() const {
  // @@protoc_insertion_point(field_get:ConnectWorker.worker_index)
  return worker_index_;
}
inline void ConnectWorker::set_worker_index(::google::protobuf::int32 value) {
  set_has_worker_index();
  worker_index_ = value;
  // @@protoc_insertion_point(field_set:ConnectWorker.worker_index)
}

// -------------------------------------------------------------------

// TargetWorker

// required int32 err_no = 1;
inline bool TargetWorker::has_err_no() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TargetWorker::set_has_err_no() {
  _has_bits_[0] |= 0x00000001u;
}
inline void TargetWorker::clear_has_err_no() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void TargetWorker::clear_err_no() {
  err_no_ = 0;
  clear_has_err_no();
}
inline ::google::protobuf::int32 TargetWorker::err_no() const {
  // @@protoc_insertion_point(field_get:TargetWorker.err_no)
  return err_no_;
}
inline void TargetWorker::set_err_no(::google::protobuf::int32 value) {
  set_has_err_no();
  err_no_ = value;
  // @@protoc_insertion_point(field_set:TargetWorker.err_no)
}

// required string worker_identify = 2;
inline bool TargetWorker::has_worker_identify() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void TargetWorker::set_has_worker_identify() {
  _has_bits_[0] |= 0x00000002u;
}
inline void TargetWorker::clear_has_worker_identify() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void TargetWorker::clear_worker_identify() {
  if (worker_identify_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    worker_identify_->clear();
  }
  clear_has_worker_identify();
}
inline const ::std::string& TargetWorker::worker_identify() const {
  // @@protoc_insertion_point(field_get:TargetWorker.worker_identify)
  return *worker_identify_;
}
inline void TargetWorker::set_worker_identify(const ::std::string& value) {
  set_has_worker_identify();
  if (worker_identify_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    worker_identify_ = new ::std::string;
  }
  worker_identify_->assign(value);
  // @@protoc_insertion_point(field_set:TargetWorker.worker_identify)
}
inline void TargetWorker::set_worker_identify(const char* value) {
  set_has_worker_identify();
  if (worker_identify_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    worker_identify_ = new ::std::string;
  }
  worker_identify_->assign(value);
  // @@protoc_insertion_point(field_set_char:TargetWorker.worker_identify)
}
inline void TargetWorker::set_worker_identify(const char* value, size_t size) {
  set_has_worker_identify();
  if (worker_identify_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    worker_identify_ = new ::std::string;
  }
  worker_identify_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:TargetWorker.worker_identify)
}
inline ::std::string* TargetWorker::mutable_worker_identify() {
  set_has_worker_identify();
  if (worker_identify_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    worker_identify_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:TargetWorker.worker_identify)
  return worker_identify_;
}
inline ::std::string* TargetWorker::release_worker_identify() {
  clear_has_worker_identify();
  if (worker_identify_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = worker_identify_;
    worker_identify_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void TargetWorker::set_allocated_worker_identify(::std::string* worker_identify) {
  if (worker_identify_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete worker_identify_;
  }
  if (worker_identify) {
    set_has_worker_identify();
    worker_identify_ = worker_identify;
  } else {
    clear_has_worker_identify();
    worker_identify_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:TargetWorker.worker_identify)
}

// required string node_type = 3;
inline bool TargetWorker::has_node_type() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void TargetWorker::set_has_node_type() {
  _has_bits_[0] |= 0x00000004u;
}
inline void TargetWorker::clear_has_node_type() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void TargetWorker::clear_node_type() {
  if (node_type_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    node_type_->clear();
  }
  clear_has_node_type();
}
inline const ::std::string& TargetWorker::node_type() const {
  // @@protoc_insertion_point(field_get:TargetWorker.node_type)
  return *node_type_;
}
inline void TargetWorker::set_node_type(const ::std::string& value) {
  set_has_node_type();
  if (node_type_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    node_type_ = new ::std::string;
  }
  node_type_->assign(value);
  // @@protoc_insertion_point(field_set:TargetWorker.node_type)
}
inline void TargetWorker::set_node_type(const char* value) {
  set_has_node_type();
  if (node_type_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    node_type_ = new ::std::string;
  }
  node_type_->assign(value);
  // @@protoc_insertion_point(field_set_char:TargetWorker.node_type)
}
inline void TargetWorker::set_node_type(const char* value, size_t size) {
  set_has_node_type();
  if (node_type_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    node_type_ = new ::std::string;
  }
  node_type_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:TargetWorker.node_type)
}
inline ::std::string* TargetWorker::mutable_node_type() {
  set_has_node_type();
  if (node_type_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    node_type_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:TargetWorker.node_type)
  return node_type_;
}
inline ::std::string* TargetWorker::release_node_type() {
  clear_has_node_type();
  if (node_type_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = node_type_;
    node_type_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void TargetWorker::set_allocated_node_type(::std::string* node_type) {
  if (node_type_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete node_type_;
  }
  if (node_type) {
    set_has_node_type();
    node_type_ = node_type;
  } else {
    clear_has_node_type();
    node_type_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:TargetWorker.node_type)
}

// optional string err_msg = 4;
inline bool TargetWorker::has_err_msg() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void TargetWorker::set_has_err_msg() {
  _has_bits_[0] |= 0x00000008u;
}
inline void TargetWorker::clear_has_err_msg() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void TargetWorker::clear_err_msg() {
  if (err_msg_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    err_msg_->clear();
  }
  clear_has_err_msg();
}
inline const ::std::string& TargetWorker::err_msg() const {
  // @@protoc_insertion_point(field_get:TargetWorker.err_msg)
  return *err_msg_;
}
inline void TargetWorker::set_err_msg(const ::std::string& value) {
  set_has_err_msg();
  if (err_msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    err_msg_ = new ::std::string;
  }
  err_msg_->assign(value);
  // @@protoc_insertion_point(field_set:TargetWorker.err_msg)
}
inline void TargetWorker::set_err_msg(const char* value) {
  set_has_err_msg();
  if (err_msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    err_msg_ = new ::std::string;
  }
  err_msg_->assign(value);
  // @@protoc_insertion_point(field_set_char:TargetWorker.err_msg)
}
inline void TargetWorker::set_err_msg(const char* value, size_t size) {
  set_has_err_msg();
  if (err_msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    err_msg_ = new ::std::string;
  }
  err_msg_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:TargetWorker.err_msg)
}
inline ::std::string* TargetWorker::mutable_err_msg() {
  set_has_err_msg();
  if (err_msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    err_msg_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:TargetWorker.err_msg)
  return err_msg_;
}
inline ::std::string* TargetWorker::release_err_msg() {
  clear_has_err_msg();
  if (err_msg_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = err_msg_;
    err_msg_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void TargetWorker::set_allocated_err_msg(::std::string* err_msg) {
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
  // @@protoc_insertion_point(field_set_allocated:TargetWorker.err_msg)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_oss_5fsys_2eproto__INCLUDED