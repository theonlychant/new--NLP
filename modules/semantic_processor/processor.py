"""
Semantic analysis and embedding processor
"""

import json
import sys
from typing import Dict, List, Any

def process(text: str) -> Dict[str, Any]:
    """Process text for semantic meaning"""
    words = text.lower().split()
    
    semantic_tags = {
        'verbs': [],
        'nouns': [],
        'conjunctions': []
    }
    
    verb_keywords = ['is', 'are', 'was', 'were', 'released', 'competing', 'has', 'have']
    conj_keywords = ['and', 'or', 'but', 'nor']
    
    for word in words:
        word_clean = word.rstrip('.,!?;:')
        if word_clean in verb_keywords:
            semantic_tags['verbs'].append(word_clean)
        elif word_clean in conj_keywords:
            semantic_tags['conjunctions'].append(word_clean)
        else:
            semantic_tags['nouns'].append(word_clean)
    
    return {
        'word_count': len(words),
        'unique_words': len(set(words)),
        'semantic_tags': semantic_tags,
        'status': 'success'
    }

if __name__ == "__main__":
    text = sys.stdin.read().strip()
    if text:
        result = process(text)
        print(json.dumps(result, indent=2))
    else:
        print(json.dumps({"status": "error", "error": "No input"}, indent=2))
