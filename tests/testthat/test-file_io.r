library(testthat)

context("Validate JSON file IO")

validjson <- system.file("extdata", "item-3.json", package = "JSONValidate")
invalidjson1 <- system.file("extdata", "item-1.json", package = "JSONValidate")
invalidjson2 <- system.file("extdata", "item-2.json", package = "JSONValidate")
schemafile <- system.file("extdata", "schema.json", package = "JSONValidate")

test_that("validate valid", {  
  expect_that(validate_jsonfile_with_schemafile(validjson, schemafile)$value, equals(0))
  expect_that(validate_jsonfile_with_schemafile(validjson, schemafile)$schema, equals(schemafile))
  expect_that(validate_jsonfile_with_schemafile(validjson, schemafile)$jsonfile, equals(validjson))
})

test_that("validate broken", {  
  expect_that(validate_jsonfile_with_schemafile(invalidjson1, schemafile), equals(list(value=1,message="Input is not valid JSON. Error(offset 14): Invalid value.", schema=schemafile, jsonfile=invalidjson1)))
  expect_that(validate_jsonfile_with_schemafile(invalidjson2, schemafile), equals(list(value=1,message="Invalid schema: #Invalid keyword: required Invalid document: #", schema=schemafile, jsonfile=invalidjson2)))
})