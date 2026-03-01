// Go Parallel Reference Validator (minimal prototype)
// Role: Concurrent referential integrity validation
package main

import (
	"fmt"
	"sync"
)

type Reference struct {
	Source int
	Target int
	Status string
}

func validateReference(ref *Reference, entityStates map[int]string, wg *sync.WaitGroup) {
	defer wg.Done()
	state, ok := entityStates[ref.Target]
	if !ok {
		ref.Status = "Invalidated"
	} else if state == "Split" {
		ref.Status = "Unresolved"
	} else {
		ref.Status = "Valid"
	}
}

func ValidatorDemo() {
	entityStates := map[int]string{1: "Defined", 2: "Defined", 3: "Split"}
	references := []Reference{{2, 1, ""}, {2, 3, ""}}
	var wg sync.WaitGroup
	for i := range references {
		wg.Add(1)
		go validateReference(&references[i], entityStates, &wg)
	}
	wg.Wait()
	for _, ref := range references {
		fmt.Printf("Reference %d -> %d: %s\n", ref.Source, ref.Target, ref.Status)
	}
}
