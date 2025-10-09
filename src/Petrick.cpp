#include "Petrick.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <climits>

// ==================== PrimeImplicantChart Implementation ====================

void PrimeImplicantChart::build(const std::vector<Implicant>& pis, 
                                 const std::vector<int>& minterms,
                                 const std::vector<int>& dont_cares) {
    prime_implicants = pis;
    
    // Step 1: Filter out only on-set minterms (exclude don't cares)
    // Don't cares are used in QM to simplify but don't need to be covered
    std::set<int> dc_set(dont_cares.begin(), dont_cares.end());
    
    for (int m : minterms) {
        if (dc_set.find(m) == dc_set.end()) {
            on_set_minterms.push_back(m);
        }
    }
    
    // Remove duplicates and sort
    std::sort(on_set_minterms.begin(), on_set_minterms.end());
    on_set_minterms.erase(std::unique(on_set_minterms.begin(), on_set_minterms.end()), 
                          on_set_minterms.end());
    
    // Build minterm to index mapping
    for (size_t i = 0; i < on_set_minterms.size(); i++) {
        minterm_to_index[on_set_minterms[i]] = i;
    }
    
    // Step 2: Build coverage matrix
    // coverage[i][j] = true if PI[i] covers minterm[j]
    coverage.resize(pis.size());
    for (size_t i = 0; i < pis.size(); i++) {
        coverage[i].resize(on_set_minterms.size(), false);
        
        for (size_t j = 0; j < on_set_minterms.size(); j++) {
            int minterm = on_set_minterms[j];
            if (pis[i].coversMinterm(minterm)) {
                coverage[i][j] = true;
            }
        }
    }
}

std::vector<EssentialPIInfo> PrimeImplicantChart::findEssentialPIs() const {
    std::vector<EssentialPIInfo> essential_pis;
    
    // For each minterm (column), count how many PIs cover it
    for (size_t j = 0; j < on_set_minterms.size(); j++) {
        int minterm = on_set_minterms[j];
        
        // Find all PIs that cover this minterm
        std::vector<int> covering_pis;
        for (size_t i = 0; i < prime_implicants.size(); i++) {
            if (coverage[i][j]) {
                covering_pis.push_back(i);
            }
        }
        
        // If only one PI covers this minterm, it's essential
        if (covering_pis.size() == 1) {
            int essential_pi = covering_pis[0];
            
            // Check if already recorded
            bool already_recorded = false;
            for (auto& epi : essential_pis) {
                if (epi.pi_index == essential_pi) {
                    epi.unique_minterms.push_back(minterm);
                    already_recorded = true;
                    break;
                }
            }
            
            if (!already_recorded) {
                EssentialPIInfo info;
                info.pi_index = essential_pi;
                info.unique_minterms.push_back(minterm);
                essential_pis.push_back(info);
            }
        }
    }
    
    return essential_pis;
}

std::vector<int> PrimeImplicantChart::getRemainingMinterms(
    const std::vector<int>& covered_pi_indices) const {
    
    std::vector<bool> covered(on_set_minterms.size(), false);
    
    // Mark minterms covered by given PIs
    for (int pi_idx : covered_pi_indices) {
        for (size_t j = 0; j < on_set_minterms.size(); j++) {
            if (coverage[pi_idx][j]) {
                covered[j] = true;
            }
        }
    }
    
    // Collect uncovered minterms
    std::vector<int> remaining;
    for (size_t j = 0; j < on_set_minterms.size(); j++) {
        if (!covered[j]) {
            remaining.push_back(on_set_minterms[j]);
        }
    }
    
    return remaining;
}

bool PrimeImplicantChart::covers(int pi_index, int minterm) const {
    auto it = minterm_to_index.find(minterm);
    if (it == minterm_to_index.end()) {
        return false;
    }
    return coverage[pi_index][it->second];
}

std::vector<int> PrimeImplicantChart::getPIsCovering(int minterm) const {
    std::vector<int> result;
    auto it = minterm_to_index.find(minterm);
    if (it == minterm_to_index.end()) {
        return result;
    }
    
    int mt_idx = it->second;
    for (size_t i = 0; i < prime_implicants.size(); i++) {
        if (coverage[i][mt_idx]) {
            result.push_back(i);
        }
    }
    
    return result;
}

void PrimeImplicantChart::print() const {
    std::cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          Prime Implicant Coverage Chart                      ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    // Print header (minterms)
    std::cout << "        ";
    for (int m : on_set_minterms) {
        std::cout << " m" << std::setw(2) << m;
    }
    std::cout << "\n        ";
    for (size_t i = 0; i < on_set_minterms.size(); i++) {
        std::cout << "----";
    }
    std::cout << "\n";
    
    // Print each PI row
    for (size_t i = 0; i < prime_implicants.size(); i++) {
        std::cout << "PI" << std::setw(2) << (i+1) << " " 
                  << prime_implicants[i].value << " |";
        
        for (size_t j = 0; j < on_set_minterms.size(); j++) {
            if (coverage[i][j]) {
                std::cout << "  ✓ ";
            } else {
                std::cout << "  · ";
            }
        }
        
        std::cout << " | m" << prime_implicants[i].mintermsToString() << "\n";
    }
    std::cout << "\n";
}

// ==================== PetrickSolver Implementation ====================

void PetrickSolver::solve(const std::vector<Implicant>& pis, 
                          const std::vector<int>& minterms,
                          const std::vector<int>& dont_cares) {
    essential_pi_indices.clear();
    additional_pi_indices.clear();
    
    // Step 1: Build coverage chart
    std::cout << "\n[Step 4] Petrick's Algorithm\n";
    chart.build(pis, minterms, dont_cares);
    chart.print();
    
    // Step 2: Find Essential Prime Implicants
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          Finding Essential Prime Implicants                   ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    auto essential_infos = chart.findEssentialPIs();
    
    if (essential_infos.empty()) {
        std::cout << "❌ No Essential Prime Implicants found.\n";
        std::cout << "   Every minterm is covered by multiple PIs.\n\n";
    } else {
        std::cout << "✅ Found " << essential_infos.size() << " Essential Prime Implicant(s):\n\n";
        
        for (const auto& epi : essential_infos) {
            essential_pi_indices.push_back(epi.pi_index);
            const Implicant& pi = chart.getPrimeImplicants()[epi.pi_index];
            
            std::cout << "   PI" << (epi.pi_index + 1) << ": " << pi.value << " is ESSENTIAL\n";
            std::cout << "      Reason: Only PI covering ";
            
            for (size_t i = 0; i < epi.unique_minterms.size(); i++) {
                std::cout << "m" << epi.unique_minterms[i];
                if (i < epi.unique_minterms.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
            std::cout << "      Total coverage: m" << pi.mintermsToString() << "\n\n";
        }
    }
    
    // Step 3: Check remaining minterms
    std::vector<int> remaining = chart.getRemainingMinterms(essential_pi_indices);
    
    if (remaining.empty()) {
        std::cout << "🎉 All minterms covered by Essential PIs!\n";
        std::cout << "   No need for Petrick's Method.\n";
        return;
    }
    
    // Step 4: Apply Petrick's Method
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          Applying Petrick's Method                            ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "Remaining uncovered minterms: ";
    for (size_t i = 0; i < remaining.size(); i++) {
        std::cout << "m" << remaining[i];
        if (i < remaining.size() - 1) std::cout << ", ";
    }
    std::cout << "\n\n";
    
    // Build and solve Petrick's function
    PetrickSOP petrick = buildPetrickFunction(remaining);
    printDetailedSteps(remaining, petrick);
    
    // Simplify
    simplify(petrick);
    
    // Select minimal cover
    PetrickTerm minimal = selectMinimalCover(petrick);
    additional_pi_indices = std::vector<int>(minimal.begin(), minimal.end());
}

PetrickSOP PetrickSolver::buildPetrickFunction(const std::vector<int>& remaining_minterms) {
    PetrickSOP result;
    
    std::cout << "Building Petrick's function P:\n\n";
    std::cout << "P = ";
    
    bool first_minterm = true;
    
    for (int minterm : remaining_minterms) {
        // Get all PIs covering this minterm
        std::vector<int> covering_pis = chart.getPIsCovering(minterm);
        
        // Print clause
        if (!first_minterm) {
            std::cout << " · ";
        }
        std::cout << "(";
        for (size_t i = 0; i < covering_pis.size(); i++) {
            std::cout << "PI" << (covering_pis[i] + 1);
            if (i < covering_pis.size() - 1) std::cout << "+";
        }
        std::cout << ")";
        
        // Convert to PetrickTerm (clause)
        PetrickTerm clause;
        for (int pi : covering_pis) {
            clause.insert(pi);
        }
        
        // Multiply into result
        if (result.empty()) {
            // First clause: initialize
            for (int pi : clause) {
                PetrickTerm term;
                term.insert(pi);
                result.push_back(term);
            }
        } else {
            // Subsequent clauses: multiply
            result = multiply(result, clause);
        }
        
        first_minterm = false;
    }
    
    std::cout << "\n\n";
    
    return result;
}

PetrickSOP PetrickSolver::multiply(const PetrickSOP& sop, const PetrickTerm& clause) {
    PetrickSOP result;
    
    // For each existing product term
    for (const auto& existing_term : sop) {
        // Multiply with each PI in the clause
        for (int pi : clause) {
            PetrickTerm new_term = existing_term;
            new_term.insert(pi);  // Set automatically handles duplicates
            
            // Check if already exists
            bool exists = false;
            for (const auto& r : result) {
                if (r == new_term) {
                    exists = true;
                    break;
                }
            }
            
            if (!exists) {
                result.push_back(new_term);
            }
        }
    }
    
    return result;
}

void PetrickSolver::simplify(PetrickSOP& sop) {
    // Remove duplicate terms
    std::sort(sop.begin(), sop.end());
    sop.erase(std::unique(sop.begin(), sop.end()), sop.end());
    
    // Apply absorption law: A + AB = A
    // Remove terms that are supersets of other terms
    PetrickSOP simplified;
    
    for (size_t i = 0; i < sop.size(); i++) {
        bool is_absorbed = false;
        
        for (size_t j = 0; j < sop.size(); j++) {
            if (i != j) {
                // If sop[j] is a subset of sop[i], then sop[i] is absorbed
                if (std::includes(sop[i].begin(), sop[i].end(),
                                 sop[j].begin(), sop[j].end()) && 
                    sop[j].size() < sop[i].size()) {
                    is_absorbed = true;
                    break;
                }
            }
        }
        
        if (!is_absorbed) {
            simplified.push_back(sop[i]);
        }
    }
    
    sop = simplified;
}

PetrickTerm PetrickSolver::selectMinimalCover(const PetrickSOP& solutions) {
    if (solutions.empty()) {
        return PetrickTerm();
    }
    
    // Find minimum number of PIs
    size_t min_pis = solutions[0].size();
    for (const auto& sol : solutions) {
        if (sol.size() < min_pis) {
            min_pis = sol.size();
        }
    }
    
    // Collect all solutions with minimum PIs
    std::vector<PetrickTerm> candidates;
    for (const auto& sol : solutions) {
        if (sol.size() == min_pis) {
            candidates.push_back(sol);
        }
    }
    
    std::cout << "Simplified solutions:\n";
    for (size_t i = 0; i < candidates.size(); i++) {
        std::cout << "   Solution " << (i+1) << ": ";
        bool first = true;
        for (int pi : candidates[i]) {
            if (!first) std::cout << " + ";
            std::cout << "PI" << (pi + 1);
            first = false;
        }
        std::cout << "  (" << candidates[i].size() << " PIs, " 
                  << countLiterals(candidates[i]) << " literals)\n";
    }
    std::cout << "\n";
    
    // If multiple candidates, choose one with fewest literals
    if (candidates.size() > 1) {
        int min_literals = INT_MAX;
        PetrickTerm best = candidates[0];
        
        for (const auto& sol : candidates) {
            int lit_count = countLiterals(sol);
            if (lit_count < min_literals) {
                min_literals = lit_count;
                best = sol;
            }
        }
        
        std::cout << "Selected solution with fewest literals: ";
        bool first = true;
        for (int pi : best) {
            if (!first) std::cout << " + ";
            std::cout << "PI" << (pi + 1);
            first = false;
        }
        std::cout << "\n\n";
        
        return best;
    }
    
    return candidates[0];
}

int PetrickSolver::countLiterals(const PetrickTerm& pis) const {
    int count = 0;
    const auto& all_pis = chart.getPrimeImplicants();
    
    for (int pi_idx : pis) {
        const std::string& value = all_pis[pi_idx].value;
        for (char c : value) {
            if (c != '-') {
                count++;
            }
        }
    }
    
    return count;
}

std::vector<int> PetrickSolver::getAllSelectedPIs() const {
    std::vector<int> all = essential_pi_indices;
    all.insert(all.end(), additional_pi_indices.begin(), additional_pi_indices.end());
    std::sort(all.begin(), all.end());
    return all;
}

std::vector<Implicant> PetrickSolver::getMinimalCover() const {
    std::vector<Implicant> result;
    auto all_indices = getAllSelectedPIs();
    const auto& all_pis = chart.getPrimeImplicants();
    
    for (int idx : all_indices) {
        result.push_back(all_pis[idx]);
    }
    
    return result;
}

void PetrickSolver::printDetailedSteps(const std::vector<int>& remaining_minterms,
                                       const PetrickSOP& petrick_function) {
    std::cout << "Expanding Petrick's function:\n";
    std::cout << "After multiplication, we get " << petrick_function.size() << " product term(s):\n\n";
    
    for (size_t i = 0; i < std::min(petrick_function.size(), size_t(10)); i++) {
        std::cout << "   ";
        bool first = true;
        for (int pi : petrick_function[i]) {
            if (!first) std::cout << "·";
            std::cout << "PI" << (pi + 1);
            first = false;
        }
        std::cout << "\n";
    }
    
    if (petrick_function.size() > 10) {
        std::cout << "   ... (" << (petrick_function.size() - 10) << " more)\n";
    }
    std::cout << "\n";
}

void PetrickSolver::printSolution() const {
    std::cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          FINAL MINIMAL COVER                                  ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    auto all_indices = getAllSelectedPIs();
    const auto& all_pis = chart.getPrimeImplicants();
    
    std::cout << "Selected Prime Implicants (" << all_indices.size() << " total):\n\n";
    
    if (!essential_pi_indices.empty()) {
        std::cout << "Essential PIs:\n";
        for (int idx : essential_pi_indices) {
            std::cout << "   PI" << (idx + 1) << ": " << all_pis[idx].value 
                     << "  (m" << all_pis[idx].mintermsToString() << ")\n";
        }
        std::cout << "\n";
    }
    
    if (!additional_pi_indices.empty()) {
        std::cout << "Additional PIs (from Petrick's Method):\n";
        for (int idx : additional_pi_indices) {
            std::cout << "   PI" << (idx + 1) << ": " << all_pis[idx].value 
                     << "  (m" << all_pis[idx].mintermsToString() << ")\n";
        }
        std::cout << "\n";
    }
    
    // Calculate total literals
    int total_literals = countLiterals(PetrickTerm(all_indices.begin(), all_indices.end()));
    std::cout << "Total: " << all_indices.size() << " PIs, " 
              << total_literals << " literals\n";
}
