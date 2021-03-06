/**@file xmlFactoryBase.cxx

@brief implementation of class xmlFactoryBase

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/xmlBuilders/xmlFactoryBase.cxx,v 1.3 2005/11/22 21:19:02 usher Exp $
*/

#include "xmlFactoryBase.h"
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

xmlFactoryBase::xmlFactoryBase(XTExprsnParser& parser)
                         :  m_parser(parser)
{}

xmlFactoryBase::~xmlFactoryBase()
{
    ///@TODO: delete trees
}

xmlFactoryBase::DOMEvector xmlFactoryBase::getXTPropertyVec(const DOMElement* activityNode) const
{ 
    DOMEvector elementVec;

    // Get the list of arguments
    DOMElement* xmlArgumentList = xmlBase::Dom::findFirstChildByName(activityNode, "ArgumentList");
    DOMElement* xmlXtProps      = xmlBase::Dom::findFirstChildByName(xmlArgumentList, "XTProps");

    // Now the vector of properties
    xmlBase::Dom::getChildrenByTagName(xmlXtProps, "Property", elementVec);

    return elementVec;
}

DOMElement* xmlFactoryBase::getXTProperty(const DOMElement* activityNode, const std::string& property) const
{
    DOMElement* element = 0;

    // Now the vector of properties
    DOMEvector xmlPropertyVec = getXTPropertyVec(activityNode);

    for(DOMEvector::iterator propIter = xmlPropertyVec.begin(); propIter != xmlPropertyVec.end(); propIter++)
    {
        DOMElement* xmlProperty = *propIter;

        std::string sName = xmlBase::Dom::getAttribute(xmlProperty, "name");

        if (sName == property)
        {
            element = xmlProperty;
            break;
        }
    }

    return element;
}

xmlFactoryBase::DOMEvector  xmlFactoryBase::getXTSubPropVec(const DOMElement* activityNode, const std::string& property) const
{
    DOMEvector elementVec;

    elementVec.clear();

    DOMElement* xtProperty = getXTProperty(activityNode, property);

    // Now the vector of properties
    if (xtProperty) xmlBase::Dom::getChildrenByTagName(xtProperty, "Property", elementVec);

    return elementVec;
}

  // Inputs a node (usually the root node)
  // and a list such as:
  //	char *straXMLPath[] = {"ActivityNode", "ModelProperties", "IMML", 
  //                           "TreeList", "TreeModel", 0};
  // And returns the node at the end, in this case: TreeModel.
  // The node given should be:
  // <xmlParent><ActivityNode><ModelProperties>
  //		<IMML><TreeList><TreeModel></TreeModel></TreeList></IMML>
  // </ModelProperties></ActivityNode></xmlParent>
  // When calling this function be sure to check Node==0 
  // or Node.isNull()==true;
  // Replace above function entirely because 
  //  - unnecessary calls to native Xerces routines
  //  - vector of strings is much nicer to deal with than array
  //  - all this requires is a simple loop, not recursion
const DOMElement* xmlFactoryBase::findXPath(const DOMElement* xmlParent, const std::vector<std::string>& nodeNames)
{
    unsigned nNames = nodeNames.size();
    if (nNames == 0) return xmlParent;

    const DOMElement* parent = xmlParent;
    DOMElement* child;
    for (unsigned iName = 0; iName < nNames; iName++) 
    {
        child = xmlBase::Dom::findFirstChildByName(parent, nodeNames[iName]);
        if (child == 0) return child;  // path ended prematurely
        parent = child;
    }
    
    return child;
}


    // Helper function creates a std::string from a DOMString
    // Usage:
    // a = getNextWord(sList, iDelimPos=-1);
    // b = getNextWord(sList, iDelimPos);

std::string xmlFactoryBase::getNextWord(std::string &sList, int &iEnd)
{
    // This allows space delimited data as well as comma, semicolon, etc.
    static char  cDELIM = ' '; 
    const char *cList;
    int iStart = iEnd + 1; // Start is previous end plus one.
    if(iStart > (int)sList.length())
    {
        // Serious error when you don't initialize iDelimPos=-1
        // before calling this function!
        std::cerr << "Error with getNextWord!" << std::endl;
        return sList;
    }
    cList = sList.c_str();
    while((cList[iStart] == cDELIM) && cList[iStart] != 0)
    { 
        iStart++; 
    }
    iEnd = sList.find(cDELIM, iStart);
    if(iEnd > (int)sList.length())
    {
        std::cerr << "Error with getNextWord" << std::endl;
    }
    return sList.substr(iStart, iEnd - iStart);
}


double xmlFactoryBase::getNextDouble(std::string &sList, int &iEnd)
{
    std::string sIValue = getNextWord(sList, iEnd);
    return atof(sIValue.c_str());
}

std::string xmlFactoryBase::indent(int depth)
{
    std::string ret(" ");; 
    for( int i =0; i < depth;++i) ret += " ";
    return ret;
}
