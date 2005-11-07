/**@file xmlWriteTextFileEngineFactory.cxx

@brief implementation of class xmlWriteTextFileEngineFactory

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/xmlWriteTextFileEngineFactory.cxx,v 1.1 2005/11/04 23:19:05 usher Exp $
*/

#include "xmlWriteTextFileEngineFactory.h"
#include "../ImActivityNodes/CreateColumnsEngineNode.h"
#include "xmlBase/XmlParser.h"
#include "facilities/Util.h"
#include <xercesc/dom/DOMElement.hpp>
#include "xmlBase/Dom.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <set> 
#include <cmath>  // for M_PI, among others

XERCES_CPP_NAMESPACE_USE


namespace {

    // output errors to console. (note: m_ prefex for data members )
    // only for statistics to show in debug output
    int  nesting_level, max_depth, node_count;  
    double min_prob, max_prob;
} // anonomous namespace

xmlWriteTextFileEngineFactory::xmlWriteTextFileEngineFactory(std::ostream& log, int iVerbosity )
                        : xmlFactoryBase(log,iVerbosity), 
                          m_log(log), m_outputLevel(iVerbosity)
{
}

IImActivityNode* xmlWriteTextFileEngineFactory::operator()(const DOMElement* xmlActivityNode)
{
    // Retrieve name and node id
    DOMElement* displayInfo = xmlBase::Dom::findFirstChildByName(xmlActivityNode, "DisplayInfo");
    std::string sType       = "PredictEngineNode";
    std::string sName       = xmlBase::Dom::getAttribute(displayInfo, "labelText");
    std::string sId         = xmlBase::Dom::getAttribute(xmlActivityNode, "id");

    // Create the node
    CreateColumnsEngineNode* node = new CreateColumnsEngineNode(sType, sName, sId);

    return node;
}

xmlWriteTextFileEngineFactory::~xmlWriteTextFileEngineFactory()
{
    ///@TODO: delete trees
}
