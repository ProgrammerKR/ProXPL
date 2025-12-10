from setuptools import setup, find_packages

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="proxpl",
    version="1.0.0",
    author="ProgrammerKR",
    author_email="programmerkr123@gmail.com",
    description="ProXPL: A robust, educational programming language",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/ProgrammerKR/ProXPL",
    packages=find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Topic :: Software Development :: Compilers",
    ],
    python_requires='>=3.8',
    entry_points={
        'console_scripts': [
            'prox=cli.main:main',
        ],
    },
    install_requires=[
        # List any dependencies here if needed in the future
    ],
)
