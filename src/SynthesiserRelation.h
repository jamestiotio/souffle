#pragma once

#include "Global.h"
#include "IndexSetAnalysis.h"
#include "RamRelation.h"

#include <numeric>

namespace souffle {

class SynthesiserRelation {
protected:
    /** Ram relation referred to by this */
    const RamRelation& relation;

    /** Indices used for this relation */
    const IndexSet& indices;

    /** The data structure used for the relation */
    std::string dataStructure;

    /** The final list of indices used */
    std::vector<std::vector<int>> computedIndices;

    /** The number of the master index */
    size_t masterIndex = -1;

    /** Is this relation used with provenance */
    const bool isProvenance;

public:
    SynthesiserRelation(const RamRelation& rel, const IndexSet& indices, const bool isProvenance = false)
            : relation(rel), indices(indices), isProvenance(isProvenance) {
        /*
// Set data structure

// Generate and set indices
std::vector<std::vector<int>> inds = indices.getAllOrders();

// generate a full index if it doesn't exist
if (inds.empty()) {
    std::vector<int> fullInd(getArity());
    std::iota(fullInd.begin(), fullInd.end(), 0);
    inds.push_back(fullInd);
}

// Non-btree data structures should have all indices expanded
if (dataStructure == "btree") {
    // Add a full index if it does not exist
    bool fullExists = false;
    // check for full ind
    for (auto& ind : inds) {
        if (ind.size() == getArity()) {
            fullExists = true;
        }
    }
    // expand the first ind to be full, it is guaranteed that at least one index exists
    if (!fullExists && !isProvenance) {
        std::set<int> curIndexElems(inds[0].begin(), inds[0].end());

        // expand index to be full
        for (int i = 0; i < getArity(); i++) {
            if (curIndexElems.find(i) == curIndexElems.end()) {
                inds[0].push_back(i);
            }
        }
    }
} else {
    // expand all indexes to be full
    for (auto& ind : inds) {
        if (ind.size() != getArity()) {
            // use a set as a cache for fast lookup
            std::set<int> curIndexElems(ind.begin(), ind.end());

            // expand index to be full
            for (int i = 0; i < getArity(); i++) {
                if (curIndexElems.find(i) == curIndexElems.end()) {
                    ind.push_back(i);
                }
            }
        }

        assert(ind.size() == getArity());
    }
}

// If this relation is used with provenance,
// we must expand all search orders to be full indices,
// since weak/strong comparators and updaters need this,
// and also add provenance annotations to the indices
if (isProvenance) {
    for (auto& ind : inds) {
        if (ind.size() < getArity() - 2) {
            // use a set as a cache for fast lookup
            std::set<int> curIndexElems(ind.begin(), ind.end());

            // expand index to be full
            for (int i = 0; i < getArity() - 2; i++) {
                if (curIndexElems.find(i) == curIndexElems.end()) {
                    ind.push_back(i);
                }
            }
        } else {
            while (ind.size() > getArity() - 2) {
                ind.pop_back();
            }
        }

        // add provenance annotations to the index
        ind.push_back(getArity() - 1);
        ind.push_back(getArity() - 2);

        assert(ind.size() == getArity());
    }
}

computedIndices = inds;
*/
    }

    /** Compute the final list of indices to be used */
    virtual void computeIndices() = 0;

    /** Get arity of relation */
    size_t getArity() const {
        return relation.getArity();
    }

    /** Get data structure of relation */
    std::string getDataStructure() const {
        return dataStructure;
    }

    /** Get list of indices used for relation,
     * guaranteed that original indices in IndexSet
     * come before any generated indices */
    std::vector<std::vector<int>> getIndices() const {
        return computedIndices;
    }

    /** Get stored IndexSet */
    const IndexSet& getIndexSet() const {
        return indices;
    }

    /** Get stored RamRelation */
    const RamRelation& getRamRelation() const {
        return relation;
    }

    /** Print type name */
    virtual std::string getTypeName() = 0;

    /** Generate relation type struct */
    virtual void generateTypeStruct(std::ostream& out) = 0;
    /*
    const {
        if (getArity() == 0) {
            return "t_nullaries";
        }

        if (getDataStructure() == "eqrel") {
            return "t_eqrel";
        }

        std::stringstream out;

        out << "t";
        out << "_" << getDataStructure();
        out << "_" << getArity();

        for (auto& ind : getIndices()) {
            out << "__" << join(ind, "_");
        }

        for (auto& search : getIndexSet().getSearches()) {
            out << "__" << search;
        }

        return out.str();
    }
    */

    static std::unique_ptr<SynthesiserRelation> getSynthesiserRelation(
            const RamRelation& ramRel, const IndexSet& indexSet, bool isProvenance);
};

class SynthesiserNullaryRelation : public SynthesiserRelation {
public:
    SynthesiserNullaryRelation(const RamRelation& ramRel, const IndexSet& indexSet, bool isProvenance)
            : SynthesiserRelation(ramRel, indexSet, isProvenance) {}

    void computeIndices() override;
    std::string getTypeName() override;
    void generateTypeStruct(std::ostream& out) override;
};

class SynthesiserDirectRelation : public SynthesiserRelation {
public:
    SynthesiserDirectRelation(const RamRelation& ramRel, const IndexSet& indexSet, bool isProvenance)
            : SynthesiserRelation(ramRel, indexSet, isProvenance) {}

    void computeIndices() override;
    std::string getTypeName() override;
    void generateTypeStruct(std::ostream& out) override;
};

class SynthesiserIndirectRelation : public SynthesiserRelation {
public:
    SynthesiserIndirectRelation(const RamRelation& ramRel, const IndexSet& indexSet, bool isProvenance)
            : SynthesiserRelation(ramRel, indexSet, isProvenance) {}

    void computeIndices() override;
    std::string getTypeName() override;
    void generateTypeStruct(std::ostream& out) override;
};

class SynthesiserBrieRelation : public SynthesiserRelation {
public:
    SynthesiserBrieRelation(const RamRelation& ramRel, const IndexSet& indexSet, bool isProvenance)
            : SynthesiserRelation(ramRel, indexSet, isProvenance) {}

    void computeIndices() override;
    std::string getTypeName() override;
    void generateTypeStruct(std::ostream& out) override;
};

class SynthesiserEqrelRelation : public SynthesiserRelation {
public:
    SynthesiserEqrelRelation(const RamRelation& ramRel, const IndexSet& indexSet, bool isProvenance)
            : SynthesiserRelation(ramRel, indexSet, isProvenance) {}

    void computeIndices() override;
    std::string getTypeName() override;
    void generateTypeStruct(std::ostream& out) override;
};

}  // end of namespace souffle
