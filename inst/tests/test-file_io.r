library(testthat)

context("Validate JSON file IO")

test_that("validate valid", {  
  expect_that(validate_jsonfile_with_schemafile("data/item-3.json", "data/schema.json")$value, equals(0))
  expect_that(validate_jsonfile_with_schemafile("data/item-3.json", "data/schema.json")$schema, equals("data/schema.json"))
  expect_that(validate_jsonfile_with_schemafile("data/item-3.json", "data/schema.json")$jsonfile, equals("data/item-3.json"))
})

test_that("validate broken", {  
  expect_that(validate_jsonfile_with_schemafile("data/item-1.json", "data/schema.json"), equals(list(value=1,message="Input is not a valid JSON. Error(offset 14): Invalid value.", schema="data/schema.json", jsonfile="data/item-1.json")))
  expect_that(validate_jsonfile_with_schemafile("data/item-2.json", "data/schema.json"), equals(list(value=1,message="Invalid schema: #Invalid keyword: required Invalid document: #", schema="data/schema.json", jsonfile="data/item-2.json")))
})