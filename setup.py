from setuptools import setup, Extension

sfc_module = Extension('paker', sources = ['module.cpp'])

setup(
    name='paker',
    version='1.0',
    description='Solves 0-1 knapsack problems.',
    ext_modules=[sfc_module]
)
