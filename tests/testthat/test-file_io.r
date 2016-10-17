library(testthat)

context("Validate JSON file-file IO")

validjson     <- system.file("extdata", "item-3.json", package = "validatejsonr")
malformedjson <- system.file("extdata", "malformed-json.json", package = "validatejsonr")
invalidjson1  <- system.file("extdata", "item-1.json", package = "validatejsonr")
invalidjson2  <- system.file("extdata", "item-2.json", package = "validatejsonr")
schemafile    <- system.file("extdata", "schema.json", package = "validatejsonr")

test_that("validate valid", {  
  expect_that(validate_jsonfile_with_schemafile(validjson, schemafile)$value, equals(0))
  expect_that(validate_jsonfile_with_schemafile(validjson, schemafile)$schema, equals(schemafile))
  expect_that(validate_jsonfile_with_schemafile(validjson, schemafile)$jsonfile, equals(validjson))
})

test_that("validate malformed", {  
  expect_that(validate_jsonfile_with_schemafile(invalidjson1, schemafile), equals(list(value=100,message="JSON Input is not well-formed JSON. Error(offset 14): Invalid value.", schema=schemafile, jsonfile=invalidjson1)))
  expect_that(validate_jsonfile_with_schemafile(malformedjson, schemafile), equals(list(value=100,message="JSON Input is not well-formed JSON. Error(offset 12): Missing a colon after a name of object member.", schema=schemafile, jsonfile=malformedjson)))
})

test_that("validate invalid", {  
  expect_that(validate_jsonfile_with_schemafile(invalidjson2, schemafile), equals(list(value=200,message="Invalid schema point: #Invalid keyword: required Invalid document point: #", schema=schemafile, jsonfile=invalidjson2)))
})

test_that("validate missing", {  
  expect_error(validate_jsonfile_with_schemafile("missing", schemafile), "Cannot access JSON file*")
  expect_error(validate_jsonfile_with_schemafile(validjson, "missing"), "Cannot access JSON Schema*")
})