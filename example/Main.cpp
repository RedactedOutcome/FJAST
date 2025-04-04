#include "pch.h"

#include <filesystem>
#include <fstream>
#include <chrono>

#include "FJASTP.h"
#include "FJASTP/Tokenizer.h"
#include "FJASTP/ASTGenerator.h"

using namespace FJASTP;

int main(int argc, char** argv){
    std::cout<<"Hello World"<<std::endl;
    HBuffer test1;

    if(argc <= 1){
        std::cout << "Using test1.js"<<std::endl;
        std::cout << "Working dir " << argv[0]<<std::endl;
        std::ifstream file("res/test1.js", std::ios::binary | std::ios::ate);
        if(!file){
            std::cout << "Failed to open file res/test1.js"<<std::endl;
            return -1;
        }

        size_t len = file.tellg();
        file.seekg(std::ios::beg);
        std::cout << "File is " << len << " bytes"<<std::endl;
        
        char* data = new char[len];
        file.read(data, len);
        test1 = HBuffer(data, len, true, true);
    }else{
        test1 = argv[1];
    }

    std::cout << "Starting"<<std::endl;
    InitializationResult initResult = FJastP::Init();
    if(initResult != InitializationResult::Success){
        std::cout << "Failed to initialize FJASTP"<<std::endl;
        return -1;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    Tokenizer t;
    std::vector<Token> tokens;
    TokenizeResult result = t.Tokenize(test1, tokens);

    if(!result){
        std::cout << "Error Tokenizing Javascript. Error " << (int)result.m_ErrorCode << " at " << result.m_Line << ":" << result.m_Column <<std::endl;
        return -1;
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = t2 - t1;
    std::cout << "Tokenizing time: " << duration.count() << " seconds\n";
    std::cout << "There are " << tokens.size() << " Tokens"<<std::endl;

    for(size_t i = 0; i < tokens.size(); i++){
        Token& token = tokens[i];
        //TODO: add pad start functions to HBuffwer
        std::cout << "Token " << i << " " << token.GetLineNumber() << ":" << token.GetColumnNumber()<< " Is (" << (int)token.GetType() << " " << token.GetValue().SubString(0, -1).GetCStr() << ")" << std::endl;
    }

    ASTGenerator astGenerator;
    std::vector<Node*> ast; 
    ast.reserve(1000);
    
    auto t3 = std::chrono::high_resolution_clock::now();

    std::cout <<"Parsing phase"<<std::endl;
    ASTGeneratorResult parseResult = astGenerator.Generate(tokens, ast);
    std::cout <<"Done Parsing"<<std::endl;

    if(!parseResult){
        Token& errorToken = tokens[parseResult.GetErrorAt()];
        printf("Failed to parse AST. Error %d at %d:%d\n", (uint8_t)parseResult.GetErrorCode(), errorToken.GetLineNumber(), errorToken.GetColumnNumber());
        return -1;
    }
    auto t4 = std::chrono::high_resolution_clock::now();
    duration = t4 - t3;
    std::cout << "AST Generation time: " << duration.count() << " seconds\n";

    for(size_t i = 0; i < ast.size(); i++){
        Node& node = *ast[i];
        //printf("AST Program Child node %d is type %d\n", i, (int)node.GetNodeType());
        node.Print(0);
    }

}