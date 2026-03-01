#!/usr/bin/env python3
"""
H++ Ontology Graph Visualizer
Generates visual representations of ontological structures and paradox detection
"""

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch
import networkx as nx
from typing import List, Tuple, Dict
import os

# Color scheme for H++ visualization
COLORS = {
    'UNDEFINED': '#CCCCCC',
    'DEFINED': '#4CAF50',
    'PARADOXICAL': '#F44336',
    'RECURSIVE': '#FF9800',
    'RESOLVED': '#2196F3',
    'CONTRADICTORY': '#9C27B0',
}

PARADOX_COLORS = {
    'NONE': '#4CAF50',
    'SELF_REFERENCE': '#FF5722',
    'CIRCULAR': '#E91E63',
    'INFINITE_REGRESS': '#9C27B0',
}


class HPPGraphVisualizer:
    """Visualizer for H++ Ontology Graphs"""
    
    def __init__(self, figsize=(14, 10)):
        self.figsize = figsize
        
    def create_identity_chain_graph(self, chain: List[str], 
                                     state: str = 'DEFINED',
                                     paradox_type: str = 'NONE',
                                     cycle_info: Tuple[int, int] = None,
                                     filename: str = 'identity_chain.pdf'):
        """
        Visualize an identity chain with paradox detection
        
        Args:
            chain: List of identity names
            state: OntologicalState as string
            paradox_type: ParadoxType as string
            cycle_info: Tuple of (cycle_start, cycle_length)
            filename: Output PDF filename
        """
        fig, ax = plt.subplots(figsize=self.figsize)
        
        # Create directed graph
        G = nx.DiGraph()
        
        # Add nodes and edges
        for i, identity in enumerate(chain):
            G.add_node(i, label=identity)
            if i > 0:
                G.add_edge(i-1, i)
        
        # Use spring layout for positioning
        pos = nx.spring_layout(G, k=2, iterations=50)
        
        # Adjust to horizontal layout if linear
        if cycle_info is None or cycle_info[1] == 0:
            pos = {i: (i * 2, 0) for i in range(len(chain))}
        
        # Node colors based on cycle detection
        node_colors = []
        if cycle_info and cycle_info[1] > 0:
            cycle_start, cycle_length = cycle_info
            cycle_end = cycle_start + cycle_length
            for i in range(len(chain)):
                if cycle_start <= i < cycle_end:
                    node_colors.append(COLORS['PARADOXICAL'])
                else:
                    node_colors.append(COLORS['DEFINED'])
        else:
            node_colors = [COLORS[state]] * len(chain)
        
        # Draw nodes
        nx.draw_networkx_nodes(G, pos, node_color=node_colors, 
                               node_size=2000, alpha=0.9, ax=ax)
        
        # Draw labels
        labels = {i: chain[i] for i in range(len(chain))}
        nx.draw_networkx_labels(G, pos, labels, font_size=10, 
                                font_weight='bold', ax=ax)
        
        # Draw edges
        nx.draw_networkx_edges(G, pos, edge_color='#333333', 
                               arrows=True, arrowsize=20, 
                               arrowstyle='->', width=2, ax=ax)
        
        # Draw cycle edge if exists
        if cycle_info and cycle_info[1] > 0:
            cycle_start, cycle_length = cycle_info
            if cycle_length > 0 and cycle_start + cycle_length < len(chain):
                cycle_end = cycle_start + cycle_length
                ax.annotate('', xy=pos[cycle_start], xytext=pos[cycle_end-1],
                           arrowprops=dict(arrowstyle='->', lw=3, 
                                         color=COLORS['PARADOXICAL'],
                                         linestyle='dashed'))
        
        # Title and info
        title = f"H++ Identity Chain: {' → '.join(chain[:5])}"
        if len(chain) > 5:
            title += "..."
        ax.set_title(title, fontsize=16, fontweight='bold', pad=20)
        
        # Add state information box
        info_text = f"State: {state}\nParadox Type: {paradox_type}\n"
        info_text += f"Chain Length: {len(chain)}\n"
        if cycle_info and cycle_info[1] > 0:
            info_text += f"Cycle: position {cycle_info[0]}, length {cycle_info[1]}"
        
        props = dict(boxstyle='round', facecolor=COLORS.get(state, '#FFFFFF'), 
                    alpha=0.3, edgecolor='black', linewidth=2)
        ax.text(0.02, 0.98, info_text, transform=ax.transAxes,
               fontsize=11, verticalalignment='top', bbox=props,
               family='monospace')
        
        # Legend
        legend_elements = [
            mpatches.Patch(color=COLORS['DEFINED'], label='Defined'),
            mpatches.Patch(color=COLORS['PARADOXICAL'], label='Paradoxical'),
            mpatches.Patch(color=COLORS['RESOLVED'], label='Resolved'),
        ]
        ax.legend(handles=legend_elements, loc='lower right', fontsize=10)
        
        ax.axis('off')
        plt.tight_layout()
        plt.savefig(filename, format='pdf', dpi=300, bbox_inches='tight')
        print(f"✓ Saved: {filename}")
        plt.close()
        
    def create_ontology_graph(self, entities: List[str], 
                               relations: List[Tuple[str, str]],
                               entity_types: Dict[str, str] = None,
                               filename: str = 'ontology_graph.pdf'):
        """
        Visualize complete ontology graph with entities and relations
        
        Args:
            entities: List of entity names
            relations: List of (from, to) tuples
            entity_types: Optional dict mapping entity to type
            filename: Output PDF filename
        """
        fig, ax = plt.subplots(figsize=self.figsize)
        
        # Create directed graph
        G = nx.DiGraph()
        G.add_nodes_from(entities)
        G.add_edges_from(relations)
        
        # Detect cycles
        try:
            cycles = list(nx.simple_cycles(G))
            has_cycles = len(cycles) > 0
        except:
            cycles = []
            has_cycles = False
        
        # Layout
        if has_cycles:
            pos = nx.spring_layout(G, k=3, iterations=100, seed=42)
        else:
            try:
                pos = nx.planar_layout(G)
            except:
                pos = nx.spring_layout(G, k=3, iterations=100, seed=42)
        
        # Node colors based on type or cycle membership
        node_colors = []
        nodes_in_cycles = set()
        for cycle in cycles:
            nodes_in_cycles.update(cycle)
        
        for node in G.nodes():
            if node in nodes_in_cycles:
                node_colors.append(COLORS['PARADOXICAL'])
            else:
                node_colors.append(COLORS['DEFINED'])
        
        # Draw nodes
        nx.draw_networkx_nodes(G, pos, node_color=node_colors,
                               node_size=3000, alpha=0.9, ax=ax,
                               edgecolors='black', linewidths=2)
        
        # Draw labels with types if available
        if entity_types:
            labels = {e: f"{e}\n({entity_types.get(e, 'entity')})" 
                     for e in entities}
        else:
            labels = {e: e for e in entities}
        
        nx.draw_networkx_labels(G, pos, labels, font_size=9,
                                font_weight='bold', ax=ax)
        
        # Draw edges
        nx.draw_networkx_edges(G, pos, edge_color='#333333',
                               arrows=True, arrowsize=25,
                               arrowstyle='->', width=2.5,
                               connectionstyle='arc3,rad=0.1', ax=ax)
        
        # Highlight cycle edges
        if cycles:
            for cycle in cycles:
                cycle_edges = [(cycle[i], cycle[(i+1) % len(cycle)]) 
                              for i in range(len(cycle))]
                nx.draw_networkx_edges(G, pos, edgelist=cycle_edges,
                                      edge_color=COLORS['PARADOXICAL'],
                                      width=3, style='dashed',
                                      arrows=True, arrowsize=25,
                                      connectionstyle='arc3,rad=0.1', ax=ax)
        
        # Title
        title = "H++ Ontology Graph"
        if has_cycles:
            title += f" - {len(cycles)} Cycle(s) Detected"
        ax.set_title(title, fontsize=16, fontweight='bold', pad=20)
        
        # Info box
        info_text = f"Entities: {len(entities)}\n"
        info_text += f"Relations: {len(relations)}\n"
        info_text += f"Cycles: {len(cycles)}\n"
        if cycles:
            info_text += f"Paradoxical: {len(nodes_in_cycles)} nodes"
        
        props = dict(boxstyle='round', 
                    facecolor=COLORS['PARADOXICAL'] if has_cycles else COLORS['DEFINED'],
                    alpha=0.3, edgecolor='black', linewidth=2)
        ax.text(0.02, 0.98, info_text, transform=ax.transAxes,
               fontsize=11, verticalalignment='top', bbox=props,
               family='monospace')
        
        # Legend
        legend_elements = [
            mpatches.Patch(color=COLORS['DEFINED'], label='Normal Entity'),
            mpatches.Patch(color=COLORS['PARADOXICAL'], label='In Cycle'),
        ]
        ax.legend(handles=legend_elements, loc='lower right', fontsize=10)
        
        ax.axis('off')
        plt.tight_layout()
        plt.savefig(filename, format='pdf', dpi=300, bbox_inches='tight')
        print(f"✓ Saved: {filename}")
        plt.close()
        
    def create_three_valued_logic_table(self, filename: str = 'logic_table.pdf'):
        """Create visual truth table for three-valued logic"""
        fig, axes = plt.subplots(1, 3, figsize=(15, 5))
        
        values = ['TRUE', 'FALSE', 'PARADOX']
        colors_map = {'TRUE': '#4CAF50', 'FALSE': '#F44336', 'PARADOX': '#FF9800'}
        
        # NOT operation
        ax = axes[0]
        not_results = {'TRUE': 'FALSE', 'FALSE': 'TRUE', 'PARADOX': 'PARADOX'}
        y_pos = [2, 1, 0]
        ax.barh(y_pos, [1]*3, color=[colors_map[not_results[v]] for v in values],
                edgecolor='black', linewidth=2, alpha=0.7)
        ax.set_yticks(y_pos)
        ax.set_yticklabels(values)
        ax.set_xlabel('NOT', fontsize=12, fontweight='bold')
        ax.set_xlim(0, 1.2)
        ax.set_title('NOT Operation', fontsize=14, fontweight='bold')
        
        # Add result labels
        for i, (v, result) in enumerate(not_results.items()):
            ax.text(1.05, y_pos[i], result, va='center', fontweight='bold', fontsize=10)
        
        # AND operation matrix
        ax = axes[1]
        and_matrix = [
            ['TRUE', 'FALSE', 'PARADOX'],
            ['FALSE', 'FALSE', 'PARADOX'],
            ['PARADOX', 'PARADOX', 'PARADOX']
        ]
        
        for i, row in enumerate(and_matrix):
            for j, cell in enumerate(row):
                color = colors_map[cell]
                rect = mpatches.Rectangle((j, 2-i), 1, 1, 
                                         facecolor=color, edgecolor='black',
                                         linewidth=2, alpha=0.7)
                ax.add_patch(rect)
                ax.text(j+0.5, 2-i+0.5, cell, ha='center', va='center',
                       fontweight='bold', fontsize=9)
        
        ax.set_xlim(0, 3)
        ax.set_ylim(0, 3)
        ax.set_xticks([0.5, 1.5, 2.5])
        ax.set_xticklabels(values, fontsize=10)
        ax.set_yticks([0.5, 1.5, 2.5])
        ax.set_yticklabels(values[::-1], fontsize=10)
        ax.set_title('AND Operation', fontsize=14, fontweight='bold')
        ax.set_xlabel('Right Operand', fontsize=11)
        ax.set_ylabel('Left Operand', fontsize=11)
        ax.set_aspect('equal')
        
        # OR operation matrix
        ax = axes[2]
        or_matrix = [
            ['TRUE', 'TRUE', 'PARADOX'],
            ['TRUE', 'FALSE', 'PARADOX'],
            ['PARADOX', 'PARADOX', 'PARADOX']
        ]
        
        for i, row in enumerate(or_matrix):
            for j, cell in enumerate(row):
                color = colors_map[cell]
                rect = mpatches.Rectangle((j, 2-i), 1, 1,
                                         facecolor=color, edgecolor='black',
                                         linewidth=2, alpha=0.7)
                ax.add_patch(rect)
                ax.text(j+0.5, 2-i+0.5, cell, ha='center', va='center',
                       fontweight='bold', fontsize=9)
        
        ax.set_xlim(0, 3)
        ax.set_ylim(0, 3)
        ax.set_xticks([0.5, 1.5, 2.5])
        ax.set_xticklabels(values, fontsize=10)
        ax.set_yticks([0.5, 1.5, 2.5])
        ax.set_yticklabels(values[::-1], fontsize=10)
        ax.set_title('OR Operation', fontsize=14, fontweight='bold')
        ax.set_xlabel('Right Operand', fontsize=11)
        ax.set_ylabel('Left Operand', fontsize=11)
        ax.set_aspect('equal')
        
        fig.suptitle('H++ Three-Valued Logic Operations', 
                    fontsize=16, fontweight='bold', y=1.02)
        
        plt.tight_layout()
        plt.savefig(filename, format='pdf', dpi=300, bbox_inches='tight')
        print(f"✓ Saved: {filename}")
        plt.close()
        
    def create_comprehensive_diagram(self, filename: str = 'hpp_comprehensive.pdf'):
        """Create comprehensive H++ system diagram"""
        fig = plt.figure(figsize=(16, 12))
        gs = fig.add_gridspec(3, 2, hspace=0.3, wspace=0.3)
        
        # Example 1: Circular Reference
        ax1 = fig.add_subplot(gs[0, 0])
        G1 = nx.DiGraph()
        chain1 = ['A', 'B', 'C', 'A']
        for i in range(len(chain1)-1):
            G1.add_edge(chain1[i], chain1[i+1])
        pos1 = nx.circular_layout(G1)
        nx.draw_networkx(G1, pos1, node_color=COLORS['PARADOXICAL'],
                        node_size=2000, font_weight='bold', ax=ax1,
                        arrows=True, arrowsize=20, edge_color='#333')
        ax1.set_title('Circular Reference\n(A → B → C → A)', 
                     fontsize=12, fontweight='bold')
        ax1.axis('off')
        
        # Example 2: Self-Reference
        ax2 = fig.add_subplot(gs[0, 1])
        G2 = nx.DiGraph()
        G2.add_edge('Self', 'Self')
        pos2 = {'Self': (0.5, 0.5)}
        nx.draw_networkx_nodes(G2, pos2, node_color=COLORS['PARADOXICAL'],
                               node_size=3000, ax=ax2)
        nx.draw_networkx_labels(G2, pos2, {'Self': 'Self'}, 
                               font_weight='bold', font_size=12, ax=ax2)
        
        # Draw self-loop
        circle = mpatches.FancyArrowPatch((0.6, 0.5), (0.4, 0.5),
                                         connectionstyle="arc3,rad=1.5",
                                         arrowstyle='->', mutation_scale=30,
                                         linewidth=3, color='#333')
        ax2.add_patch(circle)
        ax2.set_title('Self-Reference\n(Reflexive → Reflexive)',
                     fontsize=12, fontweight='bold')
        ax2.set_xlim(0, 1)
        ax2.set_ylim(0, 1)
        ax2.axis('off')
        
        # Example 3: Ontological Graph
        ax3 = fig.add_subplot(gs[1, :])
        G3 = nx.DiGraph()
        G3.add_edges_from([
            ('Being', 'Existence'),
            ('Existence', 'Essence'),
            ('Essence', 'Being'),
            ('Being', 'Reality'),
            ('Reality', 'Truth')
        ])
        pos3 = nx.spring_layout(G3, k=2, iterations=50, seed=42)
        
        cycle_nodes = ['Being', 'Existence', 'Essence']
        node_colors3 = [COLORS['PARADOXICAL'] if n in cycle_nodes 
                       else COLORS['DEFINED'] for n in G3.nodes()]
        
        nx.draw_networkx(G3, pos3, node_color=node_colors3,
                        node_size=2500, font_weight='bold', ax=ax3,
                        arrows=True, arrowsize=25, edge_color='#333',
                        width=2.5, font_size=10)
        
        # Highlight cycle
        cycle_edges = [('Being', 'Existence'), ('Existence', 'Essence'),
                      ('Essence', 'Being')]
        nx.draw_networkx_edges(G3, pos3, edgelist=cycle_edges,
                              edge_color=COLORS['PARADOXICAL'],
                              width=3, style='dashed', arrows=True,
                              arrowsize=25, ax=ax3)
        
        ax3.set_title('Ontological Graph with Cycle Detection',
                     fontsize=14, fontweight='bold')
        ax3.axis('off')
        
        # State diagram
        ax4 = fig.add_subplot(gs[2, 0])
        states = ['UNDEFINED', 'DEFINED', 'PARADOXICAL', 'RESOLVED']
        state_colors = [COLORS[s] for s in states]
        
        y_pos = range(len(states))
        ax4.barh(y_pos, [1]*len(states), color=state_colors,
                edgecolor='black', linewidth=2, alpha=0.8)
        ax4.set_yticks(y_pos)
        ax4.set_yticklabels(states, fontweight='bold')
        ax4.set_xlabel('Ontological States', fontsize=11, fontweight='bold')
        ax4.set_title('H++ State System', fontsize=12, fontweight='bold')
        ax4.set_xlim(0, 1.2)
        
        # Paradox types
        ax5 = fig.add_subplot(gs[2, 1])
        paradoxes = ['SELF_REF', 'CIRCULAR', 'INFINITE', 'NONE']
        paradox_colors = [PARADOX_COLORS.get(k.split('_')[0]+'_REFERENCE' 
                          if k == 'SELF_REF' else k+'_REGRESS' 
                          if k == 'INFINITE' else k, '#4CAF50') 
                         for k in paradoxes]
        
        y_pos = range(len(paradoxes))
        ax5.barh(y_pos, [1]*len(paradoxes), color=paradox_colors,
                edgecolor='black', linewidth=2, alpha=0.8)
        ax5.set_yticks(y_pos)
        ax5.set_yticklabels(paradoxes, fontweight='bold')
        ax5.set_xlabel('Paradox Types', fontsize=11, fontweight='bold')
        ax5.set_title('H++ Paradox Classification', fontsize=12, fontweight='bold')
        ax5.set_xlim(0, 1.2)
        
        fig.suptitle('H++ Recursive Ontology Engine - Comprehensive Overview',
                    fontsize=18, fontweight='bold', y=0.98)
        
        plt.savefig(filename, format='pdf', dpi=300, bbox_inches='tight')
        print(f"✓ Saved: {filename}")
        plt.close()


def main():
    """Generate all H++ visualization diagrams"""
    print("\n" + "="*60)
    print("H++ Ontology Graph Visualizer")
    print("="*60 + "\n")
    
    visualizer = HPPGraphVisualizer()
    
    # Example 1: Circular reference chain
    print("Generating Example 1: Circular Reference...")
    visualizer.create_identity_chain_graph(
        chain=['A', 'B', 'C', 'A'],
        state='PARADOXICAL',
        paradox_type='CIRCULAR',
        cycle_info=(0, 3),
        filename='example1_circular.pdf'
    )
    
    # Example 2: Self-reference chain
    print("Generating Example 2: Self-Reference...")
    visualizer.create_identity_chain_graph(
        chain=['Reflexive', 'Reflexive'],
        state='PARADOXICAL',
        paradox_type='SELF_REFERENCE',
        cycle_info=(0, 1),
        filename='example2_self_reference.pdf'
    )
    
    # Example 3: Normal chain (no paradox)
    print("Generating Example 3: Normal Chain...")
    visualizer.create_identity_chain_graph(
        chain=['Start', 'Middle', 'End'],
        state='DEFINED',
        paradox_type='NONE',
        filename='example3_normal_chain.pdf'
    )
    
    # Example 4: Ontology graph
    print("Generating Example 4: Ontology Graph...")
    visualizer.create_ontology_graph(
        entities=['Being', 'Existence', 'Essence', 'Reality', 'Truth'],
        relations=[
            ('Being', 'Existence'),
            ('Existence', 'Essence'),
            ('Essence', 'Being'),
            ('Being', 'Reality'),
            ('Reality', 'Truth')
        ],
        entity_types={
            'Being': 'ontological',
            'Existence': 'ontological',
            'Essence': 'ontological',
            'Reality': 'ontological',
            'Truth': 'concept'
        },
        filename='example4_ontology_graph.pdf'
    )
    
    # Example 5: Three-valued logic
    print("Generating Example 5: Three-Valued Logic Table...")
    visualizer.create_three_valued_logic_table(
        filename='example5_logic_table.pdf'
    )
    
    # Example 6: Comprehensive diagram
    print("Generating Example 6: Comprehensive Overview...")
    visualizer.create_comprehensive_diagram(
        filename='example6_comprehensive.pdf'
    )
    
    print("\n" + "="*60)
    print("✓ All diagrams generated successfully!")
    print("="*60 + "\n")
    
    print("Generated files:")
    files = [
        'example1_circular.pdf',
        'example2_self_reference.pdf',
        'example3_normal_chain.pdf',
        'example4_ontology_graph.pdf',
        'example5_logic_table.pdf',
        'example6_comprehensive.pdf'
    ]
    for f in files:
        print(f"  • {f}")
    print()


if __name__ == '__main__':
    main()
