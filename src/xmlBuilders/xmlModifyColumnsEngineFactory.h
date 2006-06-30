/**@file xmlModifyColumnsEngineFactory.h

@brief declaration of class xmlModifyColumnsEngineFactory
@author T. Usher

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/xmlBuilders/xmlModifyColumnsEngineFactory.h,v 1.2 2005/11/22 21:19:03 usher Exp $
*/

#ifndef GlastClassify_xmlModifyColumnsEngineFactory_h
#define GlastClassify_xmlModifyColumnsEngineFactory_h

#include "xmlFactoryBase.h"
#include "IxmlEngineFactory.h"
#include "../ImActivityNodes/IImActivityNode.h"

/** @class xmlModifyColumnsEngineFactory
@brief A factory for accessing decision trees

*/
class xmlModifyColumnsEngineFactory : public xmlFactoryBase, public IxmlEngineFactory
{
public:
    typedef std::vector<std::string> StringList;

    /** @brief constructotor sets up for tree production 
    @param path file path to a folder containing tree data
    @param lookup Instance of a class supplied to by user, which is called back 
    to find address of each variable
    */
    xmlModifyColumnsEngineFactory(XTExprsnParser& parser);

    /** @param name a folder name completing the path to the folder containing the tree data   
     @return a reference to a new tree. See also the evaluate() method.
     */
    IImActivityNode* operator()(const DOMElement* element);

    virtual ~xmlModifyColumnsEngineFactory();

private:
};


#endif
