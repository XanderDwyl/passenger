/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2014 Phusion
 *
 *  "Phusion Passenger" is a trademark of Hongli Lai & Ninh Bui.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */
#ifndef _PASSENGER_SERVER_KIT_HTTP_CLIENT_H_
#define _PASSENGER_SERVER_KIT_HTTP_CLIENT_H_

#include <Utils/sysqueue.h>
#include <ServerKit/Client.h>
#include <ServerKit/HttpRequest.h>
#include <ServerKit/HttpHeaderParser.h>

namespace Passenger {
namespace ServerKit {


template<typename Request = HttpRequest>
class BaseHttpClient: public BaseClient {
public:
	typedef Request RequestType;
	LIST_HEAD(RequestList, Request);

	/**
	 * @invariant
	 *     if currentRequest != NULL:
	 *         currentRequest->httpState != HttpRequest::WAITING_FOR_REFERENCES
	 *         currentRequest->httpState != HttpRequest::IN_FREELIST
	 */
	Request *currentRequest;
	HttpHeaderParser *reqHeaderParser;

	BaseHttpClient(void *server)
		: BaseClient(server),
		  currentRequest(NULL),
		  reqHeaderParser(NULL)
	{

	}

	virtual void deinitialize() {
		currentRequest = NULL;
		reqHeaderParser = NULL;
		BaseClient::deinitialize();
	}
};


#define SERVER_KIT_BASE_HTTP_CLIENT_INIT() \
	LIST_INIT(&endedRequests); \
	endedRequestCount = 0

#define DEFINE_SERVER_KIT_BASE_HTTP_CLIENT_FOOTER(ClientType, RequestType) \
	DEFINE_SERVER_KIT_BASE_CLIENT_FOOTER(ClientType); \
	/* Last field from BASE_CLIENT_FOOTER is an int, so we put an */ \
	/* unsigned int here first to avoid an alignment hole on x86_64. */ \
	unsigned int endedRequestCount; \
	typename BaseHttpClient<RequestType>::RequestList endedRequests


template<typename Request = HttpRequest>
class HttpClient: public BaseHttpClient<Request> {
public:
	HttpClient(void *server)
		: BaseHttpClient<Request>(server)
	{
		SERVER_KIT_BASE_HTTP_CLIENT_INIT();
	}

	DEFINE_SERVER_KIT_BASE_HTTP_CLIENT_FOOTER(HttpClient, Request);
};


} // namespace ServerKit
} // namespace Passenger

#endif /* _PASSENGER_SERVER_KIT_HTTP_CLIENT_H_ */
