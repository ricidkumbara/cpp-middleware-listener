#pragma once

#include <string>

int initialize_socket(std::string serverIP, int serverPort);
int send_msg(const char *message);
void end_socket();
