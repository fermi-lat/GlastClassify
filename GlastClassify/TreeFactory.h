/**@file TreeFactory.h

@brief declaration of class TreeFactory
@author T. Burnett

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/GlastClassify/TreeFactory.h,v 1.4 2005/11/04 18:10:48 burnett Exp $
*/

#ifndef GlastClassify_TreeFactory_h
#define GlastClassify_TreeFactory_h

#include "GlastClassify/ITreeFactory.h"

class DecisionTree;

#include <vector>
#include <utility>


namespace GlastClassify {

    /** @class TreeFactory
    @brief A factory for accessing decision trees

    */
    class TreeFactory : virtual public ITreeFactory {
    public:

        // forward declaration
        class GleamValues;

        /** @brief ctor sets up for tree production 
        @param path file path to a folder containing tree data
        @param lookup Instance of a class supplied to by user, which is called back 
        to find address of each variable


        */
        TreeFactory(const std::string& path, ITreeFactory::ILookupData& lookup)
            :m_path(path)
            ,m_lookup(lookup){};

        /** @class Tree
        @brief nested class definition
        This class wraps a DecisionTree object
        */
        class Tree : virtual public ITreeFactory::ITree {
        public:
            Tree( const std::string& path, ILookupData& lookup);
            double operator()()const;
            ~Tree();
            /// @return the title
            std::string title()const;

        private:
            const DecisionTree* m_filter_tree; ///< optional filter, or filter and local tree
            std::vector<const TreeFactory::Tree*> m_exclusive_trees; ///< list of (assumed) exclusive trees to evaluate
            GleamValues* m_vals;
        };

        /** @param name a folder name completing the path to the folder containing the tree data
        
         @return a reference to a new tree. See also the evaluate() method.
         */
        const ITreeFactory::ITree& operator()(const std::string& name);


        /// @return value of Tree # i for current set of values
        virtual double evaluate(int i) const; 

        /// index does the evaluate.
        virtual double operator[](int i) const{return evaluate(i);}

        virtual ~TreeFactory();


    private:

        std::string m_path;
        ITreeFactory::ILookupData& m_lookup;

        std::vector<TreeFactory::Tree*> m_trees;

    };


} // namespace

#endif
