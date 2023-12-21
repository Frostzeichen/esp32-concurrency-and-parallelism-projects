#ifndef prepareResponse
#define prepareResponse
#include <Arduino.h>

String httpResponse() {
  String httpResponse;
  httpResponse.reserve(1024);
  httpResponse = F("HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "\r\n");
  httpResponse += F(
                "<html>"
                    "<head><title>Hello</title></head>"
                    "<body>"
                        "<p>Hello</p>"
                    "</body>"
                "</html>"
                "\r\n");
  return httpResponse;
}

#endif