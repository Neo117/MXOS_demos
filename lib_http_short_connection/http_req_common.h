#ifndef _HTTP_REQ_COMMON_H_
#define _HTTP_REQ_COMMON_H_

#define HTTP_REQ_INIT_PARAM     {false, false, false, 0, NULL, 0, 0, NULL, 0, NULL, 0, 0, 0}

typedef struct _HTTP_REQ_S
{
    bool is_ssl;
    bool is_sni;
    bool is_success;
    int  status_code;
    char *domain_name;        //server domain
    uint32_t port;            //server port
    uint32_t timeout_ms;      //select timeout
    char *http_req;           //request addr
    uint32_t req_len;         //request len
    char *http_res;           //response addr
    uint32_t res_len;         //response len
    uint32_t real_res_len;    //real response len
    uint32_t user_args;
} HTTP_REQ_S;

#endif
