#pragma once

#include <google/protobuf/message.h>
#include <google/protobuf/timestamp.pb.h>

google::protobuf::Timestamp date_to_timestamp(const std::string& date, const char* fmt);
bool msg_write(const google::protobuf::Message& msg, const char* outfile);
