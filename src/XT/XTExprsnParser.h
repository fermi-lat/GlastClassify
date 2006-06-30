/**@file XTExprsnParser.h
@brief Contains class definitions for implementing a "local" ntuple  row
@author T. Usher
$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/XT/XTExprsnParser.h,v 1.1 2005/11/22 21:19:03 usher Exp $
*/

#ifndef XTExprsnParser_h
#define XTExprsnParser_h

#include "XprsnTree.h"
#include "XTtupleVars.h"

#include <iostream>
#include <map>
#include <vector>

class XTExprsnParser
{
public:
    XTExprsnParser(XTtupleMap& tuple);
    ~XTExprsnParser() {} 

    // Parse and expression
    IXTExprsnNode* parseExpression(std::string& expression);

    // Provide access mechanism to local tuple
    XTtupleMap& getXtTupleVars() {return m_tuple;}

private:
    typedef std::pair<std::string,int>         DelimPair;
    typedef std::vector<DelimPair >            DelimVector;
    typedef std::map<std::string,std::string>  DelimMap;

    // Function to find the next defined delimiter in our input string
    DelimPair      findNextDelimiter(const std::string& inString, int& startPos);
    std::string    findEnclosingParens(const std::string& expression, int& startPos, int& endPos);
    std::string    findCategoricalVal(const std::string& expression, int& startPos, int& endPos);

    // Functions to clean up the input string
    std::string    trimCharacters(std::string& expression, const char& charToTrim);
    std::string    trimTrailing(std::string& expression); 

    // Function to evaluate a string which represents a "value"
    IXTExprsnNode* parseNextExpression(std::string& expression);
    IXTExprsnNode* parseValue(std::string& expression);
    IXTExprsnNode* parseFunction(std::string& expression);

    XTtupleMap&   m_tuple;

    DelimVector   m_delimiters;
    DelimMap      m_delimMap;
};


#endif
