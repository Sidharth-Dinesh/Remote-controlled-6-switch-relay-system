/** @file <log_error>.h
 *  @brief Header file contains the function prototypes of all the functions in the log_error.cpp file
 *  @author Sidharth Dinesh 
 *  @bug No bugs found in the file  
 */

/*
 *#####################################################################
 *  Initialization block
 *  ---------------------
 *  This block contains initialization code for this particular file.
 *  It typically contains Includes, constants or global variables used
 *  throughout the file.
 *#####################################################################
 */
 
#ifndef LOG_ERROR_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define LOG_ERROR_H

/* Standard Includes */
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

/* RapidJSON Includes */
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

#include <si/shunyaInterfaces.h>
	
inline std::string getCurrentDateTime(std::string);
inline void Logger(std::string logMsg);

#endif
