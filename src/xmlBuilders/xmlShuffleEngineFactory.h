/**@file xmlShuffleEngineFactory.h

@brief declaration of class xmlShuffleEngineFactory
@author T. Usher

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/xmlBuilders/xmlShuffleEngineFactory.h,v 1.1 2005/11/07 21:50:54 usher Exp $
*/

#ifndef GlastClassify_xmlShuffleEngineFactory_h
#define GlastClassify_xmlShuffleEngineFactory_h

#include "xmlFactoryBase.h"
#include "IxmlEngineFactory.h"
#include "../ImActivityNodes/IImActivityNode.h"

class DecisionTree;

/** @class xmlShuffleEngineFactory
@brief A factory for accessing decision trees

*/
class xmlShuffleEngineFactory : public xmlFactoryBase, public IxmlEngineFactory 
{
public:
    typedef std::vector<std::string> StringList;

    /** @brief constructotor sets up for tree production 
    @param path file path to a folder containing tree data
    @param lookup Instance of a class supplied to by user, which is called back 
    to find address of each variable
    */
    xmlShuffleEngineFactory(XTExprsnParser& parser);

    /** @param name a folder name completing the path to the folder containing the tree data   
     @return a reference to a new tree. See also the evaluate() method.
     */
    IImActivityNode* operator()(const DOMElement* element);

    virtual ~xmlShuffleEngineFactory();

private:
};


#endif
