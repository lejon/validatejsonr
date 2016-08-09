library(testthat)

context("Validate JSON string IO")

schemafile <- system.file("extdata", "schema.json", package = "JSONValidate")
invalidjsonfn1 <- system.file("extdata", "item-1.json", package = "JSONValidate")
invalidjsonfn2 <- system.file("extdata", "item-2.json", package = "JSONValidate")

test_that("validate valid", {  
  json_code <- "{\"category\": \"book\", \"price\": 25,  \"title\": \"abrakadabra\"}"
  validate_result <- validate_json_with_schemafile(json_code, schemafile)
  expect_that(validate_result$value, equals(0))
  expect_that(basename(validate_result$schema), equals("schema.json"))
  expect_that(basename(validate_result$jsonfile), equals(json_code))
})

test_that("validate broken", {  
  invalidjson1 <- readChar(invalidjsonfn1, file.info(invalidjsonfn1)$size)
  invalidjson2 <- readChar(invalidjsonfn2, file.info(invalidjsonfn2)$size)
  expect_that(validate_json_with_schemafile(invalidjson1, schemafile), equals(list(value=1,message="Input is not valid JSON. Error(offset 14): Invalid value.", schema=schemafile, jsonfile=invalidjson1)))
  expect_that(validate_json_with_schemafile(invalidjson2, schemafile), equals(list(value=1,message="Invalid schema: #Invalid keyword: required Invalid document: #", schema=schemafile, jsonfile=invalidjson2)))
})