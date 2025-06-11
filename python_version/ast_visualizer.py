import graphviz
import os
from typing import List

def classify_node(label):
    if label.startswith("[FUNC_DECL]"):
        return "Function", "folder", "lightblue"
    elif label.startswith("[CALL]"):
        return "Call", "oval", "gold"
    elif label.startswith("[ASSIGN]"):
        return "Assign", "box", "lightgreen"
    elif label.startswith("[RETURN]") or label.startswith("[RET]"):
        return "Return", "box", "gray"
    elif label.startswith("[JMP]"):
        return "Jump", "diamond", "orange"
    elif label.startswith("[JZ]"):
        return "JumpTrue", "diamond", "lightsalmon"
    elif label.startswith("[JNZ]"):
        return "JumpFalse", "diamond", "tomato"
    elif label.startswith("[LABEL]"):
        return "Label", "note", "lightyellow"
    elif label.startswith("[CMP]") or label.startswith("[ADD]") or label.startswith("[SUB]") or label.startswith("[DIV]"):
        return "Op", "parallelogram", "violet"
    elif label.startswith("[PRINT]"):
        return "Print", "box", "pink"
    elif label.startswith("[IF]"):
        return "If", "octagon", "deepskyblue"
    elif label.startswith("[ELSE]"):
        return "Else", "octagon", "slateblue"
    elif label.startswith("[END_IF]"):
        return "EndIf", "octagon", "lightsteelblue"
    else:
        return "Generic", "ellipse", "white"

def sanitize_label(label):
    return label.replace('"', "'").replace("->", "→")

def visualize_ast_nested_clusters(ir: List[str], filename: str = "nested_ast_flow", outdir: str = "graphs", fmt: str = "png", debug: bool = False):
    os.makedirs(outdir, exist_ok=True)
    dot = graphviz.Digraph(comment="Rexion AST Flowchart", format=fmt, directory=outdir)
    dot.attr(rankdir='TB', fontsize='10', fontname='Courier New')

    node_counter = 0
    previous_node = None
    cluster_stack = []
    control_stack = []

    node_ids = {}

    for i, line in enumerate(ir):
        node_id = f"n{node_counter}"
        label = sanitize_label(line.strip())
        category, shape, color = classify_node(line)

        if debug:
            print(f"[{node_id}] Category: {category}, Label: {label}")

        # Handle nested if/else as clusters
        if category == "If":
            cluster_id = f"cluster_if_{len(cluster_stack)}"
            new_cluster = graphviz.Digraph(name=cluster_id)
            new_cluster.attr(label=f"IF block {len(cluster_stack)}", style="dashed", color="blue")
            cluster_stack.append(new_cluster)
            control_stack.append((node_id, "IF_TRUE"))

        elif category == "Else":
            if_cluster = cluster_stack.pop()
            dot.subgraph(if_cluster)
            cluster_id = f"cluster_else_{len(cluster_stack)}"
            new_cluster = graphviz.Digraph(name=cluster_id)
            new_cluster.attr(label=f"ELSE block {len(cluster_stack)}", style="dashed", color="purple")
            cluster_stack.append(new_cluster)
            control_stack.append((node_id, "IF_FALSE"))

        elif category == "EndIf":
            if cluster_stack:
                final = cluster_stack.pop()
                dot.subgraph(final)
            if control_stack:
                control_stack.pop()

        # Add node to current cluster or main graph
        if cluster_stack:
            cluster_stack[-1].node(node_id, label, shape=shape, style='filled', fillcolor=color, fontname="Courier")
        else:
            dot.node(node_id, label, shape=shape, style='filled', fillcolor=color, fontname="Courier")

        # Save node reference
        node_ids[i] = node_id

        # Add edge with conditional labels if applicable
        if previous_node is not None:
            edge_label = ""
            if category == "JumpTrue":
                edge_label = "true"
            elif category == "JumpFalse":
                edge_label = "false"
            elif control_stack:
                _, cond = control_stack[-1]
                if cond == "IF_TRUE":
                    edge_label = "true"
                elif cond == "IF_FALSE":
                    edge_label = "false"

            if edge_label:
                dot.edge(previous_node, node_id, label=edge_label)
            else:
                dot.edge(previous_node, node_id)

        previous_node = node_id
        node_counter += 1

    # Flush remaining cluster if any
    for rem_cluster in cluster_stack:
        dot.subgraph(rem_cluster)

    output_path = dot.render(filename=filename, cleanup=True)
    return output_path

