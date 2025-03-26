#include "pch.h"
#include "ASTGenerator.h"

namespace FJASTP{
    ASTGeneratorResult ASTGenerator::Generate(std::vector<Token>& input, std::vector<Node>& output){
        m_Input = &input;
        m_Output= &output;

        size_t tokenCount = input.size();
        for(size_t i = 0; i < tokenCount; i++){
            const Token& currentToken = input[i];

            switch(currentToken.GetType()){
            case TokenType::Keyword:{
                uint8_t keyword = currentToken.GetMetadata();
                switch(keyword){
                case (uint8_t)Keyword::Class:{
                    std::cout << "Class keyword"<<std::endl;
                    break;
                }
                }
                break;
            }
            default:
                break;
            }
        }
        return ASTGeneratorResult();
    }
}