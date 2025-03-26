#include "pch.h"

#include <filesystem>
#include <fstream>

#include "FJASTP.h"
#include "FJASTP/Tokenizer.h"
#include "FJASTP/ASTGenerator.h"

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
    FJASTP::InitializationResult initResult = FJASTP::FJASTP::Init();
    if(initResult != FJASTP::InitializationResult::Success){
        std::cout << "Failed to initialize FJASTP"<<std::endl;
        return -1;
    }

    FJASTP::Tokenizer t;
    std::vector<FJASTP::Token> tokens;
    FJASTP::TokenizeResult result = t.Tokenize(test1, tokens);
    
    if(!result){
        std::cout << "Error Tokenizing Javascript. Error " << (int)result.m_ErrorCode << " at " << result.m_Line << ":" << result.m_Column <<std::endl;
        return -1;
    }

    std::cout << "Done Tokenizing"<<std::endl;
    
    for(size_t i = 0; i < tokens.size(); i++){
        FJASTP::Token& token = tokens[i];
        //TODO: add pad start functions to HBuffwer
        std::cout << "Token " << i << " " << token.GetLineNumber() << ":" << token.GetColumnNumber()<< " Is (" << (int)token.GetType() << " " << token.GetValue().SubString(0, -1).GetCStr() << ")" << std::endl;
    }

    FJASTP::ASTGenerator astGenerator;
    std::vector<FJASTP::Node> ast; 
    FJASTP::ASTGeneratorResult parseResult = astGenerator.Generate(tokens, ast);

    if(!parseResult){
        printf("Failed to parse ast %d", (uint8_t)parseResult.GetErrorCode());
    }

    for(size_t i = 0; i < ast.size(); i++){
        FJASTP::Node& node = ast[i];
        printf("AST Program Child node %dis type %d", i, (int)node.GetNodeType());
    }
}