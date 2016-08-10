
// validate_jsonfile_with_schemafile(system.file("extdata", "item-3.json", package = "JSONValidate"), system.file("extdata", "schema.json", package = "JSONValidate"))
// validate_jsonfile_with_schemafile(system.file("extdata", "malformed-json.json", package = "JSONValidate"), system.file("extdata", "schema.json", package = "JSONValidate"))
// validate_jsonfile_with_schemafile(system.file("extdata", "item-3.json", package = "JSONValidate"), system.file("extdata", "schema.json", package = "JSONValidate"))
// validate_jsonfile_with_schemafile(system.file("extdata", "item-3.json", package = "JSONValidate"),system.file("extdata", "broken-schema.json", package = "JSONValidate"))
// validate_json_with_schemafile("{\"category\": \"book\", \"price\": 25,  \"title\": \"abrakadabra\"}", system.file("extdata", "schema.json", package = "JSONValidate"))
// validate_json_with_schemafile("{\"category\": \"book\", \"price\": 25,  \"title\": \"abrakadabra\"}", system.file("extdata", "broken-schema.json", package = "JSONValidate"))

#include "rapidjson/error/en.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <sys/stat.h>

#include <Rcpp.h>
using namespace Rcpp;

enum JSONValidateErrorCodes {
  eOK = 0,                // No error.
  eMalformedJSON = 100,   // Malformed JSON
  eInvalidJSON = 200      //
};

List build_return(int returnval, std::string return_ms, std::string schemafn, std::string jsonfn) {
    CharacterVector msg = CharacterVector::create( return_ms )  ;
    CharacterVector schfn = CharacterVector::create( schemafn )  ;
    CharacterVector jsfn = CharacterVector::create( jsonfn )  ;
    NumericVector retv   = NumericVector::create( returnval ) ;
    List z            = List::create(
      Named("value") = retv, 
      Named("message") = msg, 
      Named("schema") = schfn, 
      Named("jsonfile") = jsfn ) ;
    
    return z;
}

std::string build_schema_parse_error_msg(std::string schemafn, rapidjson::Document& d) {
  std::string msg ("Schema file '");
  msg += schemafn;
  msg += "' is not well-formed JSON\n";
  msg += " Error(offset ";
  
  char buffer [50];
  sprintf (buffer, "%d", static_cast<unsigned>(d.GetErrorOffset()));
  msg += buffer;
  // bug in g++: msg += std::to_string(static_cast<unsigned>(d.GetErrorOffset()));
  msg += "):";
  msg += GetParseError_En(d.GetParseError());
  return msg;
}

std::string build_reader_parse_error_msg(rapidjson::Reader& reader) {
  std::string msg ("JSON Input is not well-formed JSON. Error(offset ");
  char buffer [50];
  sprintf (buffer, "%d", static_cast<unsigned>(reader.GetErrorOffset()));
  msg += buffer;
  // bug in g++: msg += std::to_string(static_cast<unsigned>(reader.GetErrorOffset()));
  msg += "): ";
  msg += GetParseError_En(reader.GetParseErrorCode());
  return msg;
}

std::string build_invalid_error_msg(rapidjson::SchemaValidator& validator) {
  rapidjson::StringBuffer sb;
  validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
  std::string msg ("Invalid schema point: ");
  msg.append(sb.GetString());
  msg.append("Invalid keyword: ");
  msg.append(validator.GetInvalidSchemaKeyword());
  sb.Clear();
  validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
  msg.append (" Invalid document point: ");
  msg.append(sb.GetString());
  return msg;
}

//' Validate a JSON file against a JSON Schema file
//' 
//' Description
//' @param jsonfn The JSON file 
//' @param schemafn The JSON Schema file
//' This function will check that the supplied files are not empty and are accessible
//' and it will then validate that the JSON file is valid (and well formed) against the
//' supplied schema file. Currently only version 4 of the JSON schema is supported.
//' @examples
//' \dontrun{
//' validate_jsonfile_with_schemafile("data/item-3.json", "data/schema.json")
//' }
//' @export
//' @useDynLib JSONValidate
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
List validate_jsonfile_with_schemafile(std::string jsonfn, std::string schemafn) {
    const int READ_BUF_SIZE = 4096; // add an extra 500 bytes to buffer sizes...
  
    if(jsonfn.empty()) stop("JSON file is not set");
    if(schemafn.empty()) stop("JSON Schema file is not set");
    
    struct stat json_stat_buffer;   
    bool json_is_accessable = stat (jsonfn.c_str(), &json_stat_buffer) == 0;
    std::string jsonerrmsg ("Cannot access JSON file:");
    jsonerrmsg.append(jsonfn);
    if(!json_is_accessable) stop(jsonerrmsg.c_str());

    struct stat schema_stat_buffer;   
    bool schema_is_accessable = stat (schemafn.c_str(), &schema_stat_buffer) == 0;
    std::string schemaerrmsg ("Cannot access JSON Schema file:");
    schemaerrmsg.append(schemafn);
    if(!schema_is_accessable) stop(schemaerrmsg.c_str());

    rapidjson::Document d;
    //Rprintf("Using schema file: %s\n", schemafn.c_str());
    //Rprintf("Using json file: %s\n",   jsonfn.c_str());

    {
        FILE *fp = fopen(schemafn.c_str(), "r");
        if (!fp) {
            REprintf("Schema file '%s' not found\n", schemafn.c_str());
            std::string msg ("Schema file '%s' not found\n");
            msg.append(schemafn);
            return build_return(EXIT_FAILURE, msg, schemafn, jsonfn );
        }
        //char buffer[schema_stat_buffer.st_size+BUF_EXTRA];
        char buffer[READ_BUF_SIZE];
        rapidjson::FileReadStream fs(fp, buffer, sizeof(buffer));
        d.ParseStream(fs);
        if (d.HasParseError()) {
          fclose(fp);
          std::string msg = build_schema_parse_error_msg(schemafn, d);
          return build_return(eMalformedJSON, msg, schemafn, jsonfn);
        }
        fclose(fp);
    }
    
    rapidjson::SchemaDocument sd(d);
    rapidjson::SchemaValidator validator(sd);
    rapidjson::Reader reader;
    
    FILE *jsonfp = fopen(jsonfn.c_str(), "r");
    char jsonbuffer[READ_BUF_SIZE];
    
    rapidjson::FileReadStream is(jsonfp, jsonbuffer, sizeof(jsonbuffer));
    if (!reader.Parse(is, validator) && reader.GetParseErrorCode() != rapidjson::kParseErrorTermination) {
        std::string msg = build_reader_parse_error_msg(reader);
        return build_return(eMalformedJSON, msg, schemafn, jsonfn);
    }
    
    if (validator.IsValid()) {
        return build_return(EXIT_SUCCESS, "JSON file is valid with respect to Schema", schemafn, jsonfn);
    }
    else {
        std::string msg = build_invalid_error_msg(validator);
        return build_return(eInvalidJSON, msg, schemafn, jsonfn);
    }
}


//' Validate a JSON string against a JSON Schema file
//' 
//' Description
//' @param json_string The JSON code 
//' @param schemafn The JSON Schema file
//' This function will check that the supplied schema file is not empty and is accessible
//' and it will then validate that the JSON code is valid (and well formed) against the
//' supplied schema file.
//' @examples
//' \dontrun{
//' json_code <- "{\"category\": \"book\", \"price\": 25,  \"title\": \"abrakadabra\"}"
//' validate_json_with_schemafile(json_code, "data/schema.json")
//' }
//' @export
//' @useDynLib JSONValidate
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
List validate_json_with_schemafile(std::string json_string, std::string schemafn) {
    const int READ_BUF_SIZE = 4096; // add an extra 500 bytes to buffer sizes...
  
    if(json_string.empty()) stop("JSON is empty");
    if(schemafn.empty()) stop("JSON Schema file is not set");
    
    struct stat schema_stat_buffer;   
    bool schema_is_accessable = stat (schemafn.c_str(), &schema_stat_buffer) == 0;
    if(!schema_is_accessable) stop("Cannot access JSON Schema file");

    rapidjson::Document d;
    //Rprintf("Using schema file: %s\n", schemafn.c_str());
    //Rprintf("Using supplied json string.\n");

    {
        FILE *fp = fopen(schemafn.c_str(), "r");
        if (!fp) {
            REprintf("Schema file '%s' not found\n", schemafn.c_str());
            std::string msg ("Schema file '%s' not found\n");
            msg.append(schemafn);
            return build_return(EXIT_FAILURE, msg, schemafn, json_string );
            //return -1;
        }
        //char buffer[schema_stat_buffer.st_size+BUF_EXTRA];
        char buffer[READ_BUF_SIZE];
        rapidjson::FileReadStream fs(fp, buffer, sizeof(buffer));
        d.ParseStream(fs);
        if (d.HasParseError()) {
            fclose(fp);
            std::string msg = build_schema_parse_error_msg(schemafn, d);
            return build_return(eMalformedJSON, msg, schemafn, json_string);
            //return EXIT_FAILURE;
        }
        fclose(fp);
    }
    
    rapidjson::SchemaDocument sd(d);
    rapidjson::SchemaValidator validator(sd);
    rapidjson::Reader reader;
        
    rapidjson::StringStream is(json_string.c_str());
    if (!reader.Parse(is, validator) && reader.GetParseErrorCode() != rapidjson::kParseErrorTermination) {
        std::string msg = build_reader_parse_error_msg(reader);
        return build_return(eMalformedJSON, msg, schemafn, json_string);
    }
    
    if (validator.IsValid()) {
        return build_return(EXIT_SUCCESS, "JSON string is valid with respect to Schema", schemafn, json_string);
    }
    else {
        std::string msg = build_invalid_error_msg(validator);
        return build_return(eInvalidJSON, msg, schemafn, json_string);
    }
}

//' Validate a JSON file against a JSON Schema string
//' 
//' Description
//' @param jsonfn The JSON file 
//' @param schema_string The JSON Schema string
//' This function will check that the supplied schema is not empty and it will then validate
//' that the JSON file is accessible and valid (and well formed) w.r.t the supplied schema file.
//' @examples
//' \dontrun{
//' json_code <- "{\"category\": \"book\", \"price\": 25,  \"title\": \"abrakadabra\"}"
//' validate_json_with_schemafile(json_code, "data/schema.json")
//' }
//' @export
//' @useDynLib JSONValidate
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
List validate_jsonfile_with_schema(std::string jsonfn, std::string schema_string) {
  
  const int READ_BUF_SIZE = 4096; // add an extra 500 bytes to buffer sizes...
  
  if(jsonfn.empty()) stop("JSON file is not set");
  if(schema_string.empty()) stop("JSON Schema is empty");
  
  struct stat json_stat_buffer;   
  bool json_is_accessable = stat (jsonfn.c_str(), &json_stat_buffer) == 0;
  std::string jsonerrmsg ("Cannot access JSON file:");
  jsonerrmsg.append(jsonfn);
  if(!json_is_accessable) stop(jsonerrmsg.c_str());
  
  rapidjson::Document d;
  rapidjson::StringStream fs(schema_string.c_str());
  d.ParseStream(fs);
  if (d.HasParseError()) {
    std::string msg = build_schema_parse_error_msg(schema_string, d);
    return build_return(eMalformedJSON, msg, schema_string, jsonfn);
  }
  
  rapidjson::SchemaDocument sd(d);
  rapidjson::SchemaValidator validator(sd);
  rapidjson::Reader reader;
  
  FILE *jsonfp = fopen(jsonfn.c_str(), "r");
  char jsonbuffer[READ_BUF_SIZE];
  
  rapidjson::FileReadStream is(jsonfp, jsonbuffer, sizeof(jsonbuffer));
  if (!reader.Parse(is, validator) && reader.GetParseErrorCode() != rapidjson::kParseErrorTermination) {
    std::string msg = build_reader_parse_error_msg(reader);
    return build_return(eMalformedJSON, msg, schema_string, jsonfn);
  }
  
  if (validator.IsValid()) {
    return build_return(EXIT_SUCCESS, "JSON string is valid with respect to Schema", schema_string, jsonfn);
  }
  else {
    std::string msg = build_invalid_error_msg(validator);
    return build_return(eInvalidJSON, msg, schema_string, jsonfn);
  }
}

//' Validate a JSON string against a JSON Schema string
//' 
//' Description
//' @param json_string The JSON code 
//' @param schema_string The JSON Schema string
//' This function will check that the supplied schema is not empty and it will then validate 
//' that the JSON code is valid (and well formed) w.r.t the supplied schema.
//' @export
//' @useDynLib JSONValidate
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
List validate_json_with_schema(std::string json_string, std::string schema_string) {

  if(json_string.empty()) stop("JSON is empty");
  if(schema_string.empty()) stop("JSON Schema is empty");
  
  rapidjson::Document d;
  rapidjson::StringStream fs(schema_string.c_str());
  d.ParseStream(fs);
  if (d.HasParseError()) {
    std::string msg = build_schema_parse_error_msg(schema_string, d);
    return build_return(eMalformedJSON, msg, schema_string, json_string);
  }
  
  rapidjson::SchemaDocument sd(d);
  rapidjson::SchemaValidator validator(sd);
  rapidjson::Reader reader;
  
  rapidjson::StringStream is(json_string.c_str());
  if (!reader.Parse(is, validator) && reader.GetParseErrorCode() != rapidjson::kParseErrorTermination) {
    std::string msg = build_reader_parse_error_msg(reader);
    return build_return(eMalformedJSON, msg, schema_string, json_string);
  }
  
  if (validator.IsValid()) {
    return build_return(EXIT_SUCCESS, "JSON string is valid with respect to Schema", schema_string, json_string);
  }
  else {
    std::string msg = build_invalid_error_msg(validator);
    return build_return(eInvalidJSON, msg, schema_string, json_string);
  }
}

