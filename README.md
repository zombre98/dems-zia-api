# dems API Zia
##### The goal of this API is to provide an example on how to add modules to your Zia project
###### Produced by Armand Megrot, Bilel Fourati, Anatole Juge and Thomas Burgaud

The goal of this API is to simplify the way you add modules in your ZIA
In order to do that we tried to minimize the constraints that we impose

Documentation : https://zia.bilel-fourati.fr/

For example to add a module you just have to do:
```cpp
extern "C" {

    void registerHooks(api::StageManager &manager) {
        manager.request().hookToEnd("MyModule", [](api::Context &ctx) {
            std::cout << "I'm an example module" << std::endl;
            return api::CodeStatus::OK;
        });
    }

};
```

## AModulesManager

AModulesManager should be instanciated **only once** in your project.
It will let you load a directory of modules or a single module via the functions `loadModules` or `loadOneModule` respectively.
The modules **must** be loaded in alphabetical order.
AModulesManager provides a StageManager through `getStageManager` (cf. StageManager section below).

Each module must expose a function called `registerHooks` through the use of `extern`.
The server must call this function passing the StageManager as arguement. The module will use it to hook functions at different stages of the request process.

For example, here is some code that would go in the `registerHooks` function of a module:
```cpp
stageManager.request().hookToMiddle("MyModule", myHandlerFunction);
```
(See the following sections for an explanation of `request()` and `hookToMiddle`)

## StageManager

StageManager should handle the different stages of a request process.
Functions registered to each stage should be called when the following happens:

| Stage | Description |
| --- | --- |
| **connection** | When a connection is accepted. |
| **request** | When the server has read the request. |
| **chunks** | When data is received by chunks (see [rfc2616](https://www.ietf.org/rfc/rfc2616.txt) section 3.6.1) functions hooked to this stage will be called everytime a chunk is read. |
| **disconnection** | When the client disconnects from the server. |


## Stages

A stage is composed of three moments which permit the organization of modules in the specified stage.
They are required through the use of the three function:
* `hookToFirst`
* `hookToMiddle`
* `hookToLast`
available on each stage.

| Moment | Description |
| --- | --- |
| **first** | Early hook for modules that must prepare the request (e.g. SSL module). |
| **middle** | Mostly for processing modules (e.g. PHP module). |
| **last** | Called after processing (e.g. Logging module). |

**If multiple functions hook to the same moment they will be called in the order the modules were loaded.**

---

# How It Works (in practice)

1. A request is received 
2. The server calls the "connection" stage.
3. It calls the functions hooked by the modules to the "connection" stage:
 * First hooks are called,
 * then Middle hooks are called,
 * finally Last hooks are called.
4. It continues with the following stages...

---

# Hooked functions

Hooked functions take a Context (see Context section below) and return a status code defined by the enum:
```cpp
 enum class CodeStatus {
 	OK, // The module accepts the call
 	DECLINED, // The module declines the call
 	HTTP_ERROR // The module alerts fo an error
 };
 ```

## Context

The context given to each hook contains information about the processing of a request.
```cpp
struct Context {
    headers::HTTPMessage request;
    headers::HTTPMessage response;
    int socketFd;
};
```
* `request` will contain the original request of the client.
* `response` should be filled by the different modules.
* `socketFd` is here as a low level access to raw data.

```cpp
namespace headers {
    struct HTTPMessage {
        std::variant<Request, Response> firstLine;
        std::unique_ptr<IHeaders> headers;
        std::string body;
    };
}
```
* `firstLine` contains a Request or a Response depending on which HTTPMessage you use (see below for explanation of `std::variant`).
```cpp
namespace headers {
    struct Request {
        std::string method;
        std::string path;
        std::string httpVersion;
    };

    struct Response {
        std::string httpVersion;
        std::string statusCode;
        std::string message;
    };
}
```
* `headers` contains a `IHeaders` instance (see Headers section).
* `body` contains the request's or response's body if any.

#### std::variant

Here is an example of how to use a `std::variant`:
(a std::variant is like an `union`, but type-safe)
```cpp
// This is how you staticaly create a dems::Context.
dems::Context context{
	{
		dems::headers::Request{"GET", "/path/file", "HTTP/1.1"},
		std::make_unique<dems::headers::Heading>(),
		""
	},{
		dems::headers::Response{"HTTP/1.1", "200", "OK"},
		std::make_unique<dems::headers::Heading>(),
		""
	}, 0
};

// This is how you retrieve the Request's firstLine property using std::get and log its path member.
std::cout << std::get<dems::headers::Request>(context.request.firstLine).path << std::endl;
```

#### Headers

As headers are often used in the processing of a request, we choosed to impose the implementation of a container for headers. We made it look as simple as possible.
```cpp
class IHeaders {
public:
	virtual ~IHeaders() = default;

	virtual std::string &getHeader(const std::string &headerName) const = 0;
	virtual const std::string &getHeader(const std::string &headerName) const = 0;

	virtual void setHeader(const std::string &headerName, const std::string &value) = 0;
};
```
Every headers are key/value pairs. It's up to you to choose the underlying container.
A headers presents as follows:
* Name: `accept`
* Value: `application/json`

---

That's all !
If you have any questions contact:
[anatole.juge@epitech.eu](mailto:anatole.juge@epitech.eu?subject=G%201%20kestion%20sur%20la%20pays)

Here is a very very simple Logger module implementation:
```cpp
static constexpr char MODULE_NAME[] = "Logger";

extern "C" {

void registerHooks(dems::StageManager &manager) {
    manager.request().hookToFirst(MODULE_NAME, [](dems::Context &ctx) {
        std::cout << "Stage: Request FIRST" << std::endl;
        std::cout << ctx.response.body << std::endl;
        return dems::CodeStatus::OK;
    });

    manager.request().hookToMiddle(MODULE_NAME, [](dems::Context &ctx) {
        std::cout << "Stage: Request MIDDLE" << std::endl;
        std::cout << ctx.response.body << std::endl;
        return dems::CodeStatus::OK;
    });

    manager.request().hookToEnd(MODULE_NAME, [](dems::Context &ctx) {
        std::cout << "Stage: Request END" << std::endl;
        std::cout << ctx.response.body << std::endl;
        return dems::CodeStatus::OK;
    });
}

};
```
