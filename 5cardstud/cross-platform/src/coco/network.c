#ifdef _CMOC_VERSION_

/*
  Network functionality
*/
//#include <apple2.h>
//#include <string.h>
//#include <conio.h>
//#include <stdbool.h>
#include <cmoc.h>

#include "../platform-specific/appkey.h"
//#include <fujinet-fuji.h>
#include "../fujinet-network.h"
//#include <stdio.h>
#include "../misc.h"


#define HTTP_GET 12
#define NO_TRANSLATION 0

#define JSON 1

#define SUCCESS 1


static uint8_t trans_type = OPEN_TRANS_CRLF;
static uint8_t err = 0;

#define NET_DEBUG 1

void handle_err(char *message) {
	if (err <  0) {
		printf("err: %s\n", message);
		exit(1);
	}
}
/// @brief Retrieve a json parsed url response
/// @param url 
/// @return response length
#if 0
uint16_t getJsonResponse(char *url, unsigned char *buffer, uint16_t max_len) {
  static char buf[128];
  uint8_t c, err;

  static uint16_t count, bw, n;

#ifdef NET_DEBUG  
  printf("URL: %s\n", url);
  printf("network open\n");
#endif
  network_open(url, OPEN_MODE_HTTP_GET, OPEN_TRANS_NONE);
printf("press enter to continue\n"); readline();

printf("network json parse:\n");
  network_json_parse(url);
printf("press enter to continue\n"); readline();

printf("network json query:\n");  
  count = network_json_query(url, "", buffer);

  printf("network_json_query got %d bytes\n", count); readline();

printf("close query:\n");   
  network_close(url);
printf("press enter to continue\n"); readline();

  return count;
}
#endif


byte net_open(byte devid, byte mode, byte trans, const char *url);
void net_set_channel_mode(byte devid, byte channelMode);
byte net_parse_json(byte devid);
void fetch_json(const char *qs, char *s, int *i);
void net_close(byte devid);

uint16_t getJsonResponse(char *url, char *query, unsigned char *buffer, uint16_t max_len) 
{
    byte err;
    uint16_t i;

    err = net_open(0,HTTP_GET,NO_TRANSLATION,url);

    if (err != SUCCESS)
    {
        drawText(0,160,"OPEN ERROR.");
        return;
    }

    // Set channel mode to JSON
    net_set_channel_mode(0,JSON);

    err = net_parse_json(0);

    if (err != SUCCESS)
    {
        drawText(0,160,"JSON PARSE ERROR.");
        return;
    }

    fetch_json(query,buffer,&i);

    net_close(0);    
    return i;
}
#endif

