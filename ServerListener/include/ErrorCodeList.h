#pragma once
#ifndef __ERROR_CODE_LIST_H__
#define __ERROR_CODE_LIST_H__

//General code
#define	ERR_SUCCESS						0
#define ERR_FAILURE						1

//General Error 9xxx
#define ERR_BUFFER_IS_NULL				9001
#define ERR_ASCII_INV_DATA				9002
#define ERR_ASCII_CONVERT_FAIL			9003

//Http message error 8xxx
#define ERR_REQ_PARAM_NOT_FOUND			8001
#define ERR_SYMBOL_CONVERSION_FAILED	8002
#define ERR_INVALID_JSON				8003
#define ERR_REQ_OPS_INVALID				8004

//Database related error 7xxx
#define ERR_DB_CONNECTION_ERROR			7001
#define ERR_DB_OPS_INVALID				7002
#define ERR_DB_CLOSE_FAILED				7003
#define ERR_DB_INVALID_QUERY			7004

//Request Type 100x
#define HTTP_REQUEST_TYPE_INVALID		-1
#define HTTP_REQUEST_TYPE_GET			1000
#define HTTP_REQUEST_TYPE_POST			1001

//Operation Type 200x
#define OPS_TYPE_INVALID				-2
#define OPS_TYPE_INSERT					2000
#define OPS_TYPE_VIEW					2001
#define OPS_TYPE_UPDATE					2002
#define OPS_TYPE_DELETE					2003
#endif
