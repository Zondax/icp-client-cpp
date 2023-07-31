// This is an experimental feature to generate C++ bindings from Candid.
// You may want to manually adjust some of the types.

#pragma once

#include <optional>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

//lib-agent-cpp
#include "agent.h"
#include "func.h"
#include "idl_value.h"
#include "principal.h"

using InternetIdentityInit_assigned_user_number_range = std::tuple<
  uint64_t,
  uint64_t
>;

struct InternetIdentityInit {
  InternetIdentityInit_assigned_user_number_range assigned_user_number_range;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(InternetIdentityInit arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("assigned_user_number_range");
        auto val = IdlValue(std::move(arg.assigned_user_number_range));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<
    InternetIdentityInit
  >IdlValue::getImpl(helper::tag_type<InternetIdentityInit>){
    InternetIdentityInit result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["assigned_user_number_range"]);
        auto val = field.get<
        InternetIdentityInit_assigned_user_number_range
      >();
      if (val.has_value()) {
            result.assigned_user_number_range = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

using UserNumber = uint64_t;
using PublicKey = std::vector<uint8_t>;
using DeviceKey = PublicKey;
struct KeyType_platform {
  explicit KeyType_platform() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"platform"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{0};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    KeyType_platform 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<KeyType_platform> IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      KeyType_platform
    >() : std::nullopt;
  }
}


struct KeyType_seed_phrase {
  explicit KeyType_seed_phrase() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"seed_phrase"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{1};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    KeyType_seed_phrase 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<KeyType_seed_phrase> IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      KeyType_seed_phrase
    >() : std::nullopt;
  }
}


struct KeyType_cross_platform {
  explicit KeyType_cross_platform() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"cross_platform"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{2};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    KeyType_cross_platform 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<KeyType_cross_platform> IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      KeyType_cross_platform
    >() : std::nullopt;
  }
}


struct KeyType_unknown {
  explicit KeyType_unknown() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"unknown"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{3};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    KeyType_unknown 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<KeyType_unknown> IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      KeyType_unknown
    >() : std::nullopt;
  }
}


using KeyType = std::variant<
  KeyType_platform,
  KeyType_seed_phrase,
  KeyType_cross_platform,
  KeyType_unknown
>;

struct Purpose_authentication {
  explicit Purpose_authentication() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"authentication"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{0};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    Purpose_authentication 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<Purpose_authentication> IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      Purpose_authentication
    >() : std::nullopt;
  }
}


struct Purpose_recovery {
  explicit Purpose_recovery() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"recovery"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{1};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    Purpose_recovery 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<Purpose_recovery> IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      Purpose_recovery
    >() : std::nullopt;
  }
}


using Purpose = std::variant<Purpose_authentication, Purpose_recovery>;

using CredentialId = std::vector<uint8_t>;
struct DeviceData {
  std::string alias;
  DeviceKey pubkey;
  KeyType key_type;
  Purpose purpose;
  std::optional<CredentialId> credential_id;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(DeviceData arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("alias");
        auto val = IdlValue(std::move(arg.alias));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("pubkey");
        auto val = IdlValue(std::move(arg.pubkey));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("key_type");
        auto val = IdlValue(std::move(arg.key_type));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("purpose");
        auto val = IdlValue(std::move(arg.purpose));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("credential_id");
        auto val = IdlValue(std::move(arg.credential_id));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<DeviceData>IdlValue::getImpl(helper::tag_type<
    DeviceData
  >){
    DeviceData result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["alias"]);
        auto val = field.get<
        std::string
      >();
      if (val.has_value()) {
            result.alias = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["pubkey"]);
        auto val = field.get<
        DeviceKey
      >();
      if (val.has_value()) {
            result.pubkey = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["key_type"]);
        auto val = field.get<
        KeyType
      >();
      if (val.has_value()) {
            result.key_type = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["purpose"]);
        auto val = field.get<
        Purpose
      >();
      if (val.has_value()) {
            result.purpose = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["credential_id"]);
        auto val = field.get<
        std::optional<CredentialId>
      >();
      if (val.has_value()) {
            result.credential_id = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

struct AddTentativeDeviceResponse_device_registration_mode_off {
  explicit AddTentativeDeviceResponse_device_registration_mode_off() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"device_registration_mode_off"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{0};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    AddTentativeDeviceResponse_device_registration_mode_off 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<
    AddTentativeDeviceResponse_device_registration_mode_off
  > IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      AddTentativeDeviceResponse_device_registration_mode_off
    >() : std::nullopt;
  }
}


struct AddTentativeDeviceResponse_another_device_tentatively_added {
  explicit AddTentativeDeviceResponse_another_device_tentatively_added() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"another_device_tentatively_added"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{1};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    AddTentativeDeviceResponse_another_device_tentatively_added 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<
    AddTentativeDeviceResponse_another_device_tentatively_added
  > IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      AddTentativeDeviceResponse_another_device_tentatively_added
    >() : std::nullopt;
  }
}


using Timestamp = uint64_t;
struct AddTentativeDeviceResponse_added_tentatively {
  static constexpr std::string_view __CANDID_VARIANT_NAME{"added_tentatively"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{2};
  std::string verification_code;
  Timestamp device_registration_timeout;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    AddTentativeDeviceResponse_added_tentatively
   arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("verification_code");
        auto val = IdlValue(std::move(arg.verification_code));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("device_registration_timeout");
        auto val = IdlValue(std::move(arg.device_registration_timeout));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<
    AddTentativeDeviceResponse_added_tentatively
  >IdlValue::getImpl(helper::tag_type<
    AddTentativeDeviceResponse_added_tentatively
  >){
    AddTentativeDeviceResponse_added_tentatively result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["verification_code"]);
        auto val = field.get<
        std::string
      >();
      if (val.has_value()) {
            result.verification_code = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["device_registration_timeout"]);
        auto val = field.get<
        Timestamp
      >();
      if (val.has_value()) {
            result.device_registration_timeout = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

using AddTentativeDeviceResponse = std::variant<
  AddTentativeDeviceResponse_device_registration_mode_off,
  AddTentativeDeviceResponse_another_device_tentatively_added,
  AddTentativeDeviceResponse_added_tentatively
>;

using ChallengeKey = std::string;
struct Challenge {std::string png_base64;ChallengeKey challenge_key;};
namespace zondax {
  template <> IdlValue::IdlValue(Challenge arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("png_base64");
        auto val = IdlValue(std::move(arg.png_base64));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("challenge_key");
        auto val = IdlValue(std::move(arg.challenge_key));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<Challenge>IdlValue::getImpl(helper::tag_type<
    Challenge
  >){
    Challenge result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["png_base64"]);
        auto val = field.get<
        std::string
      >();
      if (val.has_value()) {
            result.png_base64 = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["challenge_key"]);
        auto val = field.get<
        ChallengeKey
      >();
      if (val.has_value()) {
            result.challenge_key = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

struct DeviceRegistrationInfo {
  std::optional<DeviceData> tentative_device;
  Timestamp expiration;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(DeviceRegistrationInfo arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("tentative_device");
        auto val = IdlValue(std::move(arg.tentative_device));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("expiration");
        auto val = IdlValue(std::move(arg.expiration));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<
    DeviceRegistrationInfo
  >IdlValue::getImpl(helper::tag_type<DeviceRegistrationInfo>){
    DeviceRegistrationInfo result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["tentative_device"]);
        auto val = field.get<
        std::optional<DeviceData>
      >();
      if (val.has_value()) {
            result.tentative_device = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["expiration"]);
        auto val = field.get<
        Timestamp
      >();
      if (val.has_value()) {
            result.expiration = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

struct IdentityAnchorInfo {
  std::vector<DeviceData> devices;
  std::optional<DeviceRegistrationInfo> device_registration;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(IdentityAnchorInfo arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("devices");
        auto val = IdlValue(std::move(arg.devices));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("device_registration");
        auto val = IdlValue(std::move(arg.device_registration));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<
    IdentityAnchorInfo
  >IdlValue::getImpl(helper::tag_type<IdentityAnchorInfo>){
    IdentityAnchorInfo result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["devices"]);
        auto val = field.get<
        std::vector<DeviceData>
      >();
      if (val.has_value()) {
            result.devices = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["device_registration"]);
        auto val = field.get<
        std::optional<DeviceRegistrationInfo>
      >();
      if (val.has_value()) {
            result.device_registration = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

using FrontendHostname = std::string;
using SessionKey = PublicKey;
struct GetDelegationResponse_no_such_delegation {
  explicit GetDelegationResponse_no_such_delegation() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"no_such_delegation"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{0};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    GetDelegationResponse_no_such_delegation 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<
    GetDelegationResponse_no_such_delegation
  > IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      GetDelegationResponse_no_such_delegation
    >() : std::nullopt;
  }
}


struct Delegation {
  PublicKey pubkey;
  std::optional<std::vector<zondax::Principal>> targets;
  Timestamp expiration;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(Delegation arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("pubkey");
        auto val = IdlValue(std::move(arg.pubkey));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("targets");
        auto val = IdlValue(std::move(arg.targets));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("expiration");
        auto val = IdlValue(std::move(arg.expiration));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<Delegation>IdlValue::getImpl(helper::tag_type<
    Delegation
  >){
    Delegation result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["pubkey"]);
        auto val = field.get<
        PublicKey
      >();
      if (val.has_value()) {
            result.pubkey = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["targets"]);
        auto val = field.get<
        std::optional<std::vector<zondax::Principal>>
      >();
      if (val.has_value()) {
            result.targets = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["expiration"]);
        auto val = field.get<
        Timestamp
      >();
      if (val.has_value()) {
            result.expiration = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

struct SignedDelegation {
  static constexpr std::string_view __CANDID_VARIANT_NAME{"signed_delegation"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{1};
  std::vector<uint8_t> signature;
  Delegation delegation;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(SignedDelegation arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("signature");
        auto val = IdlValue(std::move(arg.signature));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("delegation");
        auto val = IdlValue(std::move(arg.delegation));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<SignedDelegation>IdlValue::getImpl(helper::tag_type<
    SignedDelegation
  >){
    SignedDelegation result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["signature"]);
        auto val = field.get<
        std::vector<uint8_t>
      >();
      if (val.has_value()) {
            result.signature = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["delegation"]);
        auto val = field.get<
        Delegation
      >();
      if (val.has_value()) {
            result.delegation = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

using GetDelegationResponse = std::variant<
  GetDelegationResponse_no_such_delegation,
  SignedDelegation
>;

using HeaderField = std::tuple<std::string, std::string>;

struct HttpRequest {
  std::string url;
  std::string method;
  std::vector<uint8_t> body;
  std::vector<HeaderField> headers;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(HttpRequest arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("url");
        auto val = IdlValue(std::move(arg.url));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("method");
        auto val = IdlValue(std::move(arg.method));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("body");
        auto val = IdlValue(std::move(arg.body));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("headers");
        auto val = IdlValue(std::move(arg.headers));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<HttpRequest>IdlValue::getImpl(helper::tag_type<
    HttpRequest
  >){
    HttpRequest result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["url"]);
        auto val = field.get<
        std::string
      >();
      if (val.has_value()) {
            result.url = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["method"]);
        auto val = field.get<
        std::string
      >();
      if (val.has_value()) {
            result.method = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["body"]);
        auto val = field.get<
        std::vector<uint8_t>
      >();
      if (val.has_value()) {
            result.body = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["headers"]);
        auto val = field.get<
        std::vector<HeaderField>
      >();
      if (val.has_value()) {
            result.headers = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

struct Token {
  explicit Token() = default;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    Token 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<Token> IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      Token
    >() : std::nullopt;
  }
}


struct StreamingCallbackHttpResponse {
  std::optional<Token> token;
  std::vector<uint8_t> body;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(StreamingCallbackHttpResponse arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("token");
        auto val = IdlValue(std::move(arg.token));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("body");
        auto val = IdlValue(std::move(arg.body));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<
    StreamingCallbackHttpResponse
  >IdlValue::getImpl(helper::tag_type<StreamingCallbackHttpResponse>){
    StreamingCallbackHttpResponse result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["token"]);
        auto val = field.get<
        std::optional<Token>
      >();
      if (val.has_value()) {
            result.token = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["body"]);
        auto val = field.get<
        std::vector<uint8_t>
      >();
      if (val.has_value()) {
            result.body = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

struct StreamingStrategy_Callback {
  static constexpr std::string_view __CANDID_VARIANT_NAME{"Callback"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{0};
  Token token;
  zondax::Func callback;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(StreamingStrategy_Callback arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("token");
        auto val = IdlValue(std::move(arg.token));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("callback");
        auto val = IdlValue(std::move(arg.callback));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<
    StreamingStrategy_Callback
  >IdlValue::getImpl(helper::tag_type<StreamingStrategy_Callback>){
    StreamingStrategy_Callback result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["token"]);
        auto val = field.get<
        Token
      >();
      if (val.has_value()) {
            result.token = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["callback"]);
        auto val = field.get<
        zondax::Func
      >();
      if (val.has_value()) {
            result.callback = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

using StreamingStrategy = std::variant<StreamingStrategy_Callback>;

struct HttpResponse {
  std::vector<uint8_t> body;
  std::vector<HeaderField> headers;
  std::optional<StreamingStrategy> streaming_strategy;
  uint16_t status_code;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(HttpResponse arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("body");
        auto val = IdlValue(std::move(arg.body));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("headers");
        auto val = IdlValue(std::move(arg.headers));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("streaming_strategy");
        auto val = IdlValue(std::move(arg.streaming_strategy));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("status_code");
        auto val = IdlValue(std::move(arg.status_code));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<HttpResponse>IdlValue::getImpl(helper::tag_type<
    HttpResponse
  >){
    HttpResponse result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["body"]);
        auto val = field.get<
        std::vector<uint8_t>
      >();
      if (val.has_value()) {
            result.body = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["headers"]);
        auto val = field.get<
        std::vector<HeaderField>
      >();
      if (val.has_value()) {
            result.headers = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["streaming_strategy"]);
        auto val = field.get<
        std::optional<StreamingStrategy>
      >();
      if (val.has_value()) {
            result.streaming_strategy = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["status_code"]);
        auto val = field.get<
        uint16_t
      >();
      if (val.has_value()) {
            result.status_code = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

using UserKey = PublicKey;
struct ChallengeResult {ChallengeKey key;std::string chars;};
namespace zondax {
  template <> IdlValue::IdlValue(ChallengeResult arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("key");
        auto val = IdlValue(std::move(arg.key));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("chars");
        auto val = IdlValue(std::move(arg.chars));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<ChallengeResult>IdlValue::getImpl(helper::tag_type<
    ChallengeResult
  >){
    ChallengeResult result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["key"]);
        auto val = field.get<
        ChallengeKey
      >();
      if (val.has_value()) {
            result.key = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["chars"]);
        auto val = field.get<
        std::string
      >();
      if (val.has_value()) {
            result.chars = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

struct RegisterResponse_bad_challenge {
  explicit RegisterResponse_bad_challenge() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"bad_challenge"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{0};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    RegisterResponse_bad_challenge 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<RegisterResponse_bad_challenge> IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      RegisterResponse_bad_challenge
    >() : std::nullopt;
  }
}


struct RegisterResponse_canister_full {
  explicit RegisterResponse_canister_full() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"canister_full"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{1};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    RegisterResponse_canister_full 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<RegisterResponse_canister_full> IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      RegisterResponse_canister_full
    >() : std::nullopt;
  }
}


struct RegisterResponse_registered {
  static constexpr std::string_view __CANDID_VARIANT_NAME{"registered"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{2};
  UserNumber user_number;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(RegisterResponse_registered arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("user_number");
        auto val = IdlValue(std::move(arg.user_number));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<
    RegisterResponse_registered
  >IdlValue::getImpl(helper::tag_type<RegisterResponse_registered>){
    RegisterResponse_registered result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["user_number"]);
        auto val = field.get<
        UserNumber
      >();
      if (val.has_value()) {
            result.user_number = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

using RegisterResponse = std::variant<
  RegisterResponse_bad_challenge,
  RegisterResponse_canister_full,
  RegisterResponse_registered
>;

using InternetIdentityStats_assigned_user_number_range = std::tuple<
  uint64_t,
  uint64_t
>;

struct InternetIdentityStats {
  uint64_t users_registered;
  InternetIdentityStats_assigned_user_number_range assigned_user_number_range;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(InternetIdentityStats arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("users_registered");
        auto val = IdlValue(std::move(arg.users_registered));
        fields.emplace(std::make_pair(name, std::move(val)));
      }{
        auto name = std::string("assigned_user_number_range");
        auto val = IdlValue(std::move(arg.assigned_user_number_range));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<
    InternetIdentityStats
  >IdlValue::getImpl(helper::tag_type<InternetIdentityStats>){
    InternetIdentityStats result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["users_registered"]);
        auto val = field.get<
        uint64_t
      >();
      if (val.has_value()) {
            result.users_registered = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }{
      auto field = std::move(fields["assigned_user_number_range"]);
        auto val = field.get<
        InternetIdentityStats_assigned_user_number_range
      >();
      if (val.has_value()) {
            result.assigned_user_number_range = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

struct VerifyTentativeDeviceResponse_device_registration_mode_off {
  explicit VerifyTentativeDeviceResponse_device_registration_mode_off() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"device_registration_mode_off"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{0};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    VerifyTentativeDeviceResponse_device_registration_mode_off 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<
    VerifyTentativeDeviceResponse_device_registration_mode_off
  > IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      VerifyTentativeDeviceResponse_device_registration_mode_off
    >() : std::nullopt;
  }
}


struct VerifyTentativeDeviceResponse_verified {
  explicit VerifyTentativeDeviceResponse_verified() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"verified"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{1};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    VerifyTentativeDeviceResponse_verified 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<
    VerifyTentativeDeviceResponse_verified
  > IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      VerifyTentativeDeviceResponse_verified
    >() : std::nullopt;
  }
}


struct VerifyTentativeDeviceResponse_wrong_code {
  static constexpr std::string_view __CANDID_VARIANT_NAME{"wrong_code"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{2};
  uint8_t retries_left;
  
};
namespace zondax {
  template <> IdlValue::IdlValue(VerifyTentativeDeviceResponse_wrong_code arg){
    std::unordered_map<std::string, IdlValue> fields;

      {
        auto name = std::string("retries_left");
        auto val = IdlValue(std::move(arg.retries_left));
        fields.emplace(std::make_pair(name, std::move(val)));
      }
    
     *this = std::move(IdlValue::FromRecord(fields));
  }
  
  template <> std::optional<
    VerifyTentativeDeviceResponse_wrong_code
  >IdlValue::getImpl(helper::tag_type<
    VerifyTentativeDeviceResponse_wrong_code
  >){
    VerifyTentativeDeviceResponse_wrong_code result;
    auto fields = getRecord();
    {
      auto field = std::move(fields["retries_left"]);
        auto val = field.get<
        uint8_t
      >();
      if (val.has_value()) {
            result.retries_left = std::move(val.value());
        } else {
            return std::nullopt;
        }
    }
    	return std::make_optional(std::move(result));
  }
}

struct VerifyTentativeDeviceResponse_no_device_to_verify {
  explicit VerifyTentativeDeviceResponse_no_device_to_verify() = default;
  static constexpr std::string_view __CANDID_VARIANT_NAME{"no_device_to_verify"};
  static constexpr std::size_t __CANDID_VARIANT_CODE{3};
  
};
namespace zondax {
  template <> IdlValue::IdlValue(
    VerifyTentativeDeviceResponse_no_device_to_verify 
   arg): IdlValue(std::move(IdlValue::null())) {}
  template <> std::optional<
    VerifyTentativeDeviceResponse_no_device_to_verify
  > IdlValue::get() {
    return this->get<std::monostate>().has_value() ? std::make_optional<
      VerifyTentativeDeviceResponse_no_device_to_verify
    >() : std::nullopt;
  }
}


using VerifyTentativeDeviceResponse = std::variant<
  VerifyTentativeDeviceResponse_device_registration_mode_off,
  VerifyTentativeDeviceResponse_verified,
  VerifyTentativeDeviceResponse_wrong_code,
  VerifyTentativeDeviceResponse_no_device_to_verify
>;

class SERVICE {
private:
zondax::Agent agent;
public:
SERVICE(zondax::Agent&& agent): agent(std::move(agent)) {}
std::variant<std::monostate, std::string>add(UserNumber arg0, DeviceData arg1){
  auto result = agent.Update<std::monostate>("add", arg0, arg1);
  if (result.index() == 0){
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<AddTentativeDeviceResponse, std::string>add_tentative_device(
  UserNumber arg0,
  DeviceData arg1
){
  auto result = agent.Update<
    AddTentativeDeviceResponse
  >("add_tentative_device", arg0, arg1);
  if (result.index() == 0){
    return std::variant<
      AddTentativeDeviceResponse
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      AddTentativeDeviceResponse
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<Challenge, std::string>create_challenge(){
  auto result = agent.Update<Challenge>("create_challenge");
  if (result.index() == 0){
    return std::variant<
      Challenge
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      Challenge
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<Timestamp, std::string>enter_device_registration_mode(
  UserNumber arg0
){
  auto result = agent.Update<Timestamp>("enter_device_registration_mode", arg0);
  if (result.index() == 0){
    return std::variant<
      Timestamp
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      Timestamp
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<std::monostate, std::string>exit_device_registration_mode(
  UserNumber arg0
){
  auto result = agent.Update<
    std::monostate
  >("exit_device_registration_mode", arg0);
  if (result.index() == 0){
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<IdentityAnchorInfo, std::string>get_anchor_info(UserNumber arg0){
  auto result = agent.Update<IdentityAnchorInfo>("get_anchor_info", arg0);
  if (result.index() == 0){
    return std::variant<
      IdentityAnchorInfo
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      IdentityAnchorInfo
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<GetDelegationResponse, std::string>get_delegation(
  UserNumber arg0,
  FrontendHostname arg1,
  SessionKey arg2,
  Timestamp arg3
){
  auto result = agent.Query<
    GetDelegationResponse
  >("get_delegation", arg0, arg1, arg2, arg3);
  if (result.index() == 0){
    return std::variant<
      GetDelegationResponse
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      GetDelegationResponse
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<zondax::Principal, std::string>get_principal(
  UserNumber arg0,
  FrontendHostname arg1
){
  auto result = agent.Query<zondax::Principal>("get_principal", arg0, arg1);
  if (result.index() == 0){
    return std::variant<
      zondax::Principal
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      zondax::Principal
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<HttpResponse, std::string>http_request(HttpRequest arg0){
  auto result = agent.Query<HttpResponse>("http_request", arg0);
  if (result.index() == 0){
    return std::variant<
      HttpResponse
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      HttpResponse
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<std::monostate, std::string>init_salt(){
  auto result = agent.Update<std::monostate>("init_salt");
  if (result.index() == 0){
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<std::vector<DeviceData>, std::string>lookup(UserNumber arg0){
  auto result = agent.Query<std::vector<DeviceData>>("lookup", arg0);
  if (result.index() == 0){
    return std::variant<
      std::vector<DeviceData>
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      std::vector<DeviceData>
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<std::tuple<UserKey, Timestamp>, std::string>prepare_delegation(
  UserNumber arg0,
  FrontendHostname arg1,
  SessionKey arg2,
  std::optional<uint64_t> arg3
){
  auto result = agent.Update<
    std::tuple<UserKey, Timestamp>
  >("prepare_delegation", arg0, arg1, arg2, arg3);
  if (result.index() == 0){
    return std::variant<
      std::tuple<UserKey, Timestamp>
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      std::tuple<UserKey, Timestamp>
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<RegisterResponse, std::string>_3500123747_(
  DeviceData arg0,
  ChallengeResult arg1
){
  auto result = agent.Update<RegisterResponse>("register", arg0, arg1);
  if (result.index() == 0){
    return std::variant<
      RegisterResponse
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      RegisterResponse
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<std::monostate, std::string>remove(
  UserNumber arg0,
  DeviceKey arg1
){
  auto result = agent.Update<std::monostate>("remove", arg0, arg1);
  if (result.index() == 0){
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      std::monostate
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<InternetIdentityStats, std::string>stats(){
  auto result = agent.Query<InternetIdentityStats>("stats");
  if (result.index() == 0){
    return std::variant<
      InternetIdentityStats
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      InternetIdentityStats
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
std::variant<VerifyTentativeDeviceResponse, std::string>verify_tentative_device(
  UserNumber arg0,
  std::string arg1
){
  auto result = agent.Update<
    VerifyTentativeDeviceResponse
  >("verify_tentative_device", arg0, arg1);
  if (result.index() == 0){
    return std::variant<
      VerifyTentativeDeviceResponse
    , std::string>(std::in_place_index<0>, std::move(std::get<0>(result).value()));
  }else {
    return std::variant<
      VerifyTentativeDeviceResponse
    , std::string>(std::in_place_index<1>, std::get<1>(result));
  }
}
};
