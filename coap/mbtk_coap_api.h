#ifndef MBTK_COAP_API_H
#define MBTK_COAP_API_H

#ifndef FEATURE_MBTK_COAP
#define FEATURE_MBTK_COAP
#endif

#ifdef FEATURE_MBTK_COAP
#include "m_type.h"

// CoAP message types. Note, values only work as enum.
typedef enum {
	COAP_CONFIRMABLE=0x00,
	COAP_NON_CONFIRMABLE=0x10,
	COAP_ACKNOWLEDGEMENT=0x20,
	COAP_RESET=0x30
}mbtk_coap_type;

// CoAP response codes.
typedef enum  {
	COAP_EMPTY=0x00,
	COAP_GET,
	COAP_POST,
	COAP_PUT,
	COAP_DELETE,
	COAP_LASTMETHOD=0x1F,
	COAP_CREATED=0x41,
	COAP_DELETED,
	COAP_VALID,
	COAP_CHANGED,
	COAP_CONTENT,
	COAP_CONTINUE=0x5F,
	COAP_BAD_REQUEST=0x80,
	COAP_UNAUTHORIZED,
	COAP_BAD_OPTION,
	COAP_FORBIDDEN,
	COAP_NOT_FOUND,
	COAP_METHOD_NOT_ALLOWED,
	COAP_NOT_ACCEPTABLE,
	COAP_PRECONDITION_FAILED=0x8C,
	COAP_REQUEST_ENTITY_TOO_LARGE=0x8D,
	COAP_UNSUPPORTED_CONTENT_FORMAT=0x8F,
	COAP_INTERNAL_SERVER_ERROR=0xA0,
	COAP_NOT_IMPLEMENTED,
	COAP_BAD_GATEWAY,
	COAP_SERVICE_UNAVAILABLE,
	COAP_GATEWAY_TIMEOUT,
	COAP_PROXYING_NOT_SUPPORTED,
	COAP_UNDEFINED_CODE=0xFF
}mbtk_coap_code_type;

/// CoAP option numbers.
typedef enum {
	COAP_OPTION_IF_MATCH=1,
	COAP_OPTION_URI_HOST=3,
	COAP_OPTION_ETAG,
	COAP_OPTION_IF_NONE_MATCH,
	COAP_OPTION_OBSERVE,
	COAP_OPTION_URI_PORT,
	COAP_OPTION_LOCATION_PATH,
	COAP_OPTION_URI_PATH=11,
	COAP_OPTION_CONTENT_FORMAT,
	COAP_OPTION_MAX_AGE=14,
	COAP_OPTION_URI_QUERY,
	COAP_OPTION_ACCEPT=17,
	COAP_OPTION_LOCATION_QUERY=20,
	COAP_OPTION_BLOCK2=23,
	COAP_OPTION_BLOCK1=27,
	COAP_OPTION_SIZE2,
	COAP_OPTION_PROXY_URI=35,
	COAP_OPTION_PROXY_SCHEME=39,
	COAP_OPTION_SIZE1=60
} mbtk_coap_option_type;

/// CoAP content-formats.
typedef enum {
	COAP_CONTENT_FORMAT_TEXT_PLAIN = 0,
	COAP_CONTENT_FORMAT_APP_LINK  = 40,
	COAP_CONTENT_FORMAT_APP_XML,
	COAP_CONTENT_FORMAT_APP_OCTET,
	COAP_CONTENT_FORMAT_APP_EXI   = 47,
	COAP_CONTENT_FORMAT_APP_JSON  = 50
}mbtk_content_format_type;


#ifdef __cplusplus
extern "C" {
#endif
extern void coapReleaseCoapPUD();
extern int coapReset();
extern int coapValidate();

// version
extern int coapSetVersion(uint8_t version);
extern uint8_t coapGetVersion();
extern uint8_t coapGetRecvVersion();

// message type
extern void coapSetType(mbtk_coap_type type);
extern mbtk_coap_type coapGetType();

// tokens
extern int coapCoapsetTokenLength(uint8_t tokenLength);
extern int coapGetTokenLength();
extern uint8_t* coapGetTokenPointer();
extern int coapSetToken(uint8_t *token, uint8_t tokenLength);

// message code
extern void coapSetCode(mbtk_coap_code_type code);
extern mbtk_coap_code_type coapGetCode();
extern mbtk_coap_code_type coapHttpStatusToCode(int httpStatus);

// message ID
extern int coapSetMessageID(uint16_t messageID);
extern uint16_t coapGetMessageID();

// options
extern int coapAddOption(uint16_t optionNumber, uint16_t optionLength, uint8_t *optionValue);
// gets a list of all options
//extern CoapOption* getOptions();
extern int coapGetNumOptions();
 // shorthand helpers
extern int coapSetURI(char *uri);
extern int coapsetURI(char *uri, int urilen);
extern int coapGetURI(char *dst, int dstlen, int *outLen);
extern int coapAddURIQuery(char *query);

// content format helper
extern int coapSetContentFormat(mbtk_content_format_type format);

// payload
extern uint8_t* coapMallocPayload(int bytes);
extern int coapSetPayload(uint8_t *value, int len);
extern uint8_t* coapGetPayloadPointer();
extern int coapGetPayloadLength();
extern uint8_t* coapGetPayloadCopy();

// pdu
extern int coapGetPDULength();
extern uint8_t* coapGetPDUPointer();
extern void coapSetPDULength(int len);

extern void coapDeleteRecv();
extern int coapCreateRecv(uint8_t *pdu, int pduLength);
extern void coapPrintHuman(char * outBuffer) ;
extern const char* coapPrintHumanByIndex(sint15 index);
extern const char* coapPrintHexByIndex(sint15 index);
extern void coapGetOptionValueById(uint16_t optionNumber, uint16_t * optionValueLength, uint8_t * optionValuePointer);
extern uint16_t coapGetRecvMessageID();
extern mbtk_coap_type coapGetRecvType();
extern mbtk_coap_code_type coapGetRecvCode();
extern int coapGetRecvTokenLength();
extern uint8_t* coapGetRecvTokenPointer();
extern int coapGetRecvPayloadLength();
extern uint8_t* coapGetRecvPayloadPointer();

// Return If-Match length,or 0 if fail.
extern uint16_t coapGetRecvOptionIfMatch(uint16_t *optionValueLength, uint8_t *optionValuePointer);

// Return Uri-Host length,or 0 if fail.
extern uint16_t coapGetRecvOptionUriHost(uint16_t *optionValueLength, uint8_t *optionValuePointer);

// Return ETag length,or 0 if fail.
extern uint16_t coapGetRecvOptionETag(uint16_t *optionValueLength, uint8_t *optionValuePointer);

// Return If-None-Match length,or 0 if fail.
extern uint16_t coapGetRecvOptionIfNoneMatch(uint16_t *optionValueLength, uint8_t *optionValuePointer);

// Return Location-Path length,or 0 if fail.
extern uint16_t coapGetRecvOptionLocationPath(uint16_t *optionValueLength, uint8_t *optionValuePointer);

// Return Location-Query length,or 0 if fail.
extern uint16_t coapGetRecvOptionLocationQuery(uint16_t *optionValueLength, uint8_t *optionValuePointer);

// Return Proxy-Uri length,or 0 if fail.
extern uint16_t coapGetRecvOptionProxyUri(uint16_t *optionValueLength, uint8_t *optionValuePointer);

// Return Proxy-Scheme length,or 0 if fail.
extern uint16_t coapGetRecvOptionProxyScheme(uint16_t *optionValueLength, uint8_t *optionValuePointer);

// Return Uri-Path length,or 0 if fail.
extern uint16_t coapGetRecvOptionUriPath(uint16_t *optionValueLength, uint8_t *optionValuePointer);

// Return Uri-Query length,or 0 if fail.
extern uint16_t coapGetRecvOptionUriQuery(uint16_t *optionValueLength, uint8_t *optionValuePointer);

// Return 1 if get Observe success,or 0 if fail.
extern uint16_t coapGetRecvOptionObserve(uint16_t *observe);

// Return 1 if get Max-Age success,or 0 if fail.
extern uint16_t coapGetRecvOptionMaxAge(uint16_t *max_age);

// Return 1 if get Uri-Port success,or 0 if fail.
extern uint16_t coapGetRecvOptionUriPort(uint16_t *uri_port);

// Return 1 if get Size2 success,or 0 if fail.
extern uint16_t coapGetRecvOptionSize2(uint16_t *size2);

// Return 1 if get Size1 success,or 0 if fail.
extern uint16_t coapGetRecvOptionSize1(uint16_t *size1);

// Return 1 if get Block2 success,or 0 if fail.
extern uint16_t coapGetRecvOptionBlock2(uint32_t *number,uint8_t *more_flag,uint8_t *size);

// Return 1 if get Block1 success,or 0 if fail.
extern uint16_t coapGetRecvOptionBlock1(uint32_t *number,uint8_t *more_flag,uint8_t *size);

// Return 1 if get Content-Format success,or 0 if fail.
extern uint16_t coapGetRecvOptionContentFormat(mbtk_content_format_type *type);

// Return 1 if get Accept success,or 0 if fail.
extern uint16_t coapGetRecvOptionAccept(mbtk_content_format_type *type);

extern int coapHasOption(uint16_t optionNumber );
extern int coapPrintRecvPayload(char *out);
#ifdef __cplusplus
}
#endif
#endif
#endif
