/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
extern "C"{
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
};
#include "mbtk_coap.h"
#include "mbtk_coap_api.h"
/*===========================================================================

                        LOCAL DATA STRUCTURES

===========================================================================*/

#ifndef FEATURE_MBTK_COAP
#define FEATURE_MBTK_COAP
#endif

#ifdef FEATURE_MBTK_COAP
extern "C"
{


static CoapPDU * g_CoapPDU = NULL;
static CoapPDU * g_CoapRecvPointer = NULL;

static void createCoapPDU(){
	if(g_CoapPDU == 	NULL)
		g_CoapPDU = new CoapPDU();
}

void coapReleaseCoapPUD()
{
	if(g_CoapPDU){
		delete g_CoapPDU;
		g_CoapPDU = NULL;
	}
}

int coapReset() {
	createCoapPDU();
	return g_CoapPDU->reset();
}

int coapValidate(){
	createCoapPDU();
	return g_CoapPDU->validate();
}


uint8_t* coapGetPDUPointer() {
	return g_CoapPDU->getPDUPointer();
}

void coapSetPDULength(int len) {
	createCoapPDU();
	g_CoapPDU->setPDULength(len);
}

int coapSetURI(char *uri) {
	createCoapPDU();
	return g_CoapPDU->setURI(uri);
}

int coapAddURIQuery(char *query) {
	createCoapPDU();
	return g_CoapPDU->addURIQuery(query);
}

int coapGetURI(char *dst, int dstlen, int *outLen) {
	createCoapPDU();
	return  g_CoapPDU->getURI(dst, dstlen, outLen);
}

int coapSetVersion(uint8_t version) {
	createCoapPDU();
	return g_CoapPDU->setVersion(version);
}

uint8_t coapGetVersion() {
	createCoapPDU();
	return g_CoapPDU->getVersion();
}

void coapSetType(mbtk_coap_type mt) {
	createCoapPDU();
	g_CoapPDU->setType(mt);
}

mbtk_coap_type coapGetType() {
	createCoapPDU();
	return (mbtk_coap_type)g_CoapPDU->getType();
}

int GetTokenLength(uint8_t tokenLength) {
	createCoapPDU();
	return g_CoapPDU->setTokenLength(tokenLength);
}

int coapGetTokenLength() {
	createCoapPDU();
	return g_CoapPDU->getTokenLength();
}

uint8_t* coapGetTokenPointer() {
	createCoapPDU();
	return g_CoapPDU->getTokenPointer();
}

int coapSetToken(uint8_t *token, uint8_t tokenLength) {
	createCoapPDU();
	return g_CoapPDU->setToken(token, tokenLength) ;
}

void coapSetCode(mbtk_coap_code_type code) {
	createCoapPDU();
	g_CoapPDU->setCode(code);
}

mbtk_coap_code_type coapGetCode() {
	createCoapPDU();
	return (mbtk_coap_code_type)g_CoapPDU->getCode();
}

mbtk_coap_code_type coapHttpStatusToCode(int httpStatus) {
	createCoapPDU();
	return (mbtk_coap_code_type)g_CoapPDU->httpStatusToCode(httpStatus);
}

int coapSetMessageID(uint16_t messageID) {
	createCoapPDU();
	return g_CoapPDU->setMessageID(messageID);
}

uint16_t coapGetMessageID() {
	createCoapPDU();
	return g_CoapPDU->getMessageID();
}

/// Returns the length of the PDU.
int coapGetPDULength() {
	createCoapPDU();
	return g_CoapPDU->getPDULength();
}

/// Return the number of options that the PDU has.
int coapGetNumOptions() {
	createCoapPDU();
	return  g_CoapPDU->getNumOptions();
}

/*CoapOption* getOptions() {


	return g_CoapPDU->getOptions();
}
*/

int coapAddOption(uint16_t insertedOptionNumber, uint16_t optionValueLength, uint8_t *optionValue) {
	createCoapPDU();
	return  g_CoapPDU->addOption(insertedOptionNumber, optionValueLength, optionValue);
}

uint8_t* coapMallocPayload(int len) {
	createCoapPDU();
	return g_CoapPDU->mallocPayload(len);
}

int coapSetPayload(uint8_t *payload, int len) {
	createCoapPDU();
	return g_CoapPDU->setPayload(payload, len);
}

/// Returns a pointer to the payload buffer.
uint8_t* coapGetPayloadPointer() {
	createCoapPDU();
	return g_CoapPDU->getPayloadPointer();
}

/// Gets the length of the payload buffer.
int coapGetPayloadLength() {
	createCoapPDU();
	return g_CoapPDU->getPayloadLength();
}

/// Returns a pointer to a buffer which is a copy of the payload buffer (dynamically allocated).
uint8_t* coapGetPayloadCopy() {
	createCoapPDU();
	return g_CoapPDU->getPayloadCopy();
}

int coapSetContentFormat(mbtk_content_format_type format) {
	createCoapPDU();
	return g_CoapPDU->setContentFormat(format);
}

int coapHasOption(uint16_t optionNumber ) {
	createCoapPDU();
	return g_CoapPDU->hasOption(optionNumber);
}
void coapDeleteRecv()
{
	if(g_CoapRecvPointer != NULL){
		delete g_CoapRecvPointer;
		g_CoapRecvPointer = NULL;
	}

}
int coapCreateRecv(uint8_t *pdu, int pduLength)
{
	coapDeleteRecv();
	g_CoapRecvPointer = new CoapPDU(pdu, pduLength, pduLength);
	if(g_CoapRecvPointer != NULL)
	{
		if(g_CoapRecvPointer->validate())
			return g_CoapRecvPointer->getPDULength();
		else{
			coapDeleteRecv();
			return 0;
		}
	}
	else
		return 0;

}

void coapPrintHuman(char * outBuffer) {
	char type[20];
	switch(g_CoapRecvPointer->getType()) {
		case COAP_CONFIRMABLE:
			strcpy(type, "Confirmable");
		break;

		case COAP_NON_CONFIRMABLE:
			strcpy(type, "Non-Confirmable");
		break;

		case COAP_ACKNOWLEDGEMENT:
			strcpy(type, "Acknowledgement");
		break;

		case COAP_RESET:
			strcpy(type, "Reset");
		break;
	}
	char code[30];
	switch(g_CoapRecvPointer->getCode()) {
		case COAP_EMPTY:
			strcpy(code, "0.00 Empty");
		break;
		case COAP_GET:
			strcpy(code, "0.01 GET");
		break;
		case COAP_POST:
			strcpy(code, "0.02 POST");
		break;
		case COAP_PUT:
			strcpy(code, "0.03 PUT");
		break;
		case COAP_DELETE:
			strcpy(code, "0.04 DELETE");
		break;
		case COAP_CREATED:
			strcpy(code, "2.01 Created");
		break;
		case COAP_DELETED:
			strcpy(code, "2.02 Deleted");
		break;
		case COAP_VALID:
			strcpy(code, "2.03 Valid");
		break;
		case COAP_CHANGED:
			strcpy(code, "2.04 Changed");
		break;
		case COAP_CONTENT:
			strcpy(code, "2.05 Content");
		break;
		case COAP_BAD_REQUEST:
			strcpy(code, "4.00 Bad Request");
		break;
		case COAP_UNAUTHORIZED:
			strcpy(code, "4.01 Unauthorized");
		break;
		case COAP_BAD_OPTION:
			strcpy(code, "4.02 Bad Option");
		break;
		case COAP_FORBIDDEN:
			strcpy(code, "4.03 Forbidden");
		break;
		case COAP_NOT_FOUND:
			strcpy(code, "4.04 Not Found");
		break;
		case COAP_METHOD_NOT_ALLOWED:
			strcpy(code, "4.05 Method Not Allowed");
		break;
		case COAP_NOT_ACCEPTABLE:
			strcpy(type, "4.06 Not Acceptable");
		break;
		case COAP_PRECONDITION_FAILED:
			strcpy(code, "4.12 Precondition Failed");
		break;
		case COAP_REQUEST_ENTITY_TOO_LARGE:
			strcpy(code, "4.13 Request Entity Too Large");
		break;
		case COAP_UNSUPPORTED_CONTENT_FORMAT:
			strcpy(code, "4.15 Unsupported Content-Format");
		break;
		case COAP_INTERNAL_SERVER_ERROR:
			strcpy(code, "5.00 Internal Server Error");
		break;
		case COAP_NOT_IMPLEMENTED:
			strcpy(code, "5.01 Not Implemented");
		break;
		case COAP_BAD_GATEWAY:
			strcpy(code, "5.02 Bad Gateway");
		break;
		case COAP_SERVICE_UNAVAILABLE:
			strcpy(code, "5.03 Service Unavailable");
		break;
		case COAP_GATEWAY_TIMEOUT:
			strcpy(code, "5.04 Gateway Timeout");
		break;
		case COAP_PROXYING_NOT_SUPPORTED:
			strcpy(code, "5.05 Proxying Not Supported");
		break;
		default:{
			sprintf(code, "Undefined Code %u", g_CoapRecvPointer->getCode());
		}
	}

	sprintf(outBuffer,"PDU is %d bytes long\r\n"
		"CoAP Version: %d\r\n"
		"Message Type: %s\r\n"
		"Code: %s\r\n"
		"Message ID: %u\r\n",
		g_CoapRecvPointer->getPDULength(),
		g_CoapRecvPointer->getVersion(),
		type,
		code,
		g_CoapRecvPointer->getMessageID());

	// print token value
	int tokenLength = g_CoapRecvPointer->getTokenLength();
	uint8_t *tokenPointer = g_CoapRecvPointer->getPDUPointer()+COAP_HDR_SIZE;
	if(tokenLength==0) {
		sprintf(outBuffer,"%sNo token\r\n",outBuffer);
	} else {
		sprintf(outBuffer,"%sToken of %d bytes.\r\n"
			"Value: %s\r\n",
			outBuffer,tokenLength, (char*)tokenPointer);
		for(int j=0; j<tokenLength; j++) {
			sprintf(outBuffer,"%s%.2x",outBuffer, tokenPointer[j]);
		}
	}
	// print options
	CoapPDU::CoapOption* options = g_CoapRecvPointer->getOptions();
	if(options==NULL) {
		sprintf(outBuffer,"\r\n%sNO options\r\n", outBuffer);
	}

	for(int i=0; i<g_CoapRecvPointer->getNumOptions(); i++) {
		char optionNumberBuffer[20];
		switch(options[i].optionNumber) {
			case COAP_OPTION_IF_MATCH:
				strcpy(optionNumberBuffer, "IF_MATCH");
			break;
			case COAP_OPTION_URI_HOST:
				strcpy(optionNumberBuffer, "URI_HOST");
			break;
			case COAP_OPTION_ETAG:
				strcpy(optionNumberBuffer, "ETAG");
			break;
			case COAP_OPTION_IF_NONE_MATCH:
				strcpy(optionNumberBuffer, "IF_NONE_MATCH");
			break;
			case COAP_OPTION_OBSERVE:
				strcpy(optionNumberBuffer, "OBSERVE");
			break;
			case COAP_OPTION_URI_PORT:
				strcpy(optionNumberBuffer, "URI_PORT");
			break;
			case COAP_OPTION_LOCATION_PATH:
				strcpy(optionNumberBuffer, "LOCATION_PATH");
			break;
			case COAP_OPTION_URI_PATH:
				strcpy(optionNumberBuffer, "URI_PATH");
			break;
			case COAP_OPTION_CONTENT_FORMAT:
				strcpy(optionNumberBuffer, "CONTENT_FORMAT");
			break;
			case COAP_OPTION_MAX_AGE:
				strcpy(optionNumberBuffer, "MAX_AGE");
			break;
			case COAP_OPTION_URI_QUERY:
				strcpy(optionNumberBuffer, "URI_QUERY");
			break;
			case COAP_OPTION_ACCEPT:
				strcpy(optionNumberBuffer, "ACCEPT");
			break;
			case COAP_OPTION_LOCATION_QUERY:
				strcpy(optionNumberBuffer, "LOCATION_QUERY");
			break;
			case COAP_OPTION_PROXY_URI:
				strcpy(optionNumberBuffer, "PROXY_URI");
			break;
			case COAP_OPTION_PROXY_SCHEME:
				strcpy(optionNumberBuffer, "PROXY_SCHEME");
			break;
			case COAP_OPTION_BLOCK1:
				strcpy(optionNumberBuffer, "BLOCK1");
			break;
			case COAP_OPTION_BLOCK2:
				strcpy(optionNumberBuffer, "BLOCK2");
			break;
			case COAP_OPTION_SIZE1:
				strcpy(optionNumberBuffer, "SIZE1");
			break;
			case COAP_OPTION_SIZE2:
				strcpy(optionNumberBuffer, "SIZE2");
			break;
			default:
				sprintf(optionNumberBuffer, "Unknown option %u",(unsigned)options[i].optionNumber);
			break;
		}

		/*char optionValue[options[i].optionValueLength + 1];
		for(int j=0; j<options[i].optionValueLength; j++) {
			char c = options[i].optionValuePointer[j];
			if((c>='!'&&c<='~')||c==' ') {
				sprintf(optionValue, "%s%c",optionValue,c);
			} else {
				sprintf(optionValue,"%s\\%.2d",optionValue,c);
			}
		}
		sprintf(optionValue,"%s\"\r\n",optionValue);*/
		sprintf(outBuffer,"\r\n%sOPTION (%d/%d)\r\n"
			"Option number (delta): %hu (%hu)\r\n"
			"Name: %s\r\n"
			"Value length: %u\r\n"
			"Value: \"%s",
			outBuffer,
			i + 1,g_CoapRecvPointer->getNumOptions(),
			options[i].optionNumber,options[i].optionDelta,
			optionNumberBuffer,
			options[i].optionValueLength,
			(char *)options[i].optionValuePointer);

	}
	if(options)
		free(options);
	// print payload
	if(g_CoapRecvPointer->getPayloadLength() == 0) {
		sprintf(outBuffer,"%sNo payload\r\n", outBuffer);
	} else {
		sprintf(outBuffer, "%sPayload of %d bytes\r\n"
			"Value: \"%s\"\r\n", outBuffer,
			g_CoapRecvPointer->getPayloadLength(),
			(char *)g_CoapRecvPointer->getPayloadPointer());
		/*sprintf(outBuffer,"%sPayload of %d bytes\r\n", outBuffer,g_CoapRecvPointer->getPayloadLength());
		sprintf(outBuffer, "%sValue: \"", outBuffer);
		uint8_t* _payloadPointer = g_CoapRecvPointer->getPayloadPointer();
		for(int j=0; j<g_CoapRecvPointer->getPayloadLength(); j++) {
			char c = _payloadPointer[j];
			if((c>='!'&&c<='~')||c==' ') {
				sprintf(outBuffer, "%s%c", outBuffer,c);
			} else {
				sprintf(outBuffer, "%s\\%.2x",outBuffer,c);
			}
		}
		sprintf(outBuffer, "%s\"\r\n", outBuffer);*/
	}

}

void coapGetOptionValueById(uint16_t optionNumber, uint16_t * optionValueLength, uint8_t * optionValuePointer)
{
	if(g_CoapRecvPointer)
		g_CoapRecvPointer->getOptionValueById(optionNumber, optionValueLength, optionValuePointer);
}

uint16_t coapGetRecvMessageID() {
	return g_CoapRecvPointer->getMessageID();
}

mbtk_coap_type coapGetRecvType() {
	return (mbtk_coap_type)g_CoapRecvPointer->getType();
}

mbtk_coap_code_type coapGetRecvCode() {
	return (mbtk_coap_code_type)g_CoapRecvPointer->getCode();
}

int coapGetRecvTokenLength() {
	return g_CoapRecvPointer->getTokenLength();
}

uint8_t* coapGetRecvTokenPointer() {
	return g_CoapRecvPointer->getTokenPointer();
}

uint8_t coapGetRecvVersion() {
	return g_CoapRecvPointer->getVersion();
}

/// Returns a pointer to the payload buffer.
uint8_t* coapGetRecvPayloadPointer() {
	return g_CoapRecvPointer->getPayloadPointer();
}

/// Gets the length of the payload buffer.
int coapGetRecvPayloadLength() {
	return g_CoapRecvPointer->getPayloadLength();
}

// Return If-Match length,or 0 if fail.
uint16_t coapGetRecvOptionIfMatch(uint16_t *optionValueLength, uint8_t *optionValuePointer)
{
    *optionValueLength = 0;
    coapGetOptionValueById(COAP_OPTION_IF_MATCH,optionValueLength,optionValuePointer);
    return *optionValueLength;
}

// Return Uri-Host length,or 0 if fail.
uint16_t coapGetRecvOptionUriHost(uint16_t *optionValueLength, uint8_t *optionValuePointer)
{
    *optionValueLength = 0;
    coapGetOptionValueById(COAP_OPTION_URI_HOST,optionValueLength,optionValuePointer);
    return *optionValueLength;
}

// Return ETag length,or 0 if fail.
uint16_t coapGetRecvOptionETag(uint16_t *optionValueLength, uint8_t *optionValuePointer)
{
    *optionValueLength = 0;
    coapGetOptionValueById(COAP_OPTION_ETAG,optionValueLength,optionValuePointer);
    return *optionValueLength;
}

// Return If-None-Match length,or 0 if fail.
uint16_t coapGetRecvOptionIfNoneMatch(uint16_t *optionValueLength, uint8_t *optionValuePointer)
{
    *optionValueLength = 0;
    coapGetOptionValueById(COAP_OPTION_IF_NONE_MATCH,optionValueLength,optionValuePointer);
    return *optionValueLength;
}

// Return Location-Path length,or 0 if fail.
uint16_t coapGetRecvOptionLocationPath(uint16_t *optionValueLength, uint8_t *optionValuePointer)
{
    *optionValueLength = 0;
    coapGetOptionValueById(COAP_OPTION_LOCATION_PATH,optionValueLength,optionValuePointer);
    return *optionValueLength;
}

// Return Location-Query length,or 0 if fail.
uint16_t coapGetRecvOptionLocationQuery(uint16_t *optionValueLength, uint8_t *optionValuePointer)
{
    *optionValueLength = 0;
    coapGetOptionValueById(COAP_OPTION_LOCATION_QUERY,optionValueLength,optionValuePointer);
    return *optionValueLength;
}

// Return Proxy-Uri length,or 0 if fail.
uint16_t coapGetRecvOptionProxyUri(uint16_t *optionValueLength, uint8_t *optionValuePointer)
{
    *optionValueLength = 0;
    coapGetOptionValueById(COAP_OPTION_PROXY_URI,optionValueLength,optionValuePointer);
    return *optionValueLength;
}

// Return Proxy-Scheme length,or 0 if fail.
uint16_t coapGetRecvOptionProxyScheme(uint16_t *optionValueLength, uint8_t *optionValuePointer)
{
    *optionValueLength = 0;
    coapGetOptionValueById(COAP_OPTION_PROXY_SCHEME,optionValueLength,optionValuePointer);
    return *optionValueLength;
}

// Return Uri-Path length,or 0 if fail.
uint16_t coapGetRecvOptionUriPath(uint16_t *optionValueLength, uint8_t *optionValuePointer)
{
    *optionValueLength = 0;
    coapGetOptionValueById(COAP_OPTION_URI_PATH,optionValueLength,optionValuePointer);
    return *optionValueLength;
}

// Return Uri-Query length,or 0 if fail.
uint16_t coapGetRecvOptionUriQuery(uint16_t *optionValueLength, uint8_t *optionValuePointer)
{
    *optionValueLength = 0;
    coapGetOptionValueById(COAP_OPTION_URI_QUERY,optionValueLength,optionValuePointer);
    return *optionValueLength;
}

// Return 1 if get Observe success,or 0 if fail.
uint16_t coapGetRecvOptionObserve(uint16_t *observe)
{
    uint8_t buff[10];
    uint16_t buff_len = 0;
    memset(buff,0x0,10);
    coapGetOptionValueById(COAP_OPTION_OBSERVE,&buff_len,buff);
    if(buff_len > 0)
    {
        *observe = (uint16_t)atoi((char*)buff);
        return 1;
    } else {
        return 0;
    }
}

// Return 1 if get Max-Age success,or 0 if fail.
uint16_t coapGetRecvOptionMaxAge(uint16_t *max_age)
{
    uint8_t buff[10];
    uint16_t buff_len = 0;
    memset(buff,0x0,10);
    coapGetOptionValueById(COAP_OPTION_MAX_AGE,&buff_len,buff);
    if(buff_len > 0)
    {
        *max_age = (uint16_t)atoi((char*)buff);
        return 1;
    } else {
        return 0;
    }
}

// Return 1 if get Uri-Port success,or 0 if fail.
uint16_t coapGetRecvOptionUriPort(uint16_t *uri_port)
{
    uint8_t buff[10];
    uint16_t buff_len = 0;
    memset(buff,0x0,10);
    coapGetOptionValueById(COAP_OPTION_URI_PORT,&buff_len,buff);
    if(buff_len > 0)
    {
        *uri_port = (uint16_t)atoi((char*)buff);
        return 1;
    } else {
        return 0;
    }
}

// Return 1 if get Size2 success,or 0 if fail.
uint16_t coapGetRecvOptionSize2(uint16_t *size2)
{
    uint8_t buff[10];
    uint16_t buff_len = 0;
    memset(buff,0x0,10);
    coapGetOptionValueById(COAP_OPTION_SIZE2,&buff_len,buff);
    if(buff_len > 0)
    {
        *size2 = (uint16_t)atoi((char*)buff);
        return 1;
    } else {
        return 0;
    }
}

// Return 1 if get Size1 success,or 0 if fail.
uint16_t coapGetRecvOptionSize1(uint16_t *size1)
{
    uint8_t buff[10];
    uint16_t buff_len = 0;
    memset(buff,0x0,10);
    coapGetOptionValueById(COAP_OPTION_SIZE1,&buff_len,buff);
    if(buff_len > 0)
    {
        *size1 = (uint16_t)atoi((char*)buff);
        return 1;
    } else {
        return 0;
    }
}

// Return 1 if get Block2 success,or 0 if fail.
uint16_t coapGetRecvOptionBlock2(uint32_t *number,uint8_t *more_flag,uint8_t *size)
{
    uint8_t buff[10];
    uint16_t buff_len = 0;
    memset(buff,0x0,10);
    coapGetOptionValueById(COAP_OPTION_BLOCK2,&buff_len,buff);
    if(buff_len > 0)
    {
        if(buff_len == 1)
        {
            *size = (uint8_t)(buff[0] & 0x07);
            *more_flag = (uint8_t)((buff[0] & 0x08) >> 3);
            *number = (uint32_t)((buff[0] & 0xF0) >> 4);
        } else if(buff_len == 2) {
            *size = (uint8_t)(buff[1] & 0x07);
            *more_flag = (uint8_t)((buff[1] & 0x08) >> 3);
            *number = (uint32_t)(((buff[0] << 8) | (buff[1] & 0xF0)) >> 4);
        } else if(buff_len == 3) {
            *size = (uint8_t)(buff[2] & 0x07);
            *more_flag = (uint8_t)((buff[2] & 0x08) >> 3);
            *number = (uint32_t)(((buff[0] << 16) | (buff[1] << 8) | (buff[2] & 0xF0)) >> 4);
        } else {
            return 0;
        }
        return 1;
    } else {
        return 0;
    }
}

// Return 1 if get Block1 success,or 0 if fail.
uint16_t coapGetRecvOptionBlock1(uint32_t *number,uint8_t *more_flag,uint8_t *size)
{
    uint8_t buff[10];
    uint16_t buff_len = 0;
    memset(buff,0x0,10);
    coapGetOptionValueById(COAP_OPTION_BLOCK1,&buff_len,buff);
    if(buff_len > 0)
    {
        *size = (uint8_t)(buff[0] & 0x07);
        *more_flag = (uint8_t)(buff[0] & 0x08);
        if(buff_len == 1)
        {
            *number = (uint32_t)(buff[0] & 0xF0);
        } else if(buff_len == 2) {
            *number = (uint32_t)((buff[1] << 8) | (buff[0] & 0xF0));
        } else if(buff_len == 3) {
            *number = (uint32_t)((buff[2] << 16) | (buff[1] << 8) | (buff[0] & 0xF0));
        } else {
            return 0;
        }
        return 1;
    } else {
        return 0;
    }
}

// Return 1 if get Content-Format success,or 0 if fail.
uint16_t coapGetRecvOptionContentFormat(mbtk_content_format_type *type)
{
    uint8_t buff[10];
    uint16_t buff_len = 0;
    memset(buff,0x0,10);
    coapGetOptionValueById(COAP_OPTION_CONTENT_FORMAT,&buff_len,buff);
    if(buff_len > 0)
    {
        *type = (mbtk_content_format_type)atoi((char*)buff);
        return 1;
    } else {
        return 0;
    }
}

// Return 1 if get Accept success,or 0 if fail.
uint16_t coapGetRecvOptionAccept(mbtk_content_format_type *type)
{
    uint8_t buff[10];
    uint16_t buff_len = 0;
    memset(buff,0x0,10);
    coapGetOptionValueById(COAP_OPTION_ACCEPT,&buff_len,buff);
    if(buff_len > 0)
    {
        *type = (mbtk_content_format_type)atoi((char*)buff);
        return 1;
    } else {
        return 0;
    }
}

int coapPrintRecvPayload(char *out){
	// print payload
	int payloadLength = g_CoapRecvPointer->getPayloadLength();
	if(payloadLength==0) {
		return 0;
	} else {
		uint8_t* payloadPointer = g_CoapRecvPointer->getPayloadPointer();
		sprintf(out,"%s:%d\r\n",out, payloadLength*2);
		for(int j=0; j<payloadLength; j++) {
			sprintf(out,"%s%.2X",out,payloadPointer[j]);
		}
		sprintf(out,"%s\r\n",out);
		return 1;
	}
}

const char* coapPrintHumanByIndex(sint15 index){
	if(index == 0){
		createCoapPDU();
		return g_CoapPDU->printHuman();
	}
	else if(index == 1)
		return g_CoapRecvPointer->printHuman();
	return NULL;
}

const char* coapPrintHexByIndex(sint15 index){
	if(index == 0){
		createCoapPDU();
		return g_CoapPDU->printHex();
	}
	else if(index == 1)
		return g_CoapRecvPointer->printHex();
	return NULL;
}

}

#endif
