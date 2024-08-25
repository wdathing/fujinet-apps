/**
 * @brief   ISS Tracker
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Fetch current position from web
 */

#include <cmoc.h>
#include <coco.h>
#include "net.h"

#define HTTP_GET 12
#define NO_TRANSLATION 0

#define JSON 1

#define SUCCESS 1

#define API_ENDPOINT "N:HTTP://api.open-notify.org/iss-now.json"

#define QUERY_LONGITUDE "/iss_position/longitude"
#define QUERY_LATITUDE "/iss_position/latitude"
#define QUERY_TIMESTAMP "/timestamp"

/**
 * @brief fetch individual json element and populate vars.
 */
void fetch_json(const char *qs, char *s, int *i)
{
    NetworkStatus ns;

    // Set query
    net_set_json_query(0,qs);

    // Get # of bytes waiting
    net_status(0,&ns);

    // read into buffer
    net_read(0,(unsigned char *)s,ns.bytesWaiting);

    // Convert string to integer
    //*i=atoi(s);
    *i = ns.bytesWaiting;

#if 0
    printf("got %u bytes\n", i);
    if (i>0)
      printf("buffer is %s\n", s);
#endif        
}
