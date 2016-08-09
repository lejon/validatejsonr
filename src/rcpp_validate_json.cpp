
// validate_jsonfile_with_schemafile("data/item-1.json", "data/schema.json")
// validate_jsonfile_with_schemafile("data/item-3.json", "data/schema.json")
// validate_json_with_schemafile("data/item-3.json", "data/schema.json")

#include "rapidjson/error/en.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <sys/stat.h>

#include <Rcpp.h>
using namespace Rcpp;

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
  msg += "' is not valid JSON\n";
  msg += " Error(offset";
  msg += static_cast<unsigned>(d.GetErrorOffset());
  msg += "):";
  msg += GetParseError_En(d.GetParseError());
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
  
    //const int BUF_EXTRA = 500; // add an extra 500 bytes to buffer sizes...
    const int READ_BUF_SIZE = 4096; // add an extra 500 bytes to buffer sizes...
  
    if(jsonfn.empty()) stop("JSON file is not set");
    if(schemafn.empty()) stop("JSON Schmea file is not set");
    
    struct stat json_stat_buffer;   
    bool json_is_accessable = stat (jsonfn.c_str(), &json_stat_buffer) == 0;
    std::string jsonerrmsg ("Cannot access JSON file:");
    jsonerrmsg.append(jsonfn);
    if(!json_is_accessable) stop(jsonerrmsg.c_str());

    struct stat schema_stat_buffer;   
    bool schema_is_accessable = stat (schemafn.c_str(), &schema_stat_buffer) == 0;
    std::string schemaerrmsg ("Cannot access JSON Schmea file:");
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
            //return -1;
        }
        //char buffer[schema_stat_buffer.st_size+BUF_EXTRA];
        char buffer[READ_BUF_SIZE];
        rapidjson::FileReadStream fs(fp, buffer, sizeof(buffer));
        d.ParseStream(fs);
        if (d.HasParseError()) {
          fclose(fp);
          std::string msg = build_schema_parse_error_msg(schemafn, d);
          return build_return(EXIT_FAILURE, msg, schemafn, jsonfn);
          //return EXIT_FAILURE;
        }
        fclose(fp);
    }
    
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    d.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
        
    // Convert Document into SchemaDocument
    rapidjson::SchemaDocument sd(d);
    // Use reader to parse the JSON, and forward SAX events to validator
    rapidjson::SchemaValidator validator(sd);
    rapidjson::Reader reader;
    
    FILE *jsonfp = fopen(jsonfn.c_str(), "r");
    //char jsonbuffer[json_stat_buffer.st_size+BUF_EXTRA];
    char jsonbuffer[READ_BUF_SIZE];
    
    rapidjson::FileReadStream is(jsonfp, jsonbuffer, sizeof(jsonbuffer));
    if (!reader.Parse(is, validator) && reader.GetParseErrorCode() != rapidjson::kParseErrorTermination) {
        //REprintf("Parsing failed!\n");
        // Schema validator error would cause kParseErrorTermination, which will handle it in next step.
        std::string msg ("Input is not valid JSON. Error(offset ");
        msg.append(std::to_string(static_cast<unsigned>(reader.GetErrorOffset())));
        msg.append("): ");
        msg.append(GetParseError_En(reader.GetParseErrorCode()));
        return build_return(EXIT_FAILURE, msg, schemafn, jsonfn);
    }
    
    if (validator.IsValid()) {
        //Rprintf("Input JSON is valid.\n");
        return build_return(EXIT_SUCCESS, sb.GetString(), schemafn, jsonfn);
    }
    else {
        //REprintf("Input JSON is INvalid.\n");
        rapidjson::StringBuffer sb;
        validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        std::string msg ("Invalid schema: ");
        msg.append(sb.GetString());
        msg.append("Invalid keyword: ");
        msg.append(validator.GetInvalidSchemaKeyword());
        sb.Clear();
        validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
        msg.append (" Invalid document: ");
        msg.append(sb.GetString());
        //REprintf(msg.c_str());
        return build_return(EXIT_FAILURE, msg, schemafn, jsonfn);
    }
}


//' Validate a JSON string against a JSON Schema file
//' 
//' Description
//' @param jsonfn The JSON code 
//' @param schemafn The JSON Schema file
//' This function will check that the supplied schema file is not empty and is accessible
//' and it will then validate that the JSON code is valid (and well formed) against the
//' supplied schema file.
//' @examples
//' \dontrun{
//' validate_json_with_schemafile("data/item-3.json", "data/schema.json")
//' }
//' @export
//' @useDynLib JSONValidate
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
List validate_json_with_schemafile(std::string json_string, std::string schemafn) {
  
    //const int BUF_EXTRA = 500; // add an extra 500 bytes to buffer sizes...
    const int READ_BUF_SIZE = 4096; // add an extra 500 bytes to buffer sizes...
  
    if(json_string.empty()) stop("JSON is empty");
    if(schemafn.empty()) stop("JSON Schmea file is not set");
    
    struct stat schema_stat_buffer;   
    bool schema_is_accessable = stat (schemafn.c_str(), &schema_stat_buffer) == 0;
    if(!schema_is_accessable) stop("Cannot access JSON Schmea file");

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
            return build_return(EXIT_FAILURE, msg, schemafn, json_string);
            //return EXIT_FAILURE;
        }
        fclose(fp);
    }
    
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    d.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
        
    // Convert Document into SchemaDocument
    rapidjson::SchemaDocument sd(d);
    // Use reader to parse the JSON, and forward SAX events to validator
    rapidjson::SchemaValidator validator(sd);
    rapidjson::Reader reader;
        
    rapidjson::StringStream is(json_string.c_str());
    if (!reader.Parse(is, validator) && reader.GetParseErrorCode() != rapidjson::kParseErrorTermination) {
        //REprintf("Parsing failed!\n");
        // Schema validator error would cause kParseErrorTermination, which will handle it in next step.
        std::string msg ("Input is not valid JSON. Error(offset ");
        msg.append(std::to_string(static_cast<unsigned>(reader.GetErrorOffset())));
        msg.append("): ");
        msg.append(GetParseError_En(reader.GetParseErrorCode()));
        return build_return(EXIT_FAILURE, msg, schemafn, json_string);
    }
    
    if (validator.IsValid()) {
        return build_return(EXIT_SUCCESS, sb.GetString(), schemafn, json_string);
    }
    else {
        rapidjson::StringBuffer sb;
        validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        std::string msg ("Invalid schema: ");
        msg.append(sb.GetString());
        msg.append("Invalid keyword: ");
        msg.append(validator.GetInvalidSchemaKeyword());
        sb.Clear();
        validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
        msg.append (" Invalid document: ");
        msg.append(sb.GetString());
        return build_return(EXIT_FAILURE, msg, schemafn, json_string);
    }
}

