#!/usr/bin/env python3

import os
import re
from pathlib import Path
from markdown import markdown
import pdfkit  # Requires wkhtmltopdf installed

# === CONFIGURATION ===
SOURCE_DIRS = ["./", "./examples", "./official"]
DOC_OUTPUT = Path("docs/rexion_codex.md")
PDF_OUTPUT = Path("docs/rexion_codex.pdf")

INLINE_DOC_PATTERN = r'\/{2,} ?DOC: (.+)'  # Matches lines like // DOC: This is a doc line
SECTION_HEADER_TEMPLATE = "\n## {filename}\n"

def extract_doc_blocks(filepath):
    doc_lines = []
    with open(filepath, "r", encoding="utf-8") as f:
        for line in f:
            match = re.search(INLINE_DOC_PATTERN, line)
            if match:
                doc_lines.append(match.group(1).strip())
    return doc_lines

def generate_markdown():
    doc_sections = []
    for src_dir in SOURCE_DIRS:
        for root, _, files in os.walk(src_dir):
            for file in files:
                if file.endswith((".c", ".h", ".r4", ".txt")):
                    full_path = os.path.join(root, file)
                    doc_lines = extract_doc_blocks(full_path)
                    if doc_lines:
                        section = SECTION_HEADER_TEMPLATE.format(filename=os.path.relpath(full_path))
                        section += "\n" + "\n".join(f"- {line}" for line in doc_lines)
                        doc_sections.append(section)
    
    DOC_OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    with open(DOC_OUTPUT, "w", encoding="utf-8") as f:
        f.write("# Rexion Language Codex\n")
        f.write("\n".join(doc_sections))
    print(f"[✓] Markdown Codex generated: {DOC_OUTPUT}")

def generate_pdf():
    try:
        pdfkit.from_file(str(DOC_OUTPUT), str(PDF_OUTPUT))
        print(f"[✓] PDF Codex generated: {PDF_OUTPUT}")
    except Exception as e:
        print(f"[!] PDF generation failed: {e}")

if __name__ == "__main__":
    generate_markdown()
    generate_pdf()

# generate_codex.py
from pathlib import Path
import re
from fpdf import FPDF

source_files = ["lexer.c", "parser.c", "ir_codegen.c", "rexionc_main.c"]
md_output_path = Path("rexion_codex.md")
pdf_output_path = Path("rexion_codex.pdf")

def extract_doc_lines(file_path):
    doc_lines = []
    with open(file_path, "r") as f:
        for line in f:
            if "// DOC:" in line:
                content = line.split("// DOC:")[1].strip()
                doc_lines.append((file_path, content))
    return doc_lines

all_doc_lines = []
for file in source_files:
    all_doc_lines.extend(extract_doc_lines(file))

with open(md_output_path, "w") as md:
    md.write("# Rexion Language Codex (Auto-Generated)\n\n")
    current_file = None
    for file, line in all_doc_lines:
        if file != current_file:
            md.write(f"\n## {file}\n\n")
            current_file = file
        md.write(f"- {line}\n")

pdf = FPDF()
pdf.set_auto_page_break(auto=True, margin=15)
pdf.add_page()
pdf.set_font("Arial", size=12)
pdf.set_title("Rexion Language Codex")

current_file = None
for file, line in all_doc_lines:
    if file != current_file:
        pdf.set_font("Arial", 'B', 14)
        pdf.cell(200, 10, txt=f"{file}", ln=True, align="L")
        pdf.set_font("Arial", size=12)
        current_file = file
    pdf.multi_cell(0, 10, txt=f"- {line}")

pdf.output(str(pdf_output_path))
print("✅ Codex generated: rexion_codex.md and rexion_codex.pdf")
