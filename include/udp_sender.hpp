#pragma once

#include <string>

int initialize_socket();
int send_msg(const char *message);
void end_socket();
