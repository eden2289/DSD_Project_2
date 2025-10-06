// Boolean Function Minimization (Project 2)
// This program reads a PLA file and outputs minimized SOP using Quine-McCluskey + Petrick's algorithm

#include <iostream>
#include <string>
#include "PlaParser.h"

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input.pla> <output.pla>" << std::endl;
        std::cout << "Example: ./minimize pla_files/test1.pla output.pla" << std::endl;
        return 1;
    }

    std::string input_pla = argv[1];
    std::string output_pla = argv[2];
    
    std::cout << "====================================" << std::endl;
    std::cout << "  Boolean Function Minimization" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "Input  PLA: " << input_pla << std::endl;
    std::cout << "Output PLA: " << output_pla << std::endl;

    // Step 1: Parse PLA file
    PlaParser parser;
    if (!parser.parse(input_pla)) {
        std::cerr << "[Error] Failed to parse PLA file." << std::endl;
        return 1;
    }
    std::cout << "\n[Step 1] PLA Parsing" << std::endl;
    std::cout << "  ✓ Inputs: " << parser.getNumInputs() << std::endl;
    std::cout << "  ✓ Product terms: " << parser.getProductTerms().size() << std::endl;

    // Step 2: Extract minterms and don't cares
    std::vector<int> minterms = parser.getMinterms();
    std::vector<int> dont_cares = parser.getDontCares();
    
    std::cout << "\n[Step 2] Minterm Extraction" << std::endl;
    std::cout << "  ✓ On-set minterms: ";
    for (size_t i = 0; i < minterms.size(); ++i) {
        std::cout << "m" << minterms[i];
        if (i < minterms.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    std::cout << "  ✓ Don't cares: ";
    if (dont_cares.empty()) {
        std::cout << "(none)";
    } else {
        for (size_t i = 0; i < dont_cares.size(); ++i) {
            std::cout << "m" << dont_cares[i];
            if (i < dont_cares.size() - 1) std::cout << ", ";
        }
    }
    std::cout << std::endl;

    // Step 3: TODO - Implement Quine-McCluskey Algorithm
    std::cout << "\n[Step 3] Quine-McCluskey Algorithm" << std::endl;
    std::cout << "  → TODO: Find all Prime Implicants" << std::endl;

    // Step 4: TODO - Implement Petrick's Algorithm
    std::cout << "\n[Step 4] Petrick's Algorithm" << std::endl;
    std::cout << "  → TODO: Find minimal SOP cover" << std::endl;

    // Step 5: TODO - Write output PLA
    std::cout << "\n[Step 5] Write Output" << std::endl;
    std::cout << "  → TODO: Write minimized PLA to " << output_pla << std::endl;

    std::cout << "\n====================================" << std::endl;
    std::cout << "  PlaParser implementation complete!" << std::endl;
    std::cout << "  Next: Implement QM + Petrick" << std::endl;
    std::cout << "====================================" << std::endl;

    return 0;
}