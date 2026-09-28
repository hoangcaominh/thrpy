#pragma once

#include <google/protobuf/message.h>

bool msg_write(const google::protobuf::Message& msg, const char* outfile);
