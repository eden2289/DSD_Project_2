#pragma once
#include "QuineMcCluskey.h"
#include "Petrick.h"
#include <string>
#include <vector>

// Writer for PLA (Programmable Logic Array) files
// Outputs minimized Boolean function in PLA format
class PlaWriter {
private:
    int num_inputs;
    std::vector<std::string> input_names;
    std::string output_name;
    std::vector<Implicant> minimal_cover;
    
    // Convert implicant binary representation to PLA cube format
    // Example: "-0-1" stays as "-0-1"
    std::string implicantToCube(const Implicant& imp) const;
    
public:
    // Constructor
    PlaWriter(int num_vars, 
              const std::vector<std::string>& var_names,
              const std::string& out_name = "F");
    
    // Set the minimal cover (result from Petrick's method)
    void setMinimalCover(const std::vector<Implicant>& cover);
    
    // Write the minimized PLA to file
    bool write(const std::string& filename);
    
    // Calculate statistics
    int getNumProductTerms() const;
    int getTotalLiterals() const;
};
