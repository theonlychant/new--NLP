import os
import triton

def test():    # Check if the environment variable is set
    env_var = os.getenv("MY_ENV_VAR")
    assert env_var is not None, "MY_ENV_VAR should be set in the environment"
    assert env_var == "expected_value", "MY_ENV_VAR should have the expected value"