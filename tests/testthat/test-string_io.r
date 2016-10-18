testthat::context("Validate JSON string-file IO")

validjson     <- system.file("extdata", "item-3.json", package = "validatejsonr")
schemafile      <- system.file("extdata", "schema.json", package = "validatejsonr")
invalidjsonfn1  <- system.file("extdata", "item-1.json", package = "validatejsonr")
invalidjsonfn2  <- system.file("extdata", "item-2.json", package = "validatejsonr")
malformedjsonfn <- system.file("extdata", "malformed-json.json", package = "validatejsonr")
schema_code     <- readChar(schemafile, file.info(schemafile)$size)

testthat::test_that("validate valid", {  
  json_code <- "{\"category\": \"book\", \"price\": 25,  \"title\": \"abrakadabra\"}"
  validate_result <- validate_json_with_schemafile(json_code, schemafile)
  testthat::expect_that(validate_result$value, equals(0))
  testthat::expect_that(basename(validate_result$schema), equals("schema.json"))
  testthat::expect_that(basename(validate_result$jsonfile), equals(json_code))
  
  validate_result <- validate_jsonfile_with_schema(validjson, schema_code)
  testthat::expect_that(validate_result$value, equals(0))
  testthat::expect_that(validate_result$schema, equals(schema_code))
  testthat::expect_that(validate_result$jsonfile, equals(validjson))
})

testthat::test_that("validate malformed", {  
  invalidjson1 <- readChar(invalidjsonfn1, file.info(invalidjsonfn1)$size)
  malformedjson <- readChar(malformedjsonfn, file.info(malformedjsonfn)$size)
  
  # JSON string Schema file
  testthat::expect_that(validate_json_with_schemafile(invalidjson1, schemafile), equals(list(value=100,message="JSON Input is not well-formed JSON. Error(offset 14): Invalid value.", schema=schemafile, jsonfile=invalidjson1)))
  testthat::expect_that(validate_json_with_schemafile(malformedjson, schemafile)$message, equals("JSON Input is not well-formed JSON. Error(offset 12): Missing a colon after a name of object member."))
  
  # JSON file Schema string
  testthat::expect_that(validate_jsonfile_with_schema(invalidjsonfn1, schema_code), equals(list(value=100,message="JSON Input is not well-formed JSON. Error(offset 14): Invalid value.", schema=schema_code, jsonfile=invalidjsonfn1)))
  testthat::expect_that(validate_jsonfile_with_schema(malformedjsonfn, schema_code)$message, equals("JSON Input is not well-formed JSON. Error(offset 12): Missing a colon after a name of object member."))
})

testthat::test_that("validate invalid", {  
  invalidjson2 <- readChar(invalidjsonfn2, file.info(invalidjsonfn2)$size)
  testthat::expect_that(validate_json_with_schemafile(invalidjson2, schemafile), equals(list(value=200,message="Invalid schema point: #Invalid keyword: required Invalid document point: #", schema=schemafile, jsonfile=invalidjson2)))
})