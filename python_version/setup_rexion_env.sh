#!/bin/bash

echo "🔧 Setting up the Rexion Compiler environment..."

# Create virtual environment
python3 -m venv rexion_env
source rexion_env/bin/activate

# Upgrade pip and install dependencies
echo "📦 Installing dependencies..."
pip install --upgrade pip
pip install -r requirements.txt

# Optional: Install Graphviz system-level if needed
if ! command -v dot &> /dev/null
then
    echo "⚠️  Graphviz not found. Installing system package..."
    sudo apt-get install graphviz -y  # For Ubuntu/Debian
fi

# Create project structure
echo "📁 Creating project folders..."
mkdir -p src/{lexer,parser,ir,vm,codegen,debugger,ui,tests}
mkdir -p bin logs output visualizations

# Create placeholder starter files
touch src/main.py
touch src/lexer/__init__.py
touch src/parser/__init__.py
touch src/ir/__init__.py
touch src/vm/__init__.py
touch src/codegen/__init__.py
touch src/debugger/__init__.py
touch src/ui/__init__.py
touch src/tests/__init__.py

echo "✅ Rexion environment setup complete!"
echo "To get started:"
echo "  source rexion_env/bin/activate"
echo "  python src/main.py"

