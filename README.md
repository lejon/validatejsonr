# JSONValidate
An R package for validation of JSON against JSON Schema. There are functions for using JSON file with JSON Schema file,
a JSON string with a JSON Schema file and vice-versa, and finally a JSON string with a JSON Schema.

JSON Validare is based on the C++ library RapidJSON (https://github.com/miloyip/rapidjson), which currently supports 
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