NetTcpJson
=============

[![✅ Unit Tests](https://github.com/OlivierLDff/NetTcpJson/actions/workflows/unit-tests.yml/badge.svg)](https://github.com/Naostage/NetTcpJson/actions/workflows/unit-tests.yml)

<p align="center">
  <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/c/c9/JSON_vector_logo.svg/1200px-JSON_vector_logo.svg.png"/ width=200>
</p>

**NetTcpJson** library based on [NetTcp](https://github.com/OlivierLDff/NetTcp). The library provide a parser to extract json from buffer, and a tcp socket based on **NetTcp**.

## ✨ Overview

The library is separated between:

* **Extractor** where all the parsing logic happened. This is written in pure c++ and can be reuse in non Qt world
* **SocketWorker**: Implement reception/transmission via tcp socket.
* **Socket**: Create a SocketWorker
* **Server**: Create Socket for each connections.

## 🚀 How to use

### Socket - Client

```cpp
#include <NetTcpJson/Socket.hpp>

int main()
{
  // Create socket
  nettcpjson::Socket s;
  s.start("127.0.0.1", 30001);

  // Wait for JSON
  QObject::connect(s, &nettcpjson::Socket::jsonAvailable, s, [](const QString& json){
    qDebug() << "Receive JSON : " << json;
  });

  // Send JSON
  s.sendJSON("{'Hello': 'World'}");
}
```

### Server

```cpp
#include <NetTcpJson/Server.hpp>

int main()
{
  // Create socket
  nettcpjson::Server s;
  s.connectToSocket(
    [&](nettcpjson::Socket* s)
    {
      QObject::connect(s,
          &nettcpjson::Socket::jsonAvailable, s,
          [&](const QString& json)
          {
            qInfo("Receive JSON \"%s\" from client %s:%d", qPrintable(json), qPrintable(s->peerAddress()), signed(s->peerPort()));
          });
    });

  s.start("127.0.0.1", 30001);
}
```

## 🔨 Build

```bash
cmake -E make_directory build
cmake -B build -S .
cmake --build build --target "NetTcpJson" --config "Release" -j 8
```

## ✅ Run Unit Tests

```bash
cmake -E make_directory build
cmake -DNETTCPJSON_ENABLE_TESTS=ON -B build -S .
cmake --build build --target "NetTcpJsonTests" --config "Release" -j 8
cd build && ctest --build-config "Release" --progress --verbose
```

## 📄 License

```
MIT License

Copyright (c) 2021 Olivier Le Doeuff

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

```

## 👥 Authors

* [Olivier LDff](olivier.ldff@gmail.com)
