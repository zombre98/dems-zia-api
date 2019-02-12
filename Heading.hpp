//
// Created by bilel fourati on 2019-01-31.
//

#pragma once

#include <variant>
#include <memory>
#include <string>

namespace dems { namespace header {

/**
 * @class IHeading
 * @brief Interface for http header
 * This class is an interface that you must implement to access it via the Context
 */
class IHeaders {
public:
	/**
	 * The Destructor
	 */
	virtual ~IHeaders() = default;

	/**
	 * Give you the value corresponding to the Header Name
	 * For example: ( accept: application/json ) := getHeader("accept") => return "application/json"
	 * @param headerName The header name
	 * @return The value corresponding to the header name
	 */
	virtual std::string &getHeader(const std::string &headerName) const = 0;

	/**
	 * Set the value corresponding to the headerName
	 * @param headerName The header name (ex: accept)
	 * @param value The value corresponding to the header name (ex: application/json)
	 */
	virtual void setHeader(const std::string &headerName, const std::string &value) = 0;
};

/**
 * @struct Request
 * @brief Contain the first line of the http Header for the request:
 * GET /pub/WWW/TheProject.html HTTP/1.1 (See RFC for more details)
 */
struct Request {
	/**
	 * a public variable
	 * Contain the method (GET / POST / PUT / DELETE ...)
	 */
	std::string method;
	/**
	 * a public variable
	 * Contain the path ( /pub/WWW/TheProject.html )
	 */
	std::string path;
	/**
	 * a public variable
	 * Contain the http version ( HTTP/1.1 )
	 */
	std::string httpVersion;
};

/**
 * @struct Response
 * @brief Contain the first line of the http Header for the response:
 * HTTP/1.1 200 OK (See RFC for more details)
 */
struct Response {
	/**
	 * a public variable
	 * Contain the http version ( HTTP/1.1 )
	 */
	std::string httpVersion;
	/**
	 * a public variable
	 * Contain the status code (200, 404...)
	 */
	std::string statusCode;
	/**
	 * a public variable
	 * (optional) The message of the response (bad request, OK ...)
	 */
	std::string message;
};

/**
 * @struct HTTPMessage
 * @brief Describe a response or a request header
 */
struct HTTPMessage {
	/**
	 * The first line of a HTTP Header, it depends on either a request or a response
	 * For request the variant will be used as a Request Type
	 * For response the variant will be used as a Reponse Type
	 * (Technical details: https://en.cppreference.com/w/cpp/utility/variant)
	 */
	std::variant<Request, Response> firstLine;
	/**
	 * A pointer to a Heading Interface which contains all the informations about the HTTP header
	 */
	std::unique_ptr<IHeaders> headers;
	/**
	 * The Request or Response Body
	 */
	std::string body;
};

}}
