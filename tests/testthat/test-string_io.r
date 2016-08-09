library(testthat)

context("Validate JSON string-file IO")

schemafile     <- system.file("extdata", "schema.json", package = "JSONValidate")
invalidjsonfn1 <- system.file("extdata", "item-1.json", package = "JSONValidate")
invalidjsonfn2 <- system.file("extdata", "item-2.json", package = "JSONValidate")
malformedjsonfn  <- system.file("extdata", "malformed-json.json", package = "JSONValidate")

test_that("validate valid", {  
  json_code <- "{\"category\": \"book\", \"price\": 25,  \"title\": \"abrakadabra\"}"
  validate_result <- validate_json_with_schemafile(json_code, schemafile)
  expect_that(validate_result$value, equals(0))
  expect_that(basename(validate_result$schema), equals("schema.json"))
  expect_that(basename(validate_result$jsonfile), equals(json_code))
})

test_that("validate malformed", {  
  invalidjson1 <- readChar(invalidjsonfn1, file.info(invalidjsonfn1)$size)
  malformedjson <- readChar(malformedjsonfn, file.info(malformedjsonfn)$size)
  expect_that(validate_json_with_schemafile(invalidjson1, schemafile), equals(list(value=100,message="JSON Input is not well-formed JSON. Error(offset 14): Invalid value.", schema=schemafile, jsonfile=invalidjson1)))
  expect_that(validate_json_with_schemafile(malformedjson, schemafile)$message, equals("JSON Input is not well-formed JSON. Error(offset 12): Missing a colon after a name of object member."))
})

test_that("validate invalid", {  
  invalidjson2 <- readChar(invalidjsonfn2, file.info(invalidjsonfn2)$size)
  expect_that(validate_json_with_schemafile(invalidjson2, schemafile), equals(list(value=200,message="Invalid schema point: #Invalid keyword: required Invalid document point: #", schema=schemafile, jsonfile=invalidjson2)))
})