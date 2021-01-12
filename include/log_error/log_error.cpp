/*
 * @file        log_error.cxx
 * @brief       This is a program to Implement Error Handling for the Application program.
 * @author      Sidharth Dinesh
 * @bugs        No bugs found.
 */
/*

/*
 *#####################################################################
 *  Initialization block
 *  ---------------------
 *  This block contains initialization code for this particular file.
 *  It typically contains Includes, constants or global variables used
 *  throughout the file.
 *#####################################################################
 */

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

#include "log_error.h"

using namespace std;

inline std::string getCurrentDateTime(std::string s)
{
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80];
    tstruct = *localtime(&now);

    if(s=="now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if(s=="date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

    return string(buf);

};

inline void Logger(std::string logMsg)
{

    std::string filePath = "/error/log_"+getCurrentDateTime("date")+".txt";        //Writes a file (or appends existing file): /error/log_2020-11-02.txt 
    std::string now = getCurrentDateTime("now");
    std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app );   
    ofs << now << '\t' << logMsg << '\n';                                       //content: 2020-12-01 09:09:59 This is log message

    ofs.close();
}