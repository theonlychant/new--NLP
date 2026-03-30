"""
Main NLP pipeline orchestrator - coordinates multi-language execution
"""

import subprocess
import json
import os
import sys
from config import MODULES, PIPELINE_ORDER, ModuleLanguage
from typing import Dict, Any

class NLPPipeline:
    def __init__(self):
        self.modules = MODULES
        self.pipeline_order = PIPELINE_ORDER
        self.results = {}
    
    def execute(self, text: str) -> Dict[str, Any]:
        """Execute the full NLP pipeline"""
        print(f"\n Starting NLP Pipeline with {len(self.pipeline_order)} modules\n")
        print("=" * 70)
        
        for i, module_name in enumerate(self.pipeline_order, 1):
            module_config = self.modules[module_name]
            print(f"\n[{i}/{len(self.pipeline_order)}] Executing: {module_name}")
            print(f"    Language: {module_config['language'].value.upper()}")
            print(f"    Performance Tier: {module_config['tier'].value}")
            print(f"    Description: {module_config['description']}")
            
            if module_config['language'] == ModuleLanguage.CPP:
                result = self._execute_cpp_module(module_name, text)
            elif module_config['language'] == ModuleLanguage.GO:
                result = self._execute_go_module(module_name, text)
            elif module_config['language'] == ModuleLanguage.PYTHON:
                result = self._execute_python_module(module_name, text)
            else:
                result = {"status": "error", "error": "Unknown language"}
            
            self.results[module_name] = result
            
            if result.get("status") == "success":
                print(f"    ✓ Success")
            else:
                print(f"    ✗ Failed: {result.get('error', 'Unknown error')}")
        
        print("\n" + "=" * 70)
        return self.results
    
    def _execute_cpp_module(self, module_name: str, data: str) -> Dict[str, Any]:
        """Execute C++ module"""
        module_path = self.modules[module_name]['entry_point']
        binary = module_path.replace('.cpp', '')
        
        try:
            compile_cmd = ['g++', '-O3', '-std=c++17', module_path, '-o', binary, '-ljsoncpp']
            subprocess.run(compile_cmd, check=True, capture_output=True, timeout=30)
            
            result = subprocess.run([binary], input=data, text=True, 
                                  capture_output=True, timeout=10)
            
            if result.returncode == 0:
                try:
                    output = json.loads(result.stdout)
                    return {"status": "success", "data": output}
                except json.JSONDecodeError:
                    return {"status": "success", "data": result.stdout}
            else:
                return {"status": "error", "error": result.stderr[:200]}
        
        except subprocess.TimeoutExpired:
            return {"status": "error", "error": "Timeout"}
        except Exception as e:
            return {"status": "error", "error": str(e)[:200]}
    
    def _execute_go_module(self, module_name: str, data: str) -> Dict[str, Any]:
        """Execute Go module"""
        module_path = self.modules[module_name]['entry_point']
        
        try:
            result = subprocess.run(['go', 'run', module_path], 
                                  input=data, text=True,
                                  capture_output=True, timeout=10)
            
            if result.returncode == 0:
                try:
                    output = json.loads(result.stdout)
                    return {"status": "success", "data": output}
                except json.JSONDecodeError:
                    return {"status": "success", "data": result.stdout}
            else:
                return {"status": "error", "error": result.stderr[:200]}
        
        except subprocess.TimeoutExpired:
            return {"status": "error", "error": "Timeout"}
        except Exception as e:
            return {"status": "error", "error": str(e)[:200]}
    
    def _execute_python_module(self, module_name: str, data: str) -> Dict[str, Any]:
        """Execute Python module"""
        module_path = self.modules[module_name]['entry_point']
        
        try:
            result = subprocess.run(['python3', module_path], 
                                  input=data, text=True,
                                  capture_output=True, timeout=10)
            
            if result.returncode == 0:
                try:
                    output = json.loads(result.stdout)
                    return {"status": "success", "data": output}
                except json.JSONDecodeError:
                    return {"status": "success", "data": result.stdout}
            else:
                return {"status": "error", "error": result.stderr[:200]}
        
        except subprocess.TimeoutExpired:
            return {"status": "error", "error": "Timeout"}
        except Exception as e:
            return {"status": "error", "error": str(e)[:200]}

if __name__ == "__main__":
    pipeline = NLPPipeline()
    test_text = "Apple and Google are competing. Microsoft released Azure today."
    results = pipeline.execute(test_text)
    
    print("\n FINAL RESULTS:")
    print("=" * 70)
    print(json.dumps(results, indent=2))
    print("=" * 70)
