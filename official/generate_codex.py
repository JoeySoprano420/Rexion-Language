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
