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
    std::cout << "[OK] PLA parsing successful." << std::endl;
    std::cout << "  - Inputs: " << parser.getNumInputs() << std::endl;
    std::cout << "  - Product terms: " << parser.getProductTerms().size() << std::endl;

    // Step 2: TODO - Implement Quine-McCluskey Algorithm
    std::cout << "\n[TODO] Step 2: Quine-McCluskey Algorithm" << std::endl;
    std::cout << "  - Find all Prime Implicants" << std::endl;

    // Step 3: TODO - Implement Petrick's Algorithm
    std::cout << "\n[TODO] Step 3: Petrick's Algorithm" << std::endl;
    std::cout << "  - Find minimal SOP cover" << std::endl;

    // Step 4: TODO - Write output PLA
    std::cout << "\n[TODO] Step 4: Write minimized PLA to " << output_pla << std::endl;

    std::cout << "\n====================================" << std::endl;
    std::cout << "  Program structure ready!" << std::endl;
    std::cout << "  Next: Implement QM + Petrick" << std::endl;
    std::cout << "====================================" << std::endl;

    return 0;
}