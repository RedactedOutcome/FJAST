#pragma once

#ifdef FJASTP_USE_PCH
#include FJASTP_PCH_DIR
#else
#include <unordered_map>
#endif

namespace FJASTP{
    enum class InitializationResult : uint8_t{
        Success=0
    };

    enum class TokenType : uint8_t{
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

    enum class NodeType : uint8_t{
        None=0,
        EmptyExpression,
        IdentifierExpression,
        NumericalLiteralExpression,
        ArithmeticExpression,
        UnaryExpression,
        Literal,
        ClassDeclaration,
        FunctionDeclaration,
        MethodDeclaration,
        MemeberDeclaration,
        PropertyAccessExpression,
        FunctionCall,
        MethodCall,
        ParameterList,
        ArgumentList,
        Negation
    };

    enum class TokenizerError : uint8_t{
        Success=0,
        EndOfFile,
        InvalidStringLiteral,
        UnsupportedToken,
        InternalLexingError,
        InvalidUTF8Character,
        InvalidNumericalLiteral
    };

    enum class ASTGeneratorError : uint8_t{
        Success=0,
        EndOfFile                       =1,
        InvalidClassDefinition          =2,
        InvalidExpression               =3,
        InvalidNegationExpression       =4,
        InvalidUnaryExpression          =5,
        ExpectedExpressionOrStatement   =6,
        InvalidMethodDeclaration        =7,
        InvalidMethodBody               =8,
        InvalidFunctionDeclaration      =9,
        InvalidFunctionBody             =10,
        InvalidFunctionCall             =11,
        InvalidExpressionGrouping       =12,
        UnsupportedSyntax               =13,
        InvalidParameterList            =14,
        InvalidPropertyAccessExpression =15
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

    enum class ArithmeticOperations : uint8_t{
        Addition,
        Subtraction,
        Multiplication,
        Division,
        Exponentiation,
        Increment,
        Decrement
    };

    class FJastP{
    public:
        static InitializationResult Init() noexcept;

        static bool IsKeyword(const HBuffer& identifier) noexcept{return s_Keywords[identifier] > 0;}
        static uint8_t GetKeywordValue(const HBuffer& identifier) noexcept{return s_Keywords[identifier];}
    private:
        static std::unordered_map<HBuffer, uint8_t> s_Keywords;
    };
}