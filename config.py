from enum import Enum
class ModuleLanguage(Enum):
    PYTHON="python"
    CPP="cpp"
    GO="go"
class PerformanceTier(Enum):
    CRITICAL="critical"
    HIGH="high"
    MEDIUM="medium"
MODULES={"tokenizer":{"language":ModuleLanguage.CPP,"tier":PerformanceTier.CRITICAL,"description":"Tokenization","entry_point":"modules/tokenizer/tokenizer.cpp"}}
PIPELINE_ORDER=["tokenizer"]