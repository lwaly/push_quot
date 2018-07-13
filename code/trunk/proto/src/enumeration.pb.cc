// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: enumeration.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "enumeration.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace enumeration {

namespace {

const ::google::protobuf::EnumDescriptor* E_CLIENT_TYPE_descriptor_ = NULL;
const ::google::protobuf::EnumDescriptor* E_ACCOUNT_LOGIN_TYPE_descriptor_ = NULL;
const ::google::protobuf::EnumDescriptor* E_OPERATE_TYPE_descriptor_ = NULL;
const ::google::protobuf::EnumDescriptor* E_USER_STATUS_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_enumeration_2eproto() {
  protobuf_AddDesc_enumeration_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "enumeration.proto");
  GOOGLE_CHECK(file != NULL);
  E_CLIENT_TYPE_descriptor_ = file->enum_type(0);
  E_ACCOUNT_LOGIN_TYPE_descriptor_ = file->enum_type(1);
  E_OPERATE_TYPE_descriptor_ = file->enum_type(2);
  E_USER_STATUS_descriptor_ = file->enum_type(3);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_enumeration_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

}  // namespace

void protobuf_ShutdownFile_enumeration_2eproto() {
}

void protobuf_AddDesc_enumeration_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\021enumeration.proto\022\013enumeration*\254\001\n\rE_C"
    "LIENT_TYPE\022\027\n\023TYPE_CLIENT_UNKNOWN\020\000\022\022\n\016T"
    "YPE_CLIENT_PC\020\001\022\027\n\023TYPE_CLIENT_ANDROID\020\002"
    "\022\023\n\017TYPE_CLIENT_IOS\020\003\022\024\n\020TYPE_CLIENT_IPA"
    "D\020\004\022\023\n\017TYPE_CLIENT_WEB\020\005\022\025\n\021TYPE_CLIENT_"
    "COUNT\020\006*\251\001\n\024E_ACCOUNT_LOGIN_TYPE\022\031\n\025TYPE"
    "_LOGIN_BY_UNKNOWN\020\000\022\025\n\021TYPE_LOGIN_BY_UID"
    "\020\001\022\027\n\023TYPE_LOGIN_BY_PHONE\020\002\022\027\n\023TYPE_LOGI"
    "N_BY_EMAIL\020\003\022\024\n\020TYPE_LOGIN_BY_QQ\020\004\022\027\n\023TY"
    "PE_LOGIN_BY_COUNT\020\005*f\n\016E_OPERATE_TYPE\022\020\n"
    "\014TYPE_UNKNOWN\020\000\022\017\n\013TYPE_INSERT\020\001\022\017\n\013TYPE"
    "_DELETE\020\002\022\017\n\013TYPE_UPDATE\020\003\022\017\n\013TYPE_SELEC"
    "T\020\004*f\n\rE_USER_STATUS\022\036\n\032STATUS_UNKNOWN_U"
    "SER_STATUS\020\000\022\031\n\025STATUS_USER_STATUS_ON\020\001\022"
    "\032\n\026STATUS_USER_STATUS_OFF\020\002", 587);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "enumeration.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_enumeration_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_enumeration_2eproto {
  StaticDescriptorInitializer_enumeration_2eproto() {
    protobuf_AddDesc_enumeration_2eproto();
  }
} static_descriptor_initializer_enumeration_2eproto_;
const ::google::protobuf::EnumDescriptor* E_CLIENT_TYPE_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return E_CLIENT_TYPE_descriptor_;
}
bool E_CLIENT_TYPE_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* E_ACCOUNT_LOGIN_TYPE_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return E_ACCOUNT_LOGIN_TYPE_descriptor_;
}
bool E_ACCOUNT_LOGIN_TYPE_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* E_OPERATE_TYPE_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return E_OPERATE_TYPE_descriptor_;
}
bool E_OPERATE_TYPE_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* E_USER_STATUS_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return E_USER_STATUS_descriptor_;
}
bool E_USER_STATUS_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace enumeration

// @@protoc_insertion_point(global_scope)