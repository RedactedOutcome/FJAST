#pragma once

#ifdef FJASTP_USE_PCH
#include FJASTP_PCH_DIR
#else
#include <unordered_map>
#endif

namespace FJASTP{
    enum class InitializationResult{
        Success=0
    };

    enum class TokenType{
        EndOfFile=0,
        Identifier,
        Keyword,
        StringLiteral,
        NumericalLiteral,
        BoolLiteral,
        NullLiteral,
        ArithmeticOperator,
        UnaryOperator,
        LogicalOperator,
        ConditionalOperator,
        AssignmentOperator,
        BitwiseOperator,
        TernaryOperator,
        Punctuator,
        GroupingSymbol,
        RegExpLiteral,
        TemplateLiteral
    };
    enum class TokenizerError{
        Success=0,
        EndOfFile,
        InvalidStringLiteral,
        UnsupportedToken,
        InternalLexingError,
        InvalidUTF8Character,
        InvalidNumericalLiteral
    };

    enum class ASTGeneratorError{
        Success=0,
        EndOfFile,
        InvalidClassDefinition,
        InvalidExpression,
        ExpectedExpressionOrStatement,
        InvalidMethodDeclaration,
        InvalidMethodBody,
        InvalidFunctionDeclaration,
        InvalidFunctionBody,
        UnsupportedSyntax
    };

    enum class NumericalLiteralBaseMetadataFormat : uint8_t{
        Base10=0,
        Binary=1,
        Hexidecimal=2,
        ScientificNotation=3
    };

    enum class Keyword : uint8_t{
        Await=1,
        Break,
        Case,
        Catch,
        Class,
        Const,
        Continue,
        Debugger,
        Default,
        Delete,
        Do,
        Else,
        Export,
        Extends,
        Finally,
        For,
        Function,
        If,
        Import,
        In,
        InstanceOf,
        Let,
        New,
        Return,
        Super,
        Switch,
        This,
        Throw,
        Try,
        TypeOf,
        Var,
        Void,
        While,
        With,
        Yield
    };

    enum class NodeType : uint8_t{
        None=0,
        Expression,
        Literal,
        ClassDeclaration,
        FunctionDeclaration,
        MethodDeclaration,
        MemeberDeclaration
    };

    enum class ExpressionType : uint8_t{

    };
    class  FJastP{
    public:
        static InitializationResult Init() noexcept;

        static bool IsKeyword(const HBuffer& identifier) noexcept{return s_Keywords[identifier] > 0;}
        static uint8_t GetKeywordValue(const HBuffer& identifier) noexcept{return s_Keywords[identifier];}
    private:
        static std::unordered_map<HBuffer, uint8_t> s_Keywords;
    };
}