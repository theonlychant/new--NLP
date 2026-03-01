# H++ Graph Visualizer

## Overview
Python script for visualizing H++ ontology graphs, identity chains, and paradox detection.

## Installation

```bash
pip install -r requirements.txt
```

## Usage

### Generate All Diagrams
```bash
python visualizer.py
```

This generates 6 PDF files:
1. `example1_circular.pdf` - Circular reference detection
2. `example2_self_reference.pdf` - Self-reference paradox
3. `example3_normal_chain.pdf` - Normal identity chain
4. `example4_ontology_graph.pdf` - Complete ontology graph
5. `example5_logic_table.pdf` - Three-valued logic tables
6. `example6_comprehensive.pdf` - Comprehensive overview

### Custom Visualizations

#### Identity Chain
```python
from visualizer import HPPGraphVisualizer

viz = HPPGraphVisualizer()
viz.create_identity_chain_graph(
    chain=['A', 'B', 'C'],
    state='DEFINED',
    paradox_type='NONE',
    filename='my_chain.pdf'
)
```

#### Ontology Graph
```python
viz.create_ontology_graph(
    entities=['Entity1', 'Entity2', 'Entity3'],
    relations=[('Entity1', 'Entity2'), ('Entity2', 'Entity3')],
    entity_types={'Entity1': 'concept', 'Entity2': 'ontological'},
    filename='my_graph.pdf'
)
```

#### Three-Valued Logic Table
```python
viz.create_three_valued_logic_table(filename='logic.pdf')
```

## Features

- **Identity Chain Visualization**: Shows chain progression with paradox highlighting
- **Cycle Detection**: Automatically highlights circular references
- **Ontology Graphs**: Full graph visualization with type information
- **Logic Tables**: Visual truth tables for three-valued logic
- **Color Coding**: States and paradox types color-coded
- **PDF Output**: High-quality vector graphics

## Color Scheme

### Ontological States
- Gray: UNDEFINED
- Green: DEFINED
- Red: PARADOXICAL
- Orange: RECURSIVE
- Blue: RESOLVED
- Purple: CONTRADICTORY

### Paradox Types
- Green: NONE
- Orange-Red: SELF_REFERENCE
- Pink: CIRCULAR
- Purple: INFINITE_REGRESS

## Requirements

- Python 3.7+
- matplotlib
- networkx
- numpy
