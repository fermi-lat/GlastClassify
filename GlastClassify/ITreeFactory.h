/**@file ITreeFactory.h

@brief declaration of class TreeFactory
@author T. Burnett

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/GlastClassify/TreeFactory.h,v 1.3 2005/07/28 02:21:52 burnett Exp $
*/

#ifndef GlastClassify_ITreeFactory_h
#define GlastClassify_ITreeFactory_h

#include <string>
#include <utility>


namespace GlastClassify {

    /** @class ITreeFactory
    @brief An interface for a factory for accessing decision trees

    */
    class ITreeFactory {
    public:

        /** @class ILookupData
        @brief nested class definition interface that must be implemented by client
        */
        class ILookupData 
        {
        public:
            // return pair; bool is true if pointer to a double, false if pointer to a float
            virtual std::pair<bool, const void*> operator()(const std::string& name)=0;
            // return index of item in vector
            virtual int index(const std::string& name) const = 0;
            /// a bit of a kluge for ROOT. The above might have to be cast to a float*
            virtual bool isFloat()const {return false;}  
        };

        /** @class ITree
        @brief nested class definition
        This class wraps a DecisionTree object
        */
        class ITree {
        public:
            virtual ~ITree() {}
            virtual double operator()() const = 0;
            /// @return the title
            virtual std::string title()const = 0;
        };

        /** @param name a folder name completing the path to the folder containing the tree data
        
         @return a reference to a new tree. See also the evaluate() method.
         */
        virtual const ITreeFactory::ITree& operator()(const std::string& name) = 0;

        /// @return value of Tree # i for current set of values
        virtual double evaluate(int i) const = 0;

        /// index does the evaluate.
        virtual double operator[](int i) const = 0;

        virtual ~ITreeFactory() {};
    };


} // namespace

#endif
