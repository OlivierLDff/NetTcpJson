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

namespace nettcpjson {

bool Extractor::extract(const std::uint8_t* buffer, const std::size_t& length)
{
    if(!buffer || !length)
        return false;

    const std::uint8_t* realBuffer = buffer;
    std::size_t realLength = length;
    std::vector<std::uint8_t> tempData;

    // Special case that should almost never, so we can take performance penalty and not complexify parsing code
    // Let's create a copy buffer of current buffer and last
    if(!_lastBuffer.empty())
    {
        tempData = std::move(_lastBuffer);
        tempData.insert(std::end(tempData), buffer, buffer + length);

        realBuffer = tempData.data();
        realLength = tempData.size();
    }

    std::size_t bracketCount = 0;
    std::size_t bracketStart = 0;

    std::size_t bufferRemainingStart = 0;

    for(std::size_t i = 0; i < realLength; ++i)
    {
        const auto byte = realBuffer[i];
        if(byte == '{')
        {
            if(!bracketCount)
            {
                bracketStart = i;
            }

            ++bracketCount;
        }
        else if(byte == '}')
        {
            if(!bracketCount)
            {
                return false;
            }

            --bracketCount;

            if(!bracketCount)
            {
                bufferRemainingStart = i + 1;
                _availableStrings.emplace_back(reinterpret_cast<const char*>(&realBuffer[bracketStart]), i - bracketStart + 1);
            }
        }
    }

    // Store buffer for later read attempt
    if(bufferRemainingStart != realLength)
    {
        const auto toSaveLength = realLength - bufferRemainingStart;
        if(toSaveLength > _maxBufferAllowedSize)
            return false;

        if(tempData.empty() || toSaveLength != realLength)
            _lastBuffer = std::vector<std::uint8_t>(realBuffer + bufferRemainingStart, realBuffer + realLength);
        else
            _lastBuffer = std::move(tempData);
    }

    return true;
}

void Extractor::clear()
{
    _lastBuffer = {};
}

std::vector<std::string> Extractor::takeAvailableStrings()
{
    return std::move(_availableStrings);
}

const std::vector<std::string>& Extractor::availableStrings() const
{
    return _availableStrings;
}

void Extractor::setMaxBufferAllowedSize(std::size_t maxBufferAllowedSize)
{
    if(_maxBufferAllowedSize != maxBufferAllowedSize)
    {
        _maxBufferAllowedSize = maxBufferAllowedSize;
    }
}

}
