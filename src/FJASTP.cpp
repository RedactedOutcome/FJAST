#include "pch.h"
#include "FJASTP/FJASTP.h"

namespace FJASTP{
    std::unordered_map<HBuffer, uint8_t> FJastP::s_Keywords;

    InitializationResult FJastP::Init() noexcept{
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("await"),       (uint8_t)Keyword::Await));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("break"),       (uint8_t)Keyword::Break));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("case"),        (uint8_t)Keyword::Case));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("catch"),       (uint8_t)Keyword::Catch));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("class"),       (uint8_t)Keyword::Class));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("const"),       (uint8_t)Keyword::Const));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("continue"),    (uint8_t)Keyword::Continue));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("debugger"),    (uint8_t)Keyword::Debugger));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("default"),     (uint8_t)Keyword::Default));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("delete"),      (uint8_t)Keyword::Delete));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("do"),          (uint8_t)Keyword::Do));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("else"),        (uint8_t)Keyword::Else));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("export"),      (uint8_t)Keyword::Export));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("extends"),     (uint8_t)Keyword::Extends));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("finally"),     (uint8_t)Keyword::Finally));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("for"),         (uint8_t)Keyword::For));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("function"),    (uint8_t)Keyword::Function));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("if"),          (uint8_t)Keyword::If));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("import"),      (uint8_t)Keyword::Import));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("in"),          (uint8_t)Keyword::In));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("instanceof"),  (uint8_t)Keyword::InstanceOf));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("let"),         (uint8_t)Keyword::Let));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("new"),         (uint8_t)Keyword::New));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("return"),      (uint8_t)Keyword::Return));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("super"),       (uint8_t)Keyword::Super));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("switch"),      (uint8_t)Keyword::Switch));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("this"),        (uint8_t)Keyword::This));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("throw"),       (uint8_t)Keyword::Throw));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("try"),         (uint8_t)Keyword::Try));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("typeof"),      (uint8_t)Keyword::TypeOf));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("var"),         (uint8_t)Keyword::Var));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("void"),        (uint8_t)Keyword::Void));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("while"),       (uint8_t)Keyword::While));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("with"),        (uint8_t)Keyword::With));
        s_Keywords.insert(std::make_pair<HBuffer, uint8_t>(HBuffer("yield"),       (uint8_t)Keyword::Yield));
        return InitializationResult::Success;
    }
    
    const char* FJASTP::ASTGeneratorErrorStrings[17] = {
        "Success",
        "End Of File",
        "Invalid Class Definition",
        "Invalid Expression",
        "Invalid NegationExpression",
        "Invalid UnaryExpression",
        "Expected Expression Or Statement",
        "Invalid Method Declaration",
        "Invalid Method Body",
        "Invalid Function Declaration",
        "Invalid Function Body",
        "Invalid Function Call",
        "Invalid Expression Grouping",
        "Unsupported Syntax",
        "Invalid Paramter List",
        "Invalid Propery Access Expression",
        "Invalid Variable Assignment"
    };
}