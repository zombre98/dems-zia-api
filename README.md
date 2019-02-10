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

When a Stage is triggered it will call the different hooks and will give them a Context.

 The **Context** is defined as follows :

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
     
    struct HTTPMessage {
        std::variant<Request, Response> firstLine;
        std::unique_ptr<IHeaders> headers;
        std::string body;
    };
}

struct Context {
    headers::HTTPMessage request;
    headers::HTTPMessage response;
    int socketFd;
};
 ```

#### Struct Context

```cpp
struct Context {
 	headers::HTTPMessage request;
 	headers::HTTPMessage response;
 	int socketFd;
 };
```

In the `Context` structure, the field `request` will contain the original request of the client.<br/>
The field `response` should be filled by the different modules.<br/>
We give you the field `socketFd` if you want to read or write data depending on the modules. 

#### Struct HTTPMessage

```cpp
struct HTTPMessage {
	std::variant<Request, Response> firstLine;
	std::unique_ptr<IHeaders> headers;
	std::string body;
};
```

The `firstLine` field contains a Request or Response depending on which HTTPMessage you are : <br/>
* In `request` the type of firstLine will be a `struct Request`
* In `response` the type of firstLine will be a `struct Response`

To use a `std::variant` here is an example :
(an std::variant is like an union in C, but it is type-safe)

```cpp
dems::Context context{{dems::headers::Request{"GET", "/path/file", "HTTP/1.1"}, std::make_unique<dems::headers::Heading>(), ""},
                     {dems::headers::Response{"HTTP/1.1", "200", "OK"},std::make_unique<dems::headers::Heading>(), ""}, 0};


std::cout << std::get<dems::headers::Request>(context.request.firstLine).path << std::endl;
```
On the fist line we show you how to create a `dems::Context`<br/>
On the last line we take the path from the variant Request by using `std::get<T>`, `T` being the type you want (in this case, it is either `Request` or `Response`).

The `headers` field contains a definition of Interface `IHeaders`

```cpp
class IHeaders {
public:
	virtual ~IHeaders() = default;

	virtual std::string &getHeader(const std::string &headerName) const = 0;

	virtual void setHeader(const std::string &headerName, const std::string &value) = 0;
};
```

All class who inherit from `IHeaders` must provide its own container to store
the Header, a headers is composed with a name and a value:

* Name: accept
* Value: application/json<br/>

(key / value, which container to use ? ...)
<br/>
<br/>
<br/>
The last field of the HTTPMessage is the `body`.
The `body` can be anything.<br/>

example:
```html
<html>
    <head></head>
    <body>
        <h1>Hello World</h1>
    </body>
</html>"
```

That's all !
If you have any questions contact:
[anatole.juge@epitech.eu](mailto:anatole.juge@epitech.eu?subject=G%201%20kestion%20sur%20la%20pays)

Here is a very very simple Logger Implementation:
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
