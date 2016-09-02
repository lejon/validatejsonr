\name{JSONValidate-package}
\alias{JSONValidate-package}
\alias{JSONValidate}
\docType{package}
\title{
  This package supplies functions to validate JSON against JSON Schemas
}
\description{
  The current implementation uses the c++ library 'rapidjson' to supply the schema functionality, it 
  supports JSON Schema Draft v4.
  
  2016-09-09: RapidJSON passed 262 out of 263 tests in JSON Schema Test Suite (Json Schema draft 4). 
}
\details{
  There are four main combinations of the functionality available. This is made up of combinations of supplying the JSON code in a file or string and supplying the corresponding schema as a file or as a string:
  
    JSON and Schema in a file:
      validate_jsonfile_with_schemafile

    JSON as string and Schema in a file:
      validate_json_with_schemafile

    JSON as file and Schema as a string:
      validate_jsonfile_with_schema

    JSON and Schema as a string:
      validate_json_with_schema
}
\author{
Leif Jonsson (leif.jonsson@gmail.com)

Maintainer: Leif Jonsson <leif.jonsson@gmail.com>
}
\references{
  RapidJSON can be found at: https://github.com/miloyip/rapidjson
}
\keyword{ package }
\seealso{
  Optional links to other man pages
}
\examples{
  \dontrun{
     validate_jsonfile_with_schemafile("data/item-3.json", "data/schema.json")
  }
}
