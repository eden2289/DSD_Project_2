#pragma once
#include "QuineMcCluskey.h"
#include <vector>
#include <set>
#include <map>

// Type definitions for Petrick's Method
// A PetrickTerm is a set of PI indices (represents AND operation)
// Example: {0, 2, 3} means PI0 AND PI2 AND PI3
typedef std::set<int> PetrickTerm;

// A PetrickSOP is a collection of PetrickTerms (represents OR operation)
// Example: [{0,1}, {2,3}] means (PI0·PI1) + (PI2·PI3)
typedef std::vector<PetrickTerm> PetrickSOP;

// Structure to hold information about Essential Prime Implicants
struct EssentialPIInfo {
    int pi_index;                      // Index of the Essential PI
    std::vector<int> unique_minterms;  // Minterms covered ONLY by this PI
};

// Prime Implicant Coverage Chart
// Shows which PIs cover which minterms
class PrimeImplicantChart {
private:
    std::vector<int> on_set_minterms;           // Minterms that MUST be covered (excludes don't cares)
    std::vector<Implicant> prime_implicants;     // All Prime Implicants
    std::vector<std::vector<bool>> coverage;     // coverage[pi_idx][mt_idx] = does PI cover this minterm?
    
    // Mapping from minterm value to its index in on_set_minterms
    std::map<int, int> minterm_to_index;

public:
    // Build the coverage chart
    void build(const std::vector<Implicant>& pis, 
               const std::vector<int>& minterms,
               const std::vector<int>& dont_cares);
    
    // Find Essential Prime Implicants
    std::vector<EssentialPIInfo> findEssentialPIs() const;
    
    // Get minterms not covered by given PIs
    std::vector<int> getRemainingMinterms(const std::vector<int>& covered_pi_indices) const;
    
    // Check if a PI covers a specific minterm
    bool covers(int pi_index, int minterm) const;
    
    // Get all PIs that cover a specific minterm
    std::vector<int> getPIsCovering(int minterm) const;
    
    // Display the coverage chart
    void print() const;
    
    // Getters
    const std::vector<int>& getOnSetMinterms() const { return on_set_minterms; }
    const std::vector<Implicant>& getPrimeImplicants() const { return prime_implicants; }
    int getNumPIs() const { return prime_implicants.size(); }
    int getNumMinterms() const { return on_set_minterms.size(); }
};

// Petrick's Method Solver
// Finds the minimal set of PIs to cover all on-set minterms
class PetrickSolver {
private:
    PrimeImplicantChart chart;
    std::vector<int> essential_pi_indices;      // Indices of Essential PIs
    std::vector<int> additional_pi_indices;     // Additional PIs selected by Petrick's Method
    
    // Build Petrick's function for remaining minterms
    PetrickSOP buildPetrickFunction(const std::vector<int>& remaining_minterms);
    
    // Multiply SOP with a new clause (Boolean algebra)
    PetrickSOP multiply(const PetrickSOP& sop, const PetrickTerm& clause);
    
    // Simplify SOP by removing absorbed terms
    void simplify(PetrickSOP& sop);
    
    // Select the minimal solution from multiple options
    PetrickTerm selectMinimalCover(const PetrickSOP& solutions);
    
    // Count total literals in a set of PIs
    int countLiterals(const PetrickTerm& pis) const;

public:
    // Main solving function
    void solve(const std::vector<Implicant>& pis, 
               const std::vector<int>& minterms,
               const std::vector<int>& dont_cares);
    
    // Get results
    std::vector<int> getEssentialPIs() const { return essential_pi_indices; }
    std::vector<int> getAdditionalPIs() const { return additional_pi_indices; }
    std::vector<int> getAllSelectedPIs() const;
    std::vector<Implicant> getMinimalCover() const;
    
    // Display detailed solving process
    void printDetailedSteps(const std::vector<int>& remaining_minterms,
                           const PetrickSOP& petrick_function);
    
    // Display final solution
    void printSolution() const;
};
