#include "ParserHelper.h"
#include <string>
#include <antlr4/antlr4-runtime.h>
#include "HyraParser.h"
#include "HyraLexer.h"
#include "DebugPrint.h"

using namespace antlr4;

ParserHelper::ParserHelper()
{

}

tree::ParseTree* ParserHelper::Parse(const std::string& filename)
{
    std::ifstream stream(filename.c_str());
	if (!stream) 
	{
		fprintf(stderr, "ERROR < No such file or directory: %s", filename.c_str());
		return nullptr;
	}
    ANTLRInputStream input(stream);
    HyraLexer lexer(&input);
    CommonTokenStream tstream(&lexer);
    HyraParser parser(&tstream);
    auto* interpreter = parser.getInterpreter<atn::ParserATNSimulator>();

    tree::ParseTree *ptree = nullptr;
    try {
        Ref<BailErrorStrategy> bailRef(std::make_shared<BailErrorStrategy>());
        parser.setErrorHandler(bailRef);
        interpreter->setPredictionMode(atn::PredictionMode::SLL);
        ptree = parser.compilationUnit();
    } catch (ParseCancellationException&) {
        Ref<DefaultErrorStrategy> defaultErrorRef(std::make_shared<DefaultErrorStrategy>());
        tstream.reset();
        parser.reset();
        interpreter->setPredictionMode(atn::PredictionMode::LL);
        parser.setErrorHandler(defaultErrorRef);
        ptree = parser.compilationUnit();
    }
    DEBUG(ptree->getText());
    return ptree;
}

