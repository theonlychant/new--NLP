package main

import (
    "encoding/json"
    "fmt"
    "sync"
)

type Node struct {
    ID    string                 `json:"id"`
    Label string                 `json:"label"`
    Data  map[string]interface{} `json:"data"`
}

type Edge struct {
    From string `json:"from"`
    To   string `json:"to"`
    Type string `json:"type"`
}

type Graph struct {
    nodes map[string]*Node
    edges []Edge
    mu    sync.RWMutex
}

func NewGraph() *Graph {
    return &Graph{
        nodes: make(map[string]*Node),
        edges: make([]Edge, 0),
    }
}

func (g *Graph) AddNode(id, label string) {
    g.mu.Lock()
    defer g.mu.Unlock()
    g.nodes[id] = &Node{
        ID:    id,
        Label: label,
        Data:  make(map[string]interface{}),
    }
}

func (g *Graph) AddEdge(from, to, edgeType string) {
    g.mu.Lock()
    defer g.mu.Unlock()
    g.edges = append(g.edges, Edge{From: from, To: to, Type: edgeType})
}

func (g *Graph) DetectCycles() [][]string {
    g.mu.RLock()
    defer g.mu.RUnlock()
    
    var cycles [][]string
    var wg sync.WaitGroup
    var mu sync.Mutex
    
    for nodeID := range g.nodes {
        wg.Add(1)
        go func(start string) {
            defer wg.Done()
            visited := make(map[string]bool)
            path := g.dfs(start, visited, []string{})
            if len(path) > 0 {
                mu.Lock()
                cycles = append(cycles, path)
                mu.Unlock()
            }
        }(nodeID)
    }
    
    wg.Wait()
    return cycles
}

func (g *Graph) dfs(current string, visited map[string]bool, path []string) []string {
    if visited[current] {
        return path
    }
    
    visited[current] = true
    path = append(path, current)
    
    for _, edge := range g.edges {
        if edge.From == current {
            result := g.dfs(edge.To, visited, make(map[string]bool))
            if len(result) > 0 {
                return result
            }
        }
    }
    
    return []string{}
}

func main() {
    graph := NewGraph()
    graph.AddNode("apple", "Apple Inc")
    graph.AddNode("google", "Google Inc")
    graph.AddNode("microsoft", "Microsoft")
    
    graph.AddEdge("apple", "google", "competes_with")
    graph.AddEdge("google", "microsoft", "competes_with")
    
    cycles := graph.DetectCycles()
    
    result := map[string]interface{}{
        "nodes":       len(graph.nodes),
        "edges":       len(graph.edges),
        "cycles":      cycles,
        "cycle_count": len(cycles),
        "status":      "success",
    }
    
    jsonBytes, _ := json.MarshalIndent(result, "", "  ")
    fmt.Println(string(jsonBytes))
}
