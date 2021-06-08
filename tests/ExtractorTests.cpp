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

#include <NetTcpJson/Extractor.hpp>

#include <gtest/gtest.h>

class ExtractorTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

public:
    nettcpjson::Extractor extractor;
};

TEST_F(ExtractorTests, simpleJson)
{
    const std::string json = R"({"field": 123,})";
    ASSERT_TRUE(extractor.extract(reinterpret_cast<const std::uint8_t*>(json.c_str()), json.size()));

    const auto jsons = extractor.takeAvailableStrings();

    ASSERT_EQ(jsons.size(), 1);
    const auto result = jsons.at(0);
    ASSERT_EQ(json, result);

    ASSERT_TRUE(extractor.takeAvailableStrings().empty());
}

TEST_F(ExtractorTests, simpleJsonMultipleBuffer)
{
    {
        const std::string json = R"({"fiel)";
        ASSERT_TRUE(extractor.extract(reinterpret_cast<const std::uint8_t*>(json.c_str()), json.size()));
    }
    {
        const std::string json = R"(d": 1)";
        ASSERT_TRUE(extractor.extract(reinterpret_cast<const std::uint8_t*>(json.c_str()), json.size()));
    }
    {
        const std::string json = R"(23})";
        ASSERT_TRUE(extractor.extract(reinterpret_cast<const std::uint8_t*>(json.c_str()), json.size()));
    }

    const auto jsons = extractor.takeAvailableStrings();
    ASSERT_EQ(jsons.size(), 1);
    const auto& result = jsons.at(0);
    ASSERT_EQ(R"({"field": 123})", result);

    ASSERT_TRUE(extractor.lastBuffer().empty());
}

TEST_F(ExtractorTests, multipleJson)
{
    const std::string json = R"({"field": 123}{"other": [54, 12]})";
    ASSERT_TRUE(extractor.extract(reinterpret_cast<const std::uint8_t*>(json.c_str()), json.size()));

    const auto jsons = extractor.takeAvailableStrings();
    ASSERT_EQ(jsons.size(), 2);
    {
        const auto& result = jsons.at(0);
        ASSERT_EQ(R"({"field": 123})", result);
    }
    {
        const auto& result = jsons.at(1);
        ASSERT_EQ(R"({"other": [54, 12]})", result);
    }

    ASSERT_TRUE(extractor.lastBuffer().empty());
}

TEST_F(ExtractorTests, multipleJsonMultipleBuffer)
{
    {
        const std::string json = R"({"field": 123}{"other": [54, 12])";
        ASSERT_TRUE(extractor.extract(reinterpret_cast<const std::uint8_t*>(json.c_str()), json.size()));
    }
    ASSERT_EQ(extractor.availableStrings().size(), 1);
    {
        const std::string json = R"(}{'Hello':)";
        ASSERT_TRUE(extractor.extract(reinterpret_cast<const std::uint8_t*>(json.c_str()), json.size()));
    }
    ASSERT_EQ(extractor.availableStrings().size(), 2);
    {
        const std::string json = R"( "World"}{)";
        ASSERT_TRUE(extractor.extract(reinterpret_cast<const std::uint8_t*>(json.c_str()), json.size()));
    }
    ASSERT_EQ(extractor.availableStrings().size(), 3);
    {
        const std::string json = R"(})";
        ASSERT_TRUE(extractor.extract(reinterpret_cast<const std::uint8_t*>(json.c_str()), json.size()));
    }
    ASSERT_EQ(extractor.availableStrings().size(), 4);
    ASSERT_TRUE(extractor.lastBuffer().empty());

    const auto jsons = extractor.takeAvailableStrings();
    ASSERT_EQ(jsons.size(), 4);
    {
        const auto& result = jsons.at(0);
        ASSERT_EQ(R"({"field": 123})", result);
    }
    {
        const auto& result = jsons.at(1);
        ASSERT_EQ(R"({"other": [54, 12]})", result);
    }
    {
        const auto& result = jsons.at(2);
        ASSERT_EQ(R"({'Hello': "World"})", result);
    }
    {
        const auto& result = jsons.at(3);
        ASSERT_EQ(R"({})", result);
    }

    ASSERT_TRUE(extractor.lastBuffer().empty());
}
