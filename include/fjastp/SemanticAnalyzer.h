#pragma once

#ifdef FJASTP_USE_PCH
#include FJASTP_PCH_DIR
#else
#include <iostream>
#include <vector>
#include <string>
#endif

namespace FJASTP{
    class SemanticAnalyzer{
    public:
        SemanticAnalyzer();
        ~SemanticAnalyzer();
    };
}