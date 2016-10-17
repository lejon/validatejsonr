[![Build Status](https://travis-ci.org/lejon/JSONValidate.svg?branch=master)](https://travis-ci.org/lejon/JSONValidate)

# validatejsonr
An R package for validation of JSON against JSON Schema. There are functions for using JSON file with JSON Schema file,
a JSON string with a JSON Schema file and vice-versa, and finally a JSON string with a JSON Schema.

validatejsonr is based on the C++ library RapidJSON (https://github.com/miloyip/rapidjson), which currently supports 
JSON Schema Draft version 4. RapidJSON pass 262 out of 263 tests in JSON Schema Test Suite (Json Schema draft 4).

## Example Usage:

Validate a valid JSON file w.r.t Schema

```
library(JSONValidate)

  > validjson <- "inst/extdata/item-3.json"
  > schemafile <- "inst/extdata/schema.json"
  > validate_result <- validate_jsonfile_with_schemafile(validjson, schemafile)

  > validate_result$value
  [1] 0
  # 0 => success
  
  > validate_result$message
  [1] "JSON file is valid with respect to Schema"
  
  > validate_result$schema
  [1] "inst/extdata/schema.json"
  
  > validate_result$jsonfile
  [1] "inst/extdata/item-3.json"
```

Validate a non-well formed JSON file w.r.t Schema

```
library(JSONValidate)

  > validjson <- "inst/extdata/item-1.json"
  > schemafile <- "inst/extdata/schema.json"
  > validate_result <- validate_jsonfile_with_schemafile(validjson, schemafile)

  > validate_result$value
  [1] 100
  # 1XX => non-well formed JSON (syntactically incorrect)
  
  > validate_result$message
  [1] "JSON Input is not well-formed JSON. Error(offset 14): Invalid value."
  
  > validate_result$schema
  [1] "inst/extdata/schema.json"
  
  > validate_result$jsonfile
  [1] "inst/extdata/item-1.json"
```

Validate an in-valid JSON file w.r.t Schema

```
library(JSONValidate)

  > validjson <- "inst/extdata/item-1.json"
  > schemafile <- "inst/extdata/schema.json"
  > validate_result <- validate_jsonfile_with_schemafile(validjson, schemafile)

  > validate_result$value
  [1] 200
  # 2XX => invalid JSON with respect to Schema
  
  > validate_result$message
  [1] "Invalid schema point: #Invalid keyword: required Invalid document point: #"
  
  > validate_result$schema
  [1] "inst/extdata/schema.json"
  
  > validate_result$jsonfile
  [1] "inst/extdata/item-2.json"
```

## LICENSE

validatejsonr

Copyright (c) 2016, Leif Jonsson

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### Other External licenses

RapidJSON is released under the Terms of the MIT License:
---------------------------------------------------------

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Open Source Software Licensed Under the BSD License:
----------------------------------------------------

The msinttypes r29 
Copyright (c) 2006-2013 Alexander Chemeris 
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of  copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

