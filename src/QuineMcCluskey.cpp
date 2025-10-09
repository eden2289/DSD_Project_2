#include "QuineMcCluskey.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

// ==================== Implicant Class Implementation ====================

// Constructor: Create implicant from binary string and single minterm
Implicant::Implicant(const std::string& val, int minterm_num) 
    : value(val), used(false) {
    minterms.insert(minterm_num);
}

// Constructor: Create implicant from binary string and set of minterms
Implicant::Implicant(const std::string& val, const std::set<int>& mts) 
    : value(val), minterms(mts), used(false) {
}

// Count number of '1's in the binary representation
// This is used to group implicants by number of 1's
// Example: "1011" has 3 ones
int Implicant::countOnes() const {
    int count = 0;
    for (char c : value) {
        if (c == '1') count++;
    }
    return count;
}

// Check if two implicants can be combined
// Rule: They must differ in EXACTLY ONE bit position
// AND all '-' positions must match
bool Implicant::canCombineWith(const Implicant& other) const {
    if (value.length() != other.value.length()) {
        return false;
    }
    
    int diff_count = 0;
    int diff_pos = -1;
    
    for (size_t i = 0; i < value.length(); ++i) {
        if (value[i] != other.value[i]) {
            // If one has '-' and the other doesn't, they can't combine
            if (value[i] == '-' || other.value[i] == '-') {
                return false;
            }
            diff_count++;
            diff_pos = i;
        }
    }
    
    // Must differ in exactly one position
    return diff_count == 1;
}

// Combine two implicants
// The differing bit position becomes '-'
// Example: "1001" + "1011" = "10-1"
Implicant Implicant::combineWith(const Implicant& other) const {
    std::string new_value = value;
    
    // Find the differing position and replace with '-'
    for (size_t i = 0; i < value.length(); ++i) {
        if (value[i] != other.value[i]) {
            new_value[i] = '-';
            break;
        }
    }
    
    // Merge the minterm sets
    std::set<int> new_minterms = minterms;
    new_minterms.insert(other.minterms.begin(), other.minterms.end());
    
    return Implicant(new_value, new_minterms);
}

// Convert minterm set to string for display
// Example: {0, 2, 8, 10} -> "0,2,8,10"
std::string Implicant::mintermsToString() const {
    std::ostringstream oss;
    bool first = true;
    for (int m : minterms) {
        if (!first) oss << ",";
        oss << m;
        first = false;
    }
    return oss.str();
}

// Check if this implicant covers a specific minterm
bool Implicant::coversMinterm(int m) const {
    return minterms.find(m) != minterms.end();
}

// ==================== QuineMcCluskey Class Implementation ====================

// Constructor
QuineMcCluskey::QuineMcCluskey(int num_variables) 
    : num_vars(num_variables) {
}

// Helper: Convert integer to binary string with leading zeros
// Example: intToBinary(5, 4) = "0101"
std::string QuineMcCluskey::intToBinary(int num, int width) const {
    std::string binary = "";
    for (int i = width - 1; i >= 0; --i) {
        binary += ((num >> i) & 1) ? '1' : '0';
    }
    return binary;
}

// Helper: Group implicants by number of 1's
// Returns a vector where index i contains all implicants with i ones
std::vector<std::vector<Implicant>> QuineMcCluskey::groupByOnes(
    const std::vector<Implicant>& implicants) {
    
    std::vector<std::vector<Implicant>> groups(num_vars + 1);
    
    for (const auto& imp : implicants) {
        int ones = imp.countOnes();
        groups[ones].push_back(imp);
    }
    
    return groups;
}

// Helper: Combine adjacent groups
// Try to combine implicants from group i with group i+1
std::vector<Implicant> QuineMcCluskey::combineGroups(
    std::vector<std::vector<Implicant>>& groups) {
    
    std::vector<Implicant> new_implicants;
    
    // Try combining group i with group i+1
    for (size_t i = 0; i < groups.size() - 1; ++i) {
        for (auto& imp1 : groups[i]) {
            for (auto& imp2 : groups[i + 1]) {
                if (imp1.canCombineWith(imp2)) {
                    // Mark both as used
                    imp1.used = true;
                    imp2.used = true;
                    
                    // Create combined implicant
                    Implicant combined = imp1.combineWith(imp2);
                    
                    // Check if this combination already exists
                    bool exists = false;
                    for (const auto& existing : new_implicants) {
                        if (existing.value == combined.value && 
                            existing.minterms == combined.minterms) {
                            exists = true;
                            break;
                        }
                    }
                    
                    if (!exists) {
                        new_implicants.push_back(combined);
                    }
                }
            }
        }
    }
    
    return new_implicants;
}

// Main algorithm: Find all prime implicants
void QuineMcCluskey::findPrimeImplicants(
    const std::vector<int>& minterms, 
    const std::vector<int>& dont_cares) {
    
    prime_implicants.clear();
    
    // Step 1: Combine minterms and don't cares into initial implicants
    std::vector<Implicant> current_implicants;
    
    for (int m : minterms) {
        std::string binary = intToBinary(m, num_vars);
        current_implicants.push_back(Implicant(binary, m));
    }
    
    for (int dc : dont_cares) {
        std::string binary = intToBinary(dc, num_vars);
        current_implicants.push_back(Implicant(binary, dc));
    }
    
    // Step 2: Iteratively combine implicants until no more combinations possible
    bool has_new_implicants = true;
    int iteration = 0;
    
    while (has_new_implicants) {
        iteration++;
        
        // Group current implicants by number of 1's
        auto groups = groupByOnes(current_implicants);
        
        // Try to combine adjacent groups
        std::vector<Implicant> new_implicants = combineGroups(groups);
        
        // Collect prime implicants (those that couldn't be combined)
        for (auto& group : groups) {
            for (auto& imp : group) {
                if (!imp.used) {
                    // This is a prime implicant
                    bool exists = false;
                    for (const auto& pi : prime_implicants) {
                        if (pi.value == imp.value && pi.minterms == imp.minterms) {
                            exists = true;
                            break;
                        }
                    }
                    if (!exists) {
                        prime_implicants.push_back(imp);
                    }
                }
            }
        }
        
        // Check if we have new implicants for next iteration
        has_new_implicants = !new_implicants.empty();
        current_implicants = new_implicants;
    }
}

// Get the computed prime implicants
const std::vector<Implicant>& QuineMcCluskey::getPrimeImplicants() const {
    return prime_implicants;
}

// Print detailed steps for debugging
void QuineMcCluskey::printDetailedSteps(
    const std::vector<int>& minterms, 
    const std::vector<int>& dont_cares) {
    
    std::cout << "\n┌─────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│  Quine-McCluskey Algorithm - Detailed Steps            │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────┘" << std::endl;
    
    // Combine all minterms and don't cares
    std::vector<Implicant> current_implicants;
    
    std::cout << "\n📌 Initial Minterms:" << std::endl;
    std::cout << "   On-set: ";
    for (size_t i = 0; i < minterms.size(); ++i) {
        std::cout << "m" << minterms[i];
        if (i < minterms.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    std::cout << "   Don't cares: ";
    if (dont_cares.empty()) {
        std::cout << "(none)";
    } else {
        for (size_t i = 0; i < dont_cares.size(); ++i) {
            std::cout << "m" << dont_cares[i];
            if (i < dont_cares.size() - 1) std::cout << ", ";
        }
    }
    std::cout << std::endl;
    
    // Create initial implicants
    for (int m : minterms) {
        std::string binary = intToBinary(m, num_vars);
        current_implicants.push_back(Implicant(binary, m));
    }
    
    for (int dc : dont_cares) {
        std::string binary = intToBinary(dc, num_vars);
        current_implicants.push_back(Implicant(binary, dc));
    }
    
    // Iterative combination
    int iteration = 0;
    std::vector<Implicant> all_prime_implicants;
    
    while (true) {
        iteration++;
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "🔄 Column " << iteration << " - Grouping by number of 1's" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        // Group by number of 1's
        auto groups = groupByOnes(current_implicants);
        
        // Print each group
        for (size_t i = 0; i < groups.size(); ++i) {
            if (groups[i].empty()) continue;
            
            std::cout << "\nGroup " << i << " (has " << i << " ones):" << std::endl;
            std::cout << std::string(60, '-') << std::endl;
            
            for (const auto& imp : groups[i]) {
                std::cout << "  " << imp.value 
                         << "  (m" << imp.mintermsToString() << ")" << std::endl;
            }
        }
        
        // Try combinations
        std::cout << "\n🔀 Attempting combinations..." << std::endl;
        std::vector<Implicant> new_implicants = combineGroups(groups);
        
        if (new_implicants.empty()) {
            std::cout << "   ❌ No more combinations possible." << std::endl;
            
            // All remaining are prime implicants
            std::cout << "\n✅ Prime Implicants found in this column:" << std::endl;
            for (auto& group : groups) {
                for (auto& imp : group) {
                    if (!imp.used) {
                        std::cout << "   • " << imp.value 
                                 << "  (m" << imp.mintermsToString() << ")" << std::endl;
                        all_prime_implicants.push_back(imp);
                    }
                }
            }
            break;
        }
        
        std::cout << "   ✓ Found " << new_implicants.size() << " new combinations:" << std::endl;
        for (const auto& imp : new_implicants) {
            std::cout << "      " << imp.value 
                     << "  (m" << imp.mintermsToString() << ")" << std::endl;
        }
        
        // Mark prime implicants (those that couldn't be combined)
        std::vector<Implicant> pis_this_round;
        for (auto& group : groups) {
            for (auto& imp : group) {
                if (!imp.used) {
                    pis_this_round.push_back(imp);
                }
            }
        }
        
        if (!pis_this_round.empty()) {
            std::cout << "\n✅ Prime Implicants found in this column:" << std::endl;
            for (const auto& pi : pis_this_round) {
                std::cout << "   • " << pi.value 
                         << "  (m" << pi.mintermsToString() << ")" << std::endl;
                all_prime_implicants.push_back(pi);
            }
        }
        
        current_implicants = new_implicants;
    }
    
    // Final summary
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "🎯 FINAL PRIME IMPLICANTS" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    for (size_t i = 0; i < all_prime_implicants.size(); ++i) {
        std::cout << "PI" << (i + 1) << ": " << all_prime_implicants[i].value 
                 << "  covers m" << all_prime_implicants[i].mintermsToString() << std::endl;
    }
    
    std::cout << "\nTotal: " << all_prime_implicants.size() << " Prime Implicants" << std::endl;
}
