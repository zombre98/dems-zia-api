# dems API Zia
##### The goal of this API is to provide an example on how to add modules to your Zia project
###### Produced by Armand Megrot, Bilel Fourati, Anatole Juge and Thomas Burgaud

The goal of this API is to simplify the way you add modules in your ZIA<br/>
In order to do that we tried to minimize the constraints that we impose

Documentation: https://zia.bilel-fourati.fr/<br/>
Discord: https://discord.gg/YymhyAx<br/>
Issues: https://github.com/zombre98/dems-zia-api/issues

For example to add a module you just have to do:
```cpp
extern "C" {

  std::string registerHooks(dems::StageManager &manager) {
        manager.request().hookToEnd(0, "MyModule", [](dems::Context &ctx) {
            std::cout << "I'm an example module" << std::endl;
            return dems::CodeStatus::OK;
        });
        return "MyModule";
    }

};
```

## AModulesManager

AModulesManager should be instanciated **only once** in your project.<br/>
It will let you load a directory of modules or a single module via the functions `loadModules` or `loadOneModule` respectively.<br/>
AModulesManager provides a StageManager through `getStageManager` (cf. StageManager section below).

Each module must expose a function called `registerHooks` through the use of `extern` (for unix based system...), it must return the name of the module.<br/>
The server must call this function passing the StageManager as arguement. The module will use it to hook functions at different stages of the request process.

For example, here is some code that would go in the `registerHooks` function of a module:
```cpp
stageManager.request().hookToMiddle(0, "MyModule", myHandlerFunction);
```
(See the following sections for an explanation of `request()` and `hookToMiddle`)

## StageManager

StageManager should handle the four different stages of a request process.<br/>
Functions registered to each stage should be called when the following happens:

| Stage | Description |
| --- | --- |
| **connection** | When a connection is accepted. |
| **request** | When the server has read the request. |
| **chunks** | When data is received by chunks (see [rfc2616](https://www.ietf.org/rfc/rfc2616.txt) section 3.6.1) functions hooked to this stage will be called everytime a chunk is read. |
| **disconnection** | When the client disconnects from the server. |


## Stages

A stage is composed of three moments allowing the organization of modules in the specified stage.<br/>
They are used through
* `hookToFirst`,
* `hookToMiddle`
* and `hookToLast`

available on each stage.

| Moment | Description |
| --- | --- |
| **first** | Early hook for modules that must prepare the request (e.g. SSL module). |
| **middle** | Mostly for processing modules (e.g. PHP module). |
| **last** | Called after processing (e.g. Logging module). |

**If you wan't to hook multiple modules to the same hook you can choose the execution order by giving an index to the function.**<br/>
**The index will be used to sort the Stage Object map.**<br/>
**If you choose the same index on the same hook it will be ordered by the emplace order. **

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
 	HTTP_ERROR // The module alerts in case of an error
 };
 ```

## Context

The context given to each hook contains information about the processing of a request.
```cpp
struct Context {
    std::vector<uint8_t> rawData;
    headers::HTTPMessage request;
    headers::HTTPMessage response;
    int socketFd;
    dems::config::Config config;
};
```
* `request` will contain the original request of the client.
* `response` should be filled by the different modules.
* `socketFd` is here as a low level access to raw data.
* `config` contain settings of modules

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

Here is an example of how to use a `std::variant`:<br/>
(a [std::variant](https://en.cppreference.com/w/cpp/utility/variant) is like an `union`, but type-safe)
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

As headers are often used in the processing of a request, we chose to impose the implementation of a container for headers. We made it look as simple as possible.
```cpp
class IHeaders {
public:
	virtual ~IHeaders() = default;

	virtual std::string &operator[](const std::string &headerName) = 0;
	virtual const std::string &getHeader(const std::string &headerName) const = 0;

	virtual void setHeader(const std::string &headerName, const std::string &value) = 0;
};
```
Every headers are key/value pairs. It's up to you to choose the underlying container.<br/>
A headers presents as follows:
* Name: `accept`
* Value: `application/json`

#### Config

Format of the minimum configuration file :
```json
{
	"modules": {
		"PhP": {"path": "path/to/PhP", "option":"Option 1"},
		"SSL": {"path": "path/to/ssl"}
	}
}
```

---

That's all !

Here is a very very simple Logger module implementation:
```cpp
static constexpr char MODULE_NAME[] = "Logger";

extern "C" {

  std::string registerHooks(dems::StageManager &manager) {
    manager.request().hookToFirst(0, MODULE_NAME, [](dems::Context &ctx) {
        std::cout << "Stage: Request FIRST" << std::endl;
        std::cout << ctx.response.body << std::endl;
        return dems::CodeStatus::OK;
    });

    manager.request().hookToMiddle(0, MODULE_NAME, [](dems::Context &ctx) {
        std::cout << "Stage: Request MIDDLE" << std::endl;
        std::cout << ctx.response.body << std::endl;
        return dems::CodeStatus::OK;
    });

    manager.request().hookToEnd(0, MODULE_NAME, [](dems::Context &ctx) {
        std::cout << "Stage: Request END" << std::endl;
        std::cout << ctx.response.body << std::endl;
        return dems::CodeStatus::OK;
    });
    return MODULE_NAME;
}

};
```
