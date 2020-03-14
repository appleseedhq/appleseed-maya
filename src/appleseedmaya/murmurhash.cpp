
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2019 Esteban Tovagliari, The appleseedhq Organization
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// Interface header.
#include "murmurhash.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/utility.h"

// appleseed.foundation headers.
#include "foundation/containers/dictionary.h"

namespace asf = foundation;
namespace asr = renderer;

uint64_t rotl64(uint64_t x, int8_t r)
{
    return (x << r) | (x >> (64 - r));
}

uint64_t fmix(uint64_t k)
{
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccd;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53;
    k ^= k >> 33;

    return k;
}

MurmurHash::MurmurHash()
  : m_h1(0)
  , m_h2(0)
{
}

MurmurHash::MurmurHash(const MurmurHash& other)
  : m_h1(other.m_h1)
  , m_h2(other.m_h2)
{
}

const MurmurHash& MurmurHash::operator=(const MurmurHash& other)
{
    m_h1 = other.m_h1;
    m_h2 = other.m_h2;
    return *this;
}

void MurmurHash::append(const void* data, size_t bytes)
{
    const int nBlocks = static_cast<int>(bytes) / 16;

    const uint64_t c1 = 0x87c37b91114253d5;
    const uint64_t c2 = 0x4cf5ad432745937f;

    // local copies of m_h1, and m_h2. we'll work
    // with these before copying back at the end.
    // this gives the optimiser more freedom to do
    // its thing.
    uint64_t h1 = m_h1;
    uint64_t h2 = m_h2;

    // body

    const uint64_t* blocks = (const uint64_t *)data;
    for (int i = 0; i < nBlocks; i++)
    {
        uint64_t k1 = blocks[i*2];
        uint64_t k2 = blocks[i*2+1];

        k1 *= c1; k1  = rotl64(k1, 31); k1 *= c2; h1 ^= k1;

        h1 = rotl64(h1, 27); h1 += h2; h1 = h1*5 + 0x52dce729;

        k2 *= c2; k2  = rotl64(k2, 33); k2 *= c1; h2 ^= k2;

        h2 = rotl64(h2, 31); h2 += h1; h2 = h2*5 + 0x38495ab5;
    }

    // tail

    const uint8_t * tail = ((const uint8_t*)data) + nBlocks*16;

    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch(bytes & 15)
    {
    case 15: k2 ^= uint64_t(tail[14]) << 48;
    case 14: k2 ^= uint64_t(tail[13]) << 40;
    case 13: k2 ^= uint64_t(tail[12]) << 32;
    case 12: k2 ^= uint64_t(tail[11]) << 24;
    case 11: k2 ^= uint64_t(tail[10]) << 16;
    case 10: k2 ^= uint64_t(tail[ 9]) << 8;
    case  9: k2 ^= uint64_t(tail[ 8]) << 0;
           k2 *= c2; k2  = rotl64(k2,33); k2 *= c1; h2 ^= k2;

    case  8: k1 ^= uint64_t(tail[ 7]) << 56;
    case  7: k1 ^= uint64_t(tail[ 6]) << 48;
    case  6: k1 ^= uint64_t(tail[ 5]) << 40;
    case  5: k1 ^= uint64_t(tail[ 4]) << 32;
    case  4: k1 ^= uint64_t(tail[ 3]) << 24;
    case  3: k1 ^= uint64_t(tail[ 2]) << 16;
    case  2: k1 ^= uint64_t(tail[ 1]) << 8;
    case  1: k1 ^= uint64_t(tail[ 0]) << 0;
           k1 *= c1; k1  = rotl64(k1,31); k1 *= c2; h1 ^= k1;
    };

    // finalisation

    h1 ^= bytes; h2 ^= bytes;

    h1 += h2;
    h2 += h1;

    h1 = fmix(h1);
    h2 = fmix(h2);

    h1 += h2;
    h2 += h1;

    m_h1 = h1;
    m_h2 = h2;
}

bool MurmurHash::operator==(const MurmurHash& other) const
{
    return m_h1 == other.m_h1 && m_h2 == other.m_h2;
}

bool MurmurHash::operator!=(const MurmurHash& other) const
{
    return m_h1 != other.m_h1 || m_h2 != other.m_h2;
}

bool MurmurHash::operator<(const MurmurHash& other) const
{
    return m_h1 < other.m_h1 ||(m_h1 == other.m_h1 && m_h2 < other.m_h2);
}

std::string MurmurHash::toString() const
{
    std::stringstream s;
    s << std::hex << std::setfill('0')
      << std::setw(16) << m_h1
      << std::setw(16) << m_h2;
    return s.str();
}

void MurmurHash::append(const asf::StringDictionary& dictionary)
{
    for (auto it = dictionary.begin(), e = dictionary.end(); it != e; ++it)
    {
        append(it.key());
        append(it.value());
    }
}

void MurmurHash::append(const asf::Dictionary& dictionary)
{
    append(dictionary.strings());

    for (auto it = dictionary.dictionaries().begin(), e = dictionary.dictionaries().end(); it != e; ++it)
    {
        append(it.key());
        append(it.value());
    }
}

void MurmurHash::append(const asr::ParamArray& params)
{
    return append(static_cast<const asf::Dictionary&>(params));
}

std::ostream& operator<<(std::ostream& o, const MurmurHash& hash)
{
    o << hash.toString();
    return o;
}
