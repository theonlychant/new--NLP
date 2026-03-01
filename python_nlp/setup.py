from setuptools import setup, find_packages

setup(
    name="python-nlp-core",
    version="0.1.0",
    description="Core NLP utilities for Python.",
    author="Your Name",
    author_email="your@email.com",
    url="https://github.com/1proprogrammerchant/new--NLP",
    packages=find_packages(),
    python_requires=">=3.7",
    license="MIT",
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
)
