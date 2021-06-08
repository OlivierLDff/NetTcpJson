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

#include <NetTcpJson/Socket.hpp>
#include <NetTcpJson/Server.hpp>

#include <gtest/gtest.h>
#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>

class ServerTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        server.connectToSocket(
            [&](nettcpjson::Socket* s)
            {
                QObject::connect(s,
                    &nettcpjson::Socket::jsonAvailable,
                    [this, s](const QString& string)
                    {
                        //qInfo("RX \"%s\" from client %s:%d", qPrintable(string), qPrintable(s->peerAddress()), signed(s->peerPort()));
                        s->sendJson(string);
                    });
            });

        server.start("127.0.0.1", 30001);
    }

public:
    nettcpjson::Server server;
    nettcpjson::Socket client;

    QString lastLineServerRx;

    void testHelloWorld()
    {
        QSignalSpy connectedSpy(&client, &nettcpjson::Socket::isConnectedChanged);
        client.start("127.0.0.1", 30001);
        if(!client.isConnected())
        {
            ASSERT_TRUE(connectedSpy.wait());
        }

        QSignalSpy clientStringAvailable(&client, &nettcpjson::Socket::jsonAvailable);

        client.sendJson("{'Hello': 'World'}");

        ASSERT_TRUE(clientStringAvailable.wait());
        {
            const auto s = clientStringAvailable.takeFirst().at(0).toString();
            ASSERT_EQ(s, QString("{'Hello': 'World'}"));
        }
    }

    void longString()
    {
        QSignalSpy connectedSpy(&client, &nettcpjson::Socket::isConnectedChanged);
        client.start("127.0.0.1", 30001);
        if(!client.isConnected())
        {
            ASSERT_TRUE(connectedSpy.wait());
        }

        QSignalSpy clientStringAvailable(&client, &nettcpjson::Socket::jsonAvailable);

        QString longString;
        const int jsonMsgCount = 100000;
        for(int i = 0; i < jsonMsgCount; ++i)
        {
            longString += QString("{'Hello': %1}").arg(i);
        }

        client.sendJson(longString);

        while(clientStringAvailable.count() != jsonMsgCount)
        {
            ASSERT_TRUE(clientStringAvailable.wait());
        }

        for(int i = 0; i < jsonMsgCount; ++i)
        {
            const auto s = clientStringAvailable.takeFirst().at(0).toString();
            ASSERT_EQ(s, QString("{'Hello': %1}").arg(i));
        }
    }
};

TEST_F(ServerTests, helloWorld)
{
    server.setUseWorkerThread(false);
    client.setUseWorkerThread(false);
    testHelloWorld();
}

TEST_F(ServerTests, helloWorldAsync)
{
    server.setUseWorkerThread(true);
    client.setUseWorkerThread(true);
    testHelloWorld();
}

TEST_F(ServerTests, longString)
{
    server.setUseWorkerThread(false);
    client.setUseWorkerThread(false);
    longString();
}

TEST_F(ServerTests, longStringAsync)
{
    server.setUseWorkerThread(true);
    client.setUseWorkerThread(true);
    longString();
}
