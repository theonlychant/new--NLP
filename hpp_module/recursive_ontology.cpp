// High-Performance Recursive Ontology and Paradox Resolution Engine (H++)
// Plane 1: Ontological Logic Plane (Recursive/Paradox)
// Author: 1proprogrammerchant
// Implementation demonstrating H++ language capabilities

#include "recursive_ontology.hpp"
#include <iostream>

using namespace hpp;
using namespace hpp::logic;

int main() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║   H++ Recursive Ontology Engine - Demonstration      ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n\n";
    
    // ═══════════════════════════════════════════════════════════════
    // Example 1: Simple Circular Reference Detection
    // ═══════════════════════════════════════════════════════════════
    std::cout << "Example 1: Circular Reference Detection\n";
    std::cout << "─────────────────────────────────────────\n";
    
    IdentityChain chain1;
    chain1.add(Identity("A", "concept"));
    chain1.add(Identity("B", "concept"));
    chain1.add(Identity("C", "concept"));
    chain1.add(Identity("A", "concept")); // Creates circular reference
    chain1.print();
    chain1.resolve_paradox();
    std::cout << "After resolution: " << 
        (chain1.get_state() == OntologicalState::RESOLVED ? "RESOLVED" : "UNRESOLVED") 
        << "\n\n";
    
    // ═══════════════════════════════════════════════════════════════
    // Example 2: Self-Reference Detection
    // ═══════════════════════════════════════════════════════════════
    std::cout << "Example 2: Self-Reference Detection\n";
    std::cout << "─────────────────────────────────────────\n";
    
    IdentityChain chain2;
    chain2.add(Identity("Reflexive", "entity"));
    chain2.add(Identity("Reflexive", "entity")); // Self-reference
    chain2.print();
    std::cout << "\n";
    
    // ═══════════════════════════════════════════════════════════════
    // Example 3: Ontology Graph with Relations
    // ═══════════════════════════════════════════════════════════════
    std::cout << "Example 3: Ontology Graph Traversal\n";
    std::cout << "─────────────────────────────────────────\n";
    
    HPPEngine engine;
    
    // Register entities
    engine.register_entity(Identity("Being", "ontological"));
    engine.register_entity(Identity("Existence", "ontological"));
    engine.register_entity(Identity("Essence", "ontological"));
    engine.register_entity(Identity("Being", "ontological")); // Will create cycle
    
    // Register relations
    engine.register_relation("Being", "Existence");
    engine.register_relation("Existence", "Essence");
    engine.register_relation("Essence", "Being"); // Creates cycle
    
    // Execute trace
    auto chain3 = engine.execute_trace("Being", 10);
    chain3.print();
    
    engine.print_paradox_summary();
    
    // ═══════════════════════════════════════════════════════════════
    // Example 4: Three-Valued Logic Operations
    // ═══════════════════════════════════════════════════════════════
    std::cout << "Example 4: H++ Three-Valued Logic\n";
    std::cout << "─────────────────────────────────────────\n";
    
    auto t = TruthValue::TRUE;
    auto f = TruthValue::FALSE;
    auto p = TruthValue::PARADOX;
    
    std::cout << "TRUE AND FALSE = " << to_string(hpp_and(t, f)) << "\n";
    std::cout << "TRUE OR FALSE = " << to_string(hpp_or(t, f)) << "\n";
    std::cout << "NOT TRUE = " << to_string(hpp_not(t)) << "\n";
    std::cout << "TRUE AND PARADOX = " << to_string(hpp_and(t, p)) << "\n";
    std::cout << "NOT PARADOX = " << to_string(hpp_not(p)) << "\n";
    
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║   H++ Engine Execution Complete                       ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n\n";
    
    return 0;
}
