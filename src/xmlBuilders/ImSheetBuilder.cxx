/** @file ImSheetBuilder.cxx
 *    @brief implementation of classification::Tree; declaration and implementation or its private helper classification::ImSheetBuilder::Node
 *
 *    $Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/xmlBuilders/ImSheetBuilder.cxx,v 1.3 2005/11/10 21:12:18 usher Exp $
 */

#include "ImSheetBuilder.h"
    
#include "DecisionTreeBuilder.h"

#include "xmlAppendEngineFactory.h"
#include "xmlCreateColumnsEngineFactory.h"
#include "xmlFilterColumnsEngineFactory.h"
#include "xmlFilterRowsEngineFactory.h"
#include "xmlMissingValuesEngineFactory.h"
#include "xmlModifyColumnsEngineFactory.h"
#include "xmlPredictEngineFactory.h"
#include "xmlNewPredictEngineFactory.h"
#include "xmlReadTextFileEngineFactory.h"
#include "xmlShuffleEngineFactory.h"
#include "xmlSplitEngineFactory.h"
#include "xmlWriteTextFileEngineFactory.h"

#include "../ImActivityNodes/PredictEngineNode.h"


#include "xmlBase/XmlParser.h"
#include "facilities/Util.h"
#include <xercesc/dom/DOMElement.hpp>
#include "xmlBase/Dom.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <set> 

namespace {
  
    /** @class Exception 
        @brief hold a string
    */
    class Exception : public std::exception
    {
    public: 
        Exception(std::string error):m_what(error){}
        ~Exception() throw() {;}
        virtual const char *what( ) const  throw() { return m_what.c_str();} 
        std::string m_what;
    };

    // Helper function creates a std::string from a DOMString
    // Usage:
    // a = getNextWord(sList, iDelimPos=-1);
    // b = getNextWord(sList, iDelimPos);

    std::string getNextWord(std::string &sList, int &iEnd)
    {
        // This allows space delimited data as well as comma, semicolon, etc.
        static char  cDELIM = '.'; 
        const char *cList;
        int iStart = iEnd + 1; // Start is previous end plus one.
        if(iStart > (int)sList.length())
        {
            // Serious error when you don't initialize iDelimPos=-1
            // before calling this function!
            throw Exception("Error with getNextWord!");
        }
        cList = sList.c_str();
        while((cList[iStart] == cDELIM) && cList[iStart] != 0)
        { 
            iStart++; 
        }
        iEnd = sList.find(cDELIM, iStart);
        if(iEnd > (int)sList.length())
        {
            throw Exception("Error with getNextWord");
        }
        return sList.substr(iStart, iEnd - iStart);
    }

    double getNextDouble(std::string &sList, int &iEnd)
    {
        std::string sIValue = getNextWord(sList, iEnd);
        return atof(sIValue.c_str());
    }
    
    std::string indent(int depth)
    {
        std::string ret("  ");; 
        for( int i =0; i < depth; ++i) ret += "  ";
        return ret;
    }

    // I am tired of typing this....
    typedef std::vector<DOMElement*> DOMEvector;
} // anonomous namespace

XERCES_CPP_NAMESPACE_USE

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
* The idea for classification::Tree is to encapuslate the xml so that the
*   user only
*   has to provide a filename or string and we will give them access to the
*   linked list of the data which will have the ability to do neural 
*   networking algorithms.
*/


//ImSheetBuilder::ImSheetBuilder(const DOMDocument*   document, 
//                               XTtupleVars<double>& tuple,
//                               std::ostream& log) : m_headNode(0), m_tuple(tuple), m_log(log)
ImSheetBuilder::ImSheetBuilder(const DOMDocument*   document, 
                               XTcolumnVal<double>::XTtupleMap& tuple,
                               std::ostream& log) : m_headNode(0), m_tuple(tuple), m_log(log)
{
    // Check document validity
    if(document == 0)
    {
        // Error checking usually for a missing file.
        // Or sometimes due to a bad XML document.
        // Remember <P><BR></P> is only valid
        // in poorly formed html, not xml.
        // When we get a schema for UserLibrary, we'll
        // be able to use that as a validator also.
        throw Exception( "Error: invalid input file ");
    }

    // Clear everything just to be sure
    m_iNodeVec.clear();
    m_idToINodeMap.clear();
    m_typeToINodeVecMap.clear();

    // DecisionTree builder
    m_builder = new DecisionTreeBuilder(document);

    // Find the activity nodes in the document
    int numNodes = findAllActivityNodes(document);

    // Link them together
    int numLinks = linkActivityNodes(document);
  
    return;
}


ImSheetBuilder::~ImSheetBuilder()
{
    return;
}
  
int ImSheetBuilder::findAllActivityNodes(const DOMDocument* document)
{
    // Root...    
    DOMElement* domRoot = document->getDocumentElement();

    // Build map between types and implementation of that type
    std::map<std::string, IxmlEngineFactory*> nodeFactoryMap;

    XTExprsnParser parser(m_tuple);

    nodeFactoryMap["AppendEngineNode"]        = new xmlAppendEngineFactory(parser);
    nodeFactoryMap["CreateColumnsEngineNode"] = new xmlCreateColumnsEngineFactory(parser);
    nodeFactoryMap["FilterColumnsEngineNode"] = new xmlFilterColumnsEngineFactory(parser);
    nodeFactoryMap["FilterRowsEngineNode"]    = new xmlFilterRowsEngineFactory(parser);
    nodeFactoryMap["MissingValuesEngineNode"] = new xmlMissingValuesEngineFactory(parser);
    nodeFactoryMap["ModifyColumnsEngineNode"] = new xmlModifyColumnsEngineFactory(parser);
    nodeFactoryMap["PredictEngineNode"]       = new xmlNewPredictEngineFactory(parser);
    nodeFactoryMap["Toby"]                    = new xmlPredictEngineFactory(parser, m_log, 0);
    nodeFactoryMap["ReadTextFileEngineNode"]  = new xmlReadTextFileEngineFactory(parser);
    nodeFactoryMap["ShuffleEngineNode"]       = new xmlShuffleEngineFactory(parser);
    nodeFactoryMap["SplitEngineNode"]         = new xmlSplitEngineFactory(parser);
    nodeFactoryMap["WriteTextFileEngineNode"] = new xmlWriteTextFileEngineFactory(parser);

    // We now need to ensure that we're getting 
    // ActivityNode[@engineClass=='com.insightful.miner.PredictEngineNode']
    std::vector<DOMElement *> xmlActivityNodes;
    xmlBase::Dom::getDescendantsByTagName(domRoot, "ActivityNode", xmlActivityNodes);

    int cntem = 0;

    // Loop over the ActivityNodes in the input xml file
    for(DOMEvector::iterator actNodeIter = xmlActivityNodes.begin();
        actNodeIter != xmlActivityNodes.end(); actNodeIter++)
    {
        DOMElement* xmlActivityNode = *actNodeIter;

        std::string sType = xmlBase::Dom::getAttribute(xmlActivityNode, "engineClass");  
        std::string sId   = xmlBase::Dom::getAttribute(xmlActivityNode, "id");

        // Get the label for this node
        DOMElement* displayInfo = xmlBase::Dom::findFirstChildByName(xmlActivityNode, "DisplayInfo");
        std::string sDisplay    = xmlBase::Dom::getTagName(displayInfo);
        std::string sName       = xmlBase::Dom::getAttribute(displayInfo, "labelText");

        // Parse the engine class a bit to get to the unique type identifier
        int iDelim = -1;
        std::string sNewType = getNextWord(sType, iDelim);

        while(iDelim > -1)
        {
            sNewType = getNextWord(sType, iDelim);
        }

        // Parse the node if further action necessary
        //std::map<std::string, ParseFunction>::iterator parseFuncIter = m_parseMap.find(sNewType);

        // Check to make sure we "know" the type
        //if (parseFuncIter == m_parseMap.end())
        //{
        //    throw std::invalid_argument("IM file parsing finds unknown node type: "+sNewType);
        //}

        // Get this node
        IImActivityNode* iActivityNode = (*nodeFactoryMap[sNewType])(xmlActivityNode);

        cntem++;

        // Add to the list
        m_iNodeVec.push_back(iActivityNode);

        // Store in maps
        m_idToINodeMap[sId] = iActivityNode;
        m_typeToINodeVecMap[sNewType].push_back(iActivityNode);

        // Preserve Toby style for a bit longer
        if (sNewType == "PredictEngineNode")
        {
            IImActivityNode* iActivityNode = (*nodeFactoryMap["Toby"])(xmlActivityNode);

            cntem++;

            // Store in maps
            m_typeToINodeVecMap["Toby"].push_back(iActivityNode);
        }
    }

    //done
    return m_iNodeVec.size();
}


int ImSheetBuilder::linkActivityNodes(const DOMDocument* document)
{
    // Root...    
    DOMElement* domRoot = document->getDocumentElement();

    // We now need to ensure that we're getting 
    // ActivityNode[@engineClass=='com.insightful.miner.PredictEngineNode']
    std::vector<DOMElement *> xmlLinkVec;
    xmlBase::Dom::getDescendantsByTagName(domRoot, "Link", xmlLinkVec);

    int numLinks = xmlLinkVec.size();

    // Make a copy of the node vector so we can find the "head" node
    std::vector<IImActivityNode*> headVec = m_iNodeVec;

    for(DOMEvector::iterator linkIter = xmlLinkVec.begin();
        linkIter != xmlLinkVec.end(); linkIter++)
    {
        DOMElement* xmlLink = *linkIter;

        std::string sFromNode = xmlBase::Dom::getAttribute(xmlLink, "fromNode");  
        std::string sFromPort = xmlBase::Dom::getAttribute(xmlLink, "fromPort");  
        int         fromPort  = xmlBase::Dom::getIntAttribute(xmlLink, "fromPort");  
        std::string sToNode   = xmlBase::Dom::getAttribute(xmlLink, "toNode");
        std::string sToPort   = xmlBase::Dom::getAttribute(xmlLink, "toPort");
        int         toPort    = xmlBase::Dom::getIntAttribute(xmlLink, "toPort");

        // Comment out for now... (fix in print statement?)
        //if (sFromPort == "1" && sToPort == "1") continue;

        IImActivityNode* fromINode = m_idToINodeMap[sFromNode];
        IImActivityNode* toINode   = m_idToINodeMap[sToNode];

        fromINode->setNodeLink(fromPort, toINode);

        // Find the "to" node in what remains of the vector of node pointers
        std::vector<IImActivityNode*>::iterator toNodeIter = std::find(headVec.begin(), headVec.end(), toINode);

        // Can have the case that a node has already been removed
        if (toNodeIter != headVec.end())
        {
            headVec.erase(toNodeIter);
        }
    }

    // Head vector should be last man standing?
    int headVecSize = headVec.size();
    if (headVecSize > 0) 
    {
        m_headNode = headVec.front();
    }
    else
    {
        throw Exception("ImSheetBuilder did not find a HEAD node!");
    }

    //done
    return numLinks;
}

std::vector<IImActivityNode*> ImSheetBuilder::getActivityINodeVec(std::string& type)
{
    typeToINodeVecMap::iterator nodeIter = m_typeToINodeVecMap.find(type);

    if (nodeIter != m_typeToINodeVecMap.end()) return nodeIter->second;
    else                                       return std::vector<IImActivityNode*>(0);
}


// For output
void ImSheetBuilder::print(std::ostream& out) const
{
    // Start with output of details of the sheet
    out << "***********************" << std::endl;
    out << " Number of Activity nodes: " << m_iNodeVec.size() << std::endl;
    out << " Number of Types found:    " << m_typeToINodeVecMap.size() << std::endl;

    for(typeToINodeVecMap::const_iterator typeIter = m_typeToINodeVecMap.begin(); 
        typeIter != m_typeToINodeVecMap.end(); typeIter++)
    {
        out << "   Type: " << (*typeIter).first << ", Number of instances: " << (*typeIter).second.size() << std::endl;
    }
    
    int depth = 0;

    // Begin with the head node and output the nodes in their order:
    if (m_headNode) m_headNode->print(out, depth);

    return;
}
