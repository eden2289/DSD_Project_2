#pragma once
#include <string>
#include <vector>
#include <set>

// Represents a single implicant (product term) in Quine-McCluskey algorithm
// Example: "10-1" means variables: a=1, b=0, c=don't care, d=1
class Implicant {
public:
    std::string value;              // Binary representation with '-' for don't cares
    std::set<int> minterms;         // Set of minterm numbers this implicant covers
    bool used;                      // Flag: has this been combined with another implicant?
    
    // Constructor
    Implicant(const std::string& val, int minterm_num);
    Implicant(const std::string& val, const std::set<int>& mts);
    
    // Count the number of '1's in the binary representation
    // Used for grouping implicants
    int countOnes() const;
    
    // Check if this implicant can be combined with another
    // Two implicants can combine if they differ in exactly one bit position
    bool canCombineWith(const Implicant& other) const;
    
    // Combine this implicant with another
    // Returns a new implicant with '-' at the differing bit position
    Implicant combineWith(const Implicant& other) const;
    
    // Convert minterm set to string for display (e.g., "0,2,8,10")
    std::string mintermsToString() const;
    
    // Check if this implicant covers a specific minterm
    bool coversMinterm(int m) const;
};

// Implements the Quine-McCluskey algorithm for finding prime implicants
class QuineMcCluskey {
private:
    int num_vars;                           // Number of input variables
    std::vector<Implicant> prime_implicants; // Final list of prime implicants
    
    // Helper function: Group implicants by number of 1's
    std::vector<std::vector<Implicant>> groupByOnes(const std::vector<Implicant>& implicants);
    
    // Helper function: Combine one column with the next
    // Returns new implicants and marks used ones
    std::vector<Implicant> combineGroups(std::vector<std::vector<Implicant>>& groups);
    
    // Helper function: Convert integer to binary string with leading zeros
    std::string intToBinary(int num, int width) const;
    
public:
    // Constructor
    QuineMcCluskey(int num_variables);
    
    // Main algorithm: Find all prime implicants from minterms and don't cares
    // minterms: on-set minterms (must be covered)
    // dont_cares: don't care minterms (can be used but don't need to be covered)
    void findPrimeImplicants(const std::vector<int>& minterms, 
                            const std::vector<int>& dont_cares);
    
    // Get the computed prime implicants
    const std::vector<Implicant>& getPrimeImplicants() const;
    
    // Print detailed steps for debugging
    void printDetailedSteps(const std::vector<int>& minterms, 
                           const std::vector<int>& dont_cares);
};
