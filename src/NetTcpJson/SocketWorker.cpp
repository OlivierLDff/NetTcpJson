//
// MIT License
//
// Copyright (c) 2021 Olivier Le Doeuff
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <NetTcpJson/SocketWorker.hpp>
#include <NetTcpJson/Extractor.hpp>

namespace nettcpjson {

SocketWorker::SocketWorker(QObject* parent)
    : net::tcp::SocketWorker(parent)
    , _extrator(std::make_unique<Extractor>())
{
    _buffer.resize(4096);
}

SocketWorker::~SocketWorker() = default;

void SocketWorker::onConnected()
{
    net::tcp::SocketWorker::onConnected();
    _extrator->clear();
}

void SocketWorker::onDataAvailable()
{
    while(bytesAvailable())
    {
        // In debug mode, it is easier to catch bugs or overflow with a clean buffer
#ifndef NDEBUG
        std::memset(_buffer.data(), 0xCC, _buffer.size());
#endif

        const auto bytesRead = int(read(_buffer.data(), _buffer.size()));

        if(!_extrator->extract(_buffer.data(), bytesRead))
            return closeAndRestart();

        const auto jsonStrings = _extrator->takeAvailableStrings();
        Q_ASSERT(_extrator->availableStrings().empty());

        for(const auto& json: jsonStrings)
        {
            Q_EMIT jsonAvailable(QString::fromStdString(json));
        }
    }
}

void SocketWorker::sendJson(const QString& s)
{
    const auto data = s.toUtf8();
    if(!write(data.constData(), data.length()))
        return closeAndRestart();
}

void SocketWorker::setBufferSize(quint32 bufferSize)
{
    if(bufferSize)
    {
        _buffer.resize(bufferSize);
    }
}
}

#include "moc_SocketWorker.cpp"
