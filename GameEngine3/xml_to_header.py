import sys
import os
import xml.etree.ElementTree as ET

def xml_to_header(xml_file_path, header_file_path):
    try:
        tree = ET.parse(xml_file_path)
        root = tree.getroot()
    except Exception as e:
        print(f"Error parsing XML file: {e}")
        return

    lines = [
        "#ifndef TOOLTIPS_H",
        "#define TOOLTIPS_H",
        "",
        "// Generated C++ Header from Tooltip XML",
        ""
    ]

    for tooltip in root.findall('tooltip'):
        tooltip_id = tooltip.get('id')
        text_node = tooltip.find('text')
        
        if tooltip_id and text_node is not None and text_node.text:
            # Escape inner quotes for safe C-string definition
            escaped_text = text_node.text.replace('"', '\\"')
            lines.append(f'#define {tooltip_id} "{escaped_text}"')

    lines.extend(["", "#endif // TOOLTIPS_H"])

    try:
        with open(header_file_path, "w", encoding="utf-8") as f:
            f.write("\n".join(lines))
        print(f"Successfully generated {header_file_path}")
    except Exception as e:
        print(f"Error writing header file: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python script.py <input.xml> <output.h>")
    else:
        xml_to_header(sys.argv[1], sys.argv[2])
