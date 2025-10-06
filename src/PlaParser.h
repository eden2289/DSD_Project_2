#pragma once
#include <string>
#include <vector>

// Represents a single product term in a PLA file, like "1-01 1".
struct ProductTerm {
    std::string cube;   // The input part, e.g., "1-01"
    std::string output; // The output part, e.g., "1" or "-"
};

// A parser for simplified PLA (Programmable Logic Array) files.
// It extracts the number of inputs, variable names, and product terms.
class PlaParser{
public:
    // Parses the given PLA file.
    // Returns true on success, false on failure (e.g., file not found).
    bool parse(const std::string& filename);

    // Getters for the parsed data.
    int getNumInputs() const;
    const std::vector<std::string>& getInputNames() const;
    const std::vector<ProductTerm>& getProductTerms() const;
    
    // New functions for Quine-McCluskey algorithm
    std::vector<int> getMinterms() const;    // Get on-set minterms (output = "1")
    std::vector<int> getDontCares() const;   // Get don't care minterms (output = "-")

private:
    int num_inputs = 0; // Number of input variables (from .i line).
    std::vector<std::string> var_names; // Names of input variables (from .ilb line).
    std::vector<ProductTerm> product_terms; // List of product terms (output = "1" or "-")
    
    // Helper functions
    std::vector<std::string> expandCube(const std::string& cube) const;  // Expand cube with '-'
    int binaryToInt(const std::string& binary) const;  // Convert binary string to decimal
};
