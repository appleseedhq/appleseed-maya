
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2018 Esteban Tovagliari, The appleseedhq Organization
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

#ifndef APPLESEED_MAYA_MURMURHASH_H
#define APPLESEED_MAYA_MURMURHASH_H

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MString.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

// Forward declarations.
namespace foundation { class StringDictionary; }
namespace foundation { class Dictionary; }
namespace renderer   { class ParamArray; }

//
// MurmurHash.
//

// A nice class for hashing arbitrary chunks of data, based on
// code available at http://code.google.com/p/smhasher.
//
// From that page :
//
// "All MurmurHash versions are public domain software, and the
// author disclaims all copyright to their code."
//

class MurmurHash
{
  public:
    MurmurHash();
    MurmurHash(const MurmurHash& other);

    const MurmurHash &operator=(const MurmurHash& other);

    bool operator==(const MurmurHash& other) const;
    bool operator!=(const MurmurHash& other) const;
    bool operator<(const MurmurHash& other) const;

    std::string toString() const;

    template <typename T>
    void append(const T& x)
    {
        append(&x, sizeof(T));
    }

    void append(const char* str)
    {
        append(str, strlen(str));
    }

    void append(const std::string& str)
    {
        append(str.c_str(), str.size());
    }

    void append(const MString& str)
    {
        append(str.asChar(), str.length());
    }

    void append(const foundation::StringDictionary& dictionary);

    void append(const foundation::Dictionary& dictionary);

    void append(const renderer::ParamArray& params);

  private:
    void append(const void* data, size_t bytes);

    uint64_t m_h1;
    uint64_t m_h2;
};

std::ostream& operator<<(std::ostream& o, const MurmurHash& hash);

#endif  // !APPLESEED_MAYA_MURMURHASH_H
