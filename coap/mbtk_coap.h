
#ifndef __MBTK_COAP_H__
#define __MBTK_COAP_H__
//#include <stdint.h>
#include "m_type.h"
#ifndef FEATURE_MBTK_COAP
#define FEATURE_MBTK_COAP
#endif

#ifdef FEATURE_MBTK_COAP
#define COAP_HDR_SIZE 4
#define COAP_OPTION_HDR_BYTE 1

typedef unsigned char               uint8_t;
typedef unsigned short              uint16_t;
//typedef unsigned long               uint32_t;
//typedef unsigned long long          uint64_t;

#define ps_ntohs(x)                                                      \
          (((((uint16)(x) & 0x00FF) << 8) | (((uint16)(x) & 0xFF00) >> 8)))

#define endian_be16(x) ps_ntohs(x)

#define endian_load16(cast, from) ((cast)( \
        (((uint16_t)((uint8_t*)(from))[0]) << 8) | \
        (((uint16_t)((uint8_t*)(from))[1])     ) ))

#define endian_load32(cast, from) ((cast)( \
        (((uint32_t)((uint8_t*)(from))[0]) << 24) | \
        (((uint32_t)((uint8_t*)(from))[1]) << 16) | \
        (((uint32_t)((uint8_t*)(from))[2]) <<  8) | \
        (((uint32_t)((uint8_t*)(from))[3])      ) ))

#define endian_load64(cast, from) ((cast)( \
        (((uint64_t)((uint8_t*)(from))[0]) << 56) | \
        (((uint64_t)((uint8_t*)(from))[1]) << 48) | \
        (((uint64_t)((uint8_t*)(from))[2]) << 40) | \
        (((uint64_t)((uint8_t*)(from))[3]) << 32) | \
        (((uint64_t)((uint8_t*)(from))[4]) << 24) | \
        (((uint64_t)((uint8_t*)(from))[5]) << 16) | \
        (((uint64_t)((uint8_t*)(from))[6]) << 8)  | \
        (((uint64_t)((uint8_t*)(from))[7])     )  ))

#define endian_store16(to, num) \
    do { uint16_t val = endian_be16(num); memcpy(to, &val, 2); } while(0)

// CoAP PDU format

//   0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |Ver| T |  TKL  |      Code     |          Message ID           |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |   Token (if any, TKL bytes) ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |   Options (if any) ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |1 1 1 1 1 1 1 1|    Payload (if any) ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class CoapPDU {


	public:
		/// CoAP message types. Note, values only work as enum.
		enum Type {
			COAP_CONFIRMABLE=0x00,
			COAP_NON_CONFIRMABLE=0x10,
			COAP_ACKNOWLEDGEMENT=0x20,
			COAP_RESET=0x30
		};

		// CoAP response codes.
		enum Code {
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
		};

		/// CoAP option numbers.
		enum Option {
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
		};

		/// CoAP content-formats.
		enum ContentFormat {
			COAP_CONTENT_FORMAT_TEXT_PLAIN = 0,
			COAP_CONTENT_FORMAT_APP_LINK  = 40,
			COAP_CONTENT_FORMAT_APP_XML,
			COAP_CONTENT_FORMAT_APP_OCTET,
			COAP_CONTENT_FORMAT_APP_EXI   = 47,
			COAP_CONTENT_FORMAT_APP_JSON  = 50
		};

		/// Sequence of these is returned by CoapPDU::getOptions()
		struct CoapOption {
			uint16_t optionDelta;
			uint16_t optionNumber;
			uint16_t optionValueLength;
			int totalLength;
			uint8_t *optionPointer;
			uint8_t *optionValuePointer;
		};

		// construction and destruction
		CoapPDU();
		CoapPDU(uint8_t *buffer, int bufferLength, int pduLength);
		~CoapPDU();
		int reset();
		int validate();

		// version
		int setVersion(uint8_t version);
		uint8_t getVersion();

		// message type
		void setType(int type);
		int getType();

		// tokens
		int setTokenLength(uint8_t tokenLength);
		int getTokenLength();
		uint8_t* getTokenPointer();
		int setToken(uint8_t *token, uint8_t tokenLength);

		// message code
		void setCode(int code);
		int getCode();
		int httpStatusToCode(int httpStatus);

		// message ID
		int setMessageID(uint16_t messageID);
		uint16_t getMessageID();

		// options
		int addOption(uint16_t optionNumber, uint16_t optionLength, uint8_t *optionValue);
		// gets a list of all options
		CoapOption* getOptions();
		int getNumOptions();
		// shorthand helpers
		int setURI(char *uri);
		int setURI(char *uri, int urilen);
		int getURI(char *dst, int dstlen, int *outLen);
		int addURIQuery(char *query);

		// content format helper
		int setContentFormat(int format);

		// payload
		uint8_t* mallocPayload(int bytes);
		int setPayload(uint8_t *value, int len);
		uint8_t* getPayloadPointer();
		int getPayloadLength();
		uint8_t* getPayloadCopy();

		// pdu
		int getPDULength();
		uint8_t* getPDUPointer();
		void setPDULength(int len);
		void getOptionValueById(uint16_t optionNumber , uint16_t *optionValueLength,uint8_t *optionValuePointer);
		const char *printHuman();
		const char * printHex();
		int hasOption(uint16_t optionNumber);
	private:
		// variables
		uint8_t *_pdu;
		int _pduLength;

		int _constructedFromBuffer;
		int _bufferLength;

		uint8_t *_payloadPointer;
		int _payloadLength;

		int _numOptions;
		uint16_t _maxAddedOptionNumber;
		//ASBuffer content;

		// functions
		void shiftPDUUp(int shiftOffset, int shiftAmount);
		void shiftPDUDown(int startLocation, int shiftOffset, int shiftAmount);
		uint8_t codeToValue(int c);

		// option stuff
		int findInsertionPosition(uint16_t optionNumber, uint16_t *prevOptionNumber);
		int computeExtraBytes(uint16_t n);
		int insertOption(int insertionPosition, uint16_t optionDelta, uint16_t optionValueLength, uint8_t *optionValue);
		uint16_t getOptionDelta(uint8_t *option);
		void setOptionDelta(int optionPosition, uint16_t optionDelta);
		uint16_t getOptionValueLength(uint8_t *option);

};

/*
#define COAP_CODE_EMPTY 0x00

// method codes 0.01-0.31
#define COAP_CODE_GET 	0x01
#define COAP_CODE_POST 	0x02
#define COAP_CODE_PUT 	0x03
#define COAP_CODE_DELETE 0x04

// Response codes 2.00 - 5.31
// 2.00 - 2.05
#define COAP_CODE_CREATED 0x41
#define COAP_CODE_DELETED 0x42
#define COAP_CODE_VALID   0x43
#define COAP_CODE_CHANGED 0x44
#define COAP_CODE_CONTENT 0x45

// 4.00 - 4.15
#define COAP_CODE_BAD_REQUEST                0x80
#define COAP_CODE_UNAUTHORIZED               0x81
#define COAP_CODE_BAD_OPTION                 0x82
#define COAP_CODE_FORBIDDEN                  0x83
#define COAP_CODE_NOT_FOUND                  0x84
#define COAP_CODE_METHOD_NOT_ALLOWED         0x85
#define COAP_CODE_NOT_ACCEPTABLE             0x86
#define COAP_CODE_PRECONDITION_FAILED        0x8C
#define COAP_CODE_REQUEST_ENTITY_TOO_LARGE   0x8D
#define COAP_CODE_UNSUPPORTED_CONTENT_FORMAT 0x8F

// 5.00 - 5.05
#define COAP_CODE_INTERNAL_SERVER_ERROR      0xA0
#define COAP_CODE_NOT_IMPLEMENTED            0xA1
#define COAP_CODE_BAD_GATEWAY                0xA2
#define COAP_CODE_SERVICE_UNAVAILABLE        0xA3
#define COAP_CODE_GATEWAY_TIMEOUT            0xA4
#define COAP_CODE_PROXYING_NOT_SUPPORTED     0xA5
*/
#endif
#endif
