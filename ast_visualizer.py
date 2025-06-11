import graphviz

def visualize_ast(ir, filename="ast_flowchart"):
    dot = graphviz.Digraph(comment="Rexion AST Flow")
    for i, line in enumerate(ir):
        node_id = f"n{i}"
        dot.node(node_id, line)
        if i > 0:
            dot.edge(f"n{i-1}", node_id)
    dot.render(filename, format='png', cleanup=True)
