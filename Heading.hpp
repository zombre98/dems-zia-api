//
// Created by bilel fourati on 2019-01-31.
//

#pragma once

#include <variant>
#include <memory>
#include <string>

/**
 * @brief dems namespace
 */
namespace dems {

/**
 * @brief header namespace
 */
namespace header {

/**
 * @class IHeaders
 * @brief Interface for http headers
 * This class is an interface that you must implement to access it via the Context
 */
class IHeaders {
public:
	/**
	 * The Destructor
	 */
	virtual ~IHeaders() = default;

	/**
	 * Gives you the value corresponding to the header name
	 * @param headerName The header name
	 * @return The value corresponding to the header name
	 */
	virtual std::string &operator[](const std::string &headerName) = 0;

  	/**
	 * Give you the consteness value corresponding to the Header Name
	 * For example: ( accept: application/json ) := getHeader("accept") => return "application/json"
	 * @param headerName The header name
	 * @return The value corresponding to the header name
	 */
	virtual std::string const &getHeader(const std::string &headerName) const = 0;

	/*
	 * Return all the headers separated the line by one CRLF
	 * @return All headers add by setHeader of operator[]
	 */
	virtual std::string getWholeHeaders() const = 0;

	/**
	 * Sets the value corresponding to the header name
	 * @param headerName The header name (ex: accept)
	 * @param value The value corresponding to the header name (ex: application/json)
	 */
	virtual void setHeader(const std::string &headerName, const std::string &value) = 0;
};

/**
 * @struct Request
 * @brief Contains the first line of the http header for the request:
 * GET /pub/WWW/TheProject.html HTTP/1.1 (See RFC for more details)
 */
struct Request {
	/**
	 * a public variable
	 * Contains the method (GET / POST / PUT / DELETE ...)
	 */
	std::string method;
	/**
	 * a public variable
	 * Contains the path ( /pub/WWW/TheProject.html )
	 */
	std::string path;
	/**
	 * a public variable
	 * Contains the http version ( HTTP/1.1 )
	 */
	std::string httpVersion;
};

/**
 * @struct Response
 * @brief Contains the first line of the http Header for the response:
 * HTTP/1.1 200 OK (See RFC for more details)
 */
struct Response {
	/**
	 * a public variable
	 * Contains the http version ( HTTP/1.1 )
	 */
	std::string httpVersion;
	/**
	 * a public variable
	 * Contains the status code (200, 404...)
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
	 * For response the variant will be used as a Response Type
	 * (Technical details: https://en.cppreference.com/w/cpp/utility/variant)
	 */
	std::variant<Request, Response> firstLine;
	/**
	 * A pointer to a Heading Interface which contains all the information about the HTTP header
	 */
	std::unique_ptr<IHeaders> headers;
	/**
	 * The Request or Response Body
	 */
	std::string body;
};

}}
