# HTTP Server
> Yet another HTTP server implemented in C++


## Running Steps
1. Prepare environments:
```shell
$ uname -r
5.10.16.3-microsoft-standard-WSL2
$ sudo apt install build-essential cmake
$ cmake --version
cmake version 3.22.1
$ gcc --version
gcc (Ubuntu 11.2.0-19ubuntu1) 11.2.0
```
2. Build executables:
```shell
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
# or
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
3. Start the server:
```shell
./server
```

## Basic Usages
Please refer [main.cpp](./main.cpp) for details.
1. Initialize server & event loop_:
```C++
EventLoop loop_;
HTTPServer server(&loop_, 3000, 4);
```
2. Define controllers:
```C++
void getEchoPage(const HTTPRequestPtr &req, const HTTPResponsePtr &res) {
    res->send("<li> Method: " + req->method + "</li><li> URL: " + req->url + "</li>");
}
```
3. Register routes with controllers:
```C++
server.setRoute("/", getIndexPage);
server.setRoute("/echo", getEchoPage);
server.serveStatic("/public");
```
4. Start serving:
```C++
server.start();
loop_.loop_();
```