#include "PlaParser.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

// Parses a PLA file line by line.
bool PlaParser::parse(const std::string& filename){
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open " << filename << std::endl;
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string first_token;
        ss >> first_token; // Read the first word, automatically skipping whitespace

        // Skip empty lines and comments (lines starting with '#').
        if (first_token.empty() || first_token[0] == '#') {
            continue;
        }
        
        // Process lines starting with '.', which are PLA commands.
        if (first_token[0] == '.') {
            if (first_token == ".i") {
                // .i: specifies the number of inputs.
                ss >> this->num_inputs;
                this->var_names.resize(this->num_inputs);
            } else if (first_token == ".ilb") {
                // .ilb: specifies the names of the input variables.
                for (size_t i = 0; i < var_names.size(); ++i) {
                    ss >> var_names[i];
                }
            } else if (first_token == ".p") {
                // .p: specifies the number of product terms. We reserve space for efficiency.
                int num_products;
                ss >> num_products;
                this->product_terms.reserve(num_products);
            } else if (first_token == ".e") {
                // .e: marks the end of the file.
                break;
            }
            // Other commands like .o, .type are ignored.
        } else {
            // If the line does not start with '.', it's a product term.
            // The first token is the input cube.
            std::string output;
            ss >> output; // The second token is the output.
            if (!output.empty()) {
                // Modified: Accept both "1" (on-set) and "-" (don't care)
                if (output == "1" || output == "-") {
                    product_terms.push_back({first_token, output});
                }
            }
        }
    }
    file.close();
    return true;
}

// Getter for the number of inputs.
int PlaParser::getNumInputs() const {
    return this->num_inputs;
}

// Getter for the input variable names.
const std::vector<std::string>& PlaParser::getInputNames() const {
    return this->var_names;
}

// Getter for the product terms.
const std::vector<ProductTerm>& PlaParser::getProductTerms() const {
    return product_terms;
}

// ==================== NEW FUNCTIONS ====================

// Helper function: Expand a cube with '-' into all possible binary strings
// Example: expandCube("1-0") returns {"100", "110"}
std::vector<std::string> PlaParser::expandCube(const std::string& cube) const {
    std::vector<std::string> result;
    
    // Find the first '-' position
    size_t pos = cube.find('-');
    
    if (pos == std::string::npos) {
        // No '-' found, this is already a complete minterm
        result.push_back(cube);
    } else {
        // Found a '-', replace it with '0' and '1' and recursively expand
        std::string cube0 = cube;
        std::string cube1 = cube;
        
        cube0[pos] = '0';  // Replace '-' with '0'
        cube1[pos] = '1';  // Replace '-' with '1'
        
        // Recursively expand both branches
        std::vector<std::string> expanded0 = expandCube(cube0);
        std::vector<std::string> expanded1 = expandCube(cube1);
        
        // Merge results
        result.insert(result.end(), expanded0.begin(), expanded0.end());
        result.insert(result.end(), expanded1.begin(), expanded1.end());
    }
    
    return result;
}

// Helper function: Convert binary string to decimal integer
// Example: binaryToInt("1001") returns 9
int PlaParser::binaryToInt(const std::string& binary) const {
    int result = 0;
    for (char c : binary) {
        result = result * 2 + (c - '0');
    }
    return result;
}

// Get all on-set minterms (output = "1")
// This function:
// 1. Filters product terms with output = "1"
// 2. Expands cubes with '-' into all possible minterms
// 3. Converts binary strings to decimal integers
std::vector<int> PlaParser::getMinterms() const {
    std::vector<int> result;
    
    for (const auto& term : product_terms) {
        if (term.output == "1") {
            // Expand the cube (handles '-' wildcards)
            std::vector<std::string> expanded = expandCube(term.cube);
            
            // Convert each binary string to integer
            for (const auto& binary : expanded) {
                result.push_back(binaryToInt(binary));
            }
        }
    }
    
    return result;
}

// Get all don't care minterms (output = "-")
// Similar to getMinterms(), but for don't care terms
std::vector<int> PlaParser::getDontCares() const {
    std::vector<int> result;
    
    for (const auto& term : product_terms) {
        if (term.output == "-") {
            // Expand the cube (handles '-' wildcards)
            std::vector<std::string> expanded = expandCube(term.cube);
            
            // Convert each binary string to integer
            for (const auto& binary : expanded) {
                result.push_back(binaryToInt(binary));
            }
        }
    }
    
    return result;
}