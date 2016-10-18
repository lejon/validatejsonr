testthat::context("Validate JSON file-file IO")

validjson     <- system.file("extdata", "item-3.json", package = "validatejsonr")
malformedjson <- system.file("extdata", "malformed-json.json", package = "validatejsonr")
invalidjson1  <- system.file("extdata", "item-1.json", package = "validatejsonr")
invalidjson2  <- system.file("extdata", "item-2.json", package = "validatejsonr")
schemafile    <- system.file("extdata", "schema.json", package = "validatejsonr")

testthat::test_that("validate valid", {  
  testthat::expect_that(validate_jsonfile_with_schemafile(validjson, schemafile)$value, equals(0))
  testthat::expect_that(validate_jsonfile_with_schemafile(validjson, schemafile)$schema, equals(schemafile))
  testthat::expect_that(validate_jsonfile_with_schemafile(validjson, schemafile)$jsonfile, equals(validjson))
})

testthat::test_that("validate malformed", {  
  testthat::expect_that(validate_jsonfile_with_schemafile(invalidjson1, schemafile), equals(list(value=100,message="JSON Input is not well-formed JSON. Error(offset 14): Invalid value.", schema=schemafile, jsonfile=invalidjson1)))
  testthat::expect_that(validate_jsonfile_with_schemafile(malformedjson, schemafile), equals(list(value=100,message="JSON Input is not well-formed JSON. Error(offset 12): Missing a colon after a name of object member.", schema=schemafile, jsonfile=malformedjson)))
})

testthat::test_that("validate invalid", {  
  testthat::expect_that(validate_jsonfile_with_schemafile(invalidjson2, schemafile), equals(list(value=200,message="Invalid schema point: #Invalid keyword: required Invalid document point: #", schema=schemafile, jsonfile=invalidjson2)))
})

testthat::test_that("validate missing", {  
  testthat::expect_error(validate_jsonfile_with_schemafile("missing", schemafile), "Cannot access JSON file*")
  testthat::expect_error(validate_jsonfile_with_schemafile(validjson, "missing"), "Cannot access JSON Schema*")
})